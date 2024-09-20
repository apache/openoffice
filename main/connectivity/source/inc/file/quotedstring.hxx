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



#ifndef CONNECTIVITY_QUOTED_STRING_HXX
#define CONNECTIVITY_QUOTED_STRING_HXX

#include <tools/string.hxx>
#include "file/filedllapi.hxx"

namespace connectivity
{
    //==================================================================
	// Derivation of string with overloaded GetToken/GetTokenCount methods
	// Especially for FLAT FILE format: Strings can be quoted
	//==================================================================
	class OOO_DLLPUBLIC_FILE QuotedTokenizedString
	{
        ::rtl::OUString m_sString;
	public:
        QuotedTokenizedString() {}
        QuotedTokenizedString(const ::rtl::OUString& _sString) : m_sString(_sString){}

        sal_Int32 GetTokenCount( sal_Unicode cTok , sal_Unicode cStrDel ) const;
		void GetTokenSpecial( ::rtl::OUString* _rStr, sal_Int32& nStartPos, sal_Unicode cTok = ';', sal_Unicode cStrDel = '\0' ) const;
        inline ::rtl::OUString& GetString() { return m_sString; }
        inline sal_Int32 Len() const { return m_sString.getLength(); }
        inline operator ::rtl::OUString&() { return m_sString; }
	};
}

#endif // CONNECTIVITY_QUOTED_STRING_HXX
