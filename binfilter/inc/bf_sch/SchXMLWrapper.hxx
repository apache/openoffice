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


#ifndef _SCH_XMLWRP_HXX_
#define _SCH_XMLWRP_HXX_

#ifndef _SVSTOR_HXX
#include <bf_so3/svstor.hxx>
#endif
namespace com { namespace sun { namespace star {
	namespace xml {
		namespace sax {
			class XParser;
		}
	}
	namespace document {
		class XGraphicObjectResolver;
	}
	namespace frame {
		class XModel;
	}
	namespace task {
		class XStatusIndicator;
	}
	namespace lang {
		class XMultiServiceFactory;
	}
    namespace io {
        class XActiveDataSource;
    }
}}}
namespace binfilter {

//STRIP008 namespace com { namespace sun { namespace star {
//STRIP008 	namespace xml {
//STRIP008 		namespace sax {
//STRIP008 			class XParser;
//STRIP008 		}
//STRIP008 	}
//STRIP008 	namespace document {
//STRIP008 		class XGraphicObjectResolver;
//STRIP008 	}
//STRIP008 	namespace frame {
//STRIP008 		class XModel;
//STRIP008 	}
//STRIP008 	namespace task {
//STRIP008 		class XStatusIndicator;
//STRIP008 	}
//STRIP008 	namespace lang {
//STRIP008 		class XMultiServiceFactory;
//STRIP008 	}
//STRIP008     namespace io {
//STRIP008         class XActiveDataSource;
//STRIP008     }
//STRIP008 }}}

class SchXMLWrapper
{
	::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > mxModel;
	::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator > mxStatusIndicator;
	SvStorage& mrStorage;

	sal_Bool mbIsDraw : 1;
	sal_Bool mbShowProgress : 1;

	sal_Bool ExportStream(
		const ::rtl::OUString& rsStreamName,
		const ::rtl::OUString& rsServiceName,
		::com::sun::star::uno::Reference< ::com::sun::star::io::XActiveDataSource >& xDataSource,
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& xServiceFactory,
		::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArgs );

public:
	SchXMLWrapper( ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& xModel,
				   SvStorage& rStorage,
				   sal_Bool bShowProgress = TRUE );

	sal_Int32 Import();
	sal_Bool Export();
};

} //namespace binfilter
#endif	// _SCH_XMLWRP_HXX_
