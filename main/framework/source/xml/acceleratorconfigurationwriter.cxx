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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_framework.hxx"
#include <xml/acceleratorconfigurationwriter.hxx>

//_______________________________________________
// own includes
#include <acceleratorconst.h>
#include <threadhelp/readguard.hxx>

//_______________________________________________
// interface includes
#include <com/sun/star/xml/sax/XExtendedDocumentHandler.hpp>
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <com/sun/star/awt/KeyModifier.hpp>

//_______________________________________________
// other includes
#include <vcl/svapp.hxx>
#include <rtl/ustrbuf.hxx>

#include <comphelper/attributelist.hxx>


//_______________________________________________
// namespace

namespace framework{


//-----------------------------------------------
AcceleratorConfigurationWriter::AcceleratorConfigurationWriter(const AcceleratorCache&                                       rContainer,
                                                               const css::uno::Reference< css::xml::sax::XDocumentHandler >& xConfig   )
    : ThreadHelpBase(&Application::GetSolarMutex())
    , m_xConfig     (xConfig                      )
    , m_rContainer  (rContainer                   )
{
}

//-----------------------------------------------
AcceleratorConfigurationWriter::~AcceleratorConfigurationWriter()
{
}

//-----------------------------------------------
void AcceleratorConfigurationWriter::flush()
{
	// SAFE -> ----------------------------------
	ReadGuard aReadLock(m_aLock);

	css::uno::Reference< css::xml::sax::XDocumentHandler >         xCFG        = m_xConfig;
	css::uno::Reference< css::xml::sax::XExtendedDocumentHandler > xExtendedCFG(m_xConfig, css::uno::UNO_QUERY_THROW);

	aReadLock.unlock();
	// <- SAFE ----------------------------------

	// prepare attribute list
	::comphelper::AttributeList* pAttribs = new ::comphelper::AttributeList;
	css::uno::Reference< css::xml::sax::XAttributeList > xAttribs(static_cast< css::xml::sax::XAttributeList* >(pAttribs), css::uno::UNO_QUERY);

	pAttribs->AddAttribute(AL_XMLNS_ACCEL, ATTRIBUTE_TYPE_CDATA, NS_XMLNS_ACCEL);
	pAttribs->AddAttribute(AL_XMLNS_XLINK, ATTRIBUTE_TYPE_CDATA, NS_XMLNS_XLINK);

	// generate xml
	xCFG->startDocument();

	xExtendedCFG->unknown(DOCTYPE_ACCELERATORS);
	xCFG->ignorableWhitespace(::rtl::OUString());

	xCFG->startElement(AL_ELEMENT_ACCELERATORLIST, xAttribs);
	xCFG->ignorableWhitespace(::rtl::OUString());

	// TODO think about threadsafe using of cache
	AcceleratorCache::TKeyList                 lKeys = m_rContainer.getAllKeys();
	AcceleratorCache::TKeyList::const_iterator pKey  ;
	for ( pKey  = lKeys.begin();
		  pKey != lKeys.end()  ;
		  ++pKey                 )
	{
		const css::awt::KeyEvent& rKey     = *pKey;
		const ::rtl::OUString&    rCommand = m_rContainer.getCommandByKey(rKey);
		impl_ts_writeKeyCommandPair(rKey, rCommand, xCFG);
	}

	/* TODO write key-command list
	std::vector< SfxAcceleratorConfigItem>::const_iterator p;
	for ( p = m_aWriteAcceleratorList.begin(); p != m_aWriteAcceleratorList.end(); p++ )
		WriteAcceleratorItem( *p );
	*/

	xCFG->ignorableWhitespace(::rtl::OUString());
	xCFG->endElement(AL_ELEMENT_ACCELERATORLIST);
	xCFG->ignorableWhitespace(::rtl::OUString());
	xCFG->endDocument();
}

//-----------------------------------------------
void AcceleratorConfigurationWriter::impl_ts_writeKeyCommandPair(const css::awt::KeyEvent&                                     aKey    ,
                                                                 const ::rtl::OUString&                                        sCommand,
                                                                 const css::uno::Reference< css::xml::sax::XDocumentHandler >& xConfig )
{
	::comphelper::AttributeList* pAttribs = new ::comphelper::AttributeList;
	css::uno::Reference< css::xml::sax::XAttributeList > xAttribs (static_cast< css::xml::sax::XAttributeList* >(pAttribs) , css::uno::UNO_QUERY_THROW);

	::rtl::OUString sKey = m_rKeyMapping->mapCodeToIdentifier(aKey.KeyCode);
	// TODO check if key is empty!

	pAttribs->AddAttribute(AL_ATTRIBUTE_KEYCODE, ATTRIBUTE_TYPE_CDATA, sKey    );
	pAttribs->AddAttribute(AL_ATTRIBUTE_URL    , ATTRIBUTE_TYPE_CDATA, sCommand);

	if ((aKey.Modifiers & css::awt::KeyModifier::SHIFT) == css::awt::KeyModifier::SHIFT)
		pAttribs->AddAttribute(AL_ATTRIBUTE_MOD_SHIFT, ATTRIBUTE_TYPE_CDATA, ::rtl::OUString::createFromAscii("true"));

	if ((aKey.Modifiers & css::awt::KeyModifier::MOD1) == css::awt::KeyModifier::MOD1)
		pAttribs->AddAttribute(AL_ATTRIBUTE_MOD_MOD1, ATTRIBUTE_TYPE_CDATA, ::rtl::OUString::createFromAscii("true"));

	if ((aKey.Modifiers & css::awt::KeyModifier::MOD2) == css::awt::KeyModifier::MOD2)
		pAttribs->AddAttribute(AL_ATTRIBUTE_MOD_MOD2, ATTRIBUTE_TYPE_CDATA, ::rtl::OUString::createFromAscii("true"));

	if ((aKey.Modifiers & css::awt::KeyModifier::MOD3) == css::awt::KeyModifier::MOD3)
		pAttribs->AddAttribute(AL_ATTRIBUTE_MOD_MOD3, ATTRIBUTE_TYPE_CDATA, ::rtl::OUString::createFromAscii("true"));

	xConfig->ignorableWhitespace(::rtl::OUString());
	xConfig->startElement(AL_ELEMENT_ITEM, xAttribs);
	xConfig->ignorableWhitespace(::rtl::OUString());
	xConfig->endElement(AL_ELEMENT_ITEM);
	xConfig->ignorableWhitespace(::rtl::OUString());
}

} // namespace framework
