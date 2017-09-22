#ifndef RLPX_IO_H_
#define RLPX_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

int rlpx_io_connect();
int rlpx_io_on_connect();

int rlpx_io_disconnect();
int rlpx_io_on_disconnect();

int rlpx_io_send();
int rlpx_io_on_send();

int rlpx_io_recv();
int rlpx_io_on_recv();

#ifdef __cplusplus
}
#endif

#endif
