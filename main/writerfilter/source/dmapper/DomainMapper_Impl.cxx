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


#include <DomainMapper_Impl.hxx>
#include <ConversionHelper.hxx>
#include <DomainMapperTableHandler.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/graphic/XGraphic.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/container/XIndexReplace.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/drawing/XShapes.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/style/LineNumberPosition.hpp>
#include <com/sun/star/style/NumberingType.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/table/BorderLine.hpp>
#include <com/sun/star/text/ChapterFormat.hpp>
#include <com/sun/star/text/FilenameDisplayFormat.hpp>
#include <com/sun/star/text/UserDataPart.hpp>
#include <com/sun/star/text/SetVariableType.hpp>
#include <com/sun/star/text/XFootnote.hpp>
#include <com/sun/star/text/XLineNumberingProperties.hpp>
#include <com/sun/star/text/PageNumberType.hpp>
#include <com/sun/star/text/RelOrientation.hpp>
#include <com/sun/star/text/HoriOrientation.hpp>
#include <com/sun/star/text/VertOrientation.hpp>
#include <com/sun/star/text/ReferenceFieldPart.hpp>
#include <com/sun/star/text/ReferenceFieldSource.hpp>
#include <com/sun/star/text/SizeType.hpp>
#include <com/sun/star/text/TextContentAnchorType.hpp>
#include <com/sun/star/text/WrapTextMode.hpp>
#include <com/sun/star/text/XDependentTextField.hpp>
#include <com/sun/star/text/XParagraphCursor.hpp>
#include <com/sun/star/text/XRedline.hpp>
#include <com/sun/star/text/XTextAppendAndConvert.hpp>
#include <com/sun/star/text/XTextCopy.hpp>
#include <com/sun/star/text/XTextField.hpp>
#include <com/sun/star/text/XTextFieldsSupplier.hpp>
#include <com/sun/star/style/DropCapFormat.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/util/XNumberFormats.hpp>
#include <rtl/ustrbuf.hxx>
#include <rtl/string.h>
#include "FieldTypes.hxx"

#include <tools/string.hxx>
#ifdef DEBUG_DOMAINMAPPER
#include <resourcemodel/QNameToString.hxx>
#include <resourcemodel/util.hxx>
#include <dmapperLoggers.hxx>
#endif
#include <ooxml/OOXMLFastTokens.hxx>

#if DEBUG
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/style/TabStop.hpp>
#endif

#include <map>

