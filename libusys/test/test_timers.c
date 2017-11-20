#include "utimers.h"

int test_timers_fn(utimers* t, void* ctx, uint32_t tick);

int test_timers_storage();
int test_timers_trigger();

int
test_timers()
{
    int err = 0;
    err |= test_timers_storage();
    err |= test_timers_trigger();
    return err;
}

int
test_timers_storage()
{
    // the timers key is an index in an array
    int err = 0, c = 0;
    utimers timers[100];
    utimers* timer = NULL;

    // Init timers
    utimers_init(timers, 100);

    // Check init state
    err |= utimers_size(timers, 100) == 0 ? 0 : -1;

    // Add some timers
    for (uint32_t i = 0; i < 100; i++) {
        utimers_insert(timers, i, test_timers_fn, &c);
    }

    // Check timers added
    err |= utimers_size(timers, 100) == 100 ? 0 : -1;
    for (int i = 0; i < 100; i++) {
        timer = utimers_get(timers, i);
        err |= timer->key == i ? 0 : -1;
    }

    // Check shrinking
    for (int i = 50; i < 100; i++) {
        utimers_remove(timers, i);
    }
    err |= utimers_size(timers, 100) == 50 ? 0 : -1;

    // Check empty
    timer = utimers_get(timers, 50);
    if (timer) err |= -1;

    // Chec overwrite
    for (int i = 0; i < 50; i++) {
        utimers_insert(timers, i, test_timers_fn, &c);
    }
    err |= utimers_size(timers, 100) == 50 ? 0 : -1;

    // free timers
    utimers_deinit(timers, 100);
    return err;
}

int
test_timers_trigger()
{
    int err = 0;
    uint32_t now = 0, then = 0, diff;
    utimers timer[1];
    utimers_init(timer, 1);

    utimers_insert(timer, 0, test_timers_fn, &then);

    // start timer and see how long until callback is fired (100ms)
    then = 0;
    now = usys_tick();
    utimers_start(timer, 0, 100);
    while (!then) utimers_poll(timer, 1);
    diff = then - now;
    err |= (diff >= 99 && diff <= 101) ? 0 : -1;

    // start timer and see how long until callback is fired (50ms)
    then = 0;
    now = usys_tick();
    utimers_start(timer, 0, 50);
    while (!then) utimers_poll(timer, 1);
    diff = then - now;
    err |= (diff >= 49 && diff <= 51) ? 0 : -1;

    // start timer and cancel it randomly make sure break
    then = 0;
    now = usys_tick();
    utimers_start(timer, 0, 1);
    utimers_cancel(timer, 0);
    usys_msleep(10);
    for (uint32_t i = 0; i < 100; i++) utimers_poll(timer, 1);
    err |= then ? -1 : 0; // then == 0 no timer fire

    utimers_deinit(timer, 1);
    return err;
}

int
test_timers_fn(utimers* t, void* ctx, uint32_t tick)
{
    ((void)t);
    uint32_t* then = (uint32_t*)ctx;
    *then = tick;
    return 0;
}
