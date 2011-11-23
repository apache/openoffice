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



#ifndef _SVX_UNOIPSET_HXX_
#define _SVX_UNOIPSET_HXX_

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
namespace binfilter {
struct SfxItemPropertyMap;
class SfxItemSet;
class SvxIDPropertyCombineList;
class SdrItemPool;

class SvxShape;

class SvxItemPropertySet
{
	const SfxItemPropertyMap* 	_pMap;
	const SfxItemPropertyMap*	mpLastMap;
	SvxIDPropertyCombineList*	pCombiList;
	sal_Bool					mbConvertTwips;
//-/	void Generate3DAttrDefaultItem(sal_uInt16 nWID, SfxItemSet& rSet);

public:
	SvxItemPropertySet( const SfxItemPropertyMap *pMap, sal_Bool bConvertTwips = sal_False );
	~SvxItemPropertySet();

	// Methoden, die direkt mit dem ItemSet arbeiten
	::com::sun::star::uno::Any getPropertyValue( const SfxItemPropertyMap* pMap, const SfxItemSet& rSet ) const;
	void setPropertyValue( const SfxItemPropertyMap* pMap, const ::com::sun::star::uno::Any& rVal, SfxItemSet& rSet ) const;

	// Methoden, die stattdessen Any benutzen
	::com::sun::star::uno::Any getPropertyValue( const SfxItemPropertyMap* pMap ) const;
	void setPropertyValue( const SfxItemPropertyMap* pMap, const ::com::sun::star::uno::Any& rVal ) const;

	// Properties von einem anderen Set uebernehmen
	void ObtainSettingsFromPropertySet(SvxItemPropertySet& rPropSet,  SfxItemSet& rSet, ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > xSet );
	sal_Bool AreThereOwnUsrAnys() { return (pCombiList ? sal_True : sal_False); }
	::com::sun::star::uno::Any* GetUsrAnyForID(sal_uInt16 nWID) const;
	void AddUsrAnyForID(const ::com::sun::star::uno::Any& rAny, sal_uInt16 nWID);

	::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > getPropertySetInfo() const;
	const SfxItemPropertyMap* getPropertyMap() const {return _pMap;}
	void setPropertyMap( const SfxItemPropertyMap *pMap ) { _pMap = pMap; }
	const SfxItemPropertyMap* getPropertyMapEntry(const ::rtl::OUString &rName) const;

	static ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > getPropertySetInfo( const SfxItemPropertyMap* pMap );
};

}//end of namespace binfilter
#endif // _SVX_UNOIPSET_HXX_