using namespace ::com::sun::star;
using namespace ::rtl;
namespace writerfilter {
namespace dmapper{

class Exception
{
    ::std::string m_message;

public:
    Exception(const ::std::string & message = "") : m_message(message) {}
    virtual ~Exception() {}
};

struct FieldConversion
{
    ::rtl::OUString     sWordCommand;
    const sal_Char*     cFieldServiceName;
    const sal_Char*     cFieldMasterServiceName;
    FieldId             eFieldId;
};

typedef ::std::map< ::rtl::OUString, FieldConversion>
            FieldConversionMap_t;

/*-- 18.07.2006 08:56:55---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 FIB::GetData( Id nName )
{
    if( nName >= NS_rtf::LN_WIDENT && nName <= NS_rtf::LN_LCBSTTBFUSSR)
        return aFIBData[nName - NS_rtf::LN_WIDENT];
    OSL_ENSURE( false, "invalid index in FIB");
    return -1;
}
/*-- 18.07.2006 08:56:55---------------------------------------------------

  -----------------------------------------------------------------------*/
void FIB::SetData( Id nName, sal_Int32 nValue )
{
    OSL_ENSURE( nName >= NS_rtf::LN_WIDENT && nName <= NS_rtf::LN_LCBSTTBFUSSR, "invalid index in FIB");
    if( nName >= NS_rtf::LN_WIDENT && nName <= NS_rtf::LN_LCBSTTBFUSSR)
        aFIBData[nName - NS_rtf::LN_WIDENT] = nValue;
}
/*-- 01.09.2006 10:22:03---------------------------------------------------

  -----------------------------------------------------------------------*/
DomainMapper_Impl::DomainMapper_Impl(
    DomainMapper& rDMapper,
    uno::Reference < uno::XComponentContext >  xContext,
    uno::Reference< lang::XComponent >  xModel,
    SourceDocumentType eDocumentType) :
    m_eDocumentType( eDocumentType ),
    m_rDMapper( rDMapper ),
    m_xTextDocument( xModel, uno::UNO_QUERY ),
    m_xTextFactory( xModel, uno::UNO_QUERY ),
    m_xComponentContext( xContext ),
    m_bFieldMode( false ),
    m_bSetUserFieldContent( false ),
    m_bIsFirstSection( true ),
    m_bIsColumnBreakDeferred( false ),
    m_bIsPageBreakDeferred( false ),
    m_bIsInShape( false ),
    m_bShapeContextAdded( false ),
    m_pLastSectionContext( ),
    m_nCurrentTabStopIndex( 0 ),
    m_sCurrentParaStyleId(),
    m_bInStyleSheetImport( false ),
    m_bInAnyTableImport( false ),
    m_bLineNumberingSet( false ),
    m_bIsInFootnoteProperties( true ),
    m_bIsCustomFtnMark( false ),
    m_bIsParaChange( false ),
    m_bParaChanged( false ),
    m_bIsLastParaInSection( false ),
    m_bIsInComments( false )
    , m_xAnnotationField()
    , m_nAnnotationId( -1 )
    , m_aAnnotationPositions()
{
    appendTableManager( );
    GetBodyText();
    uno::Reference< text::XTextAppend > xBodyTextAppend = uno::Reference< text::XTextAppend >( m_xBodyText, uno::UNO_QUERY );
    m_aTextAppendStack.push(xBodyTextAppend);

    //todo: does it make sense to set the body text as static text interface?
    uno::Reference< text::XTextAppendAndConvert > xBodyTextAppendAndConvert( m_xBodyText, uno::UNO_QUERY );
    TableDataHandler_t::Pointer_t pTableHandler
        (new DomainMapperTableHandler(xBodyTextAppendAndConvert, *this));
    getTableManager().setHandler(pTableHandler);
}
/*-- 01.09.2006 10:22:28---------------------------------------------------

  -----------------------------------------------------------------------*/
DomainMapper_Impl::~DomainMapper_Impl()
{
    RemoveLastParagraph();
    popTableManager();
}
/*-------------------------------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< container::XNameContainer >    DomainMapper_Impl::GetPageStyles()
{
    if(!m_xPageStyles.is())
    {
        uno::Reference< style::XStyleFamiliesSupplier > xSupplier( m_xTextDocument, uno::UNO_QUERY );
        xSupplier->getStyleFamilies()->getByName(::rtl::OUString::createFromAscii("PageStyles")) >>= m_xPageStyles;
    }
    return m_xPageStyles;
}
/*-------------------------------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XText > DomainMapper_Impl::GetBodyText()
{
    if(!m_xBodyText.is() && m_xTextDocument.is())
    {
        m_xBodyText = m_xTextDocument->getText();
    }
    return m_xBodyText;
}
/*-- 21.12.2006 12:09:30---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySet > DomainMapper_Impl::GetDocumentSettings()
{
    if( !m_xDocumentSettings.is() )
    {
        m_xDocumentSettings = uno::Reference< beans::XPropertySet >(
            m_xTextFactory->createInstance(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.document.Settings"))), uno::UNO_QUERY );
    }
    return m_xDocumentSettings;
}
/*-- 21.12.2006 12:16:23---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::SetDocumentSettingsProperty( const ::rtl::OUString& rPropName, const uno::Any& rValue )
{
    uno::Reference< beans::XPropertySet > xSettings = GetDocumentSettings();
    if( xSettings.is() )
    {
        try
        {
            xSettings->setPropertyValue( rPropName, rValue );
        }
        catch( const uno::Exception& e)
        {
            (void) e;
        }
    }
}

void DomainMapper_Impl::RemoveLastParagraph( )
{
    uno::Reference< text::XTextAppend >  xTextAppend = m_aTextAppendStack.top().xTextAppend;
    try
    {
        uno::Reference< text::XTextCursor > xCursor = xTextAppend->createTextCursor();
        xCursor->gotoEnd(false);
        xCursor->goLeft( 1, true );
        xCursor->setString(::rtl::OUString());
    }
    catch( const uno::Exception& rEx)
    {
        (void)rEx;
    }
}

void DomainMapper_Impl::SetIsLastParagraphInSection( bool bIsLast )
{
    m_bIsLastParaInSection = bIsLast;
}

/*-------------------------------------------------------------------------

  -----------------------------------------------------------------------*/
void    DomainMapper_Impl::PushProperties(ContextType eId)
{
    SectionPropertyMap* pSectionContext = 0;
    PropertyMapPtr pInsert;

    switch (eId)
    {
    case CONTEXT_SECTION:
        pSectionContext = new SectionPropertyMap( m_bIsFirstSection );
        pInsert.reset(pSectionContext);
        break;
    case CONTEXT_PARAGRAPH:
        pInsert.reset(new ParagraphPropertyMap);
        break;
    default:
        pInsert.reset(new PropertyMap);
        break;
    }

    if(eId == CONTEXT_SECTION)
    {
        if( m_bIsFirstSection )
            m_bIsFirstSection = false;
        // beginning with the second section group a section has to be inserted
        // into the document
        SectionPropertyMap* pSectionContext_ = dynamic_cast< SectionPropertyMap* >( pInsert.get() );
         uno::Reference< text::XTextAppend >  xTextAppend = m_aTextAppendStack.top().xTextAppend;
         if(xTextAppend.is())
             pSectionContext_->SetStart( xTextAppend->getEnd() );
    }
    m_aPropertyStacks[eId].push( pInsert );
    m_aContextStack.push(eId);

    m_pTopContext = m_aPropertyStacks[eId].top();
}
/*-- 13.06.2007 16:18:18---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PushStyleProperties( PropertyMapPtr pStyleProperties )
{
    m_aPropertyStacks[CONTEXT_STYLESHEET].push( pStyleProperties );
    m_aContextStack.push(CONTEXT_STYLESHEET);

    m_pTopContext = m_aPropertyStacks[CONTEXT_STYLESHEET].top();
}
/*-- 28.01.2008 14:47:46---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PushListProperties(PropertyMapPtr pListProperties)
{
    m_aPropertyStacks[CONTEXT_LIST].push( pListProperties );
    m_aContextStack.push(CONTEXT_LIST);
    m_pTopContext = m_aPropertyStacks[CONTEXT_LIST].top();
}
/*-------------------------------------------------------------------------

  -----------------------------------------------------------------------*/
void    DomainMapper_Impl::PopProperties(ContextType eId)
{
    OSL_ENSURE(!m_aPropertyStacks[eId].empty(), "section stack already empty");

    if ( eId == CONTEXT_SECTION )
    {
        m_pLastSectionContext = m_aPropertyStacks[eId].top( );
    }

    if (!m_aPropertyStacks[eId].empty()) {
        m_aPropertyStacks[eId].pop();
        m_aContextStack.pop();
    }
    if(!m_aContextStack.empty() && !m_aPropertyStacks[m_aContextStack.top()].empty())

            m_pTopContext = m_aPropertyStacks[m_aContextStack.top()].top();
    else
    {
        // OSL_ENSURE(eId == CONTEXT_SECTION, "this should happen at a section context end");
        m_pTopContext.reset();
    }
}
/*-------------------------------------------------------------------------

  -----------------------------------------------------------------------*/
PropertyMapPtr DomainMapper_Impl::GetTopContextOfType(ContextType eId)
{
    PropertyMapPtr pRet;
    OSL_ENSURE( !m_aPropertyStacks[eId].empty(),
            "no context of this type available");
    if(!m_aPropertyStacks[eId].empty())
        pRet = m_aPropertyStacks[eId].top();
    return pRet;
}

/*-- 24.05.2007 15:54:51---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextAppend >  DomainMapper_Impl::GetTopTextAppend()
{
    OSL_ENSURE(!m_aTextAppendStack.empty(), "text append stack is empty" );
    return m_aTextAppendStack.top().xTextAppend;
}

/*-- 17.07.2006 08:47:04---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::InitTabStopFromStyle( const uno::Sequence< style::TabStop >& rInitTabStops )
{
    OSL_ENSURE(!m_aCurrentTabStops.size(), "tab stops already initialized");
    for( sal_Int32 nTab = 0; nTab < rInitTabStops.getLength(); ++nTab)
    {
        m_aCurrentTabStops.push_back( DeletableTabStop(rInitTabStops[nTab]) );
    }
}

/*-- 29.06.2006 13:35:33---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::ModifyCurrentTabStop( Id nId, sal_Int32 nValue)
{
    OSL_ENSURE(nId == NS_rtf::LN_dxaAdd || m_nCurrentTabStopIndex < m_aCurrentTabStops.size(),
        "tab stop creation error");

    if( nId != NS_rtf::LN_dxaAdd && m_nCurrentTabStopIndex >= m_aCurrentTabStops.size())
        return;
    static const style::TabAlign aTabAlignFromWord[] =
    {
        style::TabAlign_LEFT,
        style::TabAlign_CENTER,
        style::TabAlign_RIGHT,
        style::TabAlign_DECIMAL,
        style::TabAlign_LEFT
    };
    static const sal_Unicode aTabFillCharWord[] =
    {
        ' ',
        '.',
        '-',
        '_',
        '_',
        0xb7
    };

    switch(nId)
    {
        case NS_rtf::LN_dxaAdd: //set tab
            m_aCurrentTabStops.push_back(
                    DeletableTabStop(style::TabStop(ConversionHelper::convertTwipToMM100(nValue), style::TabAlign_LEFT, ' ', ' ')));
        break;
        case NS_rtf::LN_dxaDel: //deleted tab
        {
            //mark the tab stop at the given position as deleted
            ::std::vector<DeletableTabStop>::iterator aIt = m_aCurrentTabStops.begin();
            ::std::vector<DeletableTabStop>::iterator aEndIt = m_aCurrentTabStops.end();
            sal_Int32 nConverted = ConversionHelper::convertTwipToMM100(nValue);
            for( ; aIt != aEndIt; ++aIt)
            {
                if( aIt->Position == nConverted )
                {
                    aIt->bDeleted = true;
                    break;
                }
            }
        }
        break;
        case NS_rtf::LN_TLC: //tab leading characters - for decimal tabs
            // 0 - no leader, 1- dotted, 2 - hyphenated, 3 - single line, 4 - heavy line, 5 - middle dot
            if( nValue >= 0 &&  nValue < sal::static_int_cast<sal_Int32>(sizeof(aTabFillCharWord) / sizeof (sal_Unicode)))
                m_aCurrentTabStops[m_nCurrentTabStopIndex].FillChar = aTabFillCharWord[nValue];
        break;
        case NS_rtf::LN_JC: //tab justification
            //0 - left, 1 - centered, 2 - right, 3 - decimal 4 - bar
            if( nValue >= 0 && nValue < sal::static_int_cast<sal_Int32>(sizeof(aTabAlignFromWord) / sizeof (style::TabAlign)))
                m_aCurrentTabStops[m_nCurrentTabStopIndex].Alignment = aTabAlignFromWord[nValue];
        break;
    }
}

void DomainMapper_Impl::IncorporateTabStop( const DeletableTabStop &  rTabStop )
{
    ::std::vector<DeletableTabStop>::iterator aIt = m_aCurrentTabStops.begin();
    ::std::vector<DeletableTabStop>::iterator aEndIt = m_aCurrentTabStops.end();
    sal_Int32 nConverted = rTabStop.Position;
    bool bFound = false;
    for( ; aIt != aEndIt; ++aIt)
    {
        if( aIt->Position == nConverted )
        {
            bFound = true;
            if( rTabStop.bDeleted )
                m_aCurrentTabStops.erase( aIt );
            else
                *aIt = rTabStop;
            break;
        }
    }
    if( !bFound )
        m_aCurrentTabStops.push_back( rTabStop );
}
/*-- 29.06.2006 13:35:33---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< style::TabStop > DomainMapper_Impl::GetCurrentTabStopAndClear()
{
    uno::Sequence< style::TabStop > aRet( sal_Int32( m_aCurrentTabStops.size() ) );
    style::TabStop* pArray = aRet.getArray();
    ::std::vector<DeletableTabStop>::const_iterator aIt = m_aCurrentTabStops.begin();
    ::std::vector<DeletableTabStop>::const_iterator aEndIt = m_aCurrentTabStops.end();
    sal_Int32 nDeleted = 0;
    for(sal_Int32 nIndex = 0; aIt != aEndIt; ++aIt)
    {
        if(!aIt->bDeleted)
            pArray[nIndex++] = *aIt;
        else
            ++nDeleted;
    }
    m_aCurrentTabStops.clear();
    m_nCurrentTabStopIndex = 0;
    if(nDeleted)
    {
        aRet.realloc( aRet.getLength() - nDeleted);
    }
    return aRet;
}

/*-- 17.07.2006 09:08:26---------------------------------------------------
    returns a the value from the current paragraph style - if available
    TODO: What about parent styles?
  -----------------------------------------------------------------------*/
uno::Any DomainMapper_Impl::GetPropertyFromStyleSheet(PropertyIds eId)
{
    StyleSheetEntryPtr pEntry;
    if( m_bInStyleSheetImport )
        pEntry = GetStyleSheetTable()->FindParentStyleSheet(::rtl::OUString());
    else
        pEntry =
                GetStyleSheetTable()->FindStyleSheetByISTD(GetCurrentParaStyleId());
    while(pEntry.get( ) )
    {
        //is there a tab stop set?
        if(pEntry->pProperties)
        {
            PropertyMap::const_iterator aPropertyIter =
                    pEntry->pProperties->find(PropertyDefinition(eId, false ));
            if( aPropertyIter != pEntry->pProperties->end())
            {
                return aPropertyIter->second;
            }
        }
        //search until the property is set or no parent is available
        pEntry = GetStyleSheetTable()->FindParentStyleSheet(pEntry->sBaseStyleIdentifier);
    }
    return uno::Any();
}
/*-------------------------------------------------------------------------

  -----------------------------------------------------------------------*/
ListsManager::Pointer DomainMapper_Impl::GetListTable()
{
    if(!m_pListTable)
        m_pListTable.reset(
            new ListsManager( m_rDMapper, m_xTextFactory ));
    return m_pListTable;
}


void DomainMapper_Impl::deferBreak( BreakType deferredBreakType)
{
    switch (deferredBreakType)
    {
    case COLUMN_BREAK:
            m_bIsColumnBreakDeferred = true;
        break;
    case PAGE_BREAK:
            m_bIsPageBreakDeferred = true;
        break;
    default:
        return;
    }
}

bool DomainMapper_Impl::isBreakDeferred( BreakType deferredBreakType )
{
    switch (deferredBreakType)
    {
    case COLUMN_BREAK:
    	return m_bIsColumnBreakDeferred;
    case PAGE_BREAK:
        return m_bIsPageBreakDeferred;
    default:
        return false;
    }
}

void DomainMapper_Impl::clearDeferredBreaks()
{
    m_bIsColumnBreakDeferred = false;
    m_bIsPageBreakDeferred = false;
}
/*-------------------------------------------------------------------------

  -----------------------------------------------------------------------*/
void lcl_MoveBorderPropertiesToFrame(PropertySequence & aPropSequence,
    uno::Reference<text::XTextRange> xStartTextRange,
    uno::Reference<text::XTextRange> xEndTextRange )
{
    try
    {
        uno::Reference<text::XTextCursor> xRangeCursor = xStartTextRange->getText()->createTextCursorByRange( xStartTextRange );
        xRangeCursor->gotoRange( xEndTextRange, true );

        uno::Reference<beans::XPropertySet> xTextRangeProperties(xRangeCursor, uno::UNO_QUERY);
        if(!xTextRangeProperties.is())
            return ;

        PropertyIds aBorderProperties[] =
        {
            PROP_LEFT_BORDER,
            PROP_RIGHT_BORDER,
            PROP_TOP_BORDER,
            PROP_BOTTOM_BORDER,
            PROP_LEFT_BORDER_DISTANCE,
            PROP_RIGHT_BORDER_DISTANCE,
            PROP_TOP_BORDER_DISTANCE,
            PROP_BOTTOM_BORDER_DISTANCE
        };

        sal_uInt32 nBorderPropertyCount = sizeof( aBorderProperties ) / sizeof(PropertyIds);

        PropertyNameSupplier& rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier();
        for( sal_uInt32 nProperty = 0; nProperty < nBorderPropertyCount; ++nProperty)
        {
            ::rtl::OUString sPropertyName = rPropNameSupplier.GetName(aBorderProperties[nProperty]);
            aPropSequence.set(aBorderProperties[nProperty], xTextRangeProperties->getPropertyValue(sPropertyName));
            if( nProperty < 4 )
                xTextRangeProperties->setPropertyValue( sPropertyName, uno::makeAny(table::BorderLine()));
        }
    }
    catch( const uno::Exception& rEx )
   {
        (void)rEx;
   }
}

void lcl_CorrectIndents(PropertySequence & aPropSeq)
{
    try
    {
        uno::Any aAny;

        sal_Int32 nLeftMargin = 0;
	
#ifdef DEBUG        
        ::std::string aStr(aPropSeq.toString());
#endif
	
        aAny = aPropSeq.get(PROP_PARA_LEFT_MARGIN);
        if (aAny.hasValue())
            aAny >>= nLeftMargin;

        aAny = aPropSeq.get(PROP_LEFT_BORDER_DISTANCE);

        if (aAny.hasValue())
        {
            sal_Int32 nLeftBorderDistance = 0;
            aAny >>= nLeftBorderDistance;
            nLeftMargin -= nLeftBorderDistance;
            aPropSeq.set(PROP_PARA_LEFT_MARGIN, nLeftMargin);
        }

        aAny = aPropSeq.get(PROP_LEFT_BORDER);
        
        if (aAny.hasValue())
        {
            table::BorderLine aBorderLine;
            aAny >>= aBorderLine;
            nLeftMargin -= aBorderLine.OuterLineWidth;
            aPropSeq.set(PROP_PARA_LEFT_MARGIN, nLeftMargin);
        }

        sal_Int32 nRightMargin = 0;
        aAny = aPropSeq.get(PROP_PARA_RIGHT_MARGIN);
        if (aAny.hasValue())
            aAny >>= nRightMargin;

        aAny = aPropSeq.get(PROP_RIGHT_BORDER_DISTANCE);
        
        if (aAny.hasValue())
        {
            sal_Int32 nRightBorderDistance = 0;
            aAny >>= nRightBorderDistance;
            nRightMargin -= nRightBorderDistance;
            aPropSeq.set(PROP_PARA_RIGHT_MARGIN, nRightMargin);
        }

        aAny = aPropSeq.get(PROP_RIGHT_BORDER);
        
        if (aAny.hasValue())
        {
            table::BorderLine aBorderLine;
            aAny >>= aBorderLine;
            nRightMargin -= aBorderLine.OuterLineWidth;
            aPropSeq.set(PROP_PARA_RIGHT_MARGIN, nRightMargin);
        }
    }
    catch (const uno::Exception& rEx)
    {
        (void) rEx;
    }
    catch (const dmapper::Exception & rEx)
    {
        (void) rEx;
    }

}

/*-- 04.01.2008 10:59:19---------------------------------------------------

  -----------------------------------------------------------------------*/
void lcl_AddRangeAndStyle(
    ParagraphPropertiesPtr& pToBeSavedProperties,
    uno::Reference< text::XTextAppend > xTextAppend,
    PropertyMapPtr pPropertyMap)
{
    uno::Reference<text::XParagraphCursor> xParaCursor(
        xTextAppend->createTextCursorByRange( xTextAppend->getEnd()), uno::UNO_QUERY_THROW );
    pToBeSavedProperties->SetEndingRange(xParaCursor->getStart());
    xParaCursor->gotoStartOfParagraph( false );

    pToBeSavedProperties->SetStartingRange(xParaCursor->getStart());
    if(pPropertyMap)
    {
        PropertyMap::iterator aParaStyleIter = pPropertyMap->find(PropertyDefinition( PROP_PARA_STYLE_NAME, false ) );
        if( aParaStyleIter != pPropertyMap->end())
        {
            ::rtl::OUString sName;
            aParaStyleIter->second >>= sName;
            pToBeSavedProperties->SetParaStyleName(sName);
        }
    }
}
/*-------------------------------------------------------------------------

  -----------------------------------------------------------------------*/
//define some default frame width - 0cm ATM: this allow the frame to be wrapped around the text
#define DEFAULT_FRAME_MIN_WIDTH 0

void DomainMapper_Impl::finishParagraph( PropertyMapPtr pPropertyMap )
{
#ifdef DEBUG_DOMAINMAPPER
    dmapper_logger->startElement("finishParagraph");
#endif
    
    ParagraphPropertyMap* pParaContext = dynamic_cast< ParagraphPropertyMap* >( pPropertyMap.get() );
    TextAppendContext& rAppendContext = m_aTextAppendStack.top();
    uno::Reference< text::XTextAppend >  xTextAppend = rAppendContext.xTextAppend;
    PropertyNameSupplier& rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier();
    
#ifdef DEBUG_DOMAINMAPPER
    dmapper_logger->attribute("isTextAppend", xTextAppend.is());
#endif 
    
    if(xTextAppend.is() && ! getTableManager( ).isIgnore() && pParaContext != NULL)
    {
        try
        {
            /*the following combinations of previous and current frame settings can occur:
                (1) - no old frame and no current frame     -> no special action
                (2) - no old frame and current DropCap      -> save DropCap for later use, don't call finishParagraph
                                                            remove character properties of the DropCap?
                (3) - no old frame and current Frame        -> save Frame for later use
                (4) - old DropCap and no current frame      -> add DropCap to the properties of the finished paragraph, delete previous setting
                (5) - old DropCap and current frame         -> add DropCap to the properties of the finished paragraph, save current frame settings
                (6) - old Frame and new DropCap             -> add old Frame, save DropCap for later use
                (7) - old Frame and new same Frame          -> continue
                (8) - old Frame and new different Frame     -> add old Frame, save new Frame for later use
                (9) - old Frame and no current frame        -> add old Frame, delete previous settings

              old _and_ new DropCap must not occur
             */

            bool bIsDropCap = 
                pParaContext->IsFrameMode() && 
                sal::static_int_cast<Id>(pParaContext->GetDropCap()) != NS_ooxml::LN_Value_wordprocessingml_ST_DropCap_none;

            style::DropCapFormat aDrop;
            ParagraphPropertiesPtr pToBeSavedProperties;
            bool bKeepLastParagraphProperties = false;
            if( bIsDropCap )
            {
                uno::Reference<text::XParagraphCursor> xParaCursor(
                    xTextAppend->createTextCursorByRange(xTextAppend->getEnd()), uno::UNO_QUERY_THROW);
                //select paragraph
                xParaCursor->gotoStartOfParagraph( true );
                uno::Reference< beans::XPropertyState > xParaProperties( xParaCursor, uno::UNO_QUERY_THROW );
                xParaProperties->setPropertyToDefault(rPropNameSupplier.GetName(PROP_CHAR_ESCAPEMENT));
                xParaProperties->setPropertyToDefault(rPropNameSupplier.GetName(PROP_CHAR_HEIGHT));
                //handles (2) and part of (6)
                pToBeSavedProperties.reset( new ParagraphProperties(*pParaContext) );
                sal_Int32 nCount = xParaCursor->getString().getLength();
                pToBeSavedProperties->SetDropCapLength(nCount > 0 && nCount < 255 ? (sal_Int8)nCount : 1);
            }
            if( rAppendContext.pLastParagraphProperties.get() )
            {
                if( sal::static_int_cast<Id>(rAppendContext.pLastParagraphProperties->GetDropCap()) != NS_ooxml::LN_Value_wordprocessingml_ST_DropCap_none)
                {
                    //handles (4) and part of (5)
                    //create a DropCap property, add it to the property sequence of finishParagraph
                    sal_Int32 nLines = rAppendContext.pLastParagraphProperties->GetLines();
                    aDrop.Lines = nLines > 0 && nLines < 254 ? (sal_Int8)++nLines : 2;
                    aDrop.Count = rAppendContext.pLastParagraphProperties->GetDropCapLength();
                    aDrop.Distance  = 0; //TODO: find distance value
                    //completes (5)
                    if( pParaContext->IsFrameMode() )
                        pToBeSavedProperties.reset( new ParagraphProperties(*pParaContext) );
                }
                else if(*rAppendContext.pLastParagraphProperties == *pParaContext )
                {
                    //handles (7)
                    rAppendContext.pLastParagraphProperties->SetEndingRange(xTextAppend->getEnd());
                    bKeepLastParagraphProperties = true;
                }
                else
                {
                    //handles (8)(9) and completes (6)
                    try
                    {
                        //
                        StyleSheetEntryPtr pParaStyle =
                            m_pStyleSheetTable->FindStyleSheetByConvertedStyleName(rAppendContext.pLastParagraphProperties->GetParaStyleName());
                        
                        PropertySequence aPropSequence;
                        if ( pParaStyle.get( ) )
                        {
                            const ParagraphProperties* pStyleProperties = dynamic_cast<const ParagraphProperties*>( pParaStyle->pProperties.get() );
                            sal_Int32 nWidth =
                                rAppendContext.pLastParagraphProperties->Getw() > 0 ?
                                rAppendContext.pLastParagraphProperties->Getw() :
                                pStyleProperties->Getw();
                            bool bAutoWidth = nWidth < 1;
                            if( bAutoWidth )
                                nWidth = DEFAULT_FRAME_MIN_WIDTH;
                            aPropSequence.set(PROP_WIDTH, nWidth);
                            
                            if (rAppendContext.pLastParagraphProperties->Geth())
                            {
                                aPropSequence.set(PROP_HEIGHT, rAppendContext.pLastParagraphProperties->Geth());
                            }
                            else
                            {
                                aPropSequence.set(PROP_HEIGHT, pStyleProperties->Geth());
                            }
                            
                            if (rAppendContext.pLastParagraphProperties->GethRule() >= 0)
                            {
                                aPropSequence.set(PROP_SIZE_TYPE, rAppendContext.pLastParagraphProperties->GethRule());
                            }
                            else if (pStyleProperties->GethRule() >=0)
                            {
                                aPropSequence.set(PROP_SIZE_TYPE, pStyleProperties->GethRule());
                            }
                            else
                            {
                                aPropSequence.set(PROP_SIZE_TYPE, text::SizeType::VARIABLE);
                            }
                            
                            if (bAutoWidth)
                            {
                                aPropSequence.set(PROP_WIDTH_TYPE,  text::SizeType::MIN);
                            }                                     
                            else
                            {
                                aPropSequence.set(PROP_WIDTH_TYPE,  text::SizeType::FIX);
                            }
                            
                            sal_Int16 nHoriOrient = text::HoriOrientation::NONE;
                            if ( rAppendContext.pLastParagraphProperties->GetxAlign() >= 0)
                            {
                                nHoriOrient = static_cast<sal_Int16>(rAppendContext.pLastParagraphProperties->GetxAlign());
                            }
                            else if (pStyleProperties->GetxAlign() >= 0)
                            {
                                nHoriOrient = static_cast<sal_Int16>(pStyleProperties->GetxAlign());
                            }

                            aPropSequence.set(PROP_HORI_ORIENT, nHoriOrient);

                            if (rAppendContext.pLastParagraphProperties->IsxValid())
                            {
                                aPropSequence.set(PROP_HORI_ORIENT_POSITION, 
                                                  rAppendContext.pLastParagraphProperties->Getx());
                            }
                            else
                            {
                                aPropSequence.set(PROP_HORI_ORIENT_POSITION, 
                                                  pStyleProperties->Getx());
                            }
                            
                            if (rAppendContext.pLastParagraphProperties->GethAnchor() >= 0)
                            {
                                aPropSequence.set(PROP_HORI_ORIENT_RELATION, 
                                                  rAppendContext.pLastParagraphProperties->GethAnchor());
                            }
                            else
                            {
                                aPropSequence.set(PROP_HORI_ORIENT_RELATION, pStyleProperties->GethAnchor());
                            }
                            
                            sal_Int16 nVertOrient =  text::VertOrientation::NONE;
                            if ( rAppendContext.pLastParagraphProperties->GetyAlign() >= 0)
                            {
                                nVertOrient = static_cast<sal_Int16>(rAppendContext.pLastParagraphProperties->GetyAlign());
                            }
                            else if ( pStyleProperties->GetyAlign() >= 0)
                            {
                                nVertOrient =  static_cast<sal_Int16>(pStyleProperties->GetyAlign());
                            }

                            aPropSequence.set(PROP_VERT_ORIENT, nVertOrient);
                            
                            if ( rAppendContext.pLastParagraphProperties->IsyValid())
                            {
                                aPropSequence.set(PROP_VERT_ORIENT_POSITION, rAppendContext.pLastParagraphProperties->Gety());                                   }
                            else
                            {
                                aPropSequence.set(PROP_VERT_ORIENT_POSITION,  pStyleProperties->Gety());
                            }
                            
                            if (rAppendContext.pLastParagraphProperties->GetvAnchor() >= 0)
                            {
                                aPropSequence.set(PROP_VERT_ORIENT_RELATION, 
                                                  rAppendContext.pLastParagraphProperties->GetvAnchor());
                            }
                            else
                            {
                                aPropSequence.set(PROP_VERT_ORIENT_RELATION, 
                                                  pStyleProperties->GetvAnchor());
                            }
                            
                            if (rAppendContext.pLastParagraphProperties->GetWrap() >= 0)
                            {
                                aPropSequence.set(PROP_SURROUND, 
                                                  rAppendContext.pLastParagraphProperties->GetWrap());
                            }
                            else
                            {
                                aPropSequence.set(PROP_SURROUND, 
                                                  pStyleProperties->GetWrap());
                            }
                            
                            sal_Int32 nTopDist;

                            if (nVertOrient == text::VertOrientation::TOP)
                            {
                                nTopDist = 0;
                            }
                            else
                            {
                                if (rAppendContext.pLastParagraphProperties->GethSpace() >= 0)
                                {
                                    nTopDist = rAppendContext.pLastParagraphProperties->GethSpace();
                                }
                                else
                                {
                                    nTopDist = pStyleProperties->GethSpace();
                                }
                            }

                            aPropSequence.set(PROP_TOP_MARGIN, nTopDist);
                            
                            sal_Int32 nBottomDist = nTopDist;
                            if (nVertOrient == text::VertOrientation::BOTTOM)
                            {
                                nBottomDist = 0;
                            }
                            
                            aPropSequence.set(PROP_BOTTOM_MARGIN, nBottomDist);

                            sal_Int32 nLeftDist = 0;
                            if (nHoriOrient == text::HoriOrientation::LEFT)
                            {
                                nLeftDist = 0;
                            }
                            else if (rAppendContext.pLastParagraphProperties->GetvSpace() >= 0)
                            {
                                nLeftDist = rAppendContext.pLastParagraphProperties->GetvSpace();
                            }
                            else if (pStyleProperties->GetvSpace() >= 0)
                            {
                                nLeftDist = pStyleProperties->GetvSpace();
                            }
                            else
                            {
                                nLeftDist = 0;
                            }

                            aPropSequence.set(PROP_LEFT_MARGIN, nLeftDist);
                            
                            sal_Int32 nRightDist = nLeftDist;
                                                        
                            if ( nHoriOrient == text::HoriOrientation::RIGHT)
                            {
                                nRightDist = 0;
                            }

                            aPropSequence.set(PROP_RIGHT_MARGIN, nRightDist);
                            
                            lcl_MoveBorderPropertiesToFrame(aPropSequence,
                                                            rAppendContext.pLastParagraphProperties->GetStartingRange(),
                                                            rAppendContext.pLastParagraphProperties->GetEndingRange());
                            lcl_CorrectIndents(aPropSequence);                            
                        }
                        //frame conversion has to be executed after table conversion
                        RegisterFrameConversion(rAppendContext.pLastParagraphProperties->GetStartingRange(),
                                                rAppendContext.pLastParagraphProperties->GetEndingRange(),
                                                aPropSequence.getSequence() );
                        // next frame follows directly
                        if( pParaContext->IsFrameMode() )
                        {
                            pToBeSavedProperties.reset( new ParagraphProperties(*pParaContext) );
                            lcl_AddRangeAndStyle(pToBeSavedProperties, xTextAppend, pPropertyMap);
                        }
                    }
                    catch( const uno::Exception& rEx )
                    {
                        (void)rEx;
                    }
                }

            }
            else //
            {
                // (1) doesn't need handling
                //
                if( !bIsDropCap && pParaContext->IsFrameMode() )
                {
                    pToBeSavedProperties.reset( new ParagraphProperties(*pParaContext) );
                    lcl_AddRangeAndStyle(pToBeSavedProperties, xTextAppend, pPropertyMap);
                }
            }
            
            PropertySequence::Pointer_t pPropSeq(new PropertySequence());

            if( pPropertyMap.get() )
            {
                pPropSeq.reset(new PropertySequence(pPropertyMap->GetPropertyValues()));
            }

            if( !bIsDropCap )
            {
                if( aDrop.Lines > 1 )
                {
                    uno::Any aAny(aDrop);

                    pPropSeq->set(PROP_DROP_CAP_FORMAT, aAny);
                }

                lcl_CorrectIndents(*pPropSeq);

                uno::Reference< text::XTextRange > xTextRange =
                    xTextAppend->finishParagraph( pPropSeq->getSequence() );
                getTableManager( ).handle(xTextRange);
            
                // Set the anchor of the objects to the created paragraph
                while ( m_aAnchoredStack.size( ) > 0 && !m_bIsInShape )
                {
                    uno::Reference< text::XTextContent > xObj = m_aAnchoredStack.top( );
                    try 
                    {
#if DEBUG
                        rtl::OUString sText( xTextRange->getString( ) );
#endif
                        xObj->attach( xTextRange );
                    } 
                    catch ( uno::RuntimeException& e)
                    {
                        // this is normal: the shape is already attached
                        (void) e;
                    }   
                    m_aAnchoredStack.pop( );
                }
    
                // Get the end of paragraph character inserted
                uno::Reference< text::XTextCursor > xCur = xTextRange->getText( )->createTextCursor( );
                xCur->gotoEnd( false );
                xCur->goLeft( 1 , true );
                uno::Reference< text::XTextRange > xParaEnd( xCur, uno::UNO_QUERY );
                CheckParaRedline( xParaEnd );

                // Remove the last empty section paragraph if needed
                if ( m_bIsLastParaInSection && !m_bParaChanged )
                {
                    RemoveLastParagraph( );
                    m_bIsLastParaInSection = false;
                }

                m_bParaChanged = false;
            }
            if( !bKeepLastParagraphProperties )
                rAppendContext.pLastParagraphProperties = pToBeSavedProperties;
        }
        catch(const lang::IllegalArgumentException& rIllegal)
        {
            (void)rIllegal;
            OSL_ENSURE( false, "IllegalArgumentException in DomainMapper_Impl::finishParagraph" );
        }
        catch(const uno::Exception& rEx)
        {
            (void)rEx;
            //OSL_ENSURE( false, "ArgumentException in DomainMapper_Impl::finishParagraph" );
        }
    }

#ifdef DEBUG_DOMAINMAPPER
    dmapper_logger->endElement("finishParagraph");
#endif
}
/*-------------------------------------------------------------------------

  -----------------------------------------------------------------------*/
util::DateTime lcl_DateStringToDateTime( const ::rtl::OUString& rDateTime )
{
    util::DateTime aDateTime;
    //xsd::DateTime in the format [-]CCYY-MM-DDThh:mm:ss[Z|(+|-)hh:mm] example: 2008-01-21T10:42:00Z
    //OUString getToken( sal_Int32 token, sal_Unicode cTok, sal_Int32& index ) const SAL_THROW(())
    sal_Int32 nIndex = 0;
    ::rtl::OUString sDate = rDateTime.getToken( 0, 'T', nIndex );
    ::rtl::OUString sTime = rDateTime.getToken( 0, 'Z', nIndex );
    nIndex = 0;
    aDateTime.Year = sal_uInt16( sDate.getToken( 0, '-', nIndex ).toInt32() );
    aDateTime.Month = sal_uInt16( sDate.getToken( 0, '-', nIndex ).toInt32() );
    aDateTime.Day = sal_uInt16( sDate.copy( nIndex ).toInt32() );

    nIndex = 0;
    aDateTime.Hours = sal_uInt16( sTime.getToken( 0, ':', nIndex ).toInt32() );
    aDateTime.Minutes = sal_uInt16( sTime.getToken( 0, ':', nIndex ).toInt32() );
    aDateTime.Seconds = sal_uInt16( sTime.copy( nIndex ).toInt32() );

    return aDateTime;
}
void DomainMapper_Impl::appendTextPortion( const ::rtl::OUString& rString, PropertyMapPtr pPropertyMap )
{
    uno::Reference< text::XTextAppend >  xTextAppend = m_aTextAppendStack.top().xTextAppend;
    if(xTextAppend.is() && ! getTableManager( ).isIgnore())
    {
        try
        {
            uno::Reference< text::XTextRange > xTextRange =
                xTextAppend->appendTextPortion
                (rString, pPropertyMap->GetPropertyValues());
            CheckRedline( xTextRange );
            m_bParaChanged = true;

            //getTableManager( ).handle(xTextRange);
        }
        catch(const lang::IllegalArgumentException& rEx)
        {
            (void)rEx;
            OSL_ENSURE( false, "IllegalArgumentException in DomainMapper_Impl::appendTextPortion" );
        }
        catch(const uno::Exception& rEx)
        {
            (void)rEx;
            OSL_ENSURE( false, "Exception in DomainMapper_Impl::appendTextPortion" );
        }
    }
}
/*-- 02.11.2006 12:08:33---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::appendTextContent(
    const uno::Reference< text::XTextContent > xContent,
    const uno::Sequence< beans::PropertyValue > xPropertyValues
    )
{
    uno::Reference< text::XTextAppendAndConvert >  xTextAppendAndConvert( m_aTextAppendStack.top().xTextAppend, uno::UNO_QUERY );
    OSL_ENSURE( xTextAppendAndConvert.is(), "trying to append a text content without XTextAppendAndConvert" );
    if(xTextAppendAndConvert.is() && ! getTableManager( ).isIgnore())
    {
        try
        {
            xTextAppendAndConvert->appendTextContent( xContent, xPropertyValues );
        }
        catch(const lang::IllegalArgumentException& e)
        {
            (void) e;
        }
        catch(const uno::Exception& e)
        {
            (void) e;
        }
    }
}

/*-- 24.04.2008 08:38:07---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::appendOLE( const ::rtl::OUString& rStreamName, OLEHandlerPtr pOLEHandler )
{
    static const rtl::OUString sEmbeddedService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextEmbeddedObject"));
    try
    {
        uno::Reference< text::XTextContent > xOLE( m_xTextFactory->createInstance(sEmbeddedService), uno::UNO_QUERY_THROW );
        uno::Reference< beans::XPropertySet > xOLEProperties(xOLE, uno::UNO_QUERY_THROW);

        xOLEProperties->setPropertyValue(PropertyNameSupplier::GetPropertyNameSupplier().GetName( PROP_STREAM_NAME ),
                        uno::makeAny( rStreamName ));
        awt::Size aSize = pOLEHandler->getSize();
        if( !aSize.Width )
            aSize.Width = 1000;
        if( !aSize.Height )
            aSize.Height = 1000;
        xOLEProperties->setPropertyValue(PropertyNameSupplier::GetPropertyNameSupplier().GetName( PROP_WIDTH ),
                        uno::makeAny(aSize.Width));
        xOLEProperties->setPropertyValue(PropertyNameSupplier::GetPropertyNameSupplier().GetName( PROP_HEIGHT ),
                        uno::makeAny(aSize.Height));

        uno::Reference< graphic::XGraphic > xGraphic = pOLEHandler->getReplacement();
        xOLEProperties->setPropertyValue(PropertyNameSupplier::GetPropertyNameSupplier().GetName( PROP_GRAPHIC ),
                        uno::makeAny(xGraphic));

        //
        appendTextContent( xOLE, uno::Sequence< beans::PropertyValue >() );

    }
    catch( const uno::Exception& rEx )
    {
        (void)rEx;
        OSL_ENSURE( false, "Exception in creation of OLE object" );
    }

}
/*-- 14.12.2006 12:26:00---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySet > DomainMapper_Impl::appendTextSectionAfter(
                                    uno::Reference< text::XTextRange >& xBefore )
{
    uno::Reference< beans::XPropertySet > xRet;
    uno::Reference< text::XTextAppend >  xTextAppend = m_aTextAppendStack.top().xTextAppend;
    if(xTextAppend.is())
    {
        try
        {
            uno::Reference< text::XParagraphCursor > xCursor(
                xTextAppend->createTextCursorByRange( xBefore ), uno::UNO_QUERY_THROW);
            //the cursor has been moved to the end of the paragraph because of the appendTextPortion() calls
            xCursor->gotoStartOfParagraph( false );
            xCursor->gotoEnd( true );
            //the paragraph after this new section is already inserted
            xCursor->goLeft(1, true);
            static const rtl::OUString sSectionService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextSection"));
            uno::Reference< text::XTextContent > xSection( m_xTextFactory->createInstance(sSectionService), uno::UNO_QUERY_THROW );
            xSection->attach( uno::Reference< text::XTextRange >( xCursor, uno::UNO_QUERY_THROW) );
            xRet = uno::Reference< beans::XPropertySet > (xSection, uno::UNO_QUERY );
        }
        catch(const uno::Exception& e)
        {
            (void) e;
        }

    }

    return xRet;
}
/*-- 02.11.2006 12:08:33---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PushPageHeader(SectionPropertyMap::PageType eType)
{
    //get the section context
    PropertyMapPtr pContext = DomainMapper_Impl::GetTopContextOfType(CONTEXT_SECTION);
    //ask for the header name of the given type
    SectionPropertyMap* pSectionContext = dynamic_cast< SectionPropertyMap* >( pContext.get() );
    if(pSectionContext)
    {
        uno::Reference< beans::XPropertySet > xPageStyle =
            pSectionContext->GetPageStyle(
                GetPageStyles(),
                m_xTextFactory,
                eType == SectionPropertyMap::PAGE_FIRST );
        try
        {
            PropertyNameSupplier& rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier();
            //switch on header use
            xPageStyle->setPropertyValue(
                    rPropNameSupplier.GetName(PROP_HEADER_IS_ON),
                    uno::makeAny(sal_True) );
            // if a left header is available then header are not shared
            bool bLeft = eType == SectionPropertyMap::PAGE_LEFT;
            if( bLeft )
                xPageStyle->setPropertyValue(rPropNameSupplier.GetName(PROP_HEADER_IS_SHARED), uno::makeAny( false ));

            //set the interface
            uno::Reference< text::XText > xHeaderText;
            xPageStyle->getPropertyValue(rPropNameSupplier.GetName( bLeft ? PROP_HEADER_TEXT_LEFT : PROP_HEADER_TEXT) ) >>= xHeaderText;
            m_aTextAppendStack.push( uno::Reference< text::XTextAppend >( xHeaderText, uno::UNO_QUERY_THROW));
        }
        catch( uno::Exception& e)
        {
            (void) e;
        }
    }
}
/*-- 24.07.2006 09:41:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PushPageFooter(SectionPropertyMap::PageType eType)
{
    //get the section context
    PropertyMapPtr pContext = DomainMapper_Impl::GetTopContextOfType(CONTEXT_SECTION);
    //ask for the footer name of the given type
    SectionPropertyMap* pSectionContext = dynamic_cast< SectionPropertyMap* >( pContext.get() );
    if(pSectionContext)
    {
        uno::Reference< beans::XPropertySet > xPageStyle =
                pSectionContext->GetPageStyle(
                    GetPageStyles(),
                    m_xTextFactory,
                    eType == SectionPropertyMap::PAGE_FIRST );
        try
        {
            PropertyNameSupplier& rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier();
            //switch on footer use
            xPageStyle->setPropertyValue(
                    rPropNameSupplier.GetName(PROP_FOOTER_IS_ON),
                    uno::makeAny(sal_True) );
            // if a left header is available then footer is not shared
            bool bLeft = eType == SectionPropertyMap::PAGE_LEFT;
            if( bLeft )
                xPageStyle->setPropertyValue(rPropNameSupplier.GetName(PROP_FOOTER_IS_SHARED), uno::makeAny( false ));
            //set the interface
            uno::Reference< text::XText > xFooterText;
            xPageStyle->getPropertyValue(rPropNameSupplier.GetName( bLeft ? PROP_FOOTER_TEXT_LEFT : PROP_FOOTER_TEXT) ) >>= xFooterText;
            m_aTextAppendStack.push(uno::Reference< text::XTextAppend >( xFooterText, uno::UNO_QUERY_THROW ));
        }
        catch( uno::Exception& e)
        {
            (void) e;
        }
    }
}
/*-- 24.07.2006 09:41:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PopPageHeaderFooter()
{
    //header and footer always have an empty paragraph at the end
    //this has to be removed
    RemoveLastParagraph( );
    m_aTextAppendStack.pop();
}
/*-- 24.05.2007 14:22:28---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PushFootOrEndnote( bool bIsFootnote )
{
    try
    {
        PropertyMapPtr pTopContext = GetTopContext();
        uno::Reference< text::XText > xFootnoteText( GetTextFactory()->createInstance(
            bIsFootnote ?
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.Footnote") ) : ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.Endnote") )),
            uno::UNO_QUERY_THROW );
        uno::Reference< text::XFootnote > xFootnote( xFootnoteText, uno::UNO_QUERY_THROW );
        pTopContext->SetFootnote( xFootnote );
        if( pTopContext->GetFootnoteSymbol() != 0)
        {
            xFootnote->setLabel( ::rtl::OUString( pTopContext->GetFootnoteSymbol() ) );
        }
        FontTablePtr pFontTable = GetFontTable();
        uno::Sequence< beans::PropertyValue > aFontProperties;
        if( pFontTable && pTopContext->GetFootnoteFontId() >= 0 && pFontTable->size() > (size_t)pTopContext->GetFootnoteFontId() )
        {
            const FontEntry::Pointer_t pFontEntry(pFontTable->getFontEntry(sal_uInt32(pTopContext->GetFootnoteFontId())));
            PropertyMapPtr aFontProps( new PropertyMap );
            aFontProps->Insert(PROP_CHAR_FONT_NAME, true, uno::makeAny( pFontEntry->sFontName  ));
            aFontProps->Insert(PROP_CHAR_FONT_CHAR_SET, true, uno::makeAny( (sal_Int16)pFontEntry->nTextEncoding  ));
            aFontProps->Insert(PROP_CHAR_FONT_PITCH, true, uno::makeAny( pFontEntry->nPitchRequest  ));
            aFontProperties = aFontProps->GetPropertyValues();
        }
        else if(pTopContext->GetFootnoteFontName().getLength())
        {
            PropertyMapPtr aFontProps( new PropertyMap );
            aFontProps->Insert(PROP_CHAR_FONT_NAME, true, uno::makeAny( pTopContext->GetFootnoteFontName()  ));
            aFontProperties = aFontProps->GetPropertyValues();
        }
        appendTextContent( uno::Reference< text::XTextContent >( xFootnoteText, uno::UNO_QUERY_THROW ), aFontProperties );
        m_aTextAppendStack.push(uno::Reference< text::XTextAppend >( xFootnoteText, uno::UNO_QUERY_THROW ));

        // Redlines for the footnote anchor
        CheckRedline( xFootnote->getAnchor( ) );
    }
    catch( uno::Exception& )
    {
        OSL_ENSURE( false, "exception in PushFootOrEndnote" );
    }
}

void DomainMapper_Impl::CreateRedline( uno::Reference< text::XTextRange > xRange, RedlineParamsPtr& pRedline )
{
    if ( pRedline.get( ) )
    {
        try
        {
            ::rtl::OUString sType;
            PropertyNameSupplier & rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier(  );
            switch ( pRedline->m_nToken & 0xffff )
            {
            case ooxml::OOXML_mod:
                sType = rPropNameSupplier.GetName( PROP_FORMAT );
                break;
            case ooxml::OOXML_ins:
                sType = rPropNameSupplier.GetName( PROP_INSERT );
                break;
            case ooxml::OOXML_del:
                sType = rPropNameSupplier.GetName( PROP_DELETE );
                break;
            }
            uno::Reference < text::XRedline > xRedline( xRange, uno::UNO_QUERY_THROW );
            beans::PropertyValues aRedlineProperties( 2 );
            beans::PropertyValue * pRedlineProperties = aRedlineProperties.getArray(  );
            pRedlineProperties[0].Name = rPropNameSupplier.GetName( PROP_REDLINE_AUTHOR );
            pRedlineProperties[0].Value <<= pRedline->m_sAuthor;
            pRedlineProperties[1].Name = rPropNameSupplier.GetName( PROP_REDLINE_DATE_TIME );
            pRedlineProperties[1].Value <<= lcl_DateStringToDateTime( pRedline->m_sDate );
            
            xRedline->makeRedline( sType, aRedlineProperties );
        }
        catch( const uno::Exception & rEx )
        {
            ( void ) rEx;
// disabled: current writer redline impl. rather primitive, so it gets annoying
//            OSL_ENSURE( false, "Exception in makeRedline" );
        }
    }
}

void DomainMapper_Impl::CheckParaRedline( uno::Reference< text::XTextRange > xRange )
{
    if ( m_pParaRedline.get( ) )
    {
        CreateRedline( xRange, m_pParaRedline );
        ResetParaRedline( ); 
    }
}

void DomainMapper_Impl::CheckRedline( uno::Reference< text::XTextRange > xRange )
{
    vector<RedlineParamsPtr>::iterator pIt = m_aRedlines.begin( );
    vector< RedlineParamsPtr > aCleaned;
    for (; pIt != m_aRedlines.end( ); pIt++ )
    {
        CreateRedline( xRange, *pIt );
        
        // Adding the non-mod redlines to the temporary vector
        if ( pIt->get( ) && ( ( *pIt )->m_nToken & 0xffff ) != ooxml::OOXML_mod ) 
        {
            aCleaned.push_back( *pIt );
        }
    }

    m_aRedlines.swap( aCleaned );
}

void DomainMapper_Impl::StartParaChange( )
{
    m_bIsParaChange = true;
}

void DomainMapper_Impl::EndParaChange( )
{
    m_bIsParaChange = false;
}

/*-- 22.12.2008 13:45:15---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PushAnnotation()
{
    try
    {
        PropertyMapPtr pTopContext = GetTopContext();
        m_bIsInComments = true;
        m_xAnnotationField = uno::Reference< beans::XPropertySet >( GetTextFactory()->createInstance(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextField.Annotation") ) ),
            uno::UNO_QUERY_THROW );
        uno::Reference< text::XText > xAnnotationText;
        m_xAnnotationField->getPropertyValue(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("TextRange"))) >>= xAnnotationText;
        m_aTextAppendStack.push(uno::Reference< text::XTextAppend >( xAnnotationText, uno::UNO_QUERY_THROW ));
    }
    catch( uno::Exception& )
    {
        OSL_ENSURE( false, "exception in PushAnnotation" );
    }
}
/*-- 24.05.2007 14:22:29---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PopFootOrEndnote()
{
    RemoveLastParagraph();
    m_aTextAppendStack.pop();
}
/*-- 22.12.2008 13:45:15---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PopAnnotation()
{
    m_bIsInComments = false;

    RemoveLastParagraph();
    m_aTextAppendStack.pop();

    if ( m_nAnnotationId != -1 )
    {
        // See if the annotation will be a single position or a range.
        AnnotationPosition& aAnnotationPosition = m_aAnnotationPositions[ m_nAnnotationId ];
        if ( !aAnnotationPosition.m_xStart.is()
             || !aAnnotationPosition.m_xEnd.is() )
        {
            uno::Sequence< beans::PropertyValue > aEmptyProperties;
            appendTextContent( uno::Reference< text::XTextContent >( m_xAnnotationField, uno::UNO_QUERY_THROW ), aEmptyProperties );
        }
        else
        {
            // Create a range that points to the annotation start/end.
            uno::Reference<text::XText> xText = aAnnotationPosition.m_xStart->getText();
            uno::Reference<text::XTextCursor> xCursor = xText->createTextCursorByRange( aAnnotationPosition.m_xStart );
            xCursor->gotoRange( aAnnotationPosition.m_xEnd, true );
            uno::Reference<text::XTextRange> xTextRange(xCursor, uno::UNO_QUERY_THROW);

            // Attach the annotation to the range.
            uno::Reference<text::XTextAppend> xTextAppend = m_aTextAppendStack.top().xTextAppend;
            xTextAppend->insertTextContent(xTextRange, uno::Reference<text::XTextContent>(m_xAnnotationField, uno::UNO_QUERY_THROW), !xCursor->isCollapsed());
        }
        m_aAnnotationPositions.erase( m_nAnnotationId );
    }

    m_xAnnotationField.clear();
    m_nAnnotationId = -1;
}

void DomainMapper_Impl::PushShapeContext( const uno::Reference< drawing::XShape > xShape )
{
    m_bIsInShape = true;
    try 
    {
        // Add the shape to the text append stack
        m_aTextAppendStack.push( uno::Reference< text::XTextAppend >( xShape, uno::UNO_QUERY_THROW ) );
        m_bShapeContextAdded = true;

        // Add the shape to the anchored objects stack
        uno::Reference< text::XTextContent > xTxtContent( xShape, uno::UNO_QUERY_THROW );
        m_aAnchoredStack.push( xTxtContent );

        PropertyNameSupplier& rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier();

        uno::Reference< beans::XPropertySet > xProps( xShape, uno::UNO_QUERY_THROW );
        xProps->setPropertyValue( 
                rPropNameSupplier.GetName( PROP_ANCHOR_TYPE ),
                uno::makeAny( text::TextContentAnchorType_AT_PARAGRAPH ) );
        xProps->setPropertyValue(
                rPropNameSupplier.GetName( PROP_OPAQUE ),
                uno::makeAny( true ) );
    } 
    catch ( const uno::Exception& e )
    {
        (void) e;
#if DEBUG_DOMAINMAPPER
        dmapper_logger->element("exception");
#endif
    }
}

/*-- 20.03.2008 09:01:59---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PopShapeContext()
{
    if ( m_bShapeContextAdded )
    {
        m_aTextAppendStack.pop();
        m_bShapeContextAdded = false;
    }
    m_bIsInShape = false;
}
/*-- 12.09.2006 08:07:55---------------------------------------------------

  -----------------------------------------------------------------------*/
::rtl::OUString lcl_FindQuotedText( const ::rtl::OUString& rCommand,
                const sal_Char* cStartQuote, const sal_Unicode uEndQuote )
{
    ::rtl::OUString sRet;
    ::rtl::OUString sStartQuote( ::rtl::OUString::createFromAscii(cStartQuote) );
    sal_Int32 nStartIndex = rCommand.indexOf( sStartQuote );
    if( nStartIndex >= 0 )
    {
        sal_Int32 nStartLength = sStartQuote.getLength();
        sal_Int32 nEndIndex = rCommand.indexOf( uEndQuote, nStartIndex + nStartLength);
        if( nEndIndex > nStartIndex )
        {
            sRet = rCommand.copy( nStartIndex + nStartLength, nEndIndex - nStartIndex - nStartLength);
        }
    }
    return sRet;

}
/*-- 08.09.2006 14:05:17---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int16 lcl_ParseNumberingType( const ::rtl::OUString& rCommand )
{
    sal_Int16 nRet = style::NumberingType::PAGE_DESCRIPTOR;

    //  The command looks like: " PAGE \* Arabic "
    ::rtl::OUString sNumber = lcl_FindQuotedText(rCommand, "\\* ", ' ');

    if( sNumber.getLength() )
    {
        //todo: might make sense to hash this list, too
        struct NumberingPairs
        {
            const sal_Char* cWordName;
            sal_Int16       nType;
        };
        static const NumberingPairs aNumberingPairs[] =
        {
            {"Arabic", style::NumberingType::ARABIC}
            ,{"ROMAN", style::NumberingType::ROMAN_UPPER}
            ,{"roman", style::NumberingType::ROMAN_LOWER}
            ,{"ALPHABETIC", style::NumberingType::CHARS_UPPER_LETTER}
            ,{"alphabetic", style::NumberingType::CHARS_LOWER_LETTER}
            ,{"CircleNum", style::NumberingType::CIRCLE_NUMBER}
            ,{"ThaiArabic", style::NumberingType::CHARS_THAI}
            ,{"ThaiCardText", style::NumberingType::CHARS_THAI}
            ,{"ThaiLetter", style::NumberingType::CHARS_THAI}
//            ,{"SBCHAR", style::NumberingType::}
//            ,{"DBCHAR", style::NumberingType::}
//            ,{"DBNUM1", style::NumberingType::}
//            ,{"DBNUM2", style::NumberingType::}
//            ,{"DBNUM3", style::NumberingType::}
//            ,{"DBNUM4", style::NumberingType::}
            ,{"Aiueo", style::NumberingType::AIU_FULLWIDTH_JA}
            ,{"Iroha", style::NumberingType::IROHA_FULLWIDTH_JA}
//            ,{"ZODIAC1", style::NumberingType::}
//            ,{"ZODIAC2", style::NumberingType::}
//            ,{"ZODIAC3", style::NumberingType::}
//            ,{"CHINESENUM1", style::NumberingType::}
//            ,{"CHINESENUM2", style::NumberingType::}
//            ,{"CHINESENUM3", style::NumberingType::}
            ,{"ArabicAlpha", style::NumberingType::CHARS_ARABIC}
            ,{"ArabicAbjad", style::NumberingType::FULLWIDTH_ARABIC}
/* possible values:
style::NumberingType::

    CHARS_UPPER_LETTER_N
    CHARS_LOWER_LETTER_N
    TRANSLITERATION
    NATIVE_NUMBERING
    CIRCLE_NUMBER
    NUMBER_LOWER_ZH
    NUMBER_UPPER_ZH
    NUMBER_UPPER_ZH_TW
    TIAN_GAN_ZH
    DI_ZI_ZH
    NUMBER_TRADITIONAL_JA
    AIU_HALFWIDTH_JA
    IROHA_HALFWIDTH_JA
    NUMBER_UPPER_KO
    NUMBER_HANGUL_KO
    HANGUL_JAMO_KO
    HANGUL_SYLLABLE_KO
    HANGUL_CIRCLED_JAMO_KO
    HANGUL_CIRCLED_SYLLABLE_KO
    CHARS_HEBREW
    CHARS_NEPALI
    CHARS_KHMER
    CHARS_LAO
    CHARS_TIBETAN
    CHARS_CYRILLIC_UPPER_LETTER_BG
    CHARS_CYRILLIC_LOWER_LETTER_BG
    CHARS_CYRILLIC_UPPER_LETTER_N_BG
    CHARS_CYRILLIC_LOWER_LETTER_N_BG
    CHARS_CYRILLIC_UPPER_LETTER_RU
    CHARS_CYRILLIC_LOWER_LETTER_RU
    CHARS_CYRILLIC_UPPER_LETTER_N_RU
    CHARS_CYRILLIC_LOWER_LETTER_N_RU
    CHARS_CYRILLIC_UPPER_LETTER_SR
    CHARS_CYRILLIC_LOWER_LETTER_SR
    CHARS_CYRILLIC_UPPER_LETTER_N_SR
    CHARS_CYRILLIC_LOWER_LETTER_N_SR*/

        };
        for( sal_uInt32 nNum = 0; nNum < sizeof(aNumberingPairs)/sizeof( NumberingPairs ); ++nNum)
        {
            if( /*sCommand*/sNumber.equalsAscii(aNumberingPairs[nNum].cWordName ))
            {
                nRet = aNumberingPairs[nNum].nType;
                break;
            }
        }

    }
    return nRet;
}
/*-- 08.09.2006 13:52:09---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString lcl_ParseFormat( const ::rtl::OUString& rCommand )
{
    //  The command looks like: " DATE \@ "dd MMMM yyyy"
    return lcl_FindQuotedText(rCommand, "\\@ \"", '\"');
}
/*-- 19.09.2006 10:01:20---------------------------------------------------
extract a parameter (with or without quotes) between the command and the following backslash
  -----------------------------------------------------------------------*/
