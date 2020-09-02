//
//  task_queue.h
//  HiveApp
//
//  Created by levilin on 8/24/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef task_queue_hpp
#define task_queue_hpp

#include "build/base_export.h"
#include <memory>

namespace hive {

namespace task_manager {

class BASE_EXPORT TaskQueue : public std::enable_shared_from_this<TaskQueue> {


};

}; // namespace sequence_manager
}; // namespace hive


#endif /* task_queue_hpp */
