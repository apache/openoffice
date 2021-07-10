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

//_________________________________________________________________________________________________________________
//	includes
//_________________________________________________________________________________________________________________
#include <framework/addonsoptions.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/configitem.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <tools/debug.hxx>
#include <tools/stream.hxx>
#include <tools/color.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include "com/sun/star/util/XMacroExpander.hpp"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/beans/XPropertySet.hpp"
#include <rtl/ustrbuf.hxx>
#include <rtl/uri.hxx>
#include <comphelper/processfactory.hxx>
#include <vcl/graph.hxx>
#include <svtools/filter.hxx>
#include <vcl/dibtools.hxx>
#include <hash_map>
#include <algorithm>
#include <vector>

//_________________________________________________________________________________________________________________
//	namespaces
//_________________________________________________________________________________________________________________

using namespace ::std					;
using namespace ::utl					;
using namespace ::osl					;
using namespace ::com::sun::star::uno	;
using namespace ::com::sun::star::beans	;
using namespace ::com::sun::star::lang	;

//_________________________________________________________________________________________________________________
//	const
//_________________________________________________________________________________________________________________

#define STR_TOOLBARITEMS    "ToolBarItems"

#define ROOTNODE_ADDONMENU                              ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Office.Addons"	))
#define PATHDELIMITER                                   ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"				))
#define TOOLBARITEMS									::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(STR_TOOLBARITEMS))
#define SEPARATOR_URL_STR								"private:separator"
#define SEPARATOR_URL_LEN								17
#define SEPARATOR_URL									::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( SEPARATOR_URL_STR ))

#define PROPERTYNAME_URL                                ADDONSMENUITEM_PROPERTYNAME_URL
#define PROPERTYNAME_TITLE                              ADDONSMENUITEM_PROPERTYNAME_TITLE
#define PROPERTYNAME_TARGET                             ADDONSMENUITEM_PROPERTYNAME_TARGET
#define PROPERTYNAME_IMAGEIDENTIFIER                    ADDONSMENUITEM_PROPERTYNAME_IMAGEIDENTIFIER
#define PROPERTYNAME_CONTEXT							ADDONSMENUITEM_PROPERTYNAME_CONTEXT
#define PROPERTYNAME_SUBMENU							ADDONSMENUITEM_PROPERTYNAME_SUBMENU
#define PROPERTYNAME_CONTROLTYPE                        ADDONSMENUITEM_PROPERTYNAME_CONTROLTYPE
#define PROPERTYNAME_WIDTH                              ADDONSMENUITEM_PROPERTYNAME_WIDTH
#define PROPERTYNAME_TOOLBARITEMS                       TOOLBARITEMS

#define PROPERTYNAME_ALIGN                              STATUSBARITEM_PROPERTYNAME_ALIGN
#define PROPERTYNAME_AUTOSIZE                           STATUSBARITEM_PROPERTYNAME_AUTOSIZE
#define PROPERTYNAME_OWNERDRAW                          STATUSBARITEM_PROPERTYNAME_OWNERDRAW

#define PROPERTYNAME_IMAGESMALL							::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ImageSmall" ))
#define PROPERTYNAME_IMAGEBIG							::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ImageBig" ))
#define PROPERTYNAME_IMAGESMALLHC						::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ImageSmallHC" ))
#define PROPERTYNAME_IMAGEBIGHC							::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ImageBigHC" ))
#define PROPERTYNAME_IMAGESMALL_URL						::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ImageSmallURL" ))
#define PROPERTYNAME_IMAGEBIG_URL						::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ImageBigURL" ))
#define PROPERTYNAME_IMAGESMALLHC_URL					::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ImageSmallHCURL" ))
#define PROPERTYNAME_IMAGEBIGHC_URL						::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ImageBigHCURL" ))

#define IMAGES_NODENAME									::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("UserDefinedImages" ))
#define PRIVATE_IMAGE_URL                               ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("private:image/" ))

#define PROPERTYNAME_MERGEMENU_MERGEPOINT               ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergePoint" ))
#define PROPERTYNAME_MERGEMENU_MERGECOMMAND             ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeCommand" ))
#define PROPERTYNAME_MERGEMENU_MERGECOMMANDPARAMETER    ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeCommandParameter" ))
#define PROPERTYNAME_MERGEMENU_MERGEFALLBACK            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeFallback" ))
#define PROPERTYNAME_MERGEMENU_MERGECONTEXT             ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeContext" ))
#define PROPERTYNAME_MERGEMENU_MENUITEMS                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MenuItems" ))
#define MERGEMENU_MERGEPOINT_SEPARATOR                  '\\'

#define PROPERTYNAME_MERGETOOLBAR_TOOLBAR               ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeToolBar" ))
#define PROPERTYNAME_MERGETOOLBAR_MERGEPOINT            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergePoint" ))
#define PROPERTYNAME_MERGETOOLBAR_MERGECOMMAND          ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeCommand" ))
#define PROPERTYNAME_MERGETOOLBAR_MERGECOMMANDPARAMETER ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeCommandParameter" ))
#define PROPERTYNAME_MERGETOOLBAR_MERGEFALLBACK         ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeFallback" ))
#define PROPERTYNAME_MERGETOOLBAR_MERGECONTEXT          ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeContext" ))
#define PROPERTYNAME_MERGETOOLBAR_TOOLBARITEMS          ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(STR_TOOLBARITEMS ))

#define PROPERTYNAME_MERGESTATUSBAR_MERGEPOINT               ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergePoint" ))
#define PROPERTYNAME_MERGESTATUSBAR_MERGECOMMAND             ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeCommand" ))
#define PROPERTYNAME_MERGESTATUSBAR_MERGECOMMANDPARAMETER    ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeCommandParameter" ))
#define PROPERTYNAME_MERGESTATUSBAR_MERGEFALLBACK            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeFallback" ))
#define PROPERTYNAME_MERGESTATUSBAR_MERGECONTEXT             ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MergeContext" ))
#define PROPERTYNAME_MERGESTATUSBAR_STATUSBARITEMS           ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("StatusBarItems" ))

// The following order is mandatory. Please add properties at the end!
#define INDEX_URL               0
#define INDEX_TITLE             1
#define INDEX_IMAGEIDENTIFIER   2
#define INDEX_TARGET            3
#define INDEX_CONTEXT           4
#define INDEX_SUBMENU           5
#define INDEX_CONTROLTYPE       6
#define INDEX_WIDTH             7
#define INDEX_ALIGN             8
#define INDEX_AUTOSIZE          9
#define INDEX_OWNERDRAW         10
#define INDEX_TOOLBARITEMS      11
#define PROPERTYCOUNT_INDEX     12

// The following order is mandatory. Please add properties at the end!
#define PROPERTYCOUNT_MENUITEM							6
#define OFFSET_MENUITEM_URL                             0
#define OFFSET_MENUITEM_TITLE                           1
#define OFFSET_MENUITEM_IMAGEIDENTIFIER					2
#define OFFSET_MENUITEM_TARGET							3
#define OFFSET_MENUITEM_CONTEXT							4
#define OFFSET_MENUITEM_SUBMENU							5

// The following order is mandatory. Please add properties at the end!
#define PROPERTYCOUNT_POPUPMENU							4
#define OFFSET_POPUPMENU_TITLE							0
#define OFFSET_POPUPMENU_CONTEXT						1
#define OFFSET_POPUPMENU_SUBMENU						2
#define OFFSET_POPUPMENU_URL							3	// Used for property set

// The following order is mandatory. Please add properties at the end!
#define PROPERTYCOUNT_TOOLBAR                           2
#define OFFSET_TOOLBAR_TITLE                            0
#define OFFSET_TOOLBAR_TOOLBARITEMS                     1

// The following order is mandatory. Please add properties at the end!
#define PROPERTYCOUNT_TOOLBARITEM						7
#define OFFSET_TOOLBARITEM_URL							0
#define OFFSET_TOOLBARITEM_TITLE						1
#define OFFSET_TOOLBARITEM_IMAGEIDENTIFIER				2
#define OFFSET_TOOLBARITEM_TARGET						3
#define OFFSET_TOOLBARITEM_CONTEXT						4
#define OFFSET_TOOLBARITEM_CONTROLTYPE                  5
#define OFFSET_TOOLBARITEM_WIDTH                        6

// The following order is mandatory. Please add properties at the end!
#define PROPERTYCOUNT_STATUSBARITEM                     7
#define OFFSET_STATUSBARITEM_URL                        0
#define OFFSET_STATUSBARITEM_TITLE                      1
#define OFFSET_STATUSBARITEM_CONTEXT                    2
#define OFFSET_STATUSBARITEM_ALIGN                      3
#define OFFSET_STATUSBARITEM_AUTOSIZE                   4
#define OFFSET_STATUSBARITEM_OWNERDRAW                  5
#define OFFSET_STATUSBARITEM_WIDTH                      6


// The following order is mandatory. Please add properties at the end!
#define PROPERTYCOUNT_IMAGES							8
#define PROPERTYCOUNT_EMBEDDED_IMAGES                   4
#define OFFSET_IMAGES_SMALL								0
#define OFFSET_IMAGES_BIG								1
#define OFFSET_IMAGES_SMALLHC							2
#define OFFSET_IMAGES_BIGHC								3
#define OFFSET_IMAGES_SMALL_URL							4
#define OFFSET_IMAGES_BIG_URL							5
#define OFFSET_IMAGES_SMALLHC_URL						6
#define OFFSET_IMAGES_BIGHC_URL							7

#define PROPERTYCOUNT_MERGE_MENUBAR                     6
#define OFFSET_MERGEMENU_MERGEPOINT                     0
#define OFFSET_MERGEMENU_MERGECOMMAND                   1
#define OFFSET_MERGEMENU_MERGECOMMANDPARAMETER          2
#define OFFSET_MERGEMENU_MERGEFALLBACK                  3
#define OFFSET_MERGEMENU_MERGECONTEXT                   4
#define OFFSET_MERGEMENU_MENUITEMS                      5

#define PROPERTYCOUNT_MERGE_TOOLBAR                     7
#define OFFSET_MERGETOOLBAR_TOOLBAR                     0
#define OFFSET_MERGETOOLBAR_MERGEPOINT                  1
#define OFFSET_MERGETOOLBAR_MERGECOMMAND                2
#define OFFSET_MERGETOOLBAR_MERGECOMMANDPARAMETER       3
#define OFFSET_MERGETOOLBAR_MERGEFALLBACK               4
#define OFFSET_MERGETOOLBAR_MERGECONTEXT                5
#define OFFSET_MERGETOOLBAR_TOOLBARITEMS                6

#define PROPERTYCOUNT_MERGE_STATUSBAR                   6
#define OFFSET_MERGESTATUSBAR_MERGEPOINT                0
#define OFFSET_MERGESTATUSBAR_MERGECOMMAND              1
#define OFFSET_MERGESTATUSBAR_MERGECOMMANDPARAMETER     2
#define OFFSET_MERGESTATUSBAR_MERGEFALLBACK             3
#define OFFSET_MERGESTATUSBAR_MERGECONTEXT              4
#define OFFSET_MERGESTATUSBAR_STATUSBARITEMS            5

#define EXPAND_PROTOCOL                                 "vnd.sun.star.expand:"

const Size  aImageSizeSmall( 16, 16 );
const Size  aImageSizeBig( 26, 26 );

//_________________________________________________________________________________________________________________
//	private declarations!
//_________________________________________________________________________________________________________________

/*-****************************************************************************************************************
    @descr  struct to hold information about one menu entry.
****************************************************************************************************************-*/

namespace framework
{

class AddonsOptions_Impl : public ConfigItem
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------

	public:
		//---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------

         AddonsOptions_Impl();
        ~AddonsOptions_Impl();

		//---------------------------------------------------------------------------------------------------------
		//	overloaded methods of baseclass
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		called for notify of configmanager
			@descr		These method is called from the ConfigManager before application ends or from the
			 			PropertyChangeListener if the sub tree broadcasts changes. You must update your
						internal values.

			@seealso	baseclass ConfigItem

            @param      "lPropertyNames" is the list of properties which should be updated.
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

        virtual void Notify( const Sequence< ::rtl::OUString >& lPropertyNames );

