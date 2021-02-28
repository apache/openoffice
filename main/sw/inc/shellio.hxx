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


#ifndef SW_SHELLIO_HXX
#define SW_SHELLIO_HXX

#include <memory>
#include <boost/utility.hpp>

#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/embed/XStorage.hpp>
#include <sfx2/docfile.hxx>
#include <sfx2/fcontnr.hxx>
#include <sot/formats.hxx>
#include <sot/storage.hxx>
#include <svtools/parhtml.hxx>
#include <tools/string.hxx>
#include <tools/date.hxx>
#include <tools/time.hxx>
#include <tools/datetime.hxx>
#include <tools/ref.hxx>
#include <tools/urlobj.hxx>
#include <swdllapi.h>
#include <swtypes.hxx>
#include <docfac.hxx>	// SwDocFac
#include <errhdl.hxx>
#include <iodetect.hxx>

// some forward declarations
class SfxFilterContainer;
class SfxFilter;
class SfxItemPool;
class SfxItemSet;
class SfxMedium;
class SvPtrarr;
class SvStream;
class SvStrings;
class SvxFontItem;
class SvxMacroTableDtor;
//class Sw3Io;
class SwCntntNode;
class SwCrsrShell;
class SwDoc;
class SwPaM;
class SwTextBlocks;
struct SwPosition;
struct Writer_Impl;

//defines the amount of maximal characters in a paragraph.
//The force wrap is done by ASCII/W4W-reader. Must be greater than 200
#define MAX_ASCII_PARA 10000


class SW_DLLPUBLIC SwAsciiOptions
{
	String sFont;
	rtl_TextEncoding eCharSet;
	sal_uInt16 nLanguage;
	LineEnd eCRLF_Flag;

public:

	const String& GetFontName() const { return sFont; }
	void SetFontName( const String& rFont ) { sFont = rFont; }

	rtl_TextEncoding GetCharSet() const { return eCharSet; }
	void SetCharSet( rtl_TextEncoding nVal ) { eCharSet = nVal; }

	sal_uInt16 GetLanguage() const { return nLanguage; }
	void SetLanguage( sal_uInt16 nVal ) { nLanguage = nVal; }

	LineEnd GetParaFlags() const { return eCRLF_Flag; }
	void SetParaFlags( LineEnd eVal ) { eCRLF_Flag = eVal; }

	void Reset()
	{
		sFont.Erase();
		eCRLF_Flag = GetSystemLineEnd();
		eCharSet = ::gsl_getSystemTextEncoding();
		nLanguage = 0;
	}
	// for the automatic conversion (mail/news/...)
	void ReadUserData( const String& );
	void WriteUserData( String& );

	SwAsciiOptions() { Reset(); }
};

/**************** SwReader/Reader ************************/
// base class for different specialized readers
class Reader;
//  calls the reader with its options, document, cursor etc.
class SwReader;
// SwRead is a pointer on the read options of the base class
typedef Reader *SwRead;

class SwgReaderOption
{
	SwAsciiOptions aASCIIOpts;
	union
	{
		sal_Bool bFmtsOnly;
		struct
		{
			sal_Bool bFrmFmts: 1;
			sal_Bool bPageDescs: 1;
			sal_Bool bTxtFmts: 1;
			sal_Bool bNumRules: 1;
			sal_Bool bMerge:1;
		}  Fmts;
	} What;

public:
	void ResetAllFmtsOnly() { What.bFmtsOnly = 0; }
    sal_Bool IsFmtsOnly() const { return What.bFmtsOnly; }

	sal_Bool IsFrmFmts() const { return What.Fmts.bFrmFmts; }
	void SetFrmFmts( const sal_Bool bNew) { What.Fmts.bFrmFmts = bNew; }

	sal_Bool IsPageDescs() const { return What.Fmts.bPageDescs; }
	void SetPageDescs( const sal_Bool bNew) { What.Fmts.bPageDescs = bNew; }

	sal_Bool IsTxtFmts() const { return What.Fmts.bTxtFmts; }
	void SetTxtFmts( const sal_Bool bNew) { What.Fmts.bTxtFmts = bNew; }

