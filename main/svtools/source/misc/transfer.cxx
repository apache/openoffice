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
#include "precompiled_svtools.hxx"
#ifdef WNT
#include <tools/prewin.h>
#if defined _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable: 4917)
#endif
#include <shlobj.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif
#include <tools/postwin.h>
#endif
#include <vos/mutex.hxx>
#include <rtl/memory.h>
#include <rtl/uuid.h>
#include <rtl/uri.hxx>
#include <tools/debug.hxx>
#include <tools/urlobj.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <sot/exchange.hxx>
#include <sot/storage.hxx>
#include <vcl/bitmap.hxx>
#include <vcl/gdimtf.hxx>
#include <vcl/graph.hxx>
#include <vcl/cvtgrf.hxx>
#include <vcl/svapp.hxx>
#include <vcl/window.hxx>
#include <comphelper/processfactory.hxx>
#include <sot/filelist.hxx>
#include <cppuhelper/implbase1.hxx>

#include <comphelper/seqstream.hxx>
#include <com/sun/star/datatransfer/clipboard/XClipboardNotifier.hpp>
#include <com/sun/star/datatransfer/clipboard/XFlushableClipboard.hpp>
#include <com/sun/star/datatransfer/XMimeContentTypeFactory.hpp>
#include <com/sun/star/datatransfer/XMimeContentType.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/lang/XInitialization.hpp>

#include "svl/urlbmk.hxx"
#include "inetimg.hxx"
#include <svtools/wmf.hxx>
#include <svtools/imap.hxx>
#include <svtools/transfer.hxx>
#include <cstdio>
#include <vcl/dibtools.hxx>
#include <vcl/pngread.hxx>
#include <vcl/pngwrite.hxx>

// --------------
// - Namespaces -
// --------------

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::datatransfer;
using namespace ::com::sun::star::datatransfer::clipboard;
using namespace ::com::sun::star::datatransfer::dnd;

// --------------------------------
// - TransferableObjectDescriptor -
// --------------------------------

#define TOD_SIG1 0x01234567
#define TOD_SIG2 0x89abcdef

SvStream& operator>>( SvStream& rIStm, TransferableObjectDescriptor& rObjDesc )
{
	sal_uInt32  nSize, nViewAspect, nSig1, nSig2;

	rIStm >> nSize;
	rIStm >> rObjDesc.maClassName;
	rIStm >> nViewAspect;
	rIStm >> rObjDesc.maSize.Width();
    rIStm >> rObjDesc.maSize.Height();
	rIStm >> rObjDesc.maDragStartPos.X();
	rIStm >> rObjDesc.maDragStartPos.Y();
	rIStm.ReadByteString( rObjDesc.maTypeName, gsl_getSystemTextEncoding() );
	rIStm.ReadByteString( rObjDesc.maDisplayName, gsl_getSystemTextEncoding() );
    rIStm >> nSig1 >> nSig2;

    rObjDesc.mnViewAspect = static_cast< sal_uInt16 >( nViewAspect );

    // don't use width/height info from external objects
    if( ( TOD_SIG1 != nSig1 ) || ( TOD_SIG2 != nSig2 ) )
    {
        rObjDesc.maSize.Width() = 0;
        rObjDesc.maSize.Height() = 0;
    }

	return rIStm;
}

// -----------------------------------------------------------------------------

SvStream& operator<<( SvStream& rOStm, const TransferableObjectDescriptor& rObjDesc )
{
	const sal_uInt32    nFirstPos = rOStm.Tell(), nViewAspect = rObjDesc.mnViewAspect;
	const sal_uInt32    nSig1 = TOD_SIG1, nSig2 = TOD_SIG2;

	rOStm.SeekRel( 4 );
	rOStm << rObjDesc.maClassName;
	rOStm << nViewAspect;
	rOStm << rObjDesc.maSize.Width();
	rOStm << rObjDesc.maSize.Height();
	rOStm << rObjDesc.maDragStartPos.X();
	rOStm << rObjDesc.maDragStartPos.Y();
	rOStm.WriteByteString( rObjDesc.maTypeName, gsl_getSystemTextEncoding() );
	rOStm.WriteByteString( rObjDesc.maDisplayName, gsl_getSystemTextEncoding() );
	rOStm << nSig1 << nSig2;

	const sal_uInt32 nLastPos = rOStm.Tell();

	rOStm.Seek( nFirstPos );
	rOStm << ( nLastPos - nFirstPos );
	rOStm.Seek( nLastPos );

	return rOStm;
}

// -----------------------------------------------------------------------------
// the reading of the parameter is done using the special service ::com::sun::star::datatransfer::MimeContentType,
// a similar approach should be implemented for creation of the mimetype string;
// for now the set of acceptable characters has to be hardcoded, in future it should be part of the service that creates the mimetype
const ::rtl::OUString aQuotedParamChars = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "()<>@,;:/[]?=!#$&'*+-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ^_`abcdefghijklmnopqrstuvwxyz{|}~. " ) );

static ::rtl::OUString ImplGetParameterString( const TransferableObjectDescriptor& rObjDesc )
{
    const ::rtl::OUString   aChar( ::rtl::OUString::createFromAscii( "\"" ) );
    const ::rtl::OUString   aClassName( rObjDesc.maClassName.GetHexName() );
    ::rtl::OUString         aParams;

    if( aClassName.getLength() )
    {
        aParams += ::rtl::OUString::createFromAscii( ";classname=\"" );
        aParams += aClassName;
        aParams += aChar;
    }

    if( rObjDesc.maTypeName.Len() )
    {
        aParams += ::rtl::OUString::createFromAscii( ";typename=\"" );
        aParams += rObjDesc.maTypeName;
        aParams += aChar;
    }

    if( rObjDesc.maDisplayName.Len() )
    {
        // the display name might contain unacceptable characters, encode all of them
        // this seems to be the only parameter currently that might contain such characters
        sal_Bool pToAccept[128];
        for ( sal_Int32 nBInd = 0; nBInd < 128; nBInd++ )
            pToAccept[nBInd] = sal_False;

        for ( sal_Int32 nInd = 0; nInd < aQuotedParamChars.getLength(); nInd++ )
        {
            sal_Unicode nChar = aQuotedParamChars.getStr()[nInd];
            if ( nChar < 128 )
                pToAccept[nChar] = sal_True;
        }

        aParams += ::rtl::OUString::createFromAscii( ";displayname=\"" );
        aParams += ::rtl::Uri::encode( rObjDesc.maDisplayName, pToAccept, rtl_UriEncodeIgnoreEscapes, RTL_TEXTENCODING_UTF8 );
        aParams += aChar;
    }

    aParams += ::rtl::OUString::createFromAscii( ";viewaspect=\"" );
    aParams += ::rtl::OUString::valueOf( static_cast< sal_Int32 >( rObjDesc.mnViewAspect ) );
    aParams += aChar;

    aParams += ::rtl::OUString::createFromAscii( ";width=\"" );
    aParams += ::rtl::OUString::valueOf( rObjDesc.maSize.Width() );
    aParams += aChar;

    aParams += ::rtl::OUString::createFromAscii( ";height=\"" );
    aParams += ::rtl::OUString::valueOf( rObjDesc.maSize.Height() );
    aParams += aChar;

    aParams += ::rtl::OUString::createFromAscii( ";posx=\"" );
    aParams += ::rtl::OUString::valueOf( rObjDesc.maDragStartPos.X() );
    aParams += aChar;

    aParams += ::rtl::OUString::createFromAscii( ";posy=\"" );
    aParams += ::rtl::OUString::valueOf( rObjDesc.maDragStartPos.X() );
    aParams += aChar;

    return aParams;
}

// -----------------------------------------------------------------------------

static void ImplSetParameterString( TransferableObjectDescriptor& rObjDesc, const DataFlavorEx& rFlavorEx )
{
    Reference< XMultiServiceFactory >       xFact( ::comphelper::getProcessServiceFactory() );
    Reference< XMimeContentTypeFactory >    xMimeFact;

    try
    {
        if( xFact.is() )
        {
            xMimeFact = Reference< XMimeContentTypeFactory >( xFact->createInstance( ::rtl::OUString::createFromAscii(
                                                              "com.sun.star.datatransfer.MimeContentTypeFactory" ) ),
                                                              UNO_QUERY );
        }

        if( xMimeFact.is() )
        {
            Reference< XMimeContentType > xMimeType( xMimeFact->createMimeContentType( rFlavorEx.MimeType ) );

            if( xMimeType.is() )
            {
                const ::rtl::OUString aClassNameString( ::rtl::OUString::createFromAscii( "classname" ) );
                const ::rtl::OUString aTypeNameString( ::rtl::OUString::createFromAscii( "typename" ) );
                const ::rtl::OUString aDisplayNameString( ::rtl::OUString::createFromAscii( "displayname" ) );
                const ::rtl::OUString aViewAspectString( ::rtl::OUString::createFromAscii( "viewaspect" ) );
                const ::rtl::OUString aWidthString( ::rtl::OUString::createFromAscii( "width" ) );
                const ::rtl::OUString aHeightString( ::rtl::OUString::createFromAscii( "height" ) );
                const ::rtl::OUString aPosXString( ::rtl::OUString::createFromAscii( "posx" ) );
                const ::rtl::OUString aPosYString( ::rtl::OUString::createFromAscii( "posy" ) );

                if( xMimeType->hasParameter( aClassNameString ) )
                {
                    rObjDesc.maClassName.MakeId( xMimeType->getParameterValue( aClassNameString ) );
                }

                if( xMimeType->hasParameter( aTypeNameString ) )
                {
                    rObjDesc.maTypeName = xMimeType->getParameterValue( aTypeNameString );
                }

                if( xMimeType->hasParameter( aDisplayNameString ) )
                {
                    // the display name might contain unacceptable characters, in this case they should be encoded
                    // this seems to be the only parameter currently that might contain such characters
                    rObjDesc.maDisplayName = ::rtl::Uri::decode( xMimeType->getParameterValue( aDisplayNameString ), rtl_UriDecodeWithCharset, RTL_TEXTENCODING_UTF8 );
                }

                if( xMimeType->hasParameter( aViewAspectString ) )
                {
                    rObjDesc.mnViewAspect = static_cast< sal_uInt16 >( xMimeType->getParameterValue( aViewAspectString ).toInt32() );
                }

                if( xMimeType->hasParameter( aWidthString ) )
                {
                    rObjDesc.maSize.Width() = xMimeType->getParameterValue( aWidthString ).toInt32();
                }

                if( xMimeType->hasParameter( aHeightString ) )
                {
                    rObjDesc.maSize.Height() = xMimeType->getParameterValue( aHeightString ).toInt32();
                }

                if( xMimeType->hasParameter( aPosXString ) )
                {
                    rObjDesc.maDragStartPos.X() = xMimeType->getParameterValue( aPosXString ).toInt32();
                }

                if( xMimeType->hasParameter( aPosYString ) )
                {
                    rObjDesc.maDragStartPos.Y() = xMimeType->getParameterValue( aPosYString ).toInt32();
                }
            }
        }
    }
    catch( const ::com::sun::star::uno::Exception& )
    {
    }
}

