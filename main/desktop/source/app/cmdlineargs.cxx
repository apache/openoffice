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
#include "precompiled_desktop.hxx"
#include <cmdlineargs.hxx>
#include <vcl/svapp.hxx>
#include <rtl/uri.hxx>
#include <rtl/ustring.hxx>
#include "rtl/process.h"
#include <comphelper/processfactory.hxx>
#include <com/sun/star/uri/XExternalUriReferenceTranslator.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include "tools/getprocessworkingdir.hxx"

#include <svl/documentlockfile.hxx>

using namespace rtl;
using namespace com::sun::star::lang;
using namespace com::sun::star::uri;
using namespace com::sun::star::uno;

namespace desktop
{

namespace {

class ExtCommandLineSupplier: public CommandLineArgs::Supplier {
public:
    explicit ExtCommandLineSupplier():
        m_count(rtl_getAppCommandArgCount()),
        m_index(0)
    {
        rtl::OUString url;
        if (tools::getProcessWorkingDir(&url)) {
            m_cwdUrl.reset(url);
        }
    }

    virtual ~ExtCommandLineSupplier() {}

    virtual boost::optional< rtl::OUString > getCwdUrl() { return m_cwdUrl; }

    virtual bool next(rtl::OUString * argument) {
        OSL_ASSERT(argument != NULL);
        if (m_index < m_count) {
            rtl_getAppCommandArg(m_index++, &argument->pData);
            return true;
        } else {
            return false;
        }
    }

private:
    boost::optional< rtl::OUString > m_cwdUrl;
    sal_uInt32 m_count;
    sal_uInt32 m_index;
};

}

static CommandLineArgs::BoolParam aModuleGroupDefinition[] =
{
	CommandLineArgs::CMD_BOOLPARAM_WRITER,
	CommandLineArgs::CMD_BOOLPARAM_CALC,
	CommandLineArgs::CMD_BOOLPARAM_DRAW,
	CommandLineArgs::CMD_BOOLPARAM_IMPRESS,
	CommandLineArgs::CMD_BOOLPARAM_GLOBAL,
	CommandLineArgs::CMD_BOOLPARAM_MATH,
	CommandLineArgs::CMD_BOOLPARAM_WEB,
	CommandLineArgs::CMD_BOOLPARAM_BASE
};

CommandLineArgs::GroupDefinition CommandLineArgs::m_pGroupDefinitions[ CommandLineArgs::CMD_GRPID_COUNT ] =
{
	{ 8, aModuleGroupDefinition }
};

CommandLineArgs::Supplier::Exception::Exception() {}

CommandLineArgs::Supplier::Exception::Exception(Exception const &) {}

CommandLineArgs::Supplier::Exception::~Exception() {}

CommandLineArgs::Supplier::Exception &
CommandLineArgs::Supplier::Exception::operator =(Exception const &)
{ return *this; }

CommandLineArgs::Supplier::~Supplier() {}

// initialize class with command line parameters from process environment
CommandLineArgs::CommandLineArgs()
{
	ResetParamValues();
    ExtCommandLineSupplier s;
	ParseCommandLine_Impl( s );
}

CommandLineArgs::CommandLineArgs( Supplier& supplier )
{
	ResetParamValues();
	ParseCommandLine_Impl( supplier );
}

// ----------------------------------------------------------------------------

void CommandLineArgs::ParseCommandLine_Impl( Supplier& supplier )
{
    m_cwdUrl = supplier.getCwdUrl();
	Reference<XMultiServiceFactory> xMS(comphelper::getProcessServiceFactory(), UNO_QUERY);
	OSL_ENSURE(xMS.is(), "CommandLineArgs: no ProcessServiceFactory.");

    Reference< XExternalUriReferenceTranslator > xTranslator(
        xMS->createInstance(
        OUString::createFromAscii(
        "com.sun.star.uri.ExternalUriReferenceTranslator")),
        UNO_QUERY);

	// parse command line arguments
    bool bOpenEvent(true);
    bool bPrintEvent(false);
    bool bViewEvent(false);
    bool bStartEvent(false);
    bool bPrintToEvent(false);
    bool bPrinterName(false);
    bool bForceOpenEvent(false);
    bool bForceNewEvent(false);
    bool bDisplaySpec(false);
    bool bOpenDoc(false);

    m_eArgumentCount = NONE;

    for (;;)
    {
        ::rtl::OUString	aArg;
        if ( !supplier.next( &aArg ) )
        {
            break;
        }
        // convert file URLs to internal form #112849#
        if (aArg.indexOf(OUString::createFromAscii("file:"))==0 && 
            xTranslator.is())
        {
            OUString tmp(xTranslator->translateToInternal(aArg));
            if (tmp.getLength() > 0)
                aArg = tmp;
        }

        String aArgStr = aArg;
        if ( aArg.getLength() > 0 )
        {
            m_eArgumentCount = m_eArgumentCount == NONE ? ONE : MANY;
            if ( !InterpretCommandLineParameter( aArg ))
            {
                if ( aArgStr.GetChar(0) == '-' )
                {
                    // handle this argument as an option
                    if ( aArgStr.EqualsIgnoreCaseAscii( "-n" ))
                    {
                        // force new documents based on the following documents
                        bForceNewEvent  = true;
                        bOpenEvent      = false;
                        bForceOpenEvent = false;
                        bPrintToEvent   = false;
                        bPrintEvent     = false;
                        bViewEvent      = false;
                        bStartEvent     = false;
                        bDisplaySpec    = false;
                    }
                    else if ( aArgStr.EqualsIgnoreCaseAscii( "-o" ))
                    {
                        // force open documents regardless if they are templates or not
                        bForceOpenEvent = true;
                        bOpenEvent      = false;
                        bForceNewEvent  = false;
                        bPrintToEvent   = false;
                        bPrintEvent     = false;
                        bViewEvent      = false;
                        bStartEvent     = false;
                        bDisplaySpec    = false;
                    }
                    else if ( aArgStr.EqualsIgnoreCaseAscii( "-pt" ))
                    {
                        // Print to special printer
                        bPrintToEvent   = true;
                        bPrinterName    = true;
                        bPrintEvent     = false;
                        bOpenEvent      = false;
                        bForceNewEvent  = false;
                        bViewEvent      = false;
                        bStartEvent     = false;
                        bDisplaySpec    = false;
                        bForceOpenEvent = false;
                   }
                   else if ( aArgStr.EqualsIgnoreCaseAscii( "-p" ))
                   {
                        // Print to default printer
                        bPrintEvent     = true;
                        bPrintToEvent   = false;
                        bOpenEvent      = false;
                        bForceNewEvent  = false;
                        bForceOpenEvent = false;
                        bViewEvent      = false;
                        bStartEvent     = false;
                        bDisplaySpec    = false;
                   }
                   else if ( aArgStr.EqualsIgnoreCaseAscii( "-view" ))
                   {
                        // open in viewmode
                        bOpenEvent      = false;
                        bPrintEvent     = false;
                        bPrintToEvent   = false;
                        bForceNewEvent  = false;
                        bForceOpenEvent = false;
                        bViewEvent      = true; 
                        bStartEvent     = false;
                        bDisplaySpec    = false;
 		    }
                    else if ( aArgStr.EqualsIgnoreCaseAscii( "-show" ))
                    {
                        // open in viewmode
                        bOpenEvent      = false;
                        bViewEvent      = false;
                        bStartEvent     = true;
                        bPrintEvent     = false;
                        bPrintToEvent   = false;
                        bForceNewEvent  = false;
                        bForceOpenEvent = false;
                        bDisplaySpec    = false;
                    }
                    else if ( aArgStr.EqualsIgnoreCaseAscii( "-display" ))
                    {
                        // set display
                        bOpenEvent      = false;
                        bPrintEvent     = false;
                        bForceOpenEvent = false;
                        bPrintToEvent   = false;
                        bForceNewEvent  = false;
                        bViewEvent      = false;
                        bStartEvent     = false;
                        bDisplaySpec    = true;
                    }
                    else if ( aArgStr.EqualsIgnoreCaseAscii( "-language" ))
                    {
                        bOpenEvent      = false;
                        bPrintEvent     = false;
                        bForceOpenEvent = false;
                        bPrintToEvent   = false;
                        bForceNewEvent  = false;
                        bViewEvent      = false;
                        bStartEvent     = false;
                        bDisplaySpec    = false;
                    }

                    #ifdef MACOSX
                    /* #i84053# ignore -psn on Mac
                       Platform dependent #ifdef here is ugly, however this is currently
                       the only platform dependent parameter. Should more appear
                       we should find a better solution
                    */
                    else if ( aArgStr.CompareToAscii( "-psn", 4 ) == COMPARE_EQUAL )
                    {
                        // finder argument from MacOSX
                        bOpenEvent      = false;
                       	bPrintEvent     = false;
                        bForceOpenEvent = false;
                        bPrintToEvent   = false;
                        bForceNewEvent  = false;
                        bViewEvent      = false;
                        bStartEvent     = false;
                        bDisplaySpec    = false;
                    }
                    #endif
                }
                else
                {
                    if ( bPrinterName && bPrintToEvent )
                    {
                        // first argument after "-pt" this must be the printer name
                        AddStringListParam_Impl( CMD_STRINGPARAM_PRINTERNAME, aArgStr );
                        bPrinterName = sal_False;
                    }
                    else
                    {
                        // handle this argument as a filename
                        if ( bOpenEvent )
                        {
                            AddStringListParam_Impl( CMD_STRINGPARAM_OPENLIST, aArgStr );
                            bOpenDoc = true;
                        }
                        else if ( bViewEvent )
	                {
                            AddStringListParam_Impl( CMD_STRINGPARAM_VIEWLIST, aArgStr );
                            bOpenDoc = true;
                        }
                        else if ( bStartEvent )
                        {
                            AddStringListParam_Impl( CMD_STRINGPARAM_STARTLIST, aArgStr );
                            bOpenDoc = true;
                        }
                        else if ( bPrintEvent )
                        {
                            AddStringListParam_Impl( CMD_STRINGPARAM_PRINTLIST, aArgStr );
                            bOpenDoc = true;
                        }
                        else if ( bPrintToEvent )
                        {
                            AddStringListParam_Impl( CMD_STRINGPARAM_PRINTTOLIST, aArgStr );
                            bOpenDoc = true;
                        }
                        else if ( bForceNewEvent )
                        {
                            AddStringListParam_Impl( CMD_STRINGPARAM_FORCENEWLIST, aArgStr );
                            bOpenDoc = true;
                        }
                        else if ( bForceOpenEvent )
                        {
                            AddStringListParam_Impl( CMD_STRINGPARAM_FORCEOPENLIST, aArgStr );
                            bOpenDoc = true;
                        }
                        else if ( bDisplaySpec )
                        {
                            AddStringListParam_Impl( CMD_STRINGPARAM_DISPLAY, aArgStr );
                            bDisplaySpec = false; // only one display, not a lsit
                            bOpenEvent = true;    // set back to standard
                        }
                    }
                }
            }
        }
    }

    if ( bOpenDoc )
        m_bDocumentArgs = true;
}

void CommandLineArgs::AddStringListParam_Impl( StringParam eParam, const rtl::OUString& aParam )
{
	OSL_ASSERT( eParam >= 0 && eParam < CMD_STRINGPARAM_COUNT );
	if ( m_aStrParams[eParam].getLength() )
		m_aStrParams[eParam] += ::rtl::OUString::valueOf( (sal_Unicode)APPEVENT_PARAM_DELIMITER );
	m_aStrParams[eParam] += aParam;
	m_aStrSetParams[eParam] = sal_True;
}

void CommandLineArgs::SetBoolParam_Impl( BoolParam eParam, sal_Bool bValue )
{
	OSL_ASSERT( eParam >= 0 && eParam < CMD_BOOLPARAM_COUNT );
	m_aBoolParams[eParam] = bValue;
}

sal_Bool CommandLineArgs::InterpretCommandLineParameter( const ::rtl::OUString& aArg )
{
    String aArgStr( aArg );

    if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-minimized" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_MINIMIZED, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-invisible" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_INVISIBLE, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-norestore" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_NORESTORE, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-nodefault" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_NODEFAULT, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-bean" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_BEAN, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-plugin" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_PLUGIN, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-server" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_SERVER, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-headless" )) == sal_True )
    {
        // Headless means also invisibile, so set this parameter to true!
        SetBoolParam_Impl( CMD_BOOLPARAM_HEADLESS, sal_True );
        SetBoolParam_Impl( CMD_BOOLPARAM_INVISIBLE, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-conversionmode" )) == sal_True )
    {
        // ConversionMode means also headless and invisibile, so set these parameters to true!
        SetBoolParam_Impl( CMD_BOOLPARAM_HEADLESS, sal_True );
        SetBoolParam_Impl( CMD_BOOLPARAM_INVISIBLE, sal_True );
        SetBoolParam_Impl( CMD_BOOLPARAM_CONVERSIONMODE, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-quickstart" )) == sal_True )
    {
#if defined(ENABLE_QUICKSTART_APPLET)
        SetBoolParam_Impl( CMD_BOOLPARAM_QUICKSTART, sal_True );
#endif
        SetBoolParam_Impl( CMD_BOOLPARAM_NOQUICKSTART, sal_False );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-quickstart=no" )))
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_NOQUICKSTART, sal_True );
        SetBoolParam_Impl( CMD_BOOLPARAM_QUICKSTART, sal_False );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-terminate_after_init" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_TERMINATEAFTERINIT, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-nofirststartwizard" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_NOFIRSTSTARTWIZARD, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-nologo" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_NOLOGO, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-nolockcheck" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_NOLOCKCHECK, sal_True );
        // Workaround for automated testing
        ::svt::DocumentLockFile::AllowInteraction( sal_False );

        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-help" ))
	      || aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-h" ))
	      || aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-?" )))
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_HELP, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-helpwriter" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_HELPWRITER, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-helpcalc" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_HELPCALC, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-helpdraw" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_HELPDRAW, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-helpimpress" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_HELPIMPRESS, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-helpbase" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_HELPBASE, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-helpbasic" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_HELPBASIC, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-helpmath" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_HELPMATH, sal_True );
        return sal_True;
    }
#ifdef MACOSX
  /* #i84053# ignore -psn on Mac
     Platform dependent #ifdef here is ugly, however this is currently
     the only platform dependent parameter. Should more appear
     we should find a better solution
  */
    else if ( aArg.compareToAscii( "-psn", 4 ) == 0 )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_PSN, sal_True );
        return sal_True;
    }