	sal_Bool IsNumRules() const { return What.Fmts.bNumRules; }
	void SetNumRules( const sal_Bool bNew) { What.Fmts.bNumRules = bNew; }

	sal_Bool IsMerge() const { return What.Fmts.bMerge; }
	void SetMerge( const sal_Bool bNew ) { What.Fmts.bMerge = bNew; }

	const SwAsciiOptions& GetASCIIOpts() const { return aASCIIOpts; }
	void SetASCIIOpts( const SwAsciiOptions& rOpts ) { aASCIIOpts = rOpts; }
	void ResetASCIIOpts() { aASCIIOpts.Reset(); }

	SwgReaderOption()
	{	ResetAllFmtsOnly(); aASCIIOpts.Reset(); }
};

class SwReader: public SwDocFac
{
	SvStream* pStrm;
    SotStorageRef pStg;
    com::sun::star::uno::Reference < com::sun::star::embed::XStorage > xStg;
	SfxMedium* pMedium;		// describes file or storage (W4W)

	SwPaM* pCrsr;
	String aFileName;
    String sBaseURL;

public:
	/*
	 * Initial read. Document is created when function Read(...) is called
	 * JP 25.04.95: or if it has been passed in that one.
	 *				special case if loaded with Sw3Reader
	 */
    //SwReader( SotStorage&, const String& rFilename, SwDoc *pDoc = 0 );
    //SwReader( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >&, const String& rFilename, SwDoc *pDoc = 0 );
    SwReader( SfxMedium&, const String& rFilename, SwDoc *pDoc = 0 );
	/*
	 * read into an existing document. Document and
	 * position within the document are taken from SwPaM.
	 */
    SwReader( SvStream&, const String& rFilename, const String& rBaseURL, SwPaM& );
    //SwReader( SotStorage&, const String& rFilename, SwPaM& );
	SwReader( SfxMedium&, const String& rFilename, SwPaM& );
    SwReader( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >&, const String& rFilename, SwPaM& );

	/*
 	* only SwReader::Read(...) is the Export-Interface!!!
 	*/
	sal_Bool NeedsPasswd( const Reader& );
	sal_Bool CheckPasswd( const String&, const Reader& );
	sal_uLong Read( const Reader& );

	// ask for glossaries
	sal_Bool HasGlossaries( const Reader& );
	sal_Bool ReadGlossaries( const Reader&, SwTextBlocks&, sal_Bool bSaveRelFiles );

    const String&       GetBaseURL() const { return sBaseURL;}

protected:
    void                SetBaseURL( const String& rURL ) { sBaseURL = rURL; }
};



/* */
/****************  SPECIAL Readers ************************/

// special - Reader can be both!! (Excel, W4W, .. )
#define SW_STREAM_READER	1
#define SW_STORAGE_READER	2

class SW_DLLPUBLIC Reader
{
	friend class SwReader;
	SwDoc* pTemplate;
	String aTemplateNm;
    //String sBaseURL;

    Date aDStamp;
	Time aTStamp;
	DateTime aChkDateTime;

protected:
	SvStream* pStrm;
    SotStorageRef pStg;
    com::sun::star::uno::Reference < com::sun::star::embed::XStorage > xStg;
	SfxMedium* pMedium;		// describes file or storage  (W4W)

	SwgReaderOption aOpt;
	sal_Bool bInsertMode : 1;
	sal_Bool bTmplBrowseMode : 1;
	sal_Bool bReadUTF8: 1;		// interpreted stream as UTF-8
	sal_Bool bBlockMode: 1;
	sal_Bool bOrganizerMode : 1;
	sal_Bool bHasAskTemplateName : 1;
    sal_Bool bIgnoreHTMLComments : 1;

	virtual String GetTemplateName() const;

public:
	Reader();
	virtual ~Reader();

	virtual int GetReaderType();
	SwgReaderOption& GetReaderOpt() { return aOpt; }

	virtual void SetFltName( const String& rFltNm );
	static void SetNoOutlineNum( SwDoc& rDoc );

