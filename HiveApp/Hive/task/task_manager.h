//
//  task_manager.h
//  HiveApp
//
//  Created by levilin on 8/29/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef task_manager_hpp
#define task_manager_hpp

#include "build/base_export.h"
#include <memory>
#include "task_queue.h"

namespace hive {

namespace task_manager {

class BASE_EXPORT TaskManager {
public:
    TaskManager();
    ~TaskManager();
    
//    std::unique_ptr<<#class _Tp#>>
    //virtual Optional<PendingTask> TakeTask() = 0;
    
private:
    std::shared_ptr<TaskQueue> m_taskQueue;
    
}; // TaskManager

}; // namespace task_manager
}; // namespace hive

#endif /* task_manager_hpp */
