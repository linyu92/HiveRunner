//
//  run_loop.cpp
//  HiveApp
//
//  Created by levilin on 8/20/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#include "run_loop.h"
#include "mac/MessagePumpCFRunLoop.h"

namespace hive {

//RunLoop::RunLoop(Type type)
//    : delegate_(tls_delegate.Get().Get()),
//      type_(type),
//      origin_task_runner_(ThreadTaskRunnerHandle::Get()),
//      weak_factory_(this) {
//  DCHECK(delegate_) << "A RunLoop::Delegate must be bound to this thread prior "
//                       "to using RunLoop.";
//  DCHECK(origin_task_runner_);
//}


std::unique_ptr<MessagePump> RunLoop::CreateMessagePump() {
    return std::make_unique<MessagePumpCFRunLoop>();
}

RunLoop::RunLoop(Delegate *delegate) : m_delegate(delegate), m_pump(CreateMessagePump()) {
    
}

RunLoop::~RunLoop() {
    
}

void RunLoop::Run() {
    RunWithTimeout(TimeDelta::Max());
}

void RunLoop::BeforeRun() {
    m_running = true;
}

void RunLoop::AfterRun() {
    m_running = false;
}

void RunLoop::RunWithTimeout(TimeDelta timeout) {
        
    BeforeRun();
//    AutoReset<TimeTicks> quit_runloop_after(
//        &main_thread_only().quit_runloop_after,
//        (timeout == TimeDelta::Max()) ? TimeTicks::Max()
//                                      : time_source_->NowTicks() + timeout);
    m_pump->Run(this);
    
    AfterRun();
}

void RunLoop::onMessagePumpDoWork() {
    bool did_work = m_delegate->RunWork();
    bool resignal_work_source = did_work;
    
    TimeTicks next_time;
    
    if (IsRunning()) {
        m_delegate->RunDelayedWork(&next_time);
    }
    if (!did_work) {
        bool more_delayed_work = !next_time.is_null();
        if (more_delayed_work) {
          TimeDelta delay = next_time - TimeTicks::Now();
          if (delay > TimeDelta()) {
              m_pump->ScheduleDelayedWork(next_time);
          } else {
              resignal_work_source = true;
          }
        }
    }
    if (resignal_work_source) {
        m_pump->ScheduleWork();
    }
}

void RunLoop::onMessagePumpDoQuit() {
    
}

}; // namespace hive
