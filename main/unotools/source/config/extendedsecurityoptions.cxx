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

//_________________________________________________________________________________________________________________
//	includes
//_________________________________________________________________________________________________________________

#include <unotools/extendedsecurityoptions.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/configitem.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <tools/urlobj.hxx>
#include <tools/wldcrd.hxx>
#include <rtl/ustrbuf.hxx>

#include <unotools/pathoptions.hxx>

#include <hash_map>

#include <rtl/logfile.hxx>
#include "itemholder1.hxx"

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

#define	ROOTNODE_SECURITY				OUString(RTL_CONSTASCII_USTRINGPARAM("Office.Security"))

#define SECURE_EXTENSIONS_SET			OUString(RTL_CONSTASCII_USTRINGPARAM("SecureExtensions"))
#define EXTENSION_PROPNAME				OUString(RTL_CONSTASCII_USTRINGPARAM("/Extension"))

#define PROPERTYNAME_HYPERLINKS_OPEN	OUString(RTL_CONSTASCII_USTRINGPARAM("Hyperlinks/Open"))

#define PROPERTYHANDLE_HYPERLINKS_OPEN	0

#define PROPERTYCOUNT                   1

//_________________________________________________________________________________________________________________
//	private declarations!
//_________________________________________________________________________________________________________________

struct OUStringHashCode
{
    size_t operator()( const ::rtl::OUString& sString ) const
	{
		return sString.hashCode();
	}
};

class ExtensionHashMap : public ::std::hash_map< ::rtl::OUString,
												 sal_Int32,
												 OUStringHashCode,
												 ::std::equal_to< ::rtl::OUString > >
{
	public:
		inline void free()
		{
			ExtensionHashMap().swap( *this );
		}
};

