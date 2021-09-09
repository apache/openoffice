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



/*
 *   ToDo:
 *      - cleanup of process status things
 *      - cleanup of process spawning
 *      - cleanup of resource transfer
 */

#if defined(SOLARIS)
// The procfs may only be used without LFS in 32bits.
# ifdef _FILE_OFFSET_BITS
#   undef   _FILE_OFFSET_BITS
# endif
#endif


#ifdef FREEBSD
#include <machine/param.h>
#endif

#include "system.h"
#if defined(SOLARIS)
# include <sys/procfs.h>
#endif
#include <osl/diagnose.h>
#include <osl/mutex.h>

#ifndef _OSL_CONDITN_H_
#include <osl/conditn.h>
#endif
#include <osl/thread.h>
#include <osl/file.h>
#include <osl/signal.h>
#include <rtl/alloc.h>

#include <grp.h>

#include "procimpl.h"
#include "sockimpl.h"
#include "secimpl.h"


#define MAX_ARGS        255
#define MAX_ENVS        255

#if defined(MACOSX) || defined(IORESOURCE_TRANSFER_BSD)
#define CONTROLLEN (sizeof(struct cmsghdr) + sizeof(int))
#endif

/* implemented in file.c */
extern oslFileError FileURLToPath( char *, size_t, rtl_uString* );
extern oslFileHandle osl_createFileHandleFromFD( int fd );

/******************************************************************************
 *
 * Data Type Definition
 *
 ******************************************************************************/

typedef struct {
	int  m_hPipe;
	int  m_hConn;
	sal_Char m_Name[PATH_MAX + 1];
} Pipe;

typedef struct {
	const sal_Char*  m_pszArgs[MAX_ARGS + 1];
	oslProcessOption m_options;
	const sal_Char*  m_pszDir;
	sal_Char*        m_pszEnv[MAX_ENVS + 1];
	uid_t            m_uid;
	gid_t 		     m_gid;
	sal_Char* 		 m_name;
	oslCondition     m_started;
	oslProcessImpl*  m_pProcImpl;
	oslFileHandle	 *m_pInputWrite;
	oslFileHandle	 *m_pOutputRead;
	oslFileHandle	 *m_pErrorRead;
} ProcessData;

typedef struct _oslPipeImpl {
	int      m_Socket;
	sal_Char m_Name[PATH_MAX + 1];
} oslPipeImpl;


/******************************************************************************
 *
 * Function Declarations
 *
 *****************************************************************************/

oslProcessError SAL_CALL osl_psz_executeProcess(sal_Char *pszImageName,
												sal_Char *pszArguments[],
												oslProcessOption Options,
												oslSecurity Security,
												sal_Char *pszDirectory,
												sal_Char *pszEnvironments[],
												oslProcess *pProcess,
												oslFileHandle *pInputWrite,
												oslFileHandle *pOutputRead,
												oslFileHandle *pErrorRead );


oslProcessError SAL_CALL osl_searchPath_impl(
	const sal_Char* pszName,
	const sal_Char* pszPath,
	sal_Char Separator,
	sal_Char *pszBuffer,
	sal_uInt32 Max);


sal_Bool osl_getFullPath(const sal_Char* pszFilename, sal_Char* pszPath, sal_uInt32 MaxLen);

static oslProcessImpl* ChildList;
static oslMutex        ChildListMutex;

/******************************************************************************
 Deprecated
 Old and buggy implementation of osl_searchPath used only by
 osl_psz_executeProcess.
 A new implemenation is in file_path_helper.cxx
 *****************************************************************************/

oslProcessError SAL_CALL osl_searchPath_impl(const sal_Char* pszName, const sal_Char* pszPath,
			       sal_Char Separator, sal_Char *pszBuffer, sal_uInt32 Max)
{
	sal_Char path[PATH_MAX + 1];
	sal_Char *pchr;

	path[0] = '\0';

	OSL_ASSERT(pszName != NULL);

	if ( pszName == NULL )
	{
		return osl_Process_E_NotFound;
	}

	if (pszPath == NULL)
		pszPath = "PATH";

	if (Separator == '\0')
		Separator = ':';


	if ( (pchr = getenv(pszPath)) != NULL )
	{
		sal_Char *pstr;

		while (*pchr != '\0')
		{
			pstr = path;

			while ((*pchr != '\0') && (*pchr != Separator))
				*pstr++ = *pchr++;

			if ((pstr > path) && ((*(pstr - 1) != '/')))
				*pstr++ = '/';

			*pstr = '\0';

			strcat(path, pszName);

			if (access(path, 0) == 0)
			{
				char szRealPathBuf[PATH_MAX] = "";

				if( NULL == realpath(path, szRealPathBuf) || (strlen(szRealPathBuf) >= (sal_uInt32)Max))
					return osl_Process_E_Unknown;

				strcpy(pszBuffer, path);

				return osl_Process_E_None;
			}

			if (*pchr == Separator)
				pchr++;
		}
	}

	return osl_Process_E_NotFound;
}

/******************************************************************************
 *
 * New io resource transfer functions
 *
 *****************************************************************************/


/**********************************************
 sendFdPipe
 *********************************************/

