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

#if defined UNX && defined ALPHA
#include <fstream.hxx>
#endif
#include <vos/mutex.hxx>
#include <comphelper/processfactory.hxx>
#include <ucbhelper/content.hxx>
#include <cppuhelper/implbase1.hxx>
#include <tools/urlobj.hxx>
#include <vcl/salctype.hxx>
#include <vcl/pngread.hxx>
#include <vcl/pngwrite.hxx>
#include <vcl/svgdata.hxx>
#include <vcl/virdev.hxx>
#include <vcl/svapp.hxx>
#include <osl/file.hxx>
#include <svtools/filter.hxx>
#include "FilterConfigCache.hxx"
#include <svtools/FilterConfigItem.hxx>
#include <svtools/fltcall.hxx>
#include <svtools/wmf.hxx>
#include "gifread.hxx"
#include "jpeg.hxx"
#include "xbmread.hxx"
#include "xpmread.hxx"
#include <svl/solar.hrc>
#include <svtools/svtools.hrc>
#include "sgffilt.hxx"
#include "osl/module.hxx"
#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/awt/Size.hpp>
#include <com/sun/star/uno/XInterface.hpp>
#include <com/sun/star/uno/XWeak.hpp>
#include <com/sun/star/uno/XAggregation.hpp>
#ifndef _COM_SUN_STAR_UNO_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/svg/XSVGWriter.hpp>
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <com/sun/star/ucb/CommandAbortedException.hpp>
#include <unotools/ucbstreamhelper.hxx>
#include <unotools/localfilehelper.hxx>
#include <comphelper/processfactory.hxx>
#include <rtl/bootstrap.hxx>
#include <rtl/instance.hxx>
#include <vcl/metaact.hxx>
#include <vcl/dibtools.hxx>

#include "SvFilterOptionsDialog.hxx"

#define PMGCHUNG_msOG		0x6d734f47		// Microsoft Office Animated GIF

#if (defined OS2 && !defined ICC)

#define IMPORT_FUNCTION_NAME	"_GraphicImport"
#define EXPORT_FUNCTION_NAME	"_GraphicExport"
#define IMPDLG_FUNCTION_NAME	"_DoImportDialog"
#define EXPDLG_FUNCTION_NAME	"_DoExportDialog"

#else

#define IMPORT_FUNCTION_NAME	"GraphicImport"
#define EXPORT_FUNCTION_NAME	"GraphicExport"
#define IMPDLG_FUNCTION_NAME	"DoImportDialog"
#define EXPDLG_FUNCTION_NAME	"DoExportDialog"

#endif


// -----------
// - statics -
// -----------

using namespace ::rtl;
using namespace ::com::sun::star;

static List*		pFilterHdlList = NULL;

static ::osl::Mutex& getListMutex()
{
	static ::osl::Mutex	s_aListProtection;
	return s_aListProtection;
}

static GraphicFilter* pGraphicFilter=0;

// -------------------------
// - ImpFilterOutputStream -
// -------------------------

class ImpFilterOutputStream : public ::cppu::WeakImplHelper1< ::com::sun::star::io::XOutputStream >
{
protected:

	SvStream& 							mrStm;

    virtual void SAL_CALL				writeBytes( const ::com::sun::star::uno::Sequence< sal_Int8 >& rData ) throw (::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException) { mrStm.Write( rData.getConstArray(), rData.getLength() ); }
    virtual void SAL_CALL				flush() throw (::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException) { mrStm.Flush(); }
    virtual void SAL_CALL				closeOutput() throw() {}

public:

										ImpFilterOutputStream( SvStream& rStm ) : mrStm( rStm ) {}
										~ImpFilterOutputStream() {}
};

sal_Bool ImplDirEntryHelper::Exists( const INetURLObject& rObj )
{
	sal_Bool bExists = sal_False;

	try
	{
		::rtl::OUString	aTitle;
		::ucbhelper::Content	aCnt( rObj.GetMainURL( INetURLObject::NO_DECODE ),
							  ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XCommandEnvironment >() );

		bExists = aCnt.isDocument();
	}
	catch( ::com::sun::star::ucb::CommandAbortedException& )
	{
		DBG_ERRORFILE( "CommandAbortedException" );
	}
	catch( ::com::sun::star::ucb::ContentCreationException& )
	{
		DBG_ERRORFILE( "ContentCreationException" );
	}
	catch( ... )
	{
//		DBG_ERRORFILE( "Any other exception" );
	}
	return bExists;
}

// -----------------------------------------------------------------------------

void ImplDirEntryHelper::Kill( const String& rMainUrl )
{
	try
	{
		::ucbhelper::Content aCnt( rMainUrl,
							 ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XCommandEnvironment >() );

		aCnt.executeCommand( ::rtl::OUString::createFromAscii( "delete" ),
							 ::com::sun::star::uno::makeAny( sal_Bool( sal_True ) ) );
	}
	catch( ::com::sun::star::ucb::CommandAbortedException& )
	{
		DBG_ERRORFILE( "CommandAbortedException" );
	}
	catch( ... )
	{
		DBG_ERRORFILE( "Any other exception" );
	}
}

// --------------------
// - Helper functions -
// --------------------

//--------------------------------------------------------------------------

sal_uInt8* ImplSearchEntry( sal_uInt8* pSource, sal_uInt8* pDest, sal_uLong nComp, sal_uLong nSize )
{
	while ( nComp-- >= nSize )
	{
		sal_uLong i;
		for ( i = 0; i < nSize; i++ )
		{
			if ( ( pSource[i]&~0x20 ) != ( pDest[i]&~0x20 ) )
				break;
		}
		if ( i == nSize )
			return pSource;
		pSource++;
	}
	return NULL;
}

//--------------------------------------------------------------------------

inline String ImpGetExtension( const String &rPath )
{
	String			aExt;
	INetURLObject	aURL( rPath );
	aExt = aURL.GetFileExtension().toAsciiUpperCase();
	return aExt;
}

/*************************************************************************
|*
|*    ImpPeekGraphicFormat()
|*
|*    Beschreibung:
|*        Diese Funktion kann zweierlei:
|*        1.) Datei anlesen, Dateiformat ermitteln
|*            Eingabe-prarameter:
|*              rPath            - Dateipfad
|*              rFormatExtension - Inhalt egal
|*              bTest            - setze sal_False
|*            Ausgabe-parameter:
|*              Funkionswert     - sal_True wenn Erfolg
|*              rFormatExtension - Bei Erfolg: uebliche Dateiendung
|*                                 des Formats (Grossbuchstaben)
|*        2.) Datei anlesen, Dateiformat ueberpruefen
|*            Eingabe-prarameter:
|*              rPath            - Dateipfad
|*              rFormatExtension - uebliche Dateiendung des Formats
|*                                 (Grossbuchstaben)
|*              bTest            - setze sal_True
|*            Ausgabe-parameter:
|*              Funkionswert     - sal_False, wenn die Datei bestimmt nicht
|*                                 vom uebgebenen Format ist.
|*                                 sal_True, wenn die Datei WAHRSCHEINLICH von
|*                                 dem Format ist, ODER WENN DAS FORMAT
|*                                 DIESER FUNKTION NICHT BEKANNT IST!
|*
|*    Ersterstellung    OH 26.05.95
|*    Letzte Aenderung  OH 07.08.95
|*
*************************************************************************/