#endif
    else if ( aArgStr.Copy(0, 8).EqualsIgnoreCaseAscii( "-accept=" ))
    {
        AddStringListParam_Impl( CMD_STRINGPARAM_ACCEPT, aArgStr.Copy( 8 ) );
        return sal_True;
    }
    else if ( aArgStr.Copy(0, 10).EqualsIgnoreCaseAscii( "-unaccept=" ))
    {
        AddStringListParam_Impl( CMD_STRINGPARAM_UNACCEPT, aArgStr.Copy( 10 ) );
        return sal_True;
    }
    else if ( aArgStr.CompareIgnoreCaseToAscii( "-portal," ,
              RTL_CONSTASCII_LENGTH( "-portal," )) == COMPARE_EQUAL )
    {
        AddStringListParam_Impl( CMD_STRINGPARAM_PORTAL, aArgStr.Copy( RTL_CONSTASCII_LENGTH( "-portal," )) );
        return sal_True;
    }
    else if ( aArgStr.Copy( 0, 7 ).EqualsIgnoreCaseAscii( "-userid" ))
    {
        if ( aArgStr.Len() > 8 )
	{
	    rtl::OUString aUserDir = aArgStr;
	    AddStringListParam_Impl( CMD_STRINGPARAM_USERDIR,
				     ::rtl::Uri::decode( aUserDir.copy( 8 ), rtl_UriDecodeWithCharset, RTL_TEXTENCODING_UTF8 ) );
        }
        return sal_True;
    }
    else if ( aArgStr.Copy( 0, 15).EqualsIgnoreCaseAscii( "-clientdisplay=" ))
    {
        AddStringListParam_Impl( CMD_STRINGPARAM_CLIENTDISPLAY, aArgStr.Copy( 15 ) );
        return sal_True;
    }
    else if ( aArgStr.Copy(0, 9).EqualsIgnoreCaseAscii( "-version=" ))
    {
        AddStringListParam_Impl( CMD_STRINGPARAM_VERSION, aArgStr.Copy( 9 ) );
        return sal_True;
    }
    else if ( aArgStr.Copy(0, 10).EqualsIgnoreCaseAscii( "-language=" ))
    {
        AddStringListParam_Impl( CMD_STRINGPARAM_LANGUAGE, aArgStr.Copy( 10 ) );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-writer" )) == sal_True )
    {
        sal_Bool bAlreadySet = CheckGroupMembers( CMD_GRPID_MODULE, CMD_BOOLPARAM_WRITER );
        if ( !bAlreadySet )
	    SetBoolParam_Impl( CMD_BOOLPARAM_WRITER, sal_True );
        m_bDocumentArgs = true;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-calc" )) == sal_True )
    {
        sal_Bool bAlreadySet = CheckGroupMembers( CMD_GRPID_MODULE, CMD_BOOLPARAM_CALC );
        if ( !bAlreadySet )
	    SetBoolParam_Impl( CMD_BOOLPARAM_CALC, sal_True );
        m_bDocumentArgs = true;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-draw" )) == sal_True )
    {
        sal_Bool bAlreadySet = CheckGroupMembers( CMD_GRPID_MODULE, CMD_BOOLPARAM_DRAW );
        if ( !bAlreadySet )
	    SetBoolParam_Impl( CMD_BOOLPARAM_DRAW, sal_True );
        m_bDocumentArgs = true;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-impress" )) == sal_True )
    {
        sal_Bool bAlreadySet = CheckGroupMembers( CMD_GRPID_MODULE, CMD_BOOLPARAM_IMPRESS );
        if ( !bAlreadySet )
	    SetBoolParam_Impl( CMD_BOOLPARAM_IMPRESS, sal_True );
        m_bDocumentArgs = true;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-base" )) == sal_True )
    {
        sal_Bool bAlreadySet = CheckGroupMembers( CMD_GRPID_MODULE, CMD_BOOLPARAM_BASE );
        if ( !bAlreadySet )
	    SetBoolParam_Impl( CMD_BOOLPARAM_BASE, sal_True );
        m_bDocumentArgs = true;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-global" )) == sal_True )
    {
        sal_Bool bAlreadySet = CheckGroupMembers( CMD_GRPID_MODULE, CMD_BOOLPARAM_GLOBAL );
        if ( !bAlreadySet )
	    SetBoolParam_Impl( CMD_BOOLPARAM_GLOBAL, sal_True );
        m_bDocumentArgs = true;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-math" )) == sal_True )
    {
        sal_Bool bAlreadySet = CheckGroupMembers( CMD_GRPID_MODULE, CMD_BOOLPARAM_MATH );
        if ( !bAlreadySet )
	    SetBoolParam_Impl( CMD_BOOLPARAM_MATH, sal_True );
        m_bDocumentArgs = true;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-web" )) == sal_True )
    {
        sal_Bool bAlreadySet = CheckGroupMembers( CMD_GRPID_MODULE, CMD_BOOLPARAM_WEB );
        if ( !bAlreadySet )
	    SetBoolParam_Impl( CMD_BOOLPARAM_WEB, sal_True );
        m_bDocumentArgs = true;
        return sal_True;
    }

    return sal_False;
}

sal_Bool CommandLineArgs::CheckGroupMembers( GroupParamId nGroupId, BoolParam nExcludeMember ) const
{
    // Check if at least one bool param out of a group is set. An exclude member can be provided.
    for ( int i = 0; i < m_pGroupDefinitions[nGroupId].nCount; i++ )
    {
        BoolParam nParam = m_pGroupDefinitions[nGroupId].pGroupMembers[i];
        if ( nParam != nExcludeMember && m_aBoolParams[nParam] )
            return sal_True;
    }

    return sal_False;
}

void CommandLineArgs::ResetParamValues()
{
    int i;
    for ( i = 0; i < CMD_BOOLPARAM_COUNT; i++ )
        m_aBoolParams[i] = sal_False;
    for ( i = 0; i < CMD_STRINGPARAM_COUNT; i++ )
        m_aStrSetParams[i] = sal_False;
    m_eArgumentCount = NONE;
    m_bDocumentArgs  = false;
}

void CommandLineArgs::SetBoolParam( BoolParam eParam, sal_Bool bNewValue )
{
	osl::MutexGuard  aMutexGuard( m_aMutex );

	OSL_ASSERT( ( eParam >= 0 && eParam < CMD_BOOLPARAM_COUNT ) );
	m_aBoolParams[eParam] = bNewValue;
}

sal_Bool CommandLineArgs::IsMinimized() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_MINIMIZED ];
}

sal_Bool CommandLineArgs::IsInvisible() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_INVISIBLE ];
}

sal_Bool CommandLineArgs::IsNoRestore() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_NORESTORE ];
}

sal_Bool CommandLineArgs::IsNoDefault() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_NODEFAULT ];
}