static sal_Bool sendFdPipe(int PipeFD, int SocketFD)
{
	sal_Bool bRet = sal_False;

	struct iovec	iov[1];
	struct msghdr	msg;
	char			buf[2];	/* send_fd()/recv_fd() 2-byte protocol */
	int nSend;
	int RetCode=0;

#if defined(IOCHANNEL_TRANSFER_BSD)

	OSL_TRACE("IOCHANNEL_TRANSFER_BSD send");
/*	OSL_TRACE("sending fd %i\n",SocketFD); */

	iov[0].iov_base = buf;
	iov[0].iov_len  = sizeof(buf);
	msg.msg_iov     = iov;
	msg.msg_iovlen  = 1;
	msg.msg_name    = NULL;
	msg.msg_namelen = 0;

	msg.msg_accrights    = (caddr_t) &SocketFD;	/* addr of descriptor */
	msg.msg_accrightslen = sizeof(int);		/* pass 1 descriptor */
	buf[1] = 0;								/* zero status means OK */
	buf[0] = 0;								/* null byte flag to recv_fd() */

#else

	struct cmsghdr* cmptr = (struct cmsghdr*)malloc(CONTROLLEN);

	OSL_TRACE("!!!!!! IOCHANNEL_TRANSFER_BSD_RENO send");
/*	OSL_TRACE("sending fd %i\n",SocketFD); */

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_control = (caddr_t) cmptr;
	msg.msg_controllen = CONTROLLEN;

	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	cmptr->cmsg_len = CONTROLLEN;
	memcpy(CMSG_DATA(cmptr), &SocketFD, sizeof(int));

#endif

	if ( ( nSend = sendmsg(PipeFD, &msg, 0) ) > 0 )
	{
		bRet = sal_True;
		OSL_TRACE("sendFdPipe : send '%i' bytes\n",nSend);

	}
	else
	{
		OSL_TRACE("sendFdPipe : sending failed (%s)",strerror(errno));
	}

	nSend=read(PipeFD,&RetCode,sizeof(RetCode));

	if ( nSend > 0 && RetCode == 1 )
	{
		OSL_TRACE("sendFdPipe : resource was received\n");
	}
	else
	{
		OSL_TRACE("sendFdPipe : resource wasn't received\n");
	}

#if defined(IOCHANNEL_TRANSFER_BSD_RENO)
	free(cmptr);
#endif

	return bRet;
}

/**********************************************
 receiveFdPipe
 *********************************************/

static oslSocket receiveFdPipe(int PipeFD)
{
	oslSocket pSocket = 0;
	struct msghdr msghdr;
	struct iovec iov[1];
	char buffer[2];
	sal_Int32 nRead;
	int newfd=-1;
	int nRetCode=0;
/*	char *ptr; */

#if defined(IOCHANNEL_TRANSFER_BSD)

	OSL_TRACE("IOCHANNEL_TRANSFER_BSD receive\n");

	iov[0].iov_base = buffer;
	iov[0].iov_len = sizeof(buffer);
	msghdr.msg_name = NULL;
	msghdr.msg_namelen = 0;
	msghdr.msg_iov = iov;
	msghdr.msg_iovlen = 1;
	msghdr.msg_accrights = (caddr_t) &newfd; /* addr of descriptor */
	msghdr.msg_accrightslen = sizeof(int);	 /* receive 1 descriptor */

#else
	struct cmsghdr* cmptr = (struct cmsghdr*)malloc(CONTROLLEN);

	OSL_TRACE(" !!!! IOCHANNEL_TRANSFER_BSD_RENO receive");

	iov[0].iov_base = buffer;
	iov[0].iov_len = sizeof(buffer);
	msghdr.msg_name = NULL;
	msghdr.msg_namelen = 0;
	msghdr.msg_iov = iov;
	msghdr.msg_iovlen = 1;

	msghdr.msg_control = (caddr_t) cmptr;
	msghdr.msg_controllen = CONTROLLEN;

#endif


#if defined(IOCHANNEL_TRANSFER_BSD)

	if ( ( nRead = recvmsg(PipeFD, &msghdr, 0) ) > 0 )
	{
		OSL_TRACE("receiveFdPipe : received '%i' bytes\n",nRead);
	}
#else

	if ( ( ( nRead = recvmsg(PipeFD, &msghdr, 0) ) > 0 ) &&
		( msghdr.msg_controllen == CONTROLLEN ) )
	{
		OSL_TRACE("receiveFdPipe : received '%i' bytes\n",nRead);
		memcpy(&newfd, CMSG_DATA(cmptr), sizeof(int));
	}
#endif
	else
	{
		OSL_TRACE("receiveFdPipe : receiving failed (%s)",strerror(errno));
	}

	if ( newfd >= 0 )
	{
		pSocket = __osl_createSocketImpl(newfd);
		nRetCode=1;
		OSL_TRACE("received fd %i\n",newfd);
	}

	OSL_TRACE("receiveFdPipe : writing back %i",nRetCode);
	nRead=write(PipeFD,&nRetCode,sizeof(nRetCode));

#if defined(IOCHANNEL_TRANSFER_BSD_RENO)
	free(cmptr);
#endif

	return pSocket;
}

/**********************************************
 osl_sendResourcePipe
 *********************************************/

sal_Bool osl_sendResourcePipe(oslPipe pPipe, oslSocket pSocket)
{
	sal_Bool bRet = sal_False;

	if ( pSocket == 0 || pPipe == 0 )
	{
		return sal_False;
	}

	bRet = sendFdPipe(pPipe->m_Socket,pSocket->m_Socket);

	return bRet;
}

/**********************************************
 osl_receiveResourcePipe
 *********************************************/

oslSocket osl_receiveResourcePipe(oslPipe pPipe)
{
	oslSocket pSocket=0;

	if ( pPipe == 0 )
	{
		return 0;
	}

	pSocket = receiveFdPipe(pPipe->m_Socket);

	return (oslSocket) pSocket;
}



/******************************************************************************
 *
 * Functions for starting a process
 *
 *****************************************************************************/

