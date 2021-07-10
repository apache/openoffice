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
#include "precompiled_sw.hxx"

#include <unoparagraph.hxx>
#include <cmdid.h>
#include <unomid.h>
#include <unoparaframeenum.hxx>
#include <unotext.hxx>
#include <unotextrange.hxx>
#include <unoport.hxx>
#include <unomap.hxx>
#include <unocrsr.hxx>
#include <unoprnms.hxx>
#include <unocrsrhelper.hxx>
#include <doc.hxx>
#include <ndtxt.hxx>
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include <docsh.hxx>

#define _SVSTDARR_USHORTS
#define _SVSTDARR_USHORTSSORT
#include <svl/svstdarr.hxx>

#include <com/sun/star/beans/SetPropertyTolerantFailed.hpp>
#include <com/sun/star/beans/GetPropertyTolerantResult.hpp>
#include <com/sun/star/beans/TolerantPropertySetResultType.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/text/WrapTextMode.hpp>
#include <com/sun/star/text/TextContentAnchorType.hpp>

//UUUU
#include <swunohelper.hxx>
#include <svx/unobrushitemhelper.hxx>
#include <editeng/unoipset.hxx>
#include <svx/xflbstit.hxx>
#include <svx/xflbmtit.hxx>
#include <com/sun/star/drawing/BitmapMode.hpp>

using namespace ::com::sun::star;
using ::rtl::OUString;

/* -----------------------------01.12.00 18:09--------------------------------

 ---------------------------------------------------------------------------*/
class SwParaSelection
{
    SwCursor & m_rCursor;
public:
    SwParaSelection(SwCursor & rCursor);
    ~SwParaSelection();
};

SwParaSelection::SwParaSelection(SwCursor & rCursor)
    : m_rCursor(rCursor)
{
    if (m_rCursor.HasMark())
    {
        m_rCursor.DeleteMark();
    }
    // is it at the start?
    if (m_rCursor.GetPoint()->nContent != 0)
    {
        m_rCursor.MovePara(fnParaCurr, fnParaStart);
    }
    // or at the end already?
    if (m_rCursor.GetPoint()->nContent != m_rCursor.GetCntntNode()->Len())
    {
        m_rCursor.SetMark();
        m_rCursor.MovePara(fnParaCurr, fnParaEnd);
    }
}

SwParaSelection::~SwParaSelection()
{
    if (m_rCursor.GetPoint()->nContent != 0)
    {
        m_rCursor.DeleteMark();
        m_rCursor.MovePara(fnParaCurr, fnParaStart);
    }
}


/******************************************************************
 * forward declarations
 ******************************************************************/

beans::PropertyState lcl_SwXParagraph_getPropertyState(
                            const SwTxtNode& rTxtNode,
                            const SwAttrSet** ppSet,
                            const SfxItemPropertySimpleEntry& rEntry,
                            sal_Bool &rAttrSetFetched )
    throw (beans::UnknownPropertyException);

/******************************************************************
 * SwXParagraph
 ******************************************************************/

class SwXParagraph::Impl
    : public SwClient
{

public:
    SwXParagraph &              m_rThis;
    SwEventListenerContainer    m_ListenerContainer;
    SfxItemPropertySet const&   m_rPropSet;
    bool                        m_bIsDescriptor;
    sal_Int32                   m_nSelectionStartPos;
    sal_Int32                   m_nSelectionEndPos;
    ::rtl::OUString             m_sText;
    uno::Reference<text::XText> m_xParentText;

    Impl(   SwXParagraph & rThis,
            SwTxtNode *const pTxtNode = 0,
            uno::Reference< text::XText > const & xParent = 0,
            const sal_Int32 nSelStart = -1, const sal_Int32 nSelEnd = -1)
        : SwClient(pTxtNode)
        , m_rThis(rThis)
        , m_ListenerContainer(static_cast< ::cppu::OWeakObject* >(&rThis))
        , m_rPropSet(*aSwMapProvider.GetPropertySet(PROPERTY_MAP_PARAGRAPH))
        // #i111177# unxsols4 (Sun C++ 5.9 SunOS_sparc) may generate wrong code
        , m_bIsDescriptor((0 == pTxtNode) ? true : false)
        , m_nSelectionStartPos(nSelStart)
        , m_nSelectionEndPos(nSelEnd)
        , m_xParentText(xParent)
    {
    }

    const SwTxtNode * GetTxtNode() const {
        return static_cast<const SwTxtNode*>(GetRegisteredIn());
    }
          SwTxtNode * GetTxtNode()       {
        return static_cast<SwTxtNode*>(GetRegisteredInNonConst());
    }

    SwTxtNode & GetTxtNodeOrThrow() {
        SwTxtNode *const pTxtNode( GetTxtNode() );
        if (!pTxtNode) {
            throw uno::RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                    "SwXParagraph: disposed or invalid")), 0);
        }
        return *pTxtNode;
    }

    bool IsDescriptor() const { return m_bIsDescriptor; }

    void SetPropertyValues_Impl(
            const uno::Sequence< ::rtl::OUString >& rPropertyNames,
            const uno::Sequence< uno::Any >& rValues)
        throw (beans::UnknownPropertyException, beans::PropertyVetoException,
                lang::IllegalArgumentException, lang::WrappedTargetException,
                uno::RuntimeException);

    uno::Sequence< uno::Any >
        GetPropertyValues_Impl(
            const uno::Sequence< ::rtl::OUString >& rPropertyNames)
        throw (beans::UnknownPropertyException, lang::WrappedTargetException,
                uno::RuntimeException);

    //UUUU
    void GetSinglePropertyValue_Impl(
        const SfxItemPropertySimpleEntry& rEntry,
        const SfxItemSet& rSet,
        uno::Any& rAny ) const
    throw(uno::RuntimeException);

    uno::Sequence< beans::GetDirectPropertyTolerantResult >
        GetPropertyValuesTolerant_Impl(
            const uno::Sequence< ::rtl::OUString >& rPropertyNames,
            bool bDirectValuesOnly)
        throw (uno::RuntimeException);
protected:
    // SwClient
    virtual void Modify(const SfxPoolItem *pOld, const SfxPoolItem *pNew);

};