		/*-****************************************************************************************************//**
			@short		write changes to configuration
			@descr		These method writes the changed values into the sub tree
						and should always called in our destructor to guarantee consistency of config data.

			@seealso	baseclass ConfigItem

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

    	virtual void Commit();

		//---------------------------------------------------------------------------------------------------------
		//	public interface
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
            @short      base implementation of public interface for "SvtDynamicMenuOptions"!
            @descr      These class is used as static member of "SvtDynamicMenuOptions" ...
						=> The code exist only for one time and isn't duplicated for every instance!

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		sal_Bool										HasAddonsMenu		 () const ;
		sal_Bool										HasAddonsHelpMenu	 () const ;
        sal_Int32                                       GetAddonsToolBarCount() const ;
        const Sequence< Sequence< PropertyValue > >&	GetAddonsMenu		 () const ;
		const Sequence< Sequence< PropertyValue > >&	GetAddonsMenuBarPart () const ;
        const Sequence< Sequence< PropertyValue > >&	GetAddonsToolBarPart ( sal_uInt32 nIndex ) const ;
        const ::rtl::OUString                           GetAddonsToolbarResourceName( sal_uInt32 nIndex ) const;
        const ::rtl::OUString                           GetAddonsToolbarUIName( sal_uInt32 nIndex ) const;
		const Sequence< Sequence< PropertyValue > >&	GetAddonsHelpMenu	 () const ;
		Image											GetImageFromURL( const rtl::OUString& aURL, sal_Bool bBig, sal_Bool bHiContrast, sal_Bool bNoScale ) const;
        const MergeMenuInstructionContainer&            GetMergeMenuInstructions() const;
        bool                                            GetMergeToolbarInstructions( const ::rtl::OUString& rToolbarName, MergeToolbarInstructionContainer& rToolbarInstructions ) const;
        const MergeStatusbarInstructionContainer&       GetMergeStatusbarInstructions() const;
        void                                            ReadConfigurationData();

	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------

	private:

		struct ImageEntry
		{
			Image	aImageSmall;
			Image	aImageBig;
			Image	aImageSmallHC;
			Image	aImageBigHC;

            Image   aImageSmallNoScale;
            Image   aImageBigNoScale;
            Image   aImageSmallHCNoScale;
            Image   aImageBigHCNoScale;
		};

		typedef std::hash_map< ::rtl::OUString, ImageEntry, rtl::OUStringHash, ::std::equal_to< ::rtl::OUString > > ImageManager;
        typedef std::hash_map< ::rtl::OUString, sal_uInt32, rtl::OUStringHash, ::std::equal_to< ::rtl::OUString > > StringToIndexMap;
        typedef std::vector< Sequence< Sequence< PropertyValue > > > AddonToolBars;
        typedef ::std::hash_map< ::rtl::OUString, MergeToolbarInstructionContainer, rtl::OUStringHash, ::std::equal_to< ::rtl::OUString > > ToolbarMergingInstructions;

        enum ImageSize
        {
            IMGSIZE_SMALL,
            IMGSIZE_BIG
        };

		/*-****************************************************************************************************//**
            @short      return list of key names of our configuration management which represent our module tree
			@descr		These methods return the current list of key names! We need it to get needed values from our
                        configuration management!

			@seealso	-

            @param      "nCount"     ,   returns count of menu entries for "new"
			@return		A list of configuration key names is returned.

			@onerror	-
		*//*-*****************************************************************************************************/

		sal_Bool			 ReadAddonMenuSet( Sequence< Sequence< PropertyValue > >& aAddonMenuSeq );
		sal_Bool			 ReadOfficeMenuBarSet( Sequence< Sequence< PropertyValue > >& aAddonOfficeMenuBarSeq );
        sal_Bool			 ReadOfficeToolBarSet( AddonToolBars& rAddonOfficeToolBars,
                                                   std::vector< rtl::OUString >& rAddonOfficeToolBarResNames,
                                                   std::vector< rtl::OUString >& rAddonOfficeToolBarUINames );
        sal_Bool			 ReadToolBarItemSet( const rtl::OUString rToolBarItemSetNodeName, Sequence< Sequence< PropertyValue > >& aAddonOfficeToolBarSeq );
		sal_Bool			 ReadOfficeHelpSet( Sequence< Sequence< PropertyValue > >& aAddonOfficeHelpMenuSeq );
		sal_Bool			 ReadImages( ImageManager& aImageManager );
        sal_Bool             ReadMenuMergeInstructions( MergeMenuInstructionContainer& rContainer );
        sal_Bool             ReadToolbarMergeInstructions( ToolbarMergingInstructions& rToolbarMergeMap );
        sal_Bool             ReadStatusbarMergeInstructions( MergeStatusbarInstructionContainer& rContainer );

        sal_Bool             ReadMergeMenuData( const ::rtl::OUString& aMergeAddonInstructionBase, Sequence< Sequence< PropertyValue > >& rMergeMenu );
        sal_Bool             ReadMergeToolbarData( const ::rtl::OUString& aMergeAddonInstructionBase, Sequence< Sequence< PropertyValue > >& rMergeToolbarItems );
        sal_Bool             ReadMergeStatusbarData( const ::rtl::OUString& aMergeAddonInstructionBase, Sequence< Sequence< PropertyValue > >& rMergeStatusbar );
		sal_Bool			 ReadMenuItem( const ::rtl::OUString& aMenuItemNodeName, Sequence< PropertyValue >& aMenuItem, sal_Bool bIgnoreSubMenu = sal_False );
		sal_Bool			 ReadPopupMenu( const ::rtl::OUString& aPopupMenuNodeName, Sequence< PropertyValue >& aPopupMenu );
        sal_Bool             AppendPopupMenu( Sequence< PropertyValue >& aTargetPopupMenu, const Sequence< PropertyValue >& rSourcePopupMenu );
		sal_Bool			 ReadToolBarItem( const ::rtl::OUString& aToolBarItemNodeName, Sequence< PropertyValue >& aToolBarItem );
        sal_Bool             ReadStatusBarItem( const ::rtl::OUString& aStatusbarItemNodeName, Sequence< PropertyValue >& aStatusbarItem );
		sal_Bool			 ReadImagesItem( const ::rtl::OUString& aImagesItemNodeName, Sequence< PropertyValue >& aImagesItem );
		ImageEntry*			 ReadImageData( const ::rtl::OUString& aImagesNodeName );
		void                 ReadAndAssociateImages( const ::rtl::OUString& aURL, const ::rtl::OUString& aImageId );
		void                 ReadImageFromURL( ImageSize nImageSize, const ::rtl::OUString& aURL, Image& aImage, Image& aNoScaleImage );
		sal_Bool             HasAssociatedImages( const ::rtl::OUString& aURL );
		void                 SubstituteVariables( ::rtl::OUString& aURL );

		sal_Bool			 ReadSubMenuEntries( const Sequence< ::rtl::OUString >& aSubMenuNodeNames, Sequence< Sequence< PropertyValue > >& rSubMenu );
		void				 InsertToolBarSeparator( Sequence< Sequence< PropertyValue > >& rAddonOfficeToolBarSeq );
		::rtl::OUString			 GeneratePrefixURL();

		Sequence< ::rtl::OUString > GetPropertyNamesMergeMenuInstruction( const ::rtl::OUString& aPropertyRootName ) const;
        Sequence< ::rtl::OUString > GetPropertyNamesMenuItem( const ::rtl::OUString& aPropertyRootNode ) const;
		Sequence< ::rtl::OUString > GetPropertyNamesPopupMenu( const ::rtl::OUString& aPropertyRootNode ) const;
        Sequence< ::rtl::OUString > GetPropertyNamesToolBar( const ::rtl::OUString& aPropertyRootNode ) const;
		Sequence< ::rtl::OUString > GetPropertyNamesToolBarItem( const ::rtl::OUString& aPropertyRootNode ) const;
        Sequence< ::rtl::OUString > GetPropertyNamesStatusbarItem( const ::rtl::OUString& aPropertyRootNode ) const;
		Sequence< ::rtl::OUString > GetPropertyNamesImages( const ::rtl::OUString& aPropertyRootNode ) const;
		sal_Bool			 CreateImageFromSequence( Image& rImage, sal_Bool bBig, Sequence< sal_Int8 >& rBitmapDataSeq ) const;

	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------

	private:
		ImageEntry*	ReadOptionalImageData( const ::rtl::OUString& aMenuNodeName );

		sal_Int32                                         m_nRootAddonPopupMenuId;
		::rtl::OUString                                          m_aPropNames[PROPERTYCOUNT_INDEX];
		::rtl::OUString                                          m_aPropImagesNames[PROPERTYCOUNT_IMAGES];
        ::rtl::OUString                                          m_aPropMergeMenuNames[PROPERTYCOUNT_MERGE_MENUBAR];
        ::rtl::OUString                                          m_aPropMergeToolbarNames[PROPERTYCOUNT_MERGE_TOOLBAR];
        ::rtl::OUString                                          m_aPropMergeStatusbarNames[PROPERTYCOUNT_MERGE_STATUSBAR];
		::rtl::OUString                                          m_aEmpty;
		::rtl::OUString                                          m_aPathDelimiter;
		::rtl::OUString                                          m_aSeparator;
		::rtl::OUString                                          m_aRootAddonPopupMenuURLPrexfix;
		::rtl::OUString                                          m_aPrivateImageURL;
		Sequence< Sequence< PropertyValue > >             m_aCachedMenuProperties;
		Sequence< Sequence< PropertyValue > >             m_aCachedMenuBarPartProperties;
        AddonToolBars                                     m_aCachedToolBarPartProperties;
        std::vector< rtl::OUString >                      m_aCachedToolBarPartResourceNames;
        std::vector< rtl::OUString >                      m_aCachedToolBarPartUINames;
		Sequence< Sequence< PropertyValue > >             m_aCachedHelpMenuProperties;
		Reference< com::sun::star::util::XMacroExpander > m_xMacroExpander;
		ImageManager                                      m_aImageManager;
        Sequence< Sequence< PropertyValue > >             m_aEmptyAddonToolBar;
        MergeMenuInstructionContainer                     m_aCachedMergeMenuInsContainer;
        ToolbarMergingInstructions                        m_aCachedToolbarMergingInstructions;
        MergeStatusbarInstructionContainer                m_aCachedStatusbarMergingInstructions;
};