static void ChildStatusProc(void *pData)
{
	pid_t pid = -1;
	int   status = 0;
	int   channel[2];
	ProcessData  data;
	ProcessData *pdata;
	int		stdOutput[2] = { -1, -1 }, stdInput[2] = { -1, -1 }, stdError[2] = { -1, -1 };

	pdata = (ProcessData *)pData;

	/* make a copy of our data, because forking will only copy
	   our local stack of the thread, so the process data will not be accessible
	   in our child process */
	memcpy(&data, pData, sizeof(data));

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, channel) == -1)
		status = errno;

	fcntl(channel[0], F_SETFD, FD_CLOEXEC);
	fcntl(channel[1], F_SETFD, FD_CLOEXEC);

	/* Create redirected IO pipes */
	if ( status == 0 && data.m_pInputWrite )
		if (pipe( stdInput ) == -1)
			status = errno;

	if ( status == 0 && data.m_pOutputRead )
		if (pipe( stdOutput ) == -1)
			status = errno;

	if ( status == 0 && data.m_pErrorRead )
		if (pipe( stdError ) == -1)
			status = errno;

	if ( (status == 0) && ((pid = fork()) == 0) )
	{
		/* Child */
		int chstatus = 0;
		sal_Int32 nWrote;

		if (channel[0] != -1) close(channel[0]);

		if ((data.m_uid != (uid_t)-1) && ((data.m_uid != getuid()) || (data.m_gid != getgid())))
		{
			OSL_ASSERT(geteuid() == 0);		/* must be root */

			if (! INIT_GROUPS(data.m_name, data.m_gid) || (setuid(data.m_uid) != 0))
				OSL_TRACE("Failed to change uid and guid, errno=%d (%s)\n", errno, strerror(errno));
#if defined(LINUX) || defined (FREEBSD)
			unsetenv("HOME");
#else
			putenv("HOME=");
#endif
		}

		if (data.m_pszDir)
			chstatus = chdir(data.m_pszDir);

		if (chstatus == 0 && ((data.m_uid == (uid_t)-1) || ((data.m_uid == getuid()) && (data.m_gid == getgid()))))
		{
			int i;
			for (i = 0; data.m_pszEnv[i] != NULL; i++)
			{
				if (strchr(data.m_pszEnv[i], '=') == NULL)
				{
					unsetenv(data.m_pszEnv[i]); /*TODO: check error return*/
				}
				else
				{
					putenv(data.m_pszEnv[i]); /*TODO: check error return*/
				}
			}

			OSL_TRACE("ChildStatusProc : starting '%s'",data.m_pszArgs[0]);

			/* Connect std IO to pipe ends */

			/* Write end of stdInput not used in child process */
			if (stdInput[1] != -1) close( stdInput[1] );

			/* Read end of stdOutput not used in child process */
			if (stdOutput[0] != -1) close( stdOutput[0] );

			/* Read end of stdError not used in child process */
			if (stdError[0] != -1) close( stdError[0] );

			/* Redirect pipe ends to std IO */

			if ( stdInput[0] != STDIN_FILENO )
			{
				dup2( stdInput[0], STDIN_FILENO );
				if (stdInput[0] != -1) close( stdInput[0] );
			}

			if ( stdOutput[1] != STDOUT_FILENO )
			{
				dup2( stdOutput[1], STDOUT_FILENO );
				if (stdOutput[1] != -1) close( stdOutput[1] );
			}

			if ( stdError[1] != STDERR_FILENO )
			{
				dup2( stdError[1], STDERR_FILENO );
				if (stdError[1] != -1) close( stdError[1] );
			}

			pid=execv(data.m_pszArgs[0], (sal_Char **)data.m_pszArgs);

		}

		OSL_TRACE("Failed to exec, errno=%d (%s)\n", errno, strerror(errno));

		OSL_TRACE("ChildStatusProc : starting '%s' failed",data.m_pszArgs[0]);

		/* if we reach here, something went wrong */
		nWrote = write(channel[1], &errno, sizeof(errno));
		if (nWrote != sizeof(errno))
			OSL_TRACE("sendFdPipe : sending failed (%s)",strerror(errno));

		if (channel[1] != -1) close(channel[1]);

		_exit(255);
	}
	else
	{ /* Parent */
		int i = -1;
		if (channel[1] != -1) close(channel[1]);

		/* Close unused pipe ends */
		if (stdInput[0] != -1) close( stdInput[0] );
		if (stdOutput[1] != -1) close( stdOutput[1] );
		if (stdError[1] != -1) close( stdError[1] );

		if (pid > 0)
		{
			while (((i = read(channel[0], &status, sizeof(status))) < 0))
			{
				if (errno != EINTR)
					break;
			}
		}

		if (channel[0] != -1) close(channel[0]);

		if ((pid > 0) && (i == 0))
		{
			pid_t	child_pid;
			osl_acquireMutex(ChildListMutex);

			pdata->m_pProcImpl->m_pid = pid;
			pdata->m_pProcImpl->m_pnext = ChildList;
			ChildList = pdata->m_pProcImpl;

			/* Store used pipe ends in data structure */

			if ( pdata->m_pInputWrite )
				*(pdata->m_pInputWrite) = osl_createFileHandleFromFD( stdInput[1] );

			if ( pdata->m_pOutputRead )
				*(pdata->m_pOutputRead) = osl_createFileHandleFromFD( stdOutput[0] );

			if ( pdata->m_pErrorRead )
				*(pdata->m_pErrorRead) = osl_createFileHandleFromFD( stdError[0] );

			osl_releaseMutex(ChildListMutex);

			osl_setCondition(pdata->m_started);

			do
			{
				child_pid = waitpid(pid, &status, 0);
			} while ( 0 > child_pid && EINTR == errno );

			if ( child_pid < 0)
			{
				OSL_TRACE("Failed to wait for child process, errno=%d (%s)\n", errno, strerror(errno));

				/*
				We got an other error than EINTR. Anyway we have to wake up the
				waiting thread under any circumstances */

				child_pid = pid;
			}


			if ( child_pid > 0 )
			{
				oslProcessImpl* pChild;

				osl_acquireMutex(ChildListMutex);

				pChild = ChildList;

				/* check if it is one of our child processes */
				while (pChild != NULL)
				{
					if (pChild->m_pid == child_pid)
					{
						if (WIFEXITED(status))
							pChild->m_status = WEXITSTATUS(status);
						else
							pChild->m_status = -1;

						osl_setCondition(pChild->m_terminated);
					}

					pChild = pChild->m_pnext;
				}

				osl_releaseMutex(ChildListMutex);
			}
		}
		else
		{
			OSL_TRACE("ChildStatusProc : starting '%s' failed",data.m_pszArgs[0]);
			OSL_TRACE("Failed to launch child process, child reports errno=%d (%s)\n", status, strerror(status));

			/* Close pipe ends */
			if ( pdata->m_pInputWrite )
				*pdata->m_pInputWrite = NULL;

			if ( pdata->m_pOutputRead )
				*pdata->m_pOutputRead = NULL;

			if ( pdata->m_pErrorRead )
				*pdata->m_pErrorRead = NULL;

			if (stdInput[1] != -1) close( stdInput[1] );
			if (stdOutput[0] != -1) close( stdOutput[0] );
			if (stdError[0] != -1) close( stdError[0] );

			//if pid > 0 then a process was created, even if it later failed
			//e.g. bash searching for a command to execute, and we still
			//need to clean it up to avoid "defunct" processes
			if (pid > 0)
			{
				pid_t child_pid;
				do
				{
					child_pid = waitpid(pid, &status, 0);
				} while ( 0 > child_pid && EINTR == errno );
			}

			/* notify (and unblock) parent thread */
			osl_setCondition(pdata->m_started);
		}
	}
}

