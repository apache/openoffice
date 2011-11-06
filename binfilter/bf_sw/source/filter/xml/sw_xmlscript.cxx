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




#include <hintids.hxx>

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XDOCUMENTINFOSUPPLIER_HPP_
#include <com/sun/star/document/XDocumentInfoSupplier.hpp>
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include <bf_xmloff/xmlnmspe.hxx>
#endif

#ifndef _XMLOFF_XMLMETAI_HXX
#include <bf_xmloff/xmlscripti.hxx>
#endif

#ifndef _SVX_LANGITEM_HXX
#include <bf_svx/langitem.hxx>
#endif

#ifndef _SWDOCSH_HXX
#include "docsh.hxx"
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _DOC_HXX //autogen wg. SwDoc
#include <doc.hxx>
#endif

#ifndef _XMLIMP_HXX
#include "xmlimp.hxx"
#endif
#ifndef _XMLEXP_HXX
#include "xmlexp.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

// ---------------------------------------------------------------------

SvXMLImportContext *SwXMLImport::CreateScriptContext(
									   const OUString& rLocalName )
{
	SvXMLImportContext *pContext = 0;

	if( !(IsStylesOnlyMode() || IsInsertMode()) )
	{
		pContext = new XMLScriptContext( *this,
									XML_NAMESPACE_OFFICE, rLocalName,
									GetModel() );
	}
	
	if( !pContext )
		pContext = new SvXMLImportContext( *this, XML_NAMESPACE_OFFICE,
									   	   rLocalName );

	return pContext;
}

}
