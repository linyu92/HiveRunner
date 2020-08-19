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
    Semaphore (int count_ = 0)
        : count(count_) {}

    inline void notify()
    {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        cv.notify_one();
    }

    inline void wait(bool decreatement)
    {
        std::unique_lock<std::mutex> lock(mtx);

        while(count == 0){
            cv.wait(lock);
        }
        if (decreatement) {
            count--;
        }
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};

};

#endif /* semaphore_h */