sal_Bool CommandLineArgs::IsBean() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_BEAN ];
}

sal_Bool CommandLineArgs::IsServer() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_SERVER ];
}

sal_Bool CommandLineArgs::IsHeadless() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_HEADLESS ];
}

sal_Bool CommandLineArgs::IsConversionMode() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_CONVERSIONMODE ];
}

sal_Bool CommandLineArgs::IsQuickstart() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_QUICKSTART ];
}

sal_Bool CommandLineArgs::IsNoQuickstart() const
{
    osl::MutexGuard aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_NOQUICKSTART ];
}

sal_Bool CommandLineArgs::IsTerminateAfterInit() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_TERMINATEAFTERINIT ];
}

sal_Bool CommandLineArgs::IsNoFirstStartWizard() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_NOFIRSTSTARTWIZARD ];
}

sal_Bool CommandLineArgs::IsNoLogo() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_NOLOGO ];
}

sal_Bool CommandLineArgs::IsNoLockcheck() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_NOLOCKCHECK ];
}

sal_Bool CommandLineArgs::IsHelp() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_HELP ];
}
sal_Bool CommandLineArgs::IsHelpWriter() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_HELPWRITER ];
}

sal_Bool CommandLineArgs::IsHelpCalc() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_HELPCALC ];
}

