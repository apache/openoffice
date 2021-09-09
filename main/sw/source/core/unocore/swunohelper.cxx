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
#include "precompiled_sw.hxx"

#define _SVSTDARR_STRINGS
#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/ucb/XContentIdentifier.hpp>
#include <com/sun/star/ucb/XContentProvider.hpp>
#include <com/sun/star/ucb/XCommandEnvironment.hpp>
#include <com/sun/star/ucb/TransferInfo.hpp>
#include <com/sun/star/ucb/NameClash.hdl>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <comphelper/processfactory.hxx>
#include <comphelper/types.hxx>
#include <tools/urlobj.hxx>
#include <tools/datetime.hxx>
#include <tools/debug.hxx>
#include <ucbhelper/contentidentifier.hxx>
#include <ucbhelper/contentbroker.hxx>
#include <ucbhelper/content.hxx>
#include <svl/svstdarr.hxx>
#include <swunohelper.hxx>
#include <swunodef.hxx>
#include <errhdl.hxx>

//UUUU
#include <svx/xfillit0.hxx>
#include <svl/itemset.hxx>

namespace SWUnoHelper
{

sal_Int32 GetEnumAsInt32( const UNO_NMSPC::Any& rVal )
{
	sal_Int32 eVal;
	try
	{
		eVal = comphelper::getEnumAsINT32( rVal );
	}
	catch( UNO_NMSPC::Exception & )
	{
		eVal = 0;
		ASSERT( sal_False, "can't get EnumAsInt32" );
	}
	return eVal;
}


// methods for UCB actions
sal_Bool UCB_DeleteFile( const String& rURL )
{
	sal_Bool bRemoved;
	try
	{
		ucbhelper::Content aTempContent( rURL,
								STAR_REFERENCE( ucb::XCommandEnvironment )());
		aTempContent.executeCommand(
						rtl::OUString::createFromAscii( "delete" ),
						UNO_NMSPC::makeAny( sal_Bool( sal_True ) ) );
		bRemoved = sal_True;
	}
	catch( UNO_NMSPC::Exception& )
	{
		bRemoved = sal_False;
		ASSERT( sal_False, "Exception from executeCommand( delete )" );
	}
	return bRemoved;
}

sal_Bool UCB_CopyFile( const String& rURL, const String& rNewURL, sal_Bool bCopyIsMove )
{
	sal_Bool bCopyCompleted = sal_True;
	try
	{
		INetURLObject aURL( rNewURL );
		String sName( aURL.GetName() );
		aURL.removeSegment();
		String sMainURL( aURL.GetMainURL(INetURLObject::NO_DECODE) );

		ucbhelper::Content aTempContent( sMainURL,
								STAR_REFERENCE( ucb::XCommandEnvironment )());

		UNO_NMSPC::Any aAny;
		STAR_NMSPC::ucb::TransferInfo aInfo;
		aInfo.NameClash = STAR_NMSPC::ucb::NameClash::ERROR;
		aInfo.NewTitle = sName;
		aInfo.SourceURL = rURL;
		aInfo.MoveData = bCopyIsMove;
		aAny <<= aInfo;
		aTempContent.executeCommand(
							rtl::OUString::createFromAscii( "transfer" ),
                        	aAny );
	}
	catch( UNO_NMSPC::Exception& )
	{
		ASSERT( sal_False, "Exception from executeCommand( transfer )" );
		bCopyCompleted = sal_False;
	}
	return bCopyCompleted;
}

sal_Bool UCB_IsCaseSensitiveFileName( const String& rURL )
{
	sal_Bool bCaseSensitive;
	try
	{
		STAR_REFERENCE( lang::XMultiServiceFactory ) xMSF =
									comphelper::getProcessServiceFactory();

		INetURLObject aTempObj( rURL );
		aTempObj.SetBase( aTempObj.GetBase().toAsciiLowerCase() );
		STAR_REFERENCE( ucb::XContentIdentifier ) xRef1 = new
				ucbhelper::ContentIdentifier( xMSF,
							aTempObj.GetMainURL( INetURLObject::NO_DECODE ));

		aTempObj.SetBase(aTempObj.GetBase().toAsciiUpperCase());
		STAR_REFERENCE( ucb::XContentIdentifier ) xRef2 = new
				ucbhelper::ContentIdentifier( xMSF,
							aTempObj.GetMainURL( INetURLObject::NO_DECODE ));

		STAR_REFERENCE( ucb::XContentProvider ) xProv =
				ucbhelper::ContentBroker::get()->getContentProviderInterface();

		sal_Int32 nCompare = xProv->compareContentIds( xRef1, xRef2 );
		bCaseSensitive = 0 != nCompare;
	}
	catch( UNO_NMSPC::Exception& )
	{
		bCaseSensitive = sal_False;
		ASSERT( sal_False, "Exception from compareContentIds()" );
	}
	return bCaseSensitive;
}

sal_Bool UCB_IsReadOnlyFileName( const String& rURL )
{
	sal_Bool bIsReadOnly = sal_False;
	try
	{
		ucbhelper::Content aCnt( rURL, STAR_REFERENCE( ucb::XCommandEnvironment )());
		UNO_NMSPC::Any aAny = aCnt.getPropertyValue(
							rtl::OUString::createFromAscii( "IsReadOnly" ));
		if(aAny.hasValue())
			bIsReadOnly = *(sal_Bool*)aAny.getValue();
	}
	catch( UNO_NMSPC::Exception& )
	{
		bIsReadOnly = sal_False;
	}
	return bIsReadOnly;
}

sal_Bool UCB_IsFile( const String& rURL )
{
	sal_Bool bExists = sal_False;
	try
	{
		::ucbhelper::Content aContent( rURL, STAR_REFERENCE( ucb::XCommandEnvironment )() );
		bExists = aContent.isDocument();
	}
	catch (UNO_NMSPC::Exception &)
	{
	}
	return bExists;
}

sal_Bool UCB_IsDirectory( const String& rURL )
{
	sal_Bool bExists = sal_False;
	try
	{
		::ucbhelper::Content aContent( rURL, STAR_REFERENCE( ucb::XCommandEnvironment )() );
		bExists = aContent.isFolder();
	}
	catch (UNO_NMSPC::Exception &)
	{
	}
	return bExists;
}

