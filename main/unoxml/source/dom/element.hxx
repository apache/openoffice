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



#ifndef DOM_ELEMENT_HXX
#define DOM_ELEMENT_HXX

#include <libxml/tree.h>

#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/xml/dom/XNode.hpp>
#include <com/sun/star/xml/dom/XNodeList.hpp>
#include <com/sun/star/xml/dom/XNamedNodeMap.hpp>
#include <com/sun/star/xml/dom/NodeType.hpp>

#include <node.hxx>


using ::rtl::OUString;
using namespace com::sun::star::uno;
using namespace com::sun::star::xml::dom;

namespace DOM
{
    typedef ::cppu::ImplInheritanceHelper1<CNode, XElement > CElement_Base;

    class CElement
        : public CElement_Base
    {
    private:
        friend class CDocument;

        Reference< XAttr > setAttributeNode_Impl_Lock(
                Reference< XAttr > const& xNewAttr, bool const bNS);

    protected:
        CElement(CDocument const& rDocument, ::osl::Mutex const& rMutex,
                xmlNodePtr const pNode);

    public:

        virtual void saxify(const Reference< XDocumentHandler >& i_xHandler);

        virtual void fastSaxify( Context& i_rContext );

        virtual bool IsChildTypeAllowed(NodeType const nodeType);

        /**
        Retrieves an attribute value by name.
        */
        virtual OUString  SAL_CALL getAttribute(const OUString& name);

        /**
        Retrieves an attribute node by name.
        */
        virtual Reference< XAttr > SAL_CALL getAttributeNode(const OUString& name);

        /**
        Retrieves an Attr node by local name and namespace URI.
        */
        virtual Reference< XAttr > SAL_CALL getAttributeNodeNS(const OUString& namespaceURI, const OUString& localName);

        /**
        Retrieves an attribute value by local name and namespace URI.
        */
        virtual OUString SAL_CALL getAttributeNS(const OUString& namespaceURI, const OUString& localName);

        /**
        Returns a NodeList of all descendant Elements with a given tag name,
        in the order in which they are
        encountered in a preorder traversal of this Element tree.
        */
        virtual Reference< XNodeList > SAL_CALL getElementsByTagName(const OUString& name);

        /**
        Returns a NodeList of all the descendant Elements with a given local
        name and namespace URI in the order in which they are encountered in
        a preorder traversal of this Element tree.
        */
        virtual Reference< XNodeList > SAL_CALL getElementsByTagNameNS(const OUString& namespaceURI,
                const OUString& localName);

        /**
        The name of the element.
        */
        virtual OUString SAL_CALL getTagName();

        /**
        Returns true when an attribute with a given name is specified on this
        element or has a default value, false otherwise.
        */
        virtual sal_Bool SAL_CALL hasAttribute(const OUString& name);

        /**
        Returns true when an attribute with a given local name and namespace
        URI is specified on this element or has a default value, false otherwise.
        */
        virtual sal_Bool SAL_CALL hasAttributeNS(const OUString& namespaceURI, const OUString& localName);

        /**
        Removes an attribute by name.
        */
        virtual void SAL_CALL removeAttribute(const OUString& name);

        /**
        Removes the specified attribute node.
        */
        virtual Reference< XAttr > SAL_CALL removeAttributeNode(const Reference< XAttr >& oldAttr);

        /**
        Removes an attribute by local name and namespace URI.
        */
        virtual void SAL_CALL removeAttributeNS(const OUString& namespaceURI, const OUString& localName);

        /**
        Adds a new attribute.
        */
        virtual void SAL_CALL setAttribute(const OUString& name, const OUString& value);

        /**
        Adds a new attribute node.
        */
        virtual Reference< XAttr > SAL_CALL setAttributeNode(const Reference< XAttr >& newAttr);

        /**
        Adds a new attribute.
        */
        virtual Reference< XAttr > SAL_CALL setAttributeNodeNS(const Reference< XAttr >& newAttr);

        /**
        Adds a new attribute.
        */
        virtual void SAL_CALL setAttributeNS(
                const OUString& namespaceURI, const OUString& qualifiedName, const OUString& value);

        /**
        sets the element name
        */
        virtual void SAL_CALL setElementName(const OUString& elementName);

        // overrides for XNode base
        virtual OUString SAL_CALL getNodeName();
        virtual OUString SAL_CALL getNodeValue();
        virtual Reference< XNamedNodeMap > SAL_CALL getAttributes();
        virtual OUString SAL_CALL getLocalName();

        // resolve uno inheritance problems...
        // --- delegation for XNde base.
        virtual Reference< XNode > SAL_CALL appendChild(const Reference< XNode >& newChild)
        {
            return CNode::appendChild(newChild);
        }
        virtual Reference< XNode > SAL_CALL cloneNode(sal_Bool deep)
        {
            return CNode::cloneNode(deep);
        }
        virtual Reference< XNodeList > SAL_CALL getChildNodes()
        {
            return CNode::getChildNodes();
        }
        virtual Reference< XNode > SAL_CALL getFirstChild()
        {
            return CNode::getFirstChild();
        }
        virtual Reference< XNode > SAL_CALL getLastChild()
        {
            return CNode::getLastChild();
        }
        virtual OUString SAL_CALL getNamespaceURI()
        {
            return CNode::getNamespaceURI();
        }
        virtual Reference< XNode > SAL_CALL getNextSibling()
        {
            return CNode::getNextSibling();
        }
        virtual NodeType SAL_CALL getNodeType()
        {
            return CNode::getNodeType();
        }
        virtual Reference< XDocument > SAL_CALL getOwnerDocument()
        {
            return CNode::getOwnerDocument();
        }
        virtual Reference< XNode > SAL_CALL getParentNode()
        {
            return CNode::getParentNode();
        }
        virtual OUString SAL_CALL getPrefix()
        {
            return CNode::getPrefix();
        }
        virtual Reference< XNode > SAL_CALL getPreviousSibling()
        {
            return CNode::getPreviousSibling();
        }
        virtual sal_Bool SAL_CALL hasAttributes()
        {
            return CNode::hasAttributes();
        }
        virtual sal_Bool SAL_CALL hasChildNodes()
        {
            return CNode::hasChildNodes();
        }
        virtual Reference< XNode > SAL_CALL insertBefore(
                const Reference< XNode >& newChild, const Reference< XNode >& refChild)
        {
            return CNode::insertBefore(newChild, refChild);
        }
        virtual sal_Bool SAL_CALL isSupported(const OUString& feature, const OUString& ver)
        {
            return CNode::isSupported(feature, ver);
        }
        virtual void SAL_CALL normalize()
        {
            CNode::normalize();
        }
        virtual Reference< XNode > SAL_CALL removeChild(const Reference< XNode >& oldChild)
        {
            return CNode::removeChild(oldChild);
        }
        virtual Reference< XNode > SAL_CALL replaceChild(
                const Reference< XNode >& newChild, const Reference< XNode >& oldChild)
        {
            return CNode::replaceChild(newChild, oldChild);
        }
        virtual void SAL_CALL setNodeValue(const OUString& nodeValue)
        {
            return CNode::setNodeValue(nodeValue);
        }
        virtual void SAL_CALL setPrefix(const OUString& prefix)
        {
            return CNode::setPrefix(prefix);
        }

    };

}

#endif
