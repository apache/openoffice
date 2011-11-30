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



#ifndef _SVIPCDEF_H
#define _SVIPCDEF_H

namespace binfilter
{

#ifdef __cplusplus
extern "C" {
#endif

#if defined (WNT) || defined (WIN)
#define CDECL __cdecl
#elif defined CSET
#define CDECL _System
#else
#define CDECL
#endif

#if defined WIN
#define _EXTLIBCALL_ _pascal
#else
#define _EXTLIBCALL_ CDECL
#endif

typedef void (CDECL *IPCCallbackFunc)(void*);

extern void _EXTLIBCALL_ IPCFreeMemory(void*);
extern short _EXTLIBCALL_ IPCGetStatus(void*);
extern short _EXTLIBCALL_ IPCInit(void);
extern void  _EXTLIBCALL_ IPCDeInit(void);
extern void*  _EXTLIBCALL_ IPCConnectServer(const char*, IPCCallbackFunc);
extern void  _EXTLIBCALL_ IPCDisconnectServer(void *);
#ifdef WIN
extern long CDECL IPCCALLFUNCTION(void *,unsigned long,void *,
											   short,const char *, char, ...);
#else
extern long CDECL IPCCallFunction(void *,unsigned long,void *,
											   short,const char *, char, ...);
#endif
#ifdef __cplusplus
}
#endif

}

#endif
