//
//  platform_thread.cpp
//  Hive
//
//  Created by levilin on 8/14/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#include "platform_thread.h"


namespace hive {

namespace {
//
// PlatformThread::SetCurrentThreadPriority() doesn't query the state of the
// feature directly because FeatureList initialization is not always
// synchronized with PlatformThread::SetCurrentThreadPriority().
std::atomic<bool> g_use_thread_priorities(true);

};  // namespace

// static
void PlatformThread::SetCurrentThreadPriority(ThreadPriority priority) {
    if (g_use_thread_priorities.load()) {
      SetCurrentThreadPriorityImpl(priority);
    }
}

}; // hive
