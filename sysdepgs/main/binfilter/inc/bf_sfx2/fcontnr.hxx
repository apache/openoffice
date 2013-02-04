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


#ifndef _SFX_FCONTNR_HXX
#define _SFX_FCONTNR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif

#include <bf_sfx2/docfilt.hxx>
#include <bf_sfx2/sfxdefs.hxx>
class Window;
namespace binfilter {

class SfxFilter;
class SfxFilterArr_Impl;
class SfxObjectFacArr_Impl;
class SfxObjectFactory;
class SfxMedium;
class SfxFilterContainer_Impl;
class SfxFrame;

#define SFX_FCONTNR_REDIRECTS "Redirects"
#define SFX_FCONTNR_EXTAPP    "External"

#define SFX_FILTER_DOWNLOAD  "StarDownload (*.dnl)"

#define SFX_FILTER_CONTAINER_FACTORY 1
typedef USHORT SfxFilterContainerFlags;

class SfxRefItem : public SfxPoolItem
{
	SvRefBaseRef aRef;
public:
	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const
	{ (void)pPool;	return new SfxRefItem( *this ); }
	virtual int 			 operator==( const SfxPoolItem& rL) const
	{   return ((SfxRefItem&)rL).aRef == aRef; }
	SfxRefItem( USHORT nWhich, const SvRefBaseRef& rValue ) : SfxPoolItem( nWhich )
	{   aRef = rValue; }
	const SvRefBaseRef&      GetValue() const { return aRef; }

};

class SfxFrameWindow
{
	Window*		pWindow;
public:
				SfxFrameWindow( Window *pWin )
				 : pWindow( pWin )
				{}

	virtual		~SfxFrameWindow()
				{ delete pWindow; }
	Window*		GetWindow() const
				{ return pWindow; }
	void		SetWindow( Window *pWin )
				{ pWindow = pWin; }
};

class SfxFrameWindowFactory
{
	typedef SfxFrameWindow* (*FactoryFunc)( SfxFrame* pParent, const String& rName );
	FactoryFunc pFunc;
	String aName;
public:
	SfxFrameWindowFactory( FactoryFunc pFuncP, String aNameP );
	String GetURLWildCard() { return aName; }
	FactoryFunc GetFactory() { return pFunc; }
};

typedef ULONG (*SfxDetectFilter)(
	SfxMedium& rMedium, const SfxFilter **, SfxFilterFlags nMust, SfxFilterFlags nDont );

class SfxFilterContainer
{
protected:
	SfxFilterContainer_Impl *pImpl;
#if _SOLAR__PRIVATE
public:
	DECL_STATIC_LINK( SfxFilterContainer, LoadHdl_Impl, void* );
protected:
	void ForceFilterLoad_Impl() const;
	void RealLoad_Impl();
#endif

public:
	SfxFilterContainer( const String& rName );
	virtual ~SfxFilterContainer();

	SfxFilterContainerFlags GetFlags() const;
	void SetFlags( SfxFilterContainerFlags eFlags );

