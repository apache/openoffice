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

#include <unotools/cacheoptions.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/configitem.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>

//_________________________________________________________________________________________________________________
//	namespaces
//_________________________________________________________________________________________________________________

using namespace ::utl;
using namespace ::rtl;
using namespace ::osl;
using namespace ::com::sun::star::uno;

//_________________________________________________________________________________________________________________
//	const
//_________________________________________________________________________________________________________________

#define	ROOTNODE_START			   			OUString(RTL_CONSTASCII_USTRINGPARAM("Office.Common/Cache"	))
#define	DEFAULT_WRITEROLE					20
#define	DEFAULT_DRAWINGOLE					20
#define	DEFAULT_GRFMGR_TOTALSIZE			10000000
#define	DEFAULT_GRFMGR_OBJECTSIZE			2400000
#define	DEFAULT_GRFMGR_OBJECTRELEASE	    600

#define	PROPERTYNAME_WRITEROLE				OUString(RTL_CONSTASCII_USTRINGPARAM("Writer/OLE_Objects"))
#define	PROPERTYNAME_DRAWINGOLE				OUString(RTL_CONSTASCII_USTRINGPARAM("DrawingEngine/OLE_Objects"))
#define	PROPERTYNAME_GRFMGR_TOTALSIZE		OUString(RTL_CONSTASCII_USTRINGPARAM("GraphicManager/TotalCacheSize"))
#define	PROPERTYNAME_GRFMGR_OBJECTSIZE		OUString(RTL_CONSTASCII_USTRINGPARAM("GraphicManager/ObjectCacheSize"))
#define PROPERTYNAME_GRFMGR_OBJECTRELEASE   OUString(RTL_CONSTASCII_USTRINGPARAM("GraphicManager/ObjectReleaseTime"))

#define	PROPERTYHANDLE_WRITEROLE			0
#define	PROPERTYHANDLE_DRAWINGOLE			1
#define	PROPERTYHANDLE_GRFMGR_TOTALSIZE		2
#define	PROPERTYHANDLE_GRFMGR_OBJECTSIZE	3
#define PROPERTYHANDLE_GRFMGR_OBJECTRELEASE 4

#define	PROPERTYCOUNT						5

class SvtCacheOptions_Impl : public ConfigItem
{
public:

//---------------------------------------------------------------------------------------------------------
//	constructor / destructor
//---------------------------------------------------------------------------------------------------------

	 SvtCacheOptions_Impl();
	~SvtCacheOptions_Impl();

//---------------------------------------------------------------------------------------------------------
//	overloaded methods of baseclass
//---------------------------------------------------------------------------------------------------------

	virtual void	Commit();
    virtual void    Notify( const com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );

//---------------------------------------------------------------------------------------------------------
//	public interface
//---------------------------------------------------------------------------------------------------------

	sal_Int32		GetWriterOLE_Objects() const;
	sal_Int32		GetDrawingEngineOLE_Objects() const;
	sal_Int32		GetGraphicManagerTotalCacheSize() const;
	sal_Int32		GetGraphicManagerObjectCacheSize() const;
	sal_Int32		GetGraphicManagerObjectReleaseTime() const;

	void			SetWriterOLE_Objects( sal_Int32 nObjects );
	void			SetDrawingEngineOLE_Objects( sal_Int32 nObjects );
	void			SetGraphicManagerTotalCacheSize( sal_Int32 nTotalCacheSize );
	void			SetGraphicManagerObjectCacheSize( sal_Int32 nObjectCacheSize );
	void            SetGraphicManagerObjectReleaseTime( sal_Int32 nReleaseTimeSeconds );

//-------------------------------------------------------------------------------------------------------------
//	private methods
//-------------------------------------------------------------------------------------------------------------

private:

	static Sequence< OUString > impl_GetPropertyNames();

//-------------------------------------------------------------------------------------------------------------
//	private member
//-------------------------------------------------------------------------------------------------------------

private:

		sal_Int32	mnWriterOLE;
		sal_Int32	mnDrawingOLE;
		sal_Int32	mnGrfMgrTotalSize;
		sal_Int32	mnGrfMgrObjectSize;
        sal_Int32   mnGrfMgrObjectRelease;
};