/**********************************************
 osl_executeProcess_WithRedirectedIO
 *********************************************/

oslProcessError SAL_CALL osl_executeProcess_WithRedirectedIO(
											rtl_uString *ustrImageName,
											rtl_uString *ustrArguments[],
											sal_uInt32   nArguments,
											oslProcessOption Options,
											oslSecurity Security,
											rtl_uString *ustrWorkDir,
											rtl_uString *ustrEnvironment[],
											sal_uInt32   nEnvironmentVars,
											oslProcess *pProcess,
											oslFileHandle	*pInputWrite,
											oslFileHandle	*pOutputRead,
											oslFileHandle	*pErrorRead
											)
{

	oslProcessError Error;
	sal_Char* pszWorkDir=NULL;
	sal_Char** pArguments=NULL;
	sal_Char** pEnvironment=NULL;
	unsigned int idx;

	char szImagePath[PATH_MAX] = "";
	char szWorkDir[PATH_MAX] = "";

	if ( ustrImageName && ustrImageName->length )
	{
		FileURLToPath( szImagePath, PATH_MAX, ustrImageName );
	}

	if ( ustrWorkDir != NULL && ustrWorkDir->length )
	{
		FileURLToPath( szWorkDir, PATH_MAX, ustrWorkDir );
		pszWorkDir = szWorkDir;
	}

	if ( pArguments == NULL && nArguments > 0 )
	{
		pArguments = (sal_Char**) malloc( ( nArguments + 2 ) * sizeof(sal_Char*) );
	}


	for ( idx = 0 ; idx < nArguments ; ++idx )
	{
		rtl_String* strArg =NULL;


		rtl_uString2String( &strArg,
							rtl_uString_getStr(ustrArguments[idx]),
							rtl_uString_getLength(ustrArguments[idx]),
							osl_getThreadTextEncoding(),
							OUSTRING_TO_OSTRING_CVTFLAGS );

		pArguments[idx]=strdup(rtl_string_getStr(strArg));
		rtl_string_release(strArg);
		pArguments[idx+1]=NULL;
	}

	for ( idx = 0 ; idx < nEnvironmentVars ; ++idx )
	{
		rtl_String* strEnv=NULL;

		if ( pEnvironment == NULL )
		{
			pEnvironment = (sal_Char**) malloc( ( nEnvironmentVars + 2 ) * sizeof(sal_Char*) );
		}

		rtl_uString2String( &strEnv,
							rtl_uString_getStr(ustrEnvironment[idx]),
							rtl_uString_getLength(ustrEnvironment[idx]),
							osl_getThreadTextEncoding(),
							OUSTRING_TO_OSTRING_CVTFLAGS );

		pEnvironment[idx]=strdup(rtl_string_getStr(strEnv));
		rtl_string_release(strEnv);
		pEnvironment[idx+1]=NULL;
	}


	Error = osl_psz_executeProcess(szImagePath,
								   pArguments,
								   Options,
								   Security,
								   pszWorkDir,
								   pEnvironment,
								   pProcess,
								   pInputWrite,
								   pOutputRead,
								   pErrorRead
								   );

	if ( pArguments != NULL )
	{
		for ( idx = 0 ; idx < nArguments ; ++idx )
		{
			if ( pArguments[idx] != NULL )
			{
				free(pArguments[idx]);
			}
		}
		free(pArguments);
	}

	if ( pEnvironment != NULL )
	{
		for ( idx = 0 ; idx < nEnvironmentVars ; ++idx )
		{
			if ( pEnvironment[idx] != NULL )
			{
				free(pEnvironment[idx]);
			}
		}
		free(pEnvironment);
	}

	return Error;
}

/**********************************************
 osl_executeProcess
 *********************************************/