/*-- 11.12.98 08:12:58---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::Impl::Modify( const SfxPoolItem *pOld, const SfxPoolItem *pNew )
{
    ClientModify(this, pOld, pNew);
    if (!GetRegisteredIn())
    {
        m_ListenerContainer.Disposing();
    }
}

/*-- 11.12.98 08:12:47---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraph::SwXParagraph()
    : m_pImpl( new SwXParagraph::Impl(*this) )
{
}

/*-- 11.12.98 08:12:47---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraph::SwXParagraph(
        uno::Reference< text::XText > const & xParent,
        SwTxtNode & rTxtNode,
        const sal_Int32 nSelStart, const sal_Int32 nSelEnd)
    : m_pImpl(
        new SwXParagraph::Impl(*this, &rTxtNode, xParent, nSelStart, nSelEnd))
{
}

/*-- 11.12.98 08:12:48---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraph::~SwXParagraph()
{
}

const SwTxtNode * SwXParagraph::GetTxtNode() const
{
    return m_pImpl->GetTxtNode();
}

bool SwXParagraph::IsDescriptor() const
{
    return m_pImpl->IsDescriptor();
}

uno::Reference<text::XTextContent>
SwXParagraph::CreateXParagraph(SwDoc & rDoc, SwTxtNode& rTxtNode,
        uno::Reference< text::XText> const& i_xParent,
        const sal_Int32 nSelStart, const sal_Int32 nSelEnd)
{
    // re-use existing SwXParagraph
    // #i105557#: do not iterate over the registered clients: race condition
    uno::Reference<text::XTextContent> xParagraph;
    if ((-1 == nSelStart) && (-1 == nSelEnd)) // only use cache if no selection!
    {
        xParagraph.set(rTxtNode.GetXParagraph());
    }
    if (xParagraph.is())
    {
        return xParagraph;
    }

    // create new SwXParagraph
    uno::Reference<text::XText> xParentText(i_xParent);
    if (!xParentText.is())
    {
        SwPosition Pos( rTxtNode );
        xParentText.set(::sw::CreateParentXText( rDoc, Pos ));
    }
    SwXParagraph *const pXPara(
            new SwXParagraph(xParentText, rTxtNode, nSelStart, nSelEnd) );
    // this is why the constructor is private: need to acquire pXPara here
    xParagraph.set(pXPara);
    // in order to initialize the weak pointer cache in the core object
    if ((-1 == nSelStart) && (-1 == nSelEnd))
    {
        rTxtNode.SetXParagraph(xParagraph);
    }
    return xParagraph;
}

bool SwXParagraph::SelectPaM(SwPaM & rPaM)
{
    SwTxtNode const*const pTxtNode( GetTxtNode() );

    if (!pTxtNode)
    {
        return false;
    }

    *rPaM.GetPoint() = SwPosition( *pTxtNode );
    // set selection to the whole paragraph
    rPaM.SetMark();
    rPaM.GetMark()->nContent = pTxtNode->GetTxt().Len();
    return true;
}

/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXParagraph::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
	return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL
SwXParagraph::getSomething(const uno::Sequence< sal_Int8 >& rId)
throw (uno::RuntimeException)
{
    return ::sw::UnoTunnelImpl<SwXParagraph>(rId, this);
}

/* -----------------------------06.04.00 16:37--------------------------------

 ---------------------------------------------------------------------------*/
OUString SAL_CALL
SwXParagraph::getImplementationName() throw (uno::RuntimeException)
{
	return C2U("SwXParagraph");
}
/* -----------------------------06.04.00 16:37--------------------------------

 ---------------------------------------------------------------------------*/
static char const*const g_ServicesParagraph[] =
{
    "com.sun.star.text.TextContent",
    "com.sun.star.text.Paragraph",
    "com.sun.star.style.CharacterProperties",
    "com.sun.star.style.CharacterPropertiesAsian",
    "com.sun.star.style.CharacterPropertiesComplex",
    "com.sun.star.style.ParagraphProperties",
    "com.sun.star.style.ParagraphPropertiesAsian",
    "com.sun.star.style.ParagraphPropertiesComplex",
};
static const size_t g_nServicesParagraph(
    sizeof(g_ServicesParagraph)/sizeof(g_ServicesParagraph[0]));

sal_Bool SAL_CALL
SwXParagraph::supportsService(const OUString& rServiceName)
throw (uno::RuntimeException)
{
    return ::sw::SupportsServiceImpl(
            g_nServicesParagraph, g_ServicesParagraph, rServiceName);
}
/* -----------------------------06.04.00 16:37--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SAL_CALL
SwXParagraph::getSupportedServiceNames() throw (uno::RuntimeException)
{
    return ::sw::GetSupportedServiceNamesImpl(
            g_nServicesParagraph, g_ServicesParagraph);
}

/* -----------------------------11.07.00 14:48--------------------------------

 ---------------------------------------------------------------------------*/
void
SwXParagraph::attachToText(SwXText & rParent, SwTxtNode & rTxtNode)
{
    DBG_ASSERT(m_pImpl->m_bIsDescriptor, "Paragraph is not a descriptor");
    if (m_pImpl->m_bIsDescriptor)
    {
        m_pImpl->m_bIsDescriptor = false;
        rTxtNode.Add(m_pImpl.get());
        rTxtNode.SetXParagraph(uno::Reference<text::XTextContent>(this));
        m_pImpl->m_xParentText = &rParent;
        if (m_pImpl->m_sText.getLength())
        {
            try { setString(m_pImpl->m_sText); }
			catch(...){}
            m_pImpl->m_sText = OUString();
        }
    }
}

/*-- 11.12.98 08:12:49---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo > SAL_CALL
SwXParagraph::getPropertySetInfo()
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    static uno::Reference< beans::XPropertySetInfo > xRef =
        m_pImpl->m_rPropSet.getPropertySetInfo();
	return xRef;
}
/*-- 11.12.98 08:12:49---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXParagraph::setPropertyValue(const OUString& rPropertyName,
        const uno::Any& rValue)
throw (beans::UnknownPropertyException, beans::PropertyVetoException,
    lang::IllegalArgumentException, lang::WrappedTargetException,
    uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence<OUString> aPropertyNames(1);
    aPropertyNames.getArray()[0] = rPropertyName;
    uno::Sequence<uno::Any> aValues(1);
    aValues.getArray()[0] = rValue;
    m_pImpl->SetPropertyValues_Impl( aPropertyNames, aValues );
}

/*-- 11.12.98 08:12:49---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any
SwXParagraph::getPropertyValue(const OUString& rPropertyName)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException )
{
	vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence<OUString> aPropertyNames(1);
    aPropertyNames.getArray()[0] = rPropertyName;
    const uno::Sequence< uno::Any > aRet =
        m_pImpl->GetPropertyValues_Impl(aPropertyNames);
    return aRet.getConstArray()[0];
}
/* -----------------------------02.04.01 11:43--------------------------------

 ---------------------------------------------------------------------------*/