//_________________________________________________________________________________________________________________
//	definitions
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
AddonsOptions_Impl::AddonsOptions_Impl()
    // Init baseclasses first
    : ConfigItem( ROOTNODE_ADDONMENU ),
	m_nRootAddonPopupMenuId( 0 ),
	m_aPathDelimiter( PATHDELIMITER ),
	m_aSeparator( SEPARATOR_URL ),
	m_aRootAddonPopupMenuURLPrexfix( ADDONSPOPUPMENU_URL_PREFIX ),
	m_aPrivateImageURL( PRIVATE_IMAGE_URL )
{
	// initialize array with fixed property names
	m_aPropNames[ INDEX_URL				] = PROPERTYNAME_URL;
	m_aPropNames[ INDEX_TITLE			] = PROPERTYNAME_TITLE;
	m_aPropNames[ INDEX_TARGET			] = PROPERTYNAME_TARGET;
	m_aPropNames[ INDEX_IMAGEIDENTIFIER	] = PROPERTYNAME_IMAGEIDENTIFIER;
	m_aPropNames[ INDEX_CONTEXT			] = PROPERTYNAME_CONTEXT;
	m_aPropNames[ INDEX_SUBMENU			] = PROPERTYNAME_SUBMENU; // Submenu set!
    m_aPropNames[ INDEX_CONTROLTYPE     ] = PROPERTYNAME_CONTROLTYPE;
    m_aPropNames[ INDEX_WIDTH           ] = PROPERTYNAME_WIDTH;
    m_aPropNames[ INDEX_ALIGN           ] = PROPERTYNAME_ALIGN;
    m_aPropNames[ INDEX_AUTOSIZE        ] = PROPERTYNAME_AUTOSIZE;
    m_aPropNames[ INDEX_OWNERDRAW       ] = PROPERTYNAME_OWNERDRAW;
    m_aPropNames[ INDEX_TOOLBARITEMS    ] = PROPERTYNAME_TOOLBARITEMS;

	// initialize array with fixed images property names
	m_aPropImagesNames[ OFFSET_IMAGES_SMALL			] = PROPERTYNAME_IMAGESMALL;
	m_aPropImagesNames[ OFFSET_IMAGES_BIG			] = PROPERTYNAME_IMAGEBIG;
	m_aPropImagesNames[ OFFSET_IMAGES_SMALLHC		] = PROPERTYNAME_IMAGESMALLHC;
	m_aPropImagesNames[ OFFSET_IMAGES_BIGHC			] = PROPERTYNAME_IMAGEBIGHC;
	m_aPropImagesNames[ OFFSET_IMAGES_SMALL_URL		] = PROPERTYNAME_IMAGESMALL_URL;
	m_aPropImagesNames[ OFFSET_IMAGES_BIG_URL	    ] = PROPERTYNAME_IMAGEBIG_URL;
	m_aPropImagesNames[ OFFSET_IMAGES_SMALLHC_URL	] = PROPERTYNAME_IMAGESMALLHC_URL;
	m_aPropImagesNames[ OFFSET_IMAGES_BIGHC_URL		] = PROPERTYNAME_IMAGEBIGHC_URL;

    // initialize array with fixed merge menu property names
    m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGEPOINT    ] = PROPERTYNAME_MERGEMENU_MERGEPOINT;
    m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGECOMMAND  ] = PROPERTYNAME_MERGEMENU_MERGECOMMAND;
    m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGECOMMANDPARAMETER ] = PROPERTYNAME_MERGEMENU_MERGECOMMANDPARAMETER;
    m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGEFALLBACK ] = PROPERTYNAME_MERGEMENU_MERGEFALLBACK;
    m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGECONTEXT  ] = PROPERTYNAME_MERGEMENU_MERGECONTEXT;
    m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MENUITEMS     ] = PROPERTYNAME_MERGEMENU_MENUITEMS;

    m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_TOOLBAR               ] = PROPERTYNAME_MERGETOOLBAR_TOOLBAR;
    m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_MERGEPOINT            ] = PROPERTYNAME_MERGETOOLBAR_MERGEPOINT;
    m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_MERGECOMMAND          ] = PROPERTYNAME_MERGETOOLBAR_MERGECOMMAND;
    m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_MERGECOMMANDPARAMETER ] = PROPERTYNAME_MERGETOOLBAR_MERGECOMMANDPARAMETER;
    m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_MERGEFALLBACK         ] = PROPERTYNAME_MERGETOOLBAR_MERGEFALLBACK;
    m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_MERGECONTEXT          ] = PROPERTYNAME_MERGETOOLBAR_MERGECONTEXT;
    m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_TOOLBARITEMS          ] = PROPERTYNAME_MERGETOOLBAR_TOOLBARITEMS;

    m_aPropMergeStatusbarNames[ OFFSET_MERGESTATUSBAR_MERGEPOINT            ] = PROPERTYNAME_MERGESTATUSBAR_MERGEPOINT;
    m_aPropMergeStatusbarNames[ OFFSET_MERGESTATUSBAR_MERGECOMMAND          ] = PROPERTYNAME_MERGESTATUSBAR_MERGECOMMAND;
    m_aPropMergeStatusbarNames[ OFFSET_MERGESTATUSBAR_MERGECOMMANDPARAMETER ] = PROPERTYNAME_MERGESTATUSBAR_MERGECOMMANDPARAMETER;
    m_aPropMergeStatusbarNames[ OFFSET_MERGESTATUSBAR_MERGEFALLBACK         ] = PROPERTYNAME_MERGESTATUSBAR_MERGEFALLBACK;
    m_aPropMergeStatusbarNames[ OFFSET_MERGESTATUSBAR_MERGECONTEXT          ] = PROPERTYNAME_MERGESTATUSBAR_MERGECONTEXT;
    m_aPropMergeStatusbarNames[ OFFSET_MERGESTATUSBAR_STATUSBARITEMS        ] = PROPERTYNAME_MERGESTATUSBAR_STATUSBARITEMS;

    Reference< XComponentContext > xContext;
    Reference< com::sun::star::beans::XPropertySet > xProps( ::comphelper::getProcessServiceFactory(), UNO_QUERY );
    xProps->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DefaultContext" ))) >>= xContext;
    if ( xContext.is() )
    {
        m_xMacroExpander =  Reference< com::sun::star::util::XMacroExpander >( xContext->getValueByName(
                                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/singletons/com.sun.star.util.theMacroExpander"))),
                                UNO_QUERY );
    }

    ReadConfigurationData();

    // Enable notification mechanism of our baseclass.
	// We need it to get information about changes outside these class on our used configuration keys!
    Sequence< rtl::OUString > aNotifySeq( 1 );
    aNotifySeq[0] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "AddonUI" ));
    EnableNotification( aNotifySeq );
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
AddonsOptions_Impl::~AddonsOptions_Impl()
{
	// We must save our current values .. if user forget it!
	if( IsModified() == sal_True )
	{
		Commit();
	}
}

