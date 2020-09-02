//
//  thread_controller.h
//  HiveApp
//
//  Created by levilin on 8/29/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef thread_controller_hpp
#define thread_controller_hpp

#include "task/task_manager.h"
#include "run_loop.h"

namespace hive {

class ThreadController : public RunLoop::Delegate {
public:
    ThreadController();
    ~ThreadController();

private:
    // RunLoop::Delegate
    bool RunWork() override;
    bool RunDelayedWork(TimeTicks* next_run_time) override;
private:
    std::unique_ptr<task_manager::TaskManager> m_taskManager;
}; // ThreadController

}; // namespace hive


#endif /* thread_controller_hpp */
