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



namespace binfilter
{

#if defined(OV_DEBUG)

void ImpWriteLogFile(char*,char*);
void ImpAddHSZ( HSZ, String& );
void ImpWriteDdeStatus(char*, char* );
void ImpWriteDdeData(char*, DDESTRUCT*);

#ifdef LOGFILE
#define WRITELOG(aString) ImpWriteLogFile("\\ddeml.log",aString);
#else
#define WRITELOG(bla)
#endif
#ifdef STATUSFILE
#define WRITESTATUS(aContext) ImpWriteDdeStatus("\\ddeml.sts",aContext);
#else
#define WRITESTATUS(bla)
#endif
#ifdef DDEDATAFILE
#define WRITEDATA(data) ImpWriteDdeData("\\ddeml.dat",data);
#else
#define WRITEDATA(bla)
#endif

#else

#define WRITELOG(bla)
#define WRITESTATUS(bla)
#define WRITEDATA(bla)

#endif

APIRET MyDosAllocSharedMem(void** ppBaseAddress, char* pszName, unsigned long ulObjectSize,
	unsigned long ulFlags, char* pContextStr );

APIRET MyDosAllocMem(void** ppBaseAddress, unsigned long ulObjectSize,
	unsigned long ulFlags, char* pContextStr );

APIRET MyDosFreeMem( void* pBaseAddress, char* pContextStr );

}
