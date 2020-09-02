//
//  MessagePumpCFRunLoop.h
//  HiveApp
//
//  Created by levilin on 8/30/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef MessagePumpCFRunLoop_hpp
#define MessagePumpCFRunLoop_hpp

#include "MessagePumpCFRunLoopBase.h"

namespace hive {

class MessagePumpCFRunLoop : public MessagePumpCFRunLoopBase
{
public:
    MessagePumpCFRunLoop();
    ~MessagePumpCFRunLoop();
protected:
    // MessagePumpCFRunLoopBase
    void DoRun() override;
    bool DoQuit() override;
    
private:
    void EnterExitRunLoop(CFRunLoopActivity activity) override;
    bool quit_pending_;
};


}; // namespace hive

#endif /* MessagePumpCFRunLoop_hpp */
