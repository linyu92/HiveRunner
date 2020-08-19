//
//  thread_id_name_manager.h
//  HiveApp
//
//  Created by levilin on 8/19/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef thread_id_name_manager_hpp
#define thread_id_name_manager_hpp

#include <map>
#include <string>
#include <mutex>
#include "build/base_export.h"
#include "platform_thread.h"

namespace hive {

template <typename T>
struct DefaultSingletonTraits;

class BASE_EXPORT ThreadIdNameManager {
 public:
  static ThreadIdNameManager* GetInstance();

  static const char* GetDefaultInternedString();

  // Register the mapping between a thread |id| and |handle|.
  void RegisterThread(PlatformThreadHandle::Handle handle, PlatformThreadId id);

  // Set the name for the current thread.
  void SetName(const std::string& name);

  // Get the name for the given id.
  const char* GetName(PlatformThreadId id);

  // Unlike |GetName|, this method using TLS and avoids touching |lock_|.
  const char* GetNameForCurrentThread();

  // Remove the name for the given id.
  void RemoveName(PlatformThreadHandle::Handle handle, PlatformThreadId id);

 private:
  friend struct DefaultSingletonTraits<ThreadIdNameManager>;

  typedef std::map<PlatformThreadId, PlatformThreadHandle::Handle>
      ThreadIdToHandleMap;
  typedef std::map<PlatformThreadHandle::Handle, std::string*>
      ThreadHandleToInternedNameMap;
  typedef std::map<std::string, std::string*> NameToInternedNameMap;

  ThreadIdNameManager();
  ~ThreadIdNameManager();

  // lock_ protects the name_to_interned_name_, thread_id_to_handle_ and
  // thread_handle_to_interned_name_ maps.
  std::mutex m_mutex;

  NameToInternedNameMap name_to_interned_name_;
  ThreadIdToHandleMap thread_id_to_handle_;
  ThreadHandleToInternedNameMap thread_handle_to_interned_name_;

  // Treat the main process specially as there is no PlatformThreadHandle.
  std::string* main_process_name_;
  PlatformThreadId main_process_id_;

//  SetNameCallback set_name_callback_;
//  DISALLOW_COPY_AND_ASSIGN(ThreadIdNameManager);
};

}  // namespace hive

#endif /* thread_id_name_manager_hpp */