oslProcessError SAL_CALL osl_executeProcess(
											rtl_uString *ustrImageName,
											rtl_uString *ustrArguments[],
											sal_uInt32   nArguments,
											oslProcessOption Options,
											oslSecurity Security,
											rtl_uString *ustrWorkDir,
											rtl_uString *ustrEnvironment[],
											sal_uInt32   nEnvironmentVars,
											oslProcess *pProcess
											)
{
	return osl_executeProcess_WithRedirectedIO(
		ustrImageName,
		ustrArguments,
		nArguments,
		Options,
		Security,
		ustrWorkDir,
		ustrEnvironment,
		nEnvironmentVars,
		pProcess,
		NULL,
		NULL,
		NULL
		);
}

/**********************************************
 osl_psz_executeProcess
 *********************************************/

oslProcessError SAL_CALL osl_psz_executeProcess(sal_Char *pszImageName,
												sal_Char *pszArguments[],
												oslProcessOption Options,
												oslSecurity Security,
												sal_Char *pszDirectory,
												sal_Char *pszEnvironments[],
												oslProcess *pProcess,
												oslFileHandle	*pInputWrite,
												oslFileHandle	*pOutputRead,
												oslFileHandle	*pErrorRead
												)
{
	int     i;
	sal_Char    path[PATH_MAX + 1];
	ProcessData Data;
	oslThread hThread;

	path[0] = '\0';

	memset(&Data,0,sizeof(ProcessData));
	Data.m_pInputWrite = pInputWrite;
	Data.m_pOutputRead = pOutputRead;
	Data.m_pErrorRead = pErrorRead;

	if (pszImageName == NULL)
		pszImageName = pszArguments[0];

	OSL_ASSERT(pszImageName != NULL);

	if ( pszImageName == NULL )
	{
		return osl_Process_E_NotFound;
	}

	if ((Options & osl_Process_SEARCHPATH) &&
		(osl_searchPath_impl(pszImageName, NULL, '\0', path, sizeof(path)) == osl_Process_E_None))
		pszImageName = path;

	Data.m_pszArgs[0] = strdup(pszImageName);
	Data.m_pszArgs[1] = NULL;

	if ( pszArguments != 0 )
	{
		for (i = 0; ((i + 2) < MAX_ARGS) && (pszArguments[i] != NULL); i++)
			Data.m_pszArgs[i+1] = strdup(pszArguments[i]);
		Data.m_pszArgs[i+2] = NULL;
	}

	Data.m_options = Options;
	Data.m_pszDir  = (pszDirectory != NULL) ? strdup(pszDirectory) : NULL;

	if (pszEnvironments != NULL)
	{
		for (i = 0; ((i + 1) < MAX_ENVS) && (pszEnvironments[i] != NULL); i++)
			Data.m_pszEnv[i] = strdup(pszEnvironments[i]);
	 	Data.m_pszEnv[i+1] = NULL;
	}
	else
	 	Data.m_pszEnv[0] = NULL;

	if (Security != NULL)
	{
		Data.m_uid  = ((oslSecurityImpl*)Security)->m_pPasswd.pw_uid;
		Data.m_gid  = ((oslSecurityImpl*)Security)->m_pPasswd.pw_gid;
		Data.m_name = ((oslSecurityImpl*)Security)->m_pPasswd.pw_name;
	}
	else
		Data.m_uid = (uid_t)-1;

	Data.m_pProcImpl = (oslProcessImpl*) malloc(sizeof(oslProcessImpl));
	Data.m_pProcImpl->m_pid = 0;
	Data.m_pProcImpl->m_terminated = osl_createCondition();
	Data.m_pProcImpl->m_pnext = NULL;

	if (ChildListMutex == NULL)
		ChildListMutex = osl_createMutex();

	Data.m_started = osl_createCondition();

	hThread = osl_createThread(ChildStatusProc, &Data);

	osl_waitCondition(Data.m_started, NULL);
	osl_destroyCondition(Data.m_started);

	for (i = 0; Data.m_pszArgs[i] != NULL; i++)
		  free((void *)Data.m_pszArgs[i]);

	for (i = 0; Data.m_pszEnv[i] != NULL; i++)
		  free((void *)Data.m_pszEnv[i]);

	if ( Data.m_pszDir != NULL )
	{
		free((void *)Data.m_pszDir);
	}

	osl_destroyThread(hThread);

	if (Data.m_pProcImpl->m_pid != 0)
	{
		*pProcess = Data.m_pProcImpl;

	 	if (Options & osl_Process_WAIT)
			osl_joinProcess(*pProcess);

		return osl_Process_E_None;
	}

	osl_destroyCondition(Data.m_pProcImpl->m_terminated);
	free(Data.m_pProcImpl);

	return osl_Process_E_Unknown;
}


/******************************************************************************
 *
 * Functions for processes
 *
 *****************************************************************************/


/**********************************************
 osl_terminateProcess
 *********************************************/

oslProcessError SAL_CALL osl_terminateProcess(oslProcess Process)
{
	if (Process == NULL)
		return osl_Process_E_Unknown;

	if (kill(((oslProcessImpl*)Process)->m_pid, SIGKILL) != 0)
	{
		switch (errno)
		{
			case EPERM:
				return osl_Process_E_NoPermission;

			case ESRCH:
				return osl_Process_E_NotFound;

			default:
				return osl_Process_E_Unknown;
		}
	}

	return osl_Process_E_None;
}

/**********************************************
 osl_getProcess
 *********************************************/