	// fit the item-set of a Frm-Formats to the old Format
	static void ResetFrmFmtAttrs( SfxItemSet &rFrmSet );

	// convert frame /graphic-/OLE-Templates to old Format (without border etc.)
	static void ResetFrmFmts( SwDoc& rDoc );

	// load filter-template, position it and then set it free again
	SwDoc* GetTemplateDoc();
	sal_Bool SetTemplate( SwDoc& rDoc );
	void ClearTemplate();
	void SetTemplateName( const String& rDir );
	void MakeHTMLDummyTemplateDoc();

    sal_Bool IsReadUTF8() const { return bReadUTF8; }
	void SetReadUTF8( sal_Bool bSet ) { bReadUTF8 = bSet; }

    sal_Bool IsBlockMode() const { return bBlockMode; }
	void SetBlockMode( sal_Bool bSet ) { bBlockMode = bSet; }

    sal_Bool IsOrganizerMode() const { return bOrganizerMode; }
	void SetOrganizerMode( sal_Bool bSet ) { bOrganizerMode = bSet; }

    void SetIgnoreHTMLComments( sal_Bool bSet ) { bIgnoreHTMLComments = bSet; }

	virtual sal_Bool HasGlossaries() const;
	virtual sal_Bool ReadGlossaries( SwTextBlocks&, sal_Bool bSaveRelFiles ) const;

	// read the sections of the document, which is equal to the medium.
	// returns the count of it
	virtual sal_uInt16 GetSectionList( SfxMedium& rMedium,
									SvStrings& rStrings ) const;

	SotStorageRef getSotStorageRef() { return pStg; };
    void setSotStorageRef(SotStorageRef pStgRef) { pStg = pStgRef; };

private:
    virtual sal_uLong Read(SwDoc &, const String& rBaseURL, SwPaM &,const String &)=0;

	// all streams / storages that do need to open it must overload the method
	// (W4W!!)
	virtual int SetStrmStgPtr();
};

class AsciiReader: public Reader
{
	friend class SwReader;
    virtual sal_uLong Read( SwDoc &, const String& rBaseURL, SwPaM &,const String &);
public:
	AsciiReader(): Reader() {}
};

/*class SwgReader: public Reader
{
    virtual sal_uLong Read( SwDoc &, const String& rBaseURL, SwPaM &,const String &);
};
*/
class SW_DLLPUBLIC StgReader : public Reader
{
	String aFltName;

protected:
    sal_uLong OpenMainStream( SotStorageStreamRef& rRef, sal_uInt16& rBuffSize );

public:
	virtual int GetReaderType();
    const String& GetFltName() { return aFltName; }
	virtual void SetFltName( const String& r );
};


/*class Sw3Reader : public StgReader
{
	Sw3Io* pIO;
    virtual sal_uLong Read( SwDoc &, const String& rBaseURL, SwPaM &,const String &);
public:
	Sw3Reader() : pIO( 0 ) {}

    void   SetSw3Io( Sw3Io* pIo )     { pIO = pIo; }

	// read the sections of the document, which is equal to the medium.
	// returns the count of it
	virtual sal_uInt16 GetSectionList( SfxMedium& rMedium,
								SvStrings& rStrings ) const;
};*/

/* */
////////////////////////////////////////////////////////////////////////////
// The passed stream has to be dynamic created and before deletion of the
// instance per Stream() requested and deleted!

class SwImpBlocks;

class SW_DLLPUBLIC SwTextBlocks
{
//  friend class Sw2TextBlocks;
//  friend class Sw3IoImp;
	SwImpBlocks* pImp;
	sal_uLong 		 nErr;

public:
	SwTextBlocks( const String& );
	~SwTextBlocks();

    void Flush(){}

	SwDoc* GetDoc();
	void   ClearDoc();					// delete doc content
	const  String& GetName();
	void   SetName( const String& );
	sal_uLong GetError() const { return nErr; }

    String GetBaseURL() const;
    void   SetBaseURL( const String& rURL );

	sal_Bool   IsOld() const;
	sal_uLong  ConvertToNew();				// convert AutoText entries (ger: Textbausteine)