void AddonsOptions_Impl::ReadConfigurationData()
{
    // reset members to be read again from configuration
    m_aCachedMenuProperties = Sequence< Sequence< PropertyValue > >();
    m_aCachedMenuBarPartProperties = Sequence< Sequence< PropertyValue > >();
    m_aCachedToolBarPartProperties = AddonToolBars();
    m_aCachedHelpMenuProperties = Sequence< Sequence< PropertyValue > >();
    m_aCachedToolBarPartResourceNames.clear();
    m_aCachedToolBarPartUINames.clear();
    m_aImageManager = ImageManager();

    ReadAddonMenuSet( m_aCachedMenuProperties );
	ReadOfficeMenuBarSet( m_aCachedMenuBarPartProperties );
	ReadOfficeToolBarSet( m_aCachedToolBarPartProperties,
                          m_aCachedToolBarPartResourceNames,
                          m_aCachedToolBarPartUINames );

	ReadOfficeHelpSet( m_aCachedHelpMenuProperties );
	ReadImages( m_aImageManager );

    m_aCachedMergeMenuInsContainer.clear();
    m_aCachedToolbarMergingInstructions.clear();
    m_aCachedStatusbarMergingInstructions.clear();

    ReadMenuMergeInstructions( m_aCachedMergeMenuInsContainer );
    ReadToolbarMergeInstructions( m_aCachedToolbarMergingInstructions );
    ReadStatusbarMergeInstructions( m_aCachedStatusbarMergingInstructions );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void AddonsOptions_Impl::Notify( const Sequence< ::rtl::OUString >& /*lPropertyNames*/ )
{
    Application::PostUserEvent( STATIC_LINK( 0, AddonsOptions, Notify ) );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void AddonsOptions_Impl::Commit()
{
    DBG_ERROR( "AddonsOptions_Impl::Commit()\nNot implemented yet!\n" );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::HasAddonsMenu() const
{
	return ( m_aCachedMenuProperties.getLength() > 0 );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::HasAddonsHelpMenu	() const
{
	return ( m_aCachedHelpMenuProperties.getLength() > 0 );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Int32 AddonsOptions_Impl::GetAddonsToolBarCount() const
{
    return m_aCachedToolBarPartProperties.size();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const Sequence< Sequence< PropertyValue > >& AddonsOptions_Impl::GetAddonsMenu() const
{
	return m_aCachedMenuProperties;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const Sequence< Sequence< PropertyValue > >& AddonsOptions_Impl::GetAddonsMenuBarPart() const
{
	return m_aCachedMenuBarPartProperties;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const Sequence< Sequence< PropertyValue > >& AddonsOptions_Impl::GetAddonsToolBarPart( sal_uInt32 nIndex ) const
{
    if ( /*nIndex >= 0 &&*/ nIndex < m_aCachedToolBarPartProperties.size() )
        return m_aCachedToolBarPartProperties[nIndex];
    else
        return m_aEmptyAddonToolBar;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const ::rtl::OUString AddonsOptions_Impl::GetAddonsToolbarResourceName( sal_uInt32 nIndex ) const
{
    if ( nIndex < m_aCachedToolBarPartResourceNames.size() )
        return m_aCachedToolBarPartResourceNames[nIndex];
    else
        return rtl::OUString();
}


const ::rtl::OUString AddonsOptions_Impl::GetAddonsToolbarUIName( sal_uInt32 nIndex ) const
{
    if ( nIndex < m_aCachedToolBarPartUINames.size() )
        return m_aCachedToolBarPartUINames[nIndex];
    else
        return rtl::OUString();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const Sequence< Sequence< PropertyValue > >& AddonsOptions_Impl::GetAddonsHelpMenu	() const
{
	return m_aCachedHelpMenuProperties;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const MergeMenuInstructionContainer& AddonsOptions_Impl::GetMergeMenuInstructions() const
{
    return m_aCachedMergeMenuInsContainer;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
bool AddonsOptions_Impl::GetMergeToolbarInstructions(
    const ::rtl::OUString& rToolbarName,
    MergeToolbarInstructionContainer& rToolbarInstructions ) const
{
    ToolbarMergingInstructions::const_iterator pIter = m_aCachedToolbarMergingInstructions.find( rToolbarName );
    if ( pIter != m_aCachedToolbarMergingInstructions.end() )
    {
        rToolbarInstructions = pIter->second;
        return true;
    }
    else
        return false;
}

const MergeStatusbarInstructionContainer& AddonsOptions_Impl::GetMergeStatusbarInstructions() const
{
    return m_aCachedStatusbarMergingInstructions;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Image AddonsOptions_Impl::GetImageFromURL( const rtl::OUString& aURL, sal_Bool bBig, sal_Bool bHiContrast, sal_Bool bNoScale ) const
{
	Image aImage;

	ImageManager::const_iterator pIter = m_aImageManager.find( aURL );
	if ( pIter != m_aImageManager.end() )
	{
		if ( !bHiContrast  )
        {
			if ( bNoScale )
                aImage = ( bBig ? pIter->second.aImageBigNoScale : pIter->second.aImageSmallNoScale );
            if ( !aImage )
                aImage = ( bBig ? pIter->second.aImageBig : pIter->second.aImageSmall );
        }
		else
        {
			if ( bNoScale )
                aImage = ( bBig ? pIter->second.aImageBigHCNoScale : pIter->second.aImageSmallHCNoScale );
            if ( !aImage )
			    aImage = ( bBig ? pIter->second.aImageBigHC : pIter->second.aImageSmallHC );
        }
	}

	return aImage;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadAddonMenuSet( Sequence< Sequence< PropertyValue > >& rAddonMenuSeq )
{
	// Read the AddonMenu set and fill property sequences
	::rtl::OUString				aAddonMenuNodeName( RTL_CONSTASCII_USTRINGPARAM( "AddonUI/AddonMenu" ));
	Sequence< ::rtl::OUString >	aAddonMenuNodeSeq = GetNodeNames( aAddonMenuNodeName );
	::rtl::OUString				aAddonMenuItemNode( aAddonMenuNodeName + m_aPathDelimiter );

	sal_uInt32				nCount = aAddonMenuNodeSeq.getLength();
	sal_uInt32				nIndex = 0;
	Sequence< PropertyValue > aMenuItem( PROPERTYCOUNT_MENUITEM );

	// Init the property value sequence
	aMenuItem[ OFFSET_MENUITEM_URL				].Name = m_aPropNames[ INDEX_URL			];
	aMenuItem[ OFFSET_MENUITEM_TITLE			].Name = m_aPropNames[ INDEX_TITLE			];
	aMenuItem[ OFFSET_MENUITEM_TARGET			].Name = m_aPropNames[ INDEX_TARGET			];
	aMenuItem[ OFFSET_MENUITEM_IMAGEIDENTIFIER	].Name = m_aPropNames[ INDEX_IMAGEIDENTIFIER];
	aMenuItem[ OFFSET_MENUITEM_CONTEXT			].Name = m_aPropNames[ INDEX_CONTEXT		];
	aMenuItem[ OFFSET_MENUITEM_SUBMENU			].Name = m_aPropNames[ INDEX_SUBMENU		];	// Submenu set!

	for ( sal_uInt32 n = 0; n < nCount; n++ )
	{
		::rtl::OUString aRootMenuItemNode( aAddonMenuItemNode + aAddonMenuNodeSeq[n] );

		// Read the MenuItem
		if ( ReadMenuItem( aRootMenuItemNode, aMenuItem ) )
		{
			// Successfully read a menu item, append to our list
			sal_uInt32 nMenuItemCount = rAddonMenuSeq.getLength() + 1;
			rAddonMenuSeq.realloc( nMenuItemCount );
			rAddonMenuSeq[nIndex++] = aMenuItem;
		}
	}

	return ( rAddonMenuSeq.getLength() > 0 );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadOfficeHelpSet( Sequence< Sequence< PropertyValue > >& rAddonOfficeHelpMenuSeq )
{
	// Read the AddonMenu set and fill property sequences
	::rtl::OUString				aAddonHelpMenuNodeName( RTL_CONSTASCII_USTRINGPARAM( "AddonUI/OfficeHelp" ));
	Sequence< ::rtl::OUString >	aAddonHelpMenuNodeSeq = GetNodeNames( aAddonHelpMenuNodeName );
	::rtl::OUString				aAddonHelpMenuItemNode( aAddonHelpMenuNodeName + m_aPathDelimiter );

	sal_uInt32				nCount = aAddonHelpMenuNodeSeq.getLength();
	sal_uInt32				nIndex = 0;
	Sequence< PropertyValue > aMenuItem( PROPERTYCOUNT_MENUITEM );

	// Init the property value sequence
	aMenuItem[ OFFSET_MENUITEM_URL				].Name = m_aPropNames[ INDEX_URL			];
	aMenuItem[ OFFSET_MENUITEM_TITLE			].Name = m_aPropNames[ INDEX_TITLE			];
	aMenuItem[ OFFSET_MENUITEM_TARGET			].Name = m_aPropNames[ INDEX_TARGET			];
	aMenuItem[ OFFSET_MENUITEM_IMAGEIDENTIFIER	].Name = m_aPropNames[ INDEX_IMAGEIDENTIFIER];
	aMenuItem[ OFFSET_MENUITEM_CONTEXT			].Name = m_aPropNames[ INDEX_CONTEXT		];
	aMenuItem[ OFFSET_MENUITEM_SUBMENU			].Name = m_aPropNames[ INDEX_SUBMENU		];	// Submenu set!

	for ( sal_uInt32 n = 0; n < nCount; n++ )
	{
		::rtl::OUString aRootMenuItemNode( aAddonHelpMenuItemNode + aAddonHelpMenuNodeSeq[n] );

		// Read the MenuItem
		if ( ReadMenuItem( aRootMenuItemNode, aMenuItem, sal_True ) )
		{
			// Successfully read a menu item, append to our list
			sal_uInt32 nMenuItemCount = rAddonOfficeHelpMenuSeq.getLength() + 1;
			rAddonOfficeHelpMenuSeq.realloc( nMenuItemCount );
			rAddonOfficeHelpMenuSeq[nIndex++] = aMenuItem;
		}
	}

	return ( rAddonOfficeHelpMenuSeq.getLength() > 0 );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadOfficeMenuBarSet( Sequence< Sequence< PropertyValue > >& rAddonOfficeMenuBarSeq )
{
	// Read the OfficeMenuBar set and fill property sequences
	::rtl::OUString				aAddonMenuBarNodeName( RTL_CONSTASCII_USTRINGPARAM( "AddonUI/OfficeMenuBar" ));
	Sequence< ::rtl::OUString >	aAddonMenuBarNodeSeq = GetNodeNames( aAddonMenuBarNodeName );
	::rtl::OUString				aAddonMenuBarNode( aAddonMenuBarNodeName + m_aPathDelimiter );

	sal_uInt32				nCount = aAddonMenuBarNodeSeq.getLength();
	sal_uInt32				nIndex = 0;
	Sequence< PropertyValue > aPopupMenu( PROPERTYCOUNT_POPUPMENU );

	// Init the property value sequence
	aPopupMenu[ OFFSET_POPUPMENU_TITLE		].Name = m_aPropNames[ INDEX_TITLE	];
	aPopupMenu[ OFFSET_POPUPMENU_CONTEXT	].Name = m_aPropNames[ INDEX_CONTEXT];
	aPopupMenu[ OFFSET_POPUPMENU_SUBMENU	].Name = m_aPropNames[ INDEX_SUBMENU];
	aPopupMenu[ OFFSET_POPUPMENU_URL		].Name = m_aPropNames[ INDEX_URL	];

    StringToIndexMap aTitleToIndexMap;

    for ( sal_uInt32 n = 0; n < nCount; n++ )
	{
		::rtl::OUString aPopupMenuNode( aAddonMenuBarNode + aAddonMenuBarNodeSeq[n] );

		// Read the MenuItem
		if ( ReadPopupMenu( aPopupMenuNode, aPopupMenu ) )
		{
			// Successfully read a popup menu, append to our list
            ::rtl::OUString aPopupTitle;
            if ( aPopupMenu[OFFSET_POPUPMENU_TITLE].Value >>= aPopupTitle )
            {
                StringToIndexMap::const_iterator pIter = aTitleToIndexMap.find( aPopupTitle );
                if ( pIter != aTitleToIndexMap.end() )
                {
                    // title already there => concat both popup menus
                    Sequence< PropertyValue >& rOldPopupMenu = rAddonOfficeMenuBarSeq[pIter->second];
                    AppendPopupMenu( rOldPopupMenu, aPopupMenu );
                }
                else
                {
                    // not found
			        sal_uInt32 nMenuItemCount = rAddonOfficeMenuBarSeq.getLength() + 1;
			        rAddonOfficeMenuBarSeq.realloc( nMenuItemCount );
			        rAddonOfficeMenuBarSeq[nIndex] = aPopupMenu;
                    aTitleToIndexMap.insert( StringToIndexMap::value_type( aPopupTitle, nIndex ));
                    ++nIndex;
                }
            }
		}
	}

	return ( rAddonOfficeMenuBarSeq.getLength() > 0 );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadOfficeToolBarSet(
    AddonToolBars& rAddonOfficeToolBars,
    std::vector< rtl::OUString >& rAddonOfficeToolBarResNames,
    std::vector< rtl::OUString >& rAddonOfficeToolBarUINames )
{
	// Read the OfficeToolBar set and fill property sequences
	::rtl::OUString             aAddonToolBarNodeName( RTL_CONSTASCII_USTRINGPARAM( "AddonUI/OfficeToolBar" ));
	Sequence< ::rtl::OUString > aAddonToolBarNodeSeq = GetNodeNames( aAddonToolBarNodeName );
	::rtl::OUString             aAddonToolBarNode( aAddonToolBarNodeName + m_aPathDelimiter );

	sal_uInt32           nCount = aAddonToolBarNodeSeq.getLength();

	for ( sal_uInt32 n = 0; n < nCount; n++ )
	{
        ::rtl::OUString aToolBarNode( aAddonToolBarNode + aAddonToolBarNodeSeq[n] + m_aPathDelimiter );
        rAddonOfficeToolBarResNames.push_back( aAddonToolBarNodeSeq[n] );
        rAddonOfficeToolBars.push_back( m_aEmptyAddonToolBar );

        Sequence< Any > aToolBarNodeValues = GetProperties( GetPropertyNamesToolBar( aToolBarNode ) );

        rtl::OUString aUIName;
        aToolBarNodeValues[OFFSET_TOOLBAR_TITLE] >>= aUIName;
        rAddonOfficeToolBarUINames.push_back( aUIName );

        Reference < XInterface > xToolbarItems;
        if ( ( aToolBarNodeValues[OFFSET_TOOLBAR_TOOLBARITEMS] >>= xToolbarItems ) && xToolbarItems.is() )
        {
            ::rtl::OUStringBuffer aBuffer;
            aBuffer.append( aToolBarNode );
            aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( STR_TOOLBARITEMS ) );
            ReadToolBarItemSet( aBuffer.makeStringAndClear(), rAddonOfficeToolBars[n] );
        }
	}

	return ( !rAddonOfficeToolBars.empty() );
}


//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadToolBarItemSet( const rtl::OUString rToolBarItemSetNodeName, Sequence< Sequence< PropertyValue > >& rAddonOfficeToolBarSeq )
{
	sal_Bool					bInsertSeparator		= sal_False;
	sal_uInt32					nToolBarItemCount		= rAddonOfficeToolBarSeq.getLength();
	::rtl::OUString					aAddonToolBarItemSetNode( rToolBarItemSetNodeName + m_aPathDelimiter );
	Sequence< ::rtl::OUString >		aAddonToolBarItemSetNodeSeq = GetNodeNames( rToolBarItemSetNodeName );
	Sequence< PropertyValue >	aToolBarItem( PROPERTYCOUNT_TOOLBARITEM );

	// Init the property value sequence
	aToolBarItem[ OFFSET_TOOLBARITEM_URL				].Name = m_aPropNames[ INDEX_URL			];
	aToolBarItem[ OFFSET_TOOLBARITEM_TITLE				].Name = m_aPropNames[ INDEX_TITLE			];
	aToolBarItem[ OFFSET_TOOLBARITEM_IMAGEIDENTIFIER	].Name = m_aPropNames[ INDEX_IMAGEIDENTIFIER];
	aToolBarItem[ OFFSET_TOOLBARITEM_TARGET				].Name = m_aPropNames[ INDEX_TARGET			];
	aToolBarItem[ OFFSET_TOOLBARITEM_CONTEXT			].Name = m_aPropNames[ INDEX_CONTEXT		];
	aToolBarItem[ OFFSET_TOOLBARITEM_CONTROLTYPE		].Name = m_aPropNames[ INDEX_CONTROLTYPE	];
	aToolBarItem[ OFFSET_TOOLBARITEM_WIDTH			    ].Name = m_aPropNames[ INDEX_WIDTH		    ];

	sal_uInt32 nCount = aAddonToolBarItemSetNodeSeq.getLength();
	for ( sal_uInt32 n = 0; n < nCount; n++ )
	{
		::rtl::OUString aToolBarItemNode( aAddonToolBarItemSetNode + aAddonToolBarItemSetNodeSeq[n] );

		// Read the ToolBarItem
		if ( ReadToolBarItem( aToolBarItemNode, aToolBarItem ) )
		{
			if ( bInsertSeparator )
			{
				bInsertSeparator = sal_False;
				InsertToolBarSeparator( rAddonOfficeToolBarSeq );
			}

			// Successfully read a toolbar item, append to our list
			sal_uInt32 nAddonCount = rAddonOfficeToolBarSeq.getLength();
			rAddonOfficeToolBarSeq.realloc( nAddonCount+1 );
			rAddonOfficeToolBarSeq[nAddonCount] = aToolBarItem;
		}
	}

	return ( (sal_uInt32)rAddonOfficeToolBarSeq.getLength() > nToolBarItemCount );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
void AddonsOptions_Impl::InsertToolBarSeparator( Sequence< Sequence< PropertyValue > >& rAddonOfficeToolBarSeq )
{
	Sequence< PropertyValue >	aToolBarItem( PROPERTYCOUNT_TOOLBARITEM );

	aToolBarItem[ OFFSET_TOOLBARITEM_URL				].Name = m_aPropNames[ INDEX_URL			];
	aToolBarItem[ OFFSET_TOOLBARITEM_TITLE				].Name = m_aPropNames[ INDEX_TITLE			];
	aToolBarItem[ OFFSET_TOOLBARITEM_IMAGEIDENTIFIER	].Name = m_aPropNames[ INDEX_IMAGEIDENTIFIER];
	aToolBarItem[ OFFSET_TOOLBARITEM_TARGET				].Name = m_aPropNames[ INDEX_TARGET			];
	aToolBarItem[ OFFSET_TOOLBARITEM_CONTEXT			].Name = m_aPropNames[ INDEX_CONTEXT		];

	aToolBarItem[ OFFSET_TOOLBARITEM_URL				].Value <<= SEPARATOR_URL;
	aToolBarItem[ OFFSET_TOOLBARITEM_TITLE				].Value <<= m_aEmpty;
	aToolBarItem[ OFFSET_TOOLBARITEM_TARGET				].Value <<= m_aEmpty;
	aToolBarItem[ OFFSET_TOOLBARITEM_IMAGEIDENTIFIER	].Value <<= m_aEmpty;
	aToolBarItem[ OFFSET_TOOLBARITEM_CONTEXT			].Value <<= m_aEmpty;

	sal_uInt32 nToolBarItemCount = rAddonOfficeToolBarSeq.getLength();
	rAddonOfficeToolBarSeq.realloc( nToolBarItemCount+1 );
	rAddonOfficeToolBarSeq[nToolBarItemCount] = aToolBarItem;
}


//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadImages( ImageManager& aImageManager )
{
	// Read the user-defined Images set and fill image manager
	::rtl::OUString                aAddonImagesNodeName( RTL_CONSTASCII_USTRINGPARAM( "AddonUI/Images" ));
	Sequence< ::rtl::OUString >	aAddonImagesNodeSeq = GetNodeNames( aAddonImagesNodeName );
	::rtl::OUString                aAddonImagesNode( aAddonImagesNodeName + m_aPathDelimiter );

	sal_uInt32              nCount = aAddonImagesNodeSeq.getLength();

	// Init the property value sequence
	Sequence< ::rtl::OUString >	aAddonImageItemNodePropNames( 1 );
	::rtl::OUString                aURL;

	for ( sal_uInt32 n = 0; n < nCount; n++ )
	{
		::rtl::OUString aImagesItemNode( aAddonImagesNode + aAddonImagesNodeSeq[n] );

		// Create sequence for data access
		::rtl::OUStringBuffer aBuffer( aImagesItemNode );
		aBuffer.append( m_aPathDelimiter );
		aBuffer.append( m_aPropNames[ OFFSET_MENUITEM_URL ] );
		aAddonImageItemNodePropNames[0] = aBuffer.makeStringAndClear();

		Sequence< Any > aAddonImageItemNodeValues = GetProperties( aAddonImageItemNodePropNames );

		// An user-defined image entry must have an URL. As "ImageIdentifier" has a higher priority
		// we also check if we already have an images association.
		if (( aAddonImageItemNodeValues[0] >>= aURL ) &&
		    aURL.getLength() > 0 &&
		    !HasAssociatedImages( aURL ))
		{
			::rtl::OUStringBuffer aBuf( aImagesItemNode );
			aBuf.append( m_aPathDelimiter );
			aBuf.append( IMAGES_NODENAME );
			aBuf.append( m_aPathDelimiter );
			::rtl::OUString aImagesUserDefinedItemNode = aBuf.makeStringAndClear();

			// Read a user-defined images data
			ImageEntry* pImageEntry = ReadImageData( aImagesUserDefinedItemNode );
			if ( pImageEntry )
			{
				// Successfully read a user-defined images item, put it into our image manager
				aImageManager.insert( ImageManager::value_type( aURL, *pImageEntry ));
				delete pImageEntry; // We have the ownership of the pointer
			}
		}
	}

	return sal_True;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************

::rtl::OUString AddonsOptions_Impl::GeneratePrefixURL()
{
	// Create an unique prefixed Add-On popup menu URL so it can be identified later as a runtime popup menu.
	// They use a different image manager, so they must be identified by the sfx2/framework code.
	::rtl::OUString aPopupMenuURL;
	::rtl::OUStringBuffer aBuf( m_aRootAddonPopupMenuURLPrexfix.getLength() + 3 );
	aBuf.append( m_aRootAddonPopupMenuURLPrexfix );
	aBuf.append( ::rtl::OUString::valueOf( ++m_nRootAddonPopupMenuId ));
	aPopupMenuURL = aBuf.makeStringAndClear();
	return aPopupMenuURL;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************

sal_Bool AddonsOptions_Impl::ReadMenuMergeInstructions( MergeMenuInstructionContainer& aContainer )
{
    const ::rtl::OUString aMenuMergeRootName( RTL_CONSTASCII_USTRINGPARAM( "AddonUI/OfficeMenuBarMerging/" ));

	Sequence< ::rtl::OUString >	aAddonMergeNodesSeq = GetNodeNames( aMenuMergeRootName );
	::rtl::OUString                aAddonMergeNode( aMenuMergeRootName );

	sal_uInt32              nCount = aAddonMergeNodesSeq.getLength();

	// Init the property value sequence
	Sequence< ::rtl::OUString >	aNodePropNames( 5 );
	::rtl::OUString                aURL;

	for ( sal_uInt32 i = 0; i < nCount; i++ )
    {
		::rtl::OUString aMergeAddonInstructions( aAddonMergeNode + aAddonMergeNodesSeq[i] );

        Sequence< ::rtl::OUString > aAddonInstMergeNodesSeq = GetNodeNames( aMergeAddonInstructions );
        sal_uInt32           nCountAddons = aAddonInstMergeNodesSeq.getLength();

        for ( sal_uInt32 j = 0; j < nCountAddons; j++ )
	    {
		    ::rtl::OUStringBuffer aMergeAddonInstructionBase( aMergeAddonInstructions );
            aMergeAddonInstructionBase.append( m_aPathDelimiter );
            aMergeAddonInstructionBase.append( aAddonInstMergeNodesSeq[j] );
            aMergeAddonInstructionBase.append( m_aPathDelimiter );

            // Create sequence for data access
            ::rtl::OUStringBuffer aBuffer( aMergeAddonInstructionBase );
		    aBuffer.append( m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGEPOINT ] );
		    aNodePropNames[0] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGECOMMAND ] );
            aNodePropNames[1] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGECOMMANDPARAMETER ] );
            aNodePropNames[2] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGEFALLBACK ] );
            aNodePropNames[3] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGECONTEXT ] );
            aNodePropNames[4] = aBuffer.makeStringAndClear();

            Sequence< Any > aNodePropValues = GetProperties( aNodePropNames );

            MergeMenuInstruction aMergeMenuInstruction;
            aNodePropValues[0] >>= aMergeMenuInstruction.aMergePoint;
            aNodePropValues[1] >>= aMergeMenuInstruction.aMergeCommand;
            aNodePropValues[2] >>= aMergeMenuInstruction.aMergeCommandParameter;
            aNodePropValues[3] >>= aMergeMenuInstruction.aMergeFallback;
            aNodePropValues[4] >>= aMergeMenuInstruction.aMergeContext;

            ::rtl::OUString aMergeMenuBase = aMergeAddonInstructionBase.makeStringAndClear();
            ReadMergeMenuData( aMergeMenuBase, aMergeMenuInstruction.aMergeMenu );

            aContainer.push_back( aMergeMenuInstruction );
        }
    }

    return sal_True;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadMergeMenuData( const ::rtl::OUString& aMergeAddonInstructionBase, Sequence< Sequence< PropertyValue > >& rMergeMenu )
{
    ::rtl::OUString aMergeMenuBaseNode( aMergeAddonInstructionBase+m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MENUITEMS ] );

    Sequence< ::rtl::OUString > aSubMenuNodeNames = GetNodeNames( aMergeMenuBaseNode );
    aMergeMenuBaseNode += m_aPathDelimiter;

    // extend the node names to have full path strings
    for ( sal_uInt32 i = 0; i < (sal_uInt32)aSubMenuNodeNames.getLength(); i++ )
		aSubMenuNodeNames[i] = ::rtl::OUString( aMergeMenuBaseNode + aSubMenuNodeNames[i] );

    return ReadSubMenuEntries( aSubMenuNodeNames, rMergeMenu );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadToolbarMergeInstructions( ToolbarMergingInstructions& rCachedToolbarMergingInstructions )
{
    const ::rtl::OUString aToolbarMergeRootName( RTL_CONSTASCII_USTRINGPARAM( "AddonUI/OfficeToolbarMerging/" ));

	Sequence< ::rtl::OUString >	aAddonMergeNodesSeq = GetNodeNames( aToolbarMergeRootName );
	::rtl::OUString                aAddonMergeNode( aToolbarMergeRootName );

	sal_uInt32              nCount = aAddonMergeNodesSeq.getLength();

	// Init the property value sequence
	Sequence< ::rtl::OUString >	aNodePropNames( 6 );
	::rtl::OUString                aURL;

	for ( sal_uInt32 i = 0; i < nCount; i++ )
    {
		::rtl::OUString aMergeAddonInstructions( aAddonMergeNode + aAddonMergeNodesSeq[i] );

        Sequence< ::rtl::OUString > aAddonInstMergeNodesSeq = GetNodeNames( aMergeAddonInstructions );
        sal_uInt32           nCountAddons = aAddonInstMergeNodesSeq.getLength();

        for ( sal_uInt32 j = 0; j < nCountAddons; j++ )
	    {
		    ::rtl::OUStringBuffer aMergeAddonInstructionBase( aMergeAddonInstructions );
            aMergeAddonInstructionBase.append( m_aPathDelimiter );
            aMergeAddonInstructionBase.append( aAddonInstMergeNodesSeq[j] );
            aMergeAddonInstructionBase.append( m_aPathDelimiter );

            // Create sequence for data access
            ::rtl::OUStringBuffer aBuffer( aMergeAddonInstructionBase );
		    aBuffer.append( m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_TOOLBAR ] );
		    aNodePropNames[0] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
		    aBuffer.append( m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_MERGEPOINT ] );
		    aNodePropNames[1] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_MERGECOMMAND ] );
            aNodePropNames[2] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_MERGECOMMANDPARAMETER ] );
            aNodePropNames[3] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_MERGEFALLBACK ] );
            aNodePropNames[4] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_MERGECONTEXT ] );
            aNodePropNames[5] = aBuffer.makeStringAndClear();

            Sequence< Any > aNodePropValues = GetProperties( aNodePropNames );

            MergeToolbarInstruction aMergeToolbarInstruction;
            aNodePropValues[0] >>= aMergeToolbarInstruction.aMergeToolbar;
            aNodePropValues[1] >>= aMergeToolbarInstruction.aMergePoint;
            aNodePropValues[2] >>= aMergeToolbarInstruction.aMergeCommand;
            aNodePropValues[3] >>= aMergeToolbarInstruction.aMergeCommandParameter;
            aNodePropValues[4] >>= aMergeToolbarInstruction.aMergeFallback;
            aNodePropValues[5] >>= aMergeToolbarInstruction.aMergeContext;

            ReadMergeToolbarData( aMergeAddonInstructionBase.makeStringAndClear(),
                                  aMergeToolbarInstruction.aMergeToolbarItems );

            MergeToolbarInstructionContainer& rVector = rCachedToolbarMergingInstructions[ aMergeToolbarInstruction.aMergeToolbar ];
            rVector.push_back( aMergeToolbarInstruction );
        }
    }

    return sal_True;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadMergeToolbarData( const ::rtl::OUString& aMergeAddonInstructionBase, Sequence< Sequence< PropertyValue > >& rMergeToolbarItems )
{
    ::rtl::OUStringBuffer aBuffer( aMergeAddonInstructionBase );
    aBuffer.append( m_aPropMergeToolbarNames[ OFFSET_MERGETOOLBAR_TOOLBARITEMS ] );

    ::rtl::OUString aMergeToolbarBaseNode = aBuffer.makeStringAndClear();

    return ReadToolBarItemSet( aMergeToolbarBaseNode, rMergeToolbarItems );
}


sal_Bool AddonsOptions_Impl::ReadStatusbarMergeInstructions( MergeStatusbarInstructionContainer& aContainer )
{
    const ::rtl::OUString aStatusbarMergeRootName( RTL_CONSTASCII_USTRINGPARAM( "AddonUI/OfficeStatusbarMerging/" ));

    Sequence< ::rtl::OUString > aAddonMergeNodesSeq = GetNodeNames( aStatusbarMergeRootName );
    ::rtl::OUString aAddonMergeNode( aStatusbarMergeRootName );
    sal_uInt32  nCount = aAddonMergeNodesSeq.getLength();

    Sequence< ::rtl::OUString > aNodePropNames( 5 );
    ::rtl::OUString aURL;

    for ( sal_uInt32 i = 0; i < nCount; i++ )
    {
        ::rtl::OUString aMergeAddonInstructions( aAddonMergeNode + aAddonMergeNodesSeq[i] );

        Sequence< ::rtl::OUString > aAddonInstMergeNodesSeq = GetNodeNames( aMergeAddonInstructions );
        sal_uInt32 nCountAddons = aAddonInstMergeNodesSeq.getLength();

        for ( sal_uInt32 j = 0; j < nCountAddons; j++ )
        {
            ::rtl::OUStringBuffer aMergeAddonInstructionBase( aMergeAddonInstructions );
            aMergeAddonInstructionBase.append( m_aPathDelimiter );
            aMergeAddonInstructionBase.append( aAddonInstMergeNodesSeq[j] );
            aMergeAddonInstructionBase.append( m_aPathDelimiter );

            // Create sequence for data access
            ::rtl::OUStringBuffer aBuffer( aMergeAddonInstructionBase );
            aBuffer.append( m_aPropMergeMenuNames[ OFFSET_MERGESTATUSBAR_MERGEPOINT ] );
            aNodePropNames[0] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeMenuNames[ OFFSET_MERGESTATUSBAR_MERGECOMMAND ] );
            aNodePropNames[1] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeMenuNames[ OFFSET_MERGESTATUSBAR_MERGECOMMANDPARAMETER ] );
            aNodePropNames[2] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeMenuNames[ OFFSET_MERGESTATUSBAR_MERGEFALLBACK ] );
            aNodePropNames[3] = aBuffer.makeStringAndClear();

            aBuffer = aMergeAddonInstructionBase;
            aBuffer.append( m_aPropMergeMenuNames[ OFFSET_MERGESTATUSBAR_MERGECONTEXT ] );
            aNodePropNames[4] = aBuffer.makeStringAndClear();

            Sequence< Any > aNodePropValues = GetProperties( aNodePropNames );

            MergeStatusbarInstruction aMergeStatusbarInstruction;
            aNodePropValues[0] >>= aMergeStatusbarInstruction.aMergePoint;
            aNodePropValues[1] >>= aMergeStatusbarInstruction.aMergeCommand;
            aNodePropValues[2] >>= aMergeStatusbarInstruction.aMergeCommandParameter;
            aNodePropValues[3] >>= aMergeStatusbarInstruction.aMergeFallback;
            aNodePropValues[4] >>= aMergeStatusbarInstruction.aMergeContext;

            ReadMergeStatusbarData( aMergeAddonInstructionBase.makeStringAndClear(),
                                    aMergeStatusbarInstruction.aMergeStatusbarItems );

            aContainer.push_back( aMergeStatusbarInstruction );
        }
    }

    return sal_True;
}

