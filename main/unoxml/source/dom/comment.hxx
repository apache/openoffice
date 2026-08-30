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



#ifndef DOM_COMMENT_HXX
#define DOM_COMMENT_HXX

#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/xml/dom/XComment.hpp>

#include <characterdata.hxx>


using ::rtl::OUString;
using namespace com::sun::star::uno;
using namespace com::sun::star::xml::dom;

namespace DOM
{
    typedef ::cppu::ImplInheritanceHelper1< CCharacterData, XComment >
        CComment_Base;

    class CComment
        : public CComment_Base
    {
    private:
        friend class CDocument;

    protected:
        CComment(CDocument const& rDocument, ::osl::Mutex const& rMutex,
                xmlNodePtr const pNode);

    public:

        virtual void saxify(const Reference< XDocumentHandler >& i_xHandler);

         // --- delegations for XCharacterData
        virtual void SAL_CALL appendData(const OUString& arg)
        {
            CCharacterData::appendData(arg);
        }
        virtual void SAL_CALL deleteData(sal_Int32 offset, sal_Int32 count)
        {
            CCharacterData::deleteData(offset, count);
        }
        virtual OUString SAL_CALL getData()
        {
            return CCharacterData::getData();
        }
        virtual sal_Int32 SAL_CALL getLength()
        {
            return CCharacterData::getLength();
        }
        virtual void SAL_CALL insertData(sal_Int32 offset, const OUString& arg)
        {
            CCharacterData::insertData(offset, arg);
        }
        virtual void SAL_CALL replaceData(sal_Int32 offset, sal_Int32 count, const OUString& arg)
        {
            CCharacterData::replaceData(offset, count, arg);
        }
        virtual void SAL_CALL setData(const OUString& data)
        {
            CCharacterData::setData(data);
        }
        virtual OUString SAL_CALL subStringData(sal_Int32 offset, sal_Int32 count)
        {
            return CCharacterData::subStringData(offset, count);
        }


         // --- overrides for XNode base
        virtual OUString SAL_CALL getNodeName();
        virtual OUString SAL_CALL getNodeValue();

    // --- delegation for XNde base.
    virtual Reference< XNode > SAL_CALL appendChild(const Reference< XNode >& newChild)
    {
        return CCharacterData::appendChild(newChild);
    }
    virtual Reference< XNode > SAL_CALL cloneNode(sal_Bool deep)
    {
        return CCharacterData::cloneNode(deep);
    }
    virtual Reference< XNamedNodeMap > SAL_CALL getAttributes()
    {
        return CCharacterData::getAttributes();
    }
    virtual Reference< XNodeList > SAL_CALL getChildNodes()
    {
        return CCharacterData::getChildNodes();
    }
    virtual Reference< XNode > SAL_CALL getFirstChild()
    {
        return CCharacterData::getFirstChild();
    }
    virtual Reference< XNode > SAL_CALL getLastChild()
    {
        return CCharacterData::getLastChild();
    }
    virtual OUString SAL_CALL getLocalName()
    {
        return CCharacterData::getLocalName();
    }
    virtual OUString SAL_CALL getNamespaceURI()
    {
        return CCharacterData::getNamespaceURI();
    }
    virtual Reference< XNode > SAL_CALL getNextSibling()
    {
        return CCharacterData::getNextSibling();
    }
    virtual NodeType SAL_CALL getNodeType()
    {
        return CCharacterData::getNodeType();
    }
    virtual Reference< XDocument > SAL_CALL getOwnerDocument()
    {
        return CCharacterData::getOwnerDocument();
    }
    virtual Reference< XNode > SAL_CALL getParentNode()
    {
        return CCharacterData::getParentNode();
    }
    virtual OUString SAL_CALL getPrefix()
    {
        return CCharacterData::getPrefix();
    }
    virtual Reference< XNode > SAL_CALL getPreviousSibling()
    {
        return CCharacterData::getPreviousSibling();
    }
    virtual sal_Bool SAL_CALL hasAttributes()
    {
        return CCharacterData::hasAttributes();
    }
    virtual sal_Bool SAL_CALL hasChildNodes()
    {
        return CCharacterData::hasChildNodes();
    }
    virtual Reference< XNode > SAL_CALL insertBefore(
            const Reference< XNode >& newChild, const Reference< XNode >& refChild)
    {
        return CCharacterData::insertBefore(newChild, refChild);
    }
    virtual sal_Bool SAL_CALL isSupported(const OUString& feature, const OUString& ver)
    {
        return CCharacterData::isSupported(feature, ver);
    }
    virtual void SAL_CALL normalize()
    {
        CCharacterData::normalize();
    }
    virtual Reference< XNode > SAL_CALL removeChild(const Reference< XNode >& oldChild)
    {
        return CCharacterData::removeChild(oldChild);
    }
    virtual Reference< XNode > SAL_CALL replaceChild(
            const Reference< XNode >& newChild, const Reference< XNode >& oldChild)
    {
        return CCharacterData::replaceChild(newChild, oldChild);
    }
    virtual void SAL_CALL setNodeValue(const OUString& nodeValue)
    {
        return CCharacterData::setNodeValue(nodeValue);
    }
    virtual void SAL_CALL setPrefix(const OUString& prefix)
    {
        return CCharacterData::setPrefix(prefix);
    }

    };
}

#endif
