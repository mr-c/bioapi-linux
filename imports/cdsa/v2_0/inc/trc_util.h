/*-----------------------------------------------------------------------
 * File: TRC_UTIL.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

/* Portions:
 * (C) COPYRIGHT International Business Machines Corp. 1996, 1997
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */ 

#ifdef DO_TRACE
#ifdef BINARY_TRACE

#define TRACE(trace_info, function)\
uint32 ThisFunction = function;\
char* ThisFilter = trace_info.filter;\ uint32 Level = 1;\
\
BinWrite(ThisFilter, Level, 0, ThisFunction, 0)

#define TRACEL(trace_info, level, function)\
uint32 ThisFunction = function;\
char* ThisFilter = trace_info.filter;\ uint32 Level = level;\
\
BinWrite(ThisFilter, Level, 0, ThisFunction, 0)

#define RETURN(x)\
{\
BinWrite(ThisFilter, Level, 1, ThisFunction, (uint32)x);\ return(x);\
}

#define RETURN_EXPRESSION(x)\
{\
BinWrite(ThisFilter, Level, 2, ThisFunction, 0);\ return(x);\
}

#define VOID_RETURN\
{\
BinWrite(ThisFilter, Level, 3, ThisFunction, 0);\ return;\
}

#else /* if not BINARY_TRACE */

#define TRACE(trace_info, function)\
char* ThisFunction = trace_info.function_names[function];\ char* ThisFilter = trace_info.filter;\
uint32 Level = 1;\
\
Dprintf (ThisFilter, Level, "%s-> %s\n", IncreaseStackDepth(),
ThisFunction)

#define TRACEL(trace_info, level, function)\
char* ThisFunction = trace_info.function_names[function];\ char* ThisFilter = trace_info.filter;\
uint32 Level = level;\
\
Dprintf (ThisFilter, Level, "%s-> %s\n", IncreaseStackDepth(),
ThisFunction)

#define RETURN(x)\
{\
Dprintf(ThisFilter, Level, "%s <- %s (%d)\n", DecreaseStackDepth(), ThisFunction, x);\
return(x);\
}

#define RETURN_EXPRESSION(x)\
{\
Dprintf(ThisFilter, Level, "%s <- %s (?)\n",
DecreaseStackDepth(), ThisFunction);\
return(x);\
}

#define VOID_RETURN\
{\
Dprintf(ThisFilter, Level, "%s <- %s\n", DecreaseStackDepth(),
ThisFunction);\
return;\
}
#endif /* BINARY_TRACE */

#else /* if not TRACE */
#define TRACE(trace_info, function)
#define TRECEL(trace_info, level, function) 
#define RETURN(x) return(x);
#define RETURN_EXPRESSION(x) return(x); 
#define VOID_RETURN  return;
#endif /* TRACE */