static sal_Bool ImpPeekGraphicFormat( SvStream& rStream, String& rFormatExtension, sal_Bool bTest )
{
	sal_uInt16  i;
	sal_uInt8    sFirstBytes[ 256 ];
	sal_uLong   nFirstLong,nSecondLong;
	sal_uLong	nStreamPos = rStream.Tell();

	rStream.Seek( STREAM_SEEK_TO_END );
	sal_uLong nStreamLen = rStream.Tell() - nStreamPos;
	rStream.Seek( nStreamPos );

	if ( !nStreamLen )
	{
		SvLockBytes* pLockBytes = rStream.GetLockBytes();
		if ( pLockBytes  )
			pLockBytes->SetSynchronMode( sal_True );

		rStream.Seek( STREAM_SEEK_TO_END );
		nStreamLen = rStream.Tell() - nStreamPos;
		rStream.Seek( nStreamPos );
	}
	// Die ersten 256 Bytes in einen Buffer laden:
	if( nStreamLen >= 256 )
		rStream.Read( sFirstBytes, 256 );
	else
	{
		rStream.Read( sFirstBytes, nStreamLen );

		for( i = (sal_uInt16) nStreamLen; i < 256; i++ )
			sFirstBytes[ i ]=0;
	}

	if( rStream.GetError() )
		return sal_False;

	// Die ersten 8 Bytes in nFirstLong, nSecondLong unterbringen,
	// Big-Endian:
	for( i = 0, nFirstLong = 0L, nSecondLong = 0L; i < 4; i++ )
	{
		nFirstLong=(nFirstLong<<8)|(sal_uLong)sFirstBytes[i];
		nSecondLong=(nSecondLong<<8)|(sal_uLong)sFirstBytes[i+4];
	}

	// Folgende Variable ist nur bei bTest==sal_True interessant. Sie
	// bleibt sal_False, wenn das Format (rFormatExtension) hier noch nicht
	// einprogrammiert wurde.
	sal_Bool bSomethingTested = sal_False;

	// Nun werden die verschieden Formate ueberprueft. Dabei ist die
	// Reihenfolge nicht egal. Z.b. koennte eine MET-Datei auch durch
	// den BMP-Test gehen, umgekehrt kann eine BMP-Datei kaum durch den
	// MET-Test gehen. Also sollte MET vor BMP getestet werden.
	// Theoretisch waere aber vielleicht auch eine BMP-Datei denkbar,
	// die durch den MET-Test geht.
	// Diese Probleme gibt es natuerlich nicht nur bei MET und BMP.
	// Deshalb wird im Falle der Uberpruefung eines Formats (bTest==sal_True)
	// nur genau dieses eine Format getestet. Alles andere koennte fatale
	// Folgen haben, z.B. wenn der Benutzer sagt, es sei BMP-Datei (und es
	// ist BMP-Datei), und hier wuerde die Datei durch den MET-Test gehen...

	//--------------------------- MET ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "MET", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested=sal_True;
		if( sFirstBytes[2] == 0xd3 )
		{
			rStream.SetNumberFormatInt( NUMBERFORMAT_INT_BIGENDIAN );
			rStream.Seek( nStreamPos );
			sal_uInt16 nFieldSize;
			sal_uInt8 nMagic;
			sal_Bool bOK=sal_True;
			rStream >> nFieldSize >> nMagic;
			for (i=0; i<3; i++) {
				if (nFieldSize<6) { bOK=sal_False; break; }
				if (nStreamLen < rStream.Tell() + nFieldSize ) { bOK=sal_False; break; }
				rStream.SeekRel(nFieldSize-3);
				rStream >> nFieldSize >> nMagic;
				if (nMagic!=0xd3) { bOK=sal_False; break; }
			}
			rStream.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
			if (bOK && !rStream.GetError()) {
				rFormatExtension= UniString::CreateFromAscii( "MET", 3 );
				return sal_True;
			}
		}
	}

	//--------------------------- BMP ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "BMP", 3 ) == COMPARE_EQUAL ) )
	{
		sal_uInt8 nOffs;

		bSomethingTested=sal_True;

		// OS/2-Bitmaparray ('BA') koennen wir evtl. auch lesen,
		// dementspr. muessen wir den Offset anpassen,
		// um auf die erste Bitmap im Array zu stossen
		if ( sFirstBytes[0] == 0x42 && sFirstBytes[1] == 0x41 )
			nOffs = 14;
		else
			nOffs = 0;

		// Jetzt testen wir zunaechst auf 'BM'
		if ( sFirstBytes[0+nOffs]==0x42 && sFirstBytes[1+nOffs]==0x4d )
		{
			// unter OS/2 koennen die Reserved-Flags != 0 sein
			// (was sie eigentlich nicht duerften);
			// in diesem Fall testen wir die Groesse des BmpInfoHeaders
			if ( ( sFirstBytes[6+nOffs]==0x00 &&
				   sFirstBytes[7+nOffs]==0x00 &&
				   sFirstBytes[8+nOffs]==0x00 &&
				   sFirstBytes[9+nOffs]==0x00 ) ||
				   sFirstBytes[14+nOffs] == 0x28 ||
				   sFirstBytes[14+nOffs] == 0x0c )
			{
				rFormatExtension = UniString::CreateFromAscii( "BMP", 3 );
				return sal_True;
			}
		}
	}

	//--------------------------- WMF/EMF ------------------------------------

	if( !bTest ||
		( rFormatExtension.CompareToAscii( "WMF", 3 ) == COMPARE_EQUAL ) ||
			( rFormatExtension.CompareToAscii( "EMF", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested = sal_True;

		if ( nFirstLong==0xd7cdc69a || nFirstLong==0x01000900 )
		{
			rFormatExtension = UniString::CreateFromAscii( "WMF", 3 );
			return sal_True;
		}
		else if( nFirstLong == 0x01000000 && sFirstBytes[ 40 ] == 0x20 && sFirstBytes[ 41 ] == 0x45 &&
			sFirstBytes[ 42 ] == 0x4d && sFirstBytes[ 43 ] == 0x46 )
		{
			rFormatExtension = UniString::CreateFromAscii( "EMF", 3 );
			return sal_True;
		}
	}

	//--------------------------- PCX ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "PCX", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested=sal_True;
		if (sFirstBytes[0]==0x0a)
		{
			sal_uInt8 nVersion=sFirstBytes[1];
			sal_uInt8 nEncoding=sFirstBytes[2];
			if( ( nVersion==0 || nVersion==2 || nVersion==3 || nVersion==5 ) && nEncoding<=1 )
			{
				rFormatExtension = UniString::CreateFromAscii( "PCX", 3 );
				return sal_True;
			}
		}
	}

	//--------------------------- TIF ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "TIF", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested=sal_True;
		if ( nFirstLong==0x49492a00 || nFirstLong==0x4d4d002a )
		{
			rFormatExtension=UniString::CreateFromAscii( "TIF", 3 );
			return sal_True;
		}
	}

	//--------------------------- GIF ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "GIF", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested=sal_True;
		if ( nFirstLong==0x47494638 && (sFirstBytes[4]==0x37 || sFirstBytes[4]==0x39) && sFirstBytes[5]==0x61 )
		{
			rFormatExtension = UniString::CreateFromAscii( "GIF", 3 );
			return sal_True;
		}
	}

	//--------------------------- PNG ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "PNG", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested=sal_True;
		if (nFirstLong==0x89504e47 && nSecondLong==0x0d0a1a0a)
		{
			rFormatExtension = UniString::CreateFromAscii( "PNG", 3 );
			return sal_True;
		}
	}

	//--------------------------- JPG ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "JPG", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested=sal_True;
		if ( ( nFirstLong==0xffd8ffe0 && sFirstBytes[6]==0x4a && sFirstBytes[7]==0x46 && sFirstBytes[8]==0x49 && sFirstBytes[9]==0x46 ) ||
			 ( nFirstLong==0xffd8fffe ) || ( 0xffd8ff00 == ( nFirstLong & 0xffffff00 ) ) )
		{
			rFormatExtension = UniString::CreateFromAscii( "JPG", 3 );
			return sal_True;
		}
	}

	//--------------------------- SVM ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "SVM", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested=sal_True;
		if( nFirstLong==0x53564744 && sFirstBytes[4]==0x49 )
		{
			rFormatExtension = UniString::CreateFromAscii( "SVM", 3 );
			return sal_True;
		}
		else if( sFirstBytes[0]==0x56 && sFirstBytes[1]==0x43 && sFirstBytes[2]==0x4C &&
				 sFirstBytes[3]==0x4D && sFirstBytes[4]==0x54 && sFirstBytes[5]==0x46 )
		{
			rFormatExtension = UniString::CreateFromAscii( "SVM", 3 );
			return sal_True;
		}
	}

	//--------------------------- PCD ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "PCD", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested = sal_True;
		if( nStreamLen >= 2055 )
		{
			char sBuf[8];
			rStream.Seek( nStreamPos + 2048 );
			rStream.Read( sBuf, 7 );

			if( strncmp( sBuf, "PCD_IPI", 7 ) ==  0 )
			{
				rFormatExtension = UniString::CreateFromAscii( "PCD", 3 );
				return sal_True;
			}
		}
	}

	//--------------------------- PSD ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "PSD", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested = sal_True;
		if ( ( nFirstLong == 0x38425053 ) && ( (nSecondLong >> 16 ) == 1 ) )
		{
			rFormatExtension = UniString::CreateFromAscii( "PSD", 3 );
			return sal_True;
		}
	}

	//--------------------------- EPS ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "EPS", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested = sal_True;
		if ( ( nFirstLong == 0xC5D0D3C6 ) || ( ImplSearchEntry( sFirstBytes, (sal_uInt8*)"%!PS-Adobe", 10, 10 ) &&
			 ImplSearchEntry( &sFirstBytes[15], (sal_uInt8*)"EPS", 3, 3 ) ) )
		{
			rFormatExtension = UniString::CreateFromAscii( "EPS", 3 );
			return sal_True;
		}
	}

	//--------------------------- DXF ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "DXF", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested=sal_True;

		i=0;
		while (i<256 && sFirstBytes[i]<=32)
			i++;

		if (i<256)
		{
			if( sFirstBytes[i]=='0' )
				i++;
			else
				i=256;
		}
		while( i<256 && sFirstBytes[i]<=32 )
			i++;

		if (i+7<256)
		{
			if (strncmp((char*)(sFirstBytes+i),"SECTION",7)==0)
			{
				rFormatExtension = UniString::CreateFromAscii( "DXF", 3 );
				return sal_True;
			}
		}

		if( strncmp( (char*) sFirstBytes, "AutoCAD Binary DXF", 18 ) == 0 )
		{
			rFormatExtension = UniString::CreateFromAscii( "DXF", 3 );
			return sal_True;
		}
	}

	//--------------------------- PCT ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "PCT", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested = sal_True;
		sal_uInt8 sBuf[3];
		// store number format
		sal_uInt16 oldNumberFormat = rStream.GetNumberFormatInt();
		sal_uInt32 nOffset;	// in ms documents the pict format is used without the first 512 bytes
		for ( nOffset = 0; ( nOffset <= 512 ) && ( ( nStreamPos + nOffset + 14 ) <= nStreamLen ); nOffset += 512 )
		{
			short y1,x1,y2,x2;
			sal_Bool bdBoxOk = sal_True;

			rStream.Seek( nStreamPos + nOffset);
			// size of the pict in version 1 pict ( 2bytes) : ignored
			rStream.SeekRel(2);
			// bounding box (bytes 2 -> 9)
			rStream.SetNumberFormatInt(NUMBERFORMAT_INT_BIGENDIAN);
			rStream >> y1 >> x1 >> y2 >> x2;
			rStream.SetNumberFormatInt(oldNumberFormat); // reset format
			
			if (x1 > x2 || y1 > y2 || // bad bdbox
			    (x1 == x2 && y1 == y2) || // 1 pixel picture
			    x2-x1 > 2048 || y2-y1 > 2048 ) // picture abnormally big
			  bdBoxOk = sal_False;

			// read version op
			rStream.Read( sBuf,3 );
			// see http://developer.apple.com/legacy/mac/library/documentation/mac/pdf/Imaging_With_QuickDraw/Appendix_A.pdf
			// normal version 2 - page A23 and A24
			if ( sBuf[ 0 ] == 0x00 && sBuf[ 1 ] == 0x11 && sBuf[ 2 ] == 0x02)
			{
			  rFormatExtension = UniString::CreateFromAscii( "PCT", 3 );
			  return sal_True;
			}
			// normal version 1 - page A25
			else if (sBuf[ 0 ] == 0x11 && sBuf[ 1 ] == 0x01 && bdBoxOk) {
			  rFormatExtension = UniString::CreateFromAscii( "PCT", 3 );
			  return sal_True;
			}
			// previous code kept in order to do not break any compatibility
			// probably eroneous
			else if ( sBuf[ 0 ] == 0x00 && sBuf[ 1 ] == 0x11 && sBuf[ 2 ] == 0x01 && bdBoxOk)
			{
			  rFormatExtension = UniString::CreateFromAscii( "PCT", 3 );
			  return sal_True;
			}
		}
	}

	//------------------------- PBM + PGM + PPM ---------------------------
	if( !bTest ||
		( rFormatExtension.CompareToAscii( "PBM", 3 ) == COMPARE_EQUAL ) ||
			( rFormatExtension.CompareToAscii( "PGM", 3 ) == COMPARE_EQUAL ) ||
				( rFormatExtension.CompareToAscii( "PPM", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested=sal_True;
		if ( sFirstBytes[ 0 ] == 'P' )
		{
			switch( sFirstBytes[ 1 ] )
			{
				case '1' :
				case '4' :
					rFormatExtension = UniString::CreateFromAscii( "PBM", 3 );
				return sal_True;

				case '2' :
				case '5' :
					rFormatExtension = UniString::CreateFromAscii( "PGM", 3 );
				return sal_True;

				case '3' :
				case '6' :
					rFormatExtension = UniString::CreateFromAscii( "PPM", 3 );
				return sal_True;
			}
		}
	}

	//--------------------------- RAS( SUN RasterFile )------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "RAS", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested=sal_True;
		if( nFirstLong == 0x59a66a95 )
		{
			rFormatExtension = UniString::CreateFromAscii( "RAS", 3 );
			return sal_True;
		}
	}

	//--------------------------- XPM ------------------------------------
	if( !bTest )
	{
		bSomethingTested = sal_True;
		if( ImplSearchEntry( sFirstBytes, (sal_uInt8*)"/* XPM */", 256, 9 ) )
		{
			rFormatExtension = UniString::CreateFromAscii( "XPM", 3 );
			return sal_True;
		}
	}
	else if( rFormatExtension.CompareToAscii( "XPM", 3 ) == COMPARE_EQUAL )
	{
		bSomethingTested = sal_True;
		return sal_True;
	}

	//--------------------------- XBM ------------------------------------
	if( !bTest )
	{
		sal_uLong nSize = ( nStreamLen > 2048 ) ? 2048 : nStreamLen;
		sal_uInt8* pBuf = new sal_uInt8 [ nSize ];

		rStream.Seek( nStreamPos );
		rStream.Read( pBuf, nSize );
		sal_uInt8* pPtr = ImplSearchEntry( pBuf, (sal_uInt8*)"#define", nSize, 7 );

		if( pPtr )
		{
			if( ImplSearchEntry( pPtr, (sal_uInt8*)"_width", pBuf + nSize - pPtr, 6 ) )
			{
				rFormatExtension = UniString::CreateFromAscii( "XBM", 3 );
				delete[] pBuf;
				return sal_True;
			}
		}
		delete[] pBuf;
	}
	else if( rFormatExtension.CompareToAscii( "XBM", 3 ) == COMPARE_EQUAL )
	{
		bSomethingTested = sal_True;
		return sal_True;
	}

    //--------------------------- SVG ------------------------------------
	if( !bTest )
	{
        // check for Xml
        if( ImplSearchEntry( sFirstBytes, (sal_uInt8*)"<?xml", 256, 5 ) // is it xml
            && ImplSearchEntry( sFirstBytes, (sal_uInt8*)"version", 256, 7 )) // does it have a version (required for xml)
        {
            bool bIsSvg(false);

            // check for DOCTYPE svg combination
            if( ImplSearchEntry( sFirstBytes, (sal_uInt8*)"DOCTYPE", 256, 7 ) // 'DOCTYPE' is there
                && ImplSearchEntry( sFirstBytes, (sal_uInt8*)"svg", 256, 3 )) // 'svg' is there
            {
                bIsSvg = true;
            }

            // check for svg element in 1st 256 bytes
            if(!bIsSvg && ImplSearchEntry( sFirstBytes, (sal_uInt8*)"<svg", 256, 4 )) // '<svg'
            {
                bIsSvg = true;
            }

            if(!bIsSvg)
            {
                // it's a xml, look for '<svg' in full file. Should not happen too
                // often since the tests above will handle most cases, but can happen
                // with Svg files containing big comment headers or Svg as the host
                // language
                const sal_uLong nSize((nStreamLen > 2048) ? 2048 : nStreamLen);
                sal_uInt8* pBuf = new sal_uInt8[nSize];

                rStream.Seek(nStreamPos);
                rStream.Read(pBuf, nSize);

                if(ImplSearchEntry(pBuf, (sal_uInt8*)"<svg", nSize, 4)) // '<svg'
                {
                    bIsSvg = true;
                }

                delete[] pBuf;
            }

            if(bIsSvg)
            {
                rFormatExtension = UniString::CreateFromAscii( "SVG", 3 );
                return sal_True;
            }
        }
        else
        {
            // #119176# Svg files which have no xml header at all have shown up,
            // detect those, too
            bool bIsSvg(false);

            // check for svg element in 1st 256 bytes
            if(ImplSearchEntry( sFirstBytes, (sal_uInt8*)"<svg", 256, 4 )) // '<svg'
            {
                bIsSvg = true;
            }

            if(!bIsSvg)
            {
                // look for '<svg' in full file. Should not happen too
                // often since the tests above will handle most cases, but can happen
                // with Svg files containing big comment headers or Svg as the host
                // language
                const sal_uLong nSize((nStreamLen > 2048) ? 2048 : nStreamLen);
                sal_uInt8* pBuf = new sal_uInt8[nSize];

                rStream.Seek(nStreamPos);
                rStream.Read(pBuf, nSize);

                if(ImplSearchEntry(pBuf, (sal_uInt8*)"<svg", nSize, 4)) // '<svg'
                {
                    bIsSvg = true;
                }

                delete[] pBuf;
            }

            if(bIsSvg)
            {
                rFormatExtension = UniString::CreateFromAscii( "SVG", 3 );
                return sal_True;
            }
        }
	}
	else if( rFormatExtension.CompareToAscii( "SVG", 3 ) == COMPARE_EQUAL )
	{
		bSomethingTested = sal_True;
		return sal_True;
	}

	//--------------------------- TGA ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "TGA", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested = sal_True;

		// just a simple test for the extension
        if( rFormatExtension.CompareToAscii( "TGA", 3 ) == COMPARE_EQUAL )
			return sal_True;
	}

	//--------------------------- SGV ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "SGV", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested = sal_True;

		// just a simple test for the extension
        if( rFormatExtension.CompareToAscii( "SGV", 3 ) == COMPARE_EQUAL )
			return sal_True;
	}

	//--------------------------- SGF ------------------------------------
	if( !bTest || ( rFormatExtension.CompareToAscii( "SGF", 3 ) == COMPARE_EQUAL ) )
	{
		bSomethingTested=sal_True;
		if( sFirstBytes[ 0 ] == 'J' && sFirstBytes[ 1 ] == 'J' )
		{
			rFormatExtension = UniString::CreateFromAscii( "SGF", 3 );
			return sal_True;
		}
	}

	return bTest && !bSomethingTested;
}