oslProcess SAL_CALL osl_getProcess(oslProcessIdentifier Ident)
{
	oslProcessImpl *pProcImpl;

	if (kill(Ident, 0) != -1)
	{
		oslProcessImpl* pChild;

		if (ChildListMutex == NULL)
			ChildListMutex = osl_createMutex();

		osl_acquireMutex(ChildListMutex);

		pChild = ChildList;

		/* check if it is one of our child processes */
		while (pChild != NULL)
		{
			if (Ident == (sal_uInt32) pChild->m_pid)
				break;

			pChild = pChild->m_pnext;
		}

		pProcImpl = (oslProcessImpl*) malloc(sizeof(oslProcessImpl));
		pProcImpl->m_pid        = Ident;
		pProcImpl->m_terminated = osl_createCondition();

		if (pChild != NULL)
		{
			/* process is a child so insert into list */
			pProcImpl->m_pnext = pChild->m_pnext;
			pChild->m_pnext = pProcImpl;

			pProcImpl->m_status = pChild->m_status;

			if (osl_checkCondition(pChild->m_terminated))
				osl_setCondition(pProcImpl->m_terminated);
		}
		else
			pProcImpl->m_pnext = NULL;

		osl_releaseMutex(ChildListMutex);
	}
	else
		pProcImpl = NULL;

	return (pProcImpl);
}

/**********************************************
 osl_freeProcessHandle
 *********************************************/

void SAL_CALL osl_freeProcessHandle(oslProcess Process)
{
	if (Process != NULL)
	{
		oslProcessImpl *pChild, *pPrev = NULL;

		OSL_ASSERT(ChildListMutex != NULL);

		if ( ChildListMutex == 0 )
		{
			return;
		}

		osl_acquireMutex(ChildListMutex);

		pChild = ChildList;

		/* remove process from child list */
		while (pChild != NULL)
		{
			if (pChild == (oslProcessImpl*)Process)
			{
				if (pPrev != NULL)
					pPrev->m_pnext = pChild->m_pnext;
				else
					ChildList = pChild->m_pnext;

				break;
			}

			pPrev  = pChild;
			pChild = pChild->m_pnext;
		}

		osl_releaseMutex(ChildListMutex);

		osl_destroyCondition(((oslProcessImpl*)Process)->m_terminated);

		free(Process);
	}
}

#if defined(LINUX)
struct osl_procStat
{
	/* from 'stat' */
	pid_t pid;                /* pid */
	char command[16];         /* 'argv[0]' */ /* mfe: it all right char comm[16] in kernel! */
	char state;               /* state (running, stopped, ...) */
	pid_t ppid;               /* parent pid */
	pid_t pgrp;               /* parent group */
	int session;              /* session ID */
	int tty;                  /* no of tty */
	pid_t tpgid;              /* group of process owning the tty */
	unsigned long flags;      /* flags dunno */
	unsigned long minflt;     /* minor page faults */
	unsigned long cminflt;    /* minor page faults with children */
	unsigned long majflt;     /* major page faults */
	unsigned long cmajflt;    /* major page faults with children */
	unsigned long utime;      /* no of jiffies in user mode */
	unsigned long stime;      /* no of jiffies in kernel mode */
	unsigned long cutime;     /* no of jiffies in user mode with children */
	unsigned long cstime;     /* no of jiffies in kernel mode with children */
	unsigned long priority;   /* nice value + 15 (kernel scheduling prio)*/
	long nice;                /* nice value */
	long timeout;             /* no of jiffies of next process timeout */
	long itrealvalue;         /* no jiffies before next SIGALRM */
	unsigned long starttime;  /* process started this no of jiffies after boot */
	unsigned long vsize;      /* virtual memory size (in bytes) */
	long rss;                 /* resident set size (in pages) */
	unsigned long rss_rlim;   /* rss limit (in bytes) */
	unsigned long startcode;  /* address above program text can run */
	unsigned long endcode;    /* address below program text can run */
	unsigned long startstack; /* address of start of stack */
	unsigned long kstkesp;    /* current value of 'esp' (stack pointer) */
	unsigned long kstkeip;    /* current value of 'eip' (instruction pointer) */
	/* mfe: Linux > 2.1.7x have more signals (88) */
/*#ifdef LINUX */
	char signal[24];          /* pending signals */
	char blocked[24];         /* blocked signals */
	char sigignore[24];       /* ignored signals */
	char sigcatch[24];        /* catched signals */
/*#else*/
/*	long long signal;*/
/*	long long blocked;*/
/*	long long sigignore;*/
/*	long long sigcatch;*/
/*#endif */
	unsigned long wchan;      /* 'channel' the process is waiting in */
	unsigned long nswap;      /* ? */
	unsigned long cnswap;     /* ? */

	/* from 'status' */
	int ruid;                 /* real uid */
	int euid;                 /* effective uid */
	int suid;                 /* saved uid */
	int fuid;                 /* file access uid */
	int rgid;                 /* real gid */
	int egid;                 /* effective gid */
	int sgid;                 /* saved gid */
	int fgid;                 /* file access gid */
	unsigned long vm_size;    /* like vsize but on kb */
	unsigned long vm_lock;    /* locked pages in kb */
	unsigned long vm_rss;     /* like rss but in kb */
	unsigned long vm_data;    /* data size */
	unsigned long vm_stack;   /* stack size */
	unsigned long vm_exe;     /* executable size */
	unsigned long vm_lib;     /* library size */
};

/**********************************************
 osl_getProcStat
 *********************************************/

