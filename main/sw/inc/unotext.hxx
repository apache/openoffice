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



#ifndef SW_UNOTEXT_HXX
#define SW_UNOTEXT_HXX

#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextCopy.hpp>
#include <com/sun/star/text/XTextRangeCompare.hpp>
#include <com/sun/star/text/XRelativeTextContentInsert.hpp>
#include <com/sun/star/text/XRelativeTextContentRemove.hpp>
#include <com/sun/star/text/XTextAppendAndConvert.hpp>

#include <unobaseclass.hxx>


namespace com { namespace sun { namespace star {
    namespace text {
        class XTextContent;
    }
} } }

class SfxItemPropertySet;
class SwDoc;
class SwStartNode;
class SwPaM;
class OTextCursorHelper;
class SwXTextRange;


/* -----------------03.12.98 12:22-------------------
 *
 * --------------------------------------------------*/

class SwXText
    : public ::com::sun::star::lang::XTypeProvider
    , public ::com::sun::star::lang::XUnoTunnel
    , public ::com::sun::star::beans::XPropertySet
    , public ::com::sun::star::text::XTextAppendAndConvert
    , public ::com::sun::star::text::XTextCopy
    , public ::com::sun::star::text::XTextRangeCompare
    , public ::com::sun::star::text::XRelativeTextContentInsert
    , public ::com::sun::star::text::XRelativeTextContentRemove
{

private:

    class Impl;
    ::sw::UnoImplPtr<Impl> m_pImpl;

    virtual void PrepareForAttach(
            ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xRange,
            SwPaM const & rPam);
    virtual bool CheckForOwnMemberMeta(
            const SwPaM & rPam, const bool bAbsorb);

protected:

    bool            IsValid() const;
    void            Invalidate();
    void            SetDoc(SwDoc *const pDoc);

    virtual ~SwXText();

public: /*not protected because C++ is retarded*/
    virtual const SwStartNode *GetStartNode() const;

public:

    SwXText(SwDoc *const pDoc, const enum CursorType eType);

    const SwDoc*    GetDoc() const;
          SwDoc*    GetDoc();

    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextCursor >
        CreateCursor();


    // XInterface
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
            const ::com::sun::star::uno::Type& rType);

    // XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >
        SAL_CALL getTypes();

    static const ::com::sun::star::uno::Sequence< sal_Int8 >& getUnoTunnelId();

    // XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething(
            const ::com::sun::star::uno::Sequence< sal_Int8 >& rIdentifier);

    // XPropertySet
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertySetInfo > SAL_CALL
        getPropertySetInfo();
    virtual void SAL_CALL setPropertyValue(
            const ::rtl::OUString& rPropertyName,
            const ::com::sun::star::uno::Any& rValue);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
            const ::rtl::OUString& rPropertyName);
    virtual void SAL_CALL addPropertyChangeListener(
            const ::rtl::OUString& rPropertyName,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertyChangeListener >& xListener);
    virtual void SAL_CALL removePropertyChangeListener(
            const ::rtl::OUString& rPropertyName,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertyChangeListener >& xListener);
    virtual void SAL_CALL addVetoableChangeListener(
            const ::rtl::OUString& rPropertyName,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XVetoableChangeListener >& xListener);
    virtual void SAL_CALL removeVetoableChangeListener(
            const ::rtl::OUString& rPropertyName,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XVetoableChangeListener >& xListener);

    // XTextRange
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >
        SAL_CALL getText();
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL getStart();
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL getEnd();
    virtual ::rtl::OUString SAL_CALL getString();
    virtual void SAL_CALL setString(const ::rtl::OUString& rString);

    // XSimpleText
    virtual void SAL_CALL insertString(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xRange,
            const ::rtl::OUString& aString, sal_Bool bAbsorb);
    virtual void SAL_CALL insertControlCharacter(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xRange,
            sal_Int16 nControlCharacter, sal_Bool bAbsorb);

    // XText
    virtual void SAL_CALL insertTextContent(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xRange,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent > & xContent,
            sal_Bool bAbsorb);
    virtual void SAL_CALL removeTextContent(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent > & xContent);

    // XParagraphAppend
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL
        appendParagraph(
            const ::com::sun::star::uno::Sequence<
                    ::com::sun::star::beans::PropertyValue >&
                rCharacterAndParagraphProperties);
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL
        finishParagraph(
            const ::com::sun::star::uno::Sequence<
                    ::com::sun::star::beans::PropertyValue >&
                rCharacterAndParagraphProperties);

    // XTextPortionAppend
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL
        appendTextPortion(
            const ::rtl::OUString& rText,
            const ::com::sun::star::uno::Sequence<
                    ::com::sun::star::beans::PropertyValue >&
                rCharacterAndParagraphProperties);

    // XTextContentAppend
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL
        appendTextContent(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent >& xTextContent,
            const ::com::sun::star::uno::Sequence<
                    ::com::sun::star::beans::PropertyValue >&
                rCharacterAndParagraphProperties);

    // XTextConvert
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent > SAL_CALL
        convertToTextFrame(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange >& xStart,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange >& xEnd,
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::beans::PropertyValue >& xFrameProperties);
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextTable > SAL_CALL
        convertToTable(
            ::com::sun::star::uno::Sequence<
                ::com::sun::star::uno::Sequence<
                    ::com::sun::star::uno::Sequence<
                        ::com::sun::star::uno::Reference<
                            ::com::sun::star::text::XTextRange > > > > const&
                rTableRanges,
           ::com::sun::star::uno::Sequence<
                ::com::sun::star::uno::Sequence<
                    ::com::sun::star::uno::Sequence<
                        ::com::sun::star::beans::PropertyValue > > > const&
                rCellProperties,
           ::com::sun::star::uno::Sequence<
                ::com::sun::star::uno::Sequence<
                    ::com::sun::star::beans::PropertyValue > > const&
                rRowProperties,
           ::com::sun::star::uno::Sequence<
                ::com::sun::star::beans::PropertyValue > const&
                rTableProperties);

    // XTextCopy
    virtual void SAL_CALL copyText(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextCopy >& xSource );

    // XTextRangeCompare
    sal_Int16 SAL_CALL compareRegionStarts(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange >& xR1,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange >& xR2);
    sal_Int16 SAL_CALL compareRegionEnds(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange >& xR1,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange >& xR2);

    // XRelativeTextContentInsert
    virtual void SAL_CALL insertTextContentBefore(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent>& xNewContent,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent>& xSuccessor);
    virtual void SAL_CALL insertTextContentAfter(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent>& xNewContent,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent>& xPredecessor);

    // XRelativeTextContentRemove
    virtual void SAL_CALL removeTextContentBefore(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent>& xSuccessor);
    virtual void SAL_CALL removeTextContentAfter(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent>& xPredecessor);
};

#endif // SW_UNOTEXT_HXX
