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
#include "precompiled_basic.hxx"
#include "errobject.hxx"

#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/script/XDefaultProperty.hpp>
#include "sbintern.hxx"
#include "runtime.hxx"

using namespace ::com::sun::star;
using namespace ::ooo;

typedef ::cppu::WeakImplHelper2< vba::XErrObject, script::XDefaultProperty > ErrObjectImpl_BASE;

class ErrObject : public ErrObjectImpl_BASE
{
	rtl::OUString m_sHelpFile;
	rtl::OUString m_sSource;
	rtl::OUString m_sDescription;
	sal_Int32 m_nNumber;
	sal_Int32 m_nHelpContext;

public:
	ErrObject();
	~ErrObject();
	// Attributes
	virtual ::sal_Int32 SAL_CALL getNumber();
	virtual void SAL_CALL setNumber( ::sal_Int32 _number );
	virtual ::sal_Int32 SAL_CALL getHelpContext();
	virtual void SAL_CALL setHelpContext( ::sal_Int32 _helpcontext );
	virtual ::rtl::OUString SAL_CALL getHelpFile();
	virtual void SAL_CALL setHelpFile( const ::rtl::OUString& _helpfile );
	virtual ::rtl::OUString SAL_CALL getDescription();
	virtual void SAL_CALL setDescription( const ::rtl::OUString& _description );
	virtual ::rtl::OUString SAL_CALL getSource();
	virtual void SAL_CALL setSource( const ::rtl::OUString& _source );

	// Methods
	virtual void SAL_CALL Clear(  );
	virtual void SAL_CALL Raise( const uno::Any& Number, const uno::Any& Source, const uno::Any& Description, const uno::Any& HelpFile, const uno::Any& HelpContext );
	// XDefaultProperty
	virtual ::rtl::OUString SAL_CALL getDefaultPropertyName(  );

	// Helper method
	void setData( const uno::Any& Number, const uno::Any& Source, const uno::Any& Description,
		const uno::Any& HelpFile, const uno::Any& HelpContext );
};


ErrObject::~ErrObject()
{
}

ErrObject::ErrObject() : m_nNumber(0), m_nHelpContext(0)
{
}

sal_Int32 SAL_CALL
ErrObject::getNumber()
{
	return m_nNumber;
}

void SAL_CALL
ErrObject::setNumber( ::sal_Int32 _number )
{
	pINST->setErrorVB( _number, String() );
	::rtl::OUString _description = pINST->GetErrorMsg();
	setData( uno::makeAny( _number ), uno::Any(), uno::makeAny( _description ), uno::Any(), uno::Any() );
}

::sal_Int32 SAL_CALL
ErrObject::getHelpContext()
{
	return m_nHelpContext;
}
void SAL_CALL
ErrObject::setHelpContext( ::sal_Int32 _helpcontext )
{
	m_nHelpContext = _helpcontext;
}

::rtl::OUString SAL_CALL
ErrObject::getHelpFile()
{
	return m_sHelpFile;
}

void SAL_CALL
ErrObject::setHelpFile( const ::rtl::OUString& _helpfile )
{
	m_sHelpFile = _helpfile;
}

::rtl::OUString SAL_CALL
ErrObject::getDescription()
{
	return m_sDescription;
}

void SAL_CALL
ErrObject::setDescription( const ::rtl::OUString& _description )
{
	m_sDescription = _description;
}

::rtl::OUString SAL_CALL
ErrObject::getSource()
{
	return m_sSource;
}

void SAL_CALL
ErrObject::setSource( const ::rtl::OUString& _source )
{
	m_sSource = _source;
}

// Methods
void SAL_CALL
ErrObject::Clear(  )
{
	m_sHelpFile = rtl::OUString();
	m_sSource = m_sHelpFile;
	m_sDescription = m_sSource;
	m_nNumber = 0;
	m_nHelpContext = 0;
}

void SAL_CALL
ErrObject::Raise( const uno::Any& Number, const uno::Any& Source, const uno::Any& Description, const uno::Any& HelpFile, const uno::Any& HelpContext )
{
	setData( Number, Source, Description, HelpFile, HelpContext );
	if ( m_nNumber )
		pINST->ErrorVB( m_nNumber, m_sDescription );
}

// XDefaultProperty
::rtl::OUString SAL_CALL
ErrObject::getDefaultPropertyName(  )
{
	static rtl::OUString sDfltPropName( RTL_CONSTASCII_USTRINGPARAM("Number") );
	return sDfltPropName;
}

void ErrObject::setData( const uno::Any& Number, const uno::Any& Source, const uno::Any& Description, const uno::Any& HelpFile, const uno::Any& HelpContext )
{
	if ( !Number.hasValue() )
		throw uno::RuntimeException( rtl::OUString::createFromAscii("Missing Required Parameter"), uno::Reference< uno::XInterface >() );
	Number >>= m_nNumber;
	Description >>= m_sDescription;
	Source >>= m_sSource;
	HelpFile >>= m_sHelpFile;
	HelpContext >>= m_nHelpContext;
}

// SbxErrObject
SbxErrObject::SbxErrObject( const String& rName, const Any& rUnoObj )
	: SbUnoObject( rName, rUnoObj )
	, m_pErrObject( NULL )
{
	OSL_TRACE("SbxErrObject::SbxErrObject ctor");
	rUnoObj >>= m_xErr;
	if ( m_xErr.is() )
	{
		SetDfltProperty( uno::Reference< script::XDefaultProperty >( m_xErr, uno::UNO_QUERY_THROW )->getDefaultPropertyName() ) ;
		m_pErrObject = static_cast< ErrObject* >( m_xErr.get() );
	}
}

SbxErrObject::~SbxErrObject()
{
	OSL_TRACE("SbxErrObject::~SbxErrObject dtor");
}

uno::Reference< vba::XErrObject >
SbxErrObject::getUnoErrObject()
{
	SbxVariable* pVar = getErrObject();
	SbxErrObject* pGlobErr = static_cast< SbxErrObject* >(  pVar );
	return pGlobErr->m_xErr;
}

SbxVariableRef
SbxErrObject::getErrObject()
{
	static SbxVariableRef pGlobErr = new SbxErrObject( String(  RTL_CONSTASCII_USTRINGPARAM("Err")), uno::makeAny( uno::Reference< vba::XErrObject >( new ErrObject() ) ) );
	return pGlobErr;
}

void SbxErrObject::setNumberAndDescription( ::sal_Int32 _number, const ::rtl::OUString& _description )
{
	if( m_pErrObject != NULL )
		m_pErrObject->setData( uno::makeAny( _number ), uno::Any(), uno::makeAny( _description ), uno::Any(), uno::Any() );
}

/* vim: set noet sw=4 ts=4: */