sal_Bool CommandLineArgs::IsHelpDraw() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_HELPDRAW ];
}

sal_Bool CommandLineArgs::IsHelpImpress() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_HELPIMPRESS ];
}

sal_Bool CommandLineArgs::IsHelpBase() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_HELPBASE ];
}
sal_Bool CommandLineArgs::IsHelpMath() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_HELPMATH ];
}
sal_Bool CommandLineArgs::IsHelpBasic() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_HELPBASIC ];
}

sal_Bool CommandLineArgs::IsWriter() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_WRITER ];
}

sal_Bool CommandLineArgs::IsCalc() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_CALC ];
}

sal_Bool CommandLineArgs::IsDraw() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_DRAW ];
}

sal_Bool CommandLineArgs::IsImpress() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_IMPRESS ];
}

sal_Bool CommandLineArgs::IsBase() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_BASE ];
}

sal_Bool CommandLineArgs::IsGlobal() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_GLOBAL ];
}

sal_Bool CommandLineArgs::IsMath() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_MATH ];
}

sal_Bool CommandLineArgs::IsWeb() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return m_aBoolParams[ CMD_BOOLPARAM_WEB ];
}

sal_Bool CommandLineArgs::HasModuleParam() const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	return CheckGroupMembers( CMD_GRPID_MODULE, CMD_BOOLPARAM_COUNT );
}