//--------------------------------------------------------------------------

sal_uInt16 GraphicFilter::ImpTestOrFindFormat( const String& rPath, SvStream& rStream, sal_uInt16& rFormat )
{
	sal_uInt16 n = pConfig->GetImportFormatCount();

	// ggf. Filter bzw. Format durch anlesen ermitteln,
	// oder durch anlesen zusichern, dass das Format stimmt:
	if( rFormat == GRFILTER_FORMAT_DONTKNOW )
	{
		String aFormatExt;
		if( ImpPeekGraphicFormat( rStream, aFormatExt, sal_False ) )
		{
			for( sal_uInt16 i = 0; i < n; i++ )
			{
				if( pConfig->GetImportFormatExtension( i ).EqualsIgnoreCaseAscii( aFormatExt ) )
				{
					rFormat = i;
					return GRFILTER_OK;
				}
			}
		}
		// ggf. Filter anhand der Datei-Endung raussuchen:
		if( rPath.Len() )
		{
			String aExt( ImpGetExtension( rPath ) );
			for( sal_uInt16 i = 0; i < n; i++ )
			{
				if( pConfig->GetImportFormatExtension( i ).EqualsIgnoreCaseAscii( aExt ) )
				{
					rFormat = i;
					return GRFILTER_OK;
				}
			}
		}
		return GRFILTER_FORMATERROR;
	}
	else
	{
        String aTmpStr( pConfig->GetImportFormatExtension( rFormat ) );
		if( !ImpPeekGraphicFormat( rStream, aTmpStr, sal_True ) )
			return GRFILTER_FORMATERROR;
        if ( pConfig->GetImportFormatExtension( rFormat ).EqualsIgnoreCaseAscii( "pcd" ) )
        {
            sal_Int32 nBase = 2;    // default Base0
            if ( pConfig->GetImportFilterType( rFormat ).EqualsIgnoreCaseAscii( "pcd_Photo_CD_Base4" ) )
                nBase = 1;
            else if ( pConfig->GetImportFilterType( rFormat ).EqualsIgnoreCaseAscii( "pcd_Photo_CD_Base16" ) )
                nBase = 0;
		    String aFilterConfigPath( RTL_CONSTASCII_USTRINGPARAM( "Office.Common/Filter/Graphic/Import/PCD" ) );
            FilterConfigItem aFilterConfigItem( aFilterConfigPath );
            aFilterConfigItem.WriteInt32( String( RTL_CONSTASCII_USTRINGPARAM( "Resolution" ) ), nBase );
        }
	}

	return GRFILTER_OK;
}

//--------------------------------------------------------------------------

