//
//  thread.hpp
//  Hive
//
//  Created by levilin on 8/13/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef thread_hpp
#define thread_hpp

#include <stddef.h>

#include <memory>
#include <string>

#include <mutex>
#include "platform_thread.h"
#include "semaphore.h"
#include "run_loop.h"
#include "thread_controller.h"


namespace hive {

class BASE_EXPORT Thread : PlatformThread::Delegate {
public:
    
    explicit Thread(const std::string& name);
    
    ~Thread() override;
    
    bool Start();
    bool Stop();
    PlatformThreadId GetThreadId();
        
    bool IsRunning();
    
    bool WaitUntilThreadStarted();
    
private:
    // PlatformThread::Delegate
    void ThreadMain() override;
    
    
private:
    void Run(RunLoop* run_loop);
private:
    const std::string m_name;
    std::mutex m_threadMutex;
    PlatformThreadHandle m_thread;
    PlatformThreadId m_id = kInvalidThreadId;
    std::mutex m_runningMutex;
    bool m_running;
    Semaphore m_idSema;
    Semaphore m_startSema;
    
    
    RunLoop* m_runLoop = nullptr;
    std::unique_ptr<ThreadController> m_threadController;
};

}; // hive

#endif /* thread_hpp */
