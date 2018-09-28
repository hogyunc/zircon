// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_SYNC_MTX_H_
#define LIB_SYNC_MTX_H_

#include <zircon/compiler.h>
#include <zircon/types.h>

__BEGIN_CDECLS;

// An optimal, non-recursive mutex on Fuchsia.
//
// The |mtx_t| mutex in the standard library has several quirks in its design
// that prevent it from being optimal. For example, the |mtx_t| interface
// supports recursion, which adds a branch to |mtx_init| to check that the
// client has not asked for recusion, and |mtx_timedlock| operates in
// |struct timespec| rather than |zx_time_t|.
//
// |sync_mtx| resolves these issues.
typedef struct __TA_CAPABILITY("mutex") sync_mtx {
    zx_futex_t futex;

#ifdef __cplusplus
    sync_mtx()
        : futex(0) {}
#endif
} sync_mtx_t;

#if !defined(__cplusplus)
#define SYNC_MTX_INIT ((sync_mtx_t){0})
#endif

// Locks the mutex.
//
// The current thread will block until the mutex is acquired. The mutex is
// non-recursive, which means attempting to lock a mutex that is already held by
// this thread will deadlock.
void sync_mtx_lock(sync_mtx_t* m) __TA_ACQUIRE(m);

// Attempt to lock the mutex until |deadline|.
//
// The current thread will block until either the mutex is acquired or
// |deadline| passes.
//
// |deadline| is expressed as an absolute time in the ZX_CLOCK_MONOTONIC
// timebase.
//
// Returns |ZX_OK| if the lock is acquired, and |ZX_ERR_TIMED_OUT| if the
// deadline passes.
zx_status_t sync_mtx_timedlock(sync_mtx_t* m, zx_time_t deadline);

// Attempts to lock the mutex without blocking.
//
// Returns |ZX_OK| if the lock is obtained, and |ZX_ERR_BAD_STATE| if not.
zx_status_t sync_mtx_trylock(sync_mtx_t* m);

// Unlocks the mutex.
//
// Does nothing if the mutex is already unlocked.
void sync_mtx_unlock(sync_mtx_t* m) __TA_RELEASE(m);

__END_CDECLS;

#endif // LIB_SYNC_MTX_H_