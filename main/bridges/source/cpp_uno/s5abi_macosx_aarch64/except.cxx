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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_bridges.hxx"

#if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))
#include <exception>
#endif

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <cxxabi.h>
#include <hash_map>
#include <sys/param.h>

#include <rtl/strbuf.hxx>
#include <rtl/ustrbuf.hxx>
#include <osl/diagnose.h>
#include <osl/mutex.hxx>

#include <com/sun/star/uno/genfunc.hxx>
#include "com/sun/star/uno/RuntimeException.hpp"
#include <typelib/typedescription.hxx>
#include <uno/any2.h>

#include "share.hxx"


using namespace ::std;
using namespace ::osl;
using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::__cxxabiv1;


namespace CPPU_CURRENT_NAMESPACE
{

namespace {

typedef hash_map< void *, typelib_TypeDescription * > ThrownTypes;
typedef hash_map< OUString, type_info *, OUStringHash > ObservedRttiMap;

ThrownTypes & thrownTypes()
{
    static ThrownTypes types;
    return types;
}

ObservedRttiMap & observedRttis()
{
    static ObservedRttiMap map;
    return map;
}

Mutex & observedRttisMutex()
{
    static Mutex mutex;
    return mutex;
}

}

void dummy_can_throw_anything( char const * )
{
}

//==================================================================================================
static OUString toUNOname( char const * p ) SAL_THROW( () )
{
#if OSL_DEBUG_LEVEL > 1
    char const * start = p;
#endif

    // example: N3com3sun4star4lang24IllegalArgumentExceptionE

	OUStringBuffer buf( 64 );
    OSL_ASSERT( 'N' == *p );
    ++p; // skip N

    while ('E' != *p)
    {
        // read chars count
        long n = (*p++ - '0');
        while ('0' <= *p && '9' >= *p)
        {
            n *= 10;
            n += (*p++ - '0');
        }
        buf.appendAscii( p, n );
        p += n;
        if ('E' != *p)
            buf.append( (sal_Unicode)'.' );
    }

#if OSL_DEBUG_LEVEL > 1
    OUString ret( buf.makeStringAndClear() );
    OString c_ret( OUStringToOString( ret, RTL_TEXTENCODING_ASCII_US ) );
    fprintf( stderr, "> toUNOname(): %s => %s\n", start, c_ret.getStr() );
    return ret;
#else
    return buf.makeStringAndClear();
#endif
}

//==================================================================================================
class RTTI
{
    typedef hash_map< OUString, type_info *, OUStringHash > t_rtti_map;

    Mutex m_mutex;
	t_rtti_map m_rttis;

public:
    RTTI() SAL_THROW( () );
    ~RTTI() SAL_THROW( () );

