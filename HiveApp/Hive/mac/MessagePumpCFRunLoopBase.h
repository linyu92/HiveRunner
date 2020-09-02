//
//  MessagePumpCFRunLoopBase.h
//  HiveApp
//
//  Created by levilin on 9/1/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef MessagePumpCFRunLoopBase_hpp
#define MessagePumpCFRunLoopBase_hpp

#include "message_pump.h"

#if defined(__OBJC__)
#if defined(OS_IOS)
#import <Foundation/Foundation.h>
#else
#import <AppKit/AppKit.h>


// Clients must subclass NSApplication and implement this protocol if they use
// MessagePumpMac.
@protocol CrAppProtocol
// Must return true if -[NSApplication sendEvent:] is currently on the stack.
// See the comment for |CreateAutoreleasePool()| in the cc file for why this is
// necessary.
- (BOOL)isHandlingSendEvent;
@end
#endif  // !defined(OS_IOS)
#endif  // defined(__OBJC__)

namespace hive {

enum { kCommonModeMask = 0x1, kAllModesMask = 0xf };

constexpr CFTimeInterval kCFTimeIntervalMax = std::numeric_limits<CFTimeInterval>::max();

class MessagePumpCFRunLoopBase : public MessagePump {
public:
    MessagePumpCFRunLoopBase(int initial_mode_mask);
    
    virtual ~MessagePumpCFRunLoopBase();
    
public:
    /// MessagePump
    void Run(Delegate* delegate) override;

    void Quit() override;

    void ScheduleWork() override;

    void ScheduleDelayedWork(const TimeTicks& delayedWorkTime) override;
    
    CFRunLoopRef run_loop() const { return run_loop_; }
    
protected:
    
    virtual void DoRun() = 0;
    
    virtual bool DoQuit() = 0;
    
protected:
    class ScopedModeEnabler;
    static constexpr int kNumModes = 4;
    std::unique_ptr<ScopedModeEnabler> m_enabledModes[kNumModes];
    
private:
    Delegate* m_delegate;
    
    CFRunLoopRef run_loop_;
    
    CFRunLoopTimerRef delayed_work_timer_;
    CFRunLoopSourceRef work_source_;
    CFRunLoopSourceRef idle_work_source_;
//    CFRunLoopSourceRef nesting_deferred_work_source_;
    CFRunLoopObserverRef pre_wait_observer_;
    CFRunLoopObserverRef pre_source_observer_;
    CFRunLoopObserverRef enter_exit_observer_;
    
private:
    void SetModeMask(int mode_mask);
    
    void SetDelegate(Delegate *delegate) {
        m_delegate = delegate;
    }
    
    bool RunWork();
    
    static void RunWorkSource(void* info);
    static void RunDelayedWorkTimer(CFRunLoopTimerRef timer, void* info);
    
    static void RunIdleWorkSource(void* info);
    
    // base
    static void PreWaitObserver(CFRunLoopObserverRef observer,
                                CFRunLoopActivity activity, void* info);

    // Observer callback called before the run loop processes any sources.
    // Associated with pre_source_observer_.
    static void PreSourceObserver(CFRunLoopObserverRef observer,
                                  CFRunLoopActivity activity, void* info);

    // Observer callback called when the run loop starts and stops, at the
    // beginning and end of calls to CFRunLoopRun.  This is used to maintain
    // nesting_level_.  Associated with enter_exit_observer_.
    static void EnterExitObserver(CFRunLoopObserverRef observer,
                                  CFRunLoopActivity activity, void* info);

    // Called by EnterExitObserver after performing maintenance on nesting_level_.
    // This allows subclasses an opportunity to perform additional processing on
    // the basis of run loops starting and stopping.
    virtual void EnterExitRunLoop(CFRunLoopActivity activity);
}; // class MessagePumpCFRunLoopBase


}; // namespace hive

#endif /* MessagePumpCFRunLoopBase_hpp */
