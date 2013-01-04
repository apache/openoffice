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



#include "fltlst.hxx"

//*****************************************************************************************************************
//  includes
//*****************************************************************************************************************



#include "sfxuno.hxx"

#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>	//STRIP002 
#endif
namespace binfilter {

//*****************************************************************************************************************
//  namespaces
//*****************************************************************************************************************
using namespace ::com::sun::star;

//*****************************************************************************************************************
//  definitions
//*****************************************************************************************************************

/*-************************************************************************************************************//**
    @short          ctor
    @descr          These initialize an instance of a SfxFilterListener class. Created object listen automaticly
                    on right FilterFactory-Service for all changes and synchronize right SfxFilterContainer with
                    corresponding framework-cache.
                    We use given "sFactory" value to decide which query must be used to fill "pContainer" with new values.
                    Given "pContainer" hold us alive as uno reference and we use it to syschronize it with framework caches.
                    We will die, if he die! see dtor for further informations.

    @seealso        dtor
    @seealso        class framework::FilterCache
    @seealso        service ::document::FilterFactory

    @param          "sFactory"  , short name of module which contains filter container
    @param          "pContainer", pointer to filter container which will be informed
    @return         -

    @onerror        We show some assertions in non product version.
                    Otherwise we do nothing!
    @threadsafe     yes

    @last_change    17.10.2001 10:27
*//*-*************************************************************************************************************/
/*N*/ SfxFilterListener::SfxFilterListener( const ::rtl::OUString&    sFactory   ,
/*N*/                                             SfxFilterContainer* pContainer )
/*N*/     :   m_aMutex    (            )
/*N*/     ,   m_pContainer( pContainer )
/*N*/ {
/*N*/     // search for right factory long name by using given shortname.
/*N*/     // These value is neccessary for "ReadExternalFilter()" call during our "flushed()" function.
/*N*/     m_sFactory = ::rtl::OUString();
/*N*/     if( sFactory == DEFINE_CONST_OUSTRING("swriter") )
/*N*/         m_sFactory = DEFINE_CONST_OUSTRING("com.sun.star.text.TextDocument");
/*N*/     else
/*N*/     if( sFactory == DEFINE_CONST_OUSTRING("swriter/web") )
/*N*/         m_sFactory = DEFINE_CONST_OUSTRING("com.sun.star.text.WebDocument");
/*N*/     else
/*N*/     if( sFactory == DEFINE_CONST_OUSTRING("swriter/GlobalDocument") )
/*N*/         m_sFactory = DEFINE_CONST_OUSTRING("com.sun.star.text.GlobalDocument");
/*N*/     else
/*N*/     if( sFactory == DEFINE_CONST_OUSTRING("schart") )
/*N*/         m_sFactory = DEFINE_CONST_OUSTRING("com.sun.star.chart.ChartDocument");
/*N*/     else
/*N*/     if( sFactory == DEFINE_CONST_OUSTRING("scalc") )
/*N*/         m_sFactory = DEFINE_CONST_OUSTRING("com.sun.star.sheet.SpreadsheetDocument");
/*N*/     else
/*N*/     if( sFactory == DEFINE_CONST_OUSTRING("sdraw") )
/*N*/         m_sFactory = DEFINE_CONST_OUSTRING("com.sun.star.drawing.DrawingDocument");
/*N*/     else
/*N*/     if( sFactory == DEFINE_CONST_OUSTRING("simpress") )
/*N*/         m_sFactory = DEFINE_CONST_OUSTRING("com.sun.star.presentation.PresentationDocument");
/*N*/     else
/*N*/     if( sFactory == DEFINE_CONST_OUSTRING("smath") )
/*N*/         m_sFactory = DEFINE_CONST_OUSTRING("com.sun.star.formula.FormulaProperties");
/*N*/ 
/*N*/     OSL_ENSURE( !(m_sFactory.getLength()<1), "SfxFilterListener::SfxFilterListener()\nUnknown factory found! Can't listen for nothing ...\n" );
/*N*/ 
/*N*/     // Start listening on framework filter cache only, if factory is valid!
/*N*/     if( m_sFactory.getLength() > 0 )
/*N*/     {
/*N*/         uno::Reference< lang::XMultiServiceFactory > xSmgr = ::legacy_binfilters::getLegacyProcessServiceFactory();
/*N*/         if( xSmgr.is() == sal_True )
/*N*/         {
/*N*/             uno::Reference< util::XFlushable > xNotifier( xSmgr->createInstance( DEFINE_CONST_OUSTRING("com.sun.star.document.FilterFactory") ), uno::UNO_QUERY );
/*N*/             if( xNotifier.is() == sal_True )
/*N*/             {
/*N*/                 m_xFilterCache = xNotifier;
/*N*/                 m_xFilterCache->addFlushListener( this );
/*N*/             }
/*N*/ 
/*N*/             xNotifier = uno::Reference< util::XFlushable >( xSmgr->createInstance( DEFINE_CONST_OUSTRING("com.sun.star.document.TypeDetection") ), uno::UNO_QUERY );
/*N*/             if( xNotifier.is() == sal_True )
/*N*/             {
/*N*/                 m_xTypeCache = xNotifier;
/*N*/                 m_xTypeCache->addFlushListener( this );
/*N*/             }
/*N*/         }
/*N*/     }
/*N*/ }

/*?*/ SfxFilterListener::~SfxFilterListener()
/*?*/ {DBG_BF_ASSERT(0, "STRIP");//STRIP001 
/*?*/ }

/*?*/ void SAL_CALL SfxFilterListener::flushed( const lang::EventObject& aSource ) throw( uno::RuntimeException )
/*?*/ {{DBG_BF_ASSERT(0, "STRIP"); }//STRIP001 
/*?*/ }

/*?*/ void SAL_CALL SfxFilterListener::disposing( const lang::EventObject& aSource ) throw( uno::RuntimeException )
/*?*/ {{DBG_BF_ASSERT(0, "STRIP"); }//STRIP001 
/*?*/ }
}
