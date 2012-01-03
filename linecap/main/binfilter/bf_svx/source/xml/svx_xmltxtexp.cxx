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



/** this file implements an export of a selected EditEngine content into
	a xml stream. See bf_svx/source/inc/xmledit.hxx for interface */

#ifndef _COM_SUN_STAR_UCB_XANYCOMPAREFACTORY_HPP_ 
#include <com/sun/star/ucb/XAnyCompareFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_XSTYLEFAMILIESSUPPLIER_HPP_ 
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XACTIVEDATASOURCE_HPP_
#include <com/sun/star/io/XActiveDataSource.hpp>
#endif 











#ifndef _UTL_STREAM_WRAPPER_HXX_
#include <unotools/streamwrap.hxx>
#endif


#ifndef _XMLOFF_XMLEXP_HXX
#include "bf_xmloff/xmlexp.hxx"
#endif


#ifndef _SVX_UNOFORED_HXX 
#include "unofored.hxx"
#endif

#ifndef _SVX_UNOTEXT_HXX 
#include "unotext.hxx"
#endif

#ifndef _SVX_EDITSOURCE_HXX
#include "editsource.hxx"
#endif

#include <cppuhelper/implbase4.hxx>

#ifndef _SVX_UNOFIELD_HXX 
#include "unofield.hxx"
#endif

#ifndef SVX_UNOMOD_HXX 
#include "unomod.hxx"
#endif
#include "unonrule.hxx"
#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>	//STRIP002 
#endif
namespace binfilter {
using namespace ::com::sun::star;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::xml::sax;
using namespace ::rtl;
using namespace cppu;

///////////////////////////////////////////////////////////////////////

class SvxEditEngineSourceImpl;

///////////////////////////////////////////////////////////////////////

/*N*/ class SvxEditEngineSourceImpl
/*N*/ {
/*N*/ private:
/*N*/ 	oslInterlockedCount	maRefCount;
/*N*/ 
/*N*/ 	EditEngine*				mpEditEngine;
/*N*/ 	SvxTextForwarder*		mpTextForwarder;
/*N*/ 
/*N*/ 	~SvxEditEngineSourceImpl();
/*N*/ 
/*N*/ public:
/*N*/ 	void SAL_CALL acquire();
/*N*/ 	void SAL_CALL release();
/*N*/ 
/*N*/ 	SvxTextForwarder*		GetTextForwarder();
/*N*/ };
/*N*/ 
/*N*/ ///////////////////////////////////////////////////////////////////////
/*N*/ 
/*N*/ SvxEditEngineSourceImpl::~SvxEditEngineSourceImpl()
/*N*/ {
/*N*/ 	delete mpTextForwarder;
/*N*/ }
/*N*/ 
/*N*/ //------------------------------------------------------------------------
/*N*/ 
/*N*/ void SAL_CALL SvxEditEngineSourceImpl::acquire() 
/*N*/ {
/*N*/ 	osl_incrementInterlockedCount( &maRefCount );
/*N*/ }	
/*N*/ 
/*N*/ //------------------------------------------------------------------------
/*N*/ 
/*N*/ void SAL_CALL SvxEditEngineSourceImpl::release()
/*N*/ {
/*N*/ 	if( ! osl_decrementInterlockedCount( &maRefCount ) )
/*N*/ 		delete this;
/*N*/ }	
/*N*/ 
/*N*/ //------------------------------------------------------------------------
/*N*/ 
/*N*/ SvxTextForwarder* SvxEditEngineSourceImpl::GetTextForwarder()
/*N*/ {
/*N*/ 	if (!mpTextForwarder)
/*N*/ 		mpTextForwarder = new SvxEditEngineForwarder( *mpEditEngine );
/*N*/ 
/*N*/ 	return mpTextForwarder;
/*N*/ }
/*N*/ 
/*N*/ // --------------------------------------------------------------------
/*N*/ // SvxTextEditSource
/*N*/ // --------------------------------------------------------------------
/*N*/ 
/*N*/ SvxEditEngineSource::SvxEditEngineSource( SvxEditEngineSourceImpl* pImpl )
/*N*/ {
/*N*/ 	mpImpl = pImpl;
/*N*/ 	mpImpl->acquire();
/*N*/ }
/*N*/ 
/*N*/ //------------------------------------------------------------------------
/*N*/ 
/*N*/ SvxEditEngineSource::~SvxEditEngineSource()
/*N*/ {
/*N*/ 	mpImpl->release();
/*N*/ }
/*N*/ 
/*N*/ //------------------------------------------------------------------------
/*N*/ 
/*N*/ SvxEditSource* SvxEditEngineSource::Clone() const
/*N*/ {
/*N*/ 	return new SvxEditEngineSource( mpImpl );
/*N*/ }
/*N*/ 
/*N*/ //------------------------------------------------------------------------
/*N*/ 
/*N*/ SvxTextForwarder* SvxEditEngineSource::GetTextForwarder()
/*N*/ {
/*N*/     return mpImpl->GetTextForwarder();
/*N*/ }
/*N*/ 
/*N*/ //------------------------------------------------------------------------
/*N*/ 
/*N*/ void SvxEditEngineSource::UpdateData()
/*N*/ {
/*N*/ }
}