void SwXParagraph::Impl::SetPropertyValues_Impl(
    const uno::Sequence< OUString >& rPropertyNames,
    const uno::Sequence< uno::Any >& rValues )
throw (beans::UnknownPropertyException, beans::PropertyVetoException,
    lang::IllegalArgumentException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    SwTxtNode & rTxtNode(GetTxtNodeOrThrow());

    SwPosition aPos( rTxtNode );
    SwCursor aCursor( aPos, 0, false );
    const OUString* pPropertyNames = rPropertyNames.getConstArray();
    const uno::Any* pValues = rValues.getConstArray();
    SfxItemPropertyMap const*const pMap = m_rPropSet.getPropertyMap();
    SwParaSelection aParaSel( aCursor );
    for (sal_Int32 nProp = 0; nProp < rPropertyNames.getLength(); nProp++)
    {
        SfxItemPropertySimpleEntry const*const pEntry =
            pMap->getByName( pPropertyNames[nProp] );
        if (!pEntry)
        {
            throw beans::UnknownPropertyException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown property: "))
                    + pPropertyNames[nProp],
                static_cast< cppu::OWeakObject * >(&m_rThis));
        }
        if (pEntry->nFlags & beans::PropertyAttribute::READONLY)
        {
            throw beans::PropertyVetoException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("Property is read-only: "))
                    + pPropertyNames[nProp],
                static_cast< cppu::OWeakObject * >(&m_rThis));
        }
        SwUnoCursorHelper::SetPropertyValue(aCursor, m_rPropSet,
                pPropertyNames[nProp], pValues[nProp]);
    }
}

void SAL_CALL SwXParagraph::setPropertyValues(
    const uno::Sequence< OUString >& rPropertyNames,
    const uno::Sequence< uno::Any >& rValues )
throw (beans::PropertyVetoException, lang::IllegalArgumentException,
    lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    // workaround for bad designed API
    try
    {
        m_pImpl->SetPropertyValues_Impl( rPropertyNames, rValues );
    }
    catch (beans::UnknownPropertyException &rException)
    {
        // wrap the original (here not allowed) exception in
        // a lang::WrappedTargetException that gets thrown instead.
        lang::WrappedTargetException aWExc;
        aWExc.TargetException <<= rException;
        throw aWExc;
    }
}

/* -----------------------------02.04.01 11:43--------------------------------

 ---------------------------------------------------------------------------*/

//UUUU Support for DrawingLayer FillStyles for GetPropertyValue() usages
void SwXParagraph::Impl::GetSinglePropertyValue_Impl(
    const SfxItemPropertySimpleEntry& rEntry,
    const SfxItemSet& rSet,
    uno::Any& rAny ) const
throw(uno::RuntimeException)
{
    bool bDone(false);

    switch(rEntry.nWID)
    {
        case RES_BACKGROUND:
        {
            const SvxBrushItem aOriginalBrushItem(getSvxBrushItemFromSourceSet(rSet, RES_BACKGROUND));
            const sal_uInt8 nMemberId(rEntry.nMemberId & (~SFX_METRIC_ITEM));

            if(!aOriginalBrushItem.QueryValue(rAny, nMemberId))
            {
                OSL_ENSURE(false, "Error getting attribute from RES_BACKGROUND (!)");
            }

            bDone = true;
            break;
        }
        case OWN_ATTR_FILLBMP_MODE:
        {
            const XFillBmpStretchItem* pStretchItem = dynamic_cast< const XFillBmpStretchItem* >(&rSet.Get(XATTR_FILLBMP_STRETCH));
            const XFillBmpTileItem* pTileItem = dynamic_cast< const XFillBmpTileItem* >(&rSet.Get(XATTR_FILLBMP_TILE));

            if( pTileItem && pTileItem->GetValue() )
            {
                rAny <<= drawing::BitmapMode_REPEAT;
            }
            else if( pStretchItem && pStretchItem->GetValue() )
            {
                rAny <<= drawing::BitmapMode_STRETCH;
            }
            else
            {
                rAny <<= drawing::BitmapMode_NO_REPEAT;
            }

            bDone = true;
            break;
        }
        default: break;
    }

    if(!bDone)
    {
        // fallback to standard get value implementation used before this helper was created
        m_rPropSet.getPropertyValue(rEntry, rSet, rAny);

        if(rEntry.pType && *(rEntry.pType) == ::getCppuType((const sal_Int16*)0) && *(rEntry.pType) != rAny.getValueType())
        {
            // since the sfx uInt16 item now exports a sal_Int32, we may have to fix this here
            sal_Int32 nValue(0);

            rAny >>= nValue;
            rAny <<= static_cast< sal_Int16 >(nValue);
        }

        //UUUU check for needed metric translation
        if(rEntry.nMemberId & SFX_METRIC_ITEM)
        {
            bool bDoIt(true);

            if(XATTR_FILLBMP_SIZEX == rEntry.nWID || XATTR_FILLBMP_SIZEY == rEntry.nWID)
            {
                // exception: If these ItemTypes are used, do not convert when these are negative
                // since this means they are intended as percent values
                sal_Int32 nValue = 0;

                if(rAny >>= nValue)
                {
                    bDoIt = nValue > 0;
                }
            }

            if(bDoIt)
            {
                const SfxMapUnit eMapUnit(rSet.GetPool()->GetMetric(rEntry.nWID));

                if(eMapUnit != SFX_MAPUNIT_100TH_MM)
                {
                    SvxUnoConvertToMM(eMapUnit, rAny);
                }
            }
        }
    }
}