sal_Bool osl_getProcStat(pid_t pid, struct osl_procStat* procstat)
{
	int fd = 0;
	sal_Bool bRet = sal_False;
	char name[PATH_MAX + 1];
	snprintf(name, sizeof(name), "/proc/%u/stat", pid);

	if ((fd = open(name,O_RDONLY)) >=0 )
	{
		char* tmp=NULL;
		char prstatbuf[512];
		memset(prstatbuf,0,512);
		bRet = read(fd,prstatbuf,511) == 511;

		close(fd);
		/*printf("%s\n\n",prstatbuf);*/

		if (!bRet)
			return sal_False;

		tmp = strrchr(prstatbuf, ')');
		*tmp = '\0';
		memset(procstat->command, 0, sizeof(procstat->command));

		sscanf(prstatbuf, "%d (%15c", &procstat->pid, procstat->command);
		sscanf(tmp + 2,
			   "%c"
			   "%i %i %i %i %i"
			   "%lu %lu %lu %lu %lu"
			   "%lu %lu %lu %lu"
			   "%lu %li %li %li"
			   "%lu %lu %li %lu"
			   "%lu %lu %lu %lu %lu"
			   "%s %s %s %s"
			   "%lu %lu %lu",
			   &procstat->state,
			   &procstat->ppid,      &procstat->pgrp,    &procstat->session,    &procstat->tty,         &procstat->tpgid,
			   &procstat->flags,     &procstat->minflt,  &procstat->cminflt,    &procstat->majflt,      &procstat->cmajflt,
			   &procstat->utime,     &procstat->stime,   &procstat->cutime,     &procstat->cstime,
			   &procstat->priority,  &procstat->nice,    &procstat->timeout,    &procstat->itrealvalue,
			   &procstat->starttime, &procstat->vsize,   &procstat->rss,        &procstat->rss_rlim,
			   &procstat->startcode, &procstat->endcode, &procstat->startstack, &procstat->kstkesp,     &procstat->kstkeip,
			   procstat->signal,     procstat->blocked,  procstat->sigignore,   procstat->sigcatch,
			   &procstat->wchan,     &procstat->nswap,   &procstat->cnswap
			);
	}
	return bRet;
}

/**********************************************
 osl_getProcStatus
 *********************************************/

sal_Bool osl_getProcStatus(pid_t pid, struct osl_procStat* procstat)
{
	int fd = 0;
	char name[PATH_MAX + 1];
	snprintf(name, sizeof(name), "/proc/%u/status", pid);

	sal_Bool bRet = sal_False;

	if ((fd = open(name,O_RDONLY)) >=0 )
	{
		char* tmp=NULL;
		char prstatusbuf[512];
		memset(prstatusbuf,0,512);
		bRet = read(fd,prstatusbuf,511) == 511;

		close(fd);

		/*		printf("\n\n%s\n\n",prstatusbuf);*/

		if (!bRet)
			return sal_False;

		tmp = strstr(prstatusbuf,"Uid:");
		if(tmp)
		{
			sscanf(tmp,"Uid:\t%d\t%d\t%d\t%d",
				   &procstat->ruid, &procstat->euid, &procstat->suid, &procstat->fuid
				);
		}


		tmp = strstr(prstatusbuf,"Gid:");
		if(tmp)
		{
			sscanf(tmp,"Gid:\t%d\t%d\t%d\t%d",
				   &procstat->rgid, &procstat->egid, &procstat->sgid, &procstat->fgid
				);
		}

		tmp = strstr(prstatusbuf,"VmSize:");
		if(tmp)
		{
			sscanf(tmp,
				   "VmSize: %lu kB\n"
				   "VmLck: %lu kB\n"
				   "VmRSS: %lu kB\n"
				   "VmData: %lu kB\n"
				   "VmStk: %lu kB\n"
				   "VmExe: %lu kB\n"
				   "VmLib: %lu kB\n",
				   &procstat->vm_size, &procstat->vm_lock, &procstat->vm_rss, &procstat->vm_data,
				   &procstat->vm_stack, &procstat->vm_exe, &procstat->vm_lib
				);
		}

		tmp = strstr(prstatusbuf,"SigPnd:");
		if(tmp)
		{
			sscanf(tmp, "SigPnd: %s SigBlk: %s SigIgn: %s %*s %s",
				   procstat->signal, procstat->blocked, procstat->sigignore, procstat->sigcatch
				);
		}
	}
	return bRet;
}

#endif

/**********************************************
 osl_getProcessInfo
 *********************************************/

