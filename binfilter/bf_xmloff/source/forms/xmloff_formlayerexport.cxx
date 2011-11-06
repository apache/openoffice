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



#include <stdio.h>

#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif
#ifndef _XMLOFF_FORMS_LAYEREXPORT_HXX_
#include "layerexport.hxx"
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _XMLOFF_FORMS_OFFICEFORMS_HXX_
#include "officeforms.hxx"
#endif
namespace binfilter {


//.........................................................................
namespace xmloff
{
//.........................................................................

	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::lang;
	using namespace ::com::sun::star::awt;
	using namespace ::com::sun::star::beans;
	using namespace ::com::sun::star::container;
	using namespace ::com::sun::star::drawing;
	using namespace ::com::sun::star::frame;

	//=====================================================================
	//= OFormLayerXMLExport
	//=====================================================================

	//---------------------------------------------------------------------
	OFormLayerXMLExport::OFormLayerXMLExport(SvXMLExport& _rContext)
		:m_rContext(_rContext)
		,m_pImpl(new OFormLayerXMLExport_Impl(_rContext))
	{
	}

	//---------------------------------------------------------------------
	OFormLayerXMLExport::~OFormLayerXMLExport()
	{
		delete m_pImpl;
		m_pImpl = NULL;
	}

	//---------------------------------------------------------------------
	sal_Bool OFormLayerXMLExport::seekPage(const Reference< XDrawPage >& _rxDrawPage)
	{
		return m_pImpl->seekPage(_rxDrawPage);
	}

	//---------------------------------------------------------------------
	::rtl::OUString OFormLayerXMLExport::getControlId(const Reference< XPropertySet >& _rxControl)
	{
		return m_pImpl->getControlId(_rxControl);
	}

	//---------------------------------------------------------------------
	::rtl::OUString OFormLayerXMLExport::getControlNumberStyle( const Reference< XPropertySet >& _rxControl )
	{
		return m_pImpl->getControlNumberStyle(_rxControl);
	}

	//---------------------------------------------------------------------
	void OFormLayerXMLExport::examineForms(const Reference< XDrawPage >& _rxDrawPage)
	{
		try
		{
			m_pImpl->examineForms(_rxDrawPage);
		}
		catch(Exception&)
		{
			OSL_ENSURE(sal_False, "OFormLayerXMLExport::examine: could not examine the draw page!");
		}
	}

	//---------------------------------------------------------------------
	void OFormLayerXMLExport::exportForms(const Reference< XDrawPage >& _rxDrawPage)
	{
		m_pImpl->exportForms(_rxDrawPage);
	}

	//---------------------------------------------------------------------
	void OFormLayerXMLExport::exportAutoControlNumberStyles()
	{
		m_pImpl->exportAutoControlNumberStyles();
	}

	//---------------------------------------------------------------------
	void OFormLayerXMLExport::exportAutoStyles()
	{
		m_pImpl->exportAutoStyles();
	}

	//---------------------------------------------------------------------
	void OFormLayerXMLExport::excludeFromExport( const Reference< XControlModel > _rxControl )
	{
		m_pImpl->excludeFromExport( _rxControl );
	}

	//=========================================================================
	//= OOfficeFormsExport
	//=========================================================================
	//-------------------------------------------------------------------------
	OOfficeFormsExport::OOfficeFormsExport( SvXMLExport& _rExp )
		:m_pImpl(NULL)
	{
		m_pImpl = new OFormsRootExport(_rExp);
	}

	//-------------------------------------------------------------------------
	OOfficeFormsExport::~OOfficeFormsExport()
	{
		delete m_pImpl;
	}

//.........................................................................
}	// namespace xmloff
//.........................................................................

}//end of namespace binfilter