uno::Sequence< uno::Any > SwXParagraph::Impl::GetPropertyValues_Impl(
        const uno::Sequence< OUString > & rPropertyNames )
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    SwTxtNode & rTxtNode(GetTxtNodeOrThrow());

    uno::Sequence< uno::Any > aValues(rPropertyNames.getLength());
    SwPosition aPos( rTxtNode );
    SwPaM aPam( aPos );
    uno::Any* pValues = aValues.getArray();
    const OUString* pPropertyNames = rPropertyNames.getConstArray();
    SfxItemPropertyMap const*const pMap = m_rPropSet.getPropertyMap();
    const SwAttrSet& rAttrSet( rTxtNode.GetSwAttrSet() );
    for (sal_Int32 nProp = 0; nProp < rPropertyNames.getLength(); nProp++)
    {
        SfxItemPropertySimpleEntry const*const pEntry =
            pMap->getByName( pPropertyNames[nProp] );
        if (!pEntry)
        {
            throw beans::UnknownPropertyException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown property: "))
                    + pPropertyNames[nProp],
                static_cast< cppu::OWeakObject * >(&m_rThis));
        }
        if (! ::sw::GetDefaultTextContentValue(
                pValues[nProp], pPropertyNames[nProp], pEntry->nWID))
        {
            beans::PropertyState eTemp;
            const bool bDone = SwUnoCursorHelper::getCrsrPropertyValue(
                *pEntry, aPam, &(pValues[nProp]), eTemp, &rTxtNode );
            if (!bDone)
            {
                //UUUU
                GetSinglePropertyValue_Impl(*pEntry, rAttrSet, pValues[nProp]);
            }
        }
    }
    return aValues;
}

/* -----------------------------04.11.03 11:43--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Any > SAL_CALL
SwXParagraph::getPropertyValues(const uno::Sequence< OUString >& rPropertyNames)
throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence< uno::Any > aValues;

    // workaround for bad designed API
    try
    {
        aValues = m_pImpl->GetPropertyValues_Impl( rPropertyNames );
    }
    catch (beans::UnknownPropertyException &)
    {
        throw uno::RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                "Unknown property exception caught")),
            static_cast<cppu::OWeakObject *>(this));
    }
    catch (lang::WrappedTargetException &)
    {
        throw uno::RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                "WrappedTargetException caught")),
            static_cast<cppu::OWeakObject *>(this));
    }

    return aValues;
}

/* -----------------------------02.04.01 11:43--------------------------------

 ---------------------------------------------------------------------------*/
void SAL_CALL SwXParagraph::addPropertiesChangeListener(
    const uno::Sequence< OUString >& /*aPropertyNames*/,
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
throw (uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXParagraph::addPropertiesChangeListener(): not implemented");
}
/* -----------------------------02.04.01 11:43--------------------------------

 ---------------------------------------------------------------------------*/
void SAL_CALL SwXParagraph::removePropertiesChangeListener(
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
throw (uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXParagraph::removePropertiesChangeListener(): not implemented");
}
/* -----------------------------02.04.01 11:43--------------------------------

 ---------------------------------------------------------------------------*/
void SAL_CALL SwXParagraph::firePropertiesChangeEvent(
    const uno::Sequence< OUString >& /*aPropertyNames*/,
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
        throw(uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXParagraph::firePropertiesChangeEvent(): not implemented");
}
/* -----------------------------25.09.03 11:09--------------------------------

 ---------------------------------------------------------------------------*/

/* disabled for #i46921# */

uno::Sequence< beans::SetPropertyTolerantFailed > SAL_CALL
SwXParagraph::setPropertyValuesTolerant(
        const uno::Sequence< OUString >& rPropertyNames,
        const uno::Sequence< uno::Any >& rValues )
throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    if (rPropertyNames.getLength() != rValues.getLength())
    {
        throw lang::IllegalArgumentException();
    }

    SwTxtNode & rTxtNode(m_pImpl->GetTxtNodeOrThrow());

    //SwNode& rTxtNode = pUnoCrsr->GetPoint()->nNode.GetNode();
    //const SwAttrSet& rAttrSet = ((SwTxtNode&)rTxtNode).GetSwAttrSet();
    //sal_uInt16 nAttrCount = rAttrSet.Count();

    const sal_Int32 nProps = rPropertyNames.getLength();
    const OUString *pProp = rPropertyNames.getConstArray();

    //sal_Int32 nVals = rValues.getLength();
    const uno::Any *pValue = rValues.getConstArray();

    sal_Int32 nFailed = 0;
    uno::Sequence< beans::SetPropertyTolerantFailed > aFailed( nProps );
    beans::SetPropertyTolerantFailed *pFailed = aFailed.getArray();

    // get entry to start with
    SfxItemPropertyMap const*const pPropMap =
        m_pImpl->m_rPropSet.getPropertyMap();

    OUString sTmp;
    SwPosition aPos( rTxtNode );
    SwCursor aCursor( aPos, 0, false );
    SwParaSelection aParaSel( aCursor );
    for (sal_Int32 i = 0;  i < nProps;  ++i)
    {
        try
        {
            pFailed[ nFailed ].Name = pProp[i];

            SfxItemPropertySimpleEntry const*const pEntry =
                pPropMap->getByName( pProp[i] );
            if (!pEntry)
            {
                pFailed[ nFailed++ ].Result  =
                    beans::TolerantPropertySetResultType::UNKNOWN_PROPERTY;
            }
            else
            {
                // set property value
                // (compare to SwXParagraph::setPropertyValues)
                if (pEntry->nFlags & beans::PropertyAttribute::READONLY)
                {
                    pFailed[ nFailed++ ].Result  =
                        beans::TolerantPropertySetResultType::PROPERTY_VETO;
                }
                else
                {
                    SwUnoCursorHelper::SetPropertyValue(
                        aCursor, m_pImpl->m_rPropSet, pProp[i], pValue[i]);
                }
            }
        }
        catch (beans::UnknownPropertyException &)
        {
            // should not occur because property was searched for before
            DBG_ERROR( "unexpected exception caught" );
            pFailed[ nFailed++ ].Result =
                beans::TolerantPropertySetResultType::UNKNOWN_PROPERTY;
        }
        catch (lang::IllegalArgumentException &)
        {
            pFailed[ nFailed++ ].Result =
                beans::TolerantPropertySetResultType::ILLEGAL_ARGUMENT;
        }
        catch (beans::PropertyVetoException &)
        {
            pFailed[ nFailed++ ].Result =
                beans::TolerantPropertySetResultType::PROPERTY_VETO;
        }
        catch (lang::WrappedTargetException &)
        {
            pFailed[ nFailed++ ].Result =
                beans::TolerantPropertySetResultType::WRAPPED_TARGET;
        }
    }

    aFailed.realloc( nFailed );
    return aFailed;
}


uno::Sequence< beans::GetPropertyTolerantResult > SAL_CALL
SwXParagraph::getPropertyValuesTolerant(
        const uno::Sequence< OUString >& rPropertyNames )
throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    uno::Sequence< beans::GetDirectPropertyTolerantResult > aTmpRes(
        m_pImpl->GetPropertyValuesTolerant_Impl( rPropertyNames, false ) );
    const beans::GetDirectPropertyTolerantResult *pTmpRes =
        aTmpRes.getConstArray();

    // copy temporary result to final result type
    const sal_Int32 nLen = aTmpRes.getLength();
    uno::Sequence< beans::GetPropertyTolerantResult > aRes( nLen );
    beans::GetPropertyTolerantResult *pRes = aRes.getArray();
    for (sal_Int32 i = 0;  i < nLen;  i++)
    {
        *pRes++ = *pTmpRes++;
    }
    return aRes;
}


uno::Sequence< beans::GetDirectPropertyTolerantResult > SAL_CALL
SwXParagraph::getDirectPropertyValuesTolerant(
        const uno::Sequence< OUString >& rPropertyNames )
throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    return m_pImpl->GetPropertyValuesTolerant_Impl( rPropertyNames, true );
}