	virtual const SfxFilter* GetFilter4Mime(
		const String& rMime, SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	virtual const SfxFilter* GetFilter4ClipBoardId(
		ULONG nId, SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	virtual const SfxFilter* GetFilter4EA(
		const String& rEA, SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	virtual const SfxFilter* GetFilter4Extension(
		const String& rExt, SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	virtual const SfxFilter* GetFilter4FilterName(
		const String& rName, SfxFilterFlags nMust = 0, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	SfxFilter* GetFilter4FilterName(
		const String& rName, SfxFilterFlags nMust = 0, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED )
	{
		return (SfxFilter*)(const SfxFilter*)
			((const SfxFilterContainer* const)this)->GetFilter4FilterName(
				rName, nMust, nDont );
	}
    const SfxFilter* GetFilter4Protocol(
        const String& rURL, SfxFilterFlags nMust = 0, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	virtual ULONG            GetFilter4Content(
		SfxMedium& rMedium, const SfxFilter**, SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const ;
	virtual const SfxFilter* GetFilter4Protocol(
		SfxMedium& rMed, SfxFilterFlags nMust = 0, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	virtual const SfxFilter* GetFilter(
		const String& rName, SfxFilterFlags nMust = 0, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;

	virtual USHORT           GetFilterCount() const;
	virtual const SfxFilter* GetFilter( USHORT ) const;
	virtual ULONG            Execute( SfxMedium& rMedium, SfxFrame*& rpFrame ) const ;


	const String             GetName() const;

	void                     AddFilter( SfxFilter* pFilter, USHORT nPos );
	void                     LoadFilters(
		const String& rGroup, BOOL bInstallIni = TRUE, SfxFilterFlags nAddMask = 0, SfxFilterFlags nDelMask = 0);
/*AS    static String            ConvertToOldFilterName( const String& rNew );
    static String            ConvertToNewFilterName( const String& rNew ); */
    void                     ReadExternalFilters( const String& rDocServiceName );
};


#if _SOLAR__PRIVATE

enum SfxExecutableFilterType_Impl
{
	SFX_EXE_FILTER_BOOKMARK,
	SFX_EXE_FILTER_OS2BOOKMARK,
	SFX_EXE_FILTER_MAILTO,
	SFX_EXE_FILTER_MACRO,
	SFX_EXE_FILTER_SLOT,
	SFX_EXE_FILTER_JAVASCRIPT,
	SFX_EXE_FILTER_FACTORY,
	SFX_EXE_FILTER_EXPLORER,
	SFX_EXE_FILTER_BUGID,
	SFX_EXE_FILTER_ZIP,
	SFX_EXE_FILTER_BUGDOC,
	SFX_EXE_FILTER_CHOOSER,
	SFX_EXE_FILTER_DOWNLOAD,
	SFX_EXE_FILTER_SUBSCRIBENEWAGENT,
	SFX_EXE_FILTER_HELPID,
	SFX_EXE_FILTER_EXECUTE,
	SFX_EXE_FILTER_CDFCHANNEL,
	SFX_EXE_FILTER_COMPONENT,
	SFX_EXE_FILTER_CHAOSCHANNEL,
	SFX_EXE_FILTER_UNO,
	SFX_EXE_FILTER_COMMAND,
	SFX_EXE_FILTER_HELPENTRY,
	SFX_EXE_FILTER_SOUND,
	SFX_EXE_FILTER_EXTERNBROWSER
};

#endif

class SfxExecutableFilterTypes_Impl;
class SfxExecutableFilterContainer : public SfxFilterContainer
{
	SfxExecutableFilterTypes_Impl* pTypes;

#if _SOLAR__PRIVATE

	ULONG                    Download_Impl( SfxMedium& rMedium ) const;
	ULONG                    Choose_Impl( SfxMedium& rMedium ) const;
	ULONG                    Unzip_Impl( SfxMedium& rMedium ) const;
	String                   GetBugdocName_Impl( const String& rName ) const;
	void                     AddExeFilter( SfxFilter* pFilter, SfxExecutableFilterType_Impl eType, const String& rPattern );
#endif

public:
							 SfxExecutableFilterContainer();
							 ~SfxExecutableFilterContainer();

	virtual const SfxFilter* GetFilter4Protocol( SfxMedium& rMed, SfxFilterFlags nMust = 0, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	virtual ULONG            Execute( SfxMedium& rMedium, SfxFrame*& ) const ;

	static const SfxFilter*  CheckForFolder( SfxMedium& rMed, SfxFilterFlags nMust = 0, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED );
	static const SfxFilter*	 GetComponentFilter();
	static const SfxFilter*	 GetExplorerFilter();
};

class SfxExternalAppFilterContainer : public SfxFilterContainer
{
public:
	SfxExternalAppFilterContainer();
	virtual ULONG            Execute( SfxMedium& rMedium, SfxFrame*& ) const;
	virtual const SfxFilter* GetFilter4FilterName(
		const String& rName, SfxFilterFlags nMust = 0, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	virtual USHORT           GetFilterCount() const;
	virtual const SfxFilter* GetFilter4Protocol(
		SfxMedium& rName, SfxFilterFlags nMust = 0, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
};

class SfxPluginFilterContainer : public SfxFilterContainer
{
	BOOL						bInitialized;
	DECL_LINK( 					LoadHdl_Impl, void* );
public:
								SfxPluginFilterContainer();
	virtual ULONG            	Execute( SfxMedium& rMedium, SfxFrame*& ) const;
	virtual USHORT           	GetFilterCount() const;
	virtual const SfxFilter* 	GetFilter4Protocol( SfxMedium& rMed, SfxFilterFlags nMust = 0, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
};

class SfxFactoryFilterContainer : public SfxFilterContainer
{

	const SfxObjectFactory& rFact;
	SfxDetectFilter pFunc;

public:

	SfxFactoryFilterContainer(
		const String& rName, const SfxObjectFactory& rFactP );
	void SetDetectFilter( SfxDetectFilter pFuncP ) { pFunc = pFuncP; }
	virtual ULONG GetFilter4Content(
		SfxMedium& rMedium, const SfxFilter **,
		SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const ;
	const SfxObjectFactory& GetFactory() const { return rFact; }
};

class SfxFilterMatcher_Impl;

class SfxFilterMatcher
{
	SfxFilterMatcher_Impl *pImpl;
public:
	SfxFilterMatcher( SfxFilterContainer* pCont );
	SfxFilterMatcher(BOOL bDeleteContainers);
	~SfxFilterMatcher();

        SfxFilterContainer*       GetContainer( const String& rName ) const;
	void                      AddContainer( SfxFilterContainer* );
	ULONG                     GuessFilterIgnoringContent(
		SfxMedium& rMedium, const SfxFilter **,
		SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	ULONG                     GuessFilter(
		SfxMedium& rMedium, const SfxFilter **,
		SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	const SfxFilter*          GetFilter4ClipBoardId(
		ULONG nId, SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	const SfxFilter*          GetFilter4Extension(
		const String& rExt, SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;

/*STRIP003*/ 	ULONG                     GetFilter4Content(
/*STRIP003*/ 		SfxMedium& rMedium, const SfxFilter**, SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED, BOOL bOnlyGoodOnes=FALSE ) const ;
	const SfxFilter*          GetFilter4Protocol(
		SfxMedium& rMed, SfxFilterFlags nMust = SFX_FILTER_IMPORT, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;
	const SfxFilter*          GetFilter4FilterName(
		const String& rName, SfxFilterFlags nMust = 0, SfxFilterFlags nDont = SFX_FILTER_NOTINSTALLED ) const;


    const SfxFilter*          ResolveRedirection( const SfxFilter*, SfxMedium& rMedium ) const;
    const SfxFilter*          ResolveRedirection( const SfxFilter*, const String& rURLPath ) const;
	const SfxFilter*          ResolveAppPlug( const SfxFilter* ) const;


	friend class SfxFilterMatcherIter;
	DECL_STATIC_LINK( SfxFilterMatcher, MaybeFileHdl_Impl, String* );
};

class SfxFilterMatcherIter
{
	SfxFilterFlags nOrMask;
	SfxFilterFlags nAndMask;
	USHORT nBorder;
	int nAktFilter;
	int nAktContainer;
	SfxFilterContainer* pCont;
	const SfxFilterMatcher_Impl *pMatch;
};

}//end of namespace binfilter
#endif
