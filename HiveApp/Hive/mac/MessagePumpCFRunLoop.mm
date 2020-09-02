//
//  MessagePumpCFRunLoop.cpp
//  HiveApp
//
//  Created by levilin on 8/30/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#include "MessagePumpCFRunLoop.h"


namespace hive {

MessagePumpCFRunLoop::MessagePumpCFRunLoop() : MessagePumpCFRunLoopBase(kCommonModeMask), quit_pending_(false) {}

MessagePumpCFRunLoop::~MessagePumpCFRunLoop() {}

void MessagePumpCFRunLoop::DoRun()
{
  int result;
  do {
      @autoreleasepool {
          result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, kCFTimeIntervalMax, false);
      }
  } while (result != kCFRunLoopRunStopped && result != kCFRunLoopRunFinished);
}

bool MessagePumpCFRunLoop::DoQuit()
{
    CFRunLoopStop(run_loop());
    return true;
}

void MessagePumpCFRunLoop::EnterExitRunLoop(CFRunLoopActivity activity)
{
    
}

}; // namespace hive
