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



#ifndef _XMLOFF_XMLEVENTEXPORT_HXX
#define _XMLOFF_XMLEVENTEXPORT_HXX

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif

#ifndef _RTL_USTRING_HXX_
#include "rtl/ustring.hxx"
#endif

#ifndef _XMLOFF_XMLEVENT_HXX
#include <bf_xmloff/xmlevent.hxx>
#endif

#include <map>
namespace com { namespace sun { namespace star {
	namespace document { class XEventsSupplier; }
	namespace container { class XNameReplace; }
	namespace container { class XNameAccess; }
	namespace beans { struct PropertyValue; }
} } }
namespace binfilter {

class SvXMLExport;


typedef ::std::map< ::rtl::OUString, XMLEventExportHandler* > HandlerMap;
typedef ::std::map< ::rtl::OUString, ::rtl::OUString > NameMap;

/** 
 * Export instances of EventsSupplier services. To use this class you
 * must fulfill two conditions:
 *
 * 1) provide a translation from the API event names to XML event
 * names 
 * 2) Register XMLEventExportHandler instances for all script types
 * that you would like to export.
 *
 * The Export()-Methods all have a bUseWhitespace parameter that
 * causes the exported elements to be surrounded by whitespace, which
 * in turn causes the elements to be indented properly. By default,
 * whitespace is used, but it may not be called for in all cases (e.g
 * events attached to hyperlink within a paragraph.)
 */
class XMLEventExport
{
	const ::rtl::OUString sEventType;

	SvXMLExport& rExport;

	HandlerMap aHandlerMap;
	NameMap aNameTranslationMap;

public:
	XMLEventExport(SvXMLExport& rExport,
				   const XMLEventNameTranslation* pTranslationTable = NULL);
	~XMLEventExport();

	/// register an EventExportHandler for a particular script type
	///
	/// The handlers will be deleted when the object is destroyed, hence
	/// no pointers to a handler registered with AddHandler() should be
	/// held by anyone.
	void AddHandler( const ::rtl::OUString& rName,
					 XMLEventExportHandler* rHandler );

	/// register additional event names
	void AddTranslationTable( const XMLEventNameTranslation* pTransTable );

	/// export the events (calls EventExport::Export(Reference<XNameAcess>) )
	void Export( ::com::sun::star::uno::Reference<
					 ::com::sun::star::document::XEventsSupplier> & xAccess,
				sal_Bool bUseWhitespace = sal_True);

	/// export the events (calls EventExport::Export(Reference<XNameAcess>) )
	void Export( ::com::sun::star::uno::Reference<
					 ::com::sun::star::container::XNameReplace> & xAccess,
				sal_Bool bUseWhitespace = sal_True);

	/// export the events (writes <office:events> element)
	void Export( ::com::sun::star::uno::Reference<
					 ::com::sun::star::container::XNameAccess> & xAccess,
				sal_Bool bUseWhitespace = sal_True);

    /// export a single event (writes <office:events> element)
    void ExportSingleEvent(
        ::com::sun::star::uno::Sequence<
            ::com::sun::star::beans::PropertyValue>& rEventValues,
        const ::rtl::OUString& rEventName,
        sal_Bool bUseWhitespace = sal_True );

private:

    /// export one event (start container-element if necessary)
    void ExportEvent(
        ::com::sun::star::uno::Sequence<
            ::com::sun::star::beans::PropertyValue>& rEventValues,
        const ::rtl::OUString& rEventName,
        sal_Bool bUseWhitespace,
        sal_Bool& rExported);

	/// export the start element
	void StartElement(sal_Bool bUseWhitespace);

	/// export the end element
	void EndElement(sal_Bool bUseWhitespace);
};

}//end of namespace binfilter
#endif
