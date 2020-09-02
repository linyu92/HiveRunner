//
//  build_config.h
//  Hive
//
//  Created by levilin on 8/14/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef build_config_h
#define build_config_h

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   // win
    #define OS_WIN 1
#elif defined(__APPLE__)
    // ios / mac
    #define OS_MACOSX 1
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #define OS_IOS 1
    #endif
#elif defined(ANDROID)
    // android
    #define OS_ANDROID 1
#elif defind(__linux__)
    // linux
    #define OS_LINUX 1
#endif

// android ios mac
#if defined(OS_ANDROID) || defined(OS_LINUX) || defined(OS_MACOSX)
#define OS_POSIX 1
#endif


#if defined(COMPILER_GCC) || defined(__clang__)
#define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define WARN_UNUSED_RESULT
#endif


#define NOINLINE __attribute__((noinline))

#endif /* build_config_h */
