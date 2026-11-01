/*
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
 */


// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_bridges.hxx"

#if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))
#include <exception>
#endif

#include <stdio.h>
#include <stdlib.h>
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

// Guards BOTH thrownTypes() and observedRttis().  Every access to either map
// must hold this one mutex; they are plain hash_maps, so an insertion racing a
// find or erase is undefined behaviour.  RTTI::m_mutex may be held while
// acquiring this one (see RTTI::getRTTI), never the other way round.
Mutex & exceptionMapsMutex()
{
	static Mutex mutex;
	return mutex;
}

 // libc++ marks a type_info whose object is not unique across images by setting
 // the top bit of type_info::__type_name; comparison then falls back to strcmp
 // of the mangled name (see __non_unique_arm_rtti_bit_impl in <typeinfo>).
 // This is detected at runtime below; the synthesised RTTI mirrors whatever
 // convention the running compiler actually uses.
sal_uIntPtr const NON_UNIQUE_RTTI_BIT =
	static_cast< sal_uIntPtr >(1) << (8 * sizeof (sal_uIntPtr) - 1);

RttiSiClassLayout const * siDonor()
{
	return reinterpret_cast< RttiSiClassLayout const * >( &typeid(RttiDonorDerived) );
}
RttiClassLayout const * classDonor()
{
	return reinterpret_cast< RttiClassLayout const * >( &typeid(RttiDonorBase) );
}

// Refuse to synthesise unless the donors really have the layout we assume.
bool rttiDonorsUsable()
{
	return sizeof (void *) == 8
		&& siDonor()->pBase == static_cast< void const * >( classDonor() );
}

// Mirror the platform's own convention rather than assuming it.
bool rttiIsNonUnique()
{
	return (siDonor()->nName & NON_UNIQUE_RTTI_BIT) != 0;
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
static OString mangledRttiSymbol( OUString const & unoName ) SAL_THROW( () )
{
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
	return buf.makeStringAndClear();
}

//==================================================================================================
class RTTI
{
	typedef hash_map< OUString, type_info *, OUStringHash > t_rtti_map;

	Mutex m_mutex;
	t_rtti_map m_rttis;
	t_rtti_map m_generatedRttis;

	type_info * synthesiseRTTI(
		OString const & rSymbolName,
		typelib_CompoundTypeDescription * pTypeDescr ) SAL_THROW( () );

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
	OUString const & unoName = *(OUString const *)&pTypeDescr->aBase.pTypeName;

	// Recursive: synthesiseRTTI() re-enters getRTTI() for the base chain.
	// osl::Mutex is a PTHREAD_MUTEX_RECURSIVE (sal/osl/unx/mutex.c), so this
	// is safe.  Lock order against exceptionMapsMutex() is unchanged.
	MutexGuard guard( m_mutex );

	{
		MutexGuard observedGuard( exceptionMapsMutex() );
		ObservedRttiMap::const_iterator observed( observedRttis().find( unoName ) );
		if ( observed != observedRttis().end() )
			return observed->second;
	}

	t_rtti_map::const_iterator it = m_rttis.find( unoName );
	if ( it != m_rttis.end() )
		return it->second;

	OString mangled = mangledRttiSymbol( unoName );

	type_info * p = 0;
	{
		p = static_cast< type_info * >( dlsym( RTLD_DEFAULT, mangled.getStr() ) );
	}

	if ( p )
	{
		m_rttis[ unoName ] = p;
		return p;
	}

	// Fallback: synthesise a type_info object.
	OString symName = mangled;
	p = synthesiseRTTI( symName, pTypeDescr );
	m_rttis[ unoName ] = p;
	return p;
}

//__________________________________________________________________________________________________
type_info * RTTI::synthesiseRTTI(
	OString const & rSymbolName,
	typelib_CompoundTypeDescription * pTypeDescr ) SAL_THROW( () )
{
	if (! rttiDonorsUsable())
		return 0;

	type_info * pBaseRtti = 0;
	if (pTypeDescr->pBaseTypeDescription != 0)
	{
		pBaseRtti = getRTTI(
			(typelib_CompoundTypeDescription *) pTypeDescr->pBaseTypeDescription );
		if (pBaseRtti == 0)
			return 0;
	}

	char * pName = strdup( rSymbolName.getStr() + 4 );
	if (pName == 0)
		return 0;
	sal_uIntPtr nName = reinterpret_cast< sal_uIntPtr >( pName );
	if (rttiIsNonUnique())
		nName |= NON_UNIQUE_RTTI_BIT;

	if (pBaseRtti != 0)
	{
		RttiSiClassLayout * p = static_cast< RttiSiClassLayout * >(
			calloc( 1, sizeof (RttiSiClassLayout) ) );
		if (p == 0) { free( pName ); return 0; }
		p->pVtable = siDonor()->pVtable;
		p->nName   = nName;
		p->pBase   = pBaseRtti;
		return reinterpret_cast< type_info * >( p );
	}

	RttiClassLayout * p = static_cast< RttiClassLayout * >(
		calloc( 1, sizeof (RttiClassLayout) ) );
	if (p == 0) { free( pName ); return 0; }
	p->pVtable = classDonor()->pVtable;
	p->nName   = nName;
	return reinterpret_cast< type_info * >( p );
}

//--------------------------------------------------------------------------------------------------
static void deleteException( void * pExc )
{
	typelib_TypeDescription * pTD = 0;
	{
		MutexGuard guard( exceptionMapsMutex() );
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
	void * pCppExc;
	type_info * rtti;
	OUString typeName(
		*reinterpret_cast< OUString const * >( &pUnoExc->pType->pTypeName ) );

	OUString message;
	if ( pUnoExc->pData != 0 &&
		 *reinterpret_cast< rtl_uString * const * >( pUnoExc->pData ) != 0 )
	{
		message = *reinterpret_cast< OUString const * >( pUnoExc->pData );
	}

	{
	typelib_TypeDescription * pTypeDescr = 0;
	TYPELIB_DANGER_GET( &pTypeDescr, pUnoExc->pType );
	OSL_ASSERT( pTypeDescr );
	if (! pTypeDescr)
	{
		throw RuntimeException(
			OUString( RTL_CONSTASCII_USTRINGPARAM("cannot get typedescription for type ") ) +
			typeName + OUString( RTL_CONSTASCII_USTRINGPARAM(": ") ) + message,
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
		::uno_destructData( pCppExc, pTypeDescr, cpp_release );
		__cxa_free_exception( pCppExc );
		TYPELIB_DANGER_RELEASE( pTypeDescr );
		::uno_any_destruct( pUnoExc, 0 );
		throw RuntimeException(
			OUString( RTL_CONSTASCII_USTRINGPARAM("no rtti for type ") ) +
			typeName + OUString( RTL_CONSTASCII_USTRINGPARAM(": ") ) + message,
			Reference< XInterface >() );
	}

	{
		MutexGuard guard( exceptionMapsMutex() );
		typelib_typedescription_acquire( pTypeDescr );
		thrownTypes()[pCppExc] = pTypeDescr;
	}
	TYPELIB_DANGER_RELEASE( pTypeDescr );
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
		MutexGuard guard( exceptionMapsMutex() );
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

} // namespace CPPU_CURRENT_NAMESPACE
