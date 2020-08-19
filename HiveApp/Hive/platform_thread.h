//
//  platform_thread.h
//  Hive
//
//  Created by levilin on 8/14/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef platform_thread_hpp
#define platform_thread_hpp

#include "build/base_export.h"
#include "common/logging.h"

#if defined(OS_WIN)
#include "win/windows_types.h"
#elif defined(OS_MACOSX)
#include <mach/mach_types.h>
#include <pthread.h>
#elif defined(OS_POSIX)
#include <pthread.h>
#include <unistd.h>
#endif

namespace hive {

#if defined(OS_WIN)
typedef DWORD PlatformThreadId;
#elif defined(OS_FUCHSIA)
typedef zx_handle_t PlatformThreadId;
#elif defined(OS_MACOSX)
typedef mach_port_t PlatformThreadId;
#elif defined(OS_POSIX)
typedef pid_t PlatformThreadId;
#endif

const PlatformThreadId kInvalidThreadId(0);

enum class ThreadPriority : int {
  // Suitable for threads that shouldn't disrupt high priority work.
  BACKGROUND,
  // Default priority level.
  NORMAL,
  // Suitable for threads which generate data for the display (at ~60Hz).
  DISPLAY,
  // Suitable for low-latency, glitch-resistant audio.
//  REALTIME_AUDIO,
};


class PlatformThreadHandle {
 public:
#if defined(OS_WIN)
  typedef void* Handle;
#elif defined(OS_POSIX)
  typedef pthread_t Handle;
#endif

  constexpr PlatformThreadHandle() : handle_(0) {}

  explicit constexpr PlatformThreadHandle(Handle handle) : handle_(handle) {}

  bool is_equal(const PlatformThreadHandle& other) const {
    return handle_ == other.handle_;
  }

  bool is_null() const {
    return !handle_;
  }

  Handle platform_handle() const {
    return handle_;
  }

 private:
  Handle handle_;
};

class BASE_EXPORT PlatformThread {
public:
    class BASE_EXPORT Delegate {
    public:
        virtual void ThreadMain() = 0;
    protected:
        virtual ~Delegate() = default;
    };
    
    static PlatformThreadId CurrentId();
    
    static PlatformThreadHandle CurrentHandle();
        
    static bool CreateWithPriority(size_t stack_size,
                                   Delegate* delegate,
                                   PlatformThreadHandle* thread_handle,
                                   ThreadPriority priority);
    
    static void SetCurrentThreadPriority(ThreadPriority priority);
    
    // PlatformThreadImpl 下面方法需要平台层独立文件实现
private:
    static void SetCurrentThreadPriorityImpl(ThreadPriority priority);
public:
    static void SetName(const std::string& name);
    
    
};

}; // hive

#endif /* platform_thread_hpp */
