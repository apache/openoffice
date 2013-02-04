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


#ifndef _GLOSDOC_HXX
#define _GLOSDOC_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif


#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif
#ifndef _COM_SUN_STAR_TEXT_XAUTOTEXTGROUP_HPP_
#include <com/sun/star/text/XAutoTextGroup.hpp>
#endif
class SvStrings; 
namespace binfilter {//STRIP009
class SwTextBlocks;

class SwDocShell;
} //namespace binfilter

#ifndef SW_DECL_SWDOCSHELL_DEFINED
#define SW_DECL_SWDOCSHELL_DEFINED
#ifndef _REF_HXX
#include <tools/ref.hxx>
#endif namespace 
namespace binfilter {//STRIP009
SV_DECL_REF( SwDocShell )
} //namespace binfilter
#endif

#ifndef _CPPUHELPER_WEAKREF_HXX_ 
#include <cppuhelper/weakref.hxx>
#endif

#include <vector>
namespace binfilter {

typedef ::com::sun::star::uno::WeakReference< ::com::sun::star::text::XAutoTextGroup > AutoTextGroupRef;
typedef ::std::vector< AutoTextGroupRef > UnoAutoTextGroups;

typedef ::com::sun::star::uno::Reference< ::com::sun::star::text::XAutoTextEntry > AutoTextEntryRef;
typedef ::std::vector< AutoTextEntryRef > UnoAutoTextEntries;

#define GLOS_DELIM (sal_Unicode)'*'

// CLASS -----------------------------------------------------------------
class SwGlossaries
{
	UnoAutoTextGroups		aGlossaryGroups;
	UnoAutoTextEntries		aGlossaryEntries;
	String					aPath;
	String 					sOldErrPath;
	String					sErrPath;
	SvStrings				*pPathArr;
	SvStrings				*pGlosArr;
	BOOL					bError;
	SwTextBlocks*	GetGlosDoc(const String &rName, BOOL bCreate = TRUE) const{DBG_BF_ASSERT(0, "STRIP"); return NULL;}; //STRIP001 //STRIP001 	SwTextBlocks*	GetGlosDoc(const String &rName, BOOL bCreate = TRUE) const;
	SvStrings	    *GetNameList();
	void RemoveFileFromList( const String& rGroup );
	void InvalidateUNOOjects();

public:
	SwGlossaries();
	~SwGlossaries();

	/** returns the cached AutoTextGroup (if any) for the given group name

		@precond
			If <arg>_bCreate</arg> is <TRUE/>, the SolarMutex must be locked when calling into this method.

		@param _rGroupName
			the name of the glossaries group
		@param _bCreate
			if <TRUE/>, the group is created if it does not yet exist
	*/
	::com::sun::star::uno::Reference< ::com::sun::star::text::XAutoTextGroup >
							GetAutoTextGroup(
								const ::rtl::OUString& _rGroupName,
								bool _bCreate = false
							);

	/** returns the cached AutoTextEntry (if any) for the given group/with the given name

		@precond
			If <arg>_bCreate</arg> is <TRUE/>, the SolarMutex must be locked when calling into this method.

		@param _rGroupAccessName
			the name to access the group
		@param _rGroupName
			the name of the glossaries group, as to be passed to the entry
		@param _rEntryName
			the name of the auto text entry
		@param _bCreate
			if <TRUE/>, the entry is created if it does not yet exist
	*/
	::com::sun::star::uno::Reference< ::com::sun::star::text::XAutoTextEntry >
							GetAutoTextEntry(
								const String& _rCompleteGroupName,
								const ::rtl::OUString& _rGroupName,
								const ::rtl::OUString& _rEntryName,
								bool _bCreate = false
							);

	USHORT					GetGroupCnt();
	String					GetGroupName(USHORT	);
	String					GetGroupTitle( const String& rGroupName );

	BOOL 			FindGroupName(String & rGroup);

	SwTextBlocks*	GetGroupDoc(const String &rName,
								BOOL bCreate = FALSE) const;
	SwTextBlocks*	GetDefGroupDoc() const {return GetGroupDoc(GetDefName());}
	void			PutGroupDoc(SwTextBlocks *pBlock);
	static String 	GetDefName();
	static String	GetExtension();
 
	BOOL			NewGroupDoc(String &rGroupName, const String& rTitle);
	BOOL			RenameGroupDoc(const String& sOldGroup, String& sNewGroup, const String& rNewTitle);
	BOOL			DelGroupDoc(const String &);
	SwDocShellRef	EditGroupDoc(const String &rGrpName, const String& rShortName, BOOL bShow = TRUE );
	void			UpdateGlosPath(BOOL bFull);
};


} //namespace binfilter
#endif // _GLOSDOC_HXX