    type_info * getRTTI( typelib_CompoundTypeDescription * ) SAL_THROW( () );
};

//__________________________________________________________________________________________________
RTTI::RTTI() SAL_THROW( () )
{
}

//__________________________________________________________________________________________________
RTTI::~RTTI() SAL_THROW( () )
{
}

//__________________________________________________________________________________________________
type_info * RTTI::getRTTI( typelib_CompoundTypeDescription *pTypeDescr ) SAL_THROW( () )
{
    type_info * rtti;

    OUString const & unoName = *(OUString const *)&pTypeDescr->aBase.pTypeName;

    MutexGuard guard( m_mutex );

    {
        MutexGuard observedGuard( observedRttisMutex() );
        ObservedRttiMap::const_iterator observed( observedRttis().find( unoName ) );
        if ( observed != observedRttis().end() )
            return observed->second;
    }

    t_rtti_map::const_iterator iFind( m_rttis.find( unoName ) );
    if (iFind == m_rttis.end())
    {
        // RTTI symbol
        OStringBuffer buf( 64 );
        buf.append( RTL_CONSTASCII_STRINGPARAM("_ZTIN") );
        sal_Int32 index = 0;
        do
        {
            OUString token( unoName.getToken( 0, '.', index ) );
            buf.append( token.getLength() );
            OString c_token( OUStringToOString( token, RTL_TEXTENCODING_ASCII_US ) );
            buf.append( c_token );
        }
        while (index >= 0);
        buf.append( 'E' );

        OString symName( buf.makeStringAndClear() );
        rtti = static_cast<std::type_info *>(dlsym( RTLD_DEFAULT, symName.getStr() ));

        if (rtti)
        {
            m_rttis.insert( t_rtti_map::value_type( unoName, rtti ) );
        }
        else
            rtti = 0;
    }
    else
    {
        rtti = iFind->second;
    }

    return rtti;
}

//--------------------------------------------------------------------------------------------------
static void deleteException( void * pExc )
{
    typelib_TypeDescription * pTD = 0;
    {
        MutexGuard guard( observedRttisMutex() );
        ThrownTypes::iterator i = thrownTypes().find( pExc );
        if ( i != thrownTypes().end() )
        {
            pTD = i->second;
            thrownTypes().erase( i );
        }
    }
    if ( pTD )
    {
        ::uno_destructData( pExc, pTD, cpp_release );
        ::typelib_typedescription_release( pTD );
    }
}

//==================================================================================================
void raiseException( uno_Any * pUnoExc, uno_Mapping * pUno2Cpp )
{
#if OSL_DEBUG_LEVEL > 1
    OString cstr(
        OUStringToOString(
            *reinterpret_cast< OUString const * >( &pUnoExc->pType->pTypeName ),
            RTL_TEXTENCODING_ASCII_US ) );
    fprintf( stderr, "> uno exception occurred: %s\n", cstr.getStr() );
#endif
    void * pCppExc;
    type_info * rtti;
    OUString typeName(
        *reinterpret_cast< OUString const * >( &pUnoExc->pType->pTypeName ) );

    {
    // construct cpp exception object
	typelib_TypeDescription * pTypeDescr = 0;
	TYPELIB_DANGER_GET( &pTypeDescr, pUnoExc->pType );
    OSL_ASSERT( pTypeDescr );
    if (! pTypeDescr)
    {
        throw RuntimeException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("cannot get typedescription for type ") ) +
            *reinterpret_cast< OUString const * >( &pUnoExc->pType->pTypeName ),
            Reference< XInterface >() );
    }

	pCppExc = __cxa_allocate_exception( pTypeDescr->nSize );
	::uno_copyAndConvertData( pCppExc, pUnoExc->pData, pTypeDescr, pUno2Cpp );

	static RTTI rtti_data;
	rtti = rtti_data.getRTTI(
		(typelib_CompoundTypeDescription *) pTypeDescr );
    OSL_ENSURE( rtti, "### no rtti for throwing exception!" );
    if (! rtti)
    {
        throw RuntimeException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("no rtti for type ") ) +
            typeName,
            Reference< XInterface >() );
    }

    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        typelib_typedescription_acquire( pTypeDescr );
        thrownTypes()[pCppExc] = pTypeDescr;
    }
    TYPELIB_DANGER_RELEASE( pTypeDescr );

	// The C++ payload and its retained type description now own the value.
	::uno_any_destruct( pUnoExc, 0 );
    }

	__cxa_throw( pCppExc, rtti, deleteException );
}

void fillUnoException(
    std::type_info const & type, void * exception, uno_Any * pUnoExc,
    uno_Mapping * pCpp2Uno )
{
    typelib_TypeDescription * pExcTypeDescr = 0;
    OUString unoName( toUNOname( type.name() ) );
    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        observedRttis()[unoName] = const_cast<std::type_info *>( &type );
    }
    typelib_typedescription_getByName( &pExcTypeDescr, unoName.pData );
    if ( pExcTypeDescr == 0 )
    {
        RuntimeException aRE(
            OUString( RTL_CONSTASCII_USTRINGPARAM("exception type not found: ") ) + unoName,
            Reference< XInterface >() );
        Type const & rType = ::getCppuType( &aRE );
        uno_type_any_constructAndConvert(
            pUnoExc, &aRE, rType.getTypeLibType(), pCpp2Uno );
    }
    else
    {
        uno_any_constructAndConvert(
            pUnoExc, exception, pExcTypeDescr, pCpp2Uno );
        typelib_typedescription_release( pExcTypeDescr );
    }
}

}