sal_Bool CommandLineArgs::GetPortalConnectString( ::rtl::OUString& rPara ) const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	rPara = m_aStrParams[ CMD_STRINGPARAM_PORTAL ];
	return m_aStrSetParams[ CMD_STRINGPARAM_PORTAL ];
}

sal_Bool CommandLineArgs::GetAcceptString( ::rtl::OUString& rPara ) const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	rPara = m_aStrParams[ CMD_STRINGPARAM_ACCEPT ];
	return m_aStrSetParams[ CMD_STRINGPARAM_ACCEPT ];
}

sal_Bool CommandLineArgs::GetUnAcceptString( ::rtl::OUString& rPara ) const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	rPara = m_aStrParams[ CMD_STRINGPARAM_UNACCEPT ];
	return m_aStrSetParams[ CMD_STRINGPARAM_UNACCEPT ];
}

sal_Bool CommandLineArgs::GetOpenList( ::rtl::OUString& rPara) const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	rPara = m_aStrParams[ CMD_STRINGPARAM_OPENLIST ];
	return m_aStrSetParams[ CMD_STRINGPARAM_OPENLIST ];
}

sal_Bool CommandLineArgs::GetViewList( ::rtl::OUString& rPara) const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	rPara = m_aStrParams[ CMD_STRINGPARAM_VIEWLIST ];
	return m_aStrSetParams[ CMD_STRINGPARAM_VIEWLIST ];
}

