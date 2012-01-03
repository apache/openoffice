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
#include "XMLEventExport.hxx"
#endif


#ifndef _COM_SUN_STAR_DOCUMENT_XEVENTSSUPPLIER_HPP
#include <com/sun/star/document/XEventsSupplier.hpp>
#endif


#ifndef _TOOLS_DEBUG_HXX 
#include <tools/debug.hxx>
#endif

#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif


#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

namespace binfilter {


using namespace ::com::sun::star::uno;

using ::std::map;
using ::rtl::OUString;
using ::com::sun::star::beans::PropertyValue;
using ::com::sun::star::document::XEventsSupplier;
using ::com::sun::star::container::XNameReplace;
using ::com::sun::star::container::XNameAccess;
using ::binfilter::xmloff::token::GetXMLToken;
using ::binfilter::xmloff::token::XML_EVENTS;


XMLEventExport::XMLEventExport(SvXMLExport& rExp,
						 const XMLEventNameTranslation* pTranslationTable) :
	rExport(rExp),
	sEventType(RTL_CONSTASCII_USTRINGPARAM("EventType"))
{
	AddTranslationTable(pTranslationTable);
}

XMLEventExport::~XMLEventExport()
{
	// delete all handlers
	HandlerMap::iterator aEnd = aHandlerMap.end();
	for( HandlerMap::iterator aIter = 
			 aHandlerMap.begin();
		 aIter != aEnd;
		 aIter++ )
	{
		delete aIter->second;
	}
	aHandlerMap.clear();
}

void XMLEventExport::AddHandler( const OUString& rName,
								 XMLEventExportHandler* pHandler )
{
	DBG_ASSERT(pHandler != NULL, "Need EventExportHandler");
	if (pHandler != NULL)
	{
		aHandlerMap[rName] = pHandler;
	}
}

void XMLEventExport::AddTranslationTable( 
	const XMLEventNameTranslation* pTransTable )
{
	if (NULL != pTransTable)
	{
		// put translation table into map
		for(const XMLEventNameTranslation* pTrans = pTransTable;
			pTrans->sAPIName != NULL;
			pTrans++)
		{
			aNameTranslationMap[OUString::createFromAscii(pTrans->sAPIName)] = 
				OUString::createFromAscii(pTrans->sXMLName);
		}
	}
	// else? ignore!
}

void XMLEventExport::Export( Reference<XEventsSupplier> & rSupplier,
							 sal_Bool bWhitespace)
{
	if (rSupplier.is())
	{
		Reference<XNameAccess> xAccess(rSupplier->getEvents(), UNO_QUERY);
		Export(xAccess, bWhitespace);
	}
	// else: no supplier, no export -> ignore!
}

void XMLEventExport::Export( Reference<XNameReplace> & rReplace,
							 sal_Bool bWhitespace)
{
	Reference<XNameAccess> xAccess(rReplace, UNO_QUERY);
	Export(xAccess, bWhitespace);
}

void XMLEventExport::Export( Reference<XNameAccess> & rAccess,
							 sal_Bool bWhitespace)
{
	// early out if we don't actually get any events
	if (!rAccess.is())
	{
		return;
	}

	// have we already processed an element?
	sal_Bool bStarted = sal_False;

	// iterate over all event types
	Sequence<OUString> aNames = rAccess->getElementNames();
	sal_Int32 nCount = aNames.getLength();
	for(sal_Int32 i = 0; i < nCount; i++)
	{
		// translate name
        NameMap::iterator aIter = aNameTranslationMap.find(aNames[i]);
		if (aIter != aNameTranslationMap.end())
		{
			OUString& rXmlName = aIter->second;

			// get PropertyValues for this event
			Any aAny = rAccess->getByName( aNames[i] );
			Sequence<PropertyValue> aValues;
			aAny >>= aValues;

            // now export the current event
            ExportEvent( aValues, rXmlName, bWhitespace, bStarted );
		}
		else
		{
			// don't proceed further
			DBG_ERROR("Unknown event name.");
		}
	}

	// close <script:events> element (if it was opened before)
	if (bStarted)
	{
		EndElement(bWhitespace);
	}
}

/// export a singular event and wirte <office:events> container
void XMLEventExport::ExportSingleEvent(
    Sequence<PropertyValue>& rEventValues,
    const OUString& rEventName,
    sal_Bool bUseWhitespace )
{
    // translate the name
    NameMap::iterator aIter = aNameTranslationMap.find(rEventName);
    if (aIter != aNameTranslationMap.end())
	{
        OUString& rXmlName = aIter->second;

        // export the event ...
        sal_Bool bStarted = sal_False;
        ExportEvent( rEventValues, rXmlName, bUseWhitespace, bStarted );

        // ... and close the container element (if necessary)
        if (bStarted)
        {
            EndElement(bUseWhitespace);
        }
    }
    else
    {
        // unknown name!
        DBG_ERROR("unknown event name");
    }
}


/// export a single event
void XMLEventExport::ExportEvent( 
    Sequence<PropertyValue>& rEventValues,
    const OUString& rXmlName,
    sal_Bool bUseWhitespace,
    sal_Bool& rExported )
{
    // search for EventType value and then delegate to EventHandler
    sal_Int32 nValues = rEventValues.getLength();
    const PropertyValue* pValues = rEventValues.getConstArray();

    for(sal_Int32 nVal = 0; nVal < nValues; nVal++)
    {
        if (sEventType.equals(pValues[nVal].Name))
        {
            // found! Now find handler and delegate
            OUString sType;
            pValues[nVal].Value >>= sType;

            if (aHandlerMap.count(sType))
            {
                if (! rExported)
                {
                    // OK, we have't yet exported the enclosing
                    // element. So we do that now.
                    rExported = sal_True;
                    StartElement(bUseWhitespace);
                }

                // delegate to proper ExportEventHandler
                aHandlerMap[sType]->Export(rExport, rXmlName, 
                                           rEventValues, bUseWhitespace);
            }
            else
            {
                if (! sType.equalsAsciiL("None", sizeof("None")-1))
                {
                    DBG_ERROR("unknown event type returned by API");
                    // unknown type -> error (ignore)
                }
                // else: we ignore None fields
            }

            // early out: we don't need to look for another type
            break;
        }
        // else: we only care for EventType -> ignore
    }
}


void XMLEventExport::StartElement(sal_Bool bWhitespace)
{
	if (bWhitespace)
    {
		rExport.IgnorableWhitespace();
    }
    rExport.StartElement( XML_NAMESPACE_OFFICE, XML_EVENTS, bWhitespace);
}

void XMLEventExport::EndElement(sal_Bool bWhitespace)
{
    rExport.EndElement(XML_NAMESPACE_OFFICE, XML_EVENTS, bWhitespace);
	if (bWhitespace)
	{
		rExport.IgnorableWhitespace();
	}
}


// implement aStandardEventTable (defined in xmlevent.hxx)
const XMLEventNameTranslation aStandardEventTable[] = 
{
	{ "OnSelect", 			"on-select" },
	{ "OnInsertStart",		"on-insert-start" },
	{ "OnInsertDone",		"on-insert-done" },
	{ "OnMailMerge",		"on-mail-merge" },
	{ "OnAlphaCharInput",	"on-alpha-char-input" },
	{ "OnNonAlphaCharInput",	"on-non-alpha-char-input" },
	{ "OnResize",			"on-resize" },
	{ "OnMove",				"on-move" },
	{ "OnPageCountChange",	"page-count-change" },
	{ "OnMouseOver",		"on-mouse-over" },
	{ "OnClick",			"on-click" },
	{ "OnMouseOut",			"on-mouse-out" },
	{ "OnLoadError",		"on-load-error" },
	{ "OnLoadCancel",		"on-load-cancel" },
	{ "OnLoadDone",			"on-load-done" },
	{ "OnLoad",				"on-load" },
	{ "OnUnload",			"on-unload" },
	{ "OnStartApp",			"on-start-app" },
	{ "OnCloseApp",			"on-close-app" },
	{ "OnNew",				"on-new" },
	{ "OnSave",				"on-save" },
	{ "OnSaveAs",			"on-save-as" },
	{ "OnFocus",			"on-focus" },
	{ "OnUnfocus",			"on-unfocus" },
	{ "OnPrint",			"on-print" },
	{ "OnError",			"on-error" },
	{ "OnLoadFinished",		"on-load-finished" },
	{ "OnSaveFinished",		"on-save-finished" },
	{ "OnModifyChanged",	"on-modify-changed" },
	{ "OnPrepareUnload",	"on-prepare-unload" },
	{ "OnNewMail",			"on-new-mail" },
	{ "OnToggleFullscreen",	"on-toggle-fullscreen" },
	{ "OnSaveDone", 		"on-save-done" },
	{ "OnSaveAsDone",		"on-save-as-done" },
	{ NULL, NULL }
};
}//end of namespace binfilter
