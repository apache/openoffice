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



#ifndef _VOS_PROCESS_HXX_
#define _VOS_PROCESS_HXX_

#   include <rtl/ustring.hxx>
#   include <vos/mutex.hxx>
#   include <vos/security.hxx>
#   include <vos/object.hxx>
#   include <vos/socket.hxx>
#   include <osl/process.h>
#   include <vos/vosdllapi.h>

namespace vos
{

class OProcess;

/** helper class to fill a vector of command line arguments
 */
class VOS_DLLPUBLIC OArgumentList
{
    sal_uInt32 n_Args;
    rtl_uString** m_aVec;

public:

    OArgumentList();
    OArgumentList( sal_uInt32 nArgs, const ::rtl::OUString* aArgument1, ... );
    // switched argument list to avoid ambiguity with previous constructor.
    OArgumentList( const ::rtl::OUString aArgumentList[], sal_uInt32 nArgs );

    OArgumentList( const OArgumentList& rOther);

    OArgumentList& operator=( const OArgumentList& rOther);

    virtual ~OArgumentList();

    friend class OProcess;
};

/** helper class to fill a vector of environment settings
 */
class VOS_DLLPUBLIC OEnvironment
{
    sal_Int32 n_Vars;
    rtl_uString** m_aVec;

public:

    OEnvironment();
    OEnvironment( sal_Int32 nVars, const ::rtl::OUString* aVariable1, ... );
    // switched argument list to avoid ambiguity with previous constructor.
    OEnvironment( const ::rtl::OUString aVariableList[], sal_Int32 nVars );

    OEnvironment( const OEnvironment& rOther );

    OEnvironment& operator=( const OEnvironment& rOther );

    virtual ~OEnvironment();

    friend class OProcess;
};


/** startup child processes.
    @see OStartupInfo
    Used for starting an monitoring child processes with special features:
    <ul><li>setting environments,
    <li>setting working directories,
    <li>setting user rights and security,
    <li>providing ioresources like file descriptors and sockets.</ul>
*/
class VOS_DLLPUBLIC OProcess : public OObject
{
    VOS_DECLARE_CLASSINFO(VOS_NAMESPACE(OProcess, vos));

public:

    /** Options for execution mode:
    */
    enum TProcessOption
    {
        TOption_Wait       = osl_Process_WAIT,       // wait for completion
        TOption_SearchPath = osl_Process_SEARCHPATH, // search path for executable
        TOption_Detached   = osl_Process_DETACHED,   // run detached
        TOption_Normal     = osl_Process_NORMAL,     // run in normal window
        TOption_Hidden     = osl_Process_HIDDEN,     // run hidden
        TOption_Minimized  = osl_Process_MINIMIZED,  // run in minimized window
        TOption_Maximized  = osl_Process_MAXIMIZED,  // run in maximized window
        TOption_FullScreen = osl_Process_FULLSCREEN  // run in fullscreen window
    };

    /** Errorcodes:
    */
    enum TProcessError {
        E_None         = osl_Process_E_None,            /* no error */
        E_NotFound     = osl_Process_E_NotFound,        /* image not found */
        E_TimedOut     = osl_Process_E_TimedOut,        /* timout occurred */
        E_NoPermission = osl_Process_E_NoPermission,    /* permission denied */
        E_Unknown      = osl_Process_E_Unknown,         /* unknown error */
        E_InvalidError = osl_Process_E_InvalidError     /* unmapped error */
    };

    enum TDescriptorFlags
    {
        TFlags_None = osl_Process_DFNONE,
        TFlags_Wait = osl_Process_DFWAIT
    };

    enum TProcessData
    {
        TData_Identifier = osl_Process_IDENTIFIER,
        TData_ExitCode   = osl_Process_EXITCODE,
        TData_CpuTimes   = osl_Process_CPUTIMES,
        TData_HeapUsage  = osl_Process_HEAPUSAGE
    };

    struct TProcessInfo : public oslProcessInfo
    {
        TProcessInfo() { Size = sizeof(*this); }
    };

    typedef oslProcessIdentifier TProcessIdentifier;

    /** Creating a process object by naming the executable.
        Does not yet start the process.
        @see execute
    */

    OProcess( );

    OProcess(const ::rtl::OUString& strImageName);

    OProcess(const ::rtl::OUString& strImageName,
             const ::rtl::OUString& strWorkingDirectory);

    /// destroying a process object
    virtual ~OProcess();

    SAL_CALL operator oslProcess()
        { return m_Process; }

    SAL_CALL operator oslProcess() const
        { return m_Process; }

    static OProcess* SAL_CALL getProcess(TProcessIdentifier Identifier);

