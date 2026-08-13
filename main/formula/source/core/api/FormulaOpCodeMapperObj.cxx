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


#include "precompiled_formula.hxx"
#include "formula/FormulaOpCodeMapperObj.hxx"
#include "formula/opcode.hxx"
#include <comphelper/sequence.hxx>

// =============================================================================
namespace formula
{
// =============================================================================

    using namespace ::com::sun::star;

    // -----------------------------------------------------------------------------
// --------------------------------------------------------------------------------
sal_Bool SAL_CALL FormulaOpCodeMapperObj::supportsService( const ::rtl::OUString& _rServiceName )
{
	return ::comphelper::findValue( getSupportedServiceNames_Static(), _rServiceName, sal_True ).getLength() != 0;
}
//------------------------------------------------------------------------
FormulaOpCodeMapperObj::FormulaOpCodeMapperObj(::std::auto_ptr<FormulaCompiler> _pCompiler)
: m_pCompiler(_pCompiler)
{
}

FormulaOpCodeMapperObj::~FormulaOpCodeMapperObj()
{
}


::sal_Int32 SAL_CALL FormulaOpCodeMapperObj::getOpCodeExternal()
{
    return ocExternal;
}


::sal_Int32 SAL_CALL FormulaOpCodeMapperObj::getOpCodeUnknown()
{
    return FormulaCompiler::OpCodeMap::getOpCodeUnknown();
}


::com::sun::star::uno::Sequence< ::com::sun::star::sheet::FormulaToken >
SAL_CALL FormulaOpCodeMapperObj::getMappings(
        const ::com::sun::star::uno::Sequence< ::rtl::OUString >& rNames,
        sal_Int32 nLanguage )
{
    FormulaCompiler::OpCodeMapPtr xMap = m_pCompiler->GetOpCodeMap( nLanguage);
    if (!xMap)
        throw lang::IllegalArgumentException();
    return xMap->createSequenceOfFormulaTokens( *m_pCompiler,rNames);
}


::com::sun::star::uno::Sequence< ::com::sun::star::sheet::FormulaOpCodeMapEntry >
SAL_CALL FormulaOpCodeMapperObj::getAvailableMappings(
        sal_Int32 nLanguage, sal_Int32 nGroups )
{
    FormulaCompiler::OpCodeMapPtr xMap = m_pCompiler->GetOpCodeMap( nLanguage);
    if (!xMap)
        throw lang::IllegalArgumentException();
    return xMap->createSequenceOfAvailableMappings( *m_pCompiler,nGroups);
}
//--------------------------------------------------------------------------
::rtl::OUString SAL_CALL FormulaOpCodeMapperObj::getImplementationName(  )
{
	return getImplementationName_Static();
}
// -----------------------------------------------------------------------------
::rtl::OUString SAL_CALL FormulaOpCodeMapperObj::getImplementationName_Static()
{
	return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "simple.formula.FormulaOpCodeMapperObj" ) );
}
// --------------------------------------------------------------------------------
uno::Sequence< ::rtl::OUString > SAL_CALL FormulaOpCodeMapperObj::getSupportedServiceNames(  )
{
    return getSupportedServiceNames_Static();
}
uno::Sequence< rtl::OUString > SAL_CALL FormulaOpCodeMapperObj::getSupportedServiceNames_Static()
{
	uno::Sequence< rtl::OUString > aSeq( 1 );
	aSeq[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sheet.FormulaOpCodeMapper" ));
	return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL FormulaOpCodeMapperObj::create(
                uno::Reference< uno::XComponentContext > const & /*_xContext*/)
{
    return static_cast<sheet::XFormulaOpCodeMapper*>(new FormulaOpCodeMapperObj(::std::auto_ptr<FormulaCompiler>(new FormulaCompiler())));
}
// -----------------------------------------------------------------------------

// =============================================================================
} // formula
// =============================================================================
