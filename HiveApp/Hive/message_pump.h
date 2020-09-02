//
//  message_pump.h
//  HiveApp
//
//  Created by levilin on 8/30/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef message_pump_hpp
#define message_pump_hpp

#include "build/base_export.h"
#include "common/time/time_delta.h"
namespace hive {

class BASE_EXPORT MessagePump {
public:
    class BASE_EXPORT Delegate {
    public:
        virtual void onMessagePumpDoWork() = 0;
        
        virtual void onMessagePumpDoQuit() = 0;
    };
    
    MessagePump();
    
    virtual ~MessagePump();
    
    virtual void Run(Delegate* delegate) = 0;

    virtual void Quit() = 0;

    virtual void ScheduleWork() = 0;

    virtual void ScheduleDelayedWork(const TimeTicks& delayedWorkTime) = 0;
    
};

}; // namespace hive

#endif /* message_pump_hpp */
