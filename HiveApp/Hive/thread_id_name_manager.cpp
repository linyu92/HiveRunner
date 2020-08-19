//
//  thread_id_name_manager.cpp
//  HiveApp
//
//  Created by levilin on 8/19/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#include "thread_id_name_manager.h"
#include "common/logging.h"

namespace hive {
namespace {

static const char kDefaultName[] = "";
static std::string* g_default_name;
static ThreadIdNameManager *g_singleton = nullptr;

}

ThreadIdNameManager::ThreadIdNameManager()
    : main_process_name_(nullptr), main_process_id_(kInvalidThreadId) {
  g_default_name = new std::string(kDefaultName);

  std::unique_lock<std::mutex> lock(m_mutex);
  name_to_interned_name_[kDefaultName] = g_default_name;
}

ThreadIdNameManager::~ThreadIdNameManager() = default;

ThreadIdNameManager* ThreadIdNameManager::GetInstance() {
    static std::once_flag flag;
    std::call_once(flag, [&](){ g_singleton = new ThreadIdNameManager(); });
    return g_singleton;
}

const char* ThreadIdNameManager::GetDefaultInternedString() {
  return g_default_name->c_str();
}

void ThreadIdNameManager::RegisterThread(PlatformThreadHandle::Handle handle,
                                         PlatformThreadId id) {
  std::unique_lock<std::mutex> lock(m_mutex);
  thread_id_to_handle_[id] = handle;
  thread_handle_to_interned_name_[handle] =
      name_to_interned_name_[kDefaultName];
}


void ThreadIdNameManager::SetName(const std::string& name) {
  PlatformThreadId id = PlatformThread::CurrentId();
  std::string* leaked_str = nullptr;
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    auto iter = name_to_interned_name_.find(name);
    if (iter != name_to_interned_name_.end()) {
      leaked_str = iter->second;
    } else {
      leaked_str = new std::string(name);
      name_to_interned_name_[name] = leaked_str;
    }

    auto id_to_handle_iter = thread_id_to_handle_.find(id);

//    GetThreadNameTLS().Set(const_cast<char*>(leaked_str->c_str()));
//    if (set_name_callback_) {
//      set_name_callback_.Run(leaked_str->c_str());
//    }

    // The main thread of a process will not be created as a Thread object which
    // means there is no PlatformThreadHandler registered.
    if (id_to_handle_iter == thread_id_to_handle_.end()) {
      main_process_name_ = leaked_str;
      main_process_id_ = id;
      return;
    }
    thread_handle_to_interned_name_[id_to_handle_iter->second] = leaked_str;
  }

}

const char* ThreadIdNameManager::GetName(PlatformThreadId id) {
  std::unique_lock<std::mutex> lock(m_mutex);

  if (id == main_process_id_)
    return main_process_name_->c_str();

  auto id_to_handle_iter = thread_id_to_handle_.find(id);
  if (id_to_handle_iter == thread_id_to_handle_.end())
    return name_to_interned_name_[kDefaultName]->c_str();

  auto handle_to_name_iter =
      thread_handle_to_interned_name_.find(id_to_handle_iter->second);
  return handle_to_name_iter->second->c_str();
}

const char* ThreadIdNameManager::GetNameForCurrentThread() {
    return GetName(PlatformThread::CurrentId());
}

void ThreadIdNameManager::RemoveName(PlatformThreadHandle::Handle handle,
                                     PlatformThreadId id) {
  std::unique_lock<std::mutex> lock(m_mutex);
  auto handle_to_name_iter = thread_handle_to_interned_name_.find(handle);

  DCHECK(handle_to_name_iter != thread_handle_to_interned_name_.end());
  thread_handle_to_interned_name_.erase(handle_to_name_iter);

  auto id_to_handle_iter = thread_id_to_handle_.find(id);
  DCHECK((id_to_handle_iter!= thread_id_to_handle_.end()));
  // The given |id| may have been re-used by the system. Make sure the
  // mapping points to the provided |handle| before removal.
  if (id_to_handle_iter->second != handle)
    return;

  thread_id_to_handle_.erase(id_to_handle_iter);
}

}  // namespace base