	sal_uInt16 GetCount() const;						// return amount of AutoText entries
	sal_uInt16 GetIndex( const String& ) const;			// return Index-Number of shortnames
	sal_uInt16 GetLongIndex( const String& ) const;		//return Index-Number of langnamen
	const  String& GetShortName( sal_uInt16 ) const; 	// return a shortname of an index
	const  String& GetLongName( sal_uInt16 ) const;  	// return longname of an index

	sal_Bool   Delete( sal_uInt16 );			// delete
	sal_uInt16 Rename( sal_uInt16, const String*, const String* ); // rename
	sal_uLong  CopyBlock( SwTextBlocks& rSource, String& rSrcShort,
									const String& rLong ); // copy block

	sal_Bool   BeginGetDoc( sal_uInt16 );			// read AutoText entry
	void   EndGetDoc();						// release AutoText entry

	sal_Bool   BeginPutDoc( const String&, const String& ); // start save
	sal_uInt16 PutDoc(); 								// End save

	sal_uInt16 PutText( const String&, const String&, const String& ); // save (shortname, Text)

	sal_Bool IsOnlyTextBlock( sal_uInt16 ) const;
	sal_Bool IsOnlyTextBlock( const String& rShort ) const;

	const String& GetFileName() const;		// filename of pImp
	sal_Bool IsReadOnly() const;  				// ReadOnly-Flag of pImp

	sal_Bool GetMacroTable( sal_uInt16 nIdx, SvxMacroTableDtor& rMacroTbl );
	sal_Bool SetMacroTable( sal_uInt16 nIdx, const SvxMacroTableDtor& rMacroTbl );

    sal_Bool StartPutMuchBlockEntries();
	void EndPutMuchBlockEntries();
};

// BEGIN source/filter/basflt/fltini.cxx

extern void _InitFilter();
extern void _FinitFilter();

extern SwRead ReadAscii, /*ReadSwg, ReadSw3, */ReadHTML, ReadXML;

//SW_DLLPUBLIC SwRead SwGetReaderSw3();
SW_DLLPUBLIC SwRead SwGetReaderXML();

// END source/filter/basflt/fltini.cxx


extern sal_Bool SetHTMLTemplate( SwDoc &rDoc ); // load templates from HTML.vor (see shellio.cxx)


/* */
/////////////////////////////////////////////////////////////////////////////

/*
 * write, Writer
 */


/* base class of all Writer */

class IDocumentSettingAccess;
class IDocumentStylePoolAccess;

