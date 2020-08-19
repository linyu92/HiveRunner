//
//  thread.cpp
//  Hive
//
//  Created by levilin on 8/13/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#include "thread.h"

namespace hive {

Thread::Thread(const std::string& name) : m_name(name) {
    
}

Thread::~Thread() {
    LOG(INFO) << "thread deconstructor";
}

void Thread::ThreadMain() {
    
    m_id = PlatformThread::CurrentId();
    
    m_id_sema.notify();
    
    PlatformThread::SetName(m_name);
    
    // task_environment_->BindToCurrentThread(timer_slack_);
    
    {
      std::unique_lock<std::mutex> lock(m_runningMutex);
      m_running = true;
    }

//    RunLoop run_loop;
//    run_loop_ = &run_loop;
//    Run(run_loop_);
 
    {
      std::unique_lock<std::mutex> lock(m_runningMutex);
      m_running = false;
    }
    
//    task_environment_.reset();
//    run_loop_ = nullptr;
}

bool Thread::IsRunning() {
    std::unique_lock<std::mutex> lock(m_runningMutex);
    return m_running;
}

bool Thread::Start() {
    LOG(INFO) << "thread start!!!";
    m_messageLoop = std::unique_ptr<MessageLoop>(new MessageLoop());
    std::unique_lock<std::mutex> lock(m_mutex);
    bool success = PlatformThread::CreateWithPriority(0, this, &m_thread, ThreadPriority::NORMAL);
    DCHECK(success);
    return success;
}

PlatformThreadId Thread::GetThreadId() {
    m_id_sema.wait(false);
    return m_id;
}

}; // hive
