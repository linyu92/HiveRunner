//
//  callback_forward.h
//  Hive
//
//  Created by levilin on 8/21/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef callback_forward_h
#define callback_forward_h

namespace hive {

template <typename Signature>
class OnceCallback;

template <typename Signature>
class RepeatingCallback;

template <typename Signature>
using Callback = RepeatingCallback<Signature>;

// Syntactic sugar to make OnceClosure<void()> and RepeatingClosure<void()>
// easier to declare since they will be used in a lot of APIs with delayed
// execution.
using OnceClosure = OnceCallback<void()>;
using RepeatingClosure = RepeatingCallback<void()>;
using Closure = Callback<void()>;

}  // namespace base

#endif /* callback_forward_h */
