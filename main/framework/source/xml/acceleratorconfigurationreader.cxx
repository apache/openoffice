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
#include <xml/acceleratorconfigurationreader.hxx>

//_______________________________________________
// own includes

#ifndef __FRAMEWORK_ACCELERATORCONST_H_
#include <acceleratorconst.h>
#endif

//_______________________________________________
// interface includes
#include <com/sun/star/xml/sax/XExtendedDocumentHandler.hpp>
#include <com/sun/star/awt/KeyModifier.hpp>
#include <com/sun/star/awt/KeyEvent.hpp>
#include <com/sun/star/awt/Key.hpp>
#include <com/sun/star/container/ElementExistException.hpp>

//_______________________________________________
// other includes
#include <vcl/svapp.hxx>
#include <rtl/ustrbuf.hxx>

//_______________________________________________
// namespace

namespace framework{

//-----------------------------------------------
/* Throws a SaxException in case a wrong formatted XML
   structure was detected.

   This macro combined the given comment with a generic
   way to find out the XML line (where the error occurred)
   to format a suitable message.

   @param   COMMENT
            an ascii string, which describes the problem.
 */
#define THROW_PARSEEXCEPTION(COMMENT)                                   \
    {                                                                   \
        ::rtl::OUStringBuffer sMessage(256);                            \
        sMessage.append     (implts_getErrorLineString());              \
        sMessage.appendAscii(COMMENT                    );              \
                                                                        \
		throw css::xml::sax::SAXException(                              \
                sMessage.makeStringAndClear(),                          \
                static_cast< css::xml::sax::XDocumentHandler* >(this),  \
                css::uno::Any());                                       \
    }

//-----------------------------------------------
// XInterface
DEFINE_XINTERFACE_1(AcceleratorConfigurationReader                   ,
                    OWeakObject                                      ,
                    DIRECT_INTERFACE(css::xml::sax::XDocumentHandler))

//-----------------------------------------------
AcceleratorConfigurationReader::AcceleratorConfigurationReader(AcceleratorCache& rContainer)
    : ThreadHelpBase          (&Application::GetSolarMutex())
    , OWeakObject             (                             )
    , m_rContainer            (rContainer                   )
    , m_bInsideAcceleratorList(sal_False                    )
    , m_bInsideAcceleratorItem(sal_False                    )
{
}

//-----------------------------------------------
AcceleratorConfigurationReader::~AcceleratorConfigurationReader()
{
}

//-----------------------------------------------
void SAL_CALL AcceleratorConfigurationReader::startDocument()
    throw(css::xml::sax::SAXException,
          css::uno::RuntimeException )
{
}

//-----------------------------------------------
void SAL_CALL AcceleratorConfigurationReader::endDocument()
    throw(css::xml::sax::SAXException,
          css::uno::RuntimeException )
{
    // The xml file seems to be corrupted.
    // Because we found no end-tags... at least for
    // one list or item.
    if (
        (m_bInsideAcceleratorList) ||
        (m_bInsideAcceleratorItem)
       )
    {
        THROW_PARSEEXCEPTION("No matching start or end element 'acceleratorlist' found!")
    }
}

//-----------------------------------------------
void SAL_CALL AcceleratorConfigurationReader::startElement(const ::rtl::OUString&                                      sElement      ,
                                                           const css::uno::Reference< css::xml::sax::XAttributeList >& xAttributeList)
    throw(css::xml::sax::SAXException,
          css::uno::RuntimeException )
{
    EXMLElement eElement = AcceleratorConfigurationReader::implst_classifyElement(sElement);

    // Note: We handle "accel:item" before "accel:acceleratorlist" to perform this operation.
    // Because an item occurs very often... a list should occur one time only!
    if (eElement == E_ELEMENT_ITEM)
    {
        if (!m_bInsideAcceleratorList)
            THROW_PARSEEXCEPTION("An element \"accel:item\" must be embedded into 'accel:acceleratorlist'.")
        if (m_bInsideAcceleratorItem)
            THROW_PARSEEXCEPTION("An element \"accel:item\" is not a container.")
        m_bInsideAcceleratorItem = sal_True;

        ::rtl::OUString    sCommand;
        css::awt::KeyEvent aEvent ;

        sal_Int16 c = xAttributeList->getLength();
        sal_Int16 i = 0;
        for (i=0; i<c; ++i)
        {
            ::rtl::OUString sAttribute = xAttributeList->getNameByIndex(i);
            ::rtl::OUString sValue     = xAttributeList->getValueByIndex(i);
            EXMLAttribute   eAttribute = AcceleratorConfigurationReader::implst_classifyAttribute(sAttribute);
            switch(eAttribute)
            {
                case E_ATTRIBUTE_URL :
                    sCommand = sValue.intern();
                    break;

                case E_ATTRIBUTE_KEYCODE :
                    aEvent.KeyCode = m_rKeyMapping->mapIdentifierToCode(sValue);
                    break;

                case E_ATTRIBUTE_MOD_SHIFT :
                    aEvent.Modifiers |= css::awt::KeyModifier::SHIFT;
                    break;

                case E_ATTRIBUTE_MOD_MOD1 :
                    aEvent.Modifiers |= css::awt::KeyModifier::MOD1;
                    break;

                case E_ATTRIBUTE_MOD_MOD2 :
                    aEvent.Modifiers |= css::awt::KeyModifier::MOD2;
                    break;

                case E_ATTRIBUTE_MOD_MOD3 :
                    aEvent.Modifiers |= css::awt::KeyModifier::MOD3;
            }
        }

        // validate command and key event.
        if (
            (!sCommand.getLength()) ||
            (aEvent.KeyCode == 0  )
           )
        {
            THROW_PARSEEXCEPTION("XML element does not describe a valid accelerator nor a valid command.")
        }

        // register key event + command inside cache...
        // Check for already existing items there.
        if (!m_rContainer.hasKey(aEvent))
            m_rContainer.setKeyCommandPair(aEvent, sCommand);
        #ifdef ENABLE_WARNINGS
        else
        {
            // Attention: It's not really a reason to throw an exception and kill the office, if the configuration contains
            // multiple registrations for the same key :-) Show a warning... and ignore the second item.
            // THROW_PARSEEXCEPTION("Command is registered for the same key more than once.")
            ::rtl::OUStringBuffer sMsg(256);
            sMsg.appendAscii("Double registration detected.\nCommand = \"");
            sMsg.append     (sCommand                                     );
            sMsg.appendAscii("\"\nKeyCode = "                             );
            sMsg.append     ((sal_Int32)aEvent.KeyCode                    );
            sMsg.appendAscii("\nModifiers = "                             );
            sMsg.append     ((sal_Int32)aEvent.Modifiers                  );
            sMsg.appendAscii("\nIgnore this item!"                        );
            LOG_WARNING("AcceleratorConfigurationReader::startElement()", U2B(sMsg.makeStringAndClear()))
        }
        #endif // ENABLE_WARNINGS
    }

    if (eElement == E_ELEMENT_ACCELERATORLIST)
    {
        if (m_bInsideAcceleratorList)
            THROW_PARSEEXCEPTION("An element \"accel:acceleratorlist\" cannot be used recursive.")
        m_bInsideAcceleratorList = sal_True;
        return;
    }
}

//-----------------------------------------------
void SAL_CALL AcceleratorConfigurationReader::endElement(const ::rtl::OUString& sElement)
    throw(css::xml::sax::SAXException,
          css::uno::RuntimeException )
{
    EXMLElement eElement = AcceleratorConfigurationReader::implst_classifyElement(sElement);

    // Note: We handle "accel:item" before "accel:acceleratorlist" to perform this operation.
    // Because an item occurs very often... a list should occur one time only!
    if (eElement == E_ELEMENT_ITEM)
    {
        if (!m_bInsideAcceleratorItem)
            THROW_PARSEEXCEPTION("Found end element 'accel:item', but no start element.")
        m_bInsideAcceleratorItem = sal_False;
    }

    if (eElement == E_ELEMENT_ACCELERATORLIST)
    {
        if (!m_bInsideAcceleratorList)
            THROW_PARSEEXCEPTION("Found end element 'accel:acceleratorlist', but no start element.")
        m_bInsideAcceleratorList = sal_False;
    }
}

//-----------------------------------------------
void SAL_CALL AcceleratorConfigurationReader::characters(const ::rtl::OUString&)
    throw(css::xml::sax::SAXException,
          css::uno::RuntimeException )
{
}

//-----------------------------------------------
void SAL_CALL AcceleratorConfigurationReader::ignorableWhitespace(const ::rtl::OUString&)
    throw(css::xml::sax::SAXException,
          css::uno::RuntimeException )
{
}

//-----------------------------------------------
void SAL_CALL AcceleratorConfigurationReader::processingInstruction(const ::rtl::OUString& /*sTarget*/,
                                                                    const ::rtl::OUString& /*sData*/ )
    throw(css::xml::sax::SAXException,
          css::uno::RuntimeException )
{
}

//-----------------------------------------------
void SAL_CALL AcceleratorConfigurationReader::setDocumentLocator(const css::uno::Reference< css::xml::sax::XLocator >& xLocator)
    throw(css::xml::sax::SAXException,
          css::uno::RuntimeException )
{
    m_xLocator = xLocator;
}

//-----------------------------------------------
AcceleratorConfigurationReader::EXMLElement AcceleratorConfigurationReader::implst_classifyElement(const ::rtl::OUString& sElement)
{
    AcceleratorConfigurationReader::EXMLElement eElement;

    if (sElement.equals(NS_ELEMENT_ACCELERATORLIST))
        eElement = E_ELEMENT_ACCELERATORLIST;
    else
    if (sElement.equals(NS_ELEMENT_ITEM))
        eElement = E_ELEMENT_ITEM;
    else
        throw css::uno::RuntimeException(
                DECLARE_ASCII("Unknown XML element detected!"),
                css::uno::Reference< css::xml::sax::XDocumentHandler >());

    return eElement;
}

//-----------------------------------------------
AcceleratorConfigurationReader::EXMLAttribute AcceleratorConfigurationReader::implst_classifyAttribute(const ::rtl::OUString& sAttribute)
{
    AcceleratorConfigurationReader::EXMLAttribute eAttribute;

    if (sAttribute.equals(NS_ATTRIBUTE_KEYCODE))
        eAttribute = E_ATTRIBUTE_KEYCODE;
    else
    if (sAttribute.equals(NS_ATTRIBUTE_MOD_SHIFT))
        eAttribute = E_ATTRIBUTE_MOD_SHIFT;
    else
    if (sAttribute.equals(NS_ATTRIBUTE_MOD_MOD1))
        eAttribute = E_ATTRIBUTE_MOD_MOD1;
    else
    if (sAttribute.equals(NS_ATTRIBUTE_MOD_MOD2))
        eAttribute = E_ATTRIBUTE_MOD_MOD2;
    else
    if (sAttribute.equals(NS_ATTRIBUTE_MOD_MOD3))
        eAttribute = E_ATTRIBUTE_MOD_MOD3;
    else
    if (sAttribute.equals(NS_ATTRIBUTE_URL))
        eAttribute = E_ATTRIBUTE_URL;
    else
        throw css::uno::RuntimeException(
                DECLARE_ASCII("Unknown XML attribute detected!"),
                css::uno::Reference< css::xml::sax::XDocumentHandler >());

    return eAttribute;
}

//-----------------------------------------------
::rtl::OUString AcceleratorConfigurationReader::implts_getErrorLineString()
{
    if (!m_xLocator.is())
        return DECLARE_ASCII("Error during parsing XML. (No further info available...)");

    ::rtl::OUStringBuffer sMsg(256);
    sMsg.appendAscii("Error during parsing XML in\nline = ");
    sMsg.append     (m_xLocator->getLineNumber()           );
    sMsg.appendAscii("\ncolumn = "                         );
    sMsg.append     (m_xLocator->getColumnNumber()         );
    sMsg.appendAscii("."                                   );
    return sMsg.makeStringAndClear();
}

} // namespace framework

