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



#ifndef _XMLOFF_FORMS_CALLBACKS_HXX_
#define _XMLOFF_FORMS_CALLBACKS_HXX_

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXACCESS_HPP_
#include <com/sun/star/container/XIndexAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_SCRIPTEVENTDESCRIPTOR_HPP_
#include <com/sun/star/script/ScriptEventDescriptor.hpp>
#endif
#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif
namespace binfilter {

class SvXMLImport;
class SvXMLExport;
class SvXMLExportPropertyMapper;
class SvXMLStyleContext;

//.........................................................................
namespace xmloff
{
//.........................................................................

	class OAttribute2Property;

	//=====================================================================
	//= IFormsExportContext
	//=====================================================================
	class IFormsExportContext
	{
	public:
		virtual SvXMLExport&								getGlobalContext() = 0;
		virtual ::vos::ORef< SvXMLExportPropertyMapper >	getStylePropertyMapper() = 0;

		/** steps through a collection and exports all children of this collection
		*/
		virtual void	exportCollectionElements(
			const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& _rxCollection) = 0;

		virtual ::rtl::OUString								getObjectStyleName(
			const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxObject ) = 0;
	};

	//=====================================================================
	//= IControlIdMap
	//=====================================================================
	class IControlIdMap
	{
	public:
		/** register a control id
		*/
		virtual void	registerControlId(
			const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControl,
			const ::rtl::OUString& _rId) = 0;

		/** register references to a control.

			<p>In the XML representation, the control which is refered by others stores the ids of these other
			controls, but in "real life" :) the referring controls store the referred one as property.</p>

			<p>This method allows a referred control to announce to ids of the referring ones.</p>

			@param _rxControl
				the referred control
			@param _rReferringControls
				a (comma separated) list of control ids of referring controls
		*/
		virtual void	registerControlReferences(
			const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControl,
			const ::rtl::OUString& _rReferringControls) = 0;
	};

	//=====================================================================
	//= IFormsImportContext
	//=====================================================================
	class IFormsImportContext
	{
	public:
		virtual IControlIdMap&				getControlIdMap() = 0;
		virtual OAttribute2Property&		getAttributeMap() = 0;
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
											getServiceFactory() = 0;
		virtual SvXMLImport&				getGlobalContext() = 0;

		virtual const SvXMLStyleContext*	getStyleElement(const ::rtl::OUString& _rStyleName) const = 0;

		virtual void						enterEventContext() = 0;
		virtual void						leaveEventContext() = 0;

        /** applies the given number style to the given object
		*/
		virtual void applyControlNumberStyle(
			const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControlModel,
			const ::rtl::OUString& _rControlNumerStyleName
		) = 0;

        /** registers a control model for later binding to a spreadsheet cell value
        */
        virtual void                        registerCellValueBinding(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControlModel,
            const ::rtl::OUString& _rCellAddress
        ) = 0;

        /** registers a list-like control model for later binding to a spreadsheet cell range as list source
        */
        virtual void                        registerCellRangeListSource(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControlModel,
            const ::rtl::OUString& _rCellRangeAddress
        ) = 0;
	};

	//=====================================================================
	//= IEventAttacherManager
	//=====================================================================
	class IEventAttacherManager
	{
	public:
		virtual void registerEvents(
			const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxElement,
			const ::com::sun::star::uno::Sequence< ::com::sun::star::script::ScriptEventDescriptor >& _rEvents
			) = 0;
	};

	//=====================================================================
	//= IEventAttacher
	//=====================================================================
	class IEventAttacher
	{
	public:
		virtual void registerEvents(
			const ::com::sun::star::uno::Sequence< ::com::sun::star::script::ScriptEventDescriptor >& _rEvents
			) = 0;
	};

//.........................................................................
}	// namespace xmloff
//.........................................................................

}//end of namespace binfilter
#endif // _XMLOFF_FORMS_CALLBACKS_HXX_

