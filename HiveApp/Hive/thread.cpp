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
    
    m_idSema.Notify();
    
    PlatformThread::SetName(m_name);
    
    // task_environment_->BindToCurrentThread(timer_slack_);
    
    {
      std::unique_lock<std::mutex> lock(m_runningMutex);
      m_running = true;
    }

    m_startSema.Notify();
    
    RunLoop runLoop(m_threadController.get());
    m_runLoop = &runLoop;
    Run(m_runLoop);
 
    {
      std::unique_lock<std::mutex> lock(m_runningMutex);
      m_running = false;
    }
    
//    task_environment_.reset();
    m_runLoop = nullptr;
}

bool Thread::IsRunning() {
    std::unique_lock<std::mutex> lock(m_runningMutex);
    return m_running;
}

void Thread::Run(RunLoop* runLoop) {
  // Overridable protected method to be called from our |thread_| only.
//    assert(<#e#>)
//  DCHECK(id_event_.IsSignaled());
//  DCHECK_EQ(id_, PlatformThread::CurrentId());
  runLoop->Run();
}

bool Thread::WaitUntilThreadStarted() {
    if (!m_threadController) {
        return false;
    }
    m_startSema.Wait(false);
    return true;
}

bool Thread::Stop() {
    
    std::unique_lock<std::mutex> lock(m_threadMutex);
    
    PlatformThread::Join(m_thread);
    
    return true;
}

bool Thread::Start() {
    LOG(INFO) << "thread start!!!";
        
    m_id = kInvalidThreadId;
    m_idSema.Reset();
        
    m_threadController = std::make_unique<ThreadController>();
    m_startSema.Reset();
        
    std::unique_lock<std::mutex> lock(m_threadMutex);
    bool success = PlatformThread::CreateWithPriority(0, this, &m_thread, ThreadPriority::NORMAL);
    DCHECK(success);
    return success;
}



PlatformThreadId Thread::GetThreadId() {
    m_idSema.Wait(false);
    return m_id;
}

}; // hive
