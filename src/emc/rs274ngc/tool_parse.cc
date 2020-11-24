//    Copyright (C) 2009-2010 Jeff Epler <jepler@unpythonic.net>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "emcglb.h"
#include "emctool.h"
#include "tool_parse.h"
#include <rtapi_string.h>
#include "tooldata.hh"

/********************************************************************
*
* Description: saveToolTable(const char *filename, ...
*		Saves the tool table into file filename.
*		  Array is CANON_TOOL_MAX + 1 entries, since 0 is included.
*
* Return Value: Zero on success or -1 if file not found.
*
* Side Effects: Default setting used if the parameter not found in
*		the ini file.
*
* Called By: ioControl
*
********************************************************************/
int saveToolTable(const char *filename,
#ifdef TOOL_MMAP //{
    // toolTable[] param not used for mmap
#else //}{
	CANON_TOOL_TABLE toolTable[],
#endif //}
	char *ttcomments[CANON_POCKETS_MAX],
	int random_toolchanger)
{
    int pocket;
    FILE *fp;
    const char *name;
    int start_pocket;

    // check filename
    if (filename[0] == 0) {
	name = tool_table_file;
    } else {
	// point to name provided
	name = filename;
    }

    // open tool table file
    if (NULL == (fp = fopen(name, "w"))) {
	// can't open file
	return -1;
    }

    if(random_toolchanger) {
        start_pocket = 0;
    } else {
        start_pocket = 1;
    }
    for (pocket = start_pocket; pocket < CANON_POCKETS_MAX; pocket++) {
        CANON_TOOL_TABLE ptemp;
        ptemp = tooldata_get(pocket);
        if (ptemp.toolno != -1) {
            fprintf(fp, "T%d P%d", ptemp.toolno, random_toolchanger? pocket: ptemp.pocketno);
            if (ptemp.diameter) fprintf(fp, " D%f", ptemp.diameter);
            if (ptemp.offset.tran.x) fprintf(fp, " X%+f", ptemp.offset.tran.x);
            if (ptemp.offset.tran.y) fprintf(fp, " Y%+f", ptemp.offset.tran.y);
            if (ptemp.offset.tran.z) fprintf(fp, " Z%+f", ptemp.offset.tran.z);
            if (ptemp.offset.a)      fprintf(fp, " A%+f", ptemp.offset.a);
            if (ptemp.offset.b)      fprintf(fp, " B%+f", ptemp.offset.b);
            if (ptemp.offset.c)      fprintf(fp, " C%+f", ptemp.offset.c);
            if (ptemp.offset.u)      fprintf(fp, " U%+f", ptemp.offset.u);
            if (ptemp.offset.v)      fprintf(fp, " V%+f", ptemp.offset.v);
            if (ptemp.offset.w)      fprintf(fp, " W%+f", ptemp.offset.w);
            if (ptemp.frontangle)    fprintf(fp, " I%+f", ptemp.frontangle);
            if (ptemp.backangle)     fprintf(fp, " J%+f", ptemp.backangle);
            if (ptemp.orientation)   fprintf(fp, " Q%d",  ptemp.orientation);
            fprintf(fp, " ;%s\n", ttcomments[pocket]);
            tooldata_put(ptemp,pocket);
        }
    }
    fclose(fp);
    return 0;
}