::rtl::OUString lcl_ExtractParameter(const ::rtl::OUString& rCommand, sal_Int32 nCommandLength )
{
    sal_Int32 nStartIndex = nCommandLength;
    sal_Int32 nEndIndex = 0;
    sal_Int32 nQuoteIndex = rCommand.indexOf( '\"', nStartIndex);
    if( nQuoteIndex >= 0)
    {
        nStartIndex = nQuoteIndex + 1;
        nEndIndex = rCommand.indexOf( '\"', nStartIndex + 1) - 1;
    }
    else
    {
        nEndIndex = rCommand.indexOf( ::rtl::OUString::createFromAscii(" \\"), nStartIndex);
    }
    ::rtl::OUString sRet;
    if( nEndIndex > nStartIndex + 1 )
    {
        //remove spaces at start and end of the result
        if(nQuoteIndex <= 0)
        {
            const sal_Unicode* pCommandStr = rCommand.getStr();
            while( nStartIndex < nEndIndex && pCommandStr[nStartIndex] == ' ')
                    ++nStartIndex;
            while( nEndIndex > nStartIndex && pCommandStr[nEndIndex] == ' ')
                    --nEndIndex;
        }
        sRet = rCommand.copy( nStartIndex, nEndIndex - nStartIndex + 1);
    }
    return sRet;
}

