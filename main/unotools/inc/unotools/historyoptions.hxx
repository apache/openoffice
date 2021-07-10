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


#ifndef INCLUDED_SVTOOLS_HISTORYOPTIONS_HXX
#define INCLUDED_SVTOOLS_HISTORYOPTIONS_HXX

//_________________________________________________________________________________________________________________
//	includes
//_________________________________________________________________________________________________________________

#include "unotools/unotoolsdllapi.h"
#include <sal/types.h>
#include <osl/mutex.hxx>
#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <rtl/ustring.hxx>
#include <unotools/options.hxx>

//_________________________________________________________________________________________________________________
//	types, enums, ...
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
	@descr			The method GetList() returns a list of property values.
					Use follow defines to separate values by names.
*//*-*************************************************************************************************************/

#define	HISTORY_PROPERTYNAME_URL			::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("URL"		))
#define	HISTORY_PROPERTYNAME_FILTER			::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Filter"	))
#define	HISTORY_PROPERTYNAME_TITLE			::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Title"		))
#define	HISTORY_PROPERTYNAME_PASSWORD		::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Password"	))

/*-************************************************************************************************************//**
	@descr			You can use these enum values to specify right history if you call our interface methods.
*//*-*************************************************************************************************************/

enum EHistoryType
{
    ePICKLIST       = 0,
    eHISTORY        = 1,
    eHELPBOOKMARKS  = 2
};

//_________________________________________________________________________________________________________________
//	forward declarations
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
	@short			forward declaration to our private date container implementation
	@descr			We use these class as internal member to support small memory requirements.
					You can create the container if it is necessary. The class which use these mechanism
					is faster and smaller then a complete implementation!
*//*-*************************************************************************************************************/

class SvtHistoryOptions_Impl;

//_________________________________________________________________________________________________________________
//	declarations
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
	@short			collect informations about history features
	@descr          -

	@implements		-
	@base			-

	@devstatus		ready to use
*//*-*************************************************************************************************************/

class UNOTOOLS_DLLPUBLIC SvtHistoryOptions: public utl::detail::Options
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------

	public:

		//---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		standard constructor and destructor
			@descr		This will initialize an instance with default values.
						We implement these class with a refcount mechanism! Every instance of this class increase it
						at create and decrease it at delete time - but all instances use the same data container!
						He is implemented as a static member ...

			@seealso	member m_nRefCount
			@seealso	member m_pDataContainer

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

         SvtHistoryOptions();
        virtual ~SvtHistoryOptions();

		//---------------------------------------------------------------------------------------------------------
		//	interface
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		interface methods to get and set value of config key "org.openoffice.Office.Common/History/..."
			@descr		key "PickList"	: The last used documents displayed in the file menu.
						key "History"	: The last opened documents general.
		*//*-*****************************************************************************************************/

		/*-****************************************************************************************************//**
			@short		set/get max size of specified history
			@descr		Call this methods to get information about max. size of specified list.
						These value lay down the max count of items in these history. If a new one
						is add to it the oldest one is deleted automatically!

			@seealso	-

			@param		"eHistory" select right history.
			@param		"nSize" is the new max size of specified list. If new size smaller then the old one
						some oldest entries will be destroyed automatically!
			@return		Current max size of specified list.

			@onerror	-
		*//*-*****************************************************************************************************/

		sal_uInt32	GetSize( EHistoryType eHistory						) const	;
		void		SetSize( EHistoryType eHistory, sal_uInt32 nSize	)		;

		/*-****************************************************************************************************//**
			@short		clear complete sepcified list
			@descr		Call this methods to clear the whole list. After that GetItemCount() will return 0 ...
						but GetSize() will return the old value!

			@seealso	-

			@param		"eHistory" select right history.
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		void Clear( EHistoryType eHistory );

		/*-****************************************************************************************************//**
			@short		return complete sepcified list
			@descr		If you will show the whole list call this method to get it completly.

			@seealso	-

			@param		"eHistory" select right history.
			@return		A list of history items is returned.

			@onerror	-
		*//*-*****************************************************************************************************/

		::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > > GetList( EHistoryType eHistory ) const ;

		/*-****************************************************************************************************//**
			@short		append a new item to specified list
			@descr		You can append items to a list only - removing isn't allowed for a special item!
						The oldest entry is deleted automatically if max size arrived or you can call Clear() ...
						It exist two different overload methods to do this.
						One for user which have an complete history item and another one for uncompletly data sets!

			@seealso	method SetSize()
			@seealso	method Clear()

			@param		"eHistory" select right history.
			@param		"sURL" URL to save in history
			@param		"sFilter" filter name to save in history
			@param		"sTitle" document title to save in history
			@param		"sPassword" password to save in history
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		void AppendItem(			EHistoryType		eHistory	,
							const	::rtl::OUString&	sURL		,
							const	::rtl::OUString&	sFilter		,
							const	::rtl::OUString&	sTitle		,
							const	::rtl::OUString&	sPassword	);

	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------

	private:

		/*-****************************************************************************************************//**
			@short		return a reference to a static mutex
			@descr		Make these class threadsafe.

			@seealso	-

			@param		-
			@return		A reference to a static mutex member.

			@onerror	-
		*//*-*****************************************************************************************************/

        UNOTOOLS_DLLPRIVATE static ::osl::Mutex& GetOwnStaticMutex();

	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------

	private:

		/*Attention

			Don't initialize these static member in these header!
			a) Double dfined symbols will be detected ...
			b) and unresolved externals exist at linking time.
			Do it in your source only.
		 */

    	static SvtHistoryOptions_Impl*	m_pDataContainer	;	/// impl. data container as dynamic pointer for smaller memory requirements!
		static sal_Int32				m_nRefCount			;	/// internal ref count mechanism

};		// class SvtHistoryOptions

#endif	// #ifndef INCLUDED_SVTOOLS_HISTORYOPTIONS_HXX