class SW_DLLPUBLIC Writer
    : public SvRefBase
    , private ::boost::noncopyable
{
	SwAsciiOptions aAscOpts;
    String          sBaseURL;

	void _AddFontItem( SfxItemPool& rPool, const SvxFontItem& rFont );
	void _AddFontItems( SfxItemPool& rPool, sal_uInt16 nWhichId );

    ::std::auto_ptr<Writer_Impl> m_pImpl;

protected:

	SwPaM* pOrigPam;			// last Pam to work on
	const String* pOrigFileName;

	void ResetWriter();
	sal_Bool CopyNextPam( SwPaM ** );

	void PutNumFmtFontsInAttrPool();
	void PutEditEngFontsInAttrPool( sal_Bool bIncl_CJK_CTL = sal_True );
	void PutCJKandCTLFontsInAttrPool();

	virtual sal_uLong WriteStream() = 0;
    void                SetBaseURL( const String& rURL ) { sBaseURL = rURL; }

    IDocumentSettingAccess* getIDocumentSettingAccess();
    const IDocumentSettingAccess* getIDocumentSettingAccess() const;

	IDocumentStylePoolAccess* getIDocumentStylePoolAccess();
	const IDocumentStylePoolAccess* getIDocumentStylePoolAccess() const;

public:
	SwDoc* pDoc;
	SwPaM* pCurPam;
	sal_Bool bWriteAll : 1;
	sal_Bool bShowProgress : 1;
	sal_Bool bWriteClipboardDoc : 1;
	sal_Bool bWriteOnlyFirstTable : 1;
	sal_Bool bASCII_ParaAsCR : 1;
	sal_Bool bASCII_ParaAsBlanc : 1;
	sal_Bool bASCII_NoLastLineEnd : 1;
	sal_Bool bUCS2_WithStartChar : 1;
	sal_Bool bExportPargraphNumbering : 1;

	sal_Bool bBlock : 1;
	sal_Bool bOrganizerMode : 1;

    Writer();
	virtual ~Writer();

	virtual sal_uLong Write( SwPaM&, SfxMedium&, const String* = 0 );
			sal_uLong Write( SwPaM&, SvStream&,  const String* = 0 );
    virtual sal_uLong Write( SwPaM&, const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >&, const String* = 0, SfxMedium* = 0 );
    virtual sal_uLong Write( SwPaM&, SotStorage&, const String* = 0 );

	virtual void SetPasswd( const String& );
	virtual void SetVersion( const String&, long );
	virtual sal_Bool IsStgWriter() const;
//  virtual sal_Bool IsSw3Writer() const;

    void SetShowProgress( sal_Bool bFlag = sal_False )  { bShowProgress = bFlag; }

	const String* GetOrigFileName() const 		{ return pOrigFileName; }

	const SwAsciiOptions& GetAsciiOptions() const { return aAscOpts; }
	void SetAsciiOptions( const SwAsciiOptions& rOpt ) { aAscOpts = rOpt; }

    const String&       GetBaseURL() const { return sBaseURL;}

    // search next bookmark position from the bookmark table
	sal_Int32 FindPos_Bkmk( const SwPosition& rPos ) const;
	// build a bookmark table, which is sorted by the node position. The
	// OtherPos of the bookmarks also inserted.
	void CreateBookmarkTbl();
	// search all bookmarks in the range and return it in the Array
	sal_uInt16 GetBookmarks( const SwCntntNode& rNd,
						xub_StrLen nStt, xub_StrLen nEnd,
						SvPtrarr& rArr );

	// create a new PaM at the position
    static SwPaM * NewSwPaM(SwDoc & rDoc,
                            sal_uLong const nStartIdx, sal_uLong const nEndIdx);

	// if applicable, copy a local file into internet / cloud
	sal_Bool CopyLocalFileToINet( String& rFileNm );

	// Stream-specific routines, DO NOT USE in Storage-Writer!

	// Optimizing output on stream
	SvStream& OutLong( SvStream& rStrm, long nVal );
	SvStream& OutULong( SvStream& rStrm, sal_uLong nVal );

	// print hexadecimal number, default is 2-digit number
	SvStream& OutHex( SvStream& rStrm, sal_uLong nHex, sal_uInt8 nLen = 2 );
	// print 4-digit hexadecimal number
	inline SvStream& OutHex4( SvStream& rStrm, sal_uInt16 nHex )
		{   return OutHex( rStrm, nHex, 4 ); }

	inline SvStream& OutHex( sal_uInt16 nHex, sal_uInt8 nLen = 2 )      { return OutHex( Strm(), nHex, nLen ); }
	inline SvStream& OutHex4( sal_uInt16 nHex )     { return OutHex( Strm(), nHex, 4 ); }
    inline SvStream& OutLong( long nVal )       { return OutLong( Strm(), nVal ); }
	inline SvStream& OutULong( sal_uLong nVal )		{ return OutULong( Strm(), nVal ); }

    void SetStream(SvStream *const pStream);
	SvStream& Strm();

    void SetOrganizerMode( sal_Bool bSet ) { bOrganizerMode = bSet; }
};

#ifndef SW_DECL_WRITER_DEFINED
#define SW_DECL_WRITER_DEFINED
SV_DECL_REF(Writer)
#endif
SV_IMPL_REF(Writer)

// baseclass of all Storage-Writer
class SW_DLLPUBLIC StgWriter : public Writer
{
protected:
	String aFltName;
    SotStorageRef pStg;
    com::sun::star::uno::Reference < com::sun::star::embed::XStorage > xStg;

	// create error at call
	virtual sal_uLong WriteStream();
	virtual sal_uLong WriteStorage() = 0;
	virtual sal_uLong WriteMedium( SfxMedium& ) = 0;