    /** execute the given process.
        This process becomes a child of the caller.
        If there are any ioresources provided from the calling process, this
        function returns only, if the child process calls OStartupInfo::acceptIOResource().
        @param Options [in] describes the execution mode.
        @return only not eNONE, if too much environments are added.
        @see OStartupInfo::acceptIOResource
    */
    TProcessError SAL_CALL execute(TProcessOption Options,
                          const OArgumentList& aArgumentList = OArgumentList(),
                          const OEnvironment&  aEnvironment  = OEnvironment()
                          );

    /** execute the given process with the specified security.
        This process becomes a child of the caller.
        The process is executed with the rights of the user, for whom the security object is created.
        If there are any ioresources provided from the calling process, this
        function returns only, if the child process calls OStartupInfo::acceptIOResource().
        @param Options [in] describes the execution mode.
        @param Security [in] is a given security object for one logged in user.
        @return eNONE, if the process could be executed, otherwise an errorcode.
        @see OStartupInfo::acceptIOResource
    */
    TProcessError SAL_CALL execute(TProcessOption Options,
                          const OSecurity &Security,
                          const OArgumentList& aArgumentList = OArgumentList(),
                          const OEnvironment&  aEnvironment  = OEnvironment()
                         );

    TProcessError SAL_CALL terminate();

    TProcessError SAL_CALL getInfo(TProcessData Data, TProcessInfo* pInfo) const;

    static TProcessError SAL_CALL getCurrentInfo(TProcessData Data, TProcessInfo* pInfo);

    /** wait for the completation of this child process
        @return eNONE if child process exits, otherwise nothing.
    */
    TProcessError SAL_CALL join();

protected:
    const ::rtl::OUString m_strImageName;
    const ::rtl::OUString m_strDirectory;

    oslProcess         m_Process;
};

/** informations for client processes provided by the parent.
    @see OProcess
*/


class VOS_DLLPUBLIC OStartupInfo : public OObject
{
    VOS_DECLARE_CLASSINFO(VOS_NAMESPACE(OStartupInfo, vos));

public:
    /** Errorcodes:
    */
    enum TStartupError {
        E_None         = osl_Process_E_None,            /* no error */
        E_NotFound     = osl_Process_E_NotFound,        /* image not found */
        E_TimedOut     = osl_Process_E_TimedOut,        /* timout occurred */
        E_NoPermission = osl_Process_E_NoPermission,    /* permission denied */
        E_Unknown      = osl_Process_E_Unknown,         /* unknown error */
        E_InvalidError = osl_Process_E_InvalidError     /* unmapped error */
    };

    /** Constructor.
    */
    OStartupInfo();

    /** Destructor
    */
    ~OStartupInfo();

    /** @return the number of command line arguments.
     */
    sal_uInt32 SAL_CALL getCommandArgCount();

    /** get the nArg-th command argument passed to the main-function of this process.
        @param nArg [in] the number of arguments to return.
        @param strCommandArg [out] the string that receives the argument.
        @return eNONE
    */
    TStartupError SAL_CALL getCommandArg(sal_uInt32 nArg, ::rtl::OUString& strCommandArg);

    TStartupError SAL_CALL getExecutableFile(::rtl::OUString& strImageName)
        const;

    /** Get the value of one environment variable.
        @param Name [in] denotes the name of the variable to get.
        @param Buffer [out] is the buffer where the value of this variable is returned.
        @param Max [in] is the size of this buffer.
        @return eNONE, if the variable exist in the environment, otherwise False.
    */
    TStartupError SAL_CALL getEnvironment(const ::rtl::OUString& strVar, ::rtl::OUString& strValue);
};



/** get extended arguments from command line and an argument file
    the file name is given on the command line as "@filename"
    (filename must be in our UNC notation!)
    enumeration starts with 0 (i.e. argv[1])
    each line in the file will be treated as one argument
    @see also OProcess and OStartupInfo
*/

class OExtCommandLineImpl;

class VOS_DLLPUBLIC OExtCommandLine : public OObject
{
    VOS_DECLARE_CLASSINFO(VOS_NAMESPACE(OExtCommandLine, vos));
    static vos::OExtCommandLineImpl* pExtImpl;

public:

    /** Constructor.
    */
    OExtCommandLine();

    /** Destructor
    */
    virtual ~OExtCommandLine();

    /** @return the number of extended command line arguments.
     */
    sal_uInt32 SAL_CALL getCommandArgCount();


    /** get the nArg-th extended command argument
        @param nArg [in] the number of extended argument to return.
        @param strCommandArg [out] the string that receives the argument.
        @return sal_True   if the nArg-th argument has been retrieved successfully
        @return sal_False  on all other cases
    */
    sal_Bool SAL_CALL getCommandArg(sal_uInt32 nArg, ::rtl::OUString& strCommandArg);


};

}

#endif  // _VOS_PROCESS_HXX_


