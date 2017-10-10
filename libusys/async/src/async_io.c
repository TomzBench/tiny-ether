#include "async_io.h"

int async_io_start(async_io* self);

void async_error(async_io* self, int);

void
async_io_init(async_io* self,
              void* ctx,
              async_io_cb on_send,
              async_io_cb on_recv,
              async_io_cb on_erro)
{
    memset(self, 0, sizeof(async_io));
    self->sock = -1;
    self->ctx = ctx;
    self->tx = usys_send;
    self->rx = usys_recv;
    self->on_send = on_send;
    self->on_recv = on_recv;
    self->on_erro = on_erro;
}

void
async_io_deinit(async_io* self)
{
    if (ASYNC_IO_SOCK(self)) usys_close(&self->sock);
    memset(self, 0, sizeof(async_io));
}

int
async_io_connect(async_io* self, const char* ip, uint32_t p)
{
    if (ASYNC_IO_SOCK(self)) usys_close(&self->sock);
    int ret = usys_connect(&self->sock, ip, p);
    if (ret < 0) {
        ASYNC_IO_SET_ERRO(self);
    } else if (ret == 0) {
        self->state |= ASYNC_IO_STATE_SEND;
    } else {
        self->state |= ASYNC_IO_STATE_READY;
    }
    return ret;
}

void
async_io_close(async_io* self)
{
    ASYNC_IO_CLOSE(self);
}

int
async_io_send(async_io* self, const char* ip, uint32_t port)
{
    if (ASYNC_IO_SOCK(self)) {
        ASYNC_IO_SET_SEND(self);
        return async_io_start(self);
    } else {
        return async_io_connect(self, ip, port);
    }
}

int
async_io_recv(async_io* self)
{
    if (ASYNC_IO_SOCK(self)) {
        ASYNC_IO_SET_RECV(self);
        return 0;
    } else {
        return -1;
    }
}

int
async_io_start(async_io* self)
{
    int ret, end = self->end;
    self->c = 0;
    ret = self->tx(&self->sock, &self->b[self->c], self->end - self->c);
    if (ret > 0) {
        if (ret == end) {
            ret = 0; // Send complete, put into listen
            ASYNC_IO_SET_RECV(self);
            self->on_send(self->ctx, ret, self->b, self->end);
        } else {
            ret = 0;
        }
    } else {
        ASYNC_IO_SET_ERRO(self);
    }
    return ret;
}

int
async_io_poll_n(async_io** io, uint32_t n, uint32_t ms)
{
    uint32_t mask = 0;
    int reads[n], writes[n], err;
    for (uint32_t c = 0; c < n; c++) {
        reads[c] = async_io_state_recv(io[c]) ? io[c]->sock : -1;
        writes[c] = async_io_state_send(io[c]) ? io[c]->sock : -1;
    }
    err = usys_select(&mask, &mask, ms, reads, n, writes, n);
    if (mask) {
        for (uint32_t i = 0; i < n; i++) {
            if (mask & (0x01 << i)) async_io_poll(io[i]);
        }
    }
    return err;
}

int
async_io_poll(async_io* self)
{
    int c, ret = -1, end = self->end, start = self->c;
    ((void)start);
    if (!(ASYNC_IO_READY(self->state))) {
        if (ASYNC_IO_SOCK(self)) {
            ret = usys_sock_ready(&self->sock);
            if (ret < 0) {
                ASYNC_IO_SET_ERRO(self);
            } else if (ret > 0) {
                if (self->end) {
                    self->state |= ASYNC_IO_STATE_READY;
                    ASYNC_IO_SET_SEND(self);
                    async_io_start(self);
                } else {
                    self->state |= ASYNC_IO_STATE_READY;
                }
            }
        } else {
        }
    } else if (ASYNC_IO_SEND(self->state)) {
        for (c = 0; c < 2; c++) {
            ret = self->tx(&self->sock, &self->b[self->c], self->end - self->c);
            if (ret >= 0) {
                if (ret + (int)self->c == end) {
                    self->on_send(self->ctx, 0, self->b, self->end);
                    ASYNC_IO_SET_RECV(self); // Send complete, put into listen
                    break;
                } else if (ret == 0) {
                    ret = 0; // OK, but maybe more to send
                    break;
                } else {
                    self->c += ret;
                    ret = 0; // OK, but maybe more to send
                }
            } else {
                self->on_send(self->ctx, -1, 0, 0); // IO error
                ASYNC_IO_SET_ERRO(self);
            }
        }
    } else if (ASYNC_IO_RECV(self->state)) {
        for (c = 0; c < 2; c++) {
            ret = self->rx(&self->sock, &self->b[self->c], self->end - self->c);
            if (ret >= 0) {
                if (ret + (int)self->c == end) {
                    self->on_recv(self->ctx, -1, 0, 0); // buffer to small
                    ASYNC_IO_SET_ERRO(self);
                    break;
                } else if (ret == 0) {
                    if (c == 0) {
                        // When a readable socket returns 0 bytes on first then
                        // that means remote has disconnected.
                        ASYNC_IO_SET_ERRO(self);
                    } else {
                        self->on_recv(self->ctx, 0, self->b, self->c);
                        ret = 0; // OK no more data
                    }
                    break;
                } else {
                    self->c += ret;
                    ret = 0; // OK maybe more data
                }
            } else {
                self->on_recv(self->ctx, -1, 0, 0); // IO error
                ASYNC_IO_SET_ERRO(self);
            }
        }
    }
    return ret;
}

int
async_io_sock(async_io* self)
{
    return ASYNC_IO_SOCK(self) ? self->sock : -1;
}

int
async_io_has_sock(async_io* self)
{
    return ASYNC_IO_SOCK(self);
}

int
async_io_state_recv(async_io* self)
{
    return ASYNC_IO_READY(self->state) && ASYNC_IO_RECV(self->state);
}

int
async_io_state_send(async_io* self)
{
    return ASYNC_IO_READY(self->state) ? ASYNC_IO_SEND(self->state) : 1;
}
