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

#include <unotools/workingsetoptions.hxx>
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

#define	ROOTNODE_WORKINGSET				OUString(RTL_CONSTASCII_USTRINGPARAM("Office.Common/WorkingSet"))
#define	DEFAULT_WINDOWLIST				Sequence< OUString >()

#define	PROPERTYNAME_WINDOWLIST			OUString(RTL_CONSTASCII_USTRINGPARAM("WindowList"		))

#define	PROPERTYHANDLE_WINDOWLIST		0

#define	PROPERTYCOUNT					1

//_________________________________________________________________________________________________________________
//	private declarations!
//_________________________________________________________________________________________________________________

class SvtWorkingSetOptions_Impl : public ConfigItem
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------

	public:

		//---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------

		 SvtWorkingSetOptions_Impl();
		~SvtWorkingSetOptions_Impl();

		//---------------------------------------------------------------------------------------------------------
		//	overloaded methods of baseclass
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		called for notify of configmanager
			@descr		These method is called from the ConfigManager before application ends or from the
			 			PropertyChangeListener if the sub tree broadcasts changes. You must update your
						internal values.

			@seealso	baseclass ConfigItem

			@param		"seqPropertyNames" is the list of properties which should be updated.
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

    	virtual void Notify( const Sequence< OUString >& seqPropertyNames );

		/*-****************************************************************************************************//**
			@short		write changes to configuration
			@descr		These method writes the changed values into the sub tree
						and should always called in our destructor to guarantee consistency of config data.

			@seealso	baseclass ConfigItem

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

    	virtual void Commit();

		//---------------------------------------------------------------------------------------------------------
		//	public interface
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		access method to get internal values
			@descr		This method gives us a chance to regulate access to our internal values.
						It's not used in the moment - but it's possible for the future!

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		Sequence< OUString >	GetWindowList(												) const	;
		void					SetWindowList( const Sequence< OUString >&	seqWindowList	)		;

	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------

	private:

		/*-****************************************************************************************************//**
			@short		return list of key names of our configuration management which represent our module tree
			@descr		These methods return a static const list of key names. We need it to get needed values from our
						configuration management.

			@seealso	-

			@param		-
			@return		A list of needed configuration keys is returned.

			@onerror	-
		*//*-*****************************************************************************************************/

		static Sequence< OUString > GetPropertyNames();

	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------

	private:

		Sequence< OUString >	m_seqWindowList		;
};