/*-- 15.09.2006 10:57:57---------------------------------------------------

  -----------------------------------------------------------------------*/
::rtl::OUString lcl_ExctractAskVariableAndHint( const ::rtl::OUString& rCommand, ::rtl::OUString& rHint )
{
    // the first word after "ASK " is the variable
    // the text after the variable and before a '\' is the hint
    // if no hint is set the variable is used as hint
    // the quotes of the hint have to be removed
    sal_Int32 nIndex = rCommand.indexOf( ' ', 2);//find last space after 'ASK'
    while(rCommand.getStr()[nIndex] == ' ')
        ++nIndex;
    ::rtl::OUString sShortCommand( rCommand.copy( nIndex ) ); //cut off the " ASK "

    nIndex = 0;
    sShortCommand = sShortCommand.getToken( 0, '\\', nIndex);
    nIndex = 0;
    ::rtl::OUString sRet = sShortCommand.getToken( 0, ' ', nIndex);
    if( nIndex > 0)
        rHint = sShortCommand.copy( nIndex );
    if( !rHint.getLength() )
        rHint = sRet;
    return sRet;
}
/*-- 24.01.2007 16:04:33---------------------------------------------------

  -----------------------------------------------------------------------*/
bool lcl_FindInCommand(
    const ::rtl::OUString& rCommand,
    sal_Unicode cSwitch,
    ::rtl::OUString& rValue )
{
    bool bRet = false;
    ::rtl::OUString sSearch('\\');
    sSearch += ::rtl::OUString( cSwitch );
    sal_Int32 nIndex = rCommand.indexOf( sSearch  );
    if( nIndex >= 0 )
    {
        bRet = true;
        //find next '\' or end of string
        sal_Int32 nEndIndex = rCommand.indexOf( '\\', nIndex + 1);
        if( nEndIndex < 0 )
            nEndIndex = rCommand.getLength() - 1;
        if( nEndIndex - nIndex > 3 )
            rValue = rCommand.copy( nIndex + 3, nEndIndex - nIndex - 3);
    }
    return bRet;
}
/*-- 01.09.2006 11:48:08---------------------------------------------------

  -----------------------------------------------------------------------*/
/*void DomainMapper_Impl::CreateField( ::rtl::OUString& rCommand )
{
    static FieldConversionMap_t aFieldConversionMap;
    static bool bFilled = false;
    m_bSetUserFieldContent = false;
    if(!bFilled)
    {
        static const FieldConversion aFields[] =
        {
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ADDRESSBLOCK")),  "",                         "", FIELD_ADDRESSBLOCK  },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ADVANCE")),       "",                         "", FIELD_ADVANCE       },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ASK")),           "SetExpression",             "SetExpression", FIELD_ASK      },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AUTONUM")),       "SetExpression",            "SetExpression", FIELD_AUTONUM   },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AUTONUMLGL")),     "SetExpression",            "SetExpression", FIELD_AUTONUMLGL },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AUTONUMOUT")),     "SetExpression",            "SetExpression", FIELD_AUTONUMOUT },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AUTHOR")),        "Author",                   "", FIELD_AUTHOR       },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DATE")),          "DateTime",                 "", FIELD_DATE         },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("COMMENTS")),      "DocInfo.Description",      "", FIELD_COMMENTS     },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CREATEDATE")),    "DocInfo.CreateDateTime",   "", FIELD_CREATEDATE   },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DOCPROPERTY")),   "",                         "", FIELD_DOCPROPERTY },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DOCVARIABLE")),   "User",                     "", FIELD_DOCVARIABLE  },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("EDITTIME")),      "DocInfo.EditTime",         "", FIELD_EDITTIME     },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FILLIN")),        "Input",                    "", FIELD_FILLIN       },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FILENAME")),      "FileName",                 "", FIELD_FILENAME     },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FILESIZE")),      "",                         "", FIELD_FILESIZE     },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FORMULA")),     "",                           "", FIELD_FORMULA
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("GOTOBUTTON")),    "",                         "", FIELD_GOTOBUTTON   },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("HYPERLINK")),     "",                         "", FIELD_HYPERLINK    },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("IF")),            "ConditionalText",          "", FIELD_IF           },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("INFO")),      "","", FIELD_INFO         },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("INCLUDEPICTURE")), "",                        "", FIELD_INCLUDEPICTURE},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("KEYWORDS")),      "DocInfo.KeyWords",         "", FIELD_KEYWORDS     },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("LASTSAVEDBY")),   "DocInfo.ChangeAuthor",                         "", FIELD_LASTSAVEDBY  },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MACROBUTTON")),   "Macro",                         "", FIELD_MACROBUTTON  },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MERGEFIELD")),    "Database",                 "Database", FIELD_MERGEFIELD},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MERGEREC")),      "DatabaseNumberOfSet",      "", FIELD_MERGEREC     },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MERGESEQ")),      "",                         "", FIELD_MERGESEQ     },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("NEXT")),          "DatabaseNextSet",          "", FIELD_NEXT         },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("NEXTIF")),        "DatabaseNextSet",          "", FIELD_NEXTIF       },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PAGE")),          "PageNumber",               "", FIELD_PAGE         },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("REF")),           "",                         "", FIELD_REF          },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("REVNUM")),        "DocInfo.Revision",         "", FIELD_REVNUM       },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SAVEDATE")),      "DocInfo.Change",           "", FIELD_SAVEDATE     },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SECTION")),       "",                         "", FIELD_SECTION      },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SECTIONPAGES")),  "",                         "", FIELD_SECTIONPAGES },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SEQ")),           "SetExpression",            "SetExpression", FIELD_SEQ          },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SET")),           "","", FIELD_SET          },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SKIPIF")),"",                                 "", FIELD_SKIPIF       },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("STYLEREF")),"",                               "", FIELD_STYLEREF     },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SUBJECT")),       "DocInfo.Subject",          "", FIELD_SUBJECT      },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SYMBOL")),"",                                 "", FIELD_SYMBOL       },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TEMPLATE")),      "TemplateName",             "", FIELD_TEMPLATE},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TIME")),          "DateTime",                 "", FIELD_TIME         },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TITLE")),         "DocInfo.Title",            "", FIELD_TITLE        },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("USERINITIALS")),  "ExtendedUser",              "", FIELD_USERINITIALS},
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("USERADDRESS")),   "",                         "", FIELD_USERADDRESS  },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("USERNAME")),      "ExtendedUser",             "", FIELD_USERNAME     }
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TOC")), "com.sun.star.text.ContentIndex", "", FIELD_TOC},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TC")), "com.sun.star.text.ContentIndexMark", "", FIELD_TC},

//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("")), "", "", FIELD_},

        };
        size_t nConversions = sizeof(aFields)/sizeof(FieldConversion);
        for( size_t nConversion = 0; nConversion < nConversions; ++nConversion)
        {
            aFieldConversionMap.insert( FieldConversionMap_t::value_type(
                    aFields[nConversion].sWordCommand,
                    aFields[nConversion] ));
        }

        bFilled = true;
    }
    try
    {
        uno::Reference< uno::XInterface > xFieldInterface;
        //at first determine the field type - skip first space
        ::rtl::OUString sCommand( rCommand.copy(rCommand.getLength() ? 1 : 0) );
        sal_Int32 nSpaceIndex = sCommand.indexOf( ' ' );
        if( 0 <= nSpaceIndex )
            sCommand = sCommand.copy( 0, nSpaceIndex );

        FieldConversionMap_t::iterator aIt = aFieldConversionMap.find(sCommand);
        if(aIt != aFieldConversionMap.end())
        {
            uno::Reference< beans::XPropertySet > xFieldProperties;
            if( FIELD_HYPERLINK != aIt->second.eFieldId &&
                    FIELD_DOCPROPERTY != aIt->second.eFieldId &&
                    FIELD_TOC != aIt->second.eFieldId &&
                    FIELD_TC != aIt->second.eFieldId)
            {
                //add the service prefix
                OUString sServiceName(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextField."));
                sServiceName += ::rtl::OUString::createFromAscii(aIt->second.cFieldServiceName );
                xFieldInterface = m_xTextFactory->createInstance(sServiceName);
                xFieldProperties = uno::Reference< beans::XPropertySet >( xFieldInterface, uno::UNO_QUERY_THROW);
            }
            PropertyNameSupplier& rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier();
            switch( aIt->second.eFieldId )
            {
                case FIELD_ADDRESSBLOCK: break;
                case FIELD_ADVANCE     : break;
                case FIELD_ASK         :
                {
                    //doesn the command contain a variable name?
                    ::rtl::OUString sVariable, sHint;

                    sVariable = lcl_ExctractAskVariableAndHint( rCommand, sHint );
                    if(sVariable.getLength())
                    {
                        // determine field master name
                        uno::Reference< beans::XPropertySet > xMaster = FindOrCreateFieldMaster(
                                "com.sun.star.text.FieldMaster.SetExpression", sVariable );

                        // attach the master to the field
                        uno::Reference< text::XDependentTextField > xDependentField( xFieldInterface, uno::UNO_QUERY_THROW );
                        xDependentField->attachTextFieldMaster( xMaster );

                        // set input flag at the field
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName(PROP_IS_INPUT), uno::makeAny( true ));
                        // set the prompt
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName(PROP_HINT),
                                uno::makeAny( sHint ));
                    }
                    else
                    {
                        //don't insert the field
                        //todo: maybe import a 'normal' input field here?
                        xFieldInterface = 0;
                    }
                }
                break;
                case FIELD_AUTONUM    :
                case FIELD_AUTONUMLGL :
                case FIELD_AUTONUMOUT :
                {
                    //create a sequence field master "AutoNr"
                    uno::Reference< beans::XPropertySet > xMaster = FindOrCreateFieldMaster(
                                "com.sun.star.text.FieldMaster.SetExpression",
                                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AutoNr") ));

                    xMaster->setPropertyValue( rPropNameSupplier.GetName(PROP_SUB_TYPE),
                                uno::makeAny(text::SetVariableType::SEQUENCE));

                    //apply the numbering type
                    xFieldProperties->setPropertyValue(
                        rPropNameSupplier.GetName(PROP_NUMBERING_TYPE),
                        uno::makeAny( lcl_ParseNumberingType(rCommand) ));
                        // attach the master to the field
                    uno::Reference< text::XDependentTextField > xDependentField( xFieldInterface, uno::UNO_QUERY_THROW );
                    xDependentField->attachTextFieldMaster( xMaster );
                }
                break;
                case FIELD_AUTHOR       :
                {
                    xFieldProperties->setPropertyValue( rPropNameSupplier.GetName(PROP_FULL_NAME), uno::makeAny( true ));
                    ::rtl::OUString sParam = lcl_ExtractParameter(rCommand, sizeof(" AUTHOR") );
                    if(sParam.getLength())
                    {
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName( PROP_IS_FIXED ), uno::makeAny( true ));
                        //PROP_CURRENT_PRESENTATION is set later anyway
                    }
                }
                break;
                case FIELD_DATE:
                {
                    //not fixed,
                    xFieldProperties->setPropertyValue(
                        rPropNameSupplier.GetName(PROP_IS_FIXED),
                        uno::makeAny( false ));
                    SetNumberFormat( rCommand, xFieldProperties );
                }
                break;
                case FIELD_COMMENTS     :
                {
                    ::rtl::OUString sParam = lcl_ExtractParameter(rCommand, sizeof(" COMMENTS") );
                    if(sParam.getLength())
                    {
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName( PROP_IS_FIXED ), uno::makeAny( true ));
                        //PROP_CURRENT_PRESENTATION is set later anyway
                    }
                }
                break;
                case FIELD_CREATEDATE  :
                {
                    SetNumberFormat( rCommand, xFieldProperties );
                }
                break;
                case FIELD_DOCPROPERTY :
                {
                    //some docproperties should be imported as document statistic fields, some as DocInfo fields
                    //others should be user fields
                    ::rtl::OUString sParam = lcl_ExtractParameter(rCommand, sizeof(" DOCPROPERTY") );
                    if(sParam.getLength())
                    {
                        #define SET_ARABIC      0x01
                        #define SET_FULL_NAME   0x02
                        struct DocPropertyMap
                        {
                            const sal_Char* pDocPropertyName;
                            const sal_Char* pServiceName;
                            sal_uInt8       nFlags;
                        };
                        static const DocPropertyMap aDocProperties[] =
                        {
                            {"Author",           "Author",                  SET_FULL_NAME},
                            {"CreateTime",       "DocInfo.CreateDateTime",  0},
                            {"Characters",       "CharacterCount",          SET_ARABIC},
                            {"Comments",         "DocInfo.Description",     0},
                            {"Keywords",         "DocInfo.KeyWords",        0},
                            {"LastPrinted",      "DocInfo.PrintDateTime",   0},
                            {"LastSavedBy",      "DocInfo.ChangeAuthor",    0},
                            {"LastSavedTime",    "DocInfo.ChangeDateTime",  0},
                            {"Paragraphs",       "ParagraphCount",          SET_ARABIC},
                            {"RevisionNumber",   "DocInfo.Revision",        0},
                            {"Subject",          "DocInfo.Subject",         0},
                            {"Template",         "TemplateName",            0},
                            {"Title",            "DocInfo.Title",           0},
                            {"TotalEditingTime", "DocInfo.EditTime",        9},
                            {"Words",            "WordCount",               SET_ARABIC}

                            //other available DocProperties:
                            //Bytes, Category, CharactersWithSpaces, Company
                            //HyperlinkBase,
                            //Lines, Manager, NameofApplication, ODMADocId, Pages,
                            //Security,
                        };
                        //search for a field mapping
                        ::rtl::OUString sFieldServiceName;
                        sal_uInt16 nMap = 0;
                        for( ; nMap < sizeof(aDocProperties) / sizeof(DocPropertyMap); ++nMap )
                        {
                            if(sParam.equalsAscii(aDocProperties[nMap].pDocPropertyName))
                            {
                                sFieldServiceName = ::rtl::OUString::createFromAscii(aDocProperties[nMap].pServiceName);
                                break;
                            }
                        }
                        ::rtl::OUString sServiceName(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextField."));
                        if(sFieldServiceName.getLength())
                        {
                            sServiceName += sFieldServiceName;
                            xFieldInterface = m_xTextFactory->createInstance(sServiceName);
                            xFieldProperties = uno::Reference< beans::XPropertySet >( xFieldInterface, uno::UNO_QUERY_THROW);
                            if(0 != (aDocProperties[nMap].nFlags & SET_ARABIC))
                                xFieldProperties->setPropertyValue(
                                    rPropNameSupplier.GetName(PROP_NUMBERING_TYPE),
                                    uno::makeAny( style::NumberingType::ARABIC ));
                            else if(0 != (aDocProperties[nMap].nFlags & SET_FULL_NAME))
                                xFieldProperties->setPropertyValue(
                                    rPropNameSupplier.GetName(PROP_FULL_NAME), uno::makeAny( true ));

                        }
                        else
                        {
                            //create a user field and type
                            uno::Reference< beans::XPropertySet > xMaster =
                                FindOrCreateFieldMaster( "com.sun.star.text.FieldMaster.User", sParam );
                            sServiceName += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("User"));
                            xFieldInterface = m_xTextFactory->createInstance(sServiceName);
                            xFieldProperties = uno::Reference< beans::XPropertySet >( xFieldInterface, uno::UNO_QUERY_THROW);
                            uno::Reference< text::XDependentTextField > xDependentField( xFieldInterface, uno::UNO_QUERY_THROW );
                            xDependentField->attachTextFieldMaster( xMaster );
                            m_bSetUserFieldContent = true;
                        }
                    }
                }
                #undef SET_ARABIC
                #undef SET_FULL_NAME
                break;
                case FIELD_DOCVARIABLE  :
                {
                    ::rtl::OUString sParam = lcl_ExtractParameter(rCommand, sizeof(" DOCVARIABLE") );
                    //create a user field and type
                    uno::Reference< beans::XPropertySet > xMaster =
                        FindOrCreateFieldMaster( "com.sun.star.text.FieldMaster.User", sParam );
                    uno::Reference< text::XDependentTextField > xDependentField( xFieldInterface, uno::UNO_QUERY_THROW );
                    xDependentField->attachTextFieldMaster( xMaster );
                    m_bSetUserFieldContent = true;
                }
                break;
                case FIELD_EDITTIME     :
                    //it's a numbering type, no number format! SetNumberFormat( rCommand, xFieldProperties );
                break;
                case FIELD_FILLIN       :
                {
                    sal_Int32 nIndex = 0;
                    xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_HINT), uno::makeAny( rCommand.getToken( 1, '\"', nIndex)));
                }
                break;
                case FIELD_FILENAME:
                {
                    sal_Int32 nNumberingTypeIndex = rCommand.indexOf( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\\p")));
                    xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_FILE_FORMAT),
                            uno::makeAny( nNumberingTypeIndex > 0 ? text::FilenameDisplayFormat::FULL : text::FilenameDisplayFormat::NAME ));
                }
                break;
                case FIELD_FILESIZE     : break;
                case FIELD_FORMULA : break;
                case FIELD_GOTOBUTTON   : break;
                case FIELD_HYPERLINK:
                {
                    sal_Int32 nStartQuote = rCommand.indexOf( '\"' );
                    sal_Int32 nEndQuote = nStartQuote < rCommand.getLength() + 1 ? rCommand.indexOf( '\"', nStartQuote + 1) : -1;
                    if( nEndQuote > 0)
                        m_sHyperlinkURL = rCommand.copy(nStartQuote + 1, nEndQuote - nStartQuote - 1);
                }
                break;
                case FIELD_IF           : break;
                case FIELD_INFO         : break;
                case FIELD_INCLUDEPICTURE: break;
                case FIELD_KEYWORDS     :
                {
                    ::rtl::OUString sParam = lcl_ExtractParameter(rCommand, sizeof(" KEYWORDS") );
                    if(sParam.getLength())
                    {
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName( PROP_IS_FIXED ), uno::makeAny( true ));
                        //PROP_CURRENT_PRESENTATION is set later anyway
                    }
                }
                break;
                case FIELD_LASTSAVEDBY  : break;
                case FIELD_MACROBUTTON:
                {
                    //extract macro name
                    sal_Int32 nIndex = sizeof(" MACROBUTTON ");
                    ::rtl::OUString sMacro = rCommand.getToken( 0, ' ', nIndex);
                    xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_MACRO_NAME), uno::makeAny( sMacro ));

                    //extract quick help text
                    if( rCommand.getLength() > nIndex + 1)
                    {
                        xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_HINT),
                            uno::makeAny( rCommand.copy( nIndex )));
                    }
                }
                break;
                case FIELD_MERGEFIELD  :
                {
                    //todo: create a database field and fieldmaster pointing to a column, only
                    ::rtl::OUString sParam = lcl_ExtractParameter(rCommand, sizeof(" MERGEFIELD") );
                    //create a user field and type
                    uno::Reference< beans::XPropertySet > xMaster =
                        FindOrCreateFieldMaster( "com.sun.star.text.FieldMaster.Database", sParam );

//                    xFieldProperties->setPropertyValue(
//                             ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FieldCode")),
//                             uno::makeAny( rCommand.copy( nIndex + 1 )));
                    uno::Reference< text::XDependentTextField > xDependentField( xFieldInterface, uno::UNO_QUERY_THROW );
                    xDependentField->attachTextFieldMaster( xMaster );
                    m_bSetUserFieldContent = true;
                }
                break;
                case FIELD_MERGEREC     : break;
                case FIELD_MERGESEQ     : break;
                case FIELD_NEXT         : break;
                case FIELD_NEXTIF       : break;
                case FIELD_PAGE        :
                    xFieldProperties->setPropertyValue(
                        rPropNameSupplier.GetName(PROP_NUMBERING_TYPE),
                        uno::makeAny( lcl_ParseNumberingType(rCommand) ));
                    xFieldProperties->setPropertyValue(
                        rPropNameSupplier.GetName(PROP_SUB_TYPE),
                        uno::makeAny( text::PageNumberType_CURRENT ));

                break;
                case FIELD_REF          : break;
                case FIELD_REVNUM       : break;
                case FIELD_SAVEDATE     :
                    SetNumberFormat( rCommand, xFieldProperties );
                break;
                case FIELD_SECTION      : break;
                case FIELD_SECTIONPAGES : break;
                case FIELD_SEQ          : break;
                case FIELD_SET          : break;
                case FIELD_SKIPIF       : break;
                case FIELD_STYLEREF     : break;
                case FIELD_SUBJECT      :
                {
                    ::rtl::OUString sParam = lcl_ExtractParameter(rCommand, sizeof(" SUBJECT") );
                    if(sParam.getLength())
                    {
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName( PROP_IS_FIXED ), uno::makeAny( true ));
                        //PROP_CURRENT_PRESENTATION is set later anyway
                    }
                }
                break;
                case FIELD_SYMBOL       : break;
                case FIELD_TEMPLATE: break;
                case FIELD_TIME         :
                    SetNumberFormat( rCommand, xFieldProperties );
                break;
                case FIELD_TITLE        :
                {
                    ::rtl::OUString sParam = lcl_ExtractParameter(rCommand, sizeof(" TITLE") );
                    if(sParam.getLength())
                    {
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName( PROP_IS_FIXED ), uno::makeAny( true ));
                        //PROP_CURRENT_PRESENTATION is set later anyway
                    }
                }
                break;
                case FIELD_USERINITIALS:
                {
                    xFieldProperties->setPropertyValue(
                        rPropNameSupplier.GetName(PROP_USER_DATA_TYPE), uno::makeAny( text::UserDataPart::SHORTCUT ));
                    //todo: if initials are provided - set them as fixed content
                    ::rtl::OUString sParam = lcl_ExtractParameter(rCommand, sizeof(" USERINITIALS") );
                    if(sParam.getLength())
                    {
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName( PROP_IS_FIXED ), uno::makeAny( true ));
                        //PROP_CURRENT_PRESENTATION is set later anyway
                    }
                }
                break;
                case FIELD_USERADDRESS  : //todo: user address collects street, city ...
                break;
                case FIELD_USERNAME     : //todo: user name is firstname + lastname
                break;
                case FIELD_TOC:
                {
                    ::rtl::OUString sValue;
                    bool bTableOfFigures = false;
                    bool bHyperlinks = false;
                    bool bFromOutline = false;
                    bool bFromEntries = false;
                    ::rtl::OUString sTemplate;
                    ::rtl::OUString sChapterNoSeparator;
//                  \a Builds a table of figures but does not include the captions's label and number
                    if( lcl_FindInCommand( rCommand, 'a', sValue ))
                    { //make it a table of figures
                        bTableOfFigures = true;
                    }
//                  \b Uses a bookmark to specify area of document from which to build table of contents
//                    if( lcl_FindInCommand( rCommand, 'b', sValue ))
//                    { //todo: sValue contains the bookmark name - unsupported feature
//                    }
                    if( lcl_FindInCommand( rCommand, 'c', sValue ))
//                  \c Builds a table of figures of the given label
                    {
                        //todo: sValue contains the label's name
                        bTableOfFigures = true;
                    }
//                  \d Defines the separator between sequence and page numbers
                    if( lcl_FindInCommand( rCommand, 'd', sValue ))
                    {
                        //todo: insert the chapter number into each level and insert the separator additionally
                        sChapterNoSeparator = sValue;
                    }
//                  \f Builds a table of contents using TC entries instead of outline levels
                    if( lcl_FindInCommand( rCommand, 'f', sValue ))
                    {
                        //todo: sValue can contain a TOC entry identifier - use unclear
                        bFromEntries = true;
                    }
//                  \h Hyperlinks the entries and page numbers within the table of contents
                    if( lcl_FindInCommand( rCommand, 'h', sValue ))
                    {
                        //todo: make all entries to hyperlinks
                        bHyperlinks = true;
                    }
//                  \l Defines the TC entries field level used to build a table of contents
//                    if( lcl_FindInCommand( rCommand, 'l', sValue ))
//                    {
                            //todo: entries can only be included completely
//                    }
//                  \n Builds a table of contents or a range of entries, sucah as ?-9? in a table of contents without page numbers
//                    if( lcl_FindInCommand( rCommand, 'n', sValue ))
//                    {
                        //todo: what does the description mean?
//                    }
//                  \o  Builds a table of contents by using outline levels instead of TC entries
                    if( lcl_FindInCommand( rCommand, 'o', sValue ))
                    {
                        bFromOutline = true;
                    }
//                  \p Defines the separator between the table entry and its page number
                    if( lcl_FindInCommand( rCommand, 'p', sValue ))
                    {  }
//                  \s  Builds a table of contents by using a sequence type
                    if( lcl_FindInCommand( rCommand, 's', sValue ))
                    {  }
//                  \t  Builds a table of contents by using style names other than the standard outline styles
                    if( lcl_FindInCommand( rCommand, 't', sValue ))
                    {
                        sTemplate = sValue;
                    }
//                  \u  Builds a table of contents by using the applied paragraph outline level
                    if( lcl_FindInCommand( rCommand, 'u', sValue ))
                    {
                        bFromOutline = true;
                        //todo: what doesn 'the applied paragraph outline level' refer to?
                    }
//                  \w Preserve tab characters within table entries
//                    if( lcl_FindInCommand( rCommand, 'w', sValue ))
//                    {
                        //todo: not supported
//                    }
//                  \x Preserve newline characters within table entries
//                    if( lcl_FindInCommand( rCommand, 'x', sValue ))
//                    {
                        //todo: unsupported
//                    }
//                  \z Hides page numbers within the table of contens when shown in Web Layout View
//                    if( lcl_FindInCommand( rCommand, 'z', sValue ))
//                    { //todo: unsupported feature  }

                    m_xTOC = uno::Reference< beans::XPropertySet >(
                            m_xTextFactory->createInstance(
                                bTableOfFigures ?
                                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.IllustrationsIndex")) :
                                ::rtl::OUString::createFromAscii(aIt->second.cFieldServiceName)),
                                uno::UNO_QUERY_THROW);
                    if( !bTableOfFigures )
                    {
                        m_xTOC->setPropertyValue( rPropNameSupplier.GetName( PROP_CREATE_FROM_OUTLINE ), uno::makeAny( bFromOutline ));
                        m_xTOC->setPropertyValue( rPropNameSupplier.GetName( PROP_CREATE_FROM_MARKS ), uno::makeAny( bFromEntries ));
                        if( sTemplate.getLength() )
                        {
                            uno::Reference< container::XIndexReplace> xParaStyles;
                            m_xTOC->getPropertyValue(rPropNameSupplier.GetName(PROP_LEVEL_PARAGRAPH_STYLES)) >>= xParaStyles;
                            uno::Sequence< rtl::OUString> aStyles(1);
                            aStyles[0] = sTemplate;
                            xParaStyles->replaceByIndex(0, uno::makeAny(aStyles));
                        }
                        if(bHyperlinks  || sChapterNoSeparator.getLength())
                        {
                            uno::Reference< container::XIndexReplace> xLevelFormats;
                            m_xTOC->getPropertyValue(rPropNameSupplier.GetName(PROP_LEVEL_FORMAT)) >>= xLevelFormats;
                            sal_Int32 nLevelCount = xLevelFormats->getCount();
                            //start with level 1, 0 is the header level
                            for( sal_Int32 nLevel = 1; nLevel < nLevelCount; ++nLevel)
                            {
                                uno::Sequence< beans::PropertyValues > aLevel;
                                xLevelFormats->getByIndex( nLevel ) >>= aLevel;
                                //create a copy of the level and add two new entries - hyperlink start and end
                                bool bChapterNoSeparator  = sChapterNoSeparator.getLength() > 0;
                                sal_Int32 nAdd = (bHyperlinks && bChapterNoSeparator) ? 4 : 2;
                                uno::Sequence< beans::PropertyValues > aNewLevel( aLevel.getLength() + nAdd);
                                beans::PropertyValues* pNewLevel = aNewLevel.getArray();
                                if( bHyperlinks )
                                {
                                    beans::PropertyValues aHyperlink(1);
                                    aHyperlink[0].Name = rPropNameSupplier.GetName( PROP_TOKEN_TYPE );
                                    aHyperlink[0].Value <<= rPropNameSupplier.GetName( PROP_TOKEN_HYPERLINK_START );
                                    pNewLevel[0] = aHyperlink;
                                    aHyperlink[0].Value <<= rPropNameSupplier.GetName( PROP_TOKEN_HYPERLINK_END );
                                    pNewLevel[aNewLevel.getLength() -1] = aHyperlink;
                                }
                                if( bChapterNoSeparator )
                                {
                                    beans::PropertyValues aChapterNo(2);
                                    aChapterNo[0].Name = rPropNameSupplier.GetName( PROP_TOKEN_TYPE );
                                    aChapterNo[0].Value <<= rPropNameSupplier.GetName( PROP_TOKEN_CHAPTER_INFO );
                                    aChapterNo[1].Name = rPropNameSupplier.GetName( PROP_CHAPTER_FORMAT );
                                    //todo: is ChapterFormat::Number correct?
                                    aChapterNo[1].Value <<= (sal_Int16)text::ChapterFormat::NUMBER;
                                    pNewLevel[aNewLevel.getLength() - (bHyperlinks ? 4 : 2) ] = aChapterNo;

                                    beans::PropertyValues aChapterSeparator(2);
                                    aChapterSeparator[0].Name = rPropNameSupplier.GetName( PROP_TOKEN_TYPE );
                                    aChapterSeparator[0].Value <<= rPropNameSupplier.GetName( PROP_TOKEN_TEXT );
                                    aChapterSeparator[1].Name = rPropNameSupplier.GetName( PROP_TEXT );
                                    aChapterSeparator[1].Value <<= sChapterNoSeparator;
                                    pNewLevel[aNewLevel.getLength() - (bHyperlinks ? 3 : 1)] = aChapterSeparator;
                                }
                                //copy the 'old' entries except the last (page no)
                                for( sal_Int32 nToken = 0; nToken < aLevel.getLength() - 1; ++nToken)
                                {
                                    pNewLevel[nToken + 1] = aLevel[nToken];
                                }
                                //copy page no entry (last or last but one depending on bHyperlinks
                                sal_Int32 nPageNo = aNewLevel.getLength() - (bHyperlinks ? 2 : 3);
                                pNewLevel[nPageNo] = aLevel[aLevel.getLength() - 1];

                                xLevelFormats->replaceByIndex( nLevel, uno::makeAny( aNewLevel ) );
                            }
                        }
                    }
                }
                break;
                case FIELD_TC :
                {
                    m_xTC = uno::Reference< beans::XPropertySet >(
                        m_xTextFactory->createInstance(
                            ::rtl::OUString::createFromAscii(aIt->second.cFieldServiceName)),
                            uno::UNO_QUERY_THROW);
                    ::rtl::OUString sTCText = lcl_ExtractParameter(rCommand, sizeof(" TC") );
                    if( sTCText.getLength())
                        m_xTC->setPropertyValue(rPropNameSupplier.GetName(PROP_ALTERNATIVE_TEXT),
                            uno::makeAny(sTCText));
                    ::rtl::OUString sValue;
                    // \f TC entry in doc with multiple tables
//                    if( lcl_FindInCommand( rCommand, 'f', sValue ))
//                    {
                        // todo: unsupported
//                    }
                    if( lcl_FindInCommand( rCommand, 'l', sValue ))
                    // \l Outline Level
                    {
                        sal_Int32 nLevel = sValue.toInt32();
                        if( sValue.getLength() && nLevel >= 0 && nLevel <= 10 )
                            m_xTC->setPropertyValue(rPropNameSupplier.GetName(PROP_LEVEL), uno::makeAny( nLevel ));
                    }
//                    if( lcl_FindInCommand( rCommand, 'n', sValue ))
//                    \n Suppress page numbers
//                    {
                        //todo: unsupported feature
//                    }
                }
                break;
            }
        }
        m_xTextField = uno::Reference< text::XTextField >( xFieldInterface, uno::UNO_QUERY );
    }
    catch( uno::Exception& )
    {
    }
}
*/