sal_Bool AddonsOptions_Impl::ReadMergeStatusbarData(
    const ::rtl::OUString& aMergeAddonInstructionBase,
    Sequence< Sequence< PropertyValue > >& rMergeStatusbarItems )
{
    sal_uInt32 nStatusbarItemCount = rMergeStatusbarItems.getLength();

    ::rtl::OUStringBuffer aBuffer( aMergeAddonInstructionBase );
    aBuffer.append( m_aPropMergeStatusbarNames[ OFFSET_MERGESTATUSBAR_STATUSBARITEMS ] );
    ::rtl::OUString aMergeStatusbarBaseNode = aBuffer.makeStringAndClear();

    ::rtl::OUString aAddonStatusbarItemSetNode( aMergeStatusbarBaseNode + m_aPathDelimiter );
    Sequence< ::rtl::OUString > aAddonStatusbarItemSetNodeSeq = GetNodeNames( aMergeStatusbarBaseNode );

    Sequence< PropertyValue > aStatusbarItem( PROPERTYCOUNT_STATUSBARITEM );
    aStatusbarItem[ OFFSET_STATUSBARITEM_URL       ].Name = m_aPropNames[ INDEX_URL       ];
    aStatusbarItem[ OFFSET_STATUSBARITEM_TITLE     ].Name = m_aPropNames[ INDEX_TITLE     ];
    aStatusbarItem[ OFFSET_STATUSBARITEM_CONTEXT   ].Name = m_aPropNames[ INDEX_CONTEXT   ];
    aStatusbarItem[ OFFSET_STATUSBARITEM_ALIGN     ].Name = m_aPropNames[ INDEX_ALIGN     ];
    aStatusbarItem[ OFFSET_STATUSBARITEM_AUTOSIZE  ].Name = m_aPropNames[ INDEX_AUTOSIZE  ];
    aStatusbarItem[ OFFSET_STATUSBARITEM_OWNERDRAW ].Name = m_aPropNames[ INDEX_OWNERDRAW ];
    aStatusbarItem[ OFFSET_STATUSBARITEM_WIDTH     ].Name = m_aPropNames[ INDEX_WIDTH     ];

    sal_uInt32 nCount = aAddonStatusbarItemSetNodeSeq.getLength();
    for ( sal_uInt32 n = 0; n < nCount; n++ )
    {
        ::rtl::OUString aStatusbarItemNode( aAddonStatusbarItemSetNode + aAddonStatusbarItemSetNodeSeq[n] );

        if ( ReadStatusBarItem( aStatusbarItemNode, aStatusbarItem ) )
        {
            sal_uInt32 nAddonCount = rMergeStatusbarItems.getLength();
            rMergeStatusbarItems.realloc( nAddonCount+1 );
            rMergeStatusbarItems[nAddonCount] = aStatusbarItem;
        }
    }

    return ( (sal_uInt32)rMergeStatusbarItems.getLength() > nStatusbarItemCount );
}