sal_Bool CommandLineArgs::GetStartList( ::rtl::OUString& rPara) const
{
      osl::MutexGuard  aMutexGuard( m_aMutex );
      rPara = m_aStrParams[ CMD_STRINGPARAM_STARTLIST ];
      return m_aStrSetParams[ CMD_STRINGPARAM_STARTLIST ];
}

sal_Bool CommandLineArgs::GetForceOpenList( ::rtl::OUString& rPara) const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	rPara = m_aStrParams[ CMD_STRINGPARAM_FORCEOPENLIST ];
	return m_aStrSetParams[ CMD_STRINGPARAM_FORCEOPENLIST ];
}

sal_Bool CommandLineArgs::GetForceNewList( ::rtl::OUString& rPara) const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	rPara = m_aStrParams[ CMD_STRINGPARAM_FORCENEWLIST ];
	return m_aStrSetParams[ CMD_STRINGPARAM_FORCENEWLIST ];
}

sal_Bool CommandLineArgs::GetPrintList( ::rtl::OUString& rPara) const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	rPara = m_aStrParams[ CMD_STRINGPARAM_PRINTLIST ];
	return m_aStrSetParams[ CMD_STRINGPARAM_PRINTLIST ];
}

sal_Bool CommandLineArgs::GetPrintToList( ::rtl::OUString& rPara ) const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	rPara = m_aStrParams[ CMD_STRINGPARAM_PRINTTOLIST ];
	return m_aStrSetParams[ CMD_STRINGPARAM_PRINTTOLIST ];
}