/*-- 11.09.2006 13:16:35---------------------------------------------------

  -----------------------------------------------------------------------*/
/*bool DomainMapper_Impl::IsFieldAvailable() const
{
    return m_xTextField.is() || m_xTOC.is() || m_xTC.is() || m_sHyperlinkURL.getLength();
}
*/
/*-- 14.09.2006 12:46:52---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::GetCurrentLocale(lang::Locale& rLocale)
{
    PropertyMapPtr pTopContext = GetTopContext();
    PropertyDefinition aCharLocale( PROP_CHAR_LOCALE, true );
    PropertyMap::iterator aLocaleIter = pTopContext->find( aCharLocale );
    if( aLocaleIter != pTopContext->end())
        aLocaleIter->second >>= rLocale;
    else
    {
        PropertyMapPtr pParaContext = GetTopContextOfType(CONTEXT_PARAGRAPH);
        aLocaleIter = pParaContext->find(aCharLocale);
        if( aLocaleIter != pParaContext->end())
        {
            aLocaleIter->second >>= rLocale;
        }
    }
}

/*-- 14.09.2006 12:52:58---------------------------------------------------
    extract the number format from the command and apply the resulting number
    format to the XPropertySet
  -----------------------------------------------------------------------*/
void DomainMapper_Impl::SetNumberFormat( const ::rtl::OUString& rCommand,
                            uno::Reference< beans::XPropertySet >& xPropertySet )
{
    OUString sFormatString = lcl_ParseFormat( rCommand );
    // find \h - hijri/luna calendar todo: what about saka/era calendar?
    bool bHijri = 0 < rCommand.indexOf( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\\h ")));
    lang::Locale aUSLocale;
    aUSLocale.Language = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("en"));
    aUSLocale.Country = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("US"));

    //determine current locale - todo: is it necessary to initialize this locale?
    lang::Locale aCurrentLocale = aUSLocale;
    GetCurrentLocale( aCurrentLocale );
    ::rtl::OUString sFormat = ConversionHelper::ConvertMSFormatStringToSO( sFormatString, aCurrentLocale, bHijri);

    //get the number formatter and convert the string to a format value
    try
    {
        uno::Reference< util::XNumberFormatsSupplier > xNumberSupplier( m_xTextDocument, uno::UNO_QUERY_THROW );
        long nKey = xNumberSupplier->getNumberFormats()->addNewConverted( sFormat, aUSLocale, aCurrentLocale );
        xPropertySet->setPropertyValue(
            PropertyNameSupplier::GetPropertyNameSupplier().GetName(PROP_NUMBER_FORMAT),
            uno::makeAny( nKey ));
    }
    catch(const uno::Exception& e)
    {
        (void) e;
    }
}

/*-- 15.09.2006 15:10:20---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySet > DomainMapper_Impl::FindOrCreateFieldMaster(
        const sal_Char* pFieldMasterService, const ::rtl::OUString& rFieldMasterName )
            throw(::com::sun::star::uno::Exception)
{
    // query master, create if not available
    uno::Reference< text::XTextFieldsSupplier > xFieldsSupplier( GetTextDocument(), uno::UNO_QUERY );
    uno::Reference< container::XNameAccess > xFieldMasterAccess = xFieldsSupplier->getTextFieldMasters();
    uno::Reference< beans::XPropertySet > xMaster;
    ::rtl::OUString sFieldMasterService( ::rtl::OUString::createFromAscii(pFieldMasterService) );
    ::rtl::OUStringBuffer aFieldMasterName;
    aFieldMasterName.appendAscii( pFieldMasterService );
    aFieldMasterName.append(sal_Unicode('.'));
    aFieldMasterName.append(rFieldMasterName);
    ::rtl::OUString sFieldMasterName = aFieldMasterName.makeStringAndClear();
    if(xFieldMasterAccess->hasByName(sFieldMasterName))
    {
        //get the master
        xMaster = uno::Reference< beans::XPropertySet >(xFieldMasterAccess->getByName(sFieldMasterName),
                                                                            uno::UNO_QUERY_THROW);
    }
    else
    {
        //create the master
        xMaster = uno::Reference< beans::XPropertySet >(
                m_xTextFactory->createInstance(sFieldMasterService), uno::UNO_QUERY_THROW);
        //set the master's name
//        sal_Int32 nIndex = rtl_str_indexOfStr( pFieldMasterService, "Database" );
//        if( nIndex < 0 )
            xMaster->setPropertyValue(
                    PropertyNameSupplier::GetPropertyNameSupplier().GetName(PROP_NAME),
                    uno::makeAny(rFieldMasterName));
//        else
//        {
//            xMaster->setPropertyValue(
//                    PropertyNameSupplier::GetPropertyNameSupplier().GetName(PROP_DATA_COLUMN_NAME),
//                    uno::makeAny(rFieldMasterName));
//        }
    }
    return xMaster;
}

/*-- 29.01.2007 11:33:10---------------------------------------------------
//field context starts with a 0x13
  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PushFieldContext()
{
#ifdef DEBUG_DOMAINMAPPER
    dmapper_logger->element("pushFieldContext");
#endif
    
    uno::Reference< text::XTextAppend >  xTextAppend = m_aTextAppendStack.top().xTextAppend;
    //insert a dummy char to make sure the start range doesn't move together with the to-be-appended text
    xTextAppend->appendTextPortion(::rtl::OUString( '-' ), uno::Sequence< beans::PropertyValue >() );
    uno::Reference< text::XTextCursor > xCrsr = xTextAppend->createTextCursorByRange( xTextAppend->getEnd() );
    xCrsr->goLeft( 1, false );
    m_aFieldStack.push( FieldContextPtr( new FieldContext( xCrsr->getStart() ) ) );
}
/*-- 29.01.2007 11:33:13---------------------------------------------------
//the current field context waits for the completion of the command
  -----------------------------------------------------------------------*/
bool DomainMapper_Impl::IsOpenFieldCommand() const
{
    return !m_aFieldStack.empty() && !m_aFieldStack.top()->IsCommandCompleted();
}
/*-- 29.01.2007 11:33:13---------------------------------------------------
//the current field context waits for the completion of the command
  -----------------------------------------------------------------------*/
bool DomainMapper_Impl::IsOpenField() const
{
    return !m_aFieldStack.empty();
}
/*-- 29.01.2007 11:49:13---------------------------------------------------

  -----------------------------------------------------------------------*/
FieldContext::FieldContext(uno::Reference< text::XTextRange > xStart) :
    m_bFieldCommandCompleted( false )
    ,m_xStartRange( xStart )
{
}
/*-- 29.01.2007 11:48:44---------------------------------------------------

  -----------------------------------------------------------------------*/
FieldContext::~FieldContext()
{
}
/*-- 29.01.2007 11:48:45---------------------------------------------------

  -----------------------------------------------------------------------*/
void FieldContext::AppendCommand(const ::rtl::OUString& rPart)
{
    m_sCommand += rPart;
}
    
::std::vector<rtl::OUString> FieldContext::GetCommandParts() const
{
    ::std::vector<rtl::OUString> aResult;
    sal_Int32 nIndex = 0;
    bool bInString = false;
    OUString sPart;
    while (nIndex != -1)
    {
        OUString sToken = GetCommand().getToken(0, ' ', nIndex);
        bool bInStringNext = bInString;
        
        if (sToken.getLength() == 0)
            continue;
        
        if (sToken.getStr()[0] == '"')
        {
            bInStringNext = true;
            sToken = sToken.copy(1);
        }
        if (sToken.getStr()[sToken.getLength() - 1] == '"')
        {
            bInStringNext = false;
            sToken = sToken.copy(0, sToken.getLength() - 1);
        }

        if (bInString)
        {
            if (bInStringNext)
            {
                sPart += OUString(' ');
                sPart += sToken;
            }
            else 
            {
                sPart += sToken;
                aResult.push_back(sPart);
            }
        }
        else 
        {
            if (bInStringNext)
            {
                sPart = sToken;
            }
            else 
            {
                aResult.push_back(sToken);
            }
        }
        
        bInString = bInStringNext;
    }
    
    return aResult;
}
    
