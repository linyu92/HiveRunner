//
//  thread_controller.cpp
//  HiveApp
//
//  Created by levilin on 8/29/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#include "thread_controller.h"

namespace hive {

ThreadController::ThreadController()
    : m_taskManager(new task_manager::TaskManager())
{

}

ThreadController::~ThreadController()
{
    
}

#pragma mark - RunLoop::Delegate
bool ThreadController::RunWork()
{
    return true;
}
bool ThreadController::RunDelayedWork(TimeTicks* next_run_time)
{
    return true;
}


}; // namespace hive
