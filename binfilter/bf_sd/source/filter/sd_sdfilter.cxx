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



#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATORFACTORY_HPP_
#include <com/sun/star/task/XStatusIndicatorFactory.hpp>
#endif

#include <tools/debug.hxx>
#include <osl/file.hxx>
#include <vos/module.hxx>
#include <bf_svtools/pathoptions.hxx>
#include <bf_sfx2/docfile.hxx>
#include <bf_sfx2/progress.hxx>

#include "bf_sd/docshell.hxx"

#include "sdresid.hxx"
#include "pres.hxx"
#include "drawdoc.hxx"
#include "sdfilter.hxx"
#include "glob.hrc"

namespace binfilter {

// --------------
// - Namespaces -
// --------------

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::frame;

// ------------
// - SdFilter -
// ------------

SdFilter::SdFilter( SfxMedium& rMedium, SdDrawDocShell& rDocShell, sal_Bool	bShowProgress ) :
	mrMedium( rMedium ),
	mrDocShell( rDocShell ),
	mrDocument( *rDocShell.GetDoc() ),
	mxModel( rDocShell.GetModel() ),
	mpProgress( NULL ),
	mbIsDraw( rDocShell.GetDocumentType() == DOCUMENT_TYPE_DRAW ),
	mbShowProgress( bShowProgress )
{
}

// -----------------------------------------------------------------------------

SdFilter::~SdFilter()
{
	delete mpProgress;
}

// -----------------------------------------------------------------------------

void SdFilter::CreateStatusIndicator()
{
	try
	{
		if (mxModel.is())
		{
			Reference< XController > xController( mxModel->getCurrentController());
			if( xController.is())
			{
				Reference< XFrame > xFrame( xController->getFrame());
				if( xFrame.is())
				{
					Reference< XStatusIndicatorFactory > xFactory( xFrame, UNO_QUERY );
					if( xFactory.is())
					{
						mxStatusIndicator = xFactory->createStatusIndicator();
					}
				}
			}
		}
	}
	catch( Exception& )
	{
	}
}

// -----------------------------------------------------------------------------

void SdFilter::CreateProgress()
{
	mpProgress = new SfxProgress( &mrDocShell, String( SdResId( STR_LOAD_DOC ) ), 100 );
	mpProgress->SetState( 0, 100 );
}

// -----------------------------------------------------------------------------

sal_Bool SdFilter::Import()
{
	DBG_ERROR( "Not implemented" );
	return sal_False;
}

// -----------------------------------------------------------------------------

sal_Bool SdFilter::Export()
{
	DBG_ERROR( "Not implemented" );
	return sal_False;
}
}