sal_Bool CommandLineArgs::GetPrinterName( ::rtl::OUString& rPara ) const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	rPara = m_aStrParams[ CMD_STRINGPARAM_PRINTERNAME ];
	return m_aStrSetParams[ CMD_STRINGPARAM_PRINTERNAME ];
}

sal_Bool CommandLineArgs::GetLanguage( ::rtl::OUString& rPara ) const
{
	osl::MutexGuard  aMutexGuard( m_aMutex );
	rPara = m_aStrParams[ CMD_STRINGPARAM_LANGUAGE ];
	return m_aStrSetParams[ CMD_STRINGPARAM_LANGUAGE ];
}

sal_Bool CommandLineArgs::IsEmpty() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_eArgumentCount == NONE;
}

sal_Bool CommandLineArgs::IsEmptyOrAcceptOnly() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );

    return m_eArgumentCount == NONE || 
           ( ( m_eArgumentCount == ONE ) && ( m_aStrParams[ CMD_STRINGPARAM_ACCEPT ].getLength() )) ||
           ( ( m_eArgumentCount == ONE ) && m_aBoolParams[ CMD_BOOLPARAM_PSN ] );
}

sal_Bool CommandLineArgs::WantsToLoadDocument() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_bDocumentArgs;
}

} // namespace desktop