uno::Sequence< beans::GetDirectPropertyTolerantResult >
SwXParagraph::Impl::GetPropertyValuesTolerant_Impl(
        const uno::Sequence< OUString >& rPropertyNames,
        bool bDirectValuesOnly )
throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    SwTxtNode & rTxtNode(GetTxtNodeOrThrow());

    // #i46786# Use SwAttrSet pointer for determining the state.
    //          Use the value SwAttrSet (from the paragraph OR the style)
    //          for determining the actual value(s).
    const SwAttrSet* pAttrSet = rTxtNode.GetpSwAttrSet();
    const SwAttrSet& rValueAttrSet = rTxtNode.GetSwAttrSet();

    sal_Int32 nProps = rPropertyNames.getLength();
    const OUString *pProp = rPropertyNames.getConstArray();

    uno::Sequence< beans::GetDirectPropertyTolerantResult > aResult( nProps );
    beans::GetDirectPropertyTolerantResult *pResult = aResult.getArray();
    sal_Int32 nIdx = 0;

    // get entry to start with
    SfxItemPropertyMap const*const pPropMap = m_rPropSet.getPropertyMap();

    for (sal_Int32 i = 0;  i < nProps;  ++i)
    {
        DBG_ASSERT( nIdx < nProps, "index out ouf bounds" );
        beans::GetDirectPropertyTolerantResult &rResult = pResult[nIdx];

        try
        {
            rResult.Name = pProp[i];

            SfxItemPropertySimpleEntry const*const pEntry =
                pPropMap->getByName( pProp[i] );
            if (!pEntry)  // property available?
            {
                rResult.Result =
                    beans::TolerantPropertySetResultType::UNKNOWN_PROPERTY;
            }
            else
            {
                // get property state
                // (compare to SwXParagraph::getPropertyState)
                sal_Bool bAttrSetFetched = sal_True;
                beans::PropertyState eState = lcl_SwXParagraph_getPropertyState(
                            rTxtNode, &pAttrSet, *pEntry, bAttrSetFetched );
                rResult.State  = eState;

//                if (bDirectValuesOnly  &&  PropertyState_DIRECT_VALUE != eState)
//                    rResult.Result = beans::TolerantPropertySetResultType::NO_DIRECT_VALUE;
//                else
                rResult.Result = beans::TolerantPropertySetResultType::UNKNOWN_FAILURE;
                if (!bDirectValuesOnly ||
                    (beans::PropertyState_DIRECT_VALUE == eState))
                {
                    // get property value
                    // (compare to SwXParagraph::getPropertyValue(s))
                    uno::Any aValue;
                    if (! ::sw::GetDefaultTextContentValue(
                                aValue, pProp[i], pEntry->nWID ) )
                    {
                        SwPosition aPos( rTxtNode );
                        SwPaM aPam( aPos );
                        // handle properties that are not part of the attribute
                        // and thus only pretend to be paragraph attributes
                        beans::PropertyState eTemp;
                        const bool bDone =
                            SwUnoCursorHelper::getCrsrPropertyValue(
                                    *pEntry, aPam, &aValue, eTemp, &rTxtNode );

                        // if not found try the real paragraph attributes...
                        if (!bDone)
                        {
                            //UUUU
                            GetSinglePropertyValue_Impl(*pEntry, rValueAttrSet, aValue);
                        }
                    }

                    rResult.Value  = aValue;
                    rResult.Result = beans::TolerantPropertySetResultType::SUCCESS;

                    nIdx++;
                }
                // this assertion should never occur!
                DBG_ASSERT( nIdx < 1  ||  pResult[nIdx - 1].Result != beans::TolerantPropertySetResultType::UNKNOWN_FAILURE,
                        "unknown failure while retrieving property" );

            }
        }
        catch (beans::UnknownPropertyException &)
        {
            // should not occur because property was searched for before
            DBG_ERROR( "unexpected exception caught" );
            rResult.Result = beans::TolerantPropertySetResultType::UNKNOWN_PROPERTY;
        }
        catch (lang::IllegalArgumentException &)
        {
            rResult.Result = beans::TolerantPropertySetResultType::ILLEGAL_ARGUMENT;
        }
        catch (beans::PropertyVetoException &)
        {
            rResult.Result = beans::TolerantPropertySetResultType::PROPERTY_VETO;
        }
        catch (lang::WrappedTargetException &)
        {
            rResult.Result = beans::TolerantPropertySetResultType::WRAPPED_TARGET;
        }
    }

    // resize to actually used size
    aResult.realloc( nIdx );

    return aResult;
}

/* -----------------------------12.09.00 11:09--------------------------------

 ---------------------------------------------------------------------------*/