//_________________________________________________________________________________________________________________
//	definitions
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtWorkingSetOptions_Impl::SvtWorkingSetOptions_Impl()
	// Init baseclasses first
    :	ConfigItem			( ROOTNODE_WORKINGSET	)
	// Init member then.
	,	m_seqWindowList		( DEFAULT_WINDOWLIST	)
{
	// Use our static list of configuration keys to get his values.
	Sequence< OUString >	seqNames	= GetPropertyNames	(			);
	Sequence< Any >			seqValues	= GetProperties		( seqNames	);

	// Safe impossible cases.
	// We need values from ALL configuration keys.
	// Follow assignment use order of values in relation to our list of key names!
	DBG_ASSERT( !(seqNames.getLength()!=seqValues.getLength()), "SvtWorkingSetOptions_Impl::SvtWorkingSetOptions_Impl()\nI miss some values of configuration keys!\n" );

	// Copy values from list in right order to our internal member.
	sal_Int32 nPropertyCount = seqValues.getLength();
	for( sal_Int32 nProperty=0; nProperty<nPropertyCount; ++nProperty )
	{
		// Safe impossible cases.
		// Check any for valid value.
		DBG_ASSERT( !(seqValues[nProperty].hasValue()==sal_False), "SvtWorkingSetOptions_Impl::SvtWorkingSetOptions_Impl()\nInvalid property value detected!\n" );
        switch( nProperty )
        {
            case PROPERTYHANDLE_WINDOWLIST		:	{
														DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_SEQUENCE), "SvtWorkingSetOptions_Impl::SvtWorkingSetOptions_Impl()\nWho has changed the value type of \"Office.Common\\WorkingSet\\WindowList\"?" );
														seqValues[nProperty] >>= m_seqWindowList;
													}
													break;
        }
	}

	// Enable notification mechanism of our baseclass.
	// We need it to get information about changes outside these class on our used configuration keys!
	EnableNotification( seqNames );
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtWorkingSetOptions_Impl::~SvtWorkingSetOptions_Impl()
{
	// We must save our current values .. if user forget it!
	if( IsModified() == sal_True )
	{
		Commit();
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtWorkingSetOptions_Impl::Notify( const Sequence< OUString >& seqPropertyNames )
{
	// Use given list of updated properties to get his values from configuration directly!
	Sequence< Any > seqValues = GetProperties( seqPropertyNames );
	// Safe impossible cases.
	// We need values from ALL notified configuration keys.
	DBG_ASSERT( !(seqPropertyNames.getLength()!=seqValues.getLength()), "SvtWorkingSetOptions_Impl::Notify()\nI miss some values of configuration keys!\n" );
	// Step over list of property names and get right value from coreesponding value list to set it on internal members!
	sal_Int32 nCount = seqPropertyNames.getLength();
	for( sal_Int32 nProperty=0; nProperty<nCount; ++nProperty )
	{
		if( seqPropertyNames[nProperty] == PROPERTYNAME_WINDOWLIST )
		{
			DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_SEQUENCE), "SvtWorkingSetOptions_Impl::Notify()\nWho has changed the value type of \"Office.Common\\WorkingSet\\WindowList\"?" );
			seqValues[nProperty] >>= m_seqWindowList;
		}
        #if OSL_DEBUG_LEVEL > 1
		else DBG_ASSERT( sal_False, "SvtWorkingSetOptions_Impl::Notify()\nUnkown property detected ... I can't handle these!\n" );
		#endif
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtWorkingSetOptions_Impl::Commit()
{
	// Get names of supported properties, create a list for values and copy current values to it.
	Sequence< OUString >	seqNames	= GetPropertyNames	();
	sal_Int32				nCount		= seqNames.getLength();
	Sequence< Any >			seqValues	( nCount );
	for( sal_Int32 nProperty=0; nProperty<nCount; ++nProperty )
	{
        switch( nProperty )
        {
            case PROPERTYHANDLE_WINDOWLIST		:	{
                										seqValues[nProperty] <<= m_seqWindowList;
													}
                									break;
        }
	}
	// Set properties in configuration.
	PutProperties( seqNames, seqValues );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Sequence< OUString > SvtWorkingSetOptions_Impl::GetWindowList() const
{
	return m_seqWindowList;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtWorkingSetOptions_Impl::SetWindowList( const Sequence< OUString >& seqWindowList )
{
	m_seqWindowList = seqWindowList;
	SetModified();
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Sequence< OUString > SvtWorkingSetOptions_Impl::GetPropertyNames()
{
	// Build static list of configuration key names.
	static const OUString pProperties[] =
	{
		PROPERTYNAME_WINDOWLIST	,
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
SvtWorkingSetOptions_Impl*	SvtWorkingSetOptions::m_pDataContainer	= NULL	;
sal_Int32					SvtWorkingSetOptions::m_nRefCount		= 0		;

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtWorkingSetOptions::SvtWorkingSetOptions()
{
    // Global access, must be guarded (multithreading!).
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Increase our refcount ...
	++m_nRefCount;
	// ... and initialize our data container only if it not already exist!
    if( m_pDataContainer == NULL )
	{
        m_pDataContainer = new SvtWorkingSetOptions_Impl;
		ItemHolder1::holdConfigItem(E_WORKINGSETOPTIONS);
	}
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtWorkingSetOptions::~SvtWorkingSetOptions()
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
Sequence< OUString > SvtWorkingSetOptions::GetWindowList() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetWindowList();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtWorkingSetOptions::SetWindowList( const Sequence< OUString >& seqWindowList )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetWindowList( seqWindowList );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Mutex& SvtWorkingSetOptions::GetOwnStaticMutex()
{
	// Initialize static mutex only for one time!
    static Mutex* pMutex = NULL;
	// If this method first called (Mutex not already exist!) ...
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
