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
#include "message_loop.h"
#include "semaphore.h"

namespace hive {

class BASE_EXPORT Thread : PlatformThread::Delegate {
public:
    
    explicit Thread(const std::string& name);
    
    ~Thread() override;
    
    bool Start();
    
    PlatformThreadId GetThreadId();
        
    bool IsRunning();
        
private:
    // PlatformThread::Delegate
    void ThreadMain() override;
    
private:
    const std::string m_name;
    std::mutex m_mutex;
    std::unique_ptr<MessageLoop> m_messageLoop;
    PlatformThreadHandle m_thread;
    PlatformThreadId m_id = kInvalidThreadId;
    std::mutex m_runningMutex;
    bool m_running;
    Semaphore m_id_sema;
};

}; // hive

#endif /* thread_hpp */