oslProcessError SAL_CALL osl_getProcessInfo(oslProcess Process, oslProcessData Fields, oslProcessInfo* pInfo)
{
	pid_t   pid;

	if (Process == NULL)
		pid = getpid();
	else
		pid = ((oslProcessImpl*)Process)->m_pid;

	if (! pInfo || (pInfo->Size != sizeof(oslProcessInfo)))
		return osl_Process_E_Unknown;

	pInfo->Fields = 0;

	if (Fields & osl_Process_IDENTIFIER)
	{
		pInfo->Ident  = pid;
		pInfo->Fields |= osl_Process_IDENTIFIER;
	}

	if (Fields & osl_Process_EXITCODE)
	{
		if ((Process != NULL) &&
			osl_checkCondition(((oslProcessImpl*)Process)->m_terminated))
		{
			pInfo->Code = ((oslProcessImpl*)Process)->m_status;
			pInfo->Fields |= osl_Process_EXITCODE;
		}
	}

	if (Fields & (osl_Process_HEAPUSAGE | osl_Process_CPUTIMES))
	{

#if defined(SOLARIS)

		int  fd;
		sal_Char name[PATH_MAX + 1];

		snprintf(name, sizeof(name), "/proc/%u", pid);

		if ((fd = open(name, O_RDONLY)) >= 0)
		{
			prstatus_t prstatus;

			if (ioctl(fd, PIOCSTATUS, &prstatus) >= 0)
			{
				if (Fields & osl_Process_CPUTIMES)
				{
					pInfo->UserTime.Seconds   = prstatus.pr_utime.tv_sec;
					pInfo->UserTime.Nanosec   = prstatus.pr_utime.tv_nsec;
					pInfo->SystemTime.Seconds = prstatus.pr_stime.tv_sec;
					pInfo->SystemTime.Nanosec = prstatus.pr_stime.tv_nsec;

					pInfo->Fields |= osl_Process_CPUTIMES;
				}

				if (Fields & osl_Process_HEAPUSAGE)
				{
					pInfo->HeapUsage = prstatus.pr_brksize;

					pInfo->Fields |= osl_Process_HEAPUSAGE;
				}

				close(fd);

				return (pInfo->Fields == Fields) ? osl_Process_E_None : osl_Process_E_Unknown;
			}
			else
				close(fd);
		}

#elif defined(HPUX)

		struct pst_status prstatus;

		if (pstat_getproc(&prstatus, sizeof(prstatus), (size_t)0, pid) == 1)
		{
			if (Fields & osl_Process_CPUTIMES)
			{
				pInfo->UserTime.Seconds   = prstatus.pst_utime;
				pInfo->UserTime.Nanosec   = 500000L;
				pInfo->SystemTime.Seconds = prstatus.pst_stime;
				pInfo->SystemTime.Nanosec = 500000L;

				pInfo->Fields |= osl_Process_CPUTIMES;
			}

			if (Fields & osl_Process_HEAPUSAGE)
			{
				pInfo->HeapUsage = prstatus.pst_vdsize*PAGESIZE;

				pInfo->Fields |= osl_Process_HEAPUSAGE;
			}

			return (pInfo->Fields == Fields) ? osl_Process_E_None : osl_Process_E_Unknown;
		}

#elif defined(LINUX)

		if ( (Fields & osl_Process_CPUTIMES) || (Fields & osl_Process_HEAPUSAGE) )
		{
			struct osl_procStat procstat;
			memset(&procstat,0,sizeof(procstat));

			if ( (Fields & osl_Process_CPUTIMES) && osl_getProcStat(pid, &procstat) )
			{
				/*
				 *  mfe:
				 *  We calculate only time of the process proper.
				 *  Threads are processes, we do not consider their time here!
				 *  (For this, cutime and cstime should be used, it seems not
				 *   to work in 2.0.36)
				 */

				long clktck;
				unsigned long hz;
				unsigned long userseconds;
				unsigned long systemseconds;

				clktck = sysconf(_SC_CLK_TCK);
				if (clktck < 0) {
					return osl_Process_E_Unknown;
				}
				hz = (unsigned long) clktck;

				userseconds = procstat.utime/hz;
				systemseconds = procstat.stime/hz;

				pInfo->UserTime.Seconds   = userseconds;
				pInfo->UserTime.Nanosec   = procstat.utime - (userseconds * hz);
				pInfo->SystemTime.Seconds = systemseconds;
				pInfo->SystemTime.Nanosec = procstat.stime - (systemseconds * hz);

				pInfo->Fields |= osl_Process_CPUTIMES;
			}

			if ( (Fields & osl_Process_HEAPUSAGE) && osl_getProcStatus(pid, &procstat) )
			{
				/*
				 *  mfe:
				 *  vm_data (found in status) shows the size of the data segment
				 *  it a rough approximation of the core heap size
				 */
				pInfo->HeapUsage = procstat.vm_data*1024;

				pInfo->Fields |= osl_Process_HEAPUSAGE;
			}
		}

		return (pInfo->Fields == Fields) ? osl_Process_E_None : osl_Process_E_Unknown;
#endif

	}

	return (pInfo->Fields == Fields) ? osl_Process_E_None : osl_Process_E_Unknown;
}


/***********************************************
 helper function for osl_joinProcessWithTimeout
 **********************************************/

static int is_timeout(const struct timeval* tend)
{
	struct timeval tcurrent;
	gettimeofday(&tcurrent, NULL);
	return (tcurrent.tv_sec >= tend->tv_sec);
}

/**********************************************
 kill(pid, 0) is useful for checking if a
 process is still alive, but remember that
 kill even returns 0 if the process is already
 a zombie.
 *********************************************/

static int is_process_dead(pid_t pid)
{
	return ((-1 == kill(pid, 0)) && (ESRCH == errno));
}

/**********************************************
 osl_joinProcessWithTimeout
 *********************************************/

oslProcessError SAL_CALL osl_joinProcessWithTimeout(oslProcess Process, const TimeValue* pTimeout)
{
	oslProcessImpl* pChild    = ChildList;
	oslProcessError osl_error = osl_Process_E_None;

	OSL_PRECOND(Process, "osl_joinProcess: Invalid parameter");
	OSL_ASSERT(ChildListMutex);

	if (NULL == Process || 0 == ChildListMutex)
		return osl_Process_E_Unknown;

	osl_acquireMutex(ChildListMutex);

	/* check if process is a child of ours */
	while (pChild != NULL)
	{
		if (pChild == (oslProcessImpl*)Process)
			break;

		pChild = pChild->m_pnext;
	}

	osl_releaseMutex(ChildListMutex);

	if (pChild != NULL)
	{
		oslConditionResult cond_res = osl_waitCondition(pChild->m_terminated, pTimeout);

		if (osl_cond_result_timeout == cond_res)
		    osl_error = osl_Process_E_TimedOut;
		else if (osl_cond_result_ok != cond_res)
		    osl_error = osl_Process_E_Unknown;
	}
	else /* alien process; StatusThread will not be able
			to set the condition terminated */
	{
		pid_t pid = ((oslProcessImpl*)Process)->m_pid;

		if (pTimeout)
		{
			int timeout = 0;
			struct timeval tend;

			gettimeofday(&tend, NULL);

			tend.tv_sec += pTimeout->Seconds;

			while (!is_process_dead(pid) && ((timeout = is_timeout(&tend)) == 0))
				sleep(1);

			if (timeout)
				osl_error = osl_Process_E_TimedOut;
		}
		else /* infinite */
		{
			while (!is_process_dead(pid))
				sleep(1);
		}
	}
	return osl_error;
}

/**********************************************
 osl_joinProcess
 *********************************************/

oslProcessError SAL_CALL osl_joinProcess(oslProcess Process)
{
	return osl_joinProcessWithTimeout(Process, NULL);
}
