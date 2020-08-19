//
//  platform_thread_posix.cpp
//  HiveApp
//
//  Created by levilin on 8/14/20.
//  Copyright © 2020 levilin. All rights reserved.
//
#include "platform_thread.h"
#include "thread_id_name_manager.h"
#include <pthread.h>
#include <memory>

namespace hive {

namespace {

struct ThreadParams {
  ThreadParams()
      : delegate(nullptr), joinable(false), priority(ThreadPriority::NORMAL) {}

  PlatformThread::Delegate* delegate;
  bool joinable;
  ThreadPriority priority;
};

size_t GetDefaultThreadStackSize(const pthread_attr_t& attributes) {
#if defined(OS_IOS)
  return 0;
#else
  // The Mac OS X default for a pthread stack size is 512kB.
  // Libc-594.1.4/pthreads/pthread.c's pthread_attr_init uses
  // DEFAULT_STACK_SIZE for this purpose.
  //
  // 512kB isn't quite generous enough for some deeply recursive threads that
  // otherwise request the default stack size by specifying 0. Here, adopt
  // glibc's behavior as on Linux, which is to use the current stack size
  // limit (ulimit -s) as the default stack size. See
  // glibc-2.11.1/nptl/nptl-init.c's __pthread_initialize_minimal_internal. To
  // avoid setting the limit below the Mac OS X default or the minimum usable
  // stack size, these values are also considered. If any of these values
  // can't be determined, or if stack size is unlimited (ulimit -s unlimited),
  // stack_size is left at 0 to get the system default.
  //
  // Mac OS X normally only applies ulimit -s to the main thread stack. On
  // contemporary OS X and Linux systems alike, this value is generally 8MB
  // or in that neighborhood.
  size_t default_stack_size = 0;
  struct rlimit stack_rlimit;
  if (pthread_attr_getstacksize(&attributes, &default_stack_size) == 0 &&
      getrlimit(RLIMIT_STACK, &stack_rlimit) == 0 &&
      stack_rlimit.rlim_cur != RLIM_INFINITY) {
    default_stack_size =
        std::max(std::max(default_stack_size,
                          static_cast<size_t>(PTHREAD_STACK_MIN)),
                 static_cast<size_t>(stack_rlimit.rlim_cur));
  }
  return default_stack_size;
#endif
}


void* ThreadFunc(void* params) {
    PlatformThread::Delegate* delegate = nullptr;
    
    {
        std::unique_ptr<ThreadParams> thread_params(static_cast<ThreadParams*>(params));

        delegate = thread_params->delegate;
//        if (!thread_params->joinable) {
//            base::ThreadRestrictions::SetSingletonAllowed(false);
//        }
//#if !defined(OS_NACL)
    // Threads on linux/android may inherit their priority from the thread
    // where they were created. This explicitly sets the priority of all new
    // threads.
        PlatformThread::SetCurrentThreadPriority(thread_params->priority);
//#endif
    }
    ThreadIdNameManager::GetInstance()->RegisterThread(
        PlatformThread::CurrentHandle().platform_handle(),
        PlatformThread::CurrentId());

    delegate->ThreadMain();
    
    return nullptr;
}

bool CreateThread(size_t stack_size,
                  bool joinable,
                  PlatformThread::Delegate* delegate,
                  PlatformThreadHandle* thread_handle,
                  ThreadPriority priority) {
    DCHECK(thread_handle);
    
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    // Pthreads are joinable by default, so only specify the detached
    // attribute if the thread should be non-joinable.
    if (!joinable)
      pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);

    // Get a better default if available.
    if (stack_size == 0)
      stack_size = GetDefaultThreadStackSize(attributes);

    if (stack_size > 0)
      pthread_attr_setstacksize(&attributes, stack_size);

    std::unique_ptr<ThreadParams> params(new ThreadParams);
    params->delegate = delegate;
    params->joinable = joinable;
    params->priority = priority;

    pthread_t handle;
    int err = pthread_create(&handle, &attributes, ThreadFunc, params.get());
    bool success = !err;
    if (success) {
        // ThreadParams should be deleted on the created thread after used.
        params.release();
    } else {
      // Value of |handle| is undefined if pthread_create fails.
        handle = 0;
//      errno = err;
        LOG(ERROR) << "pthread_create";
    }
    *thread_handle = PlatformThreadHandle(handle);

    pthread_attr_destroy(&attributes);

    return success;
}


}; // namespace


// static
PlatformThreadId PlatformThread::CurrentId() {
  // Pthreads doesn't have the concept of a thread ID, so we have to reach down
  // into the kernel.
#if defined(OS_MACOSX)
  return pthread_mach_thread_np(pthread_self());
#elif defined(OS_LINUX)
  static NoDestructor<InitAtFork> init_at_fork;
  if (g_thread_id == -1) {
    g_thread_id = syscall(__NR_gettid);
  } else {
    DCHECK_EQ(g_thread_id, syscall(__NR_gettid))
        << "Thread id stored in TLS is different from thread id returned by "
           "the system. It is likely that the process was forked without going "
           "through fork().";
  }
  return g_thread_id;
#elif defined(OS_ANDROID)
  return gettid();
#else
  return reinterpret_cast<int64_t>(pthread_self());
#endif
}

// static
PlatformThreadHandle PlatformThread::CurrentHandle() {
  return PlatformThreadHandle(pthread_self());
}

// static
bool PlatformThread::CreateWithPriority(size_t stack_size,
                                        Delegate* delegate,
                                        PlatformThreadHandle* thread_handle,
                                        ThreadPriority priority) {
    return CreateThread(stack_size, true /* joinable thread */, delegate,
                        thread_handle, priority);
}


}; // hive
