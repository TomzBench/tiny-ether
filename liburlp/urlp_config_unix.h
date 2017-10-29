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

#ifndef URLP_CONFIG_LINUX_EMU_H_
#define URLP_CONFIG_LINUX_EMU_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define URLP_CONFIG_ANYSIZE_ARRAY 1
#define URLP_IS_BIGENDIAN 1

#define urlp_malloc_fn malloc
#define urlp_free_fn free
#define urlp_clz_fn __builtin_clz

#endif
