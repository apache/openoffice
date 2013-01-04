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



#ifndef _SD_XMLVIEWSETTINGSCONTEXT_HXX
#define _SD_XMLVIEWSETTINGSCONTEXT_HXX


#ifndef _COM_SUN_STAR_AWT_RECTANGLE_HPP_
#include <com/sun/star/awt/Rectangle.hpp>
#endif

#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif
namespace binfilter {

class SdXMLImport;

class SdXMLViewSettingsContext : public SvXMLImportContext
{
	::com::sun::star::awt::Rectangle maVisArea;

public:
	SdXMLViewSettingsContext( SdXMLImport& rImport, USHORT nPrfx, const ::rtl::OUString& rLName,
									  const ::com::sun::star::uno::Reference<
									  ::com::sun::star::xml::sax::XAttributeList>& xAttrList);
	virtual ~SdXMLViewSettingsContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
													const ::rtl::OUString& rLocalName,
													const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );
	virtual void EndElement();
};

}//end of namespace binfilter
#endif
