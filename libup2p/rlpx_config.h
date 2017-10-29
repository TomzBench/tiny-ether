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

#ifndef URLPX_CONFIG_COMMON_H_
#define URLPX_CONFIG_COMMON_H_

// Include a user config file or use a default
#ifdef URLPX_CONFIG_FILE
#include URLPX_CONFIG_FILE
#elif URLPX_CONFIG_UNIX
#include "rlpx_config_unix.h"
#endif

// P2P client name
#define RLPX_CLIENT_ID_STR "tiny-ether"
#define RLPX_CLIENT_ID_LEN (sizeof(RLPX_CLIENT_ID_STR) - 1)
#define RLPX_VERSION_P2P 4

// DEVP2P client string max size (from "hello" packet)
#define RLPX_CLIENT_MAX_LEN 80

#endif
