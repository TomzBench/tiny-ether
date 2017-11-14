#include "test.h"
#include "usys_time.h"

#define TEST_SIZE 100
#define NUM_ROUNDS 20

// Expose private for test
extern int rlpx_io_on_send_to(void* ctx, int err, const uint8_t* b, uint32_t l);

// Mock settings
extern async_io_mock_settings g_io_mock_tcp_settings;
extern async_io_mock_settings g_io_mock_udp_settings;

// Mock callbacks
int test_mock_on_recv(void* ctx, int err, uint8_t* b, uint32_t l);
int test_mock_on_send(void* ctx, int err, const uint8_t* b, uint32_t l);

// Counter for pass/fail test
uint32_t g_test_io_bytes_sent = 0;

int
test_io()
{
    int err = -1;
    uint32_t c, now, udp[2];     // alice bob receive ports
    uint8_t b[TEST_SIZE];        // Transmitting tx
    uecc_ctx keys[2];            // alice/bob keys
    rlpx_io io[2];               // alice bob
    memset(b, 'A', TEST_SIZE);   // init vector
    uecc_key_init_new(&keys[0]); // alice key
    uecc_key_init_new(&keys[1]); // bob key
    udp[0] = 20202;              // alice udp
    udp[1] = 20203;              // bob udp

    // Init rlpx io
    rlpx_io_udp_init(&io[0], &keys[0], &udp[0]);
    rlpx_io_udp_init(&io[1], &keys[1], &udp[1]);
    async_io_on_recv(&io[0].io, test_mock_on_recv);
    async_io_on_recv(&io[1].io, test_mock_on_recv);
    async_io_on_send(&io[0].io, test_mock_on_send);

    // Add mock tx/rx net driver
    async_io_install_mock(&io[0].io, &g_io_mock_udp_settings);
    async_io_install_mock(&io[1].io, &g_io_mock_udp_settings);

    // Flood transmit queue to max
    for (c = 0; c < NUM_ROUNDS; c++) {
        rlpx_io_sendto(&io[0], usys_atoh("127.0.0.1"), udp[1], b, TEST_SIZE);
    }

    // Process queue
    now = usys_now();
    while (async_io_state_send(&io[0].io)) async_io_poll(&io[0].io);

    // Make sure we sent all
    if (!(g_test_io_bytes_sent == (NUM_ROUNDS * TEST_SIZE))) goto EXIT;
    if (!(io[0].outgoing_count == 0)) goto EXIT;
    if (!(io[0].outgoing_bytes == 0)) goto EXIT;

    // Reset bytes sent
    g_test_io_bytes_sent = 0;

    // Set throttle
    rlpx_io_outgoing_throttle(io, 2000);

    // Flood transmit queue to max +1 (test tx fail condition)
    for (c = 0; c < NUM_ROUNDS + 1; c++) {
        rlpx_io_sendto(&io[0], usys_atoh("127.0.0.1"), udp[1], b, TEST_SIZE);
    }

    // Process queue
    now = usys_now();
    while (async_io_state_send(&io[0].io)) async_io_poll(&io[0].io);

    // Make sure we sent all but not more than we allow for
    if (!(g_test_io_bytes_sent == (NUM_ROUNDS * TEST_SIZE))) goto EXIT;
    if (!(io[0].outgoing_count == 0)) goto EXIT;
    if (!(io[0].outgoing_bytes == 0)) goto EXIT;

    // Flood transmit queue then shutdown (test memleak)
    for (c = 0; c < NUM_ROUNDS; c++) {
        rlpx_io_sendto(&io[0], usys_atoh("127.0.0.1"), udp[1], b, TEST_SIZE);
    }

    err = 0;
EXIT:
    // Cleanup
    uecc_key_deinit(&keys[0]);
    uecc_key_deinit(&keys[1]);
    rlpx_io_deinit(&io[0]);
    rlpx_io_deinit(&io[1]);
    return err;
}

int
test_mock_on_recv(void* ctx, int err, uint8_t* b, uint32_t l)
{
    rlpx_io* io = (rlpx_io*)ctx;
    ((void)io);
    ((void)b);
    ((void)l);
    ((void)err);
    return 0;
}

int
test_mock_on_send(void* ctx, int err, const uint8_t* b, uint32_t l)
{
    rlpx_io* io = (rlpx_io*)ctx;
    ((void)b);
    ((void)l);
    ((void)err);
    rlpx_io_on_send_to(io, err, b, l);
    g_test_io_bytes_sent += l;
    return 0;
}