sal_Bool AddonsOptions_Impl::ReadStatusBarItem(
    const ::rtl::OUString& aStatusarItemNodeName,
    Sequence< PropertyValue >& aStatusbarItem )
{
    sal_Bool bResult( sal_False );
    ::rtl::OUString aURL;
    ::rtl::OUString aAddonStatusbarItemTreeNode( aStatusarItemNodeName + m_aPathDelimiter );
    Sequence< Any > aStatusbarItemNodePropValues;

    aStatusbarItemNodePropValues = GetProperties( GetPropertyNamesStatusbarItem( aAddonStatusbarItemTreeNode ) );

    // Command URL is required
    if (( aStatusbarItemNodePropValues[ OFFSET_STATUSBARITEM_URL ] >>= aURL ) && aURL.getLength() > 0 )
    {
        aStatusbarItem[ OFFSET_STATUSBARITEM_URL        ].Value <<= aURL;
        aStatusbarItem[ OFFSET_STATUSBARITEM_TITLE      ].Value <<= aStatusbarItemNodePropValues[ OFFSET_STATUSBARITEM_TITLE     ];
        aStatusbarItem[ OFFSET_STATUSBARITEM_CONTEXT    ].Value <<= aStatusbarItemNodePropValues[ OFFSET_STATUSBARITEM_CONTEXT   ];
        aStatusbarItem[ OFFSET_STATUSBARITEM_ALIGN      ].Value <<= aStatusbarItemNodePropValues[ OFFSET_STATUSBARITEM_ALIGN     ];
        aStatusbarItem[ OFFSET_STATUSBARITEM_AUTOSIZE   ].Value <<= aStatusbarItemNodePropValues[ OFFSET_STATUSBARITEM_AUTOSIZE  ];;
        aStatusbarItem[ OFFSET_STATUSBARITEM_OWNERDRAW  ].Value <<= aStatusbarItemNodePropValues[ OFFSET_STATUSBARITEM_OWNERDRAW ];

        // Configuration uses hyper for long. Therefore transform into sal_Int32
        sal_Int64 nValue( 0 );
        aStatusbarItemNodePropValues[ OFFSET_STATUSBARITEM_WIDTH ] >>= nValue;
        aStatusbarItem[ OFFSET_STATUSBARITEM_WIDTH ].Value <<= sal_Int32( nValue );

        bResult = sal_True;
    }

    return bResult;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadMenuItem( const ::rtl::OUString& aMenuNodeName, Sequence< PropertyValue >& aMenuItem, sal_Bool bIgnoreSubMenu )
{
	sal_Bool			bResult = sal_False;
	::rtl::OUString			aStrValue;
	::rtl::OUString			aAddonMenuItemTreeNode( aMenuNodeName + m_aPathDelimiter );
	Sequence< Any >		aMenuItemNodePropValues;

	aMenuItemNodePropValues = GetProperties( GetPropertyNamesMenuItem( aAddonMenuItemTreeNode ) );
	if (( aMenuItemNodePropValues[ OFFSET_MENUITEM_TITLE ] >>= aStrValue ) && aStrValue.getLength() > 0 )
	{
		aMenuItem[ OFFSET_MENUITEM_TITLE ].Value <<= aStrValue;

		::rtl::OUString aRootSubMenuName( aAddonMenuItemTreeNode + m_aPropNames[ INDEX_SUBMENU ] );
		Sequence< ::rtl::OUString > aRootSubMenuNodeNames = GetNodeNames( aRootSubMenuName );
		if ( aRootSubMenuNodeNames.getLength() > 0 && !bIgnoreSubMenu )
		{
			// Set a unique prefixed Add-On popup menu URL so it can be identified later
			::rtl::OUString aPopupMenuURL     = GeneratePrefixURL();
            ::rtl::OUString aPopupMenuImageId;

            aMenuItemNodePropValues[ OFFSET_MENUITEM_IMAGEIDENTIFIER ] >>= aPopupMenuImageId;
            ReadAndAssociateImages( aPopupMenuURL, aPopupMenuImageId );

			// A popup menu must have a title and can have a URL and ImageIdentifier
			// Set the other property values to empty
			aMenuItem[ OFFSET_MENUITEM_URL				].Value <<= aPopupMenuURL;
			aMenuItem[ OFFSET_MENUITEM_TARGET			].Value <<= m_aEmpty;
			aMenuItem[ OFFSET_MENUITEM_IMAGEIDENTIFIER	].Value <<= aPopupMenuImageId;
			aMenuItem[ OFFSET_MENUITEM_CONTEXT			].Value <<= aMenuItemNodePropValues[ OFFSET_MENUITEM_CONTEXT ];

			// Continue to read the sub menu nodes
			Sequence< Sequence< PropertyValue > > aSubMenuSeq;
			::rtl::OUString aSubMenuRootNodeName( aRootSubMenuName + m_aPathDelimiter );
			for ( sal_uInt32 n = 0; n < (sal_uInt32)aRootSubMenuNodeNames.getLength(); n++ )
				aRootSubMenuNodeNames[n] = ::rtl::OUString( aSubMenuRootNodeName + aRootSubMenuNodeNames[n] );
			ReadSubMenuEntries( aRootSubMenuNodeNames, aSubMenuSeq );
			aMenuItem[ OFFSET_MENUITEM_SUBMENU ].Value <<= aSubMenuSeq;
			bResult = sal_True;
		}
		else if (( aMenuItemNodePropValues[ OFFSET_MENUITEM_URL ] >>= aStrValue ) && aStrValue.getLength() > 0 )
		{
			// A simple menu item => read the other properties;
			::rtl::OUString aMenuImageId;

			aMenuItemNodePropValues[ OFFSET_MENUITEM_IMAGEIDENTIFIER ] >>= aMenuImageId;
 			ReadAndAssociateImages( aStrValue, aMenuImageId );

			aMenuItem[ OFFSET_MENUITEM_URL				].Value <<= aStrValue;
			aMenuItem[ OFFSET_MENUITEM_TARGET			].Value <<= aMenuItemNodePropValues[ OFFSET_MENUITEM_TARGET			];
			aMenuItem[ OFFSET_MENUITEM_IMAGEIDENTIFIER	].Value <<= aMenuImageId;
		    aMenuItem[ OFFSET_MENUITEM_CONTEXT			].Value <<= aMenuItemNodePropValues[ OFFSET_MENUITEM_CONTEXT        ];
			aMenuItem[ OFFSET_MENUITEM_SUBMENU			].Value <<= Sequence< Sequence< PropertyValue > >(); // Submenu set!

			bResult = sal_True;
		}
	}
	else if (( aMenuItemNodePropValues[ OFFSET_MENUITEM_URL ] >>= aStrValue ) &&
			  aStrValue.equalsAsciiL( SEPARATOR_URL_STR, SEPARATOR_URL_LEN ))
	{
		// Separator
		aMenuItem[ OFFSET_MENUITEM_URL				].Value <<= aStrValue;
		aMenuItem[ OFFSET_MENUITEM_TARGET			].Value <<= m_aEmpty;
		aMenuItem[ OFFSET_MENUITEM_IMAGEIDENTIFIER	].Value <<= m_aEmpty;
		aMenuItem[ OFFSET_MENUITEM_CONTEXT			].Value <<= m_aEmpty;
		aMenuItem[ OFFSET_MENUITEM_SUBMENU			].Value <<= Sequence< Sequence< PropertyValue > >(); // Submenu set!
		bResult = sal_True;
	}

	return bResult;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadPopupMenu( const ::rtl::OUString& aPopupMenuNodeName, Sequence< PropertyValue >& aPopupMenu )
{
	sal_Bool			bResult = sal_False;
	::rtl::OUString			aStrValue;
	::rtl::OUString			aAddonPopupMenuTreeNode( aPopupMenuNodeName + m_aPathDelimiter );
	Sequence< Any >		aPopupMenuNodePropValues;

	aPopupMenuNodePropValues = GetProperties( GetPropertyNamesPopupMenu( aAddonPopupMenuTreeNode ) );
	if (( aPopupMenuNodePropValues[ OFFSET_POPUPMENU_TITLE ] >>= aStrValue ) &&
		 aStrValue.getLength() > 0 )
	{
		aPopupMenu[ OFFSET_POPUPMENU_TITLE ].Value <<= aStrValue;

		::rtl::OUString aRootSubMenuName( aAddonPopupMenuTreeNode + m_aPropNames[ INDEX_SUBMENU ] );
		Sequence< ::rtl::OUString > aRootSubMenuNodeNames = GetNodeNames( aRootSubMenuName );
		if ( aRootSubMenuNodeNames.getLength() > 0 )
		{
			// A top-level popup menu needs a title
			// Set a unique prefixed Add-On popup menu URL so it can be identified later
			::rtl::OUString aPopupMenuURL = GeneratePrefixURL();

            aPopupMenu[ OFFSET_POPUPMENU_URL		].Value <<= aPopupMenuURL;
			aPopupMenu[ OFFSET_POPUPMENU_CONTEXT	].Value <<= aPopupMenuNodePropValues[ OFFSET_POPUPMENU_CONTEXT ];

			// Continue to read the sub menu nodes
			Sequence< Sequence< PropertyValue > > aSubMenuSeq;
			::rtl::OUString aSubMenuRootNodeName( aRootSubMenuName + m_aPathDelimiter );
			for ( sal_uInt32 n = 0; n < (sal_uInt32)aRootSubMenuNodeNames.getLength(); n++ )
				aRootSubMenuNodeNames[n] = ::rtl::OUString( aSubMenuRootNodeName + aRootSubMenuNodeNames[n] );
			ReadSubMenuEntries( aRootSubMenuNodeNames, aSubMenuSeq );
			aPopupMenu[ OFFSET_POPUPMENU_SUBMENU ].Value <<= aSubMenuSeq;
			bResult = sal_True;
		}
	}

	return bResult;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::AppendPopupMenu( Sequence< PropertyValue >& rTargetPopupMenu, const Sequence< PropertyValue >& rSourcePopupMenu )
{
    Sequence< Sequence< PropertyValue > > aTargetSubMenuSeq;
    Sequence< Sequence< PropertyValue > > aSourceSubMenuSeq;

    if (( rTargetPopupMenu[ OFFSET_POPUPMENU_SUBMENU ].Value >>= aTargetSubMenuSeq ) &&
        ( rSourcePopupMenu[ OFFSET_POPUPMENU_SUBMENU ].Value >>= aSourceSubMenuSeq ))
    {
        sal_uInt32 nIndex = aTargetSubMenuSeq.getLength();
        aTargetSubMenuSeq.realloc( nIndex + aSourceSubMenuSeq.getLength() );
        for ( sal_uInt32 i = 0; i < sal_uInt32( aSourceSubMenuSeq.getLength() ); i++ )
            aTargetSubMenuSeq[nIndex++] = aSourceSubMenuSeq[i];
        rTargetPopupMenu[ OFFSET_POPUPMENU_SUBMENU ].Value <<= aTargetSubMenuSeq;
    }

    return sal_True;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadToolBarItem( const ::rtl::OUString& aToolBarItemNodeName, Sequence< PropertyValue >& aToolBarItem )
{
	sal_Bool			bResult = sal_False;
	::rtl::OUString			aTitle;
	::rtl::OUString			aURL;
	::rtl::OUString			aAddonToolBarItemTreeNode( aToolBarItemNodeName + m_aPathDelimiter );
	Sequence< Any >		aToolBarItemNodePropValues;

	aToolBarItemNodePropValues = GetProperties( GetPropertyNamesToolBarItem( aAddonToolBarItemTreeNode ) );

	// A toolbar item must have a command URL
	if (( aToolBarItemNodePropValues[ OFFSET_TOOLBARITEM_URL ] >>= aURL ) && aURL.getLength() > 0 )
	{
		if ( aURL.equals( SEPARATOR_URL ))
		{
			// A speparator toolbar item only needs a URL
			aToolBarItem[ OFFSET_TOOLBARITEM_URL				].Value <<= aURL;
			aToolBarItem[ OFFSET_TOOLBARITEM_TITLE				].Value <<= m_aEmpty;
			aToolBarItem[ OFFSET_TOOLBARITEM_TARGET				].Value <<= m_aEmpty;
			aToolBarItem[ OFFSET_TOOLBARITEM_IMAGEIDENTIFIER	].Value <<= m_aEmpty;
			aToolBarItem[ OFFSET_TOOLBARITEM_CONTEXT			].Value <<= m_aEmpty;
			aToolBarItem[ OFFSET_TOOLBARITEM_CONTROLTYPE		].Value <<= m_aEmpty;
			aToolBarItem[ OFFSET_TOOLBARITEM_WIDTH		        ].Value <<= sal_Int32( 0 );

			bResult = sal_True;
		}
		else if (( aToolBarItemNodePropValues[ OFFSET_TOOLBARITEM_TITLE	] >>= aTitle ) && aTitle.getLength() > 0 )
		{
			// A normal toolbar item must also have title => read the other properties;
			::rtl::OUString aImageId;

            // Try to map a user-defined image URL to our internal private image URL
			aToolBarItemNodePropValues[ OFFSET_TOOLBARITEM_IMAGEIDENTIFIER ] >>= aImageId;
 			ReadAndAssociateImages( aURL, aImageId );

			aToolBarItem[ OFFSET_TOOLBARITEM_URL				].Value <<= aURL;
			aToolBarItem[ OFFSET_TOOLBARITEM_TITLE				].Value <<= aTitle;
			aToolBarItem[ OFFSET_TOOLBARITEM_TARGET				].Value <<= aToolBarItemNodePropValues[ OFFSET_TOOLBARITEM_TARGET	   ];
			aToolBarItem[ OFFSET_TOOLBARITEM_IMAGEIDENTIFIER	].Value <<= aImageId;
			aToolBarItem[ OFFSET_TOOLBARITEM_CONTEXT			].Value <<= aToolBarItemNodePropValues[ OFFSET_TOOLBARITEM_CONTEXT	   ];
			aToolBarItem[ OFFSET_TOOLBARITEM_CONTROLTYPE		].Value <<= aToolBarItemNodePropValues[ OFFSET_TOOLBARITEM_CONTROLTYPE ];

            // Configuration uses hyper for long. Therefore transform into sal_Int32
            sal_Int64 nValue( 0 );
            aToolBarItemNodePropValues[ OFFSET_TOOLBARITEM_WIDTH ] >>= nValue;
            aToolBarItem[ OFFSET_TOOLBARITEM_WIDTH		        ].Value <<= sal_Int32( nValue );

			bResult = sal_True;
		}
	}

	return bResult;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::ReadSubMenuEntries( const Sequence< ::rtl::OUString >& aSubMenuNodeNames, Sequence< Sequence< PropertyValue > >& rSubMenuSeq )
{
	Sequence< PropertyValue > aMenuItem( PROPERTYCOUNT_MENUITEM );

	// Init the property value sequence
	aMenuItem[ OFFSET_MENUITEM_URL				].Name = PROPERTYNAME_URL;
	aMenuItem[ OFFSET_MENUITEM_TITLE			].Name = PROPERTYNAME_TITLE;
	aMenuItem[ OFFSET_MENUITEM_TARGET			].Name = PROPERTYNAME_TARGET;
	aMenuItem[ OFFSET_MENUITEM_IMAGEIDENTIFIER	].Name = PROPERTYNAME_IMAGEIDENTIFIER;
	aMenuItem[ OFFSET_MENUITEM_CONTEXT			].Name = PROPERTYNAME_CONTEXT;
	aMenuItem[ OFFSET_MENUITEM_SUBMENU			].Name = PROPERTYNAME_SUBMENU;	// Submenu set!

	sal_uInt32 nIndex = 0;
	sal_uInt32 nCount = aSubMenuNodeNames.getLength();
	for ( sal_uInt32 n = 0; n < nCount; n++ )
	{
		if ( ReadMenuItem( aSubMenuNodeNames[n], aMenuItem ))
		{
			sal_uInt32 nSubMenuCount = rSubMenuSeq.getLength() + 1;
			rSubMenuSeq.realloc( nSubMenuCount );
			rSubMenuSeq[nIndex++] = aMenuItem;
		}
	}

	return sal_True;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::HasAssociatedImages( const ::rtl::OUString& aURL )
{
	ImageManager::const_iterator pIter = m_aImageManager.find( aURL );
	return ( pIter != m_aImageManager.end() );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
void AddonsOptions_Impl::SubstituteVariables( ::rtl::OUString& aURL )
{
    if (( aURL.compareToAscii( RTL_CONSTASCII_STRINGPARAM( EXPAND_PROTOCOL )) == 0 ) &&
        m_xMacroExpander.is() )
    {
        // cut protocol
        ::rtl::OUString macro( aURL.copy( sizeof ( EXPAND_PROTOCOL ) -1 ) );
        // decode uric class chars
        macro = ::rtl::Uri::decode(
            macro, rtl_UriDecodeWithCharset, RTL_TEXTENCODING_UTF8 );
        // expand macro string
        aURL = m_xMacroExpander->expandMacros( macro );
    }
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
void AddonsOptions_Impl::ReadImageFromURL( ImageSize nImageSize, const ::rtl::OUString& aImageURL, Image& aImage, Image& aImageNoScale )
{
    SvStream* pStream = UcbStreamHelper::CreateStream( aImageURL, STREAM_STD_READ );
    if ( pStream && ( pStream->GetErrorCode() == 0 ))
    {
        // Use graphic class to also support more graphic formats (bmp,png,...)
        Graphic aGraphic;

        GraphicFilter* pGF = GraphicFilter::GetGraphicFilter();
        pGF->ImportGraphic( aGraphic, String(), *pStream, GRFILTER_FORMAT_DONTKNOW );

        BitmapEx aBitmapEx = aGraphic.GetBitmapEx();

	    const Size aSize = ( nImageSize == IMGSIZE_SMALL ) ? aImageSizeSmall : aImageSizeBig; // Sizes used for menu/toolbox images

        Size aBmpSize = aBitmapEx.GetSizePixel();
	    if ( aBmpSize.Width() > 0 && aBmpSize.Height() > 0 )
        {
            // Support non-transparent bitmaps to be downward compatible with OOo 1.1.x addons
            if( !aBitmapEx.IsTransparent() )
                aBitmapEx = BitmapEx( aBitmapEx.GetBitmap(), COL_LIGHTMAGENTA );

            // A non-scaled bitmap can have a flexible width, but must have a defined height!
            Size aNoScaleSize( aBmpSize.Width(), aSize.Height() );
            if ( aBmpSize != aNoScaleSize )
            {
                BitmapEx aNoScaleBmp( aBitmapEx );
                aNoScaleBmp.Scale( aNoScaleSize, BMP_SCALE_INTERPOLATE );
            }
            else
                aImageNoScale = Image( aBitmapEx );

            if ( aBmpSize != aSize )
                aBitmapEx.Scale( aSize, BMP_SCALE_INTERPOLATE );

            aImage = Image( aBitmapEx );
        }
    }

    delete pStream;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
void AddonsOptions_Impl::ReadAndAssociateImages( const ::rtl::OUString& aURL, const ::rtl::OUString& aImageId )
{
    const int   MAX_NUM_IMAGES = 4;
    const char* aExtArray[MAX_NUM_IMAGES] = { "_16", "_26", "_16h", "_26h" };
    const char* pBmpExt = ".bmp";

    if ( aImageId.getLength() == 0 )
        return;

    bool        bImageFound = true;
    ImageEntry  aImageEntry;
    ::rtl::OUString    aImageURL( aImageId );

    SubstituteVariables( aImageURL );

    // Loop to create the four possible image names and try to read the bitmap files
    for ( int i = 0; i < MAX_NUM_IMAGES; i++ )
    {
        ::rtl::OUStringBuffer aFileURL( aImageURL );
        aFileURL.appendAscii( aExtArray[i] );
        aFileURL.appendAscii( pBmpExt );

        Image aImage;
        Image aImageNoScale;
        ReadImageFromURL( ((i==0)||(i==2)) ? IMGSIZE_SMALL : IMGSIZE_BIG, aFileURL.makeStringAndClear(), aImage, aImageNoScale );
        if ( !!aImage )
        {
            bImageFound = true;
            switch ( i )
            {
                case 0:
                    aImageEntry.aImageSmall          = aImage;
                    aImageEntry.aImageSmallNoScale   = aImageNoScale;
                    break;
                case 1:
                    aImageEntry.aImageBig            = aImage;
                    aImageEntry.aImageBigNoScale     = aImageNoScale;
                    break;
                case 2:
                    aImageEntry.aImageSmallHC        = aImage;
                    aImageEntry.aImageSmallHCNoScale = aImageNoScale;
                    break;
                case 3:
                    aImageEntry.aImageBigHC          = aImage;
                    aImageEntry.aImageBigHCNoScale   = aImageNoScale;
                    break;
            }
        }
    }

    if ( bImageFound )
        m_aImageManager.insert( ImageManager::value_type( aURL, aImageEntry ));
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
AddonsOptions_Impl::ImageEntry* AddonsOptions_Impl::ReadImageData( const ::rtl::OUString& aImagesNodeName )
{
	Sequence< ::rtl::OUString > aImageDataNodeNames = GetPropertyNamesImages( aImagesNodeName );
	Sequence< Any >		 aPropertyData;
	Sequence< sal_Int8 > aImageDataSeq;
	::rtl::OUString             aImageURL;

	ImageEntry* pEntry = NULL;

    // It is possible to use both forms (embedded image data and URLs to external bitmap files) at the
    // same time. Embedded image data has a higher priority.
	aPropertyData = GetProperties( aImageDataNodeNames );
	for ( int i = 0; i < PROPERTYCOUNT_IMAGES; i++ )
	{
        if ( i < PROPERTYCOUNT_EMBEDDED_IMAGES )
        {
            // Extract image data from the embedded hex binary sequence
            Image aImage;
		    if (( aPropertyData[i] >>= aImageDataSeq ) &&
		        aImageDataSeq.getLength() > 0 &&
			    ( CreateImageFromSequence( aImage,
									    (( i == OFFSET_IMAGES_BIG ) ||
									    ( i == OFFSET_IMAGES_BIGHC )),
									    aImageDataSeq )) )
		    {
			    if ( !pEntry )
				    pEntry = new ImageEntry;

			    if ( i == OFFSET_IMAGES_SMALL )
				    pEntry->aImageSmall = aImage;
			    else if ( i == OFFSET_IMAGES_BIG )
				    pEntry->aImageBig = aImage;
			    else if ( i == OFFSET_IMAGES_SMALLHC )
				    pEntry->aImageSmallHC = aImage;
			    else
				    pEntry->aImageBigHC = aImage;
		    }
        }
        else
        {
            // Retrieve image data from a external bitmap file. Make sure that embedded image data
            // has a higher priority.
            aPropertyData[i] >>= aImageURL;

            if ( aImageURL.getLength() > 0 )
            {
                Image aImage;
                Image aImageNoScale;

                SubstituteVariables( aImageURL );
                ReadImageFromURL( ((i==OFFSET_IMAGES_SMALL_URL)||(i==OFFSET_IMAGES_SMALLHC_URL)) ? IMGSIZE_SMALL : IMGSIZE_BIG,
                                  aImageURL, aImage, aImageNoScale );
                if ( !!aImage )
                {
                    if ( !pEntry )
                        pEntry = new ImageEntry;

                    if ( i == OFFSET_IMAGES_SMALL_URL && !pEntry->aImageSmall )
                    {
                        pEntry->aImageSmall = aImage;
                        pEntry->aImageSmallNoScale = aImageNoScale;
                    }
                    else if ( i == OFFSET_IMAGES_BIG_URL && !pEntry->aImageBig )
                    {
                        pEntry->aImageBig = aImage;
                        pEntry->aImageBigNoScale = aImageNoScale;
                    }
			        else if ( i == OFFSET_IMAGES_SMALLHC_URL && !pEntry->aImageSmallHC )
                    {
				        pEntry->aImageSmallHC = aImage;
				        pEntry->aImageSmallHCNoScale = aImageNoScale;
                    }
			        else if ( !pEntry->aImageBigHC )
                    {
				        pEntry->aImageBigHC = aImage;
				        pEntry->aImageBigHCNoScale = aImageNoScale;
                    }
                }
            }
        }
	}

	return pEntry;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
sal_Bool AddonsOptions_Impl::CreateImageFromSequence( Image& rImage, sal_Bool bBig, Sequence< sal_Int8 >& rBitmapDataSeq ) const
{
	sal_Bool	bResult = sal_False;
	Size		aSize = bBig ? aImageSizeBig : aImageSizeSmall; // Sizes used for menu/toolbox images

	if ( rBitmapDataSeq.getLength() > 0 )
	{
		SvMemoryStream	aMemStream( rBitmapDataSeq.getArray(), rBitmapDataSeq.getLength(), STREAM_STD_READ );
		BitmapEx 		aBitmapEx;

        ReadDIBBitmapEx(aBitmapEx, aMemStream);

		// Scale bitmap to fit the correct size for the menu/toolbar. Use best quality
		if ( aBitmapEx.GetSizePixel() != aSize )
			aBitmapEx.Scale( aSize, BMP_SCALE_INTERPOLATE );

        if( !aBitmapEx.IsTransparent() )
        {
            // Support non-transparent bitmaps to be downward compatible with OOo 1.1.x addons
            aBitmapEx = BitmapEx( aBitmapEx.GetBitmap(), COL_LIGHTMAGENTA );
        }

		rImage = Image( aBitmapEx );
		bResult = sal_True;
	}

	return bResult;
}

//*****************************************************************************************************************
//	private methods
//*****************************************************************************************************************
Sequence< ::rtl::OUString > AddonsOptions_Impl::GetPropertyNamesMergeMenuInstruction( const ::rtl::OUString& aPropertyRootNode ) const
{
	Sequence< ::rtl::OUString > lResult( PROPERTYCOUNT_MERGE_MENUBAR );

    // Create property names dependent from the root node name
    lResult[ OFFSET_MERGEMENU_MERGEPOINT            ] = ::rtl::OUString( aPropertyRootNode + m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGEPOINT    ] );
    lResult[ OFFSET_MERGEMENU_MERGECOMMAND          ] = ::rtl::OUString( aPropertyRootNode + m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGECOMMAND  ] );
    lResult[ OFFSET_MERGEMENU_MERGECOMMANDPARAMETER ] = ::rtl::OUString( aPropertyRootNode + m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGECOMMANDPARAMETER ] );
    lResult[ OFFSET_MERGEMENU_MERGEFALLBACK         ] = ::rtl::OUString( aPropertyRootNode + m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGEFALLBACK ] );
    lResult[ OFFSET_MERGEMENU_MERGECONTEXT          ] = ::rtl::OUString( aPropertyRootNode + m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MERGECONTEXT  ] );
    lResult[ OFFSET_MERGEMENU_MENUITEMS             ] = ::rtl::OUString( aPropertyRootNode + m_aPropMergeMenuNames[ OFFSET_MERGEMENU_MENUITEMS     ] );

	return lResult;
}

Sequence< ::rtl::OUString > AddonsOptions_Impl::GetPropertyNamesMenuItem( const ::rtl::OUString& aPropertyRootNode ) const
{
	Sequence< ::rtl::OUString > lResult( PROPERTYCOUNT_MENUITEM );

    // Create property names dependent from the root node name
	lResult[OFFSET_MENUITEM_URL]             = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_URL			 ] );
	lResult[OFFSET_MENUITEM_TITLE]           = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_TITLE			 ] );
	lResult[OFFSET_MENUITEM_IMAGEIDENTIFIER] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_IMAGEIDENTIFIER ] );
	lResult[OFFSET_MENUITEM_TARGET]          = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_TARGET			 ] );
	lResult[OFFSET_MENUITEM_CONTEXT]         = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_CONTEXT	     ] );
	lResult[OFFSET_MENUITEM_SUBMENU]         = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_SUBMENU	     ] );

	return lResult;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Sequence< ::rtl::OUString > AddonsOptions_Impl::GetPropertyNamesPopupMenu( const ::rtl::OUString& aPropertyRootNode ) const
{
	// The URL is automatically set and not read from the configuration.
	Sequence< ::rtl::OUString > lResult( PROPERTYCOUNT_POPUPMENU-1 );

	// Create property names dependent from the root node name
	lResult[OFFSET_POPUPMENU_TITLE]   = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_TITLE	] );
	lResult[OFFSET_POPUPMENU_CONTEXT] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_CONTEXT	] );
	lResult[OFFSET_POPUPMENU_SUBMENU] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_SUBMENU	] );

	return lResult;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Sequence< ::rtl::OUString > AddonsOptions_Impl::GetPropertyNamesToolBar( const ::rtl::OUString& aPropertyRootNode ) const
{
    Sequence< ::rtl::OUString > lResult( PROPERTYCOUNT_TOOLBAR );

    // Create property names dependent from the root node name
    lResult[OFFSET_TOOLBAR_TITLE]        = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_TITLE ] );
    lResult[OFFSET_TOOLBAR_TOOLBARITEMS] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_TOOLBARITEMS ] );

    return lResult;
}