/*-- 29.01.2007 11:33:15---------------------------------------------------
//collect the pieces of the command
  -----------------------------------------------------------------------*/
void DomainMapper_Impl::AppendFieldCommand(::rtl::OUString& rPartOfCommand)
{
#ifdef DEBUG_DOMAINMAPPER
    dmapper_logger->startElement("appendFieldCommand");
    dmapper_logger->chars(rPartOfCommand);
    dmapper_logger->endElement("appendFieldCommand");
#endif
    
    FieldContextPtr pContext = m_aFieldStack.top();
    OSL_ENSURE( pContext.get(), "no field context available");
    if( pContext.get() )
    {
        pContext->AppendCommand( rPartOfCommand );
    }
}
/*-- 13.12.2007 11:45:43---------------------------------------------------

  -----------------------------------------------------------------------*/
typedef std::multimap < sal_Int32, ::rtl::OUString > TOCStyleMap;    

const FieldConversionMap_t & lcl_GetFieldConversion()
{
static FieldConversionMap_t aFieldConversionMap;
static bool bFilled = false;
if(!bFilled)
{
    static const FieldConversion aFields[] =
    {
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ADDRESSBLOCK")),  "",                         "", FIELD_ADDRESSBLOCK  },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ADVANCE")),       "",                         "", FIELD_ADVANCE       },
        {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ASK")),           "SetExpression",             "SetExpression", FIELD_ASK      },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AUTONUM")),       "SetExpression",            "SetExpression", FIELD_AUTONUM   },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AUTONUMLGL")),     "SetExpression",            "SetExpression", FIELD_AUTONUMLGL },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AUTONUMOUT")),     "SetExpression",            "SetExpression", FIELD_AUTONUMOUT },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AUTHOR")),        "Author",                   "", FIELD_AUTHOR       },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DATE")),          "DateTime",                 "", FIELD_DATE         },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("COMMENTS")),      "DocInfo.Description",      "", FIELD_COMMENTS     },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CREATEDATE")),    "DocInfo.CreateDateTime",   "", FIELD_CREATEDATE   },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DOCPROPERTY")),   "",                         "", FIELD_DOCPROPERTY },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DOCVARIABLE")),   "User",                     "", FIELD_DOCVARIABLE  },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("EDITTIME")),      "DocInfo.EditTime",         "", FIELD_EDITTIME     },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FILLIN")),        "Input",                    "", FIELD_FILLIN       },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FILENAME")),      "FileName",                 "", FIELD_FILENAME     },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FILESIZE")),      "",                         "", FIELD_FILESIZE     },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FORMULA")),     "",                           "", FIELD_FORMULA },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FORMCHECKBOX")),     "",                           "", FIELD_FORMCHECKBOX},
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FORMDROPDOWN")),     "",                           "", FIELD_FORMDROWDOWN},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FORMTEXT")),     "Input", "", FIELD_FORMTEXT},
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("GOTOBUTTON")),    "",                         "", FIELD_GOTOBUTTON   },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("HYPERLINK")),     "",                         "", FIELD_HYPERLINK    },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("IF")),            "ConditionalText",          "", FIELD_IF           },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("INFO")),      "","", FIELD_INFO         },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("INCLUDEPICTURE")), "",                        "", FIELD_INCLUDEPICTURE},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("KEYWORDS")),      "DocInfo.KeyWords",         "", FIELD_KEYWORDS     },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("LASTSAVEDBY")),   "DocInfo.ChangeAuthor",                         "", FIELD_LASTSAVEDBY  },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MACROBUTTON")),   "Macro",                         "", FIELD_MACROBUTTON  },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MERGEFIELD")),    "Database",                 "Database", FIELD_MERGEFIELD},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MERGEREC")),      "DatabaseNumberOfSet",      "", FIELD_MERGEREC     },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MERGESEQ")),      "",                         "", FIELD_MERGESEQ     },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("NEXT")),          "DatabaseNextSet",          "", FIELD_NEXT         },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("NEXTIF")),        "DatabaseNextSet",          "", FIELD_NEXTIF       },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PAGE")),          "PageNumber",               "", FIELD_PAGE         },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("REF")),           "GetReference",             "", FIELD_REF          },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("REVNUM")),        "DocInfo.Revision",         "", FIELD_REVNUM       },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SAVEDATE")),      "DocInfo.Change",           "", FIELD_SAVEDATE     },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SECTION")),       "",                         "", FIELD_SECTION      },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SECTIONPAGES")),  "",                         "", FIELD_SECTIONPAGES },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SEQ")),           "SetExpression",            "SetExpression", FIELD_SEQ          },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SET")),           "","", FIELD_SET          },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SKIPIF")),"",                                 "", FIELD_SKIPIF       },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("STYLEREF")),"",                               "", FIELD_STYLEREF     },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SUBJECT")),       "DocInfo.Subject",          "", FIELD_SUBJECT      },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SYMBOL")),"",                                 "", FIELD_SYMBOL       },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TEMPLATE")),      "TemplateName",             "", FIELD_TEMPLATE},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TIME")),          "DateTime",                 "", FIELD_TIME         },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TITLE")),         "DocInfo.Title",            "", FIELD_TITLE        },
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("USERINITIALS")),  "ExtendedUser",              "", FIELD_USERINITIALS},
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("USERADDRESS")),   "",                         "", FIELD_USERADDRESS  },
//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("USERNAME")),      "ExtendedUser",             "", FIELD_USERNAME     }
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TOC")), "com.sun.star.text.ContentIndex", "", FIELD_TOC},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TC")), "com.sun.star.text.ContentIndexMark", "", FIELD_TC},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("NUMCHARS")), "CharacterCount", "", FIELD_NUMCHARS},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("NUMWORDS")), "WordCount", "", FIELD_NUMWORDS},
            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("NUMPAGES")), "PageCount", "", FIELD_NUMPAGES},

//            {::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("")), "", "", FIELD_},

        };
        size_t nConversions = sizeof(aFields)/sizeof(FieldConversion);
        for( size_t nConversion = 0; nConversion < nConversions; ++nConversion)
        {
            aFieldConversionMap.insert( FieldConversionMap_t::value_type(
                aFields[nConversion].sWordCommand,
                aFields[nConversion] ));
        }

        bFilled = true;
    }
    
    return aFieldConversionMap;
}        

void DomainMapper_Impl::handleFieldAsk
    (FieldContextPtr pContext,
     PropertyNameSupplier& rPropNameSupplier,
     uno::Reference< uno::XInterface > & xFieldInterface,
     uno::Reference< beans::XPropertySet > xFieldProperties)
{
    //doesn the command contain a variable name?
    ::rtl::OUString sVariable, sHint;

    sVariable = lcl_ExctractAskVariableAndHint( pContext->GetCommand(), 
        sHint );
    if(sVariable.getLength())
    {
        // determine field master name
        uno::Reference< beans::XPropertySet > xMaster = 
            FindOrCreateFieldMaster
            ("com.sun.star.text.FieldMaster.SetExpression", sVariable );

        // attach the master to the field
        uno::Reference< text::XDependentTextField > xDependentField
            ( xFieldInterface, uno::UNO_QUERY_THROW );
        xDependentField->attachTextFieldMaster( xMaster );

        // set input flag at the field
        xFieldProperties->setPropertyValue(
            rPropNameSupplier.GetName(PROP_IS_INPUT), uno::makeAny( true ));
        // set the prompt
        xFieldProperties->setPropertyValue(
            rPropNameSupplier.GetName(PROP_HINT),
            uno::makeAny( sHint ));
    }
    else
    {
        //don't insert the field
        //todo: maybe import a 'normal' input field here?
        xFieldInterface = 0;
    }
}

void DomainMapper_Impl::handleAutoNum
    (FieldContextPtr pContext,
    PropertyNameSupplier& rPropNameSupplier,
    uno::Reference< uno::XInterface > & xFieldInterface,
    uno::Reference< beans::XPropertySet > xFieldProperties)
{
    //create a sequence field master "AutoNr"
    uno::Reference< beans::XPropertySet > xMaster = 
    FindOrCreateFieldMaster
        ("com.sun.star.text.FieldMaster.SetExpression",
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AutoNr") ));

    xMaster->setPropertyValue( rPropNameSupplier.GetName(PROP_SUB_TYPE),
        uno::makeAny(text::SetVariableType::SEQUENCE));

    //apply the numbering type
    xFieldProperties->setPropertyValue(
        rPropNameSupplier.GetName(PROP_NUMBERING_TYPE),
        uno::makeAny( lcl_ParseNumberingType(pContext->GetCommand()) ));
        // attach the master to the field
    uno::Reference< text::XDependentTextField > xDependentField
        ( xFieldInterface, uno::UNO_QUERY_THROW );
    xDependentField->attachTextFieldMaster( xMaster );
}

void DomainMapper_Impl::handleAuthor
    (FieldContextPtr pContext,
    PropertyNameSupplier& rPropNameSupplier,
     uno::Reference< uno::XInterface > & /*xFieldInterface*/,
     uno::Reference< beans::XPropertySet > xFieldProperties)
{
    xFieldProperties->setPropertyValue
        ( rPropNameSupplier.GetName(PROP_FULL_NAME), uno::makeAny( true ));
    ::rtl::OUString sParam = 
        lcl_ExtractParameter(pContext->GetCommand(), sizeof(" AUTHOR") );
    if(sParam.getLength())
    {
        xFieldProperties->setPropertyValue(
                rPropNameSupplier.GetName( PROP_IS_FIXED ), 
                uno::makeAny( true ));
        //PROP_CURRENT_PRESENTATION is set later anyway
    }
}       

    void DomainMapper_Impl::handleDocProperty
        (FieldContextPtr pContext,
        PropertyNameSupplier& rPropNameSupplier,
        uno::Reference< uno::XInterface > & xFieldInterface,
        uno::Reference< beans::XPropertySet > xFieldProperties)
{
    //some docproperties should be imported as document statistic fields, some as DocInfo fields
    //others should be user fields
    ::rtl::OUString sParam = 
        lcl_ExtractParameter(pContext->GetCommand(), sizeof(" DOCPROPERTY") );
        
    if(sParam.getLength())
    {
        #define SET_ARABIC      0x01
        #define SET_FULL_NAME   0x02
        struct DocPropertyMap
        {
            const sal_Char* pDocPropertyName;
            const sal_Char* pServiceName;
            sal_uInt8       nFlags;
        };
        static const DocPropertyMap aDocProperties[] =
        {
            {"Author",           "Author",                  SET_FULL_NAME},
            {"CreateTime",       "DocInfo.CreateDateTime",  0},
            {"Characters",       "CharacterCount",          SET_ARABIC},
            {"Comments",         "DocInfo.Description",     0},
            {"Keywords",         "DocInfo.KeyWords",        0},
            {"LastPrinted",      "DocInfo.PrintDateTime",   0},
            {"LastSavedBy",      "DocInfo.ChangeAuthor",    0},
            {"LastSavedTime",    "DocInfo.ChangeDateTime",  0},
            {"Paragraphs",       "ParagraphCount",          SET_ARABIC},
            {"RevisionNumber",   "DocInfo.Revision",        0},
            {"Subject",          "DocInfo.Subject",         0},
            {"Template",         "TemplateName",            0},
            {"Title",            "DocInfo.Title",           0},
            {"TotalEditingTime", "DocInfo.EditTime",        9},
            {"Words",            "WordCount",               SET_ARABIC}

            //other available DocProperties:
            //Bytes, Category, CharactersWithSpaces, Company
            //HyperlinkBase,
            //Lines, Manager, NameofApplication, ODMADocId, Pages,
            //Security,
        };
        //search for a field mapping
        ::rtl::OUString sFieldServiceName;
        sal_uInt16 nMap = 0;
        for( ; nMap < sizeof(aDocProperties) / sizeof(DocPropertyMap); 
            ++nMap )
        {
            if(sParam.equalsAscii(aDocProperties[nMap].pDocPropertyName))
            {
                sFieldServiceName = 
                ::rtl::OUString::createFromAscii
                (aDocProperties[nMap].pServiceName);
                break;
            }
        }
        ::rtl::OUString sServiceName(RTL_CONSTASCII_USTRINGPARAM
            ("com.sun.star.text.TextField."));
        bool bIsCustomField = false;
        if(!sFieldServiceName.getLength())
        {
            //create a custom property field
            sServiceName += 
                ::rtl::OUString::createFromAscii("DocInfo.Custom");
            bIsCustomField = true;
        }
        else
        {
            sServiceName += sFieldServiceName;
        }
        xFieldInterface = m_xTextFactory->createInstance(sServiceName);
        xFieldProperties = 
            uno::Reference< beans::XPropertySet >( xFieldInterface, 
                uno::UNO_QUERY_THROW);
        if( bIsCustomField )
            xFieldProperties->setPropertyValue(
                rPropNameSupplier.GetName(PROP_NAME), uno::makeAny( sParam ));
        else
        {
            if(0 != (aDocProperties[nMap].nFlags & SET_ARABIC))
                xFieldProperties->setPropertyValue(
                    rPropNameSupplier.GetName(PROP_NUMBERING_TYPE),
                    uno::makeAny( style::NumberingType::ARABIC ));
            else if(0 != (aDocProperties[nMap].nFlags & SET_FULL_NAME))
                xFieldProperties->setPropertyValue(
                    rPropNameSupplier.GetName(PROP_FULL_NAME), 
                        uno::makeAny( true ));
        }
    }

#undef SET_ARABIC
#undef SET_FULL_NAME
}

void DomainMapper_Impl::handleToc
    (FieldContextPtr pContext,
    PropertyNameSupplier& rPropNameSupplier,
     uno::Reference< uno::XInterface > & /*xFieldInterface*/,
     uno::Reference< beans::XPropertySet > /*xFieldProperties*/,
    const ::rtl::OUString & sTOCServiceName)
{
    ::rtl::OUString sValue;
    bool bTableOfFigures = false;
    bool bHyperlinks = false;
    bool bFromOutline = false;
    bool bFromEntries = false;
    sal_Int16 nMaxLevel = 10;
    ::rtl::OUString sTemplate;
    ::rtl::OUString sChapterNoSeparator;
//                  \a Builds a table of figures but does not include the captions's label and number
    if( lcl_FindInCommand( pContext->GetCommand(), 'a', sValue ))
    { //make it a table of figures
        bTableOfFigures = true;
    }
//                  \b Uses a bookmark to specify area of document from which to build table of contents
//                    if( lcl_FindInCommand( pContext->GetCommand(), 'b', sValue ))
//                    { //todo: sValue contains the bookmark name - unsupported feature
//                    }
    if( lcl_FindInCommand( pContext->GetCommand(), 'c', sValue ))
//                  \c Builds a table of figures of the given label
    {
                        //todo: sValue contains the label's name
        bTableOfFigures = true;
    }
//                  \d Defines the separator between sequence and page numbers
    if( lcl_FindInCommand( pContext->GetCommand(), 'd', sValue ))
    {
                        //todo: insert the chapter number into each level and insert the separator additionally
        sChapterNoSeparator = sValue;
    }
//                  \f Builds a table of contents using TC entries instead of outline levels
    if( lcl_FindInCommand( pContext->GetCommand(), 'f', sValue ))
    {
                        //todo: sValue can contain a TOC entry identifier - use unclear
        bFromEntries = true;
    }
//                  \h Hyperlinks the entries and page numbers within the table of contents
    if( lcl_FindInCommand( pContext->GetCommand(), 'h', sValue ))
    {
                        //todo: make all entries to hyperlinks
        bHyperlinks = true;
    }
//                  \l Defines the TC entries field level used to build a table of contents
//                    if( lcl_FindInCommand( pContext->GetCommand(), 'l', sValue ))
//                    {
                            //todo: entries can only be included completely
//                    }
//                  \n Builds a table of contents or a range of entries, sucah as ?-9? in a table of contents without page numbers
//                    if( lcl_FindInCommand( pContext->GetCommand(), 'n', sValue ))
//                    {
                        //todo: what does the description mean?
//                    }
//                  \o  Builds a table of contents by using outline levels instead of TC entries
    if( lcl_FindInCommand( pContext->GetCommand(), 'o', sValue ))
    {
        bFromOutline = true;
        UniString sParam( sValue );
        if (!sParam.Len())
            nMaxLevel = WW_OUTLINE_MAX;
        else
        {
            xub_StrLen nIndex = 0;
            sParam.GetToken( 0, '-', nIndex );
            nMaxLevel = sal_Int16( sParam.Copy( nIndex ).ToInt32( ) );
        }
    }
//                  \p Defines the separator between the table entry and its page number
    if( lcl_FindInCommand( pContext->GetCommand(), 'p', sValue ))
        {  }
//                  \s  Builds a table of contents by using a sequence type
    if( lcl_FindInCommand( pContext->GetCommand(), 's', sValue ))
        {  }
//                  \t  Builds a table of contents by using style names other than the standard outline styles
    if( lcl_FindInCommand( pContext->GetCommand(), 't', sValue ))
    {
        sal_Int32 nPos = 0;
        ::rtl::OUString sToken = sValue.getToken( 1, '"', nPos);
        sTemplate = sToken.getLength() ? sToken : sValue;
    }
//                  \u  Builds a table of contents by using the applied paragraph outline level
    if( lcl_FindInCommand( pContext->GetCommand(), 'u', sValue ))
    {
        bFromOutline = true;
                        //todo: what doesn 'the applied paragraph outline level' refer to?
    }
//                  \w Preserve tab characters within table entries
//                    if( lcl_FindInCommand( pContext->GetCommand(), 'w', sValue ))
//                    {
                        //todo: not supported
//                    }
//                  \x Preserve newline characters within table entries
//                    if( lcl_FindInCommand( pContext->GetCommand(), 'x', sValue ))
//                    {
                        //todo: unsupported
//                    }
//                  \z Hides page numbers within the table of contens when shown in Web Layout View
//                    if( lcl_FindInCommand( pContext->GetCommand(), 'z', sValue ))
//                    { //todo: unsupported feature  }

                    //if there's no option then it should be created from outline
    if( !bFromOutline && !bFromEntries && !sTemplate.getLength()  )
        bFromOutline = true;

    uno::Reference< beans::XPropertySet > xTOC(
        m_xTextFactory->createInstance
        ( bTableOfFigures ?
              ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM
                ("com.sun.star.text.IllustrationsIndex"))
            : sTOCServiceName), 
         uno::UNO_QUERY_THROW);
    xTOC->setPropertyValue(rPropNameSupplier.GetName( PROP_TITLE ), uno::makeAny(::rtl::OUString()));
    if( !bTableOfFigures )
    {
        xTOC->setPropertyValue( rPropNameSupplier.GetName( PROP_LEVEL ), uno::makeAny( nMaxLevel ) );
        xTOC->setPropertyValue( rPropNameSupplier.GetName( PROP_CREATE_FROM_OUTLINE ), uno::makeAny( bFromOutline ));
        xTOC->setPropertyValue( rPropNameSupplier.GetName( PROP_CREATE_FROM_MARKS ), uno::makeAny( bFromEntries ));
        if( sTemplate.getLength() )
        {
                            //the string contains comma separated the names and related levels
                            //like: "Heading 1,1,Heading 2,2"
            TOCStyleMap aMap;
            sal_Int32 nLevel;
            sal_Int32 nPosition = 0;
            while( nPosition >= 0)
            {
                ::rtl::OUString sStyleName = sTemplate.getToken( 0, ',', nPosition );
                                //empty tokens should be skipped
                while( !sStyleName.getLength() && nPosition > 0 )
                    sStyleName = sTemplate.getToken( 0, ',', nPosition );
                nLevel = sTemplate.getToken( 0, ',', nPosition ).toInt32();
                if( !nLevel )
                    nLevel = 1;
                if( sStyleName.getLength() )
                    aMap.insert( TOCStyleMap::value_type(nLevel, sStyleName) );
            }
            uno::Reference< container::XIndexReplace> xParaStyles;
            xTOC->getPropertyValue(rPropNameSupplier.GetName(PROP_LEVEL_PARAGRAPH_STYLES)) >>= xParaStyles;
            for( nLevel = 1; nLevel < 10; ++nLevel)
            {
                sal_Int32 nLevelCount = aMap.count( nLevel );
                if( nLevelCount  )
                {
                    TOCStyleMap::iterator aTOCStyleIter = aMap.find( nLevel );

                    uno::Sequence< rtl::OUString> aStyles( nLevelCount );
                    for ( sal_Int32 nStyle = 0; nStyle < nLevelCount; ++nStyle, ++aTOCStyleIter )
                    {
                        aStyles[nStyle] = aTOCStyleIter->second;
                    }
                    xParaStyles->replaceByIndex(nLevel - 1, uno::makeAny(aStyles));
                }
            }
            xTOC->setPropertyValue(rPropNameSupplier.GetName(PROP_CREATE_FROM_LEVEL_PARAGRAPH_STYLES), uno::makeAny( true ));

        }
        if(bHyperlinks  || sChapterNoSeparator.getLength())
        {
            uno::Reference< container::XIndexReplace> xLevelFormats;
            xTOC->getPropertyValue(rPropNameSupplier.GetName(PROP_LEVEL_FORMAT)) >>= xLevelFormats;
            sal_Int32 nLevelCount = xLevelFormats->getCount();
                            //start with level 1, 0 is the header level
            for( sal_Int32 nLevel = 1; nLevel < nLevelCount; ++nLevel)
            {
                uno::Sequence< beans::PropertyValues > aLevel;
                xLevelFormats->getByIndex( nLevel ) >>= aLevel;
                                //create a copy of the level and add two new entries - hyperlink start and end
                bool bChapterNoSeparator  = sChapterNoSeparator.getLength() > 0;
                sal_Int32 nAdd = (bHyperlinks && bChapterNoSeparator) ? 4 : 2;
                uno::Sequence< beans::PropertyValues > aNewLevel( aLevel.getLength() + nAdd);
                beans::PropertyValues* pNewLevel = aNewLevel.getArray();
                if( bHyperlinks )
                {
                    beans::PropertyValues aHyperlink(1);
                    aHyperlink[0].Name = rPropNameSupplier.GetName( PROP_TOKEN_TYPE );
                    aHyperlink[0].Value <<= rPropNameSupplier.GetName( PROP_TOKEN_HYPERLINK_START );
                    pNewLevel[0] = aHyperlink;
                    aHyperlink[0].Value <<= rPropNameSupplier.GetName( PROP_TOKEN_HYPERLINK_END );
                    pNewLevel[aNewLevel.getLength() -1] = aHyperlink;
                }
                if( bChapterNoSeparator )
                {
                    beans::PropertyValues aChapterNo(2);
                    aChapterNo[0].Name = rPropNameSupplier.GetName( PROP_TOKEN_TYPE );
                    aChapterNo[0].Value <<= rPropNameSupplier.GetName( PROP_TOKEN_CHAPTER_INFO );
                    aChapterNo[1].Name = rPropNameSupplier.GetName( PROP_CHAPTER_FORMAT );
                                    //todo: is ChapterFormat::Number correct?
                    aChapterNo[1].Value <<= (sal_Int16)text::ChapterFormat::NUMBER;
                    pNewLevel[aNewLevel.getLength() - (bHyperlinks ? 4 : 2) ] = aChapterNo;

                    beans::PropertyValues aChapterSeparator(2);
                    aChapterSeparator[0].Name = rPropNameSupplier.GetName( PROP_TOKEN_TYPE );
                    aChapterSeparator[0].Value <<= rPropNameSupplier.GetName( PROP_TOKEN_TEXT );
                    aChapterSeparator[1].Name = rPropNameSupplier.GetName( PROP_TEXT );
                    aChapterSeparator[1].Value <<= sChapterNoSeparator;
                    pNewLevel[aNewLevel.getLength() - (bHyperlinks ? 3 : 1)] = aChapterSeparator;
                }
                                //copy the 'old' entries except the last (page no)
                for( sal_Int32 nToken = 0; nToken < aLevel.getLength() - 1; ++nToken)
                {
                    pNewLevel[nToken + 1] = aLevel[nToken];
                }
                                //copy page no entry (last or last but one depending on bHyperlinks
                sal_Int32 nPageNo = aNewLevel.getLength() - (bHyperlinks ? 2 : 3);
                pNewLevel[nPageNo] = aLevel[aLevel.getLength() - 1];

                xLevelFormats->replaceByIndex( nLevel, uno::makeAny( aNewLevel ) );
            }
        }
    }
    pContext->SetTOC( xTOC );
}

