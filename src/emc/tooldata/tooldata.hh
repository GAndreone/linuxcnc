/*
** Copyright: 2020
** Author:    Dewey Garrett <dgarrett@panix.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#ifndef TOOL_DATA_H
#define TOOL_DATA_H

#include "canon.hh"
#include "emc_nml.hh"

#ifdef CPLUSPLUS
extern"C" {
#endif

//----------------------------------------------------------
// tool_tbl_*(): access to internal tool table data:
int                     tool_tbl_put(struct CANON_TOOL_TABLE t,int idx);
struct CANON_TOOL_TABLE tool_tbl_get(int idx);

void   tool_tbl_last_index_set(int idx);
int    tool_tbl_last_index_get(void);

//----------------------------------------------------------
//mmap specific
int  tool_mmap_creator(EMC_TOOL_STAT const  *ptr);
int  tool_mmap_user(void);
void tool_mmap_close(void);

//----------------------------------------------------------
//nml specific
int tool_nml_register(CANON_TOOL_TABLE *base);

#ifdef CPLUSPLUS
}
#endif
#endif
