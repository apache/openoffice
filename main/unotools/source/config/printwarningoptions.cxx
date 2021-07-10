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
#include "precompiled_unotools.hxx"
#ifndef GCC
#endif

//_________________________________________________________________________________________________________________
//	includes
//_________________________________________________________________________________________________________________

#include <unotools/printwarningoptions.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/configitem.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>

#include <itemholder1.hxx>

//_________________________________________________________________________________________________________________
//	namespaces
//_________________________________________________________________________________________________________________

using namespace ::utl					;
using namespace ::rtl					;
using namespace ::osl					;
using namespace ::com::sun::star::uno	;

//_________________________________________________________________________________________________________________
//	const
//_________________________________________________________________________________________________________________

#define ROOTNODE_START                  OUString(RTL_CONSTASCII_USTRINGPARAM("Office.Common/Print"))

#define PROPERTYNAME_PAPERSIZE          OUString(RTL_CONSTASCII_USTRINGPARAM("Warning/PaperSize"))
#define PROPERTYNAME_PAPERORIENTATION   OUString(RTL_CONSTASCII_USTRINGPARAM("Warning/PaperOrientation"))
#define PROPERTYNAME_NOTFOUND           OUString(RTL_CONSTASCII_USTRINGPARAM("Warning/NotFound"))
#define PROPERTYNAME_TRANSPARENCY       OUString(RTL_CONSTASCII_USTRINGPARAM("Warning/Transparency"))
#define PROPERTYNAME_PRINTINGMODIFIESDOCUMENT  OUString(RTL_CONSTASCII_USTRINGPARAM("PrintingModifiesDocument"))

#define	PROPERTYHANDLE_PAPERSIZE		0
#define	PROPERTYHANDLE_PAPERORIENTATION	1
#define	PROPERTYHANDLE_NOTFOUND	        2
#define	PROPERTYHANDLE_TRANSPARENCY 	3
#define PROPERTYHDL_PRINTINGMODIFIESDOCUMENT            4

#define PROPERTYCOUNT                   5

class SvtPrintWarningOptions_Impl : public ConfigItem
{
public:

//---------------------------------------------------------------------------------------------------------
//	constructor / destructor
//---------------------------------------------------------------------------------------------------------

	 SvtPrintWarningOptions_Impl();
	~SvtPrintWarningOptions_Impl();

//---------------------------------------------------------------------------------------------------------
//	overloaded methods of baseclass
//---------------------------------------------------------------------------------------------------------

	virtual void Commit();
    virtual void    Notify( const com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );

//---------------------------------------------------------------------------------------------------------
//	public interface
//---------------------------------------------------------------------------------------------------------

	sal_Bool	IsPaperSize() const { return m_bPaperSize; }
	sal_Bool	IsPaperOrientation() const { return m_bPaperOrientation; }
	sal_Bool	IsNotFound() const { return m_bNotFound; }
	sal_Bool	IsTransparency() const { return m_bTransparency; }
    sal_Bool    IsModifyDocumentOnPrintingAllowed() const { return m_bModifyDocumentOnPrintingAllowed; }

	void		SetPaperSize( sal_Bool bState ) { m_bPaperSize = bState; SetModified(); }
	void		SetPaperOrientation( sal_Bool bState ) { m_bPaperOrientation = bState; SetModified(); }
	void		SetNotFound( sal_Bool bState ) { m_bNotFound = bState; SetModified(); }
	void		SetTransparency( sal_Bool bState ) { m_bTransparency = bState; SetModified(); }
    void        SetModifyDocumentOnPrintingAllowed( sal_Bool bState ) { m_bModifyDocumentOnPrintingAllowed = bState; SetModified(); }

//-------------------------------------------------------------------------------------------------------------
//	private methods
//-------------------------------------------------------------------------------------------------------------

private:

	static Sequence< OUString > impl_GetPropertyNames();

//-------------------------------------------------------------------------------------------------------------
//	private member
//-------------------------------------------------------------------------------------------------------------

private:

	sal_Bool	m_bPaperSize;
	sal_Bool	m_bPaperOrientation;
	sal_Bool	m_bNotFound;
	sal_Bool	m_bTransparency;
    sal_Bool    m_bModifyDocumentOnPrintingAllowed;
};

//_________________________________________________________________________________________________________________
//	definitions
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtPrintWarningOptions_Impl::SvtPrintWarningOptions_Impl() :
	ConfigItem( ROOTNODE_START	),
	m_bPaperSize( sal_False ),
	m_bPaperOrientation( sal_False ),
	m_bNotFound( sal_False ),
    m_bTransparency( sal_True ),
    m_bModifyDocumentOnPrintingAllowed( sal_True )
{
	Sequence< OUString >	seqNames( impl_GetPropertyNames() );
	Sequence< Any >			seqValues( GetProperties( seqNames ) );

	DBG_ASSERT( !(seqNames.getLength()!=seqValues.getLength()), "SvtPrintWarningOptions_Impl::SvtPrintWarningOptions_Impl()\nI miss some values of configuration keys!\n" );

	// Copy values from list in right order to our internal member.
	sal_Int32 nPropertyCount = seqValues.getLength();
	sal_Int32 nProperty	= 0;

	for( nProperty=0; nProperty<nPropertyCount; ++nProperty )
	{
		DBG_ASSERT( !(seqValues[nProperty].hasValue()==sal_False), "SvtPrintWarningOptions_Impl::SvtPrintWarningOptions_Impl()\nInvalid property value for property detected!\n" );

        switch( nProperty )
        {
            case PROPERTYHANDLE_PAPERSIZE:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "Invalid type" );
				seqValues[nProperty] >>= m_bPaperSize;
			}
			break;

            case PROPERTYHANDLE_PAPERORIENTATION:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "Invalid type" );
				seqValues[nProperty] >>= m_bPaperOrientation;
			}
			break;

            case PROPERTYHANDLE_NOTFOUND:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "Invalid type" );
				seqValues[nProperty] >>= m_bNotFound;
			}
			break;

            case PROPERTYHANDLE_TRANSPARENCY:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "Invalid type" );
				seqValues[nProperty] >>= m_bTransparency;
			}
			break;
            case PROPERTYHDL_PRINTINGMODIFIESDOCUMENT:
            {
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "Invalid type" );
                seqValues[nProperty] >>= m_bModifyDocumentOnPrintingAllowed;
			}
			break;

        }
	}
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtPrintWarningOptions_Impl::~SvtPrintWarningOptions_Impl()
{
	if( IsModified() )
		Commit();
}

