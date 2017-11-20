// Copyright 2017 Altronix Corp.
// This file is part of the tiny-ether library
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @author Thomas Chiantia <thomas@altronix>
 * @date 2017
 */

/**
 * @file utimers.h
 *
 * @brief Simple wrapper around OS primitives to better support non standard
 * enviorments.
 */
#ifndef UTIMERS_H_
#define UTIMERS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usys_config.h"
#include "usys_time.h"

#define UTIMERS_EMPTY 0x01
#define UTIMERS_ARMED 0x02

#define UTIMERS_IS_EMPTY(t) (t.flags & UTIMERS_EMPTY)

typedef struct utimers
{
    uint32_t ms, fire;
    uint8_t flags;
    int key;
    void* ctx;
    int (*fn)(struct utimers*, void*, uint32_t);
} utimers;

typedef int (*utimers_fn)(utimers*, void*, uint32_t);

static inline void
utimers_init(utimers* timers, int count)
{
    memset(timers, 0, sizeof(utimers) * count);
    for (int i = 0; i < count; i++) timers[i].flags |= UTIMERS_EMPTY;
}

static inline void
utimers_deinit(utimers* timers, int count)
{
    memset(timers, 0, sizeof(utimers) * count);
}

static inline uint32_t
utimers_size(utimers* t, int c)
{
    uint32_t n = 0;
    for (int i = 0; i < c; i++) {
        if (!(UTIMERS_IS_EMPTY(t[i]))) n++;
    }
    return n;
}

static inline utimers*
utimers_get(utimers* timers, uint32_t idx)
{
    return !UTIMERS_IS_EMPTY(timers[idx]) ? &timers[idx] : NULL;
}

static inline int
utimers_insert(utimers* timers, int idx, utimers_fn fn, void* ctx)
{
    timers[idx].ms = timers[idx].flags = 0;
    timers[idx].fn = fn;
    timers[idx].ctx = ctx;
    timers[idx].key = idx;
    return 0;
}

static inline int
utimers_remove(utimers* timers, int idx)
{
    timers[idx].flags |= UTIMERS_EMPTY;
    return 0;
}

static inline int
utimers_start(utimers* timers, int idx, uint32_t ms)
{
    timers[idx].flags |= UTIMERS_ARMED;
    if (ms) timers[idx].ms = ms;
    timers[idx].fire = usys_tick() + timers[idx].ms;
    return 0;
}

static inline int
utimers_cancel(utimers* timers, int idx)
{
    timers[idx].flags &= (~(UTIMERS_ARMED));
    return 0;
}

static inline void
utimers_poll(utimers* timers, uint32_t count)
{
    uint32_t tick = usys_tick();
    for (uint32_t i = 0; i < count; i++) {
        if ((timers[i].flags & UTIMERS_ARMED) &&
            (!(timers[i].flags & UTIMERS_EMPTY)) && //
            (tick >= timers[i].fire)) {
            timers[i].flags &= (~(UTIMERS_ARMED));
            timers[i].fn(&timers[i], timers[i].ctx, tick);
        }
    }
}

#ifdef __cplusplus
}
#endif
#endif
