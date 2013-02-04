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


namespace com { namespace sun { namespace star {
	namespace uno { template<class X> class Reference; }
	namespace frame { class XModel; }
	namespace task { class XStatusIndicator; }
	namespace lang { class XMultiServiceFactory; }
	namespace uno { class XInterface; }
	namespace xml {
		namespace sax { struct InputSource; } }
} } }

#ifndef SC_XMLWRAP_HXX
#define SC_XMLWRAP_HXX

#include <bf_svtools/bf_solar.h>

namespace binfilter {


class SvStorage;
class ScDocument;
class SfxMedium;
class ScMySharedData;

//STRIP008 namespace com { namespace sun { namespace star {
//STRIP008 	namespace uno { template<class X> class Reference; }
//STRIP008 	namespace frame { class XModel; }
//STRIP008 	namespace task { class XStatusIndicator; }
//STRIP008 	namespace lang { class XMultiServiceFactory; }
//STRIP008 	namespace uno { class XInterface; }
//STRIP008 	namespace xml {
//STRIP008 		namespace sax { struct InputSource; } }
//STRIP008 } } }

class ScXMLImportWrapper
{
	ScDocument&		rDoc;
	SfxMedium*		pMedium;
	SvStorage*		pStorage;

	::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator> GetStatusIndicator(
		::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& rModel);
	::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator> GetStatusIndicator();

	sal_uInt32 ImportFromComponent(::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& xServiceFactory,
		::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel>& xModel,
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& xXMLParser,
		::com::sun::star::xml::sax::InputSource& aParserInput,
		const ::rtl::OUString& sComponentName, const ::rtl::OUString& sDocName, const ::rtl::OUString& sOldDocName,
		::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any>& aArgs,
		sal_Bool bMustBeSuccessfull);

	sal_Bool ExportToComponent(::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& xServiceFactory,
		::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel>& xModel,
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& xWriter,
		::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& aDescriptor,
		const ::rtl::OUString& sName,	const ::rtl::OUString& sMediaType, const ::rtl::OUString& sComponentName,
		const sal_Bool bPlainText, ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any>& aArgs,
		ScMySharedData*& pSharedData);

public:
	ScXMLImportWrapper(ScDocument& rD, SfxMedium* pM, SvStorage* pS);
	BOOL Import(sal_Bool bStylesOnly);
	BOOL Export(sal_Bool bStylesOnly);
};

} //namespace binfilter
#endif

