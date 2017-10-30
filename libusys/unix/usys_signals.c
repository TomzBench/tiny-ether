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
