//
//  ViewController.m
//  HiveApp
//
//  Created by levilin on 8/14/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#import "ViewController.h"
#include "Hive.h"


using namespace hive;

@interface ViewController ()
{
    Thread *_thread;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    SetLogPrefix("[HiveApp]");
    SetLogMessageHandler(&onLogMessageHandlerFunction);
    
    // Do any additional setup after loading the view.
    _thread = new Thread("12");
    _thread->Start();
    
//    _thread->task_runner().get();
}

bool onLogMessageHandlerFunction(int severity,const char* file, int line, size_t message_start, const std::string& str) {
    NSString *log = [NSString stringWithFormat:@"%s",str.c_str()];
    NSLog(@"%@",log);
    return true;
}

@end
