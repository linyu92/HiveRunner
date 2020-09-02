//
//  run_loop.h
//  HiveApp
//
//  Created by levilin on 8/20/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef run_loop_hpp
#define run_loop_hpp

#include "build/base_export.h"
#include "common/time/time_delta.h"
#include "message_pump.h"
namespace hive {

class BASE_EXPORT RunLoop : public MessagePump::Delegate {
public:
    class Delegate {
    public:
        virtual bool RunWork() = 0;
        virtual bool RunDelayedWork(TimeTicks* next_run_time) = 0;
    };
public:
    RunLoop(Delegate *delegate);
    ~RunLoop();

    // Run the current RunLoop::Delegate. This blocks until Quit is called. Before
    // calling Run, be sure to grab the QuitClosure in order to stop the
    // RunLoop::Delegate asynchronously.
    void Run();
private:
    // MessagePump::Delegate
    void onMessagePumpDoWork();
    void onMessagePumpDoQuit();
private:
    void RunWithTimeout(TimeDelta timeout);
    std::unique_ptr<MessagePump> CreateMessagePump();
    void BeforeRun();
    void AfterRun();
    bool IsRunning() {
        return m_running;
    }
private:
    Delegate *m_delegate;
    std::unique_ptr<MessagePump> m_pump;
    bool m_running;
    
}; // RunLoop

}; // namespace hive

#endif /* run_loop_hpp */