    using Writer::Write;

public:
    StgWriter() : Writer() {}

	virtual sal_Bool IsStgWriter() const;

    virtual sal_uLong Write( SwPaM&, const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >&, const String* = 0, SfxMedium* = 0 );
    virtual sal_uLong Write( SwPaM&, SotStorage&, const String* = 0 );

    SotStorage& GetStorage() const       { return *pStg; }
};

/*class Sw3Writer : public StgWriter
{
	Sw3Io* pIO;
	sal_Bool bSaveAs : 1;

	virtual sal_uLong WriteStorage();
	virtual sal_uLong WriteMedium( SfxMedium& );

public:
	Sw3Writer() : pIO( 0 ), bSaveAs( sal_False ) {}

    virtual sal_Bool IsSw3Writer() const;
};

*/

// Interface class for a generic access on special writer

class SwWriter
{
	SvStream* pStrm;
    SotStorageRef pStg;
    com::sun::star::uno::Reference < com::sun::star::embed::XStorage > xStg;
	SfxMedium* pMedium;

	SwPaM* pOutPam;
	SwCrsrShell *pShell;
	SwDoc &rDoc;

    //String sBaseURL;

    sal_Bool bWriteAll;

public:
	sal_uLong Write( WriterRef& rxWriter, const String* = 0);

	SwWriter( SvStream&, SwCrsrShell &,sal_Bool bWriteAll = sal_False );
	SwWriter( SvStream&, SwDoc & );
	SwWriter( SvStream&, SwPaM &, sal_Bool bWriteAll = sal_False );

//  SwWriter( SotStorage&, SwCrsrShell &,sal_Bool bWriteAll = sal_False );
    SwWriter( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >&, SwDoc& );
//  SwWriter( SotStorage&, SwPaM&, sal_Bool bWriteAll = sal_False );

	SwWriter( SfxMedium&, SwCrsrShell &,sal_Bool bWriteAll = sal_False );
	SwWriter( SfxMedium&, SwDoc & );
//	SwWriter( SfxMedium&, SwPaM&, sal_Bool bWriteAll = sal_False );

    //const String&       GetBaseURL() const { return sBaseURL;}
};


/* */
/////////////////////////////////////////////////////////////////////////////

typedef Reader* (*FnGetReader)();
typedef void (*FnGetWriter)(const String&, const String& rBaseURL, WriterRef&);
sal_uLong SaveOrDelMSVBAStorage( SfxObjectShell&, SotStorage&, sal_Bool, const String& );
sal_uLong GetSaveWarningOfMSVBAStorage( SfxObjectShell &rDocS );

struct SwReaderWriterEntry
{
    Reader* pReader;
    FnGetReader fnGetReader;
    FnGetWriter fnGetWriter;
    sal_Bool bDelReader;

    SwReaderWriterEntry( const FnGetReader fnReader, const FnGetWriter fnWriter, sal_Bool bDel )
        : pReader( NULL ), fnGetReader( fnReader ), fnGetWriter( fnWriter ), bDelReader( bDel )
    {}

    /// Get access to the reader
    Reader* GetReader();

    /// Get access to the writer
    void GetWriter( const String& rNm, const String& rBaseURL, WriterRef& xWrt ) const;
};

namespace SwReaderWriter
{
    /// Return reader based on ReaderWriterEnum
    Reader* GetReader( ReaderWriterEnum eReader );

    /// Return reader based on the name
    Reader* GetReader( const String& rFltName );

    /// Return writer based on the name
    void GetWriter( const String& rFltName, const String& rBaseURL, WriterRef& xWrt );
}

void GetRTFWriter( const String&, const String&, WriterRef& );
void GetASCWriter( const String&, const String&, WriterRef& );
//void GetSw3Writer( const String&, const String&, WriterRef& );
void GetHTMLWriter( const String&, const String&, WriterRef& );
void GetXMLWriter( const String&, const String&, WriterRef& );
void GetWW8Writer( const String&, const String&, WriterRef& );

#endif
