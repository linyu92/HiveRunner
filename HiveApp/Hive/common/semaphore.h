//
//  semaphore.h
//  HiveApp
//
//  Created by levilin on 8/19/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef semaphore_h
#define semaphore_h

#include <mutex>
#include <condition_variable>

namespace hive {

class Semaphore {
public:
    Semaphore (int count = 0)
        : m_count(count) {}

    inline void Notify()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_count++;
        m_cv.notify_one();
    }

    inline void Wait(bool decreatement)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        while(m_count == 0){
            m_cv.wait(lock);
        }
        if (decreatement) {
            m_count--;
        }
    }
    
    inline void Reset()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_count = 0;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    int m_count;
};

};

#endif /* semaphore_h */
