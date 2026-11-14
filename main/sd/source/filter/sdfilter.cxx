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
#include "precompiled_sd.hxx"
#include <com/sun/star/task/XStatusIndicatorFactory.hpp>

#include <memory>
#include <set>

#include <tools/debug.hxx>
#include <osl/module.hxx>
#include <osl/mutex.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/progress.hxx>
#include <svl/itemset.hxx>

#include "../ui/inc/DrawDocShell.hxx"
#include "../ui/inc/strings.hrc"

#include "sdresid.hxx"
#include "pres.hxx"
#include "drawdoc.hxx"
#include "sdfilter.hxx"

// --------------
// - Namespaces -
// --------------

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::frame;

// ------------
// - SdFilter -
// ------------

SdFilter::SdFilter( SfxMedium& rMedium, ::sd::DrawDocShell& rDocShell, sal_Bool	bShowProgress )
:	mxModel( rDocShell.GetModel() )
,	mrMedium( rMedium )
,	mrDocShell( rDocShell )
,	mrDocument( *rDocShell.GetDoc() )
,	mbIsDraw( rDocShell.GetDocumentType() == DOCUMENT_TYPE_DRAW )
,	mbShowProgress( bShowProgress )
{
}

// -----------------------------------------------------------------------------

SdFilter::~SdFilter()
{
}

// -----------------------------------------------------------------------------

::rtl::OUString SdFilter::ImplGetFullLibraryName( const ::rtl::OUString& rLibraryName ) const
{
	String aTemp( ::rtl::OUString::createFromAscii( SVLIBRARY( "?" ) ) );
	xub_StrLen nIndex = aTemp.Search( (sal_Unicode)'?' );
	aTemp.Replace( nIndex, 1, rLibraryName );
	::rtl::OUString aLibraryName( aTemp );
	return aLibraryName;
}

// -----------------------------------------------------------------------------

extern "C" { static void SAL_CALL thisModule() {} }

::osl::Module* SdFilter::OpenLibrary( const ::rtl::OUString& rLibraryName ) const
{
    const ::rtl::OUString aFullName( ImplGetFullLibraryName( rLibraryName ) );
    std::unique_ptr< osl::Module > mod(new osl::Module);
    if ( !mod->loadRelative( &thisModule, aFullName ) )
        return 0;

    // Callers delete the returned Module, which unloads the library.  But an
    // import leaves objects in the document whose virtual-function tables live
    // in the filter library or in a library it pulls in (sdfilt -> msfilter):
    // once that is unloaded, the next virtual call on such an object jumps into
    // unmapped memory.  Opening a .ppt as the first document crashed that way
    // in SdrObjListIter right after the import; with any document opened
    // before, msfilter was still held by another component and survived.
    // Take one extra reference per library and never release it, so a filter
    // library stays loaded for the rest of the process once it has been used.
    static std::set< ::rtl::OUString > aPinned;
    ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if ( aPinned.insert( aFullName ).second )
        osl_loadModuleRelative( &thisModule, aFullName.pData, SAL_LOADMODULE_DEFAULT );

    return mod.release();
}

// -----------------------------------------------------------------------------

void SdFilter::CreateStatusIndicator()
{
	// The status indicator must be retrieved from the provided medium arguments
	const SfxUnoAnyItem* pStatusBarItem = static_cast<const SfxUnoAnyItem*>(
			mrMedium.GetItemSet()->GetItem(SID_PROGRESS_STATUSBAR_CONTROL) );

	if ( pStatusBarItem )
		pStatusBarItem->GetValue() >>= mxStatusIndicator;

//	try
//	{
//		if (mxModel.is())
//		{
//			Reference< XController > xController( mxModel->getCurrentController());
//			if( xController.is())
//			{
//				Reference< XFrame > xFrame( xController->getFrame());
//				if( xFrame.is())
//				{
//					Reference< XStatusIndicatorFactory > xFactory( xFrame, UNO_QUERY );
//					if( xFactory.is())
//					{
//						mxStatusIndicator = xFactory->createStatusIndicator();
//					}
//				}
//			}
//		}
//	}
//	catch( Exception& )
//	{
//	}
}