static Graphic ImpGetScaledGraphic( const Graphic& rGraphic, FilterConfigItem& rConfigItem )
{
	Graphic		aGraphic;
	ByteString	aResMgrName( "svt", 3 );
	ResMgr*		pResMgr;

	pResMgr = ResMgr::CreateResMgr( aResMgrName.GetBuffer(), Application::GetSettings().GetUILocale() );

    sal_Int32 nLogicalWidth = rConfigItem.ReadInt32( String( RTL_CONSTASCII_USTRINGPARAM( "LogicalWidth" ) ), 0 );
	sal_Int32 nLogicalHeight = rConfigItem.ReadInt32( String( RTL_CONSTASCII_USTRINGPARAM( "LogicalHeight" ) ), 0 );

	if ( rGraphic.GetType() != GRAPHIC_NONE )
	{
        sal_Int32 nMode = rConfigItem.ReadInt32( String( RTL_CONSTASCII_USTRINGPARAM( "ExportMode" ) ), -1 );

		if ( nMode == -1 )	// the property is not there, this is possible, if the graphic filter
		{					// is called via UnoGraphicExporter and not from a graphic export Dialog
			nMode = 0;		// then we are defaulting this mode to 0
			if ( nLogicalWidth || nLogicalHeight )
				nMode = 2;
		}


		Size aOriginalSize;
		Size aPrefSize( rGraphic.GetPrefSize() );
		MapMode aPrefMapMode( rGraphic.GetPrefMapMode() );
		if ( aPrefMapMode == MAP_PIXEL )
			aOriginalSize = Application::GetDefaultDevice()->PixelToLogic( aPrefSize, MAP_100TH_MM );
		else
			aOriginalSize = Application::GetDefaultDevice()->LogicToLogic( aPrefSize, aPrefMapMode, MAP_100TH_MM );
		if ( !nLogicalWidth )
			nLogicalWidth = aOriginalSize.Width();
		if ( !nLogicalHeight )
			nLogicalHeight = aOriginalSize.Height();
		if( rGraphic.GetType() == GRAPHIC_BITMAP )
		{

            // Aufloesung wird eingestellt
			if( nMode == 1 )
			{
				Bitmap 		aBitmap( rGraphic.GetBitmap() );
				MapMode		aMap( MAP_100TH_INCH );

                sal_Int32   nDPI = rConfigItem.ReadInt32( String( RTL_CONSTASCII_USTRINGPARAM( "Resolution" ) ), 75 );
				Fraction	aFrac( 1, Min( Max( nDPI, sal_Int32( 75 ) ), sal_Int32( 600 ) ) );

				aMap.SetScaleX( aFrac );
				aMap.SetScaleY( aFrac );

				Size aOldSize = aBitmap.GetSizePixel();
				aGraphic = rGraphic;
				aGraphic.SetPrefMapMode( aMap );
				aGraphic.SetPrefSize( Size( aOldSize.Width() * 100,
										   aOldSize.Height() * 100 ) );
			}
			// Groesse wird eingestellt
			else if( nMode == 2 )
			{
				aGraphic = rGraphic;
				aGraphic.SetPrefMapMode( MapMode( MAP_100TH_MM ) );
				aGraphic.SetPrefSize( Size( nLogicalWidth, nLogicalHeight ) );
			}
			else
				aGraphic = rGraphic;

            sal_Int32 nColors = rConfigItem.ReadInt32( String( RTL_CONSTASCII_USTRINGPARAM( "Color" ) ), 0 ); // #92767#
            if ( nColors )  // graphic conversion necessary ?
            {
                BitmapEx aBmpEx( aGraphic.GetBitmapEx() );
                aBmpEx.Convert( (BmpConversion)nColors );   // the entries in the xml section have the same meaning as
                aGraphic = aBmpEx;                          // they have in the BmpConversion enum, so it should be
            }                                               // allowed to cast them
		}
		else
		{
			if( ( nMode == 1 ) || ( nMode == 2 ) )
			{
				GDIMetaFile	aMtf( rGraphic.GetGDIMetaFile() );
				::com::sun::star::awt::Size aDefaultSize( 10000, 10000 );
				Size aNewSize( OutputDevice::LogicToLogic( Size( nLogicalWidth, nLogicalHeight ), MAP_100TH_MM, aMtf.GetPrefMapMode() ) );

				if( aNewSize.Width() && aNewSize.Height() )
				{
					const Size aPreferredSize( aMtf.GetPrefSize() );
					aMtf.Scale( Fraction( aNewSize.Width(), aPreferredSize.Width() ),
								Fraction( aNewSize.Height(), aPreferredSize.Height() ) );
				}
				aGraphic = Graphic( aMtf );
			}
			else
				aGraphic = rGraphic;
		}

	}
	else
		aGraphic = rGraphic;

	delete pResMgr;

	return aGraphic;
}

static String ImpCreateFullFilterPath( const String& rPath, const String& rFilterName )
{
	::rtl::OUString	aPathURL;

	::osl::FileBase::getFileURLFromSystemPath( rPath, aPathURL );
	aPathURL += String( '/' );

	::rtl::OUString	aSystemPath;
	::osl::FileBase::getSystemPathFromFileURL( aPathURL, aSystemPath );
	aSystemPath += ::rtl::OUString( rFilterName );

	return String( aSystemPath );
}


// --------------------------
// - ImpFilterLibCacheEntry -
// --------------------------

class ImpFilterLibCache;

struct ImpFilterLibCacheEntry
{
	ImpFilterLibCacheEntry*	mpNext;
	osl::Module			    maLibrary;
	String					maFiltername;
	PFilterCall				mpfnImport;
	PFilterDlgCall			mpfnImportDlg;

							ImpFilterLibCacheEntry( const String& rPathname, const String& rFiltername );
	int						operator==( const String& rFiltername ) const { return maFiltername == rFiltername; }

	PFilterCall				GetImportFunction();
	PFilterDlgCall			GetImportDlgFunction();
	PFilterCall				GetExportFunction() { return (PFilterCall) maLibrary.getFunctionSymbol( UniString::CreateFromAscii( EXPORT_FUNCTION_NAME ) ); }
	PFilterDlgCall			GetExportDlgFunction() { return (PFilterDlgCall) maLibrary.getFunctionSymbol( UniString::CreateFromAscii( EXPDLG_FUNCTION_NAME ) ); }
};

// ------------------------------------------------------------------------

ImpFilterLibCacheEntry::ImpFilterLibCacheEntry( const String& rPathname, const String& rFiltername ) :
		mpNext			( NULL ),
		maLibrary		( rPathname ),
		maFiltername	( rFiltername ),
		mpfnImport		( NULL ),
		mpfnImportDlg	( NULL )
{
}

// ------------------------------------------------------------------------

PFilterCall ImpFilterLibCacheEntry::GetImportFunction()
{
	if( !mpfnImport )
		mpfnImport = (PFilterCall) maLibrary.getFunctionSymbol( UniString::CreateFromAscii( IMPORT_FUNCTION_NAME ) );

	return mpfnImport;
}

// ------------------------------------------------------------------------

PFilterDlgCall ImpFilterLibCacheEntry::GetImportDlgFunction()
{
	if( !mpfnImportDlg )
		mpfnImportDlg = (PFilterDlgCall)maLibrary.getFunctionSymbol( UniString::CreateFromAscii( IMPDLG_FUNCTION_NAME ) );

	return mpfnImportDlg;
}

// ---------------------
// - ImpFilterLibCache -
// ---------------------

class ImpFilterLibCache
{
	ImpFilterLibCacheEntry*	mpFirst;
	ImpFilterLibCacheEntry*	mpLast;

public:
							ImpFilterLibCache();
							~ImpFilterLibCache();

	ImpFilterLibCacheEntry*	GetFilter( const String& rFilterPath, const String& rFiltername );
};

// ------------------------------------------------------------------------

ImpFilterLibCache::ImpFilterLibCache() :
	mpFirst		( NULL ),
	mpLast		( NULL )
{
}

// ------------------------------------------------------------------------

ImpFilterLibCache::~ImpFilterLibCache()
{
	ImpFilterLibCacheEntry*	pEntry = mpFirst;
	while( pEntry )
	{
		ImpFilterLibCacheEntry* pNext = pEntry->mpNext;
		delete pEntry;
		pEntry = pNext;
	}
}

// ------------------------------------------------------------------------

