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

#include "usys_log.h"

void
usys_log_(USYS_LOG_LEVEL lvl, const char* fmt, ...)
{
    char* usys_colors[] = USYS_LOG_COLORS;
    va_list ap;

    // Switch font color
    printf("%s", usys_colors[lvl]);

    // print user
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    // newline
    printf("\n" USYS_LOG_RESET);
}
