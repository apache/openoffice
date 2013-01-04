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



#ifndef _XMLOFF_PROPERTYHANDLER_TABSTOPTYPES_HXX
#include <tabsthdl.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_ 
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef _COM_SUN_STAR_STYLE_TABSTOP_HPP_
#include <com/sun/star/style/TabStop.hpp>
#endif
namespace binfilter {

using namespace ::com::sun::star;

///////////////////////////////////////////////////////////////////////////////
//
// class XMLFontFamilyNamePropHdl
//

XMLTabStopPropHdl::~XMLTabStopPropHdl()
{
	// Nothing to do
}

sal_Bool XMLTabStopPropHdl::equals( const uno::Any& r1, const uno::Any& r2 ) const
{
	sal_Bool bEqual = sal_False;

	uno::Sequence< style::TabStop> aSeq1;
	if( r1 >>= aSeq1 )
	{
		uno::Sequence< style::TabStop> aSeq2;
		if( r2 >>= aSeq2 )
		{
			if( aSeq1.getLength() == aSeq2.getLength() )
			{
				bEqual = sal_True;
				if( aSeq1.getLength() > 0 )
				{
					const style::TabStop* pTabs1 = aSeq1.getConstArray();
					const style::TabStop* pTabs2 = aSeq2.getConstArray();

					int i=0;

					do
					{
						bEqual = ( pTabs1[i].Position == pTabs2[i].Position       &&
								   pTabs1[i].Alignment == pTabs2[i].Alignment     &&
								   pTabs1[i].DecimalChar == pTabs2[i].DecimalChar &&
								   pTabs1[i].FillChar == pTabs2[i].FillChar );
						i++;

					} while( bEqual && i < aSeq1.getLength() );
				}
			}
		}
	}

	return bEqual;
}

sal_Bool XMLTabStopPropHdl::importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	return sal_False; 
}

sal_Bool XMLTabStopPropHdl::exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	return sal_False;
}

}//end of namespace binfilter