ImpFilterLibCacheEntry* ImpFilterLibCache::GetFilter( const String& rFilterPath, const String& rFilterName )
{
	ImpFilterLibCacheEntry*	pEntry = mpFirst;

	while( pEntry )
	{
		if( *pEntry == rFilterName )
			break;
		else
			pEntry = pEntry->mpNext;
	}
	if( !pEntry )
	{
		String aPhysicalName( ImpCreateFullFilterPath( rFilterPath, rFilterName ) );
		pEntry = new ImpFilterLibCacheEntry( aPhysicalName, rFilterName );

		if ( pEntry->maLibrary.is() )
		{
			if( !mpFirst )
				mpFirst = mpLast = pEntry;
			else
				mpLast = mpLast->mpNext = pEntry;
		}
		else
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
	return pEntry;
};

// ------------------------------------------------------------------------

namespace { struct Cache : public rtl::Static<ImpFilterLibCache, Cache> {}; }

// -----------------
// - GraphicFilter -
// -----------------

GraphicFilter::GraphicFilter( sal_Bool bConfig ) :
	bUseConfig	      ( bConfig ),
	nExpGraphHint     ( 0 )
{
	ImplInit();
}

// ------------------------------------------------------------------------

GraphicFilter::~GraphicFilter()
{
	{
		::osl::MutexGuard aGuard( getListMutex() );
		pFilterHdlList->Remove( (void*)this );
		if ( !pFilterHdlList->Count() )
		{
			delete pFilterHdlList, pFilterHdlList = NULL;
    		delete pConfig;
		}
	}


	delete pErrorEx;
}

// ------------------------------------------------------------------------

void GraphicFilter::ImplInit()
{
	{
		::osl::MutexGuard aGuard( getListMutex() );

		if ( !pFilterHdlList )
		{
			pFilterHdlList = new List;
			pConfig = new FilterConfigCache( bUseConfig );
		}
		else
			pConfig = ((GraphicFilter*)pFilterHdlList->First())->pConfig;

		pFilterHdlList->Insert( (void*)this );
	}

    if( bUseConfig )
    {
        rtl::OUString url(RTL_CONSTASCII_USTRINGPARAM("$OOO_BASE_DIR/program"));
        rtl::Bootstrap::expandMacros(url); //TODO: detect failure
        utl::LocalFileHelper::ConvertURLToPhysicalName(url, aFilterPath);
    }

	pErrorEx = new FilterErrorEx;
	bAbort = sal_False;
}

// ------------------------------------------------------------------------

sal_uLong GraphicFilter::ImplSetError( sal_uLong nError, const SvStream* pStm )
{
	pErrorEx->nFilterError = nError;
	pErrorEx->nStreamError = pStm ? pStm->GetError() : ERRCODE_NONE;
	return nError;
}
// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::GetImportFormatCount()
{
	return pConfig->GetImportFormatCount();
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::GetImportFormatNumber( const String& rFormatName )
{
	return pConfig->GetImportFormatNumber( rFormatName );
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::GetImportFormatNumberForMediaType( const String& rMediaType )
{
	return pConfig->GetImportFormatNumberForMediaType( rMediaType );
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::GetImportFormatNumberForShortName( const String& rShortName )
{
	return pConfig->GetImportFormatNumberForShortName( rShortName );
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::GetImportFormatNumberForTypeName( const String& rType )
{
	return pConfig->GetImportFormatNumberForTypeName( rType );
}

// ------------------------------------------------------------------------

String GraphicFilter::GetImportFormatName( sal_uInt16 nFormat )
{
	return pConfig->GetImportFormatName( nFormat );
}

// ------------------------------------------------------------------------

String GraphicFilter::GetImportFormatTypeName( sal_uInt16 nFormat )
{
	return pConfig->GetImportFilterTypeName( nFormat );
}

// ------------------------------------------------------------------------

String GraphicFilter::GetImportFormatMediaType( sal_uInt16 nFormat )
{
	return pConfig->GetImportFormatMediaType( nFormat );
}

// ------------------------------------------------------------------------

String GraphicFilter::GetImportFormatShortName( sal_uInt16 nFormat )
{
	return pConfig->GetImportFormatShortName( nFormat );
}

// ------------------------------------------------------------------------

String GraphicFilter::GetImportOSFileType( sal_uInt16 )
{
	String aOSFileType;
	return aOSFileType;
}

// ------------------------------------------------------------------------

String GraphicFilter::GetImportWildcard( sal_uInt16 nFormat, sal_Int32 nEntry )
{
	return pConfig->GetImportWildcard( nFormat, nEntry );
}

// ------------------------------------------------------------------------

sal_Bool GraphicFilter::IsImportPixelFormat( sal_uInt16 nFormat )
{
	return pConfig->IsImportPixelFormat( nFormat );
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::GetExportFormatCount()
{
	return pConfig->GetExportFormatCount();
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::GetExportFormatNumber( const String& rFormatName )
{
	return pConfig->GetExportFormatNumber( rFormatName );
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::GetExportFormatNumberForMediaType( const String& rMediaType )
{
	return pConfig->GetExportFormatNumberForMediaType( rMediaType );
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::GetExportFormatNumberForShortName( const String& rShortName )
{
	return pConfig->GetExportFormatNumberForShortName( rShortName );
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::GetExportFormatNumberForTypeName( const String& rType )
{
	return pConfig->GetExportFormatNumberForTypeName( rType );
}

// ------------------------------------------------------------------------

String GraphicFilter::GetExportFormatName( sal_uInt16 nFormat )
{
	return pConfig->GetExportFormatName( nFormat );
}

// ------------------------------------------------------------------------

String GraphicFilter::GetExportFormatTypeName( sal_uInt16 nFormat )
{
	return pConfig->GetExportFilterTypeName( nFormat );
}

// ------------------------------------------------------------------------

String GraphicFilter::GetExportFormatMediaType( sal_uInt16 nFormat )
{
	return pConfig->GetExportFormatMediaType( nFormat );
}

// ------------------------------------------------------------------------

String GraphicFilter::GetExportFormatShortName( sal_uInt16 nFormat )
{
	return pConfig->GetExportFormatShortName( nFormat );
}

// ------------------------------------------------------------------------

String GraphicFilter::GetExportOSFileType( sal_uInt16 )
{
	String aOSFileType;
	return aOSFileType;
}

// ------------------------------------------------------------------------

String GraphicFilter::GetExportWildcard( sal_uInt16 nFormat, sal_Int32 nEntry )
{
	return pConfig->GetExportWildcard( nFormat, nEntry );
}

// ------------------------------------------------------------------------

sal_Bool GraphicFilter::IsExportPixelFormat( sal_uInt16 nFormat )
{
	return pConfig->IsExportPixelFormat( nFormat );
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::CanImportGraphic( const INetURLObject& rPath,
										sal_uInt16 nFormat, sal_uInt16* pDeterminedFormat )
{
	sal_uInt16	nRetValue = GRFILTER_FORMATERROR;
	DBG_ASSERT( rPath.GetProtocol() != INET_PROT_NOT_VALID, "GraphicFilter::CanImportGraphic() : ProtType == INET_PROT_NOT_VALID" );

	String		aMainUrl( rPath.GetMainURL( INetURLObject::NO_DECODE ) );
	SvStream*	pStream = ::utl::UcbStreamHelper::CreateStream( aMainUrl, STREAM_READ | STREAM_SHARE_DENYNONE );
	if ( pStream )
	{
		nRetValue = CanImportGraphic( aMainUrl, *pStream, nFormat, pDeterminedFormat );
		delete pStream;
	}
	return nRetValue;
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::CanImportGraphic( const String& rMainUrl, SvStream& rIStream,
										sal_uInt16 nFormat, sal_uInt16* pDeterminedFormat )
{
	sal_uLong nStreamPos = rIStream.Tell();
	sal_uInt16 nRes = ImpTestOrFindFormat( rMainUrl, rIStream, nFormat );

	rIStream.Seek(nStreamPos);

	if( nRes==GRFILTER_OK && pDeterminedFormat!=NULL )
		*pDeterminedFormat = nFormat;

	return (sal_uInt16) ImplSetError( nRes, &rIStream );
}

// ------------------------------------------------------------------------
//SJ: TODO, we need to create a GraphicImporter component
sal_uInt16 GraphicFilter::ImportGraphic( Graphic& rGraphic, const INetURLObject& rPath,
									 sal_uInt16 nFormat, sal_uInt16 * pDeterminedFormat, sal_uInt32 nImportFlags )
{
	sal_uInt16 nRetValue = GRFILTER_FORMATERROR;
	DBG_ASSERT( rPath.GetProtocol() != INET_PROT_NOT_VALID, "GraphicFilter::ImportGraphic() : ProtType == INET_PROT_NOT_VALID" );

	String		aMainUrl( rPath.GetMainURL( INetURLObject::NO_DECODE ) );
	SvStream*	pStream = ::utl::UcbStreamHelper::CreateStream( aMainUrl, STREAM_READ | STREAM_SHARE_DENYNONE );
	if ( pStream )
	{
		nRetValue = ImportGraphic( rGraphic, aMainUrl, *pStream, nFormat, pDeterminedFormat, nImportFlags );
		delete pStream;
	}
	return nRetValue;
}

sal_uInt16 GraphicFilter::ImportGraphic( Graphic& rGraphic, const String& rPath, SvStream& rIStream,
									 sal_uInt16 nFormat, sal_uInt16* pDeterminedFormat, sal_uInt32 nImportFlags )
{
	return ImportGraphic( rGraphic, rPath, rIStream, nFormat, pDeterminedFormat, nImportFlags, NULL );
}

//-------------------------------------------------------------------------

sal_uInt16 GraphicFilter::ImportGraphic( Graphic& rGraphic, const String& rPath, SvStream& rIStream,
									 sal_uInt16 nFormat, sal_uInt16* pDeterminedFormat, sal_uInt32 nImportFlags,
									 com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue >* pFilterData )
{
	String					aFilterName;
	sal_uLong					nStmBegin;
	sal_uInt16					nStatus;
	GraphicReader*			pContext = rGraphic.GetContext();
	GfxLinkType				eLinkType = GFX_LINK_TYPE_NONE;
	sal_Bool					bDummyContext = ( pContext == (GraphicReader*) 1 );
	const sal_Bool				bLinkSet = rGraphic.IsLink();
    FilterConfigItem*       pFilterConfigItem = NULL;

	Size					aPreviewSizeHint( 0, 0 );
	sal_Bool				bAllowPartialStreamRead = sal_False;
	sal_Bool				bCreateNativeLink = sal_True;

	ResetLastError();

	if ( pFilterData )
	{
		sal_Int32 i;
		for ( i = 0; i < pFilterData->getLength(); i++ )
		{
			if ( (*pFilterData)[ i ].Name.equalsAscii( "PreviewSizeHint" ) )
			{
				awt::Size aSize;
				if ( (*pFilterData)[ i ].Value >>= aSize )
				{
					aPreviewSizeHint = Size( aSize.Width, aSize.Height );
					if ( aSize.Width || aSize.Height )
						nImportFlags |= GRFILTER_I_FLAGS_FOR_PREVIEW;
					else
						nImportFlags &=~GRFILTER_I_FLAGS_FOR_PREVIEW;
				}
			}
			else if ( (*pFilterData)[ i ].Name.equalsAscii( "AllowPartialStreamRead" ) )
			{
				(*pFilterData)[ i ].Value >>= bAllowPartialStreamRead;
				if ( bAllowPartialStreamRead )
					nImportFlags |= GRFILTER_I_FLAGS_ALLOW_PARTIAL_STREAMREAD;
				else
					nImportFlags &=~GRFILTER_I_FLAGS_ALLOW_PARTIAL_STREAMREAD;
			}
			else if ( (*pFilterData)[ i ].Name.equalsAscii( "CreateNativeLink" ) )
			{
				(*pFilterData)[ i ].Value >>= bCreateNativeLink;
			}
		}
	}

	if( !pContext || bDummyContext )
	{
		if( bDummyContext )
		{
			rGraphic.SetContext( NULL );
			nStmBegin = 0;
		}
		else
			nStmBegin = rIStream.Tell();

		bAbort = sal_False;
		nStatus = ImpTestOrFindFormat( rPath, rIStream, nFormat );
		// Falls Pending, geben wir GRFILTER_OK zurueck,
		// um mehr Bytes anzufordern
		if( rIStream.GetError() == ERRCODE_IO_PENDING )
		{
			rGraphic.SetContext( (GraphicReader*) 1 );
			rIStream.ResetError();
			rIStream.Seek( nStmBegin );
			return (sal_uInt16) ImplSetError( GRFILTER_OK );
		}

		rIStream.Seek( nStmBegin );

		if( ( nStatus != GRFILTER_OK ) || rIStream.GetError() )
			return (sal_uInt16) ImplSetError( ( nStatus != GRFILTER_OK ) ? nStatus : GRFILTER_OPENERROR, &rIStream );

		if( pDeterminedFormat )
			*pDeterminedFormat = nFormat;

		aFilterName = pConfig->GetImportFilterName( nFormat );
	}
	else
	{
		if( pContext && !bDummyContext )
			aFilterName = pContext->GetUpperFilterName();

		nStmBegin = 0;
		nStatus = GRFILTER_OK;
	}

	// read graphic
	if ( pConfig->IsImportInternalFilter( nFormat ) )
	{
		if( aFilterName.EqualsIgnoreCaseAscii( IMP_GIF )  )
		{
			if( rGraphic.GetContext() == (GraphicReader*) 1 )
				rGraphic.SetContext( NULL );

			if( !ImportGIF( rIStream, rGraphic ) )
				nStatus = GRFILTER_FILTERERROR;
			else
				eLinkType = GFX_LINK_TYPE_NATIVE_GIF;
		}
		else if( aFilterName.EqualsIgnoreCaseAscii( IMP_PNG ) )
		{
			if ( rGraphic.GetContext() == (GraphicReader*) 1 )
				rGraphic.SetContext( NULL );

            vcl::PNGReader aPNGReader( rIStream );

            // ignore animation for previews and set preview size
            if( aPreviewSizeHint.Width() || aPreviewSizeHint.Height() )
            {
                // position the stream at the end of the image if requested
                if( !bAllowPartialStreamRead )
                    aPNGReader.GetChunks();
            }
            else
            {
                // check if this PNG contains a GIF chunk!
                const std::vector< vcl::PNGReader::ChunkData >&    rChunkData = aPNGReader.GetChunks();
                std::vector< vcl::PNGReader::ChunkData >::const_iterator aIter( rChunkData.begin() );
                std::vector< vcl::PNGReader::ChunkData >::const_iterator aEnd ( rChunkData.end() );
                while( aIter != aEnd )
                {
                    // Microsoft Office is storing Animated GIFs in following chunk
                    if ( aIter->nType == PMGCHUNG_msOG )
                    {
                        sal_uInt32 nChunkSize = aIter->aData.size();
                        if ( nChunkSize > 11 )
                        {
                            const std::vector< sal_uInt8 >& rData = aIter->aData;
                            SvMemoryStream aIStrm( (void*)&rData[ 11 ], nChunkSize - 11, STREAM_READ );
                            ImportGIF( aIStrm, rGraphic );
                            eLinkType = GFX_LINK_TYPE_NATIVE_PNG;
                            break;
                        }
                    }
                    aIter++;
                }
            }

			if ( eLinkType == GFX_LINK_TYPE_NONE )
			{
				BitmapEx aBmpEx( aPNGReader.Read( aPreviewSizeHint ) );
				if ( aBmpEx.IsEmpty() )
					nStatus = GRFILTER_FILTERERROR;
				else
				{
					rGraphic = aBmpEx;
					eLinkType = GFX_LINK_TYPE_NATIVE_PNG;
				}
			}
		}
		else if( aFilterName.EqualsIgnoreCaseAscii( IMP_JPEG ) )
		{
			if( rGraphic.GetContext() == (GraphicReader*) 1 )
				rGraphic.SetContext( NULL );

            // set LOGSIZE flag always, if not explicitly disabled
            // (see #90508 and #106763)
            if( 0 == ( nImportFlags & GRFILTER_I_FLAGS_DONT_SET_LOGSIZE_FOR_JPEG ) )
                nImportFlags |= GRFILTER_I_FLAGS_SET_LOGSIZE_FOR_JPEG;

			if( !ImportJPEG( rIStream, rGraphic, NULL, nImportFlags ) )
				nStatus = GRFILTER_FILTERERROR;
			else
				eLinkType = GFX_LINK_TYPE_NATIVE_JPG;
		}
		else if( aFilterName.EqualsIgnoreCaseAscii( IMP_SVG ) )
		{
			if( rGraphic.GetContext() == (GraphicReader*) 1 )
				rGraphic.SetContext( NULL );

            const sal_uInt32 nStmPos(rIStream.Tell());
            const sal_uInt32 nStmLen(rIStream.Seek(STREAM_SEEK_TO_END) - nStmPos);
            bool bOkay(false);

            if(nStmLen)
            {
                SvgDataArray aNewData(new sal_uInt8[nStmLen]);

                rIStream.Seek(nStmPos);
                rIStream.Read(aNewData.get(), nStmLen);

                if(!rIStream.GetError())
                {
                    SvgDataPtr aSvgDataPtr(
                        new SvgData(
                            aNewData,
                            nStmLen,
                            rPath));

                    rGraphic = Graphic(aSvgDataPtr);
                    bOkay = true;
                }
            }

            if(bOkay)
            {
                eLinkType = GFX_LINK_TYPE_NATIVE_SVG;
            }
            else
            {
                nStatus = GRFILTER_FILTERERROR;
            }
		}
		else if( aFilterName.EqualsIgnoreCaseAscii( IMP_XBM ) )
		{
			if( rGraphic.GetContext() == (GraphicReader*) 1 )
				rGraphic.SetContext( NULL );

			if( !ImportXBM( rIStream, rGraphic ) )
				nStatus = GRFILTER_FILTERERROR;
		}
		else if( aFilterName.EqualsIgnoreCaseAscii( IMP_XPM ) )
		{
			if( rGraphic.GetContext() == (GraphicReader*) 1 )
				rGraphic.SetContext( NULL );

			if( !ImportXPM( rIStream, rGraphic ) )
				nStatus = GRFILTER_FILTERERROR;
		}
        else if ( aFilterName.EqualsIgnoreCaseAscii( IMP_BMP )
                  || aFilterName.EqualsIgnoreCaseAscii( IMP_SVMETAFILE ) )
        {
            // SV interne Importfilter fuer Bitmaps und MetaFiles
            rIStream >> rGraphic;

            if( rIStream.GetError() )
            {
                nStatus = GRFILTER_FORMATERROR;
            }
            else
            {
                if ( aFilterName.EqualsIgnoreCaseAscii( IMP_BMP ) )
                {
                    // #15508# added BMP type (checked, works)
                    eLinkType = GFX_LINK_TYPE_NATIVE_BMP;
                }
            }
        }
        else if( aFilterName.EqualsIgnoreCaseAscii( IMP_WMF ) ||
				aFilterName.EqualsIgnoreCaseAscii( IMP_EMF ) )
		{
			GDIMetaFile aMtf;
			if( !ConvertWMFToGDIMetaFile( rIStream, aMtf, NULL ) )
				nStatus = GRFILTER_FORMATERROR;
			else
			{
				rGraphic = aMtf;
				eLinkType = GFX_LINK_TYPE_NATIVE_WMF;
			}
		}
		else if( aFilterName.EqualsIgnoreCaseAscii( IMP_SVSGF )
				|| aFilterName.EqualsIgnoreCaseAscii( IMP_SVSGV ) )
		{
			sal_uInt16			nVersion;
			unsigned char	nTyp = CheckSgfTyp( rIStream, nVersion );

			switch( nTyp )
			{
				case SGF_BITIMAGE:
				{
					SvMemoryStream aTempStream;
					if( aTempStream.GetError() )
						return GRFILTER_OPENERROR;

					if( !SgfBMapFilter( rIStream, aTempStream ) )
						nStatus = GRFILTER_FILTERERROR;
					else
					{
						aTempStream.Seek( 0L );
						aTempStream >> rGraphic;

						if( aTempStream.GetError() )
							nStatus = GRFILTER_FILTERERROR;
					}
				}
				break;

				case SGF_SIMPVECT:
				{
					GDIMetaFile aMtf;
					if( !SgfVectFilter( rIStream, aMtf ) )
						nStatus = GRFILTER_FILTERERROR;
					else
						rGraphic = Graphic( aMtf );
				}
				break;

				case SGF_STARDRAW:
				{
					if( nVersion != SGV_VERSION )
						nStatus = GRFILTER_VERSIONERROR;
					else
					{
						GDIMetaFile aMtf;
						if( !SgfSDrwFilter( rIStream, aMtf,
								INetURLObject(aFilterPath) ) )
						{
							nStatus = GRFILTER_FILTERERROR;
						}
						else
							rGraphic = Graphic( aMtf );
					}
				}
				break;

				default:
				{
					nStatus = GRFILTER_FORMATERROR;
				}
				break;
			}
		}
		else
			nStatus = GRFILTER_FILTERERROR;
	}
	else
	{
		ImpFilterLibCacheEntry*	pFilter = NULL;

		// find first filter in filter pathes
		xub_StrLen i, nTokenCount = aFilterPath.GetTokenCount( ';' );
		ImpFilterLibCache &rCache = Cache::get();
		for( i = 0; ( i < nTokenCount ) && ( pFilter == NULL ); i++ )
			pFilter = rCache.GetFilter( aFilterPath.GetToken(i), aFilterName );
		if( !pFilter )
			nStatus = GRFILTER_FILTERERROR;
		else
		{
			PFilterCall pFunc = pFilter->GetImportFunction();

			if( !pFunc )
				nStatus = GRFILTER_FILTERERROR;
			else
			{
                String aShortName;
                if( nFormat != GRFILTER_FORMAT_DONTKNOW )
                {
                    aShortName = GetImportFormatShortName( nFormat ).ToUpperAscii();
                    if ( ( pFilterConfigItem == NULL ) && aShortName.EqualsAscii( "PCD" ) )
                    {
		                String aFilterConfigPath( RTL_CONSTASCII_USTRINGPARAM( "Office.Common/Filter/Graphic/Import/PCD" ) );
                        pFilterConfigItem = new FilterConfigItem( aFilterConfigPath );
                    }
                }
				if( !(*pFunc)( rIStream, rGraphic, pFilterConfigItem, sal_False ) )
					nStatus = GRFILTER_FORMATERROR;
				else
				{
					// try to set link type if format matches
					if( nFormat != GRFILTER_FORMAT_DONTKNOW )
					{
						if( aShortName.CompareToAscii( TIF_SHORTNAME ) == COMPARE_EQUAL )
							eLinkType = GFX_LINK_TYPE_NATIVE_TIF;
						else if( aShortName.CompareToAscii( MET_SHORTNAME ) == COMPARE_EQUAL )
							eLinkType = GFX_LINK_TYPE_NATIVE_MET;
						else if( aShortName.CompareToAscii( PCT_SHORTNAME ) == COMPARE_EQUAL )
							eLinkType = GFX_LINK_TYPE_NATIVE_PCT;
					}
				}
			}
		}
	}

	if( nStatus == GRFILTER_OK && bCreateNativeLink && ( eLinkType != GFX_LINK_TYPE_NONE ) && !rGraphic.GetContext() && !bLinkSet )
	{
		const sal_uLong nStmEnd = rIStream.Tell();
		const sal_uLong	nBufSize = nStmEnd - nStmBegin;

		if( nBufSize )
		{
			sal_uInt8*	pBuf=0;
			try
			{
				pBuf = new sal_uInt8[ nBufSize ];
			}
	    		catch (std::bad_alloc)
			{
				nStatus = GRFILTER_TOOBIG;
			}

			if( nStatus == GRFILTER_OK )
			{
				rIStream.Seek( nStmBegin );
				rIStream.Read( pBuf, nBufSize );
				rGraphic.SetLink( GfxLink( pBuf, nBufSize, eLinkType, sal_True ) );
			}
		}
	}

	// Set error code or try to set native buffer
	if( nStatus != GRFILTER_OK )
	{
		if( bAbort )
			nStatus = GRFILTER_ABORT;

		ImplSetError( nStatus, &rIStream );
		rIStream.Seek( nStmBegin );
		rGraphic.Clear();
	}

    delete pFilterConfigItem;
	return nStatus;
}


// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::ExportGraphic( const Graphic& rGraphic, const INetURLObject& rPath,
	sal_uInt16 nFormat, const uno::Sequence< beans::PropertyValue >* pFilterData )
{
	sal_uInt16	nRetValue = GRFILTER_FORMATERROR;
	DBG_ASSERT( rPath.GetProtocol() != INET_PROT_NOT_VALID, "GraphicFilter::ExportGraphic() : ProtType == INET_PROT_NOT_VALID" );
	sal_Bool		bAlreadyExists = ImplDirEntryHelper::Exists( rPath );

	String		aMainUrl( rPath.GetMainURL( INetURLObject::NO_DECODE ) );
	SvStream*	pStream = ::utl::UcbStreamHelper::CreateStream( aMainUrl, STREAM_WRITE | STREAM_TRUNC );
	if ( pStream )
	{
		nRetValue = ExportGraphic( rGraphic, aMainUrl, *pStream, nFormat, pFilterData );
		delete pStream;

		if( ( GRFILTER_OK != nRetValue ) && !bAlreadyExists )
			ImplDirEntryHelper::Kill( aMainUrl );
	}
	return nRetValue;
}

// ------------------------------------------------------------------------

sal_uInt16 GraphicFilter::ExportGraphic( const Graphic& rGraphic, const String& rPath,
	SvStream& rOStm, sal_uInt16 nFormat, const uno::Sequence< beans::PropertyValue >* pFilterData )
{
	sal_uInt16 nFormatCount = GetExportFormatCount();

	ResetLastError();
	nExpGraphHint = 0;

	if( nFormat == GRFILTER_FORMAT_DONTKNOW )
	{
		INetURLObject aURL( rPath );
		String aExt( aURL.GetFileExtension().toAsciiUpperCase() );


		for( sal_uInt16 i = 0; i < nFormatCount; i++ )
		{
			if ( pConfig->GetExportFormatExtension( i ).EqualsIgnoreCaseAscii( aExt ) )
			{
				nFormat=i;
				break;
			}
		}
	}
	if( nFormat >= nFormatCount )
		return (sal_uInt16) ImplSetError( GRFILTER_FORMATERROR );

	FilterConfigItem aConfigItem( (uno::Sequence< beans::PropertyValue >*)pFilterData );
	String aFilterName( pConfig->GetExportFilterName( nFormat ) );

	bAbort				= sal_False;
	sal_uInt16		nStatus = GRFILTER_OK;
	GraphicType	eType;
	Graphic		aGraphic( rGraphic );

	aGraphic = ImpGetScaledGraphic( rGraphic, aConfigItem );
	eType = aGraphic.GetType();

	if( pConfig->IsExportPixelFormat( nFormat ) )
	{
		if( eType != GRAPHIC_BITMAP )
		{
			Size aSizePixel;
			sal_uLong nColorCount,nBitsPerPixel,nNeededMem,nMaxMem;
			VirtualDevice aVirDev;

			// Maximalen Speicherbedarf fuer das Bildes holen:
//			if( GetOptionsConfig() )
//				nMaxMem = (UINT32)GetOptionsConfig()->ReadKey( "VEC-TO-PIX-MAX-KB", "1024" ).ToInt32();
//			else
				nMaxMem = 1024;

			nMaxMem *= 1024; // In Bytes

			// Berechnen, wie gross das Bild normalerweise werden wuerde:
			aSizePixel=aVirDev.LogicToPixel(aGraphic.GetPrefSize(),aGraphic.GetPrefMapMode());

			// Berechnen, wieviel Speicher das Bild benoetigen wuerde:
			nColorCount=aVirDev.GetColorCount();
			if      (nColorCount<=2)     nBitsPerPixel=1;
			else if (nColorCount<=4)     nBitsPerPixel=2;
			else if (nColorCount<=16)    nBitsPerPixel=4;
			else if (nColorCount<=256)   nBitsPerPixel=8;
			else if (nColorCount<=65536) nBitsPerPixel=16;
			else                         nBitsPerPixel=24;
			nNeededMem=((sal_uLong)aSizePixel.Width()*(sal_uLong)aSizePixel.Height()*nBitsPerPixel+7)/8;

			// ggf. Groesse des Bildes einschraenken:
			if (nMaxMem<nNeededMem)
			{
				double fFak=sqrt(((double)nMaxMem)/((double)nNeededMem));
				aSizePixel.Width()=(sal_uLong)(((double)aSizePixel.Width())*fFak);
				aSizePixel.Height()=(sal_uLong)(((double)aSizePixel.Height())*fFak);
			}

			aVirDev.SetMapMode(MapMode(MAP_PIXEL));
			aVirDev.SetOutputSizePixel(aSizePixel);
			Graphic aGraphic2=aGraphic;
			aGraphic2.Draw(&aVirDev,Point(0,0),aSizePixel); // Gemein: dies aendert den MapMode
			aVirDev.SetMapMode(MapMode(MAP_PIXEL));
			aGraphic=Graphic(aVirDev.GetBitmap(Point(0,0),aSizePixel));
		}
	}
	if( rOStm.GetError() )
		nStatus = GRFILTER_IOERROR;
	if( GRFILTER_OK == nStatus )
	{
		if ( pConfig->IsExportInternalFilter( nFormat ) )
		{
			if( aFilterName.EqualsIgnoreCaseAscii( EXP_BMP ) )
			{
				Bitmap aBmp( aGraphic.GetBitmap() );
				sal_Int32 nColorRes = aConfigItem.ReadInt32( String( RTL_CONSTASCII_USTRINGPARAM( "Colors" ) ), 0 );
				if ( nColorRes && ( nColorRes <= (sal_uInt16)BMP_CONVERSION_24BIT) )
				{
					if( !aBmp.Convert( (BmpConversion) nColorRes ) )
						aBmp = aGraphic.GetBitmap();
				}
				ResMgr*		pResMgr = CREATERESMGR( svt );
                sal_Bool    bRleCoding = aConfigItem.ReadBool( String( RTL_CONSTASCII_USTRINGPARAM( "RLE_Coding" ) ), sal_True );
				// Wollen wir RLE-Kodiert speichern?
				WriteDIB(aBmp, rOStm, bRleCoding, true);
				delete pResMgr;

				if( rOStm.GetError() )
					nStatus = GRFILTER_IOERROR;
			}
			else if( aFilterName.EqualsIgnoreCaseAscii( EXP_SVMETAFILE ) )
			{
				sal_Int32 nVersion = aConfigItem.ReadInt32( String( RTL_CONSTASCII_USTRINGPARAM( "Version" ) ), 0 ) ;
				if ( nVersion )
					rOStm.SetVersion( nVersion );

                // #119735# just use GetGDIMetaFile, it will create a bufferd version of contained bitmap now automatically
				GDIMetaFile aMTF(aGraphic.GetGDIMetaFile());

				aMTF.Write( rOStm );

                if( rOStm.GetError() )
					nStatus = GRFILTER_IOERROR;
			}
			else if ( aFilterName.EqualsIgnoreCaseAscii( EXP_WMF ) )
			{
                // #119735# just use GetGDIMetaFile, it will create a bufferd version of contained bitmap now automatically
				if ( !ConvertGDIMetaFileToWMF( aGraphic.GetGDIMetaFile(), rOStm, &aConfigItem ) )
					nStatus = GRFILTER_FORMATERROR;

                if( rOStm.GetError() )
					nStatus = GRFILTER_IOERROR;
			}
			else if ( aFilterName.EqualsIgnoreCaseAscii( EXP_EMF ) )
			{
                // #119735# just use GetGDIMetaFile, it will create a bufferd version of contained bitmap now automatically
				if ( !ConvertGDIMetaFileToEMF( aGraphic.GetGDIMetaFile(), rOStm, &aConfigItem ) )
					nStatus = GRFILTER_FORMATERROR;

                if( rOStm.GetError() )
					nStatus = GRFILTER_IOERROR;
			}
			else if( aFilterName.EqualsIgnoreCaseAscii( EXP_JPEG ) )
			{
			    bool bExportedGrayJPEG = false;
				if( !ExportJPEG( rOStm, aGraphic, pFilterData, &bExportedGrayJPEG ) )
					nStatus = GRFILTER_FORMATERROR;
				nExpGraphHint = bExportedGrayJPEG ? GRFILTER_OUTHINT_GREY : 0;

				if( rOStm.GetError() )
					nStatus = GRFILTER_IOERROR;
			}
			else if ( aFilterName.EqualsIgnoreCaseAscii( EXP_PNG ) )
			{
				vcl::PNGWriter aPNGWriter( aGraphic.GetBitmapEx(), pFilterData );
				if ( pFilterData )
				{
					sal_Int32 k, j, i = 0;
					for ( i = 0; i < pFilterData->getLength(); i++ )
					{
						if ( (*pFilterData)[ i ].Name.equalsAscii( "AdditionalChunks" ) )
						{
							com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue > aAdditionalChunkSequence;
							if ( (*pFilterData)[ i ].Value >>= aAdditionalChunkSequence )
							{
								for ( j = 0; j < aAdditionalChunkSequence.getLength(); j++ )
								{
									if ( aAdditionalChunkSequence[ j ].Name.getLength() == 4 )
									{
										sal_uInt32 nChunkType = 0;
										for ( k = 0; k < 4; k++ )
										{
											nChunkType <<= 8;
											nChunkType |= (sal_uInt8)aAdditionalChunkSequence[ j ].Name[ k ];
										}
										com::sun::star::uno::Sequence< sal_Int8 > aByteSeq;
										if ( aAdditionalChunkSequence[ j ].Value >>= aByteSeq )
										{
											std::vector< vcl::PNGWriter::ChunkData >& rChunkData = aPNGWriter.GetChunks();
											if ( rChunkData.size() )
											{
												sal_uInt32 nChunkLen = aByteSeq.getLength();

												vcl::PNGWriter::ChunkData aChunkData;
												aChunkData.nType = nChunkType;
												if ( nChunkLen )
												{
													aChunkData.aData.resize( nChunkLen );
													rtl_copyMemory( &aChunkData.aData[ 0 ], aByteSeq.getConstArray(), nChunkLen );
												}
												std::vector< vcl::PNGWriter::ChunkData >::iterator aIter = rChunkData.end() - 1;
												rChunkData.insert( aIter, aChunkData );
											}
										}
									}
								}
							}
						}
					}
				}
				aPNGWriter.Write( rOStm );

				if( rOStm.GetError() )
					nStatus = GRFILTER_IOERROR;
			}
			else if( aFilterName.EqualsIgnoreCaseAscii( EXP_SVG ) )
			{
                bool bDone(false);
               
                // do we have a native SVG RenderGraphic, whose data can be written directly?
                const SvgDataPtr aSvgDataPtr(rGraphic.getSvgData());

                if(aSvgDataPtr.get() && aSvgDataPtr->getSvgDataArrayLength())
				{
                    rOStm.Write(aSvgDataPtr->getSvgDataArray().get(), aSvgDataPtr->getSvgDataArrayLength());
            
           			if( rOStm.GetError() )
                    {
                        nStatus = GRFILTER_IOERROR;
                    }
                    else
                    {
                        bDone = true;
                    }
                }

                if( !bDone )
                {
                    // do the normal GDIMetaFile export instead
                    try
                    {
                        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );

                        if( xMgr.is() )
                        {
                            ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > xSaxWriter( xMgr->createInstance(
                                ::rtl::OUString::createFromAscii( "com.sun.star.xml.sax.Writer" ) ), ::com::sun::star::uno::UNO_QUERY );
							
							com::sun::star::uno::Sequence< com::sun::star::uno::Any > aArguments( 1 );
							aArguments[ 0 ] <<= aConfigItem.GetFilterData();
                            ::com::sun::star::uno::Reference< ::com::sun::star::svg::XSVGWriter > xSVGWriter( xMgr->createInstanceWithArguments(
                                ::rtl::OUString::createFromAscii( "com.sun.star.svg.SVGWriter" ), aArguments ), ::com::sun::star::uno::UNO_QUERY );

                            if( xSaxWriter.is() && xSVGWriter.is() )
                            {
                                ::com::sun::star::uno::Reference< ::com::sun::star::io::XActiveDataSource > xActiveDataSource(
                                    xSaxWriter, ::com::sun::star::uno::UNO_QUERY );

                                if( xActiveDataSource.is() )
                                {
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >	xStmIf(
                                        static_cast< ::cppu::OWeakObject* >( new ImpFilterOutputStream( rOStm ) ) );

                                    SvMemoryStream aMemStm( 65535, 65535 );

                                    aMemStm.SetCompressMode( COMPRESSMODE_FULL );
                                    
                                    // #119735# just use GetGDIMetaFile, it will create a bufferd version of contained bitmap now automatically
                                    ( (GDIMetaFile&) aGraphic.GetGDIMetaFile() ).Write( aMemStm );

                                    xActiveDataSource->setOutputStream( ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream >(
                                        xStmIf, ::com::sun::star::uno::UNO_QUERY ) );
                                    ::com::sun::star::uno::Sequence< sal_Int8 > aMtfSeq( (sal_Int8*) aMemStm.GetData(), aMemStm.Tell() );
                                    xSVGWriter->write( xSaxWriter, aMtfSeq );
                                }
                            }
                        }
                    }
                    catch( ::com::sun::star::uno::Exception& )
                    {
                        nStatus = GRFILTER_IOERROR;
                    }
                }
			}
			else
				nStatus = GRFILTER_FILTERERROR;
		}
		else
		{
			xub_StrLen i, nTokenCount = aFilterPath.GetTokenCount( ';' );
			for ( i = 0; i < nTokenCount; i++ )
			{
				String aPhysicalName( ImpCreateFullFilterPath( aFilterPath.GetToken( i ), aFilterName ) );
				osl::Module aLibrary( aPhysicalName );

				PFilterCall pFunc = (PFilterCall) aLibrary.getFunctionSymbol( UniString::CreateFromAscii( EXPORT_FUNCTION_NAME ) );
				// Dialog in DLL ausfuehren
				if( pFunc )
				{
					if ( !(*pFunc)( rOStm, aGraphic, &aConfigItem, sal_False ) )
						nStatus = GRFILTER_FORMATERROR;
					break;
				}
				else
					nStatus = GRFILTER_FILTERERROR;
			}
		}
	}
	if( nStatus != GRFILTER_OK )
	{
		if( bAbort )
			nStatus = GRFILTER_ABORT;

		ImplSetError( nStatus, &rOStm );
	}
	return nStatus;
}

// ------------------------------------------------------------------------

sal_Bool GraphicFilter::Setup( sal_uInt16 )
{
	return sal_False;
}

/* ------------------------------------------------------------------------
	No Import filter has a dialog, so
   the following two methods are obsolete */

sal_Bool GraphicFilter::HasImportDialog( sal_uInt16 )
{
	return sal_True;
//	return pConfig->IsImportDialog( nFormat );
}

// ------------------------------------------------------------------------

sal_Bool GraphicFilter::DoImportDialog( Window*, sal_uInt16 )
{
	return sal_True;
}

// ------------------------------------------------------------------------

sal_Bool GraphicFilter::HasExportDialog( sal_uInt16 nFormat )
{
	return pConfig->IsExportDialog( nFormat );
}

// ------------------------------------------------------------------------

sal_Bool GraphicFilter::DoExportDialog( Window* pWindow, sal_uInt16 nFormat )
{
	return DoExportDialog( pWindow, nFormat, FUNIT_MM );
}

sal_Bool GraphicFilter::DoExportDialog( Window*, sal_uInt16 nFormat, FieldUnit )
{
    sal_Bool bRet = sal_False;
 	com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >
        xSMgr( ::comphelper::getProcessServiceFactory() );

    uno::Reference< com::sun::star::uno::XInterface > xFilterOptionsDialog
        ( xSMgr->createInstance( rtl::OUString::createFromAscii( "com.sun.star.svtools.SvFilterOptionsDialog" ) ),
            com::sun::star::uno::UNO_QUERY );
    if ( xFilterOptionsDialog.is() )
    {
    	com::sun::star::uno::Reference< com::sun::star::ui::dialogs::XExecutableDialog > xExecutableDialog
            ( xFilterOptionsDialog, ::com::sun::star::uno::UNO_QUERY );
    	com::sun::star::uno::Reference< com::sun::star::beans::XPropertyAccess > xPropertyAccess
            ( xFilterOptionsDialog, ::com::sun::star::uno::UNO_QUERY );
        if ( xExecutableDialog.is() && xPropertyAccess.is() )
        {
            com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > aMediaDescriptor( 1 );
            aMediaDescriptor[ 0 ].Name = String( RTL_CONSTASCII_USTRINGPARAM( "FilterName" ) );
            rtl::OUString aStr( pConfig->GetExportInternalFilterName( nFormat ) );
            aMediaDescriptor[ 0 ].Value <<= aStr;
            xPropertyAccess->setPropertyValues( aMediaDescriptor );
            bRet = xExecutableDialog->execute() == com::sun::star::ui::dialogs::ExecutableDialogResults::OK;
        }
    }
    return bRet;
}

// ------------------------------------------------------------------------

const FilterErrorEx& GraphicFilter::GetLastError() const
{
	return *pErrorEx;
}

// ------------------------------------------------------------------------

void GraphicFilter::ResetLastError()
{
	pErrorEx->nFilterError = pErrorEx->nStreamError = 0UL;
}

// ------------------------------------------------------------------------

const Link GraphicFilter::GetFilterCallback() const
{
    const Link aLink( LINK( this, GraphicFilter, FilterCallback ) );
    return aLink;
}

// ------------------------------------------------------------------------

IMPL_LINK( GraphicFilter, FilterCallback, ConvertData*, pData )
{
	long nRet = 0L;

	if( pData )
	{
		sal_uInt16		nFormat = GRFILTER_FORMAT_DONTKNOW;
		ByteString	aShortName;
		switch( pData->mnFormat )
		{
			case( CVT_BMP ): aShortName = BMP_SHORTNAME; break;
			case( CVT_GIF ): aShortName = GIF_SHORTNAME; break;
			case( CVT_JPG ): aShortName = JPG_SHORTNAME; break;
			case( CVT_MET ): aShortName = MET_SHORTNAME; break;
			case( CVT_PCT ): aShortName = PCT_SHORTNAME; break;
			case( CVT_PNG ): aShortName = PNG_SHORTNAME; break;
			case( CVT_SVM ): aShortName = SVM_SHORTNAME; break;
			case( CVT_TIF ): aShortName = TIF_SHORTNAME; break;
			case( CVT_WMF ): aShortName = WMF_SHORTNAME; break;
			case( CVT_EMF ): aShortName = EMF_SHORTNAME; break;
			case( CVT_SVG ): aShortName = SVG_SHORTNAME; break;

			default:
			break;
		}
		if( GRAPHIC_NONE == pData->maGraphic.GetType() || pData->maGraphic.GetContext() ) // Import
		{
			// Import
			nFormat = GetImportFormatNumberForShortName( String( aShortName.GetBuffer(), RTL_TEXTENCODING_UTF8 ) );
			nRet = ImportGraphic( pData->maGraphic, String(), pData->mrStm ) == 0;
		}
		else if( aShortName.Len() )
		{
			// Export
			nFormat = GetExportFormatNumberForShortName( String( aShortName.GetBuffer(), RTL_TEXTENCODING_UTF8 ) );
			nRet = ExportGraphic( pData->maGraphic, String(), pData->mrStm, nFormat ) == 0;
		}
	}
	return nRet;
}

// ------------------------------------------------------------------------

GraphicFilter* GraphicFilter::GetGraphicFilter()
{
	if( !pGraphicFilter )
	{
		pGraphicFilter = new GraphicFilter;
		pGraphicFilter->GetImportFormatCount();
	}
	return pGraphicFilter;
}

int GraphicFilter::LoadGraphic( const String &rPath, const String &rFilterName,
				 Graphic& rGraphic, GraphicFilter* pFilter,
				 sal_uInt16* pDeterminedFormat )
{
	if ( !pFilter )
		pFilter = GetGraphicFilter();

	const sal_uInt16 nFilter = rFilterName.Len() && pFilter->GetImportFormatCount()
					? pFilter->GetImportFormatNumber( rFilterName )
					: GRFILTER_FORMAT_DONTKNOW;

	SvStream* pStream = NULL;
	INetURLObject aURL( rPath );

	if ( aURL.HasError() || INET_PROT_NOT_VALID == aURL.GetProtocol() )
	{
		aURL.SetSmartProtocol( INET_PROT_FILE );
		aURL.SetSmartURL( rPath );
	}
	else if ( INET_PROT_FILE != aURL.GetProtocol() )
	{
		pStream = ::utl::UcbStreamHelper::CreateStream( rPath, STREAM_READ );
	}

	int nRes = GRFILTER_OK;
	if ( !pStream )
		nRes = pFilter->ImportGraphic( rGraphic, aURL, nFilter, pDeterminedFormat );
	else
		nRes = pFilter->ImportGraphic( rGraphic, rPath, *pStream, nFilter, pDeterminedFormat );

#ifdef DBG_UTIL
	if( nRes )
		DBG_WARNING2( "GrafikFehler [%d] - [%s]", nRes, rPath.GetBuffer() );
#endif

	return nRes;
}
