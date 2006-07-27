/*-----------------------------------------------------------------------
 * File: THREADS.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#include "internal.h"



/* thread create/join functions */

#ifdef WIN32

typedef struct port_thread_param_func
{
    port_ThreadProc ThreadFunc;
    void *FuncParams;
} PORT_THREAD_PARAM_FUNC, *PORT_THREAD_PARAM_FUNC_PTR;



void __cdecl
StartDetachedWin32Thread(void *Args)
{
    PORT_THREAD_PARAM_FUNC_PTR ThreadParams;

    ThreadParams = (PORT_THREAD_PARAM_FUNC_PTR) Args;

    if(NULL != ThreadParams)
    {
        ERR(ThreadParams->ThreadFunc(ThreadParams->FuncParams));

        local_free(Args,
                   NULL);
    }

    _endthread();
}

unsigned __stdcall
StartAttachedWin32Thread(void *Args)
{
    PORT_THREAD_PARAM_FUNC_PTR ThreadParams;
    CSSM_RETURN err = CSSM_OK;

    ThreadParams = (PORT_THREAD_PARAM_FUNC_PTR) Args;

    if(NULL != ThreadParams)
    {
        ERR(err = ThreadParams->ThreadFunc(ThreadParams->FuncParams));

        local_free(Args,
                   NULL);
    }
    else
    {
        ERR(err = CSSMERR_CSSM_INTERNAL_ERROR);
    }

    _endthreadex(err);

    return err;
}

#endif  /* WIN32 */


CSSM_RETURN CSSMAPI
port_CreateThread(port_ThreadProc Proc,
                  void *ProcParam,
                  CSSM_BOOL Detached,
                  PORT_THREAD_HANDLE *NewThread)
{
    PORT_THREAD_HANDLE hThread = 0;
    CSSM_RETURN err = CSSM_OK;


    if(CSSM_TRUE == port_IsBadCodePtr((CSSM_PROC_ADDR)Proc))
    {
        ERR(err = CSSMERR_CSSM_INVALID_POINTER);
    }
    else
    {

#if defined (WIN32)

        PORT_THREAD_PARAM_FUNC_PTR Params = local_malloc(sizeof(PORT_THREAD_PARAM_FUNC),
                                                         NULL);
        if(NULL != Params)
        {
            Params->ThreadFunc = Proc;
            Params->FuncParams = ProcParam;

            if(CSSM_TRUE == Detached)
            {
                if(-1 == _beginthread(StartDetachedWin32Thread,
                                      0,
                                      Params))
                {
                    ERR(err = CSSMERR_CSSM_INTERNAL_ERROR);
                    local_free(Params,
                               NULL);
                }
            }
            else
            {
                unsigned threadID; /* Patch by SmartTrust */

				hThread = (void*)_beginthreadex(NULL,
                                                0,
                                                StartAttachedWin32Thread,
                                                Params,
                                                0,
                                                &threadID);
                if(0 == hThread)
                {
                    ERR(err = CSSMERR_CSSM_INTERNAL_ERROR);
                    local_free(Params,
                               NULL);
                }
                else
                {
                    if(NULL != NewThread)
                    {
                        *NewThread = hThread;
                    }
                }
            }
        }
        else
        {
            ERR(err = CSSMERR_CSSM_MEMORY_ERROR);
        }
#elif defined (POSIX)
    pthread_attr_t attr;

    if (pthread_attr_init(&attr))
    {
        ERR(err = CSSMERR_CSSM_INTERNAL_ERROR);
        RETURN(err);
    }

    if(pthread_attr_setdetachstate(&attr,
                       (CSSM_TRUE == Detached) ?
                       PTHREAD_CREATE_DETACHED :
                       PTHREAD_CREATE_JOINABLE))
    {
        ERR(err = CSSMERR_CSSM_INTERNAL_ERROR);
        goto attr_destroy;
    }

    if(pthread_create(&hThread, &attr,
              (void *(*)(void *)) Proc, ProcParam))
    {
        ERR(err = CSSMERR_CSSM_INTERNAL_ERROR);
        goto attr_destroy;
    }

    if(NULL != NewThread)
    {
        *NewThread = hThread;
    }

    attr_destroy:

    if(pthread_attr_destroy(&attr))
    {
        ERR(err = CSSMERR_CSSM_INTERNAL_ERROR);
    }
#else
    /* for other systems -- put code here */
#endif

    }

    RETURN(err);
}


CSSM_RETURN CSSMAPI
port_JoinThread(PORT_THREAD_HANDLE Thread,
                CSSM_RETURN *ThreadReturn)
{
    CSSM_RETURN err = CSSM_OK;

#ifdef WIN32

    CSSM_RETURN localThreadRet;

    if(WAIT_FAILED != WaitForSingleObject(Thread,
                                          INFINITE))
    {
        if(FALSE != GetExitCodeThread(Thread,
                                      (unsigned long*)&localThreadRet))
        {
            if(NULL != ThreadReturn)
            {
                *ThreadReturn = localThreadRet;
            }
        }
        else
        {
            ERR(err = CSSMERR_CSSM_INTERNAL_ERROR);
        }

        CloseHandle(Thread);
    }
    else
    {
        ERR(err = CSSMERR_CSSM_INTERNAL_ERROR);
    }
#elif defined (POSIX)
    CSSM_RETURN localThreadRet;

    if(pthread_join(Thread, (void*)&localThreadRet))
    {
        ERR(err = CSSMERR_CSSM_INTERNAL_ERROR);
        RETURN(err);
    }

    if(NULL != ThreadReturn)
    {
        *ThreadReturn = localThreadRet;
    }
#endif /* WIN32 */

    return err;
}

