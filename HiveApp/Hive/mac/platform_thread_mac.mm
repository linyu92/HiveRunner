//
//  platform_thread_mac.cpp
//  HiveApp
//
//  Created by levilin on 8/19/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "common/logging.h"
#include "platform_thread.h"
#include "thread_id_name_manager.h"

namespace hive {

namespace {
NSString* const kThreadPriorityKey = @"CrThreadPriorityKey";
}; // namespace

// static
void PlatformThread::SetName(const std::string& name) {
  ThreadIdNameManager::GetInstance()->SetName(name);
  // Mac OS X does not expose the length limit of the name, so
  // hardcode it.
  const int kMaxNameLength = 63;
  std::string shortened_name = name.substr(0, kMaxNameLength);
  // pthread_setname() fails (harmlessly) in the sandbox, ignore when it does.
  // See http://crbug.com/47058
  pthread_setname_np(shortened_name.c_str());
}

// static
void PlatformThread::SetCurrentThreadPriorityImpl(ThreadPriority priority) {
  // Changing the priority of the main thread causes performance regressions.
  // https://crbug.com/601270
//  DCHECK(![[NSThread currentThread] isMainThread]);

  switch (priority) {
    case ThreadPriority::BACKGROUND:
      [[NSThread currentThread] setThreadPriority:0];
      break;
    case ThreadPriority::NORMAL:
    case ThreadPriority::DISPLAY:
      [[NSThread currentThread] setThreadPriority:0.5];
      break;
//    case ThreadPriority::REALTIME_AUDIO:
//      SetPriorityRealtimeAudio();
//      DCHECK_EQ([[NSThread currentThread] threadPriority], 1.0);
//      break;
  }
  [[NSThread currentThread] threadDictionary][kThreadPriorityKey] =
      @(static_cast<int>(priority));
}


}; // hive