// -----------------------------------------
// - TransferableHelper::TerminateListener -
// -----------------------------------------

TransferableHelper::TerminateListener::TerminateListener( TransferableHelper& rTransferableHelper ) :
	mrParent( rTransferableHelper )
{
}

// -----------------------------------------------------------------------------

TransferableHelper::TerminateListener::~TerminateListener()
{
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableHelper::TerminateListener::disposing( const EventObject& ) throw( RuntimeException )
{
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableHelper::TerminateListener::queryTermination( const EventObject& ) throw( TerminationVetoException, RuntimeException )
{
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableHelper::TerminateListener::notifyTermination( const EventObject& ) throw( RuntimeException )
{
	mrParent.ImplFlush();
}

// ----------------------
// - TransferableHelper -
// ----------------------

TransferableHelper::TransferableHelper() :
	mpFormats( new DataFlavorExVector ),
    mpObjDesc( NULL )
{
}

// -----------------------------------------------------------------------------

TransferableHelper::~TransferableHelper()
{
    delete mpObjDesc;
	delete mpFormats;
}

// -----------------------------------------------------------------------------

Any SAL_CALL TransferableHelper::getTransferData( const DataFlavor& rFlavor ) throw( UnsupportedFlavorException, IOException, RuntimeException )
{
	if( !maAny.hasValue() || !mpFormats->size() || ( maLastFormat != rFlavor.MimeType ) )
	{
		const ::vos::OGuard aGuard( Application::GetSolarMutex() );

		maLastFormat = rFlavor.MimeType;
		maAny = Any();

		try
		{
    	    DataFlavor  aSubstFlavor;
            sal_Bool    bDone = sal_False;

			// add formats if not already done
            if( !mpFormats->size() )
				AddSupportedFormats();

	        // check alien formats first and try to get a substitution format
            if( SotExchange::GetFormatDataFlavor( FORMAT_STRING, aSubstFlavor ) &&
                TransferableDataHelper::IsEqual( aSubstFlavor, rFlavor ) )
            {
			    GetData( aSubstFlavor );
                bDone = maAny.hasValue();
            }
            else if(SotExchange::GetFormatDataFlavor(SOT_FORMATSTR_ID_BMP, aSubstFlavor ) 
                && TransferableDataHelper::IsEqual( aSubstFlavor, rFlavor )
                && SotExchange::GetFormatDataFlavor(FORMAT_BITMAP, aSubstFlavor))
            {
			    GetData( aSubstFlavor );
                bDone = sal_True;
            }
            else if( SotExchange::GetFormatDataFlavor( SOT_FORMATSTR_ID_EMF, aSubstFlavor ) &&
                     TransferableDataHelper::IsEqual( aSubstFlavor, rFlavor ) &&
                     SotExchange::GetFormatDataFlavor( FORMAT_GDIMETAFILE, aSubstFlavor ) )
            {
			    GetData( aSubstFlavor );

			    if( maAny.hasValue() )
			    {
				    Sequence< sal_Int8 > aSeq;

				    if( maAny >>= aSeq )
				    {
					    SvMemoryStream*	pSrcStm = new SvMemoryStream( (char*) aSeq.getConstArray(), aSeq.getLength(), STREAM_WRITE | STREAM_TRUNC );
					    GDIMetaFile		aMtf;

					    *pSrcStm >> aMtf;
					    delete pSrcStm;

					    Graphic			aGraphic( aMtf );
					    SvMemoryStream	aDstStm( 65535, 65535 );

					    if( GraphicConverter::Export( aDstStm, aGraphic, CVT_EMF ) == ERRCODE_NONE )
                        {
						    maAny <<= ( aSeq = Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( aDstStm.GetData() ),
                                                                     aDstStm.Seek( STREAM_SEEK_TO_END ) ) );
                            bDone = sal_True;
                        }
				    }
			    }
            }
            else if( SotExchange::GetFormatDataFlavor( SOT_FORMATSTR_ID_WMF, aSubstFlavor ) &&
                     TransferableDataHelper::IsEqual( aSubstFlavor, rFlavor ) &&
                     SotExchange::GetFormatDataFlavor( FORMAT_GDIMETAFILE, aSubstFlavor ) )
            {
			    GetData( aSubstFlavor );

			    if( maAny.hasValue() )
			    {
				    Sequence< sal_Int8 > aSeq;

				    if( maAny >>= aSeq )
				    {
					    SvMemoryStream*	pSrcStm = new SvMemoryStream( (char*) aSeq.getConstArray(), aSeq.getLength(), STREAM_WRITE | STREAM_TRUNC );
					    GDIMetaFile		aMtf;

					    *pSrcStm >> aMtf;
					    delete pSrcStm;

					    SvMemoryStream	aDstStm( 65535, 65535 );
						
						// taking wmf without file header
						if ( ConvertGDIMetaFileToWMF( aMtf, aDstStm, NULL, sal_False ) )
                        {
						    maAny <<= ( aSeq = Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( aDstStm.GetData() ),
                                                                     aDstStm.Seek( STREAM_SEEK_TO_END ) ) );
                            bDone = sal_True;
                        }
				    }
			    }
            }

            // reset Any if substitute doesn't work
            if( !bDone && maAny.hasValue() )
                maAny = Any();

            // if any is not yet filled, use standard format
            if( !maAny.hasValue() )
                GetData( rFlavor );

#ifdef DEBUG
            if( maAny.hasValue() && ::com::sun::star::uno::TypeClass_STRING != maAny.getValueType().getTypeClass() )
                fprintf( stderr, "TransferableHelper delivers sequence of data [ %s ]\n", ByteString( String( rFlavor.MimeType), RTL_TEXTENCODING_ASCII_US ).GetBuffer() );
#endif
        }
		catch( const ::com::sun::star::uno::Exception& )
		{
		}

		if( !maAny.hasValue() )
			throw UnsupportedFlavorException();
	}

	return maAny;
}

// -----------------------------------------------------------------------------

Sequence< DataFlavor > SAL_CALL TransferableHelper::getTransferDataFlavors() throw( RuntimeException )
{
	const ::vos::OGuard aGuard( Application::GetSolarMutex() );

	try
	{
		if( !mpFormats->size() )
			AddSupportedFormats();
	}
	catch( const ::com::sun::star::uno::Exception& )
	{
	}

	Sequence< DataFlavor >			aRet( mpFormats->size() );
	DataFlavorExVector::iterator	aIter( mpFormats->begin() ), aEnd( mpFormats->end() );
	sal_uInt32						nCurPos = 0;

    while( aIter != aEnd )
    {
    	aRet[ nCurPos++ ] = *aIter++;
    }

	return aRet;
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL TransferableHelper::isDataFlavorSupported( const DataFlavor& rFlavor ) throw( RuntimeException )
{
	const ::vos::OGuard aGuard( Application::GetSolarMutex() );
	sal_Bool			bRet = sal_False;

	try
	{
		if( !mpFormats->size() )
			AddSupportedFormats();
	}
	catch( const ::com::sun::star::uno::Exception& )
	{
	}

	DataFlavorExVector::iterator aIter( mpFormats->begin() ), aEnd( mpFormats->end() );

	while( aIter != aEnd )
	{
        if( TransferableDataHelper::IsEqual( *aIter, rFlavor ) )
        {
            aIter = aEnd;
            bRet = sal_True;
        }
        else
            aIter++;
    }

	return bRet;
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableHelper::lostOwnership( const Reference< XClipboard >&, const Reference< XTransferable >& ) throw( RuntimeException )
{
	const ::vos::OGuard aGuard( Application::GetSolarMutex() );

	try
	{
		if( mxTerminateListener.is() )
		{
			Reference< XMultiServiceFactory > xFact( ::comphelper::getProcessServiceFactory() );

			if( xFact.is() )
			{
				Reference< XDesktop > xDesktop( xFact->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.frame.Desktop" ) ), UNO_QUERY );

				if( xDesktop.is() )
					xDesktop->removeTerminateListener( mxTerminateListener );
			}

			mxTerminateListener = Reference< XTerminateListener >();
		}

		ObjectReleased();
	}
	catch( const ::com::sun::star::uno::Exception& )
	{
	}
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableHelper::disposing( const EventObject& ) throw( RuntimeException )
{
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableHelper::dragDropEnd( const DragSourceDropEvent& rDSDE ) throw( RuntimeException )
{
	const ::vos::OGuard aGuard( Application::GetSolarMutex() );

	try
	{
		DragFinished( rDSDE.DropSuccess ? ( rDSDE.DropAction & ~DNDConstants::ACTION_DEFAULT ) : DNDConstants::ACTION_NONE );
		ObjectReleased();
	}
	catch( const ::com::sun::star::uno::Exception& )
	{
	}
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableHelper::dragEnter( const DragSourceDragEvent& ) throw( RuntimeException )
{
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableHelper::dragExit( const DragSourceEvent& ) throw( RuntimeException )
{
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableHelper::dragOver( const DragSourceDragEvent& ) throw( RuntimeException )
{
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableHelper::dropActionChanged( const DragSourceDragEvent& ) throw( RuntimeException )
{
}

// -----------------------------------------------------------------------------

sal_Int64 SAL_CALL TransferableHelper::getSomething( const Sequence< sal_Int8 >& rId ) throw( RuntimeException )
{
    sal_Int64 nRet;

    if( ( rId.getLength() == 16 ) &&
        ( 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(), rId.getConstArray(), 16 ) ) )
    {
        nRet = sal::static_int_cast<sal_Int64>(reinterpret_cast<sal_IntPtr>(this));
    }
    else
        nRet = 0;

	return nRet;
}

// -----------------------------------------------------------------------------

void TransferableHelper::ImplFlush()
{
	if( mxClipboard.is() )
	{
		Reference< XFlushableClipboard >	xFlushableClipboard( mxClipboard, UNO_QUERY );
		const sal_uInt32					nRef = Application::ReleaseSolarMutex();

		try
		{
			if( xFlushableClipboard.is() )
			 	xFlushableClipboard->flushClipboard();
		}
		catch( const ::com::sun::star::uno::Exception& )
		{
			DBG_ERROR( "Could not flush clipboard" );
		}

		Application::AcquireSolarMutex( nRef );
	}
}

// -----------------------------------------------------------------------------

void TransferableHelper::AddFormat( SotFormatStringId nFormat )
{
    DataFlavor aFlavor;

    if( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) )
        AddFormat( aFlavor );
}

// -----------------------------------------------------------------------------

void TransferableHelper::AddFormat( const DataFlavor& rFlavor )
{
	DataFlavorExVector::iterator    aIter( mpFormats->begin() ), aEnd( mpFormats->end() );
    sal_Bool                        bAdd = sal_True;

	while( aIter != aEnd )
	{
        if( TransferableDataHelper::IsEqual( *aIter, rFlavor ) )
        {
            // update MimeType for SOT_FORMATSTR_ID_OBJECTDESCRIPTOR in every case
            if( ( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR == aIter->mnSotId ) && mpObjDesc )
            {
                DataFlavor aObjDescFlavor;

                SotExchange::GetFormatDataFlavor( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR, aObjDescFlavor );
                aIter->MimeType = aObjDescFlavor.MimeType;
                aIter->MimeType += ::ImplGetParameterString( *mpObjDesc );

#ifdef DEBUG
                fprintf( stderr, "TransferableHelper exchanged objectdescriptor [ %s ]\n",
                         ByteString( String( aIter->MimeType), RTL_TEXTENCODING_ASCII_US ).GetBuffer() );
#endif
            }

            aIter = aEnd;
            bAdd = sal_False;
        }
        else
            aIter++;
    }

    if( bAdd )
    {
    	DataFlavorEx   aFlavorEx;
        DataFlavor     aObjDescFlavor;

		aFlavorEx.MimeType = rFlavor.MimeType;
		aFlavorEx.HumanPresentableName = rFlavor.HumanPresentableName;
		aFlavorEx.DataType = rFlavor.DataType;
		aFlavorEx.mnSotId = SotExchange::RegisterFormat( rFlavor );

        if( ( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR == aFlavorEx.mnSotId ) && mpObjDesc )
            aFlavorEx.MimeType += ::ImplGetParameterString( *mpObjDesc );

		mpFormats->push_back( aFlavorEx );

		if( FORMAT_BITMAP == aFlavorEx.mnSotId )
		{
			AddFormat( SOT_FORMATSTR_ID_PNG );
			AddFormat( SOT_FORMATSTR_ID_BMP );
		}
		else if( FORMAT_GDIMETAFILE == aFlavorEx.mnSotId )
		{
			AddFormat( SOT_FORMATSTR_ID_EMF );
			AddFormat( SOT_FORMATSTR_ID_WMF );
		}
    }
}

// -----------------------------------------------------------------------------

void TransferableHelper::RemoveFormat( SotFormatStringId nFormat )
{
    DataFlavor aFlavor;

    if( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) )
        RemoveFormat( aFlavor );
}

// -----------------------------------------------------------------------------

void TransferableHelper::RemoveFormat( const DataFlavor& rFlavor )
{
	DataFlavorExVector::iterator aIter( mpFormats->begin() ), aEnd( mpFormats->end() );

	while( aIter != aEnd )
	{
		if( TransferableDataHelper::IsEqual( *aIter, rFlavor ) )
		{
			aIter = mpFormats->erase( aIter );
			aEnd = mpFormats->end();
		}
		else
			++aIter;
	}
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::HasFormat( SotFormatStringId nFormat )
{
	DataFlavorExVector::iterator	aIter( mpFormats->begin() ), aEnd( mpFormats->end() );
	sal_Bool						bRet = sal_False;

	while( aIter != aEnd )
	{
		if( nFormat == (*aIter).mnSotId )
		{
			aIter = aEnd;
			bRet = sal_True;
		}
		else
			++aIter;
	}

	return bRet;
}

// -----------------------------------------------------------------------------

void TransferableHelper::ClearFormats()
{
	mpFormats->clear();
    maAny.clear();
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetAny( const Any& rAny, const DataFlavor& )
{
	maAny = rAny;
	return( maAny.hasValue() );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetString( const ::rtl::OUString& rString, const DataFlavor& rFlavor )
{
	DataFlavor aFileFlavor;

	if( rString.getLength() &&
		SotExchange::GetFormatDataFlavor( FORMAT_FILE, aFileFlavor ) &&
		TransferableDataHelper::IsEqual( aFileFlavor, rFlavor ) )
	{
		const String			aString( rString );
		const ByteString		aByteStr( aString, gsl_getSystemTextEncoding() );
		Sequence< sal_Int8 >	aSeq( aByteStr.Len() + 1 );

		rtl_copyMemory( aSeq.getArray(), aByteStr.GetBuffer(), aByteStr.Len() );
		aSeq[ aByteStr.Len() ] = 0;
		maAny <<= aSeq;
	}
	else
		maAny <<= rString;

	return( maAny.hasValue() );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetBitmapEx( const BitmapEx& rBitmapEx, const DataFlavor& rFlavor )
{
	if( !rBitmapEx.IsEmpty() )
	{
        SvMemoryStream aMemStm( 65535, 65535 );

        if(rFlavor.MimeType.equalsIgnoreAsciiCase(::rtl::OUString::createFromAscii("image/png")))
        {
            // write a PNG
            ::vcl::PNGWriter aPNGWriter(rBitmapEx);

            aPNGWriter.Write(aMemStm);
        }
        else
        {
            const Bitmap aBitmap(rBitmapEx.GetBitmap());

            // #124085# take out DIBV5 for writing to the clipboard
            //if(rBitmapEx.IsTransparent())
            //{
            //    const Bitmap aMask(rBitmapEx.GetAlpha().GetBitmap());
            //
            //    // explicitly use Bitmap::Write with bCompressed = sal_False and bFileHeader = sal_True
            //    WriteDIBV5(aBitmap, aMask, aMemStm);
            //}
            //else
            //{
                // explicitly use Bitmap::Write with bCompressed = sal_False and bFileHeader = sal_True
                WriteDIB(aBitmap, aMemStm, false, true);
            //}
        }

        maAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( aMemStm.GetData() ), aMemStm.Seek( STREAM_SEEK_TO_END ) );
	}

	return( maAny.hasValue() );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetGDIMetaFile( const GDIMetaFile& rMtf, const DataFlavor& )
{
	if( rMtf.GetActionCount() )
	{
		SvMemoryStream aMemStm( 65535, 65535 );

		( (GDIMetaFile&) rMtf ).Write( aMemStm );
		maAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( aMemStm.GetData() ), aMemStm.Seek( STREAM_SEEK_TO_END ) );
	}

	return( maAny.hasValue() );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetGraphic( const Graphic& rGraphic, const DataFlavor& )
{
	if( rGraphic.GetType() != GRAPHIC_NONE )
	{
		SvMemoryStream aMemStm( 65535, 65535 );

        aMemStm.SetVersion( SOFFICE_FILEFORMAT_50 );
        aMemStm.SetCompressMode( COMPRESSMODE_NATIVE );
		aMemStm << rGraphic;
		maAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( aMemStm.GetData() ), aMemStm.Seek( STREAM_SEEK_TO_END ) );
	}

	return( maAny.hasValue() );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetImageMap( const ImageMap& rIMap, const ::com::sun::star::datatransfer::DataFlavor& )
{
	SvMemoryStream aMemStm( 8192, 8192 );

    aMemStm.SetVersion( SOFFICE_FILEFORMAT_50 );
    rIMap.Write( aMemStm, String() );
	maAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( aMemStm.GetData() ), aMemStm.Seek( STREAM_SEEK_TO_END ) );

	return( maAny.hasValue() );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetTransferableObjectDescriptor( const TransferableObjectDescriptor& rDesc,
															  const ::com::sun::star::datatransfer::DataFlavor& )
{
    PrepareOLE( rDesc );

	SvMemoryStream aMemStm( 1024, 1024 );

	aMemStm << rDesc;
	maAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( aMemStm.GetData() ), aMemStm.Tell() );

	return( maAny.hasValue() );
 }

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetINetBookmark( const INetBookmark& rBmk,
											  const ::com::sun::star::datatransfer::DataFlavor& rFlavor )
{
	rtl_TextEncoding eSysCSet = gsl_getSystemTextEncoding();

	switch( SotExchange::GetFormat( rFlavor ) )
	{
		case( SOT_FORMATSTR_ID_SOLK ):
		{
			ByteString sURL( rBmk.GetURL(), eSysCSet ),
					   sDesc( rBmk.GetDescription(), eSysCSet );
			ByteString sOut( ByteString::CreateFromInt32( sURL.Len() ));
			( sOut += '@' ) += sURL;
			sOut += ByteString::CreateFromInt32( sDesc.Len() );
			( sOut += '@' ) += sDesc;

			Sequence< sal_Int8 > aSeq( sOut.Len() );
			memcpy( aSeq.getArray(), sOut.GetBuffer(), sOut.Len() );
			maAny <<= aSeq;
		}
		break;

		case( FORMAT_STRING ):
			maAny <<= ::rtl::OUString( rBmk.GetURL() );
			break;

		case( SOT_FORMATSTR_ID_UNIFORMRESOURCELOCATOR ):
		{
			ByteString sURL( rBmk.GetURL(), eSysCSet );
			Sequence< sal_Int8 > aSeq( sURL.Len() );
			memcpy( aSeq.getArray(), sURL.GetBuffer(), sURL.Len() );
			maAny <<= aSeq;
		}
		break;

		case( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK ):
		{
			Sequence< sal_Int8 > aSeq( 2048 );

			memset( aSeq.getArray(), 0, 2048 );
			strcpy( reinterpret_cast< char* >( aSeq.getArray() ), ByteString( rBmk.GetURL(), eSysCSet).GetBuffer() );
			strcpy( reinterpret_cast< char* >( aSeq.getArray() ) + 1024, ByteString( rBmk.GetDescription(), eSysCSet ).GetBuffer() );

			maAny <<= aSeq;
		}
		break;

#ifdef WNT
		case SOT_FORMATSTR_ID_FILEGRPDESCRIPTOR:
		{
			Sequence< sal_Int8 >	aSeq( sizeof( FILEGROUPDESCRIPTOR ) );
			FILEGROUPDESCRIPTOR*	pFDesc = (FILEGROUPDESCRIPTOR*) aSeq.getArray();
			FILEDESCRIPTOR&			rFDesc1 = pFDesc->fgd[ 0 ];

			pFDesc->cItems = 1;
			memset( &rFDesc1, 0, sizeof( FILEDESCRIPTOR ) );
			rFDesc1.dwFlags = FD_LINKUI;

			ByteString aStr( rBmk.GetDescription(), eSysCSet );
			for( sal_uInt16 nChar = 0; nChar < aStr.Len(); ++nChar )
				if( strchr( "\\/:*?\"<>|", aStr.GetChar( nChar ) ) )
					aStr.Erase( nChar--, 1 );

			aStr.Insert( "Shortcut to ", 0 );
			aStr += ".URL";
			strcpy( rFDesc1.cFileName, aStr.GetBuffer() );

			maAny <<= aSeq;
		}
		break;

		case SOT_FORMATSTR_ID_FILECONTENT:
		{
			String aStr( RTL_CONSTASCII_STRINGPARAM( "[InternetShortcut]\x0aURL=" ) );
			maAny <<= ::rtl::OUString( aStr += rBmk.GetURL() );
		}
		break;
#endif

		default:
		break;
	}

	return( maAny.hasValue() );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetINetImage( const INetImage& rINtImg,
										   const ::com::sun::star::datatransfer::DataFlavor& rFlavor )
{
	SvMemoryStream aMemStm( 1024, 1024 );

    aMemStm.SetVersion( SOFFICE_FILEFORMAT_50 );
	rINtImg.Write( aMemStm, SotExchange::GetFormat( rFlavor ) );

	maAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( aMemStm.GetData() ), aMemStm.Seek( STREAM_SEEK_TO_END ) );

	return( maAny.hasValue() );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetFileList( const FileList& rFileList,
										  const ::com::sun::star::datatransfer::DataFlavor& )
{
	SvMemoryStream aMemStm( 4096, 4096 );

    aMemStm.SetVersion( SOFFICE_FILEFORMAT_50 );
	aMemStm << rFileList;

	maAny <<= Sequence< sal_Int8 >( static_cast< const sal_Int8* >( aMemStm.GetData() ),
                                       aMemStm.Seek( STREAM_SEEK_TO_END ) );

	return( maAny.hasValue() );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetObject( void* pUserObject, sal_uInt32 nUserObjectId, const DataFlavor& rFlavor )
{
	SotStorageStreamRef xStm( new SotStorageStream( String() ) );

    xStm->SetVersion( SOFFICE_FILEFORMAT_50 );

	if( pUserObject && WriteObject( xStm, pUserObject, nUserObjectId, rFlavor ) )
	{
		const sal_uInt32		nLen = xStm->Seek( STREAM_SEEK_TO_END );
		Sequence< sal_Int8 >	aSeq( nLen );

		xStm->Seek( STREAM_SEEK_TO_BEGIN );
		xStm->Read( aSeq.getArray(),  nLen );

		if( nLen && ( SotExchange::GetFormat( rFlavor ) == SOT_FORMAT_STRING ) )
        {
            //JP 24.7.2001: as I know was this only for the writer application and this
            //		        writes now UTF16 format into the stream
            //JP 6.8.2001:  and now it writes UTF8 because then exist no problem with
            //		        little / big endians! - Bug 88121
			maAny <<= ::rtl::OUString( reinterpret_cast< const sal_Char* >( aSeq.getConstArray() ), nLen - 1, RTL_TEXTENCODING_UTF8 );
        }
		else
			maAny <<= aSeq;
	}

	return( maAny.hasValue() );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::SetInterface( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rIf,
										   const ::com::sun::star::datatransfer::DataFlavor& )
{
	maAny <<= rIf;
	return( maAny.hasValue() );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableHelper::WriteObject( SotStorageStreamRef&, void*, sal_uInt32, const DataFlavor& )
{
	DBG_ERROR( "TransferableHelper::WriteObject( ... ) not implemented" );
	return sal_False;
}

// -----------------------------------------------------------------------------

void TransferableHelper::DragFinished( sal_Int8 )
{
}

// -----------------------------------------------------------------------------

void TransferableHelper::ObjectReleased()
{
}

// -----------------------------------------------------------------------------

void TransferableHelper::PrepareOLE( const TransferableObjectDescriptor& rObjDesc )
{
    delete mpObjDesc;
    mpObjDesc = new TransferableObjectDescriptor( rObjDesc );

    if( HasFormat( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR ) )
        AddFormat( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR );
}

// -----------------------------------------------------------------------------

void TransferableHelper::CopyToClipboard( Window *pWindow ) const
{
	DBG_ASSERT( pWindow, "Window pointer is NULL" );
    Reference< XClipboard > xClipboard;

    if( pWindow )
        xClipboard = pWindow->GetClipboard();

    if( xClipboard.is() )
        mxClipboard = xClipboard;

	if( mxClipboard.is() && !mxTerminateListener.is() )
	{
		const sal_uInt32 nRef = Application::ReleaseSolarMutex();

		try
		{
            TransferableHelper*                 pThis = const_cast< TransferableHelper* >( this );
			Reference< XMultiServiceFactory >   xFact( ::comphelper::getProcessServiceFactory() );

			if( xFact.is() )
			{
				Reference< XDesktop > xDesktop( xFact->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.frame.Desktop" ) ), UNO_QUERY );

				if( xDesktop.is() )
					xDesktop->addTerminateListener( pThis->mxTerminateListener = new TerminateListener( *pThis ) );
			}

			mxClipboard->setContents( pThis, pThis );
		}
		catch( const ::com::sun::star::uno::Exception& )
		{
		}

		Application::AcquireSolarMutex( nRef );
	}
}

// -----------------------------------------------------------------------------

void TransferableHelper::CopyToSelection( Window *pWindow ) const
{
	DBG_ASSERT( pWindow, "Window pointer is NULL" );
    Reference< XClipboard > xSelection;

    if( pWindow )
        xSelection = pWindow->GetPrimarySelection();

	if( xSelection.is() && !mxTerminateListener.is() )
	{
		const sal_uInt32 nRef = Application::ReleaseSolarMutex();

		try
		{
            TransferableHelper*                 pThis = const_cast< TransferableHelper* >( this );
			Reference< XMultiServiceFactory >   xFact( ::comphelper::getProcessServiceFactory() );

			if( xFact.is() )
			{
				Reference< XDesktop > xDesktop( xFact->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.frame.Desktop" ) ), UNO_QUERY );

				if( xDesktop.is() )
					xDesktop->addTerminateListener( pThis->mxTerminateListener = new TerminateListener( *pThis ) );
			}

			xSelection->setContents( pThis, pThis );
		}
		catch( const ::com::sun::star::uno::Exception& )
		{
		}

		Application::AcquireSolarMutex( nRef );
	}
}

// -----------------------------------------------------------------------------

void TransferableHelper::StartDrag( Window* pWindow, sal_Int8 nDnDSourceActions,
									sal_Int32 nDnDPointer, sal_Int32 nDnDImage )

{
	DBG_ASSERT( pWindow, "Window pointer is NULL" );
    Reference< XDragSource > xDragSource( pWindow->GetDragSource() );

	if( xDragSource.is() )
	{
        /*
         *    #96792# release mouse before actually starting DnD.
         *    This is necessary for the X11 DnD implementation to work.
         */
        if( pWindow->IsMouseCaptured() )
            pWindow->ReleaseMouse();

        const Point	aPt( pWindow->GetPointerPosPixel() );

		// On Mac OS X we are forced to execute 'startDrag' synchronously
		// contrary to the XDragSource interface specification because
		// we can receive drag events from the system only in the main 
		// thread
#if !defined(QUARTZ)
		const sal_uInt32 nRef = Application::ReleaseSolarMutex();
#endif 

		try
		{
			DragGestureEvent	aEvt;
			aEvt.DragAction = DNDConstants::ACTION_COPY;
			aEvt.DragOriginX = aPt.X();
			aEvt.DragOriginY = aPt.Y();
			aEvt.DragSource = xDragSource;

			xDragSource->startDrag( aEvt, nDnDSourceActions, nDnDPointer, nDnDImage, this, this );
		}
		catch( const ::com::sun::star::uno::Exception& )
		{
		}

		// See above for the reason of this define
#if !defined(QUARTZ)
		Application::AcquireSolarMutex( nRef );
#endif 
	}
}

// -----------------------------------------------------------------------------

void TransferableHelper::ClearSelection( Window *pWindow )
{
	DBG_ASSERT( pWindow, "Window pointer is NULL" );
    Reference< XClipboard > xSelection( pWindow->GetPrimarySelection() );

	if( xSelection.is() )
        xSelection->setContents( NULL, NULL );
}

// -----------------------------------------------------------------------------

Reference< XClipboard> TransferableHelper::GetSystemClipboard()
{
    Window *pFocusWindow = Application::GetFocusWindow();

    if( pFocusWindow )
        return pFocusWindow->GetClipboard();

	return 	Reference< XClipboard > ();
}

// -----------------------------------------------------------------------------

const Sequence< sal_Int8 >& TransferableHelper::getUnoTunnelId()
{
    static Sequence< sal_Int8 > aSeq;

	if( !aSeq.getLength() )
	{
		static osl::Mutex           aCreateMutex;
    	osl::Guard< osl::Mutex >    aGuard( aCreateMutex );

		aSeq.realloc( 16 );
    	rtl_createUuid( reinterpret_cast< sal_uInt8* >( aSeq.getArray() ), 0, sal_True );
	}


    return aSeq;
}

// ---------------------------------
// - TransferableClipboardNotifier -
// ---------------------------------

class TransferableClipboardNotifier : public ::cppu::WeakImplHelper1< XClipboardListener >
{
private:
    ::osl::Mutex&                   mrMutex;
    Reference< XClipboardNotifier > mxNotifier;
	TransferableDataHelper*		    mpListener;

protected:
	// XClipboardListener
    virtual void SAL_CALL changedContents( const clipboard::ClipboardEvent& event ) throw (RuntimeException);

    // XEventListener
    virtual void SAL_CALL disposing( const EventObject& Source ) throw (RuntimeException);

public:
    TransferableClipboardNotifier( const Reference< XClipboard >& _rxClipboard, TransferableDataHelper& _rListener, ::osl::Mutex& _rMutex );

    /// determines whether we're currently listening
    inline bool isListening() const { return !isDisposed(); }

    /// determines whether the instance is disposed
    inline bool isDisposed() const { return mpListener == NULL; }

    /// makes the instance non-functional
    void    dispose();
};

// -----------------------------------------------------------------------------

TransferableClipboardNotifier::TransferableClipboardNotifier( const Reference< XClipboard >& _rxClipboard, TransferableDataHelper& _rListener, ::osl::Mutex& _rMutex )
    :mrMutex( _rMutex )
    ,mxNotifier( _rxClipboard, UNO_QUERY )
    ,mpListener( &_rListener )
{
    osl_incrementInterlockedCount( &m_refCount );
    {
        if ( mxNotifier.is() )
		    mxNotifier->addClipboardListener( this );
        else
            // born dead
            mpListener = NULL;
    }
    osl_decrementInterlockedCount( &m_refCount );
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableClipboardNotifier::changedContents( const clipboard::ClipboardEvent& event ) throw (RuntimeException)
{
	::vos::OGuard aSolarGuard( Application::GetSolarMutex() );
        // the SolarMutex here is necessary, since
        // - we cannot call mpListener without our own mutex locked
        // - Rebind respectively InitFormats (called by Rebind) will
        // try to lock the SolarMutex, too
    ::osl::MutexGuard aGuard( mrMutex );
	if( mpListener )
		mpListener->Rebind( event.Contents );
}

// -----------------------------------------------------------------------------

void SAL_CALL TransferableClipboardNotifier::disposing( const EventObject& ) throw (RuntimeException)
{
    // clipboard is being disposed. Hmm. Okay, become disfunctional myself.
	dispose();
}

// -----------------------------------------------------------------------------

void TransferableClipboardNotifier::dispose()
{
    ::osl::MutexGuard aGuard( mrMutex );

    Reference< XClipboardListener > xKeepMeAlive( this );

    if ( mxNotifier.is() )
		mxNotifier->removeClipboardListener( this );
    mxNotifier.clear();

    mpListener = NULL;
}

// -------------------------------
// - TransferableDataHelper_Impl -
// -------------------------------

struct TransferableDataHelper_Impl
{
    ::osl::Mutex                    maMutex;
    TransferableClipboardNotifier*  mpClipboardListener;

    TransferableDataHelper_Impl()
        :mpClipboardListener( NULL )
    {
    }
};

// --------------------------
// - TransferableDataHelper -
// --------------------------

TransferableDataHelper::TransferableDataHelper() :
    mpFormats( new DataFlavorExVector ),
    mpObjDesc( new TransferableObjectDescriptor ),
    mpImpl( new TransferableDataHelper_Impl )
{
}

// -----------------------------------------------------------------------------

TransferableDataHelper::TransferableDataHelper( const Reference< ::com::sun::star::datatransfer::XTransferable >& rxTransferable ) :
    mxTransfer( rxTransferable ),
    mpFormats( new DataFlavorExVector ),
    mpObjDesc( new TransferableObjectDescriptor ),
    mpImpl( new TransferableDataHelper_Impl )
{
	InitFormats();
}

// -----------------------------------------------------------------------------

TransferableDataHelper::TransferableDataHelper( const TransferableDataHelper& rDataHelper ) :
    mxTransfer( rDataHelper.mxTransfer ),
    mxClipboard( rDataHelper.mxClipboard ),
	mpFormats( new DataFlavorExVector( *rDataHelper.mpFormats ) ),
    mpObjDesc( new TransferableObjectDescriptor( *rDataHelper.mpObjDesc ) ),
    mpImpl( new TransferableDataHelper_Impl )
{
}

// -----------------------------------------------------------------------------

TransferableDataHelper& TransferableDataHelper::operator=( const TransferableDataHelper& rDataHelper )
{
	if ( this != &rDataHelper )
	{
        ::osl::MutexGuard aGuard( mpImpl->maMutex );

        bool bWasClipboardListening = ( NULL != mpImpl->mpClipboardListener );

        if ( bWasClipboardListening )
            StopClipboardListening();

        mxTransfer = rDataHelper.mxTransfer;
		delete mpFormats, mpFormats = new DataFlavorExVector( *rDataHelper.mpFormats );
        delete mpObjDesc, mpObjDesc = new TransferableObjectDescriptor( *rDataHelper.mpObjDesc );
		mxClipboard = rDataHelper.mxClipboard;

        if ( bWasClipboardListening )
            StartClipboardListening();
	}

	return *this;
}

// -----------------------------------------------------------------------------

TransferableDataHelper::~TransferableDataHelper()
{
	StopClipboardListening( );
    {
        ::osl::MutexGuard aGuard( mpImpl->maMutex );
	    delete mpFormats, mpFormats = NULL;
        delete mpObjDesc, mpObjDesc = NULL;
    }
    delete mpImpl;
}

// -----------------------------------------------------------------------------

void TransferableDataHelper::FillDataFlavorExVector( const Sequence< DataFlavor >& rDataFlavorSeq,
                                                     DataFlavorExVector& rDataFlavorExVector )
{
    try
    {
	    Reference< XMultiServiceFactory >       xFact( ::comphelper::getProcessServiceFactory() );
        Reference< XMimeContentTypeFactory >    xMimeFact;
	    DataFlavorEx		                    aFlavorEx;
        const ::rtl::OUString                   aCharsetStr( ::rtl::OUString::createFromAscii( "charset" ) );

	    if( xFact.is() )
		    xMimeFact = Reference< XMimeContentTypeFactory >( xFact->createInstance( ::rtl::OUString::createFromAscii(
                                                              "com.sun.star.datatransfer.MimeContentTypeFactory" ) ),
                                                              UNO_QUERY );

	    for( sal_Int32 i = 0; i < rDataFlavorSeq.getLength(); i++ )
	    {
		    const DataFlavor&	            rFlavor = rDataFlavorSeq[ i ];
            Reference< XMimeContentType >   xMimeType;

            try
            {
                if( xMimeFact.is() && rFlavor.MimeType.getLength() )
                    xMimeType = xMimeFact->createMimeContentType( rFlavor.MimeType );
            }
            catch( const ::com::sun::star::uno::Exception& )
	        {

	        }

		    aFlavorEx.MimeType = rFlavor.MimeType;
		    aFlavorEx.HumanPresentableName = rFlavor.HumanPresentableName;
		    aFlavorEx.DataType = rFlavor.DataType;
		    aFlavorEx.mnSotId = SotExchange::RegisterFormat( rFlavor );

		    rDataFlavorExVector.push_back( aFlavorEx );

            // add additional formats for special mime types
            if(SOT_FORMATSTR_ID_BMP == aFlavorEx.mnSotId || SOT_FORMATSTR_ID_PNG == aFlavorEx.mnSotId)
		    {
			    if( SotExchange::GetFormatDataFlavor( SOT_FORMAT_BITMAP, aFlavorEx ) )
			    {
				    aFlavorEx.mnSotId = SOT_FORMAT_BITMAP;
				    rDataFlavorExVector.push_back( aFlavorEx );
			    }
		    }
            else if( SOT_FORMATSTR_ID_WMF == aFlavorEx.mnSotId || SOT_FORMATSTR_ID_EMF == aFlavorEx.mnSotId )
		    {
			    if( SotExchange::GetFormatDataFlavor( SOT_FORMAT_GDIMETAFILE, aFlavorEx ) )
			    {
				    aFlavorEx.mnSotId = SOT_FORMAT_GDIMETAFILE;
				    rDataFlavorExVector.push_back( aFlavorEx );
			    }
		    }
            else if ( SOT_FORMATSTR_ID_HTML_SIMPLE == aFlavorEx.mnSotId  )
            {
                // #104735# HTML_SIMPLE may also be inserted without comments
                aFlavorEx.mnSotId = SOT_FORMATSTR_ID_HTML_NO_COMMENT;
                rDataFlavorExVector.push_back( aFlavorEx );
            }
            else if( xMimeType.is() && xMimeType->getFullMediaType().equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "text/plain" ) ) )
            {
                // add, if it is a UTF-8 byte buffer
                if( xMimeType->hasParameter( aCharsetStr ) )
                {
                    const ::rtl::OUString aCharset( xMimeType->getParameterValue( aCharsetStr ) );

                    if( xMimeType->getParameterValue( aCharsetStr ).equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "unicode" ) ) ||
					    xMimeType->getParameterValue( aCharsetStr ).equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "utf-16" ) ) )
                    {
                        rDataFlavorExVector[ rDataFlavorExVector.size() - 1 ].mnSotId = FORMAT_STRING;

                    }
                }
            }
            else if( xMimeType.is() && xMimeType->getFullMediaType().equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "text/rtf" ) ) )
            {
                rDataFlavorExVector[ rDataFlavorExVector.size() - 1 ].mnSotId = FORMAT_RTF;
            }
            else if( xMimeType.is() && xMimeType->getFullMediaType().equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "text/html" ) ) )

            {
                rDataFlavorExVector[ rDataFlavorExVector.size() - 1 ].mnSotId = SOT_FORMATSTR_ID_HTML;
            }
            else if( xMimeType.is() && xMimeType->getFullMediaType().equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "text/uri-list" ) ) )
            {
                rDataFlavorExVector[ rDataFlavorExVector.size() - 1 ].mnSotId = SOT_FORMAT_FILE_LIST;
            }
            else if( xMimeType.is() && xMimeType->getFullMediaType().equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/x-openoffice-objectdescriptor-xml" ) ) )
            {
                rDataFlavorExVector[ rDataFlavorExVector.size() - 1 ].mnSotId = SOT_FORMATSTR_ID_OBJECTDESCRIPTOR;
            }
	    }
    }
	catch( const ::com::sun::star::uno::Exception& )
	{
	}
}

// -----------------------------------------------------------------------------

void TransferableDataHelper::InitFormats()
{
	::vos::OGuard aSolarGuard( Application::GetSolarMutex() );
    ::osl::MutexGuard aGuard( mpImpl->maMutex );

    mpFormats->clear();
    delete mpObjDesc, mpObjDesc = new TransferableObjectDescriptor;

    if( mxTransfer.is() )
    {
        TransferableDataHelper::FillDataFlavorExVector( mxTransfer->getTransferDataFlavors(), *mpFormats );

        DataFlavorExVector::iterator aIter( mpFormats->begin() ), aEnd( mpFormats->end() );

        while( aIter != aEnd )
        {
            if( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR == aIter->mnSotId )
            {
                ImplSetParameterString( *mpObjDesc, *aIter );
                aIter = aEnd;
            }
            else
                ++aIter;
        }
    }
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::HasFormat( SotFormatStringId nFormat ) const
{
    ::osl::MutexGuard aGuard( mpImpl->maMutex );

	DataFlavorExVector::iterator	aIter( mpFormats->begin() ), aEnd( mpFormats->end() );
	sal_Bool						bRet = sal_False;

	while( aIter != aEnd )
	{
		if( nFormat == (*aIter++).mnSotId )
		{
			aIter = aEnd;
			bRet = sal_True;
		}
	}

	return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::HasFormat( const DataFlavor& rFlavor ) const
{
    ::osl::MutexGuard aGuard( mpImpl->maMutex );

    DataFlavorExVector::iterator	aIter( mpFormats->begin() ), aEnd( mpFormats->end() );
	sal_Bool						bRet = sal_False;

	while( aIter != aEnd )
	{
		if( TransferableDataHelper::IsEqual( rFlavor, *aIter++ ) )
		{
			aIter = aEnd;
			bRet = sal_True;
		}
	}

	return bRet;
}

// -----------------------------------------------------------------------------

sal_uInt32 TransferableDataHelper::GetFormatCount() const
{
    ::osl::MutexGuard aGuard( mpImpl->maMutex );
	return mpFormats->size();
}

// -----------------------------------------------------------------------------


SotFormatStringId TransferableDataHelper::GetFormat( sal_uInt32 nFormat ) const
{
    ::osl::MutexGuard aGuard( mpImpl->maMutex );
	DBG_ASSERT( nFormat < mpFormats->size(), "TransferableDataHelper::GetFormat: invalid format index" );
	return( ( nFormat < mpFormats->size() ) ? (*mpFormats)[ nFormat ].mnSotId : 0 );
}

// -----------------------------------------------------------------------------

DataFlavor TransferableDataHelper::GetFormatDataFlavor( sal_uInt32 nFormat ) const
{
    ::osl::MutexGuard aGuard( mpImpl->maMutex );
	DBG_ASSERT( nFormat < mpFormats->size(), "TransferableDataHelper::GetFormat: invalid format index" );

    DataFlavor aRet;

	if( nFormat < mpFormats->size() )
		aRet = (*mpFormats)[ nFormat ];

	return aRet;
}

// -----------------------------------------------------------------------------

Reference< XTransferable > TransferableDataHelper::GetXTransferable() const
{
	Reference< XTransferable > xRet;

	if( mxTransfer.is() )
	{
		try
		{
			xRet = mxTransfer;

			// do a dummy call to check, if this interface is valid (nasty)
			Sequence< DataFlavor > aTestSeq( xRet->getTransferDataFlavors() );

		}
		catch( const ::com::sun::star::uno::Exception& )
		{
			xRet = Reference< XTransferable >();
		}
	}

	return xRet;
}

// -----------------------------------------------------------------------------

Any	TransferableDataHelper::GetAny( SotFormatStringId nFormat ) const
{
	Any aReturn;

	DataFlavor aFlavor;
	if ( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) )
		aReturn = GetAny( aFlavor );

	return aReturn;
}


// -----------------------------------------------------------------------------

Any TransferableDataHelper::GetAny( const DataFlavor& rFlavor ) const
{
    ::osl::MutexGuard aGuard( mpImpl->maMutex );
	Any aRet;

	try
	{
		if( mxTransfer.is() )
        {
            DataFlavorExVector::iterator    aIter( mpFormats->begin() ), aEnd( mpFormats->end() );
            const SotFormatStringId         nRequestFormat = SotExchange::GetFormat( rFlavor );

            if( nRequestFormat )
            {
    	        // try to get alien format first
	            while( aIter != aEnd )
	            {
                    if( ( nRequestFormat == (*aIter).mnSotId ) && !rFlavor.MimeType.equalsIgnoreAsciiCase( (*aIter).MimeType ) )
            			aRet = mxTransfer->getTransferData( *aIter );

                    if( aRet.hasValue() )
                        aIter = aEnd;
                    else
                        aIter++;
	            }
            }

            if( !aRet.hasValue() )
    			aRet = mxTransfer->getTransferData( rFlavor );
        }
	}
	catch( const ::com::sun::star::uno::Exception& )
	{
	}

	return aRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetString( SotFormatStringId nFormat, String& rStr )
{
    ::rtl::OUString aOUString;
    sal_Bool        bRet = GetString( nFormat, aOUString );

    rStr = aOUString;

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetString( const DataFlavor& rFlavor, String& rStr )
{
    ::rtl::OUString aOUString;
    sal_Bool        bRet = GetString( rFlavor, aOUString );

    rStr = aOUString;

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetString( SotFormatStringId nFormat, ::rtl::OUString& rStr )
{
	DataFlavor aFlavor;
	return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetString( aFlavor, rStr ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetString( const DataFlavor& rFlavor, ::rtl::OUString& rStr )
{
	Any         aAny( GetAny( rFlavor ) );
    sal_Bool    bRet = sal_False;

    if( aAny.hasValue() )
	{
		::rtl::OUString         aOUString;
		Sequence< sal_Int8 >    aSeq;

        if( aAny >>= aOUString )
        {
			rStr = aOUString;
            bRet = sal_True;
        }
		else if( aAny >>= aSeq )
		{

			const sal_Char* pChars = reinterpret_cast< const sal_Char* >( aSeq.getConstArray() );
			sal_Int32       nLen = aSeq.getLength();

            //JP 10.10.2001: 92930 - don't copy the last zero characterinto the string.
            //DVO 2002-05-27: strip _all_ trailing zeros
			while( nLen && ( 0 == *( pChars + nLen - 1 ) ) )
				--nLen;

			rStr = ::rtl::OUString( pChars, nLen, gsl_getSystemTextEncoding() );
            bRet = sal_True;
		}
	}

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetBitmapEx( SotFormatStringId nFormat, BitmapEx& rBmpEx )
{
    if(FORMAT_BITMAP == nFormat)
    {
        // try to get PNG first
        DataFlavor aFlavor;

        if(SotExchange::GetFormatDataFlavor(SOT_FORMATSTR_ID_PNG, aFlavor))
        {
            if(GetBitmapEx(aFlavor, rBmpEx))
            {
                return true;
            }
        }
    }

    DataFlavor aFlavor;
    return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetBitmapEx( aFlavor, rBmpEx ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetBitmapEx( const DataFlavor& rFlavor, BitmapEx& rBmpEx )
{
    SotStorageStreamRef xStm;
    DataFlavor aSubstFlavor;
    bool bRet(GetSotStorageStream(rFlavor, xStm));
    bool bSuppressPNG(false); // #122982# If PNG stream not accessed, but BMP one, suppress trying to load PNG

    if(!bRet && HasFormat(SOT_FORMATSTR_ID_PNG) && SotExchange::GetFormatDataFlavor(SOT_FORMATSTR_ID_PNG, aSubstFlavor))
    {
        // when no direct success, try if PNG is available
        bRet = GetSotStorageStream(aSubstFlavor, xStm);
    }

    if(!bRet && HasFormat(SOT_FORMATSTR_ID_BMP) && SotExchange::GetFormatDataFlavor(SOT_FORMATSTR_ID_BMP, aSubstFlavor))
    {
        // when no direct success, try if BMP is available
        bRet = GetSotStorageStream(aSubstFlavor, xStm);
        bSuppressPNG = bRet;
    }

    if(bRet)
    {
        if(!bSuppressPNG && rFlavor.MimeType.equalsIgnoreAsciiCase(::rtl::OUString::createFromAscii("image/png")))
        {
            // it's a PNG, import to BitmapEx
            ::vcl::PNGReader aPNGReader(*xStm);

            rBmpEx = aPNGReader.Read();
        }

        if(rBmpEx.IsEmpty())
        {
            Bitmap aBitmap;
            Bitmap aMask;

            // explicitly use Bitmap::Read with bFileHeader = sal_True
            // #124085# keep DIBV5 for read from clipboard, but should not happen
            ReadDIBV5(aBitmap, aMask, *xStm);

            if(aMask.IsEmpty())
            {
                rBmpEx = aBitmap;
            }
            else
            {
                rBmpEx = BitmapEx(aBitmap, aMask);
            }
        }

        bRet = (ERRCODE_NONE == xStm->GetError() && !rBmpEx.IsEmpty());

        /* SJ: #110748# At the moment we are having problems with DDB inserted as DIB. The
           problem is, that some graphics are inserted much too big because the nXPelsPerMeter
           and nYPelsPerMeter of the bitmap fileheader isn't including the correct value.
           Due to this reason the following code assumes that bitmaps with a logical size
           greater than 50 cm aren't having the correct mapmode set.
        
           The following code should be removed if DDBs and DIBs are supported via clipboard
           properly.
        */
        if(bRet)
        {
            const MapMode aMapMode(rBmpEx.GetPrefMapMode());

            if(MAP_PIXEL != aMapMode.GetMapUnit())
            {
                const Size aSize(OutputDevice::LogicToLogic(rBmpEx.GetPrefSize(), aMapMode, MAP_100TH_MM));

                // #122388# This wrongly corrects in the given case; changing from 5000 100th mm to
                // the described 50 cm (which is 50000 100th mm)
                if((aSize.Width() > 50000) || (aSize.Height() > 50000))
                {
                    rBmpEx.SetPrefMapMode(MAP_PIXEL);

                    // #122388# also adapt size by applying the mew MapMode
                    const Size aNewSize(OutputDevice::LogicToLogic(aSize, MAP_100TH_MM, MAP_PIXEL));
                    rBmpEx.SetPrefSize(aNewSize);
                }
            }
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetGDIMetaFile( SotFormatStringId nFormat, GDIMetaFile& rMtf )
{
	DataFlavor aFlavor;
	return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetGDIMetaFile( aFlavor, rMtf ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetGDIMetaFile( const DataFlavor& rFlavor, GDIMetaFile& rMtf )
{
	SotStorageStreamRef xStm;
	DataFlavor			aSubstFlavor;
	sal_Bool			bRet = sal_False;

	if( GetSotStorageStream( rFlavor, xStm ) )
	{
		*xStm >> rMtf;
		bRet = ( xStm->GetError() == ERRCODE_NONE );
	}

	if( !bRet &&
		HasFormat( SOT_FORMATSTR_ID_EMF ) &&
		SotExchange::GetFormatDataFlavor( SOT_FORMATSTR_ID_EMF, aSubstFlavor ) &&
		GetSotStorageStream( aSubstFlavor, xStm ) )
	{
		Graphic aGraphic;

		if( GraphicConverter::Import( *xStm, aGraphic ) == ERRCODE_NONE )
		{
			rMtf = aGraphic.GetGDIMetaFile();
			bRet = sal_True;
		}
	}

	if( !bRet &&
		HasFormat( SOT_FORMATSTR_ID_WMF ) &&
		SotExchange::GetFormatDataFlavor( SOT_FORMATSTR_ID_WMF, aSubstFlavor ) &&
		GetSotStorageStream( aSubstFlavor, xStm ) )
	{
		Graphic aGraphic;

		if( GraphicConverter::Import( *xStm, aGraphic ) == ERRCODE_NONE )
		{
			rMtf = aGraphic.GetGDIMetaFile();
			bRet = sal_True;
		}
	}

	return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetGraphic( SotFormatStringId nFormat, Graphic& rGraphic )
{
    if(FORMAT_BITMAP == nFormat)
    {
        // try to get PNG first
        DataFlavor aFlavor;

        if(SotExchange::GetFormatDataFlavor(SOT_FORMATSTR_ID_PNG, aFlavor))
        {
            if(GetGraphic(aFlavor, rGraphic))
            {
                return true;
            }
        }
    }

    DataFlavor aFlavor;
    return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetGraphic( aFlavor, rGraphic ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetGraphic( const ::com::sun::star::datatransfer::DataFlavor& rFlavor, Graphic& rGraphic )
{
	DataFlavor	aFlavor;
	sal_Bool	bRet = sal_False;

    if(SotExchange::GetFormatDataFlavor(SOT_FORMATSTR_ID_PNG, aFlavor) && 
        TransferableDataHelper::IsEqual(aFlavor, rFlavor))
	{
        // try to get PNG first
		BitmapEx aBmpEx;

		if( ( bRet = GetBitmapEx( aFlavor, aBmpEx ) ) == sal_True )
			rGraphic = aBmpEx;
	}
	else if(SotExchange::GetFormatDataFlavor( SOT_FORMAT_BITMAP, aFlavor ) &&
		TransferableDataHelper::IsEqual( aFlavor, rFlavor ) )
	{
		BitmapEx aBmpEx;

		if( ( bRet = GetBitmapEx( aFlavor, aBmpEx ) ) == sal_True )
			rGraphic = aBmpEx;
	}
	else if( SotExchange::GetFormatDataFlavor( SOT_FORMAT_GDIMETAFILE, aFlavor ) &&
			 TransferableDataHelper::IsEqual( aFlavor, rFlavor ) )
	{
		GDIMetaFile aMtf;

		if( ( bRet = GetGDIMetaFile( aFlavor, aMtf ) ) == sal_True )
			rGraphic = aMtf;
	}
	else
	{
		SotStorageStreamRef xStm;

		if( GetSotStorageStream( rFlavor, xStm ) )
		{
			*xStm >> rGraphic;
			bRet = ( xStm->GetError() == ERRCODE_NONE );
		}
	}

	return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetImageMap( SotFormatStringId nFormat, ImageMap& rIMap )
{
	DataFlavor aFlavor;
    return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetImageMap( aFlavor, rIMap ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetImageMap( const ::com::sun::star::datatransfer::DataFlavor& rFlavor, ImageMap& rIMap )
{
	SotStorageStreamRef xStm;
	sal_Bool			bRet = GetSotStorageStream( rFlavor, xStm );

	if( bRet )
	{
        rIMap.Read( *xStm, String() );
		bRet = ( xStm->GetError() == ERRCODE_NONE );
	}

	return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetTransferableObjectDescriptor( SotFormatStringId nFormat, TransferableObjectDescriptor& rDesc )
{
	DataFlavor aFlavor;
	return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetTransferableObjectDescriptor( aFlavor, rDesc ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetTransferableObjectDescriptor( const ::com::sun::star::datatransfer::DataFlavor&, TransferableObjectDescriptor& rDesc )
{
    rDesc = *mpObjDesc;
    return true;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetINetBookmark( SotFormatStringId nFormat, INetBookmark& rBmk )
{
	DataFlavor aFlavor;
	return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetINetBookmark( aFlavor, rBmk ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetINetBookmark( const ::com::sun::star::datatransfer::DataFlavor& rFlavor, INetBookmark& rBmk )
{
	sal_Bool bRet = sal_False;
	if( HasFormat( rFlavor ))
	{
	const SotFormatStringId nFormat = SotExchange::GetFormat( rFlavor );
	switch( nFormat )
	{
		case( SOT_FORMATSTR_ID_SOLK ):
		case( SOT_FORMATSTR_ID_UNIFORMRESOURCELOCATOR ):
		{
			String aString;
			if( GetString( rFlavor, aString ) )
			{
				if( SOT_FORMATSTR_ID_UNIFORMRESOURCELOCATOR == nFormat )
				{
					rBmk = INetBookmark( aString, aString );
					bRet = sal_True;
				}
				else
				{
					String		aURL, aDesc;
					sal_uInt16	nStart = aString.Search( '@' ), nLen = (sal_uInt16) aString.ToInt32();

					if( !nLen && aString.GetChar( 0 ) != '0' )
					{
						DBG_WARNING( "SOLK: 1. len=0" );
					}
					if( nStart == STRING_NOTFOUND || nLen > aString.Len() - nStart - 3 )
					{
						DBG_WARNING( "SOLK: 1. illegal start or wrong len" );
					}
					aURL = aString.Copy( nStart + 1, nLen );

					aString.Erase( 0, nStart + 1 + nLen );
					nStart = aString.Search( '@' );
					nLen = (sal_uInt16) aString.ToInt32();

					if( !nLen && aString.GetChar( 0 ) != '0' )
					{
						DBG_WARNING( "SOLK: 2. len=0" );
					}
					if( nStart == STRING_NOTFOUND || nLen > aString.Len() - nStart - 1 )
					{
						DBG_WARNING( "SOLK: 2. illegal start or wrong len" );
					}
					aDesc = aString.Copy( nStart+1, nLen );

					rBmk = INetBookmark( aURL, aDesc );
					bRet = sal_True;
				}
			}
		}
		break;

		case( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK ):
		{
			Sequence< sal_Int8 > aSeq;

			if( GetSequence( rFlavor, aSeq ) && ( 2048 == aSeq.getLength() ) )
			{
				rBmk = INetBookmark( String( reinterpret_cast< const sal_Char* >( aSeq.getConstArray() ), gsl_getSystemTextEncoding() ),
									 String( reinterpret_cast< const sal_Char* >( aSeq.getConstArray() ) + 1024, gsl_getSystemTextEncoding() ) );
				bRet = sal_True;
			}
		}
		break;

#ifdef WNT
		case SOT_FORMATSTR_ID_FILEGRPDESCRIPTOR:
		{
			Sequence< sal_Int8 > aSeq;

			if( GetSequence( rFlavor, aSeq ) && aSeq.getLength() )
			{
				FILEGROUPDESCRIPTOR* pFDesc = (FILEGROUPDESCRIPTOR*) aSeq.getConstArray();

				if( pFDesc->cItems )
				{
					ByteString			aDesc( pFDesc->fgd[ 0 ].cFileName );
					rtl_TextEncoding	eTextEncoding = gsl_getSystemTextEncoding();

					if( ( aDesc.Len() > 4 ) && aDesc.Copy( aDesc.Len() - 4 ).EqualsIgnoreCaseAscii( ".URL" ) )
					{
						SvStream* pStream = ::utl::UcbStreamHelper::CreateStream( INetURLObject( String( aDesc, eTextEncoding ) ).GetMainURL( INetURLObject::NO_DECODE ),
                                                                                  STREAM_STD_READ );

						if( !pStream || pStream->GetError() )
						{
							DataFlavor aFileContentFlavor;

							aSeq.realloc( 0 );
							delete pStream;

							if( SotExchange::GetFormatDataFlavor( SOT_FORMATSTR_ID_FILECONTENT, aFileContentFlavor ) &&
								GetSequence( aFileContentFlavor, aSeq ) && aSeq.getLength() )
							{
								pStream = new SvMemoryStream( (sal_Char*) aSeq.getConstArray(), aSeq.getLength(), STREAM_STD_READ );
							}
							else
								pStream = NULL;
						}

						if( pStream )
						{
							ByteString	aLine;
							sal_Bool	bSttFnd = sal_False;

							while( pStream->ReadLine( aLine ) )
							{
								if( aLine.EqualsIgnoreCaseAscii( "[InternetShortcut]" ) )
									bSttFnd = sal_True;
								else if( bSttFnd && aLine.Copy( 0, 4 ).EqualsIgnoreCaseAscii( "URL=" ) )
								{
									rBmk = INetBookmark( String( aLine.Erase( 0, 4 ), eTextEncoding ),
														 String( aDesc.Erase( aDesc.Len() - 4 ), eTextEncoding ) );
									bRet = sal_True;
									break;
								}
							}

							delete pStream;
						}
					}
				}
			}
		}
		break;
#endif

	}
	}
	return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetINetImage( SotFormatStringId nFormat,
												INetImage& rINtImg )
{
	DataFlavor aFlavor;
	return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetINetImage( aFlavor, rINtImg ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetINetImage(
		const ::com::sun::star::datatransfer::DataFlavor& rFlavor,
		INetImage& rINtImg )
{
	SotStorageStreamRef xStm;
	sal_Bool bRet = GetSotStorageStream( rFlavor, xStm );

	if( bRet )
		bRet = rINtImg.Read( *xStm, SotExchange::GetFormat( rFlavor ) );
	return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetFileList( SotFormatStringId nFormat,
												FileList& rFileList )
{
	DataFlavor aFlavor;
	return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetFileList( aFlavor, rFileList ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetFileList(
			const ::com::sun::star::datatransfer::DataFlavor&,
			FileList& rFileList )
{
	SotStorageStreamRef xStm;
    sal_Bool            bRet = sal_False;

    for( sal_uInt32 i = 0, nFormatCount = GetFormatCount(); ( i < nFormatCount ) && !bRet; ++i )
    {
        if( SOT_FORMAT_FILE_LIST == GetFormat( i ) )
        {
            const DataFlavor aFlavor( GetFormatDataFlavor( i ) );

            if( GetSotStorageStream( aFlavor, xStm ) )
            {
                if( aFlavor.MimeType.indexOf( ::rtl::OUString::createFromAscii( "text/uri-list" ) ) > -1 )
                {
                    ByteString aByteString;

                    while( xStm->ReadLine( aByteString ) )
                        if( aByteString.Len() && aByteString.GetChar( 0 ) != '#' )
                            rFileList.AppendFile( String( aByteString, RTL_TEXTENCODING_UTF8 ) );

                    bRet = sal_True;
                 }
                 else
                    bRet = ( ( *xStm >> rFileList ).GetError() == ERRCODE_NONE );
            }
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetSequence( SotFormatStringId nFormat, Sequence< sal_Int8 >& rSeq )
{
	DataFlavor aFlavor;
	return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetSequence( aFlavor, rSeq ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetSequence( const DataFlavor& rFlavor, Sequence< sal_Int8 >& rSeq )
{
#ifdef DEBUG
    fprintf( stderr, "TransferableDataHelper requests sequence of data\n" );
#endif

    const Any aAny( GetAny( rFlavor ) );
	return( aAny.hasValue() && ( aAny >>= rSeq ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetSotStorageStream( SotFormatStringId nFormat, SotStorageStreamRef& rxStream )
{
    DataFlavor aFlavor;
	return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetSotStorageStream( aFlavor, rxStream ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetSotStorageStream( const DataFlavor& rFlavor, SotStorageStreamRef& rxStream )
{
	Sequence< sal_Int8 >	aSeq;
	sal_Bool				bRet = GetSequence( rFlavor, aSeq );

	if( bRet )
	{
		rxStream = new SotStorageStream( String() );
		rxStream->Write( aSeq.getConstArray(), aSeq.getLength() );
		rxStream->Seek( 0 );
	}

	return bRet;
}

sal_Bool TransferableDataHelper::GetInputStream( SotFormatStringId nFormat, Reference < XInputStream >& rxStream )
{
	DataFlavor aFlavor;
    return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetInputStream( aFlavor, rxStream ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetInputStream( const DataFlavor& rFlavor, Reference < XInputStream >& rxStream )
{
	Sequence< sal_Int8 >	aSeq;
	sal_Bool				bRet = GetSequence( rFlavor, aSeq );

	if( bRet )
          rxStream = new ::comphelper::SequenceInputStream( aSeq );

	return bRet;
}

// -----------------------------------------------------------------------------


sal_Bool TransferableDataHelper::GetInterface( SotFormatStringId nFormat, Reference< XInterface >& rIf )
{
	DataFlavor aFlavor;
	return( SotExchange::GetFormatDataFlavor( nFormat, aFlavor ) && GetInterface( aFlavor, rIf ) );
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::GetInterface( const DataFlavor& rFlavor, Reference< XInterface >& rIf )
{
	const Any aAny( GetAny( rFlavor ) );
	return( aAny.hasValue() && ( aAny >>= rIf ) );
}

// -----------------------------------------------------------------------------
void TransferableDataHelper::Rebind( const Reference< XTransferable >& _rxNewContent )
{
	mxTransfer = _rxNewContent;
	InitFormats();
}

// -----------------------------------------------------------------------------

sal_Bool TransferableDataHelper::StartClipboardListening( )
{
    ::osl::MutexGuard aGuard( mpImpl->maMutex );

    StopClipboardListening( );

	mpImpl->mpClipboardListener = new TransferableClipboardNotifier( mxClipboard, *this, mpImpl->maMutex );
    mpImpl->mpClipboardListener->acquire();

	return mpImpl->mpClipboardListener->isListening();
}

// -----------------------------------------------------------------------------

void TransferableDataHelper::StopClipboardListening( )
{
    ::osl::MutexGuard aGuard( mpImpl->maMutex );

	if ( mpImpl->mpClipboardListener )
    {
        mpImpl->mpClipboardListener->dispose();
        mpImpl->mpClipboardListener->release();
        mpImpl->mpClipboardListener = NULL;
    }
}

// -----------------------------------------------------------------------------

TransferableDataHelper TransferableDataHelper::CreateFromSystemClipboard( Window * pWindow )
{
	DBG_ASSERT( pWindow, "Window pointer is NULL" );

    Reference< XClipboard >	xClipboard;
   	TransferableDataHelper	aRet;

    if( pWindow )
        xClipboard = pWindow->GetClipboard();

    if( xClipboard.is() )
   	{
   		try

    	{
	    	Reference< XTransferable > xTransferable( xClipboard->getContents() );

		    if( xTransferable.is() )
			{
    			aRet = TransferableDataHelper( xTransferable );
   				aRet.mxClipboard = xClipboard;
					// also copy the clipboard - 99030 - 23.05.2002 - fs@openoffice.org
			}
   		}
    	catch( const ::com::sun::star::uno::Exception& )
	    {
   		}
    }

	return aRet;
}


// -----------------------------------------------------------------------------

TransferableDataHelper TransferableDataHelper::CreateFromSelection( Window* pWindow )
{
	DBG_ASSERT( pWindow, "Window pointer is NULL" );

    Reference< XClipboard >	xSelection;
   	TransferableDataHelper	aRet;

    if( pWindow )
        xSelection = pWindow->GetPrimarySelection();

    if( xSelection.is() )
   	{
        const sal_uInt32 nRef = Application::ReleaseSolarMutex();

  		try
    	{
	    	Reference< XTransferable > xTransferable( xSelection->getContents() );

		    if( xTransferable.is() )
   			{
    			aRet = TransferableDataHelper( xTransferable );
   				aRet.mxClipboard = xSelection;
		    }
   		}
    	catch( const ::com::sun::star::uno::Exception& )
	    {
   		}

        Application::AcquireSolarMutex( nRef );
    }

	return aRet;
}

// -----------------------------------------------------------------------------
sal_Bool TransferableDataHelper::IsEqual( const ::com::sun::star::datatransfer::DataFlavor& rInternalFlavor,
										  const ::com::sun::star::datatransfer::DataFlavor& rRequestFlavor,
                                          sal_Bool )
{
    Reference< XMultiServiceFactory >       xFact( ::comphelper::getProcessServiceFactory() );
    Reference< XMimeContentTypeFactory >    xMimeFact;
	sal_Bool								bRet = sal_False;

    try
    {
		if( xFact.is() )
    	    xMimeFact = Reference< XMimeContentTypeFactory >( xFact->createInstance( ::rtl::OUString::createFromAscii(
                                                              "com.sun.star.datatransfer.MimeContentTypeFactory" ) ),
                                                              UNO_QUERY );

        if( xMimeFact.is() )
		{
            Reference< XMimeContentType > xRequestType1( xMimeFact->createMimeContentType( rInternalFlavor.MimeType ) );
            Reference< XMimeContentType > xRequestType2( xMimeFact->createMimeContentType( rRequestFlavor.MimeType ) );

			if( xRequestType1.is() && xRequestType2.is() )
            {
                if( xRequestType1->getFullMediaType().equalsIgnoreAsciiCase( xRequestType2->getFullMediaType() ) )
			    {
                    if( xRequestType1->getFullMediaType().equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "text/plain" ) ) )
                    {
                        // special handling for text/plain media types
                        const ::rtl::OUString aCharsetString( ::rtl::OUString::createFromAscii( "charset" ) );

                        if( !xRequestType2->hasParameter( aCharsetString ) ||
						    xRequestType2->getParameterValue( aCharsetString ).equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "utf-16" ) ) ||
						    xRequestType2->getParameterValue( aCharsetString ).equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "unicode" ) ) )
                        {
                            bRet = sal_True;
                        }
                    }
                    else if( xRequestType1->getFullMediaType().equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/x-openoffice" ) ) )
                    {
                        // special handling for application/x-openoffice media types
                        const ::rtl::OUString aFormatString( ::rtl::OUString::createFromAscii( "windows_formatname" ) );

                        if( xRequestType1->hasParameter( aFormatString ) &&
                            xRequestType2->hasParameter( aFormatString ) &&
                            xRequestType1->getParameterValue( aFormatString ).equalsIgnoreAsciiCase( xRequestType2->getParameterValue( aFormatString ) ) )
                        {
                            bRet = sal_True;
                        }
                    }
                    else
                        bRet = sal_True;
			    }
            }
		}
    }
    catch( const ::com::sun::star::uno::Exception& )
	{
        bRet = rInternalFlavor.MimeType.equalsIgnoreAsciiCase( rRequestFlavor.MimeType );
	}

    return bRet;
}