//_________________________________________________________________________________________________________________
//	definitions
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtCacheOptions_Impl::SvtCacheOptions_Impl() :
	ConfigItem( ROOTNODE_START	),
	mnWriterOLE( DEFAULT_WRITEROLE ),
	mnDrawingOLE( DEFAULT_DRAWINGOLE ),
	mnGrfMgrTotalSize( DEFAULT_GRFMGR_TOTALSIZE ),
	mnGrfMgrObjectSize( DEFAULT_GRFMGR_OBJECTSIZE ),
    mnGrfMgrObjectRelease( DEFAULT_GRFMGR_OBJECTRELEASE )
{
	Sequence< OUString >	seqNames( impl_GetPropertyNames() );
	Sequence< Any >			seqValues	= GetProperties( seqNames )	;

	DBG_ASSERT( !(seqNames.getLength()!=seqValues.getLength()), "SvtCacheOptions_Impl::SvtCacheOptions_Impl()\nI miss some values of configuration keys!\n" );

	// Copy values from list in right order to our internal member.
	sal_Int32 nPropertyCount = seqValues.getLength();
	sal_Int32 nProperty	= 0;

	for( nProperty=0; nProperty<nPropertyCount; ++nProperty )
	{
		if( seqValues[ nProperty ].hasValue() )
		{
			switch( nProperty )
			{
				case PROPERTYHANDLE_WRITEROLE:
				{
					if( seqValues[ nProperty ].getValueTypeClass() == TypeClass_LONG )
						seqValues[nProperty] >>= mnWriterOLE;
				}
				break;

				case PROPERTYHANDLE_DRAWINGOLE:
				{
					if( seqValues[ nProperty ].getValueTypeClass() == TypeClass_LONG )
						seqValues[nProperty] >>= mnDrawingOLE;
				}
				break;

				case PROPERTYHANDLE_GRFMGR_TOTALSIZE:
				{
					if( seqValues[ nProperty ].getValueTypeClass() == TypeClass_LONG )
						seqValues[nProperty] >>= mnGrfMgrTotalSize;
				}
				break;

				case PROPERTYHANDLE_GRFMGR_OBJECTSIZE:
				{
					if( seqValues[ nProperty ].getValueTypeClass() == TypeClass_LONG )
						seqValues[nProperty] >>= mnGrfMgrObjectSize;
				}
				break;

				case PROPERTYHANDLE_GRFMGR_OBJECTRELEASE:
				{
					if( seqValues[ nProperty ].getValueTypeClass() == TypeClass_LONG )
						seqValues[nProperty] >>= mnGrfMgrObjectRelease;
				}
				break;
			}
		}
	}
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtCacheOptions_Impl::~SvtCacheOptions_Impl()
{
	if( IsModified() )
		Commit();
}

//*****************************************************************************************************************
//	Commit
//*****************************************************************************************************************
void SvtCacheOptions_Impl::Commit()
{
	Sequence< OUString >	aSeqNames( impl_GetPropertyNames() );
	Sequence< Any >			aSeqValues( aSeqNames.getLength() );

	for( sal_Int32 nProperty = 0, nCount = aSeqNames.getLength(); nProperty < nCount; ++nProperty )
	{
        switch( nProperty )
        {
            case PROPERTYHANDLE_WRITEROLE:
				aSeqValues[nProperty] <<= mnWriterOLE;
			break;

            case PROPERTYHANDLE_DRAWINGOLE:
				aSeqValues[nProperty] <<= mnDrawingOLE;
			break;

            case PROPERTYHANDLE_GRFMGR_TOTALSIZE:
				aSeqValues[nProperty] <<= mnGrfMgrTotalSize;
			break;

            case PROPERTYHANDLE_GRFMGR_OBJECTSIZE:
				aSeqValues[nProperty] <<= mnGrfMgrObjectSize;
			break;

            case PROPERTYHANDLE_GRFMGR_OBJECTRELEASE:
				aSeqValues[nProperty] <<= mnGrfMgrObjectRelease;
			break;
        }
	}

	PutProperties( aSeqNames, aSeqValues );
}

void SvtCacheOptions_Impl::Notify( const Sequence< rtl::OUString >&  )
{
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Int32 SvtCacheOptions_Impl::GetWriterOLE_Objects() const
{
	return mnWriterOLE;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Int32 SvtCacheOptions_Impl::GetDrawingEngineOLE_Objects() const
{
	return mnDrawingOLE;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Int32 SvtCacheOptions_Impl::GetGraphicManagerTotalCacheSize() const
{
	return mnGrfMgrTotalSize;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Int32 SvtCacheOptions_Impl::GetGraphicManagerObjectCacheSize() const
{
	return mnGrfMgrObjectSize;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Int32 SvtCacheOptions_Impl::GetGraphicManagerObjectReleaseTime() const
{
	return mnGrfMgrObjectRelease;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCacheOptions_Impl::SetWriterOLE_Objects( sal_Int32 nWriterOLE )
{
	mnWriterOLE = nWriterOLE;
	SetModified();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCacheOptions_Impl::SetDrawingEngineOLE_Objects( sal_Int32 nDrawingOLE )
{
	mnDrawingOLE = nDrawingOLE;
	SetModified();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCacheOptions_Impl::SetGraphicManagerTotalCacheSize( sal_Int32 nGrfMgrTotalSize )
{
	mnGrfMgrTotalSize = nGrfMgrTotalSize;
	SetModified();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCacheOptions_Impl::SetGraphicManagerObjectCacheSize( sal_Int32 nGrfMgrObjectSize )
{
	mnGrfMgrObjectSize = nGrfMgrObjectSize;
	SetModified();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCacheOptions_Impl::SetGraphicManagerObjectReleaseTime( sal_Int32 nGrfMgrObjectReleaseTime )
{
	mnGrfMgrObjectRelease = nGrfMgrObjectReleaseTime;
	SetModified();
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Sequence< OUString > SvtCacheOptions_Impl::impl_GetPropertyNames()
{
	// Build static list of configuration key names.
	static const OUString pProperties[] =
	{
		PROPERTYNAME_WRITEROLE,
		PROPERTYNAME_DRAWINGOLE,
		PROPERTYNAME_GRFMGR_TOTALSIZE,
		PROPERTYNAME_GRFMGR_OBJECTSIZE,
		PROPERTYNAME_GRFMGR_OBJECTRELEASE
	};
	// Initialize return sequence with these list ...
	static const Sequence< OUString > seqPropertyNames( pProperties, PROPERTYCOUNT );
	// ... and return it.
	return seqPropertyNames;
}

//*****************************************************************************************************************
//	initialize static member
//	DON'T DO IT IN YOUR HEADER!
//	see definition for further informations
//*****************************************************************************************************************
SvtCacheOptions_Impl*	SvtCacheOptions::m_pDataContainer = NULL;
sal_Int32				SvtCacheOptions::m_nRefCount = 0;

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtCacheOptions::SvtCacheOptions()
{
    // Global access, must be guarded (multithreading!).
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Increase our refcount ...
	++m_nRefCount;
	// ... and initialize our data container only if it is not already!
    if( m_pDataContainer == NULL )
	{
        m_pDataContainer = new SvtCacheOptions_Impl();
	}
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtCacheOptions::~SvtCacheOptions()
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
sal_Int32 SvtCacheOptions::GetWriterOLE_Objects() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetWriterOLE_Objects();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Int32 SvtCacheOptions::GetDrawingEngineOLE_Objects() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetDrawingEngineOLE_Objects();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Int32 SvtCacheOptions::GetGraphicManagerTotalCacheSize() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetGraphicManagerTotalCacheSize();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Int32 SvtCacheOptions::GetGraphicManagerObjectCacheSize() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetGraphicManagerObjectCacheSize();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Int32 SvtCacheOptions::GetGraphicManagerObjectReleaseTime() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetGraphicManagerObjectReleaseTime();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCacheOptions::SetWriterOLE_Objects( sal_Int32 nWriterOLE )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetWriterOLE_Objects( nWriterOLE );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCacheOptions::SetDrawingEngineOLE_Objects( sal_Int32 nDrawingOLE )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetDrawingEngineOLE_Objects( nDrawingOLE );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCacheOptions::SetGraphicManagerTotalCacheSize( sal_Int32 nGrfMgrTotalSize )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetGraphicManagerTotalCacheSize( nGrfMgrTotalSize );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCacheOptions::SetGraphicManagerObjectCacheSize( sal_Int32 nGrfMgrObjectSize )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetGraphicManagerObjectCacheSize( nGrfMgrObjectSize );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCacheOptions::SetGraphicManagerObjectReleaseTime( sal_Int32 nGrfMgrObjectReleaseTime )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetGraphicManagerObjectReleaseTime( nGrfMgrObjectReleaseTime );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Mutex& SvtCacheOptions::GetOwnStaticMutex()
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
