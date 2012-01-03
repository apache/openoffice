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



#ifndef INCLUDED_I18NPOOL_MSLANGID_HXX
#include <i18npool/mslangid.hxx>
#endif

#ifndef _COMPHELPER_PROPERTSETINFO_HXX_ 
#include <comphelper/propertysetinfo.hxx>
#endif

#ifndef _EEITEM_HXX 
#include <bf_svx/eeitem.hxx>
#endif

#ifndef _SVX_UNOPOOL_HXX_ 
#include <bf_svx/unopool.hxx>
#endif

#include "drawdoc.hxx"
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::rtl;
using namespace ::cppu;
using namespace ::comphelper;

LanguageType SdUnoGetLanguage( const lang::Locale& rLocale )
{
	//	empty language -> LANGUAGE_SYSTEM
	if ( rLocale.Language.getLength() == 0 )
		return LANGUAGE_SYSTEM;

	LanguageType eRet = MsLangId::convertLocaleToLanguage( rLocale );
	if ( eRet == LANGUAGE_NONE )
		eRet = LANGUAGE_SYSTEM;			//! or throw an exception?

	return eRet;
}

class SdUnoDrawPool :	public SvxUnoDrawPool
{
public:
	SdUnoDrawPool( SdDrawDocument* pModel ) throw();
	virtual ~SdUnoDrawPool() throw();

protected:
	virtual void putAny( SfxItemPool* pPool, const PropertyMapEntry* pEntry, const Any& rValue ) throw( UnknownPropertyException, IllegalArgumentException);

private:
	SdDrawDocument* mpDrawModel;
};

SdUnoDrawPool::SdUnoDrawPool( SdDrawDocument* pModel ) throw()
: SvxUnoDrawPool( pModel ), mpDrawModel( pModel )
{
}

SdUnoDrawPool::~SdUnoDrawPool() throw()
{
}

void SdUnoDrawPool::putAny( SfxItemPool* pPool, const comphelper::PropertyMapEntry* pEntry, const Any& rValue )
	throw(UnknownPropertyException, IllegalArgumentException)
{
	switch( pEntry->mnHandle )
	{
	case EE_CHAR_LANGUAGE:
	case EE_CHAR_LANGUAGE_CJK:
	case EE_CHAR_LANGUAGE_CTL:
		{
			lang::Locale aLocale;
			if( rValue >>= aLocale )
				mpDrawModel->SetLanguage( SdUnoGetLanguage( aLocale ), pEntry->mnHandle );
		}
	}
	SvxUnoDrawPool::putAny( pPool, pEntry, rValue );
}

Reference< XInterface > SdUnoCreatePool( SdDrawDocument* pDrawModel )
{
	return (uno::XAggregation*)new SdUnoDrawPool( pDrawModel );
}
}
