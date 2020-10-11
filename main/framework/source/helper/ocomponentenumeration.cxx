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
#include "precompiled_framework.hxx"

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________
#include <helper/ocomponentenumeration.hxx>

#ifndef _FRAMEWORK_THREADHELP_RESETABLEGUARD_HXX_
#include <threadhelp/resetableguard.hxx>
#endif

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	includes of other projects
//_________________________________________________________________________________________________________________
#include <vcl/svapp.hxx>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace framework{

using namespace ::com::sun::star::container		;
using namespace ::com::sun::star::lang			;
using namespace ::com::sun::star::uno			;
using namespace ::cppu							;
using namespace ::osl							;
using namespace ::rtl							;

//_________________________________________________________________________________________________________________
//	non exported const
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	non exported definitions
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	declarations
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
OComponentEnumeration::OComponentEnumeration( const Sequence< css::uno::Reference< XComponent > >& seqComponents )
		//	Init baseclasses first
		//	Attention:
		//		Don't change order of initialization!
		//      ThreadHelpBase is a struct with a mutex as member. We can't use a mutex as member, while
		//		we must garant right initialization and a valid value of this! First initialize
		//		baseclasses and then members. And we need the mutex for other baseclasses !!!
        :   ThreadHelpBase  ( &Application::GetSolarMutex() )
		// Init member
        ,   m_nPosition     ( 0                             )   // 0 is the first position for a valid list and the right value for an invalid list to!
        ,   m_seqComponents ( seqComponents                 )
{
	// Safe impossible states
	// "Method" not defined for ALL parameters!
	LOG_ASSERT( impldbg_checkParameter_OComponentEnumerationCtor( seqComponents ), "OComponentEnumeration::OComponentEnumeration()\nInvalid parameter detected!\n" )
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
OComponentEnumeration::~OComponentEnumeration()
{
	// Reset instance, free memory ....
	impl_resetObject();
}

//*****************************************************************************************************************
//	XEventListener
//*****************************************************************************************************************
void SAL_CALL OComponentEnumeration::disposing( const EventObject&
#if OSL_DEBUG_LEVEL > 0
aEvent
#endif
) throw( RuntimeException )
{
	// Ready for multithreading
	ResetableGuard aGuard( m_aLock );

	// Safe impossible cases
	// This method is not specified for all incoming parameters.
	LOG_ASSERT( impldbg_checkParameter_disposing( aEvent ), "OComponentEnumeration::disposing()\nInvalid parameter detected!\n" )

	// Reset instance to defaults, release references and free memory.
	impl_resetObject();
}

//*****************************************************************************************************************
//	XEnumeration
//*****************************************************************************************************************
sal_Bool SAL_CALL OComponentEnumeration::hasMoreElements() throw( RuntimeException )
{
	// Ready for multithreading
	ResetableGuard aGuard( m_aLock );

	// First position in a valid list is 0.
	// => The last one is getLength() - 1!
	// m_nPosition's current value is the position for the next element, which will be return, if user call "nextElement()"
	// => We have more elements if current position less then the length of the list!
	return ( m_nPosition < (sal_uInt32)(m_seqComponents.getLength()) );
}

//*****************************************************************************************************************
//	XEnumeration
//*****************************************************************************************************************
Any SAL_CALL OComponentEnumeration::nextElement() throw(	NoSuchElementException	,
										 					WrappedTargetException	,
															RuntimeException		)
{
	// Ready for multithreading
	ResetableGuard aGuard( m_aLock );

	// If we have no elements or end of enumeration is arrived ...
	if ( hasMoreElements() == sal_False )
	{
		// .. throw an exception!
		throw NoSuchElementException();
	}

	// Else; Get next element from list ...
	Any aComponent;
	aComponent <<= m_seqComponents[m_nPosition];
	// ... and step to next element!
	++m_nPosition;

	// Return listitem.
	return aComponent;
}

//*****************************************************************************************************************
//	proteced method
//*****************************************************************************************************************
void OComponentEnumeration::impl_resetObject()
{
	// Attention:
	// Write this for multiple calls - NOT AT THE SAME TIME - but for more then one call again)!
	// It exist two ways to call this method. From destructor and from disposing().
	// I can't say, which one is the first. Normally the disposing-call - but other way ....

	// Delete list of components.
	m_seqComponents.realloc( 0 );
	// Reset position in list.
	// The list has no elements anymore. m_nPosition is normally the current position in list for nextElement!
	// But a position of 0 in a list of 0 items is an invalid state. This constellation can't work in future.
	// End of enumeration is arrived!
	// (see hasMoreElements() for more details...)
	m_nPosition = 0 ;
}

//_________________________________________________________________________________________________________________
//	debug methods
//_________________________________________________________________________________________________________________

/*-----------------------------------------------------------------------------------------------------------------
	The follow methods checks the parameter for other functions. If a parameter or his value is non valid,
	we return "sal_False". (else sal_True) This mechanism is used to throw an ASSERT!

	ATTENTION

		If you miss a test for one of this parameters, contact the author or add it himself !(?)
		But ... look for right testing! See using of this methods!
-----------------------------------------------------------------------------------------------------------------*/

#ifdef ENABLE_ASSERTIONS

//*****************************************************************************************************************
// An empty list is allowed ... hasMoreElements() will return false then!
sal_Bool OComponentEnumeration::impldbg_checkParameter_OComponentEnumerationCtor( const Sequence< css::uno::Reference< XComponent > >& seqComponents )
{
	// Set default return value.
	sal_Bool bOK = sal_True;
	// Check parameter.
	if	(
			( &seqComponents == NULL )
		)
	{
		bOK = sal_False ;
	}
	// Return result of check.
	return bOK ;
}

//*****************************************************************************************************************
sal_Bool OComponentEnumeration::impldbg_checkParameter_disposing( const EventObject& aEvent )
{
	// Set default return value.
	sal_Bool bOK = sal_True;
	// Check parameter.
	if	(
			( &aEvent				==	NULL		)	||
			( aEvent.Source.is()	==	sal_False	)
		)
	{
		bOK = sal_False ;
	}
	// Return result of check.
	return bOK ;
}

#endif	//	#ifdef ENABLE_ASSERTIONS

}		//	namespace framework
