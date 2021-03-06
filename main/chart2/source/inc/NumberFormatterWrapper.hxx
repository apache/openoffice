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


#ifndef _CHART2_TOOLS_NUMBERFORMATTERWRAPPER_HXX
#define _CHART2_TOOLS_NUMBERFORMATTERWRAPPER_HXX

#include "charttoolsdllapi.hxx"
#include <svl/zforlist.hxx>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>

//.............................................................................
namespace chart
{
//.............................................................................

//-----------------------------------------------------------------------------
/**
*/
class FixedNumberFormatter;

class OOO_DLLPUBLIC_CHARTTOOLS NumberFormatterWrapper
{
public:
    NumberFormatterWrapper( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier >& xSupplier );
    virtual ~NumberFormatterWrapper();

    SvNumberFormatter* getSvNumberFormatter() const;
    ::com::sun::star::uno::Reference< com::sun::star::util::XNumberFormatsSupplier >
                getNumberFormatsSupplier() { return m_xNumberFormatsSupplier; };

    rtl::OUString getFormattedString( sal_Int32 nNumberFormatKey, double fValue, sal_Int32& rLabelColor, bool& rbColorChanged ) const;
    Date    getNullDate() const;

private: //private member
    ::com::sun::star::uno::Reference< com::sun::star::util::XNumberFormatsSupplier >
                        m_xNumberFormatsSupplier;

    SvNumberFormatter* m_pNumberFormatter;
    ::com::sun::star::uno::Any m_aNullDate;
};


class OOO_DLLPUBLIC_CHARTTOOLS FixedNumberFormatter
{
public:
    FixedNumberFormatter( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier >& xSupplier
        , sal_Int32 nNumberFormatKey );
    virtual ~FixedNumberFormatter();

    rtl::OUString getFormattedString( double fValue, sal_Int32& rLabelColor, bool& rbColorChanged ) const;

private:
    NumberFormatterWrapper      m_aNumberFormatterWrapper;
    sal_uLong                       m_nNumberFormatKey;
};

//.............................................................................
} //namespace chart
//.............................................................................
#endif
