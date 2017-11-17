#include "usys_timers.h"

#define TIMERS_TEST_STORAGE_SIZE 200

int test_timers_fn(usys_timer_key key, void* ctx, uint32_t tick);

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
    // the timers key is an index in an array, our hash value is i+100 because
    // we want it to be different than the iterator to make sure api is correct
    // during test. So that is why you see i+100 in the loops using the api
    int err = 0, c = 0;
    usys_timers_context timers;
    usys_timer* timer = NULL;

    // Allocate timers
    usys_timers_init(&timers, TIMERS_TEST_STORAGE_SIZE);

    // Put some timers into our map
    for (uint32_t i = 0; i < 100; i++) {
        usys_timers_insert(&timers, i + 100, test_timers_fn, &c, i);
    }

    // Verify we have our timers
    for (uint32_t i = 0; i < 100; i++) {
        timer = usys_timers_get(&timers, i + 100);
        err |= (timer && timer->ms == i) ? 0 : -1;
    }

    // Check empty
    timer = usys_timers_get(&timers, 1);
    if (timer) err |= -1;

    // Check shrinking
    for (uint32_t i = 50; i < 100; i++) {
        usys_timers_remove(&timers, i + 100);
    }
    err |= usys_timers_size(&timers) == 50 ? 0 : -1;

    // Check overwrite
    for (uint32_t i = 0; i < 50; i++) {
        usys_timers_insert(&timers, i + 100, test_timers_fn, &c, i);
    }
    err |= usys_timers_size(&timers) == 50 ? 0 : -1;

    // Check overflow bounds
    for (uint32_t i = 1; i <= TIMERS_TEST_STORAGE_SIZE + 1; i++) {
        usys_timers_insert(&timers, i + 200, test_timers_fn, &c, i);
    }
    err |= usys_timers_size(&timers) == TIMERS_TEST_STORAGE_SIZE ? 0 : -1;

    // Really Check overflow bounds
    for (uint32_t i = 1; i <= TIMERS_TEST_STORAGE_SIZE * 10; i++) {
        usys_timers_insert(&timers, i + 200, test_timers_fn, &c, i);
    }
    err |= usys_timers_size(&timers) == TIMERS_TEST_STORAGE_SIZE ? 0 : -1;

    // Free hash table
    usys_timers_deinit(&timers);
    return err;
}

int
test_timers_trigger()
{
    // Note you'll fail test if you're stepping through with the debugger
    int err = 0;
    uint32_t now = 0, then = 0, diff;
    usys_timers_context timers;
    usys_timer_key key;

    // Init timer context
    usys_timers_init(&timers, 1);

    // Create a timer
    key = usys_timers_insert(&timers, 1234, test_timers_fn, &then, 0);

    // start timer and see how long until callback is fired (100ms)
    then = 0;
    now = usys_tick();
    usys_timers_start(&timers, 1234, 100);
    while (!then) usys_timers_poll(&timers);
    diff = then - now;
    err |= (diff >= 99 && diff <= 101) ? 0 : -1;

    // start timer and see how long until callback is fired (50ms)
    then = 0;
    now = usys_tick();
    usys_timers_start(&timers, 1234, 50);
    while (!then) usys_timers_poll(&timers);
    diff = then - now;
    err |= (diff >= 49 && diff <= 51) ? 0 : -1;

    // start timer and cancel it randomly make sure break
    then = 0;
    now = usys_tick();
    usys_timers_start(&timers, 1234, 1);
    usys_timers_cancel(&timers, 1234);
    usys_msleep(10);
    for (uint32_t i = 0; i < 100; i++) usys_timers_poll(&timers);
    err |= then ? -1 : 0; // then == 0 no timer fire

    usys_timers_deinit(&timers);
    return err;
}

int
test_timers_fn(usys_timer_key key, void* ctx, uint32_t tick)
{
    ((void)key);
    uint32_t* then = (uint32_t*)ctx;
    *then = tick;
    return 0;
}
