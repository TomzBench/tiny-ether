#include "async_io.h"

// Override system IO with MOCK implementation OR other IO implementation.
// Useful for test or portability.
usys_io_send_fn g_usys_async_io_send = usys_send;
usys_io_recv_fn g_usys_async_io_recv = usys_recv;
usys_io_ready_fn g_usys_async_io_ready = usys_sock_ready;
usys_io_connect_fn g_usys_async_io_connect = usys_connect;
usys_io_close_fn g_usys_async_io_close = usys_close;

// Private prototypes.
void async_error(async_io* self, int);

// Public
void
async_io_init(async_io* self, void* ctx, const async_io_settings* settings)
{
    // Zero mem
    memset(self, 0, sizeof(async_io));

    // Init state
    self->sock = -1;
    self->ctx = ctx;
    self->settings = *settings;

    // Need to override empty callbacks.
    if (!self->settings.tx) self->settings.tx = g_usys_async_io_send;
    if (!self->settings.rx) self->settings.rx = g_usys_async_io_recv;
    if (!self->settings.close) self->settings.close = g_usys_async_io_close;
    if (!self->settings.ready) self->settings.ready = g_usys_async_io_ready;
    if (!self->settings.connect)
        self->settings.connect = g_usys_async_io_connect;
    if (!self->settings.on_connect)
        self->settings.on_connect = async_io_default_on_connect;
    if (!self->settings.on_accept)
        self->settings.on_accept = async_io_default_on_accept;
    if (!self->settings.on_erro)
        self->settings.on_erro = async_io_default_on_erro;
    if (!self->settings.on_send)
        self->settings.on_send = async_io_default_on_send;
    if (!self->settings.on_recv)
        self->settings.on_recv = async_io_default_on_recv;
}

void
async_io_deinit(async_io* self)
{
    if (ASYNC_IO_SOCK(self)) self->settings.close(&self->sock);
    memset(self, 0, sizeof(async_io));
}

int
async_io_connect(async_io* self, const char* ip, uint32_t p)
{
    if (ASYNC_IO_SOCK(self)) self->settings.close(&self->sock);
    int ret = self->settings.connect(&self->sock, ip, p);
    if (ret < 0) {
        ASYNC_IO_SET_ERRO(self);
    } else if (ret == 0) {
        self->state |= ASYNC_IO_STATE_SEND;
    } else {
        self->state |= ASYNC_IO_STATE_RECV | ASYNC_IO_STATE_READY;
        self->settings.on_connect(self->ctx);
    }
    return ret;
}

void
async_io_close(async_io* self)
{
    ASYNC_IO_CLOSE(self);
}

const void*
async_io_memcpy(async_io* self, uint32_t idx, void* mem, size_t l)
{
    return memcpy(&self->b[idx], mem, l);
}

int
async_io_print(async_io* self, uint32_t idx, const char* fmt, ...)
{
    int l;
    va_list ap;
    va_start(ap, fmt);
    l = vsnprintf((char*)&self->b[idx], sizeof(self->b) - idx, fmt, ap);
    if (l >= 0) self->len = l;
    va_end(ap);
    return l;
}

int
async_io_send(async_io* self)
{
    if (ASYNC_IO_SOCK(self)) {
        ASYNC_IO_SET_SEND(self);
        return 0;
    } else {
        return -1;
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
    int c, ret = -1, end = self->len, start = self->c;
    ((void)start);
    if (!(ASYNC_IO_READY(self->state))) {
        if (ASYNC_IO_SOCK(self)) {
            ret = self->settings.ready(&self->sock);
            if (ret < 0) {
                ASYNC_IO_SET_ERRO(self);
            } else {
                ASYNC_IO_SET_READY(self);
                ASYNC_IO_SET_RECV(self);
            }
        } else {
        }
    } else if (ASYNC_IO_SEND(self->state)) {
        for (c = 0; c < 2; c++) {
            ret = self->settings.tx(&self->sock, &self->b[self->c],
                                    self->len - self->c);
            if (ret >= 0) {
                if (ret + (int)self->c == end) {
                    self->settings.on_send(self->ctx, 0, self->b, self->len);
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
                self->settings.on_send(self->ctx, -1, 0, 0); // IO error
                ASYNC_IO_SET_ERRO(self);
            }
        }
    } else if (ASYNC_IO_RECV(self->state)) {
        for (c = 0; c < 2; c++) {
            ret = self->settings.rx(&self->sock, &self->b[self->c],
                                    self->len - self->c);
            if (ret >= 0) {
                if (ret + (int)self->c == end) {
                    self->settings.on_recv(self->ctx, -1, 0,
                                           0); // buffer to small
                    ASYNC_IO_SET_ERRO(self);
                    break;
                } else if (ret == 0) {
                    if (c == 0) {
                        // When a readable socket returns 0 bytes on first then
                        // that means remote has disconnected.
                        ASYNC_IO_SET_ERRO(self);
                    } else {
                        self->settings.on_recv(self->ctx, 0, self->b, self->c);
                        ret = 0; // OK no more data
                    }
                    break;
                } else {
                    self->c += ret;
                    ret = 0; // OK maybe more data
                }
            } else {
                self->settings.on_recv(self->ctx, -1, 0, 0); // IO error
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
