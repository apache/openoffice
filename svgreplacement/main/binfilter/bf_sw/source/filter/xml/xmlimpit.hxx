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



#ifndef _XMLIMPIT_HXX
#define _XMLIMPIT_HXX

#include <limits.h>

#include <bf_svtools/bf_solar.h>
#ifndef _REF_HXX
#include <tools/ref.hxx>
#endif


#ifndef _COM_SUN_STAR_XML_SAX_XATTRIBUTELIST_HPP_
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#endif

#ifndef _XMLITMAP_HXX
#include "xmlitmap.hxx"
#endif

namespace rtl { class OUString; }
namespace binfilter {

class SfxPoolItem;
class SfxItemSet; 

class SvXMLUnitConverter; 
class SvXMLAttributeList; 
class SvXMLNamespaceMap; 
struct SvXMLItemMapEntry;

class SvXMLImportItemMapper
{
protected:
	SvXMLItemMapEntriesRef mrMapEntries;
	USHORT nUnknownWhich;

public:
	SvXMLImportItemMapper( SvXMLItemMapEntriesRef rMapEntries ,
						   USHORT nUnknWhich=USHRT_MAX );
	virtual ~SvXMLImportItemMapper();

	/** fills the given itemset with the attributes in the given list */
	void importXML( SfxItemSet& rSet,
					::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > xAttrList,
					const SvXMLUnitConverter& rUnitConverter,
					const SvXMLNamespaceMap& rNamespaceMap ) const;

	/** this method is called for every item that has the
		MID_FLAG_SPECIAL_ITEM_IMPORT flag set */
	virtual BOOL handleSpecialItem( const SvXMLItemMapEntry& rEntry,
									SfxPoolItem& rItem,
									SfxItemSet& rSet,
									const ::rtl::OUString& rValue,
									const SvXMLUnitConverter& rUnitConverter,
									const SvXMLNamespaceMap& rNamespaceMap ) const;

	/** this method is called for every item that has the
		MID_FLAG_NO_ITEM_IMPORT flag set */
	virtual BOOL handleNoItem( const SvXMLItemMapEntry& rEntry,
							   SfxItemSet& rSet,
							   const ::rtl::OUString& rValue,
							   const SvXMLUnitConverter& rUnitConverter,
							   const SvXMLNamespaceMap& rNamespaceMap ) const;
	
	/** This method is called when all attributes have benn processed. It
	  * may be used to remove items that are incomplete */
	virtual void finished( SfxItemSet& rSet ) const;

	inline void setMapEntries( SvXMLItemMapEntriesRef rMapEntries );
	inline SvXMLItemMapEntriesRef getMapEntries() const;


    /** This method is called for every item that should be set based
        upon an XML attribute value. */
    static sal_Bool PutXMLValue( 
        SfxPoolItem& rItem,
        const ::rtl::OUString& rValue, 
        sal_uInt16 nMemberId,
        const SvXMLUnitConverter& rUnitConverter );
};

inline void
SvXMLImportItemMapper::setMapEntries( SvXMLItemMapEntriesRef rMapEntries )
{
	mrMapEntries = rMapEntries;
}

inline SvXMLItemMapEntriesRef
SvXMLImportItemMapper::getMapEntries() const
{
	return mrMapEntries;
}


} //namespace binfilter
#endif	//  _XMLIMPIT_HXX