Sequence< ::rtl::OUString > AddonsOptions_Impl::GetPropertyNamesToolBarItem( const ::rtl::OUString& aPropertyRootNode ) const
{
	Sequence< ::rtl::OUString > lResult( PROPERTYCOUNT_TOOLBARITEM );

	// Create property names dependent from the root node name
	lResult[0] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_URL			  ] );
	lResult[1] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_TITLE		  ] );
	lResult[2] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_IMAGEIDENTIFIER] );
	lResult[3] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_TARGET		  ] );
	lResult[4] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_CONTEXT		  ] );
	lResult[5] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_CONTROLTYPE	  ] );
	lResult[6] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_WIDTH		  ] );

	return lResult;
}

Sequence< ::rtl::OUString > AddonsOptions_Impl::GetPropertyNamesStatusbarItem(
    const ::rtl::OUString& aPropertyRootNode ) const
{
    Sequence< ::rtl::OUString > lResult( PROPERTYCOUNT_STATUSBARITEM );

    lResult[0] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_URL       ] );
    lResult[1] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_TITLE     ] );
    lResult[2] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_CONTEXT   ] );
    lResult[3] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_ALIGN     ] );
    lResult[4] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_AUTOSIZE  ] );
    lResult[5] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_OWNERDRAW ] );
    lResult[6] = ::rtl::OUString( aPropertyRootNode + m_aPropNames[ INDEX_WIDTH     ] );

    return lResult;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Sequence< ::rtl::OUString > AddonsOptions_Impl::GetPropertyNamesImages( const ::rtl::OUString& aPropertyRootNode ) const
{
	Sequence< ::rtl::OUString > lResult( PROPERTYCOUNT_IMAGES );

	// Create property names dependent from the root node name
	lResult[0] = ::rtl::OUString( aPropertyRootNode + m_aPropImagesNames[ OFFSET_IMAGES_SMALL		] );
	lResult[1] = ::rtl::OUString( aPropertyRootNode + m_aPropImagesNames[ OFFSET_IMAGES_BIG		] );
	lResult[2] = ::rtl::OUString( aPropertyRootNode + m_aPropImagesNames[ OFFSET_IMAGES_SMALLHC	] );
	lResult[3] = ::rtl::OUString( aPropertyRootNode + m_aPropImagesNames[ OFFSET_IMAGES_BIGHC		] );
	lResult[4] = ::rtl::OUString( aPropertyRootNode + m_aPropImagesNames[ OFFSET_IMAGES_SMALL_URL  ] );
	lResult[5] = ::rtl::OUString( aPropertyRootNode + m_aPropImagesNames[ OFFSET_IMAGES_BIG_URL	] );
	lResult[6] = ::rtl::OUString( aPropertyRootNode + m_aPropImagesNames[ OFFSET_IMAGES_SMALLHC_URL] );
	lResult[7] = ::rtl::OUString( aPropertyRootNode + m_aPropImagesNames[ OFFSET_IMAGES_BIGHC_URL	] );

	return lResult;
}

