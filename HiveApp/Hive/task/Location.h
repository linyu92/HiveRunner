//
//  Location.h
//  HiveApp
//
//  Created by levilin on 9/2/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef Location_hpp
#define Location_hpp

#include <stddef.h>

#include <cassert>
#include <functional>
#include <string>

#include "build/base_export.h"

namespace hive {

// Location provides basic info where of an object was constructed, or was
// significantly brought to life.
class BASE_EXPORT Location {
 public:
  Location();
  Location(const Location& other);

  // Only initializes the file name and program counter, the source information
  // will be null for the strings, and -1 for the line number.
  // TODO(http://crbug.com/760702) remove file name from this constructor.
  Location(const char* file_name, const void* program_counter);

  // Constructor should be called with a long-lived char*, such as __FILE__.
  // It assumes the provided value will persist as a global constant, and it
  // will not make a copy of it.
  Location(const char* function_name,
           const char* file_name,
           int line_number,
           const void* program_counter);

  // Comparator for hash map insertion. The program counter should uniquely
  // identify a location.
  bool operator==(const Location& other) const {
    return program_counter_ == other.program_counter_;
  }

  // Returns true if there is source code location info. If this is false,
  // the Location object only contains a program counter or is
  // default-initialized (the program counter is also null).
  bool has_source_info() const { return function_name_ && file_name_; }

  // Will be nullptr for default initialized Location objects and when source
  // names are disabled.
  const char* function_name() const { return function_name_; }

  // Will be nullptr for default initialized Location objects and when source
  // names are disabled.
  const char* file_name() const { return file_name_; }

  // Will be -1 for default initialized Location objects and when source names
  // are disabled.
  int line_number() const { return line_number_; }

  // The address of the code generating this Location object. Should always be
  // valid except for default initialized Location objects, which will be
  // nullptr.
  const void* program_counter() const { return program_counter_; }

  // Converts to the most user-readable form possible. If function and filename
  // are not available, this will return "pc:<hex address>".
  std::string ToString() const;

  static Location CreateFromHere(const char* file_name);
  static Location CreateFromHere(const char* function_name,
                                 const char* file_name,
                                 int line_number);

 private:
  const char* function_name_ = nullptr;
  const char* file_name_ = nullptr;
  int line_number_ = -1;
  const void* program_counter_ = nullptr;
};

BASE_EXPORT const void* GetProgramCounter();

// Full source information should be included.
#define FROM_HERE FROM_HERE_WITH_EXPLICIT_FUNCTION(__func__)
#define FROM_HERE_WITH_EXPLICIT_FUNCTION(function_name) \
  ::base::Location::CreateFromHere(function_name, __FILE__, __LINE__)

}; // namespace hive

#endif /* Location_hpp */