class SvtExtendedSecurityOptions_Impl : public ConfigItem
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------

	public:

		//---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------

		 SvtExtendedSecurityOptions_Impl();
		~SvtExtendedSecurityOptions_Impl();

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
			@short		Access method to check for security problems
			@descr		Different methods to check for security related problems.

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		sal_Bool										IsSecureHyperlink( const rtl::OUString& aURL ) const;
		Sequence< rtl::OUString >						GetSecureExtensionList() const;

		SvtExtendedSecurityOptions::OpenHyperlinkMode	GetOpenHyperlinkMode();
		void											SetOpenHyperlinkMode( SvtExtendedSecurityOptions::OpenHyperlinkMode aMode );
        sal_Bool                                        IsOpenHyperlinkModeReadOnly() const;

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

		/*-****************************************************************************************************//**
			@short		Fills the hash map with all extensions known to be secure
			@descr		These methods fills the given hash map object with all extensions known to be secure.

			@seealso	-

			@param		aHashMap
						A hash map to be filled with secure extension strings.
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/
		void FillExtensionHashMap( ExtensionHashMap& aHashMap );

	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------

	private:
		OUString										m_aSecureExtensionsSetName;
		OUString										m_aExtensionPropName;

		SvtExtendedSecurityOptions::OpenHyperlinkMode	m_eOpenHyperlinkMode;
        sal_Bool                                        m_bROOpenHyperlinkMode;
		ExtensionHashMap								m_aExtensionHashMap;
};

//_________________________________________________________________________________________________________________
//	definitions
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtExtendedSecurityOptions_Impl::SvtExtendedSecurityOptions_Impl()
	// Init baseclasses first
    :	ConfigItem			( ROOTNODE_SECURITY			),
	m_aSecureExtensionsSetName( SECURE_EXTENSIONS_SET ),
    m_aExtensionPropName( EXTENSION_PROPNAME ),
    m_bROOpenHyperlinkMode(sal_False)
	// Init member then.
{
	// Fill the extension hash map with all secure extension strings
	FillExtensionHashMap( m_aExtensionHashMap );

	Sequence< OUString >	seqNames	= GetPropertyNames();
	Sequence< Any >			seqValues	= GetProperties( seqNames );
    Sequence< sal_Bool >    seqRO       = GetReadOnlyStates ( seqNames  );

	sal_Int32 nPropertyCount = seqValues.getLength();
	for( sal_Int32 nProperty=0; nProperty<nPropertyCount; ++nProperty )
	{
		// Safe impossible cases.
		// Check any for valid value.
		DBG_ASSERT( !(seqValues[nProperty].hasValue()==sal_False), "SvtExtendedSecurityOptions_Impl::SvtExtendedSecurityOptions_Impl()\nInvalid property value detected!\n" );
        switch( nProperty )
		{
			case PROPERTYHANDLE_HYPERLINKS_OPEN:
			{
				DBG_ASSERT( ( seqValues[nProperty].getValueTypeClass() == TypeClass_LONG ), "SvtExtendedSecurityOptions_Impl::SvtExtendedSecurityOptions_Impl()\nWho has changed the value type of 'Hyperlink/Open'?" );

				sal_Int32 nMode = SvtExtendedSecurityOptions::OPEN_WITHSECURITYCHECK;
				if ( seqValues[nProperty] >>= nMode )
					m_eOpenHyperlinkMode = (SvtExtendedSecurityOptions::OpenHyperlinkMode)nMode;
				else {
					DBG_ERROR("Wrong type for Open mode!");
                }
                m_bROOpenHyperlinkMode = seqRO[nProperty];
			}
			break;
		}
	}

	// Enable notification mechanism of our baseclass.
	// We need it to get information about changes outside these class on our used configuration keys!
	Sequence< OUString > seqNotifyNames( 1 );
	seqNotifyNames[0] = m_aSecureExtensionsSetName;
	EnableNotification( seqNotifyNames );
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtExtendedSecurityOptions_Impl::~SvtExtendedSecurityOptions_Impl()
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
void SvtExtendedSecurityOptions_Impl::Notify( const Sequence< OUString >& )
{
	// Not implemented
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtExtendedSecurityOptions_Impl::Commit()
{
	// Get names of supported properties, create a list for values and copy current values to it.
	Sequence< OUString >	seqNames	= GetPropertyNames	();
	sal_Int32				nCount		= seqNames.getLength();
	Sequence< Any >			seqValues	( nCount );
	for( sal_Int32 nProperty=0; nProperty<nCount; ++nProperty )
	{
        switch( nProperty )
        {
            case PROPERTYHANDLE_HYPERLINKS_OPEN:	{
														seqValues[nProperty] <<= (sal_Int32)m_eOpenHyperlinkMode;
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
sal_Bool SvtExtendedSecurityOptions_Impl::IsSecureHyperlink( const OUString& aURL ) const
{
	INetURLObject aURLObject( aURL );

	String aExtension = aURLObject.getExtension();
	aExtension.ToLowerAscii();

	ExtensionHashMap::const_iterator pIter = m_aExtensionHashMap.find( aExtension );
	if ( pIter != m_aExtensionHashMap.end() )
		return sal_True;
	else
		return sal_False;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Sequence< OUString > SvtExtendedSecurityOptions_Impl::GetSecureExtensionList() const
{
	Sequence< OUString > aResult( m_aExtensionHashMap.size() );

	sal_Int32 nIndex = 0;
	for ( ExtensionHashMap::const_iterator pIter = m_aExtensionHashMap.begin();
			pIter != m_aExtensionHashMap.end(); pIter++ )
	{
		aResult[nIndex++] = pIter->first;
	}

	return aResult;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
SvtExtendedSecurityOptions::OpenHyperlinkMode SvtExtendedSecurityOptions_Impl::GetOpenHyperlinkMode()
{
	return m_eOpenHyperlinkMode;
}
/* -----------------09.07.2003 11:26-----------------

 --------------------------------------------------*/
