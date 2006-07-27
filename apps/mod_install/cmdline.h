/*-----------------------------------------------------------------------
 * File: CMDLINE.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

unsigned CommandInitParse(char **argv, unsigned argc);
unsigned CommandGetOption(char option, char **parameter, unsigned *index);
unsigned CommandGetNextOption( char *option, char **parameter, unsigned index );