// future: make this non-static for other callers
static int tool_parse_one_line(CANON_TOOL_TABLE *entry, //result
                               int  *fakepocket,
                               int   random_toolchanger,
                               char *buffer,
                               char *ttcomments[])
{
    char orig_line[CANON_TOOL_ENTRY_LEN];
    const char *token;
    char *buff, *comment;
    int toolno,orientation,valid=1;
    EmcPose offset; //tlo
    double diameter, frontangle, backangle;
    int pocket = 0;
    int realpocket = 0;

    rtapi_strxcpy(orig_line, buffer);
    toolno = -1;
    diameter = frontangle = backangle = 0.0;
    orientation = 0;
    ZERO_EMC_POSE(offset);
    buff = strtok(buffer, ";");
    if (strlen(buff) <=1) {
        //fprintf(stderr,"skip blankline %s\n",__FILE__);
        return 0;
    }
    comment = strtok(NULL, "\n");

    token = strtok(buff, " ");
    while (token != NULL) {
        switch (toupper(token[0])) {
        case 'T':
            if (sscanf(&token[1], "%d", &toolno) != 1)
                valid = 0;
            break;
        case 'P':
            if (sscanf(&token[1], "%d", &pocket) != 1) {
                valid = 0;
                break;
            }
            realpocket = pocket;
            if (!random_toolchanger) {
                (*fakepocket)++;
                if (*fakepocket >= CANON_POCKETS_MAX) {
                    printf("too many tools. skipping tool %d\n", toolno);
                    valid = 0;
                    break;
                }
                pocket = *fakepocket;
            }
            if (pocket < 0 || pocket >= CANON_POCKETS_MAX) {
                printf("max pocket number is %d. skipping tool %d\n", CANON_POCKETS_MAX - 1, toolno);
                valid = 0;
                break;
            }
            break;
        case 'D':
            if (sscanf(&token[1], "%lf", &diameter) != 1)
                valid = 0;
            break;
        case 'X':
            if (sscanf(&token[1], "%lf", &offset.tran.x) != 1)
                valid = 0;
            break;
        case 'Y':
            if (sscanf(&token[1], "%lf", &offset.tran.y) != 1)
                valid = 0;
            break;
        case 'Z':
            if (sscanf(&token[1], "%lf", &offset.tran.z) != 1)
                valid = 0;
            break;
        case 'A':
            if (sscanf(&token[1], "%lf", &offset.a) != 1)
                valid = 0;
            break;
        case 'B':
            if (sscanf(&token[1], "%lf", &offset.b) != 1)
                valid = 0;
            break;
        case 'C':
            if (sscanf(&token[1], "%lf", &offset.c) != 1)
                valid = 0;
            break;
        case 'U':
            if (sscanf(&token[1], "%lf", &offset.u) != 1)
                valid = 0;
            break;
        case 'V':
            if (sscanf(&token[1], "%lf", &offset.v) != 1)
                valid = 0;
            break;
        case 'W':
            if (sscanf(&token[1], "%lf", &offset.w) != 1)
                valid = 0;
            break;
        case 'I':
            if (sscanf(&token[1], "%lf", &frontangle) != 1)
                valid = 0;
            break;
        case 'J':
            if (sscanf(&token[1], "%lf", &backangle) != 1)
                valid = 0;
            break;
        case 'Q':
            if (sscanf(&token[1], "%d", &orientation) != 1)
                valid = 0;
            break;
        default:
            if (strncmp(token, "\n", 1) != 0)
                valid = 0;
            break;
        }
        token = strtok(NULL, " ");
    }

    if (valid) {
        entry->toolno = toolno;
        entry->pocketno = realpocket;
        entry->offset = offset;
        entry->diameter = diameter;
        entry->frontangle = frontangle;
        entry->backangle = backangle;
        entry->orientation = orientation;
        //fprintf(stderr,"@@@ pno=%d tno=%d diam=%.3f\n",
        //        pocket,toolno,entry->diameter);
        tooldata_put(*entry,pocket);

        if (ttcomments && comment) {
             strcpy(ttcomments[pocket], comment);
         }
    } else {
         return -1;
    }
    return 0;
} // tool_parse_one_line()

int loadToolTable(const char *filename,
#ifdef TOOL_MMAP //{
             // toolTable[] param not used for mmap
#else //}{
			 CANON_TOOL_TABLE toolTable[],
#endif //}
			 char *ttcomments[],
			 int random_toolchanger)
{
    int t;
    FILE *fp;
    char buffer[CANON_TOOL_ENTRY_LEN];
    char orig_line[CANON_TOOL_ENTRY_LEN];
    int  fakepocket = 0;

    if(!filename) return -1;

    // open tool table file
    if (NULL == (fp = fopen(filename, "r"))) {
	// can't open file
	return -1;
    }
    // clear out tool table
    // (Set vars to indicate no tool in pocket):
    for (t = random_toolchanger? 0: 1; t < CANON_POCKETS_MAX; t++) {
        CANON_TOOL_TABLE temp;
        temp.toolno = -1;
        temp.pocketno = -1;
        ZERO_EMC_POSE(temp.offset);
        temp.diameter = 0.0;
        temp.frontangle = 0.0;
        temp.backangle = 0.0;
        temp.orientation = 0;
        tooldata_put(temp,t);
        if(ttcomments) ttcomments[t][0] = '\0';
    }

    // after initializing all available pockets:
    tooldata_last_index_set(0);
    // subsequent read from filename will update the last pocket
    // which becomes available by tooldata_last_index_get()

    /*
      Override 0's with codes from tool file
      File format is:

      <header>
      <pocket # 0..CANON_TOOL_MAX> <FMS id> <length> <diameter>
      ...

    */

    while (!feof(fp)) {
        // for nonrandom machines, just read the tools into pockets 1..n
        // no matter their tool numbers.  NB leave the spindle pocket 0
        // unchanged/empty.
        if (NULL == fgets(buffer, CANON_TOOL_ENTRY_LEN, fp)) {
            break;
        }
        CANON_TOOL_TABLE ptemp;
        rtapi_strxcpy(orig_line, buffer);
        if (tool_parse_one_line(&ptemp,
                                &fakepocket,
                                random_toolchanger,
                                buffer,
                                ttcomments)) {
            printf("File: %s Unrecognized line skipped:\n    %s",filename, orig_line);
            continue;
        }

        CANON_TOOL_TABLE ztemp = tooldata_get(0);
        if (!random_toolchanger && ztemp.toolno == ptemp.toolno) {
            ztemp = ptemp;
            tooldata_put(ztemp,0);
        }
    } //while

    // close the file
    fclose(fp);

    return 0;
} // loadToolTable()