bool ::sw::GetDefaultTextContentValue(
        uno::Any& rAny, const OUString& rPropertyName, sal_uInt16 nWID)
{
	if(!nWID)
	{
		if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_ANCHOR_TYPE)))
			nWID = FN_UNO_ANCHOR_TYPE;
		else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_ANCHOR_TYPES)))
			nWID = FN_UNO_ANCHOR_TYPES;
		else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_TEXT_WRAP)))
			nWID = FN_UNO_TEXT_WRAP;
		else
			return sal_False;
	}

	switch(nWID)
	{
		case FN_UNO_TEXT_WRAP:  rAny <<= text::WrapTextMode_NONE; break;
		case FN_UNO_ANCHOR_TYPE: rAny <<= text::TextContentAnchorType_AT_PARAGRAPH; break;
		case FN_UNO_ANCHOR_TYPES:
		{	uno::Sequence<text::TextContentAnchorType> aTypes(1);
			text::TextContentAnchorType* pArray = aTypes.getArray();
			pArray[0] = text::TextContentAnchorType_AT_PARAGRAPH;
			rAny.setValue(&aTypes, ::getCppuType((uno::Sequence<text::TextContentAnchorType>*)0));
		}
		break;
		default:
			return sal_False;
	}
	return sal_True;
}
/*-- 11.12.98 08:12:50---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXParagraph::addPropertyChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXParagraph::addPropertyChangeListener(): not implemented");
}

/*-- 11.12.98 08:12:50---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXParagraph::removePropertyChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXParagraph::removePropertyChangeListener(): not implemented");
}

/*-- 11.12.98 08:12:50---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXParagraph::addVetoableChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXParagraph::addVetoableChangeListener(): not implemented");
}

/*-- 11.12.98 08:12:51---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXParagraph::removeVetoableChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXParagraph::removeVetoableChangeListener(): not implemented");
}

//-----------------------------------------------------------------------------
beans::PropertyState lcl_SwXParagraph_getPropertyState(
    // SwUnoCrsr& rUnoCrsr,
    const SwTxtNode& rTxtNode,
    const SwAttrSet** ppSet,
    const SfxItemPropertySimpleEntry& rEntry,
    sal_Bool &rAttrSetFetched)
    throw (beans::UnknownPropertyException)
{
    beans::PropertyState eRet(beans::PropertyState_DEFAULT_VALUE);

    if(!(*ppSet) && !rAttrSetFetched)
    {
        (*ppSet) = rTxtNode.GetpSwAttrSet();
        rAttrSetFetched = sal_True;
    }

    SwPosition aPos(rTxtNode);
    SwPaM aPam(aPos);
    bool bDone(false);

    switch(rEntry.nWID)
    {
        case FN_UNO_NUM_RULES:
        {
            // if numbering is set, return it; else do nothing
            SwUnoCursorHelper::getNumberingProperty(aPam,eRet,NULL);
            bDone = true;
            break;
        }
        case FN_UNO_ANCHOR_TYPES:
        {
            bDone = true;
            break;
        }
        case RES_ANCHOR:
        {
            bDone = (MID_SURROUND_SURROUNDTYPE == rEntry.nMemberId);
            break;
        }
        case RES_SURROUND:
        {
            bDone = (MID_ANCHOR_ANCHORTYPE == rEntry.nMemberId);
            break;
        }
        case FN_UNO_PARA_STYLE:
        case FN_UNO_PARA_CONDITIONAL_STYLE_NAME:
        {
            SwFmtColl* pFmt = SwUnoCursorHelper::GetCurTxtFmtColl(aPam,rEntry.nWID == FN_UNO_PARA_CONDITIONAL_STYLE_NAME);
            eRet = pFmt ? beans::PropertyState_DIRECT_VALUE : beans::PropertyState_AMBIGUOUS_VALUE;
            bDone = true;
            break;
        }
        case FN_UNO_PAGE_STYLE:
        {
            String sVal;
            SwUnoCursorHelper::GetCurPageStyle(aPam,sVal);
            eRet = sVal.Len() ? beans::PropertyState_DIRECT_VALUE : beans::PropertyState_AMBIGUOUS_VALUE;
            bDone = true;
            break;
        }

        //UUUU DrawingLayer PropertyStyle support
        case OWN_ATTR_FILLBMP_MODE:
        {
            if(*ppSet)
            {
                if(SFX_ITEM_SET == (*ppSet)->GetItemState(XATTR_FILLBMP_STRETCH, false)
                    || SFX_ITEM_SET == (*ppSet)->GetItemState(XATTR_FILLBMP_TILE, false))
                {
                    eRet = beans::PropertyState_DIRECT_VALUE;
                }
                else
                {
                    eRet = beans::PropertyState_AMBIGUOUS_VALUE;
                }

                bDone = true;
            }
            break;
        }
        case RES_BACKGROUND:
        {
            if(*ppSet)
            {
                if(SWUnoHelper::needToMapFillItemsToSvxBrushItemTypes(**ppSet))
                {
                    eRet = beans::PropertyState_DIRECT_VALUE;
                    bDone = true;
                }
            }
            break;
        }
    }

    if(!bDone)
    {
        if((*ppSet) && SFX_ITEM_SET == (*ppSet)->GetItemState(rEntry.nWID,sal_False))
        {
            eRet = beans::PropertyState_DIRECT_VALUE;
        }
    }

    return eRet;
}

/*-- 05.03.99 11:37:30---------------------------------------------------

  -----------------------------------------------------------------------*/
beans::PropertyState SAL_CALL
SwXParagraph::getPropertyState(const OUString& rPropertyName)
throw (beans::UnknownPropertyException, uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());

    SwTxtNode & rTxtNode(m_pImpl->GetTxtNodeOrThrow());

    const SwAttrSet* pSet = 0;
    SfxItemPropertySimpleEntry const*const pEntry =
        m_pImpl->m_rPropSet.getPropertyMap()->getByName(rPropertyName);
    if (!pEntry)
    {
        throw beans::UnknownPropertyException(
            OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown property: "))
                + rPropertyName,
            static_cast<cppu::OWeakObject *>(this));
    }
    sal_Bool bDummy = sal_False;
    const beans::PropertyState eRet =
        lcl_SwXParagraph_getPropertyState(rTxtNode, &pSet, *pEntry, bDummy);
	return eRet;
}
/*-- 05.03.99 11:37:32---------------------------------------------------

  -----------------------------------------------------------------------*/

uno::Sequence< beans::PropertyState > SAL_CALL
SwXParagraph::getPropertyStates(
		const uno::Sequence< OUString >& PropertyNames)
