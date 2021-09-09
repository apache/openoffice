/**************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *************************************************************/



#include "osl/diagnose.h"
#include "system.h"

#ifndef HAVE_DLFCN_H

#if defined(LINUX) || defined(SOLARIS) || defined(FREEBSD)
#define HAVE_DLFCN_H
#endif /* LINUX || SOLARIS || FREEBSD */

#endif /* HAVE_DLFCN_H */


#ifdef HAVE_DLFCN_H

#ifndef INCLUDED_DLFCN_H
#include <dlfcn.h>
#define INCLUDED_DLFCN_H
#endif

#endif /* HAVE_DLFCN_H */
#include "osl/thread.h"

#ifndef INCLUDED_PTHREAD_H
#include <pthread.h>
#define INCLUDED_PTHREAD_H
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>
#define INCLUDED_STDDEF_H
#endif

#include "printtrace.h"

/************************************************************************/
/* Internal data structures and functions */
/************************************************************************/

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef pfunc_osl_printDebugMessage oslDebugMessageFunc;
static oslDebugMessageFunc volatile g_pDebugMessageFunc = 0;

typedef pfunc_osl_printDetailedDebugMessage oslDetailedDebugMessageFunc;
static oslDetailedDebugMessageFunc volatile g_pDetailedDebugMessageFunc = 0;

static void osl_diagnose_backtrace_Impl (
	oslDebugMessageFunc f);

#define OSL_DIAGNOSE_OUTPUTMESSAGE(f, s) \
((f != 0) ? (*(f))((s)) : (void)fprintf(stderr, "%s", (s)))

#if defined (LINUX) || defined (SOLARIS) || defined(FREEBSD)
/************************************************************************/
/* osl_diagnose_frame_Impl */
/************************************************************************/
static void osl_diagnose_frame_Impl (
	oslDebugMessageFunc f,
	int                 depth,
	void *              pc)
{
	const char *fname = NULL, *sname = NULL;
	void       *fbase = NULL, *saddr = NULL;
	ptrdiff_t   offset;
	char        szMessage[1024];

#ifdef INCLUDED_DLFCN_H
	Dl_info dli;
	if (dladdr (pc, &dli) != 0)
	{
		fname = dli.dli_fname;
		fbase = dli.dli_fbase;
		sname = dli.dli_sname;
		saddr = dli.dli_saddr;
	}
#endif /* INCLUDED_DLFCN_H */

	if (saddr)
		offset = (ptrdiff_t)(pc) - (ptrdiff_t)(saddr);
	else if (fbase)
		offset = (ptrdiff_t)(pc) - (ptrdiff_t)(fbase);
	else
		offset = (ptrdiff_t)(pc);

	snprintf (szMessage, sizeof(szMessage),
			  "Backtrace: [%d] %s: %s+0x%" SAL_PRI_PTRDIFFT "x\n",
			  depth,
			  fname ? fname : "<unknown>",
			  sname ? sname : "???",
			  offset);

	OSL_DIAGNOSE_OUTPUTMESSAGE(f, szMessage);
}
#endif

/************************************************************************/
/* osl_diagnose_backtrace_Impl */
/************************************************************************/
#if defined(LINUX)

#include <execinfo.h>

#define FRAME_COUNT 64
#define FRAME_OFFSET 1

static void osl_diagnose_backtrace_Impl (oslDebugMessageFunc f)
{
	void * ppFrames[FRAME_COUNT];
	int    i, n;

	n = backtrace (ppFrames, FRAME_COUNT);
	for (i = FRAME_OFFSET; i < n; i++)
	{
		osl_diagnose_frame_Impl (f, (i - FRAME_OFFSET), ppFrames[i]);
	}
}

#elif defined(SOLARIS)

#include <pthread.h>
#include <setjmp.h>
#include <sys/frame.h>

#if defined(SPARC)

#if defined IS_LP64

#define FRAME_PTR_OFFSET 1
#define FRAME_OFFSET     0
#define STACK_BIAS       0x7ff

#else

#define FRAME_PTR_OFFSET 1
#define FRAME_OFFSET     0
#define STACK_BIAS       0

#endif

#elif defined(INTEL)

#define FRAME_PTR_OFFSET 3
#define FRAME_OFFSET     0
#define STACK_BIAS       0

#endif /* (SPARC || INTEL) */

static void osl_diagnose_backtrace_Impl (oslDebugMessageFunc f)
{
	jmp_buf        ctx;
	long           fpval;
	struct frame * fp;
	int            i;

#if defined(SPARC)
	asm("ta 3");
#endif /* SPARC */
	setjmp (ctx);

	fpval = ((long*)(ctx))[FRAME_PTR_OFFSET];
	fp = (struct frame*)((char*)(fpval) + STACK_BIAS);

	for (i = 0; (i < FRAME_OFFSET) && (fp != NULL); i++)
		fp = (struct frame*)((char*)(fp->fr_savfp) + STACK_BIAS);

	for (i = 0; (fp != NULL) && (fp->fr_savpc != 0); i++)
	{
		struct frame * prev = (struct frame*)((char*)(fp->fr_savfp) + STACK_BIAS);
		osl_diagnose_frame_Impl (f, i, (void*)(fp->fr_savpc));
		fp = (prev > fp) ? prev : 0;
	}
}

