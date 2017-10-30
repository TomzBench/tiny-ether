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

#include "ktable.h"
#include "urand.h"

int
ktable_init(ktable* ctx, knode* id)
{
    ctx->node = *id;
    return 0;
}

void
ktable_deinit(ktable* ctx)
{
    ((void)ctx);
}

int
ktable_add_node(ktable* ctx, knode* node)
{
    return 0;
}

uint32_t
ktable_buckets_len(ktable* ctx)
{
    return 0;
}
