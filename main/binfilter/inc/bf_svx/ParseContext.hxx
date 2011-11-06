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


#ifndef SVX_QUERYDESIGNCONTEXT_HXX
#define SVX_QUERYDESIGNCONTEXT_HXX

#ifndef CONNECTIVITY_IPARSECONTEXT_HXX
#include <connectivity/IParseContext.hxx>
#endif

#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX 
#include <unotools/localedatawrapper.hxx>
#endif
namespace binfilter {

namespace svxform
{
	//==========================================================================
	//= OSystemParseContext
	//==========================================================================
	class OSystemParseContext : public ::connectivity::IParseContext
	{
	private:
		ByteString m_aSQLInternationals;

	public:
		OSystemParseContext();

		virtual ~OSystemParseContext();
		// retrieves language specific error messages
		virtual ::rtl::OUString getErrorMessage(ErrorCode _eCodes) const;

		// retrieves language specific keyword strings (only ASCII allowed)
		virtual ::rtl::OString getIntlKeywordAscii(InternationalKeyCode _eKey) const;

		// finds out, if we have an international keyword (only ASCII allowed)
		virtual InternationalKeyCode getIntlKeyCode(const ::rtl::OString& rToken) const;

		/** get's a locale instance which should be used when parsing in the context specified by this instance
			<p>if this is not overridden by derived classes, it returns the static default locale.</p>
		*/
		virtual ::com::sun::star::lang::Locale getPreferredLocale( ) const;

	public:
		// helper methods to ease access to some of the characteristics of the locale
		sal_Unicode getNumDecimalSep( ) const;
		sal_Unicode getNumThousandSep( ) const;
	};

	//==========================================================================
	//= OParseContextClient
	//==========================================================================
	/** helper class which needs access to a (shared and ref-counted) OSystemParseContext
		instance.
	*/
	class OParseContextClient
	{
	protected:
		OParseContextClient();
		virtual ~OParseContextClient();
	};
}
}//end of namespace binfilter
#endif // SVX_QUERYDESIGNCONTEXT_HXX