sal_Bool SvtExtendedSecurityOptions_Impl::IsOpenHyperlinkModeReadOnly() const
{
    return m_bROOpenHyperlinkMode;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtExtendedSecurityOptions_Impl::SetOpenHyperlinkMode( SvtExtendedSecurityOptions::OpenHyperlinkMode eNewMode )
{
	m_eOpenHyperlinkMode = eNewMode;
	SetModified();
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
void SvtExtendedSecurityOptions_Impl::FillExtensionHashMap( ExtensionHashMap& aHashMap )
{
	// Get sequence with secure extensions from configuration
	Sequence< OUString >	seqNodes = GetNodeNames( m_aSecureExtensionsSetName );

	OUString				aValue;
	Sequence< Any >			aValues;
	Sequence< OUString >	aPropSeq( 1 );
	for ( int i = 0; i < seqNodes.getLength(); i++ )
	{
		// Create access name for property
		OUStringBuffer aExtEntryProp( m_aSecureExtensionsSetName );
		aExtEntryProp.appendAscii( "/" );
		aExtEntryProp.append( seqNodes[i] );
		aExtEntryProp.append( m_aExtensionPropName );

		aPropSeq[0] = aExtEntryProp.makeStringAndClear();
		aValues = GetProperties( aPropSeq );
		if ( aValues.getLength() == 1 )
		{
			// Don't use value if sequence has not the correct length
			if ( aValues[0] >>= aValue )
				// Add extension into secure extensions hash map
				aHashMap.insert( ExtensionHashMap::value_type( aValue.toAsciiLowerCase(), 1 ) );
			else
			{
				DBG_ERRORFILE( "SvtExtendedSecurityOptions_Impl::FillExtensionHashMap(): not string value?" );
			}
		}
	}
}

//*****************************************************************************************************************
//	private method (currently not used)
//*****************************************************************************************************************
Sequence< OUString > SvtExtendedSecurityOptions_Impl::GetPropertyNames()
{
	// Build static list of configuration key names.
	static const OUString pProperties[] =
	{
		PROPERTYNAME_HYPERLINKS_OPEN
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
SvtExtendedSecurityOptions_Impl*	SvtExtendedSecurityOptions::m_pDataContainer	= NULL	;
sal_Int32							SvtExtendedSecurityOptions::m_nRefCount			= 0		;

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtExtendedSecurityOptions::SvtExtendedSecurityOptions()
{
    // Global access, must be guarded (multithreading!).
    MutexGuard aGuard( GetInitMutex() );
	// Increase our refcount ...
	++m_nRefCount;
	// ... and initialize our data container only if it not already exist!
    if( m_pDataContainer == NULL )
	{
       RTL_LOGFILE_CONTEXT(aLog, "unotools ( ??? ) ::SvtExtendedSecurityOptions_Impl::ctor()");
       m_pDataContainer = new SvtExtendedSecurityOptions_Impl;

        ItemHolder1::holdConfigItem(E_EXTENDEDSECURITYOPTIONS);
	}
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtExtendedSecurityOptions::~SvtExtendedSecurityOptions()
{
    // Global access, must be guarded (multithreading!)
    MutexGuard aGuard( GetInitMutex() );
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
sal_Bool SvtExtendedSecurityOptions::IsSecureHyperlink( const rtl::OUString& aURL ) const
{
    MutexGuard aGuard( GetInitMutex() );
	return m_pDataContainer->IsSecureHyperlink( aURL );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Sequence< rtl::OUString > SvtExtendedSecurityOptions::GetSecureExtensionList() const
{
    MutexGuard aGuard( GetInitMutex() );
	return m_pDataContainer->GetSecureExtensionList();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
SvtExtendedSecurityOptions::OpenHyperlinkMode SvtExtendedSecurityOptions::GetOpenHyperlinkMode()
{
    MutexGuard aGuard( GetInitMutex() );
	return m_pDataContainer->GetOpenHyperlinkMode();
}
/* -----------------09.07.2003 11:26-----------------

 --------------------------------------------------*/
sal_Bool SvtExtendedSecurityOptions::IsOpenHyperlinkModeReadOnly() const
{
    return m_pDataContainer->IsOpenHyperlinkModeReadOnly();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtExtendedSecurityOptions::SetOpenHyperlinkMode( SvtExtendedSecurityOptions::OpenHyperlinkMode eMode )
{
    MutexGuard aGuard( GetInitMutex() );
	m_pDataContainer->SetOpenHyperlinkMode( eMode );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Mutex& SvtExtendedSecurityOptions::GetInitMutex()
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