//*****************************************************************************************************************
//	initialize static member
//	DON'T DO IT IN YOUR HEADER!
//	see definition for further informations
//*****************************************************************************************************************
AddonsOptions_Impl*		AddonsOptions::m_pDataContainer = NULL  ;
sal_Int32				AddonsOptions::m_nRefCount      = 0     ;

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
AddonsOptions::AddonsOptions()
{
    // Global access, must be guarded (multithreading!).
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Increase our refcount ...
	++m_nRefCount;
	// ... and initialize our data container only if it not already exist!
    if( m_pDataContainer == NULL )
	{
        m_pDataContainer = new AddonsOptions_Impl;
	}
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
AddonsOptions::~AddonsOptions()
{
    // Global access, must be guarded (multithreading!)
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Decrease our refcount.
	--m_nRefCount;
	// If last instance was deleted ...
	// we must destroy our static data container!
    if( m_nRefCount <= 0 )
	{
		delete m_pDataContainer;
		m_pDataContainer = NULL;
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool AddonsOptions::HasAddonsMenu() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->HasAddonsMenu();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************

sal_Bool AddonsOptions::HasAddonsHelpMenu() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->HasAddonsHelpMenu();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************

sal_Int32 AddonsOptions::GetAddonsToolBarCount() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetAddonsToolBarCount();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const Sequence< Sequence< PropertyValue > >& AddonsOptions::GetAddonsMenu() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetAddonsMenu();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const Sequence< Sequence< PropertyValue > >& AddonsOptions::GetAddonsMenuBarPart() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetAddonsMenuBarPart();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const Sequence< Sequence< PropertyValue > >& AddonsOptions::GetAddonsToolBarPart( sal_uInt32 nIndex ) const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetAddonsToolBarPart( nIndex );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const ::rtl::OUString AddonsOptions::GetAddonsToolbarResourceName( sal_uInt32 nIndex ) const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetAddonsToolbarResourceName( nIndex );
}

const ::rtl::OUString AddonsOptions::GetAddonsToolbarUIName( sal_uInt32 nIndex ) const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetAddonsToolbarUIName( nIndex );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const Sequence< Sequence< PropertyValue > >& AddonsOptions::GetAddonsHelpMenu() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetAddonsHelpMenu();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
const MergeMenuInstructionContainer& AddonsOptions::GetMergeMenuInstructions() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetMergeMenuInstructions();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
bool AddonsOptions::GetMergeToolbarInstructions(
    const ::rtl::OUString& rToolbarName,
    MergeToolbarInstructionContainer& rToolbarInstructions ) const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetMergeToolbarInstructions(
        rToolbarName, rToolbarInstructions );
}

const MergeStatusbarInstructionContainer& AddonsOptions::GetMergeStatusbarInstructions() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetMergeStatusbarInstructions();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Image AddonsOptions::GetImageFromURL( const rtl::OUString& aURL, sal_Bool bBig, sal_Bool bHiContrast, sal_Bool bNoScale ) const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetImageFromURL( aURL, bBig, bHiContrast, bNoScale );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Image AddonsOptions::GetImageFromURL( const rtl::OUString& aURL, sal_Bool bBig, sal_Bool bHiContrast ) const
{
    return GetImageFromURL( aURL, bBig, bHiContrast, sal_False );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Mutex& AddonsOptions::GetOwnStaticMutex()
{
	// Initialize static mutex only for one time!
    static Mutex* pMutex = NULL;
	// If these method first called (Mutex not already exist!) ...
    if( pMutex == NULL )
    {
		// ... we must create a new one. Protect follow code with the global mutex -
		// It must be - we create a static variable!
        MutexGuard aGuard( Mutex::getGlobalMutex() );
		// We must check our pointer again - because it can be that another instance of our class will be faster than these!
        if( pMutex == NULL )
        {
			// Create the new mutex and set it for return on static variable.
            static Mutex aMutex;
            pMutex = &aMutex;
        }
    }
	// Return new created or already existing mutex object.
    return *pMutex;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
IMPL_STATIC_LINK_NOINSTANCE( AddonsOptions, Notify, void*, EMPTYARG )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->ReadConfigurationData();
	return 0;
}

}

