#include "test.h"

// Encrypt a udp packet
int test_discovery_write();
// Decrypt a udp packet
int test_discovery_read();
// Test some protocol ops
int test_discovery_protocol();

int
test_discovery()
{
    int err = 0;
    err |= test_discovery_read();
    err |= test_discovery_write();
    err |= test_discovery_protocol();
    return err;
}

int
test_discovery_write()
{
    return 0;
}

int
test_discovery_read()
{
    return 0;
}

int
test_discovery_protocol()
{
    return 0;
}