throw (beans::UnknownPropertyException, uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());

    SwTxtNode & rTxtNode(m_pImpl->GetTxtNodeOrThrow());

	const OUString* pNames = PropertyNames.getConstArray();
	uno::Sequence< beans::PropertyState > aRet(PropertyNames.getLength());
	beans::PropertyState* pStates = aRet.getArray();
    SfxItemPropertyMap const*const pMap = m_pImpl->m_rPropSet.getPropertyMap();
    const SwAttrSet* pSet = 0;
    sal_Bool bAttrSetFetched = sal_False;

    for (sal_Int32 i = 0, nEnd = PropertyNames.getLength(); i < nEnd;
            ++i, ++pStates, ++pNames)
    {
        SfxItemPropertySimpleEntry const*const pEntry =
            pMap->getByName( *pNames );
        if (!pEntry)
        {
            throw beans::UnknownPropertyException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown property: "))
                    + *pNames,
                static_cast<cppu::OWeakObject *>(this));
        }

        if (bAttrSetFetched && !pSet && isATR(pEntry->nWID))
        {
            *pStates = beans::PropertyState_DEFAULT_VALUE;
        }
        else
        {
            *pStates = lcl_SwXParagraph_getPropertyState(
                rTxtNode, &pSet, *pEntry, bAttrSetFetched );
        }
    }

	return aRet;
}

/*-- 05.03.99 11:37:33---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXParagraph::setPropertyToDefault(const OUString& rPropertyName)
throw (beans::UnknownPropertyException, uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());

    SwTxtNode & rTxtNode(m_pImpl->GetTxtNodeOrThrow());

    SwPosition aPos( rTxtNode );
    SwCursor aCursor( aPos, 0, false );
    if (rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_ANCHOR_TYPE))  ||
        rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_ANCHOR_TYPES)) ||
        rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_TEXT_WRAP)))
    {
        return;
    }

    // select paragraph
    SwParaSelection aParaSel( aCursor );
    SfxItemPropertySimpleEntry const*const pEntry =
        m_pImpl->m_rPropSet.getPropertyMap()->getByName( rPropertyName );
    if (!pEntry)
    {
        throw beans::UnknownPropertyException(
            OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown property: "))
                + rPropertyName,
            static_cast<cppu::OWeakObject *>(this));
    }

    if (pEntry->nFlags & beans::PropertyAttribute::READONLY)
    {
        throw uno::RuntimeException(
            OUString(RTL_CONSTASCII_USTRINGPARAM("Property is read-only: "))
                + rPropertyName,
            static_cast<cppu::OWeakObject *>(this));
    }

    const bool bBelowFrmAtrEnd(pEntry->nWID < RES_FRMATR_END);
    const bool bDrawingLayerRange(XATTR_FILL_FIRST <= pEntry->nWID && XATTR_FILL_LAST >= pEntry->nWID);

    if(bBelowFrmAtrEnd || bDrawingLayerRange)
    {
        SvUShortsSort aWhichIds;

        //UUUU For FillBitmapMode two IDs have to be reset (!)
        if(OWN_ATTR_FILLBMP_MODE == pEntry->nWID)
        {
            aWhichIds.Insert(XATTR_FILLBMP_STRETCH);
            aWhichIds.Insert(XATTR_FILLBMP_TILE);
        }
        else
        {
            aWhichIds.Insert(pEntry->nWID);
        }

        if (pEntry->nWID < RES_PARATR_BEGIN)
        {
            aCursor.GetDoc()->ResetAttrs(aCursor, sal_True, &aWhichIds);
        }
        else
        {
            // for paragraph attributes the selection must be extended
            // to paragraph boundaries
            SwPosition aStart( *aCursor.Start() );
            SwPosition aEnd  ( *aCursor.End()   );
            ::std::auto_ptr<SwUnoCrsr> pTemp( aCursor.GetDoc()->CreateUnoCrsr(aStart, sal_False) );

            if(!SwUnoCursorHelper::IsStartOfPara(*pTemp))
            {
                pTemp->MovePara(fnParaCurr, fnParaStart);
            }

            pTemp->SetMark();
            *pTemp->GetPoint() = aEnd;
            //pTemp->Exchange();

            SwUnoCursorHelper::SelectPam(*pTemp, true);

            if (!SwUnoCursorHelper::IsEndOfPara(*pTemp))
            {
                pTemp->MovePara(fnParaCurr, fnParaEnd);
            }

            pTemp->GetDoc()->ResetAttrs(*pTemp, sal_True, &aWhichIds);
        }
    }
    else
    {
        SwUnoCursorHelper::resetCrsrPropertyValue(*pEntry, aCursor);
    }
}

/*-- 05.03.99 11:37:33---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SAL_CALL
SwXParagraph::getPropertyDefault(const OUString& rPropertyName)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    SwTxtNode & rTxtNode(m_pImpl->GetTxtNodeOrThrow());

	uno::Any aRet;
    if (::sw::GetDefaultTextContentValue(aRet, rPropertyName))
    {
        return aRet;
    }

    SfxItemPropertySimpleEntry const*const pEntry =
        m_pImpl->m_rPropSet.getPropertyMap()->getByName(rPropertyName);
    if (!pEntry)
    {
        throw beans::UnknownPropertyException(
            OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown property: "))
                + rPropertyName,
            static_cast<cppu::OWeakObject *>(this));
    }

    const bool bBelowFrmAtrEnd(pEntry->nWID < RES_FRMATR_END);
    const bool bDrawingLayerRange(XATTR_FILL_FIRST <= pEntry->nWID && XATTR_FILL_LAST >= pEntry->nWID);

    if(bBelowFrmAtrEnd || bDrawingLayerRange)
    {
        const SfxPoolItem& rDefItem = rTxtNode.GetDoc()->GetAttrPool().GetDefaultItem(pEntry->nWID);

        rDefItem.QueryValue(aRet, pEntry->nMemberId);
    }

	return aRet;
}

/*-- 11.12.98 08:12:51---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXParagraph::attach(const uno::Reference< text::XTextRange > & /*xTextRange*/)
throw (lang::IllegalArgumentException, uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());
    // SwXParagraph will only created in order to be inserted by
    // 'insertTextContentBefore' or 'insertTextContentAfter' therefore
    // they cannot be attached
    throw uno::RuntimeException();
}

