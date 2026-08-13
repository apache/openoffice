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



#ifndef DOM_ENTITYREFERENCE_HXX
#define DOM_ENTITYREFERENCE_HXX

#include <libxml/tree.h>

#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/xml/dom/XEntityReference.hpp>

#include <node.hxx>


using ::rtl::OUString;
using namespace com::sun::star::uno;
using namespace com::sun::star::xml::dom;

namespace DOM
{
    typedef ::cppu::ImplInheritanceHelper1< CNode, XEntityReference >
        CEntityReference_Base;

    class CEntityReference
        : public CEntityReference_Base
    {
    private:
        friend class CDocument;

    protected:
        CEntityReference(
            CDocument const& rDocument, ::osl::Mutex const& rMutex,
            xmlNodePtr const pNode);

    public:
        virtual bool IsChildTypeAllowed(NodeType const nodeType);

        // ---- resolve uno inheritance problems...
        // overrides for XNode base
        virtual OUString SAL_CALL getNodeName();
        virtual OUString SAL_CALL getNodeValue();
    // --- delegation for XNde base.
    virtual Reference< XNode > SAL_CALL appendChild(const Reference< XNode >& newChild)
    {
        return CNode::appendChild(newChild);
    }
    virtual Reference< XNode > SAL_CALL cloneNode(sal_Bool deep)
    {
        return CNode::cloneNode(deep);
    }
    virtual Reference< XNamedNodeMap > SAL_CALL getAttributes()
    {
        return CNode::getAttributes();
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
    virtual OUString SAL_CALL getLocalName()
    {
        return CNode::getLocalName();
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
