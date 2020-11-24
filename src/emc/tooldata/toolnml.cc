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

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "tooldata.hh"

static CANON_TOOL_TABLE *thetable;

int tool_nml_register(CANON_TOOL_TABLE *basetable)
{
    if (!basetable) {
        fprintf(stderr,"%8d!!!PROBLEM tool_nml_register()\n",getpid());
        return -1;
    }
    thetable = basetable;
    return 0;
} //tool_nml_register

void tooldata_last_index_set(int idx)  //force last_index
{
    return;  //noop for nml
} //tooldata_last_index_set()

int tooldata_last_index_get(void)
{
    return CANON_POCKETS_MAX -1; //fixed value with nml
} // tooldata_last_index_get()


int tooldata_put(CANON_TOOL_TABLE from,int idx)
{
    if (!thetable) {
        fprintf(stderr,"%8d!!!tool_nml_put() NOT INITIALIZED\n",getpid());
    }
    if (idx < 0 || idx >= CANON_POCKETS_MAX) {
        fprintf(stderr,"!!!%8d PROBLEM: tooldata_put(): bad idx=%d, maxallowed=%d\n",
                getpid(),idx,CANON_POCKETS_MAX-1);
        idx = 0;
        fprintf(stderr,"!!!continuing using idx=%d\n",idx);
    }
    *(thetable + idx) = from;

    return 0;
} // tooldata_put()

struct CANON_TOOL_TABLE tooldata_get(int idx)
{
    if (idx < 0 || idx >= CANON_POCKETS_MAX) {
        fprintf(stderr,"!!!%8d PROBLEM tooldata_get(): idx=%d, maxallowed=%d\n",
                getpid(),idx,CANON_POCKETS_MAX-1);
        idx = 0;
        fprintf(stderr,"!!!continuing using idx=%d\n",idx);
    }

    if (!thetable) {
        fprintf(stderr,"!!!%8d PROBLEM tooldata_get(): idx=%d thetable=%p\n",
                getpid(),idx,thetable);
        return (*(struct CANON_TOOL_TABLE*)(NULL));
    }

    return  *(struct CANON_TOOL_TABLE*)(thetable + idx);
} // tooldata_get()
