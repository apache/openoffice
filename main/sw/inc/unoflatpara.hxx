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



#ifndef _UNOFLATPARA_HXX
#define _UNOFLATPARA_HXX

#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/text/XFlatParagraph.hpp>
#include <com/sun/star/text/XFlatParagraphIterator.hpp>
#include <calbck.hxx>
#include <modeltoviewhelper.hxx>
#include <unotextmarkup.hxx>

#include <set>

namespace css = com::sun::star;

namespace com { namespace sun { namespace star { namespace container {
    class XStringKeyMap;
} } } }

namespace com { namespace sun { namespace star { namespace text {
    class XTextRange;
} } } }

class SwTxtNode;
class SwDoc;

/******************************************************************************
 * SwXFlatParagraph
 ******************************************************************************/

class SwXFlatParagraph:
    public ::cppu::WeakImplHelper2
    <
        css::text::XFlatParagraph,
        css::lang::XUnoTunnel
    >,
    public SwXTextMarkup
{
public:
    SwXFlatParagraph( SwTxtNode& rTxtNode, rtl::OUString aExpandText, const ModelToViewHelper::ConversionMap* pConversionMap );
    virtual ~SwXFlatParagraph();

    virtual 	::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
    virtual void SAL_CALL acquire(  ) throw();
    virtual void SAL_CALL release(  ) throw();

	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

    // text::XTextMarkup:
    virtual css::uno::Reference< css::container::XStringKeyMap > SAL_CALL getMarkupInfoContainer();

    virtual void SAL_CALL commitStringMarkup(::sal_Int32 nType, const ::rtl::OUString & aIdentifier, ::sal_Int32 nStart, ::sal_Int32 nLength,
		                           const css::uno::Reference< css::container::XStringKeyMap > & xMarkupInfoContainer);

    virtual void SAL_CALL commitTextRangeMarkup(::sal_Int32 nType, const ::rtl::OUString & aIdentifier, const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange> & xRange,
                                                const css::uno::Reference< css::container::XStringKeyMap > & xMarkupInfoContainer);



    // text::XFlatParagraph:
    virtual ::rtl::OUString SAL_CALL getText();
    virtual ::sal_Bool SAL_CALL isModified();
    virtual void SAL_CALL setChecked(::sal_Int32 nType, ::sal_Bool bVal);
    virtual ::sal_Bool SAL_CALL isChecked(::sal_Int32 nType);
    virtual css::lang::Locale SAL_CALL getLanguageOfText(::sal_Int32 nPos, ::sal_Int32 nLen);
    virtual css::lang::Locale SAL_CALL getPrimaryLanguageOfText(::sal_Int32 nPos, ::sal_Int32 nLen);
    virtual void SAL_CALL changeText(::sal_Int32 nPos, ::sal_Int32 nLen, const ::rtl::OUString & aNewText, const css::uno::Sequence< css::beans::PropertyValue > & aAttributes);
    virtual void SAL_CALL changeAttributes(::sal_Int32 nPos, ::sal_Int32 nLen, const css::uno::Sequence< css::beans::PropertyValue > & aAttributes);
    virtual css::uno::Sequence< ::sal_Int32 > SAL_CALL getLanguagePortions();

    const SwTxtNode* getTxtNode() const;

    static const css::uno::Sequence< sal_Int8 >& getUnoTunnelId();

    // XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething(const css::uno::Sequence< sal_Int8 >& rId);

private:
    SwXFlatParagraph( const SwXFlatParagraph & ); // not defined
    SwXFlatParagraph & operator = ( const SwXFlatParagraph & ); // not defined

    rtl::OUString maExpandText;
};

/******************************************************************************
 * SwXFlatParagraphIterator
 ******************************************************************************/

class SwXFlatParagraphIterator:
    public ::cppu::WeakImplHelper1
    <
        css::text::XFlatParagraphIterator
    >,
    public SwClient     // to get notified when doc is closed...
{
public:
    SwXFlatParagraphIterator( SwDoc& rDoc, sal_Int32 nType, sal_Bool bAutomatic );
    virtual ~SwXFlatParagraphIterator();

    // text::XFlatParagraphIterator:
    virtual css::uno::Reference< css::text::XFlatParagraph > SAL_CALL getFirstPara();
    virtual css::uno::Reference< css::text::XFlatParagraph > SAL_CALL getNextPara();
    virtual css::uno::Reference< css::text::XFlatParagraph > SAL_CALL getLastPara();
    virtual css::uno::Reference< css::text::XFlatParagraph > SAL_CALL getParaBefore(const css::uno::Reference< css::text::XFlatParagraph > & xPara);
    virtual css::uno::Reference< css::text::XFlatParagraph > SAL_CALL getParaAfter(const css::uno::Reference< css::text::XFlatParagraph > & xPara);

protected:
    // SwClient
    virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew );

private:
    SwXFlatParagraphIterator( const SwXFlatParagraphIterator & ); // not defined
    SwXFlatParagraphIterator & operator =(const SwXFlatParagraphIterator & ); // not defined

    // container to hold the 'hard' references as long as necessary and valid
    std::set< css::uno::Reference< css::text::XFlatParagraph > >    m_aFlatParaList;

    SwDoc* mpDoc;
    const sal_Int32 mnType;
    const sal_Bool mbAutomatic;

    sal_uLong mnCurrentNode;    // used for non-automatic mode
    sal_uLong mnStartNode;      // used for non-automatic mode
    sal_uLong mnEndNode;        // used for non-automatic mode
    sal_Bool mbWrapped;     // used for non-automatic mode
};

#endif
