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



#ifndef SVX_UNOFDESC_HXX
#define SVX_UNOFDESC_HXX

#ifndef _COM_SUN_STAR_AWT_FONTDESCRIPTOR_HPP_
#include <com/sun/star/awt/FontDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYSTATE_HPP_
#include <com/sun/star/beans/PropertyState.hpp>
#endif

#ifndef _SFXITEMSET_HXX //autogen
#include <bf_svtools/itemset.hxx>
#endif

#ifndef _SV_FONT_HXX //autogen
#include <vcl/font.hxx>
#endif
namespace binfilter {

class SvxUnoFontDescriptor
{
public:
	static void ConvertToFont( const ::com::sun::star::awt::FontDescriptor& rDesc, Font& rFont );
	static void ConvertFromFont( const Font& rFont, ::com::sun::star::awt::FontDescriptor& rDesc );

	static void FillItemSet( const ::com::sun::star::awt::FontDescriptor& rDesc, SfxItemSet& rSet );
	static void FillFromItemSet( const SfxItemSet& rSet, ::com::sun::star::awt::FontDescriptor& rDesc );

	static void setPropertyToDefault( SfxItemSet& rSet );
	static ::com::sun::star::uno::Any getPropertyDefault( SfxItemPool* pPool );

};

}//end of namespace binfilter
#endif

