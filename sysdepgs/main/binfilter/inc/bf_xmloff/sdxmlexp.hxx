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



#ifndef _SDXMLEXP_HXX
#define _SDXMLEXP_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_ 
#include <com/sun/star/container/XIndexContainer.hpp>
#endif

#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATOR_HPP_
#include <com/sun/star/task/XStatusIndicator.hpp>
#endif
namespace binfilter {

sal_uInt32 SdXMLExportDoc(
		const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel>& rMod, 
		const ::rtl::OUString& rFileName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler>& rHandler,
		const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >& rGrfContainer,
		::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator >& rStatusIndicator,
		BOOL bShowProgr, BOOL bIsDraw );

}//end of namespace binfilter
#endif	//  _SDXMLEXP_HXX

