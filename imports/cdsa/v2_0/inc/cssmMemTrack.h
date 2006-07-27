/*-----------------------------------------------------------------------
 * File: CSSMMEMTRACK.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#ifndef CSSM_MEMTRACK_H
#define CSSM_MEMTRACK_H


#if defined (WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* The design of the following macros was motivated by the need for *
 * minimal code changes.  As a result, some of these macros appear  *
 * to violate principles of good programming.                       */


#ifndef CSSM_MEMTRACK_ON

#define CSSM_MALLOC_WRAPPER(s, t, u) (s)((t), (u))
#define CSSM_CALLOC_WRAPPER(s, t, u, v) (s)((t) ,(u), (v))
#define CSSM_REALLOC_WRAPPER(s, t, u, v) (s)((t), (u), (v))
#define CSSM_FREE_WRAPPER(s, t, u) (s)((t), (u))

#define mds_malloc(s, t) mds_malloc((s), (t))
#define mds_calloc(s, t, u) mds_calloc((s), (t) ,(u))
#define mds_realloc(s, t, u) mds_realloc((s), (t), (u))
#define mds_free(s, t) mds_free((s), (t))

#define TAL_Malloc(s, t) TAL_Malloc((s), (t))
#define TAL_Calloc(s, t, u) TAL_Calloc((s), (t) ,(u))
#define TAL_Realloc(s, t, u) TAL_Realloc((s), (t), (u))
#define TAL_Free(s, t) TAL_Free((s), (t))

#define TAL_CallocMem(s, t, u, v) TAL_CallocMem((s), (t), (u), (v))

#define wrapped_malloc_func(s, t, u, v) wrapped_malloc_func((s), (t), (u), (v))
#define wrapped_calloc_func(s, t, u, v, w) wrapped_calloc_func((s), (t), (u), (v), (w))
#define wrapped_realloc_func(s, t, u, v, w) wrapped_realloc_func((s), (t), (u), (v), (w))
#define wrapped_free_func(s, t, u, v) wrapped_free_func((s), (t), (u), (v))

#define app_malloc(s, t) _app_malloc((s), (t))
#define app_calloc(s, t, u) _app_calloc((s), (t) ,(u))
#define app_realloc(s, t, u) _app_realloc((s), (t), (u))
#define app_free(s, t) _app_free((s), (t))

#define dummy_malloc(s, t) dummy_malloc((s), (t))
#define dummy_calloc(s, t, u) dummy_calloc((s), (t) ,(u))
#define dummy_realloc(s, t, u) dummy_realloc((s), (t), (u))
#define dummy_free(s, t) dummy_free((s), (t))

#else   /* CSSM_MEMTRACK_ON */

#define CSSM_MALLOC_WRAPPER(s, t, u) (s)((t), (u), __FILE__, __LINE__)
#define CSSM_CALLOC_WRAPPER(s, t, u, v) (s)((t) ,(u), (v), __FILE__, __LINE__)
#define CSSM_REALLOC_WRAPPER(s, t, u, v) (s)((t), (u), (v), __FILE__, __LINE__)
#define CSSM_FREE_WRAPPER(s, t, u) (s)((t), (u), __FILE__, __LINE__ )

#define mds_malloc(s, t) mds_malloc((s), (t), __FILE__, __LINE__)
#define mds_calloc(s, t, u) mds_calloc((s), (t) ,(u), __FILE__, __LINE__)
#define mds_realloc(s, t, u) mds_realloc((s), (t), (u), __FILE__, __LINE__)
#define mds_free(s, t) mds_free((s), (t), __FILE__, __LINE__)

#define TAL_Malloc(s, t) TAL_Malloc((s), (t), __FILE__, __LINE__)
#define TAL_Calloc(s, t, u) TAL_Calloc((s), (t) ,(u), __FILE__, __LINE__)
#define TAL_Realloc(s, t, u) TAL_Realloc((s), (t), (u), __FILE__, __LINE__)
#define TAL_Free(s, t) TAL_Free((s), (t), __FILE__, __LINE__)

#define TAL_CallocMem(s, t, u, v) TAL_CallocMem((s), (t), (u), (v), __FILE__, __LINE__)

#define wrapped_malloc_func(s, t, u, v) wrapped_malloc_func((s), (t), (u), (v), __FILE__, __LINE__)
#define wrapped_calloc_func(s, t, u, v, w) wrapped_calloc_func((s), (t), (u), (v), (w), __FILE__, __LINE__)
#define wrapped_realloc_func(s, t, u, v, w) wrapped_realloc_func((s), (t), (u), (v), (w), __FILE__, __LINE__)
#define wrapped_free_func(s, t, u, v) wrapped_free_func((s), (t), (u), (v), __FILE__, __LINE__)

#define app_malloc(s, t) _app_malloc((s), (t), __FILE__, __LINE__)
#define app_calloc(s, t, u) _app_calloc((s), (t) ,(u), __FILE__, __LINE__)
#define app_realloc(s, t, u) _app_realloc((s), (t), (u), __FILE__, __LINE__)
#define app_free(s, t) _app_free((s), (t), __FILE__, __LINE__)

#define dummy_malloc(s, t) dummy_malloc((s), (t), __FILE__, __LINE__)
#define dummy_calloc(s, t, u) dummy_calloc((s), (t) ,(u), __FILE__, __LINE__)
#define dummy_realloc(s, t, u) dummy_realloc((s), (t), (u), __FILE__, __LINE__)
#define dummy_free(s, t) dummy_free((s), (t), __FILE__, __LINE__)

#endif  /* CSSM_MEMTRACK_ON */

#ifdef __cplusplus
}
#endif

#endif
