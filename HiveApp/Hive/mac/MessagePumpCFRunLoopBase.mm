//
//  MessagePumpCFRunLoopBase.cpp
//  HiveApp
//
//  Created by levilin on 9/1/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#include "MessagePumpCFRunLoopBase.h"

namespace hive {

class MessagePumpCFRunLoopBase::ScopedModeEnabler
{
public:
    ScopedModeEnabler(MessagePumpCFRunLoopBase* owner, int modeIndex) : m_owner(owner), m_modeIndex(modeIndex)
    {
        CFRunLoopRef loop = m_owner->run_loop_;
        CFRunLoopAddTimer(loop, m_owner->delayed_work_timer_, mode());
        CFRunLoopAddSource(loop, m_owner->work_source_, mode());
        CFRunLoopAddSource(loop, m_owner->idle_work_source_, mode());
//        CFRunLoopAddSource(loop, m_owner->nesting_deferred_work_source_, mode());
        CFRunLoopAddObserver(loop, m_owner->pre_wait_observer_, mode());
        CFRunLoopAddObserver(loop, m_owner->pre_source_observer_, mode());
        CFRunLoopAddObserver(loop, m_owner->enter_exit_observer_, mode());
    }
    
    ~ScopedModeEnabler()
    {
      CFRunLoopRef loop = m_owner->run_loop_;
      CFRunLoopRemoveObserver(loop, m_owner->enter_exit_observer_, mode());
      CFRunLoopRemoveObserver(loop, m_owner->pre_source_observer_, mode());
      CFRunLoopRemoveObserver(loop, m_owner->pre_wait_observer_, mode());
      CFRunLoopRemoveSource(loop, m_owner->idle_work_source_, mode());
      CFRunLoopRemoveSource(loop, m_owner->work_source_, mode());
      CFRunLoopRemoveTimer(loop, m_owner->delayed_work_timer_, mode());
    }
    