	// get a list of files from the folder of the URL
	// options: pExtension = 0 -> all, else this specific extension
	// 			pDateTime != 0 -> returns also the modified date/time of
	//                       the files in a SvPtrarr -->
	//                       !! objects must be deleted from the caller!!
sal_Bool UCB_GetFileListOfFolder( const String& rURL, SvStrings& rList,
								const String* pExtension,
								SvPtrarr* pDateTimeList )
{
	sal_Bool bOk = sal_False;
	try
	{
		ucbhelper::Content aCnt( rURL, STAR_REFERENCE( ucb::XCommandEnvironment )());
		STAR_REFERENCE( sdbc::XResultSet ) xResultSet;

		sal_uInt16 nSeqSize = pDateTimeList ? 2 : 1;
		UNO_NMSPC::Sequence < rtl::OUString > aProps( nSeqSize );
		rtl::OUString* pProps = aProps.getArray();
		pProps[ 0 ] = rtl::OUString::createFromAscii( "Title" );
		if( pDateTimeList )
			pProps[ 1 ] = rtl::OUString::createFromAscii( "DateModified" );

		try
		{
			xResultSet = aCnt.createCursor( aProps, ::ucbhelper::INCLUDE_DOCUMENTS_ONLY );
		}
		catch( UNO_NMSPC::Exception& )
		{
			DBG_ERRORFILE( "create cursor failed!" );
		}

		if( xResultSet.is() )
		{
			STAR_REFERENCE( sdbc::XRow ) xRow( xResultSet, UNO_NMSPC::UNO_QUERY );
			xub_StrLen nExtLen = pExtension ? pExtension->Len() : 0;
        	try
        	{
            	if( xResultSet->first() )
            	{
                	do {
						String sTitle( xRow->getString( 1 ) );
						if( !nExtLen ||
							( sTitle.Len() > nExtLen &&
							  sTitle.Equals( *pExtension,
							  			sTitle.Len() - nExtLen, nExtLen )) )
						{
							String* pStr = new String( sTitle );
							rList.Insert( pStr, rList.Count() );

							if( pDateTimeList )
							{
								STAR_NMSPC::util::DateTime aStamp = xRow->getTimestamp(2);
								::DateTime* pDateTime = new ::DateTime(
										::Date( aStamp.Day,
												aStamp.Month,
												aStamp.Year ),
										::Time( aStamp.Hours,
												aStamp.Minutes,
												aStamp.Seconds,
												aStamp.HundredthSeconds ));
								void* p = pDateTime;
								pDateTimeList->Insert( p,
													pDateTimeList->Count() );
							}
						}

					} while( xResultSet->next() );
				}
				bOk = sal_True;
			}
            catch( UNO_NMSPC::Exception& )
            {
                DBG_ERRORFILE( "Exception caught!" );
            }
		}
	}
	catch( UNO_NMSPC::Exception& )
	{
		DBG_ERRORFILE( "Exception caught!" );
		bOk = sal_False;
	}
	return bOk;
}

//UUUU
bool needToMapFillItemsToSvxBrushItemTypes(const SfxItemSet& rSet)
{
    const XFillStyleItem* pXFillStyleItem(static_cast< const XFillStyleItem*  >(rSet.GetItem(XATTR_FILLSTYLE, false)));

	if(!pXFillStyleItem)
	{
		return false;
	}

	// here different FillStyles can be excluded for export; it will depend on the
	// quality these fallbacks can reach. That again is done in getSvxBrushItemFromSourceSet,
	// take a look there how the superset of DrawObject FillStyles is mapped to SvxBrushItem.
	// For now, take them all - except XFILL_NONE

	if(XFILL_NONE != pXFillStyleItem->GetValue())
	{
		return true;
	}

	// if(XFILL_SOLID == pXFillStyleItem->GetValue() || XFILL_BITMAP == pXFillStyleItem->GetValue())
	// {
	//     return true;
	// }

	return false;
}

}

// eof