/*-- 11.12.98 08:12:51---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange > SAL_CALL
SwXParagraph::getAnchor() throw (uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());

    SwTxtNode & rTxtNode(m_pImpl->GetTxtNodeOrThrow());

    SwPosition aPos( rTxtNode );
    SwCursor aCursor( aPos, 0, false );
    // select paragraph
    SwParaSelection aParaSel( aCursor );
    const uno::Reference< text::XTextRange >  xRet =
        new SwXTextRange(aCursor, m_pImpl->m_xParentText);
    return xRet;
}

/*-- 11.12.98 08:12:52---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXParagraph::dispose() throw (uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());

    SwTxtNode *const pTxtNode( m_pImpl->GetTxtNode() );
    if (pTxtNode)
    {
        SwCursor aCursor( SwPosition( *pTxtNode ), 0, false );
        // select paragraph
        {
            SwParaSelection aParaSel( aCursor );
            pTxtNode->GetDoc()->DelFullPara(aCursor);
        }
        m_pImpl->m_ListenerContainer.Disposing();
    }
}

/*-- 11.12.98 08:12:52---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXParagraph::addEventListener(
        const uno::Reference< lang::XEventListener > & xListener)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (!m_pImpl->GetTxtNode())
    {
		throw uno::RuntimeException();
    }
    m_pImpl->m_ListenerContainer.AddListener(xListener);
}
/*-- 11.12.98 08:12:53---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXParagraph::removeEventListener(
        const uno::Reference< lang::XEventListener > & xListener)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (!m_pImpl->GetTxtNode() ||
        !m_pImpl->m_ListenerContainer.RemoveListener(xListener))
    {
		throw uno::RuntimeException();
    }
}

/*-- 11.12.98 08:12:53---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< container::XEnumeration >  SAL_CALL
SwXParagraph::createEnumeration() throw (uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());

    SwTxtNode & rTxtNode(m_pImpl->GetTxtNodeOrThrow());

    SwPosition aPos( rTxtNode );
    SwPaM aPam ( aPos );
    const uno::Reference< container::XEnumeration > xRef =
        new SwXTextPortionEnumeration(aPam, m_pImpl->m_xParentText,
            m_pImpl->m_nSelectionStartPos, m_pImpl->m_nSelectionEndPos);
    return xRef;
}

/*-- 11.12.98 08:12:54---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SAL_CALL SwXParagraph::getElementType() throw (uno::RuntimeException)
{
    return text::XTextRange::static_type();
}
/*-- 11.12.98 08:12:54---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SAL_CALL SwXParagraph::hasElements() throw (uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());
    return (GetTxtNode()) ? sal_True : sal_False;
}

/*-- 11.12.98 08:12:55---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XText > SAL_CALL
SwXParagraph::getText() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    return m_pImpl->m_xParentText;
}

/*-- 11.12.98 08:12:55---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange > SAL_CALL
SwXParagraph::getStart() throw (uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());

    SwTxtNode & rTxtNode(m_pImpl->GetTxtNodeOrThrow());

    SwPosition aPos( rTxtNode );
    SwCursor aCursor( aPos, 0, false );
    SwParaSelection aParaSel( aCursor );
    SwPaM aPam( *aCursor.Start() );
    uno::Reference< text::XText >  xParent = getText();
    const uno::Reference< text::XTextRange > xRet =
        new SwXTextRange(aPam, xParent);
	return xRet;
}
/*-- 11.12.98 08:12:56---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange > SAL_CALL
SwXParagraph::getEnd() throw (uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());

    SwTxtNode & rTxtNode(m_pImpl->GetTxtNodeOrThrow());

    SwPosition aPos( rTxtNode );
    SwCursor aCursor( aPos, 0, false );
    SwParaSelection aParaSel( aCursor );
    SwPaM aPam( *aCursor.End() );
    uno::Reference< text::XText >  xParent = getText();
    const uno::Reference< text::XTextRange > xRet =
        new SwXTextRange(aPam, xParent);
	return xRet;
}

/*-- 11.12.98 08:12:56---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SAL_CALL SwXParagraph::getString() throw (uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());
	OUString aRet;
    SwTxtNode const*const pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        SwPosition aPos( *pTxtNode );
        SwCursor aCursor( aPos, 0, false );
        SwParaSelection aParaSel( aCursor );
        SwUnoCursorHelper::GetTextFromPam(aCursor, aRet);
    }
    else if (m_pImpl->IsDescriptor())
    {
        aRet = m_pImpl->m_sText;
    }
    else
    {
		throw uno::RuntimeException();
    }
	return aRet;
}
/*-- 11.12.98 08:12:57---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXParagraph::setString(const OUString& aString)
throw (uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());

    SwTxtNode const*const pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        SwPosition aPos( *pTxtNode );
        SwCursor aCursor( aPos, 0, false );
        if (!SwUnoCursorHelper::IsStartOfPara(aCursor)) {
            aCursor.MovePara(fnParaCurr, fnParaStart);
        }
        SwUnoCursorHelper::SelectPam(aCursor, true);
        if (pTxtNode->GetTxt().Len()) {
            aCursor.MovePara(fnParaCurr, fnParaEnd);
        }
        SwUnoCursorHelper::SetString(aCursor, aString);
        SwUnoCursorHelper::SelectPam(aCursor, false);
    }
    else if (m_pImpl->IsDescriptor())
    {
        m_pImpl->m_sText = aString;
    }
	else
    {
		throw uno::RuntimeException();
    }
}

/* -----------------23.03.99 12:49-------------------
 *
 * --------------------------------------------------*/
uno::Reference< container::XEnumeration > SAL_CALL
SwXParagraph::createContentEnumeration(const OUString& rServiceName)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (!rServiceName.equalsAscii("com.sun.star.text.TextContent"))
    {
        throw uno::RuntimeException();
    }

    SwTxtNode & rTxtNode(m_pImpl->GetTxtNodeOrThrow());

    SwPosition aPos( rTxtNode );
    SwPaM aPam( aPos );
    uno::Reference< container::XEnumeration > xRet =
        new SwXParaFrameEnumeration(aPam, PARAFRAME_PORTION_PARAGRAPH);
	return xRet;
}
/* -----------------23.03.99 12:49-------------------
 *
 * --------------------------------------------------*/
uno::Sequence< OUString > SAL_CALL
SwXParagraph::getAvailableServiceNames() throw (uno::RuntimeException)
{
	uno::Sequence< OUString > aRet(1);
	OUString* pArray = aRet.getArray();
	pArray[0] = C2U("com.sun.star.text.TextContent");
	return aRet;
}


// MetadatableMixin
::sfx2::Metadatable* SwXParagraph::GetCoreObject()
{
    SwTxtNode *const pTxtNode( m_pImpl->GetTxtNode() );
    return pTxtNode;
}

uno::Reference<frame::XModel> SwXParagraph::GetModel()
{
    SwTxtNode *const pTxtNode( m_pImpl->GetTxtNode() );
    if (pTxtNode)
    {
        SwDocShell const*const pShell( pTxtNode->GetDoc()->GetDocShell() );
        return (pShell) ? pShell->GetModel() : 0;
    }
    return 0;
}