    const CFStringRef& mode() const {
      static const CFStringRef modes[] = {
          // The standard Core Foundation "common modes" constant. Must always be
          // first in this list to match the value of kCommonModeMask.
          kCFRunLoopCommonModes,

          // Mode that only sees Chrome work sources.
          CFSTR("kMessageLoopExclusiveRunLoopMode"),

          // Process work when NSMenus are fading out.
          CFSTR("com.apple.hitoolbox.windows.windowfadingmode"),

          // Process work when AppKit is highlighting an item on the main menubar.
          CFSTR("NSUnhighlightMenuRunLoopMode"),
      };
      return modes[m_modeIndex];
    }

private:
    MessagePumpCFRunLoopBase* const m_owner;  // Weak. Owns this.
    const int m_modeIndex;
}; // MessagePumpCFRunLoopBase::ScopedModeEnabler


MessagePumpCFRunLoopBase::MessagePumpCFRunLoopBase(int initial_mode_mask)
{
    run_loop_ = CFRunLoopGetCurrent();
    CFRetain(run_loop_);

    // Set a repeating timer with a preposterous firing time and interval.  The
    // timer will effectively never fire as-is.  The firing time will be adjusted
    // as needed when ScheduleDelayedWork is called.
    CFRunLoopTimerContext timer_context = CFRunLoopTimerContext();
    timer_context.info = this;
    delayed_work_timer_ = CFRunLoopTimerCreate(NULL,                // allocator
                                               kCFTimeIntervalMax,  // fire time
                                               kCFTimeIntervalMax,  // interval
                                               0,                   // flags
                                               0,                   // priority
                                               RunDelayedWorkTimer,
                                               &timer_context);

    CFRunLoopSourceContext source_context = CFRunLoopSourceContext();
    source_context.info = this;
    source_context.perform = RunWorkSource;
    work_source_ = CFRunLoopSourceCreate(NULL,  // allocator
                                         1,     // priority
                                         &source_context);
    source_context.perform = RunIdleWorkSource;
    idle_work_source_ = CFRunLoopSourceCreate(NULL,  // allocator
                                              2,     // priority
                                              &source_context);
//    source_context.perform = RunNestingDeferredWorkSource;
//    nesting_deferred_work_source_ = CFRunLoopSourceCreate(NULL,  // allocator
//                                                          0,     // priority
//                                                          &source_context);

    CFRunLoopObserverContext observer_context = CFRunLoopObserverContext();
    observer_context.info = this;
    pre_wait_observer_ = CFRunLoopObserverCreate(NULL,  // allocator
                                                 kCFRunLoopBeforeWaiting,
                                                 true,  // repeat
                                                 0,     // priority
                                                 PreWaitObserver,
                                                 &observer_context);
    pre_source_observer_ = CFRunLoopObserverCreate(NULL,  // allocator
                                                   kCFRunLoopBeforeSources,
                                                   true,  // repeat
                                                   0,     // priority
                                                   PreSourceObserver,
                                                   &observer_context);
    enter_exit_observer_ = CFRunLoopObserverCreate(NULL,  // allocator
                                                   kCFRunLoopEntry |
                                                       kCFRunLoopExit,
                                                   true,  // repeat
                                                   0,     // priority
                                                   EnterExitObserver,
                                                   &observer_context);
    SetModeMask(kCommonModeMask);
}

MessagePumpCFRunLoopBase::~MessagePumpCFRunLoopBase()
{
    SetModeMask(0);
    CFRelease(enter_exit_observer_);
    CFRelease(pre_source_observer_);
    CFRelease(pre_wait_observer_);
    CFRelease(idle_work_source_);
    CFRelease(work_source_);
    CFRelease(delayed_work_timer_);
    CFRelease(run_loop_);
}

void MessagePumpCFRunLoopBase::Run(Delegate* delegate)
{
    SetDelegate(delegate);
    ScheduleWork();
    DoRun();
    
    SetDelegate(nullptr);
}

void MessagePumpCFRunLoopBase::Quit()
{
    
}

void MessagePumpCFRunLoopBase::ScheduleWork()
{
    CFRunLoopSourceSignal(work_source_);
    CFRunLoopWakeUp(run_loop_);
}

void MessagePumpCFRunLoopBase::ScheduleDelayedWork(const TimeTicks& delayedWorkTime)
{
    
}

void MessagePumpCFRunLoopBase::RunDelayedWorkTimer(CFRunLoopTimerRef timer, void* info)
{
  MessagePumpCFRunLoopBase* self = static_cast<MessagePumpCFRunLoopBase*>(info);

  // The timer won't fire again until it's reset.
//  self->delayed_work_fire_time_ = kCFTimeIntervalMax;

//  self->SetDelayedWorkTimerValid(false);

  CFRunLoopSourceSignal(self->work_source_);
}

// Called from the run loop.
// static
void MessagePumpCFRunLoopBase::RunWorkSource(void* info)
{
  MessagePumpCFRunLoopBase* self = static_cast<MessagePumpCFRunLoopBase*>(info);
//  base::mac::CallWithEHFrame(^{
    self->RunWork();
//  });
}

// Called by MessagePumpCFRunLoopBaseBase::RunWorkSource.
bool MessagePumpCFRunLoopBase::RunWork()
{
    m_delegate->onMessagePumpDoWork();
    return true;
}

// Called from the run loop.
// static
void MessagePumpCFRunLoopBase::RunIdleWorkSource(void* info)
{
  MessagePumpCFRunLoopBase* self = static_cast<MessagePumpCFRunLoopBase*>(info);
//  base::mac::CallWithEHFrame(^{
//    self->RunIdleWork();
//  });
}

// Called from the run loop.
// static
void MessagePumpCFRunLoopBase::PreWaitObserver(CFRunLoopObserverRef observer,
                                               CFRunLoopActivity activity,
                                               void* info)
{
  MessagePumpCFRunLoopBase* self = static_cast<MessagePumpCFRunLoopBase*>(info);
//  base::mac::CallWithEHFrame(^{
//    // Attempt to do some idle work before going to sleep.
//    self->RunIdleWork();
//
//    // The run loop is about to go to sleep.  If any of the work done since it
//    // started or woke up resulted in a nested run loop running,
//    // nesting-deferred work may have accumulated.  Schedule it for processing
//    // if appropriate.
//    self->MaybeScheduleNestingDeferredWork();
//  });
}

// Called from the run loop.
// static
void MessagePumpCFRunLoopBase::PreSourceObserver(CFRunLoopObserverRef observer,
                                                 CFRunLoopActivity activity,
                                                 void* info) {
  MessagePumpCFRunLoopBase* self = static_cast<MessagePumpCFRunLoopBase*>(info);

  // The run loop has reached the top of the loop and is about to begin
  // processing sources.  If the last iteration of the loop at this nesting
  // level did not sleep or exit, nesting-deferred work may have accumulated
  // if a nested loop ran.  Schedule nesting-deferred work for processing if
  // appropriate.
//  base::mac::CallWithEHFrame(^{
//    self->MaybeScheduleNestingDeferredWork();
//  });
}

void MessagePumpCFRunLoopBase::SetModeMask(int mode_mask)
{
  for (size_t i = 0; i < kNumModes; ++i) {
    bool enable = mode_mask & (0x1 << i);
    if (enable == !m_enabledModes[i]) {
      m_enabledModes[i] =
          enable ? std::make_unique<ScopedModeEnabler>(this, i) : nullptr;
    }
  }
}


// Called from the run loop.
// static
void MessagePumpCFRunLoopBase::EnterExitObserver(CFRunLoopObserverRef observer,
                                                 CFRunLoopActivity activity,
                                                 void* info)
{
  MessagePumpCFRunLoopBase* self = static_cast<MessagePumpCFRunLoopBase*>(info);

//  switch (activity) {
//    case kCFRunLoopEntry:
//      ++self->nesting_level_;
//      if (self->nesting_level_ > self->deepest_nesting_level_) {
//        self->deepest_nesting_level_ = self->nesting_level_;
//      }
//      break;
//
//    case kCFRunLoopExit:
//      // Not all run loops go to sleep.  If a run loop is stopped before it
//      // goes to sleep due to a CFRunLoopStop call, or if the timeout passed
//      // to CFRunLoopRunInMode expires, the run loop may proceed directly from
//      // handling sources to exiting without any sleep.  This most commonly
//      // occurs when CFRunLoopRunInMode is passed a timeout of 0, causing it
//      // to make a single pass through the loop and exit without sleep.  Some
//      // native loops use CFRunLoop in this way.  Because PreWaitObserver will
//      // not be called in these case, MaybeScheduleNestingDeferredWork needs
//      // to be called here, as the run loop exits.
//      //
//      // MaybeScheduleNestingDeferredWork consults self->nesting_level_
//      // to determine whether to schedule nesting-deferred work.  It expects
//      // the nesting level to be set to the depth of the loop that is going
//      // to sleep or exiting.  It must be called before decrementing the
//      // value so that the value still corresponds to the level of the exiting
//      // loop.
//      base::mac::CallWithEHFrame(^{
//        self->MaybeScheduleNestingDeferredWork();
//      });
//      --self->nesting_level_;
//      break;
//
//    default:
//      break;
//  }
//
//  base::mac::CallWithEHFrame(^{
//    self->EnterExitRunLoop(activity);
//  });
}

// Called by MessagePumpCFRunLoopBaseBase::EnterExitRunLoop.  The default
// implementation is a no-op.
void MessagePumpCFRunLoopBase::EnterExitRunLoop(CFRunLoopActivity activity)
{
    
}

}; // namespace hive