#elif defined(FREEBSD)

#include <setjmp.h>
#include "backtrace.h" /* for struct frame */

#if defined(POWERPC) || defined(POWERPC64)

#define FRAME_PTR_OFFSET 1
#define FRAME_OFFSET     0
#define STACK_BIAS       0x7ff

#else /* defined(X86) || defined(X86_64) */

#define FRAME_PTR_OFFSET 3
#define FRAME_OFFSET 0

#endif /* (X86 || X86_64) */

static void osl_diagnose_backtrace_Impl (oslDebugMessageFunc f)
{
	struct frame * fp;
	jmp_buf        ctx;
	int            i;

	setjmp (ctx);
	fp = (struct frame*)(((long*)(ctx))[FRAME_PTR_OFFSET]);

	for (i = 0; (i < FRAME_OFFSET) && (fp != NULL); i++)
		fp = fp->fr_savfp;

	for (i = 0; (fp != NULL) && (fp->fr_savpc != 0); i++)
	{
		struct frame * prev = fp->fr_savfp;
		osl_diagnose_frame_Impl (f, i, (void*)(fp->fr_savpc));
		fp = (prev > fp) ? prev : 0;
	}
}

#else /* (LINUX || SOLARIS || FREEBSD) */

static void osl_diagnose_backtrace_Impl (oslDebugMessageFunc f)
{
	/* not yet implemented */
}

#endif /* (LINUX || SOLARIS || FREEBSD) */

/************************************************************************/
/* osl_assertFailedLine */
/************************************************************************/
sal_Bool SAL_CALL osl_assertFailedLine (
	const sal_Char* pszFileName,
	sal_Int32       nLine,
	const sal_Char* pszMessage)
{
	oslDebugMessageFunc f = g_pDebugMessageFunc;
	char szMessage[1024];

	// after reporting the assertion, abort if told so by SAL_DIAGNOSE_ABORT, but *not* if
	// assertions are routed to some external instance
	char const * env = getenv( "SAL_DIAGNOSE_ABORT" );
	sal_Bool const doAbort = ( ( env != NULL ) && ( *env != '\0' ) && ( f == NULL ) );

	/* If there's a callback for detailed messages, use it */
	if ( g_pDetailedDebugMessageFunc != NULL )
	{
		g_pDetailedDebugMessageFunc( pszFileName, nLine, pszMessage );
		return sal_False;
	}

	/* if SAL assertions are disabled in general, stop here */
	if ( getenv("DISABLE_SAL_DBGBOX") )
		return doAbort;

	/* format message into buffer */
	if (pszMessage != NULL)
	{
		snprintf(szMessage, sizeof(szMessage),
				 "Error: File %s, Line %" SAL_PRIdINT32 ": %s\n",
				 pszFileName, nLine, pszMessage);
	}
	else
	{
		snprintf(szMessage, sizeof(szMessage),
				 "Error: File %s, Line %" SAL_PRIdINT32 "\n",
				 pszFileName, nLine);
	}

	/* acquire lock to serialize output message(s) */
	pthread_mutex_lock(&g_mutex);

	/* output message buffer */
	OSL_DIAGNOSE_OUTPUTMESSAGE(f, szMessage);

	/* output backtrace */
	osl_diagnose_backtrace_Impl(f);

	/* release lock and leave */
	pthread_mutex_unlock(&g_mutex);

	return doAbort;
}

/************************************************************************/
/* osl_breakDebug */
/************************************************************************/
void SAL_CALL osl_breakDebug()
{
	abort();
}

/************************************************************************/
/* osl_reportError */
/************************************************************************/
sal_Int32 SAL_CALL osl_reportError (
	sal_uInt32 nType,
	const sal_Char* pszMessage)
{
	(void) nType; /* unused */
	fputs(pszMessage, stderr);
	return 0;
}

/************************************************************************/
/* osl_setDebugMessageFunc */
/************************************************************************/
oslDebugMessageFunc SAL_CALL osl_setDebugMessageFunc (
	oslDebugMessageFunc pNewFunc)
{
	oslDebugMessageFunc pOldFunc = g_pDebugMessageFunc;
	g_pDebugMessageFunc = pNewFunc;
	return pOldFunc;
}

/************************************************************************/
/* osl_setDetailedDebugMessageFunc */
/************************************************************************/
pfunc_osl_printDetailedDebugMessage SAL_CALL osl_setDetailedDebugMessageFunc (
	pfunc_osl_printDetailedDebugMessage pNewFunc)
{
	oslDetailedDebugMessageFunc pOldFunc = g_pDetailedDebugMessageFunc;
	g_pDetailedDebugMessageFunc = pNewFunc;
	return pOldFunc;
}

/************************************************************************/
/* osl_trace */
/************************************************************************/
void osl_trace(char const * pszFormat, ...) {
	va_list args;
	va_start(args, pszFormat);
	printTrace((unsigned long) getpid(), pszFormat, args);
	va_end(args);
}
