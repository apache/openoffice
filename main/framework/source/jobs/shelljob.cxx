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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_framework.hxx"

//_______________________________________________
// include own header

#include <jobs/shelljob.hxx>
#include <jobs/jobconst.hxx>
#include <threadhelp/readguard.hxx>
#include <services.h>

//_______________________________________________
// include others

#include <osl/file.hxx>
#include <osl/process.h>
#include <vcl/svapp.hxx>
#include <rtl/ustrbuf.hxx>
#include <comphelper/sequenceashashmap.hxx>

//_______________________________________________
// include interfaces

#include <com/sun/star/util/XStringSubstitution.hpp>

//_______________________________________________
// namespace

namespace framework{

//_______________________________________________
// definitions

/** address job configuration inside argument set provided on method execute(). */
static const ::rtl::OUString PROP_JOBCONFIG = ::rtl::OUString::createFromAscii("JobConfig");

/** address job configuration property "Command". */
static const ::rtl::OUString PROP_COMMAND = ::rtl::OUString::createFromAscii("Command");

/** address job configuration property "Arguments". */
static const ::rtl::OUString PROP_ARGUMENTS = ::rtl::OUString::createFromAscii("Arguments");

/** address job configuration property "DeactivateJobIfDone". */
static const ::rtl::OUString PROP_DEACTIVATEJOBIFDONE = ::rtl::OUString::createFromAscii("DeactivateJobIfDone");

/** address job configuration property "CheckExitCode". */
static const ::rtl::OUString PROP_CHECKEXITCODE = ::rtl::OUString::createFromAscii("CheckExitCode");

//-----------------------------------------------

DEFINE_XSERVICEINFO_MULTISERVICE(ShellJob                   ,
                                 ::cppu::OWeakObject        ,
                                 SERVICENAME_JOB            ,
                                 IMPLEMENTATIONNAME_SHELLJOB)

DEFINE_INIT_SERVICE(ShellJob,
                    {
                        /*  Attention
                            I think we don't need any mutex or lock here ... because we are called by our own static method impl_createInstance()
                            to create a new instance of this class by our own supported service factory.
                            see macro DEFINE_XSERVICEINFO_MULTISERVICE and "impl_initService()" for further informations!
                        */
                    }
                   )
                    
//-----------------------------------------------
ShellJob::ShellJob(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR)
    : ThreadHelpBase(     )
    , m_xSMGR       (xSMGR)
{
}

//-----------------------------------------------
ShellJob::~ShellJob()
{
}

//-----------------------------------------------
css::uno::Any SAL_CALL ShellJob::execute(const css::uno::Sequence< css::beans::NamedValue >& lJobArguments)
    throw(css::lang::IllegalArgumentException,
          css::uno::Exception                ,
          css::uno::RuntimeException         )
{
	::comphelper::SequenceAsHashMap lArgs  (lJobArguments);
	::comphelper::SequenceAsHashMap lOwnCfg(lArgs.getUnpackedValueOrDefault(PROP_JOBCONFIG, css::uno::Sequence< css::beans::NamedValue >()));

	const ::rtl::OUString                       sCommand                   = lOwnCfg.getUnpackedValueOrDefault(PROP_COMMAND                  , ::rtl::OUString());
    const css::uno::Sequence< ::rtl::OUString > lCommandArguments          = lOwnCfg.getUnpackedValueOrDefault(PROP_ARGUMENTS                , css::uno::Sequence< ::rtl::OUString >());
	const ::sal_Bool                            bDeactivateJobIfDone       = lOwnCfg.getUnpackedValueOrDefault(PROP_DEACTIVATEJOBIFDONE      , sal_True         );
	const ::sal_Bool                            bCheckExitCode             = lOwnCfg.getUnpackedValueOrDefault(PROP_CHECKEXITCODE            , sal_True         );

    // replace all might existing place holder.
    ::rtl::OUString sRealCommand = impl_substituteCommandVariables(sCommand);
        
    // Command is required as minimum.
    // If it does not exists ... we can't do our job.
    // Deactivate such miss configured job silently .-)
	if (sRealCommand.getLength() < 1)
	    return ShellJob::impl_generateAnswer4Deactivation();

    // do it
    ::sal_Bool bDone = impl_execute(sRealCommand, lCommandArguments, bCheckExitCode);
    if (! bDone)
        return css::uno::Any();
        
    // Job was done ... user configured deactivation of this job
    // in such case.
    if (bDeactivateJobIfDone)
        return ShellJob::impl_generateAnswer4Deactivation();
    
    // There was no decision about deactivation of this job.
    // So we have to return nothing here !
    return css::uno::Any();
}

//-----------------------------------------------
css::uno::Any ShellJob::impl_generateAnswer4Deactivation()
{
	css::uno::Sequence< css::beans::NamedValue > aAnswer(1);
	aAnswer[0].Name  = JobConst::ANSWER_DEACTIVATE_JOB();
	aAnswer[0].Value = css::uno::makeAny(sal_True);

	return css::uno::makeAny(aAnswer);
}

//-----------------------------------------------
::rtl::OUString ShellJob::impl_substituteCommandVariables(const ::rtl::OUString& sCommand)
{
    // SYNCHRONIZED ->
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SYNCHRONIZED

	try
	{
              css::uno::Reference< css::util::XStringSubstitution > xSubst           (  xSMGR->createInstance(SERVICENAME_SUBSTITUTEPATHVARIABLES), css::uno::UNO_QUERY_THROW);
		const ::sal_Bool											bSubstRequired   = sal_True;
		const ::rtl::OUString										sCompleteCommand = xSubst->substituteVariables(sCommand, bSubstRequired);
    
        return sCompleteCommand;
	}
	catch(const css::uno::Exception&)
	{}

    return ::rtl::OUString();
}

//-----------------------------------------------
::sal_Bool ShellJob::impl_execute(const ::rtl::OUString&                       sCommand      ,
                                  const css::uno::Sequence< ::rtl::OUString >& lArguments    ,
                                        ::sal_Bool                             bCheckExitCode)
{
          ::rtl_uString**  pArgs    = NULL;
    const ::sal_Int32      nArgs    = lArguments.getLength ();
          oslProcessOption nOptions = osl_Process_WAIT;
          oslProcess       hProcess(0);

    if (nArgs > 0)
        pArgs = reinterpret_cast< ::rtl_uString** >(const_cast< ::rtl::OUString* >(lArguments.getConstArray()));
    
    oslProcessError eError = osl_executeProcess(sCommand.pData, pArgs, nArgs, nOptions, NULL, NULL, NULL, 0, &hProcess);

    // executable not found or couldn't be started
    if (eError != osl_Process_E_None)
        return sal_False;

    ::sal_Bool bRet = sal_True;
    if (bCheckExitCode)
    {
        // check its return codes ...
        oslProcessInfo aInfo;
        aInfo.Size = sizeof (oslProcessInfo);
        eError = osl_getProcessInfo(hProcess, osl_Process_EXITCODE, &aInfo);

        if (eError != osl_Process_E_None)
            bRet = sal_False;
        else
            bRet = (aInfo.Code == 0);
    }
    osl_freeProcessHandle(hProcess);
    return bRet;
}

} // namespace framework