//*****************************************************************************************************************
//	Commit
//*****************************************************************************************************************
void SvtPrintWarningOptions_Impl::Commit()
{
	Sequence< OUString >	aSeqNames( impl_GetPropertyNames() );
	Sequence< Any >			aSeqValues( aSeqNames.getLength() );

	for( sal_Int32 nProperty = 0, nCount = aSeqNames.getLength(); nProperty < nCount; ++nProperty )
	{
        switch( nProperty )
        {
            case PROPERTYHANDLE_PAPERSIZE:
				aSeqValues[nProperty] <<= m_bPaperSize;
			break;

            case PROPERTYHANDLE_PAPERORIENTATION:
				aSeqValues[nProperty] <<= m_bPaperOrientation;
			break;

            case PROPERTYHANDLE_NOTFOUND:
				aSeqValues[nProperty] <<= m_bNotFound;
			break;

            case PROPERTYHANDLE_TRANSPARENCY:
				aSeqValues[nProperty] <<= m_bTransparency;
			break;
            case PROPERTYHDL_PRINTINGMODIFIESDOCUMENT:
                aSeqValues[nProperty] <<= m_bModifyDocumentOnPrintingAllowed;
			break;
        }
	}

	PutProperties( aSeqNames, aSeqValues );
}

void SvtPrintWarningOptions_Impl::Notify( const Sequence< rtl::OUString >&  )
{
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Sequence< OUString > SvtPrintWarningOptions_Impl::impl_GetPropertyNames()
{
	// Build static list of configuration key names.
	static const OUString pProperties[] =
	{
		PROPERTYNAME_PAPERSIZE,
		PROPERTYNAME_PAPERORIENTATION,
		PROPERTYNAME_NOTFOUND,
        PROPERTYNAME_TRANSPARENCY,
        PROPERTYNAME_PRINTINGMODIFIESDOCUMENT
	};

    // Initialize return sequence with these list ...
	static const Sequence< OUString > seqPropertyNames( pProperties, PROPERTYCOUNT );

	return seqPropertyNames;
}

//*****************************************************************************************************************
//	initialize static member
//	DON'T DO IT IN YOUR HEADER!
//	see definition for further informations
//*****************************************************************************************************************
SvtPrintWarningOptions_Impl*    SvtPrintWarningOptions::m_pDataContainer = NULL;
sal_Int32				        SvtPrintWarningOptions::m_nRefCount = 0;

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtPrintWarningOptions::SvtPrintWarningOptions()
{
    // Global access, must be guarded (multithreading!).
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Increase our refcount ...
	++m_nRefCount;
	// ... and initialize our data container only if it not already!
    if( m_pDataContainer == NULL )
	{
        m_pDataContainer = new SvtPrintWarningOptions_Impl();
		ItemHolder1::holdConfigItem(E_PRINTWARNINGOPTIONS);
	}
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtPrintWarningOptions::~SvtPrintWarningOptions()
{
    // Global access, must be guarded (multithreading!)
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Decrease our refcount.
	--m_nRefCount;
	// If last instance was deleted ...
	// we must destroy our static data container!
    if( m_nRefCount <= 0 )
	{
		delete m_pDataContainer;
		m_pDataContainer = NULL;
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtPrintWarningOptions::IsPaperSize() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsPaperSize();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtPrintWarningOptions::IsPaperOrientation() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsPaperOrientation();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtPrintWarningOptions::IsNotFound() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsNotFound();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtPrintWarningOptions::IsTransparency() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsTransparency();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtPrintWarningOptions::SetPaperSize( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetPaperSize( bState );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtPrintWarningOptions::SetPaperOrientation( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetPaperOrientation( bState );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtPrintWarningOptions::SetNotFound( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetNotFound( bState );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtPrintWarningOptions::SetTransparency( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetTransparency( bState );
}
// -----------------------------------------------------------------------------

sal_Bool SvtPrintWarningOptions::IsModifyDocumentOnPrintingAllowed() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsModifyDocumentOnPrintingAllowed();
}

// -----------------------------------------------------------------------------

void SvtPrintWarningOptions::SetModifyDocumentOnPrintingAllowed( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetModifyDocumentOnPrintingAllowed( bState ) ;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Mutex& SvtPrintWarningOptions::GetOwnStaticMutex()
{
	// Initialize static mutex only for one time!
    static Mutex* pMutex = NULL;
	// If these method first called (Mutex not already exist!) ...
    if( pMutex == NULL )
    {
		// ... we must create a new one. Protect follow code with the global mutex -
		// It must be - we create a static variable!
        MutexGuard aGuard( Mutex::getGlobalMutex() );
		// We must check our pointer again - because it can be that another instance of our class will be faster than these!
        if( pMutex == NULL )
        {
			// Create the new mutex and set it for return on static variable.
            static Mutex aMutex;
            pMutex = &aMutex;
        }
    }
	// Return new created or already existing mutex object.
    return *pMutex;
}