void DomainMapper_Impl::AddAnnotationPosition(
    const bool bStart,
    const sal_Int32 nAnnotationId )
{
    if (m_aTextAppendStack.empty())
        return;

    // Create a cursor, pointing to the current position.
    uno::Reference<text::XTextAppend>  xTextAppend = m_aTextAppendStack.top().xTextAppend;
    uno::Reference<text::XTextRange> xCurrent;
    if (xTextAppend.is())
    {
        uno::Reference<text::XTextCursor> xCursor = xTextAppend->createTextCursorByRange(xTextAppend->getEnd());
        xCurrent = xCursor->getStart();
    }

    // And save it, to be used by PopAnnotation() later.
    AnnotationPosition& aAnnotationPosition = m_aAnnotationPositions[ nAnnotationId ];
    if ( bStart )
    {
        aAnnotationPosition.m_xStart = xCurrent;
    }
    else
    {
        aAnnotationPosition.m_xEnd = xCurrent;
    }
    m_aAnnotationPositions[ nAnnotationId ] = aAnnotationPosition;
}

/*-- 29.01.2007 11:33:16---------------------------------------------------
//the field command has to be closed (0x14 appeared)
  -----------------------------------------------------------------------*/
void DomainMapper_Impl::CloseFieldCommand()
{
#ifdef DEBUG_DOMAINMAPPER
    dmapper_logger->element("closeFieldCommand");
#endif
    
    FieldContextPtr pContext = m_aFieldStack.top();
    OSL_ENSURE( pContext.get(), "no field context available");
    if( pContext.get() )
    {
        m_bSetUserFieldContent = false;
        FieldConversionMap_t aFieldConversionMap = lcl_GetFieldConversion();
        
        try
        {
            uno::Reference< uno::XInterface > xFieldInterface;
            //at first determine the field type - erase leading and trailing whitespaces
            ::rtl::OUString sCommand( pContext->GetCommand().trim() );
            sal_Int32 nSpaceIndex = sCommand.indexOf( ' ' );
            if( 0 <= nSpaceIndex )
                sCommand = sCommand.copy( 0, nSpaceIndex );

            FieldConversionMap_t::iterator aIt = aFieldConversionMap.find(sCommand);
            if(aIt != aFieldConversionMap.end())
            {
                uno::Reference< beans::XPropertySet > xFieldProperties;
                bool bCreateField = true;
                switch (aIt->second.eFieldId)
                {
                case FIELD_HYPERLINK:
                case FIELD_DOCPROPERTY:
                case FIELD_TOC:
                case FIELD_TC:
                case FIELD_FORMCHECKBOX:
                    bCreateField = false;
                    break;
                default:
                    break;
                }
                if( bCreateField)
                {
                    //add the service prefix
                    OUString sServiceName(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextField."));
                    sServiceName += ::rtl::OUString::createFromAscii(aIt->second.cFieldServiceName );

#ifdef DEBUG_DOMAINMAPPER
                    dmapper_logger->startElement("fieldService");
                    dmapper_logger->chars(sServiceName);
                    dmapper_logger->endElement("fieldService");
#endif
                    
                    xFieldInterface = m_xTextFactory->createInstance(sServiceName);
                    xFieldProperties = uno::Reference< beans::XPropertySet >( xFieldInterface, uno::UNO_QUERY_THROW);
                }
                PropertyNameSupplier& rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier();
                switch( aIt->second.eFieldId )
                {
                    case FIELD_ADDRESSBLOCK: break;
                    case FIELD_ADVANCE     : break;
                    case FIELD_ASK         :
                        handleFieldAsk(pContext, rPropNameSupplier, xFieldInterface, xFieldProperties);
                    break;
                    case FIELD_AUTONUM    :
                    case FIELD_AUTONUMLGL :
                    case FIELD_AUTONUMOUT :
                        handleAutoNum(pContext, rPropNameSupplier, xFieldInterface, xFieldProperties);
                    break;
                    case FIELD_AUTHOR       :
                        handleAuthor(pContext, rPropNameSupplier, xFieldInterface, xFieldProperties);
                    break;
                    case FIELD_DATE:
                    {
                        //not fixed,
                        xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_IS_FIXED),
                            uno::makeAny( false ));
                        xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_IS_DATE),
                            uno::makeAny( true ));
                        SetNumberFormat( pContext->GetCommand(), xFieldProperties );
                    }
                    break;
                    case FIELD_COMMENTS     :
                    {
                        ::rtl::OUString sParam = lcl_ExtractParameter(pContext->GetCommand(), sizeof(" COMMENTS") );
                        if(sParam.getLength())
                        {
                            xFieldProperties->setPropertyValue(
                                    rPropNameSupplier.GetName( PROP_IS_FIXED ), uno::makeAny( true ));
                            //PROP_CURRENT_PRESENTATION is set later anyway
                        }
                    }
                    break;
                    case FIELD_CREATEDATE  :
                    {
                        SetNumberFormat( pContext->GetCommand(), xFieldProperties );
                    }
                    break;
                    case FIELD_DOCPROPERTY :
                        handleDocProperty(pContext, rPropNameSupplier, xFieldInterface, xFieldProperties);
                    break;
                    case FIELD_DOCVARIABLE  :
                    {
                        ::rtl::OUString sParam = lcl_ExtractParameter(pContext->GetCommand(), sizeof(" DOCVARIABLE") );
                        //create a user field and type
                        uno::Reference< beans::XPropertySet > xMaster =
                            FindOrCreateFieldMaster( "com.sun.star.text.FieldMaster.User", sParam );
                        uno::Reference< text::XDependentTextField > xDependentField( xFieldInterface, uno::UNO_QUERY_THROW );
                        xDependentField->attachTextFieldMaster( xMaster );
                        m_bSetUserFieldContent = true;
                    }
                    break;
                    case FIELD_EDITTIME     :
                        //it's a numbering type, no number format! SetNumberFormat( pContext->GetCommand(), xFieldProperties );
                    break;
                    case FIELD_FILLIN       :
                    {
                        sal_Int32 nIndex = 0;
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName(PROP_HINT), uno::makeAny( pContext->GetCommand().getToken( 1, '\"', nIndex)));
                    }
                    break;
                    case FIELD_FILENAME:
                    {
                        sal_Int32 nNumberingTypeIndex = pContext->GetCommand().indexOf( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\\p")));
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName(PROP_FILE_FORMAT),
                                uno::makeAny( nNumberingTypeIndex > 0 ? text::FilenameDisplayFormat::FULL : text::FilenameDisplayFormat::NAME_AND_EXT ));
                    }
                    break;
                    case FIELD_FILESIZE     : break;
                    case FIELD_FORMULA : break;
                    case FIELD_FORMCHECKBOX : 
                        {
                            FFDataHandler::Pointer_t 
                                pFFDataHandler(pContext->getFFDataHandler());
                            FormControlHelper::Pointer_t 
                                pFormControlHelper(new FormControlHelper
                                                   (FIELD_FORMCHECKBOX,
                                                    m_xTextDocument, pFFDataHandler));
                            pContext->setFormControlHelper(pFormControlHelper);
                        }
                        break;
                    case FIELD_FORMDROPDOWN : break;
                    case FIELD_FORMTEXT :
                    {
                        FFDataHandler::Pointer_t pFFDataHandler
                            (pContext->getFFDataHandler());

                        xFieldProperties->setPropertyValue
                            (rPropNameSupplier.GetName(PROP_HINT),
                            uno::makeAny(pFFDataHandler->getStatusText()));
                        xFieldProperties->setPropertyValue
                            (rPropNameSupplier.GetName(PROP_HELP),
                            uno::makeAny(pFFDataHandler->getHelpText()));
                        xFieldProperties->setPropertyValue
                            (rPropNameSupplier.GetName(PROP_CONTENT),
                            uno::makeAny(pFFDataHandler->getTextDefault()));
                    }
                    break;
                    case FIELD_GOTOBUTTON   : break;
                    case FIELD_HYPERLINK:
                    {
                        ::std::vector<rtl::OUString> aParts = pContext->GetCommandParts();
                        ::std::vector<rtl::OUString>::const_iterator aItEnd = aParts.end();
                        ::std::vector<rtl::OUString>::const_iterator aPartIt = aParts.begin();

                        OUString sURL;
                        
                        while (aPartIt != aItEnd) 
                        {
                            if (aPartIt->equalsAscii("\\l"))
                            {
                                aPartIt++;
                                
                                if (aPartIt == aItEnd)
                                    break;
                                
                                sURL = OUString('#');
                                sURL += *aPartIt;
                            }
                            else if (aPartIt->equalsAscii("\\m") ||
                                     aPartIt->equalsAscii("\\n"))
                            {
                            }
                            else if (aPartIt->equalsAscii("\\o") ||
                                     aPartIt->equalsAscii("\\t"))
                            {
                                aPartIt++;
                                
                                if (aPartIt == aItEnd)
                                    break;
                            }
                            else 
                            {
                                sURL = *aPartIt;
                            }

                            aPartIt++;
                        }
                        
                        if (sURL.getLength() > 0)
                        {
                            pContext->SetHyperlinkURL(sURL);
                        }
                    }
                    break;
                    case FIELD_IF           : break;
                    case FIELD_INFO         : break;
                    case FIELD_INCLUDEPICTURE: break;
                    case FIELD_KEYWORDS     :
                    {
                        ::rtl::OUString sParam = lcl_ExtractParameter(pContext->GetCommand(), sizeof(" KEYWORDS") );
                        if(sParam.getLength())
                        {
                            xFieldProperties->setPropertyValue(
                                    rPropNameSupplier.GetName( PROP_IS_FIXED ), uno::makeAny( true ));
                            //PROP_CURRENT_PRESENTATION is set later anyway
                        }
                    }
                    break;
                    case FIELD_LASTSAVEDBY  : break;
                    case FIELD_MACROBUTTON:
                    {
                        //extract macro name
                        sal_Int32 nIndex = sizeof(" MACROBUTTON ");
                        ::rtl::OUString sMacro = pContext->GetCommand().getToken( 0, ' ', nIndex);
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName(PROP_MACRO_NAME), uno::makeAny( sMacro ));

                        //extract quick help text
                        if( pContext->GetCommand().getLength() > nIndex + 1)
                        {
                            xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName(PROP_HINT),
                                uno::makeAny( pContext->GetCommand().copy( nIndex )));
                        }
                    }
                    break;
                    case FIELD_MERGEFIELD  :
                    {
                        //todo: create a database field and fieldmaster pointing to a column, only
                        ::rtl::OUString sParam = lcl_ExtractParameter(pContext->GetCommand(), sizeof(" MERGEFIELD") );
                        //create a user field and type
                        uno::Reference< beans::XPropertySet > xMaster =
                            FindOrCreateFieldMaster( "com.sun.star.text.FieldMaster.Database", sParam );

    //                    xFieldProperties->setPropertyValue(
    //                             ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FieldCode")),
    //                             uno::makeAny( pContext->GetCommand().copy( nIndex + 1 )));
                        uno::Reference< text::XDependentTextField > xDependentField( xFieldInterface, uno::UNO_QUERY_THROW );
                        xDependentField->attachTextFieldMaster( xMaster );
                        m_bSetUserFieldContent = true;
                    }
                    break;
                    case FIELD_MERGEREC     : break;
                    case FIELD_MERGESEQ     : break;
                    case FIELD_NEXT         : break;
                    case FIELD_NEXTIF       : break;
                    case FIELD_PAGE        :
                        xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_NUMBERING_TYPE),
                            uno::makeAny( lcl_ParseNumberingType(pContext->GetCommand()) ));
                        xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_SUB_TYPE),
                            uno::makeAny( text::PageNumberType_CURRENT ));

                    break;
                    case FIELD_REF:
                    {
                        ::rtl::OUString sBookmark = lcl_ExtractParameter(pContext->GetCommand(), sizeof(" REF") );
                        xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_REFERENCE_FIELD_SOURCE),
                            uno::makeAny( sal_Int16(text::ReferenceFieldSource::BOOKMARK)) );
                        xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_SOURCE_NAME),
                            uno::makeAny( sBookmark) );
                        sal_Int16 nFieldPart = text::ReferenceFieldPart::TEXT;
                        ::rtl::OUString sValue;
                        if( lcl_FindInCommand( pContext->GetCommand(), 'p', sValue ))
                        {
                            //above-below
                            nFieldPart = text::ReferenceFieldPart::UP_DOWN;
                        }
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName( PROP_REFERENCE_FIELD_PART ), uno::makeAny( nFieldPart ));
                    }
                    break;
                    case FIELD_REVNUM       : break;
                    case FIELD_SAVEDATE     :
                        SetNumberFormat( pContext->GetCommand(), xFieldProperties );
                    break;
                    case FIELD_SECTION      : break;
                    case FIELD_SECTIONPAGES : break;
                    case FIELD_SEQ          : 
					{
                        // command looks like: " SEQ Table \* ARABIC "
                        ::rtl::OUString sCmd(pContext->GetCommand());
                        // find the sequence name, e.g. "SEQ"
                        ::rtl::OUString sSeqName = lcl_FindQuotedText(sCmd, "SEQ ", '\\');
                        sSeqName = sSeqName.trim();

                        // create a sequence field master using the sequence name
                        uno::Reference< beans::XPropertySet > xMaster = FindOrCreateFieldMaster(
                                    "com.sun.star.text.FieldMaster.SetExpression",
                                    sSeqName);

                        xMaster->setPropertyValue( 
                            rPropNameSupplier.GetName(PROP_SUB_TYPE),
                            uno::makeAny(text::SetVariableType::SEQUENCE));

                        // apply the numbering type
                        xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_NUMBERING_TYPE),
                            uno::makeAny( lcl_ParseNumberingType(pContext->GetCommand()) ));
                        
                        // attach the master to the field
                        uno::Reference< text::XDependentTextField > xDependentField( xFieldInterface, uno::UNO_QUERY_THROW );
                        xDependentField->attachTextFieldMaster( xMaster );
                    }
                    break;
                    case FIELD_SET          : break;
                    case FIELD_SKIPIF       : break;
                    case FIELD_STYLEREF     : break;
                    case FIELD_SUBJECT      :
                    {
                        ::rtl::OUString sParam = lcl_ExtractParameter(pContext->GetCommand(), sizeof(" SUBJECT") );
                        if(sParam.getLength())
                        {
                            xFieldProperties->setPropertyValue(
                                    rPropNameSupplier.GetName( PROP_IS_FIXED ), uno::makeAny( true ));
                            //PROP_CURRENT_PRESENTATION is set later anyway
                        }
                    }
                    break;
                    case FIELD_SYMBOL       : break;
                    case FIELD_TEMPLATE: break;
                    case FIELD_TIME         :
                        SetNumberFormat( pContext->GetCommand(), xFieldProperties );
                    break;
                    case FIELD_TITLE        :
                    {
                        ::rtl::OUString sParam = lcl_ExtractParameter(pContext->GetCommand(), sizeof(" TITLE") );
                        if(sParam.getLength())
                        {
                            xFieldProperties->setPropertyValue(
                                    rPropNameSupplier.GetName( PROP_IS_FIXED ), uno::makeAny( true ));
                            //PROP_CURRENT_PRESENTATION is set later anyway
                        }
                    }
                    break;
                    case FIELD_USERINITIALS:
                    {
                        xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_USER_DATA_TYPE), uno::makeAny( text::UserDataPart::SHORTCUT ));
                        //todo: if initials are provided - set them as fixed content
                        ::rtl::OUString sParam = lcl_ExtractParameter(pContext->GetCommand(), sizeof(" USERINITIALS") );
                        if(sParam.getLength())
                        {
                            xFieldProperties->setPropertyValue(
                                    rPropNameSupplier.GetName( PROP_IS_FIXED ), uno::makeAny( true ));
                            //PROP_CURRENT_PRESENTATION is set later anyway
                        }
                    }
                    break;
                    case FIELD_USERADDRESS  : //todo: user address collects street, city ...
                    break;
                    case FIELD_USERNAME     : //todo: user name is firstname + lastname
                    break;
                    case FIELD_TOC:
                        handleToc(pContext, rPropNameSupplier, xFieldInterface, xFieldProperties, 
                                  ::rtl::OUString::createFromAscii(aIt->second.cFieldServiceName));
                    break;
                    case FIELD_TC :
                    {
                        uno::Reference< beans::XPropertySet > xTC(
                            m_xTextFactory->createInstance(
                                ::rtl::OUString::createFromAscii(aIt->second.cFieldServiceName)),
                                uno::UNO_QUERY_THROW);
                        ::rtl::OUString sTCText = lcl_ExtractParameter(pContext->GetCommand(), sizeof(" TC") );
                        if( sTCText.getLength())
                            xTC->setPropertyValue(rPropNameSupplier.GetName(PROP_ALTERNATIVE_TEXT),
                                uno::makeAny(sTCText));
                        ::rtl::OUString sValue;
                        // \f TC entry in doc with multiple tables
    //                    if( lcl_FindInCommand( pContext->GetCommand(), 'f', sValue ))
    //                    {
                            // todo: unsupported
    //                    }
                        if( lcl_FindInCommand( pContext->GetCommand(), 'l', sValue ))
                        // \l Outline Level
                        {
                            sal_Int32 nLevel = sValue.toInt32();
                            if( sValue.getLength() && nLevel >= 0 && nLevel <= 10 )
                                xTC->setPropertyValue(rPropNameSupplier.GetName(PROP_LEVEL), uno::makeAny( (sal_Int16)nLevel ));
                        }
    //                    if( lcl_FindInCommand( pContext->GetCommand(), 'n', sValue ))
    //                    \n Suppress page numbers
    //                    {
                            //todo: unsupported feature
    //                    }
                        pContext->SetTC( xTC );
                    }
                    break;
                    case  FIELD_NUMCHARS:
                    case  FIELD_NUMWORDS:
                    case  FIELD_NUMPAGES:
                        xFieldProperties->setPropertyValue(
                            rPropNameSupplier.GetName(PROP_NUMBERING_TYPE),
                            uno::makeAny( lcl_ParseNumberingType(pContext->GetCommand()) ));
                        break;

                }
            }
            //set the text field if there is any
            pContext->SetTextField( uno::Reference< text::XTextField >( xFieldInterface, uno::UNO_QUERY ) );
        }
        catch( uno::Exception& rEx)
        {
            (void)rEx;
            OSL_ENSURE( false, "Exception in CloseFieldCommand()" );
        }
        pContext->SetCommandCompleted();
    }
}
/*-- 29.01.2007 11:33:16---------------------------------------------------
//the _current_ fields require a string type result while TOCs accept richt results
  -----------------------------------------------------------------------*/
