/*-----------------------------------------------------------------------
 * File: APPMEMTRACK.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#ifndef APPMEMTRACK_H
#define APPMEMTRACK_H

//#if _DEBUG
#ifdef CSSM_MEMTRACK_ON

#define CMdsUtil_malloc(s, t) CMdsUtil_malloc((s), (t), __FILE__, __LINE__)
#define CMdsUtil_calloc(s, t, u) CMdsUtil_calloc((s), (t) ,(u), __FILE__, __LINE__)
#define CMdsUtil_realloc(s, t, u) CMdsUtil_realloc((s), (t), (u), __FILE__, __LINE__)
#define CMdsUtil_free(s, t) CMdsUtil_free((s), (t), __FILE__, __LINE__)

#define APP_MALLOC_WRAPPER(s, t, u) (s)((t), (u), __FILE__, __LINE__)
#define APP_CALLOC_WRAPPER(s, t, u, v) (s)((t) ,(u), (v), __FILE__, __LINE__)
#define APP_REALLOC_WRAPPER(s, t, u, v) (s)((t), (u), (v), __FILE__, __LINE__)
#define APP_FREE_WRAPPER(s, t, u) (s)((t), (u), __FILE__, __LINE__)

#else

#define CMdsUtil_malloc(s, t) CMdsUtil_malloc((s), (t))
#define CMdsUtil_calloc(s, t, u) CMdsUtil_calloc((s), (t) ,(u))
#define CMdsUtil_realloc(s, t, u) CMdsUtil_realloc((s), (t), (u))
#define CMdsUtil_free(s, t) CMdsUtil_free((s), (t))

#define APP_MALLOC_WRAPPER(s, t, u) (s)((t), (u), NULL, 0)
#define APP_CALLOC_WRAPPER(s, t, u, v) (s)((t) ,(u), (v), NULL, 0)
#define APP_REALLOC_WRAPPER(s, t, u, v) (s)((t), (u), (v), NULL, 0)
#define APP_FREE_WRAPPER(s, t, u) (s)((t), (u),NULL,0)

#endif // _DEBUG

#endif // APPMEMTRACK_H

