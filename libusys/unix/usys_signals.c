#include "usys_signals.h"
#include <signal.h>

int s_usys_interrupted = 0;
void s_usys_signal_handler(int x);
void s_usys_signal_catch(void);

void
s_usys_signal_handler(int x)
{
    ((void)x);
    s_usys_interrupted = 1;
}

void
s_usys_signal_catch(void)
{
    struct sigaction action;
    action.sa_handler = s_usys_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

void
usys_install_signal_handlers()
{
    s_usys_signal_catch();
    signal(SIGPIPE, SIG_IGN);
}

int
usys_running()
{
    return !s_usys_interrupted;
}

void
usys_shutdown()
{
    s_usys_interrupted = 1;
}