bool DomainMapper_Impl::IsFieldResultAsString()
{
    bool bRet = false;
    OSL_ENSURE( !m_aFieldStack.empty(), "field stack empty?");
    FieldContextPtr pContext = m_aFieldStack.top();
    OSL_ENSURE( pContext.get(), "no field context available");
    if( pContext.get() )
    {
        bRet = pContext->GetTextField().is();
    }
    return bRet;
}
/*-- 01.09.2006 11:48:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::SetFieldResult( ::rtl::OUString& rResult )
{
#ifdef DEBUG_DOMAINMAPPER
    dmapper_logger->startElement("setFieldResult");
    dmapper_logger->chars(rResult);
#endif
    
    FieldContextPtr pContext = m_aFieldStack.top();
    OSL_ENSURE( pContext.get(), "no field context available");
    if( pContext.get() )
    {
        uno::Reference<text::XTextField> xTextField = pContext->GetTextField();
        try
        {
            PropertyNameSupplier& rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier();
            OSL_ENSURE( xTextField.is()
            //||m_xTOC.is() ||m_xTC.is()
            //||m_sHyperlinkURL.getLength()
            , "DomainMapper_Impl::SetFieldResult: field not created" );
            if(xTextField.is())
            {
                try
                {
                    if( m_bSetUserFieldContent )
                    {
                        // user field content has to be set at the field master
                        uno::Reference< text::XDependentTextField > xDependentField( xTextField, uno::UNO_QUERY_THROW );
                        xDependentField->getTextFieldMaster()->setPropertyValue(
                                rPropNameSupplier.GetName(PROP_CONTENT),
                             uno::makeAny( rResult ));
                    }
                    else
                    {
                        uno::Reference< beans::XPropertySet > xFieldProperties( xTextField, uno::UNO_QUERY_THROW);
                        xFieldProperties->setPropertyValue(
                                rPropNameSupplier.GetName(PROP_CURRENT_PRESENTATION),
                             uno::makeAny( rResult ));
                    }
                }
                catch( const beans::UnknownPropertyException& e)
                {
                    (void) e;
                    //some fields don't have a CurrentPresentation (DateTime)
                }
            }
        }
        catch( uno::Exception& e)
        {
            (void) e;
        }
    }
}

void DomainMapper_Impl::SetFieldFFData(FFDataHandler::Pointer_t pFFDataHandler)
{
#ifdef DEBUG_DOMAINMAPPER
    dmapper_logger->startElement("setFieldFFData");
#endif
    
    FieldContextPtr pContext = m_aFieldStack.top();
    if (pContext.get())
    {
        pContext->setFFDataHandler(pFFDataHandler);
    }
    
#ifdef DEBUG_DOMAINMAPPER
    dmapper_logger->endElement("setFieldFFData");
#endif
}

/*-- 29.01.2007 11:33:17---------------------------------------------------
//the end of field is reached (0x15 appeared) - the command might still be open
  -----------------------------------------------------------------------*/
void DomainMapper_Impl::PopFieldContext()
{
#ifdef DEBUG_DOMAINMAPPER
    dmapper_logger->element("popFieldContext");
#endif
    
    FieldContextPtr pContext = m_aFieldStack.top();
    OSL_ENSURE( pContext.get(), "no field context available");
    if( pContext.get() )
    {
        if( !pContext->IsCommandCompleted() )
            CloseFieldCommand();

        //insert the field, TC or TOC
        uno::Reference< text::XTextAppend >  xTextAppend = m_aTextAppendStack.top().xTextAppend;
        if(xTextAppend.is())
        {
            try
            {
                uno::Reference< text::XTextCursor > xCrsr = xTextAppend->createTextCursorByRange(pContext->GetStartRange());
                //remove the dummy character
                xCrsr->goRight( 1, true );
                xCrsr->setString( ::rtl::OUString() );
                uno::Reference< text::XTextContent > xToInsert( pContext->GetTOC(), uno::UNO_QUERY );
                if( xToInsert.is() )
                {
                    xCrsr->gotoEnd( true );
                    xToInsert->attach( uno::Reference< text::XTextRange >( xCrsr, uno::UNO_QUERY_THROW ));
                }
                else
                {
                    xToInsert = uno::Reference< text::XTextContent >(pContext->GetTC(), uno::UNO_QUERY);
                    if( !xToInsert.is() )
                        xToInsert = uno::Reference< text::XTextContent >(pContext->GetTextField(), uno::UNO_QUERY);
                    if( xToInsert.is() )
                    {
                        uno::Reference< text::XTextAppendAndConvert > xTextAppendAndConvert( xTextAppend, uno::UNO_QUERY_THROW );
                        xTextAppendAndConvert->appendTextContent( xToInsert, uno::Sequence< beans::PropertyValue >() );
                    }
                    else 
                    {
                        FormControlHelper::Pointer_t pFormControlHelper(pContext->getFormControlHelper());
                        if (pFormControlHelper.get() != NULL)
                        {
                            uno::Reference<text::XTextRange> xTxtRange(xCrsr, uno::UNO_QUERY);
                            pFormControlHelper->insertControl(xTxtRange);
                        }                    
                        else if(pContext->GetHyperlinkURL().getLength())
                        {
                            PropertyNameSupplier& rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier();
                            xCrsr->gotoEnd( true );
                            
                            uno::Reference< beans::XPropertySet > xCrsrProperties( xCrsr, uno::UNO_QUERY_THROW );
                            xCrsrProperties->setPropertyValue(rPropNameSupplier.GetName(PROP_HYPER_LINK_U_R_L), uno::
                                                              makeAny(pContext->GetHyperlinkURL()));
                        }
                    }
                }
            }
            catch(const lang::IllegalArgumentException& )
            {
                OSL_ENSURE( false, "IllegalArgumentException in PopFieldContext()" );
            }
            catch(const uno::Exception& )
            {
                OSL_ENSURE( false, "exception in PopFieldContext()" );
            }
        }
        //
        //TOCs have to include all the imported content
        //...
    }
    //remove the field context
    m_aFieldStack.pop();
}
/*-- 11.06.2007 16:19:00---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::AddBookmark(
    const ::rtl::OUString& rBookmarkName,
    const sal_Int32 nId )
{
    uno::Reference< text::XTextAppend >  xTextAppend = m_aTextAppendStack.top().xTextAppend;
    BookmarkMap_t::iterator aBookmarkIter = m_aBookmarkMap.find( nId );
    //is the bookmark name already registered?
    try
    {
        if( aBookmarkIter != m_aBookmarkMap.end() )
        {
            static const rtl::OUString sBookmarkService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.Bookmark"));
            uno::Reference< text::XTextContent > xBookmark( m_xTextFactory->createInstance( sBookmarkService ), uno::UNO_QUERY_THROW );
            uno::Reference< text::XTextCursor > xCursor;
            uno::Reference< text::XText > xText = aBookmarkIter->second.m_xTextRange->getText();
            if( aBookmarkIter->second.m_bIsStartOfText )
                xCursor = xText->createTextCursorByRange( xText->getStart() );
            else
            {
                xCursor = xText->createTextCursorByRange( aBookmarkIter->second.m_xTextRange );
                xCursor->goRight( 1, false );
            }

            xCursor->gotoRange( xTextAppend->getEnd(), true );
            uno::Reference< container::XNamed > xBkmNamed( xBookmark, uno::UNO_QUERY_THROW );
            //todo: make sure the name is not used already!
            xBkmNamed->setName( aBookmarkIter->second.m_sBookmarkName );
            xTextAppend->insertTextContent( uno::Reference< text::XTextRange >( xCursor, uno::UNO_QUERY_THROW), xBookmark, !xCursor->isCollapsed() );
            m_aBookmarkMap.erase( aBookmarkIter );
        }
        else
        {
            //otherwise insert a text range as marker
            uno::Reference< text::XTextCursor > xCursor = xTextAppend->createTextCursorByRange( xTextAppend->getEnd() );
            bool bIsStart = !xCursor->goLeft(1, false);
            uno::Reference< text::XTextRange > xCurrent = xCursor->getStart();
            m_aBookmarkMap.insert(BookmarkMap_t::value_type( nId, BookmarkInsertPosition( bIsStart, rBookmarkName, xCurrent ) ));
        }
    }
    catch( const uno::Exception& e)
    {
        (void) e;
        //TODO: What happens to bookmarks where start and end are at different XText objects?
    }
}
/*-- 01.11.2006 14:57:44---------------------------------------------------

  -----------------------------------------------------------------------*/
GraphicImportPtr DomainMapper_Impl::GetGraphicImport(GraphicImportType eGraphicImportType)
{
    if(!m_pGraphicImport)
        m_pGraphicImport.reset( new GraphicImport( m_xComponentContext, m_xTextFactory, m_rDMapper, eGraphicImportType ) );
    return m_pGraphicImport;
}
/*-- 09.08.2007 10:19:45---------------------------------------------------
    reset graphic import if the last import resulted in a shape, not a graphic
  -----------------------------------------------------------------------*/
void DomainMapper_Impl::ResetGraphicImport()
{
    m_pGraphicImport.reset();
}
/*-- 01.11.2006 09:25:40---------------------------------------------------

  -----------------------------------------------------------------------*/
void  DomainMapper_Impl::ImportGraphic(writerfilter::Reference< Properties >::Pointer_t ref, GraphicImportType eGraphicImportType)
{
    GetGraphicImport(eGraphicImportType);
    if( eGraphicImportType != IMPORT_AS_DETECTED_INLINE && eGraphicImportType != IMPORT_AS_DETECTED_ANCHOR )
    {
        //create the graphic
        ref->resolve( *m_pGraphicImport );
    }

    //insert it into the document at the current cursor position

    uno::Reference<text::XTextContent> xTextContent
        (m_pGraphicImport->GetGraphicObject());

    //insert it into the document at the current cursor position
    OSL_ENSURE( xTextContent.is(), "DomainMapper_Impl::ImportGraphic");
    if( xTextContent.is())
        appendTextContent( xTextContent, uno::Sequence< beans::PropertyValue >() );

    m_pGraphicImport.reset();
}

/*-- 28.12.2006 14:00:47---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::SetLineNumbering( sal_Int32 nLnnMod, sal_Int32 nLnc, sal_Int32 ndxaLnn )
{
    if( !m_bLineNumberingSet )
    {
        const PropertyNameSupplier& rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier();

        try
        {
            uno::Reference< text::XLineNumberingProperties > xLineProperties( m_xTextDocument, uno::UNO_QUERY_THROW );
            uno::Reference< beans::XPropertySet > xProperties = xLineProperties->getLineNumberingProperties();
            uno::Any aTrue( uno::makeAny( true ));
            xProperties->setPropertyValue( rPropNameSupplier.GetName( PROP_IS_ON                  ), aTrue);
            xProperties->setPropertyValue( rPropNameSupplier.GetName( PROP_COUNT_EMPTY_LINES      ), aTrue );
            xProperties->setPropertyValue( rPropNameSupplier.GetName( PROP_COUNT_LINES_IN_FRAMES  ), uno::makeAny( false ) );
            xProperties->setPropertyValue( rPropNameSupplier.GetName( PROP_INTERVAL               ), uno::makeAny( static_cast< sal_Int16 >( nLnnMod )));
            xProperties->setPropertyValue( rPropNameSupplier.GetName( PROP_DISTANCE               ), uno::makeAny( ConversionHelper::convertTwipToMM100(ndxaLnn) ));
            xProperties->setPropertyValue( rPropNameSupplier.GetName( PROP_NUMBER_POSITION        ), uno::makeAny( style::LineNumberPosition::LEFT));
            xProperties->setPropertyValue( rPropNameSupplier.GetName( PROP_NUMBERING_TYPE         ), uno::makeAny( style::NumberingType::ARABIC));
            xProperties->setPropertyValue( rPropNameSupplier.GetName( PROP_RESTART_AT_EACH_PAGE   ), uno::makeAny( nLnc == 0 ));
        }
        catch( const uno::Exception& e)
        {
            (void) e;
        }

/*
        { SW_PROP_NAME(UNO_NAME_CHAR_STYLE_NAME
        { SW_PROP_NAME(UNO_NAME_COUNT_EMPTY_LINES
        { SW_PROP_NAME(UNO_NAME_COUNT_LINES_IN_FRAMES
        { SW_PROP_NAME(UNO_NAME_DISTANCE
        { SW_PROP_NAME(UNO_NAME_IS_ON
        { SW_PROP_NAME(UNO_NAME_INTERVAL
        { SW_PROP_NAME(UNO_NAME_SEPARATOR_TEXT
        { SW_PROP_NAME(UNO_NAME_NUMBER_POSITION
        { SW_PROP_NAME(UNO_NAME_NUMBERING_TYPE
        { SW_PROP_NAME(UNO_NAME_RESTART_AT_EACH_PAGE
        { SW_PROP_NAME(UNO_NAME_SEPARATOR_INTERVAL
*/
    }
    m_bLineNumberingSet = true;
}
/*-- 31.08.2007 13:50:49---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::SetPageMarginTwip( PageMarElement eElement, sal_Int32 nValue )
{
    nValue = ConversionHelper::convertTwipToMM100(nValue);
    switch(eElement)
    {
        case PAGE_MAR_TOP    : m_aPageMargins.top     = nValue; break;
        case PAGE_MAR_RIGHT  : m_aPageMargins.right   = nValue; break;
        case PAGE_MAR_BOTTOM : m_aPageMargins.bottom  = nValue; break;
        case PAGE_MAR_LEFT   : m_aPageMargins.left    = nValue; break;
        case PAGE_MAR_HEADER : m_aPageMargins.header  = nValue; break;
        case PAGE_MAR_FOOTER : m_aPageMargins.footer  = nValue; break;
        case PAGE_MAR_GUTTER : m_aPageMargins.gutter  = nValue; break;
    }
}

/*-- 31.08.2007 13:47:50---------------------------------------------------

  -----------------------------------------------------------------------*/
_PageMar::_PageMar()
{
    header = footer = top = bottom = ConversionHelper::convertTwipToMM100( sal_Int32(1440));
    right = left = ConversionHelper::convertTwipToMM100( sal_Int32(1800));
    gutter = 0;
}

/*-- 07.03.2008 12:07:27---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::RegisterFrameConversion(
        uno::Reference< text::XTextRange >      xFrameStartRange,
        uno::Reference< text::XTextRange >      xFrameEndRange,
        uno::Sequence< beans::PropertyValue >   aFrameProperties
        )
{
    OSL_ENSURE(
        !m_aFrameProperties.getLength() && !m_xFrameStartRange.is() && !m_xFrameEndRange.is(),
        "frame properties not removed");
    m_aFrameProperties = aFrameProperties;
    m_xFrameStartRange = xFrameStartRange;
    m_xFrameEndRange   = xFrameEndRange;
}
/*-- 07.03.2008 12:07:33---------------------------------------------------

  -----------------------------------------------------------------------*/
bool DomainMapper_Impl::ExecuteFrameConversion()
{
    bool bRet = false;
    if( m_xFrameStartRange.is() && m_xFrameEndRange.is() )
    {
        bRet = true;
        try
        {
            uno::Reference< text::XTextAppendAndConvert > xTextAppendAndConvert( GetTopTextAppend(), uno::UNO_QUERY_THROW );
            xTextAppendAndConvert->convertToTextFrame(
                m_xFrameStartRange,
                m_xFrameEndRange,
                m_aFrameProperties );
        }
        catch( const uno::Exception& rEx)
        {
            (void)rEx;
            bRet = false;
        }
        m_xFrameStartRange = 0;
        m_xFrameEndRange = 0;
        m_aFrameProperties.realloc( 0 );
    }
    return bRet;
}

void DomainMapper_Impl::AddNewRedline(  )
{
    RedlineParamsPtr pNew( new RedlineParams );
    pNew->m_nToken = ooxml::OOXML_mod;
    if ( !m_bIsParaChange ) 
    {
        m_aRedlines.push_back( pNew );
    }
    else
    {
        m_pParaRedline.swap( pNew );
    }
}

RedlineParamsPtr DomainMapper_Impl::GetTopRedline(  )
{
    RedlineParamsPtr pResult;
    if ( !m_bIsParaChange && m_aRedlines.size(  ) > 0 )
        pResult = m_aRedlines.back(  );
    else if ( m_bIsParaChange )
        pResult = m_pParaRedline;
    return pResult;
}

sal_Int32 DomainMapper_Impl::GetCurrentRedlineToken(  )
{
    sal_Int32 nToken = 0;
    RedlineParamsPtr pCurrent( GetTopRedline(  ) );
    if ( pCurrent.get(  ) )
        nToken = pCurrent->m_nToken;
    return nToken;
}

void DomainMapper_Impl::SetCurrentRedlineAuthor( rtl::OUString sAuthor )
{
    if (m_xAnnotationField.is())
    {
        m_xAnnotationField->setPropertyValue(
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Author")),
            uno::makeAny(sAuthor) );
    }
    else
    {
        RedlineParamsPtr pCurrent( GetTopRedline(  ) );
        if ( pCurrent.get(  ) )
            pCurrent->m_sAuthor = sAuthor;
    }
}

void DomainMapper_Impl::SetCurrentRedlineInitials( rtl::OUString sInitials )
{
    if (m_xAnnotationField.is())
    {
        m_xAnnotationField->setPropertyValue(
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Initials")),
            uno::makeAny(sInitials) );
    }
}

void DomainMapper_Impl::SetCurrentRedlineDate( rtl::OUString sDate )
{
    if (m_xAnnotationField.is())
    {
        m_xAnnotationField->setPropertyValue(
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DateTimeValue")),
            uno::makeAny( ConversionHelper::convertDateTime( sDate ) ) );
    }
    else
    {
        RedlineParamsPtr pCurrent( GetTopRedline(  ) );
        if ( pCurrent.get(  ) )
            pCurrent->m_sDate = sDate;
    }
}

void DomainMapper_Impl::SetCurrentRedlineId( sal_Int32 sId )
{
    if (m_xAnnotationField.is())
    {
        m_nAnnotationId = sId;
    }
    else
    {
        RedlineParamsPtr pCurrent( GetTopRedline(  ) );
        if ( pCurrent.get(  ) )
            pCurrent->m_nId = sId;
    }
}

void DomainMapper_Impl::SetCurrentRedlineToken( sal_Int32 nToken )
{
    RedlineParamsPtr pCurrent( GetTopRedline(  ) );
    if ( pCurrent.get(  ) )
        pCurrent->m_nToken = nToken;
}

/*-- 19.03.2008 11:35:38---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapper_Impl::RemoveCurrentRedline( )
{
    if ( m_aRedlines.size( ) > 0 )
    {
        m_aRedlines.pop_back( );
    }
}

void DomainMapper_Impl::ResetParaRedline( )
{
    if ( m_pParaRedline.get( ) )
    {
        RedlineParamsPtr pEmpty;
        m_pParaRedline.swap( pEmpty );
    }
}

/*-- 22.09.2009 10:26:19---------------------------------------------------
     
-----------------------------------------------------------------------*/
void DomainMapper_Impl::ApplySettingsTable()
{
    if( m_pSettingsTable )
    {
        try
        {
            uno::Reference< beans::XPropertySet > xTextDefaults(
                                                                m_xTextFactory->createInstance(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.Defaults"))), uno::UNO_QUERY_THROW );
            sal_Int32 nDefTab = m_pSettingsTable->GetDefaultTabStop();
            xTextDefaults->setPropertyValue( PropertyNameSupplier::GetPropertyNameSupplier().GetName( PROP_TAB_STOP_DISTANCE ), uno::makeAny(nDefTab) );
        }
        catch(const uno::Exception& e)
        {
            (void) e;
        }    
    }    
}

SectionPropertyMap * DomainMapper_Impl::GetSectionContext()
{
    SectionPropertyMap* pSectionContext = 0;
    //the section context is not available before the first call of startSectionGroup()
    if( !IsAnyTableImport() )
    {
        PropertyMapPtr pContext = GetTopContextOfType(CONTEXT_SECTION);
        OSL_ENSURE(pContext.get(), "Section context is not in the stack!");
        pSectionContext = dynamic_cast< SectionPropertyMap* >( pContext.get() );
    }
    
    return pSectionContext;
}

}}
