#include "usys_timers.h"

#define TIMERS_TEST_STORAGE_SIZE 200

int test_timers_fn(usys_timer* t, void* ctx, uint32_t tick);

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
    int err = 0, count = 0;
    usys_timers_context timers;
    usys_timer* timer = NULL;

    // Allocate timers
    usys_timers_init(&timers, TIMERS_TEST_STORAGE_SIZE);

    // Put some timers into our map
    for (uint32_t i = 1; i <= 100; i++) {
        usys_timers_insert(&timers, i, test_timers_fn, &count, i);
    }

    // Verify we have our timers
    for (uint32_t i = 1; i <= 100; i++) {
        timer = usys_timers_get(&timers, i);
        err |= (timer && timer->ms == i) ? 0 : -1;
    }

    // Check empty
    timer = usys_timers_get(&timers, 101);
    if (timer) err |= -1;

    // Check shrinking
    for (uint32_t i = 51; i <= 100; i++) {
        usys_timers_remove(&timers, i);
    }
    err = usys_timers_size(&timers) == 50 ? 0 : -1;

    // Check overwrite
    for (uint32_t i = 1; i <= 50; i++) {
        usys_timers_insert(&timers, i, test_timers_fn, &count, i);
    }
    err = usys_timers_size(&timers) == 50 ? 0 : -1;

    // Check overflow bounds
    for (uint32_t i = 1; i <= TIMERS_TEST_STORAGE_SIZE + 1; i++) {
        usys_timers_insert(&timers, i, test_timers_fn, &count, i);
    }
    err = usys_timers_size(&timers) == TIMERS_TEST_STORAGE_SIZE ? 0 : -1;

    // Really Check overflow bounds
    for (uint32_t i = 1; i <= TIMERS_TEST_STORAGE_SIZE * 10; i++) {
        usys_timers_insert(&timers, i, test_timers_fn, &count, i);
    }
    err = usys_timers_size(&timers) == TIMERS_TEST_STORAGE_SIZE ? 0 : -1;

    // Free hash table
    usys_timers_deinit(&timers);
    return err;
}

int
test_timers_trigger()
{
    int err = 0;
    usys_timers_context timers;
    usys_timers_init(&timers, 1);
    usys_timers_deinit(&timers);
    return err;
}

int
test_timers_fn(usys_timer* timer, void* ctx, uint32_t tick)
{
    int* err_p = (int*)ctx;
    *err_p = *err_p + 1;
    return 0;
}
