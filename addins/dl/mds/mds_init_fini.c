/*-----------------------------------------------------------------------
 * File: MDS_INIT_FINI.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */
 
#if defined (LINUX) || defined (SOLARIS)
static int _init() __attribute__ ((constructor));
static int _fini() __attribute__ ((destructor));
#endif

#ifdef UNIX
int _init_mds();
int _fini_mds();
int _init()
{
  _init_mds();
}
int _fini()
{
  _fini_mds();
}

#endif
