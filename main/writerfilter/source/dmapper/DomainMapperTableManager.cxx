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


#include <DomainMapperTableManager.hxx>
#include <resourcemodel/WW8ResourceModel.hxx>
#include <BorderHandler.hxx>
#include <CellColorHandler.hxx>
#include <CellMarginHandler.hxx>
#include <ConversionHelper.hxx>
#include <MeasureHandler.hxx>
#include <TDefTableHandler.hxx>
#include <com/sun/star/text/HoriOrientation.hpp>
#include <com/sun/star/text/SizeType.hpp>
#include <com/sun/star/text/TableColumnSeparator.hpp>
#include <com/sun/star/text/VertOrientation.hpp>
#include <ooxml/resourceids.hxx>
#include <doctok/sprmids.hxx>
#include <dmapperLoggers.hxx>

namespace writerfilter {
namespace dmapper {
    
using namespace ::com::sun::star;
using namespace ::std;
/*-- 23.04.2007 14:57:49---------------------------------------------------

  -----------------------------------------------------------------------*/
DomainMapperTableManager::DomainMapperTableManager(bool bOOXML) :
    m_nCellCounterForCurrentRow(),
    m_nGridSpanOfCurrentCell( 1 ),
    m_nCurrentCellBorderIndex(),
    m_nCurrentHeaderRepeatCount(),
    m_nTableWidthOfCurrentTable(),
    m_bOOXML( bOOXML ),
    m_pTablePropsHandler( new TablePropertiesHandler( bOOXML ) )
{
    m_pTablePropsHandler->SetTableManager( this );
    
#ifdef DEBUG_DMAPPER_TABLE_HANDLER
#ifdef DEBUG_TABLE
    setTagLogger(dmapper_logger);
#endif
#endif
}
/*-- 23.04.2007 14:57:49---------------------------------------------------

  -----------------------------------------------------------------------*/
DomainMapperTableManager::~DomainMapperTableManager()
{
    delete m_pTablePropsHandler;
  m_pTablePropsHandler = NULL;
}
/*-- 23.04.2007 15:25:37---------------------------------------------------

  -----------------------------------------------------------------------*/
bool DomainMapperTableManager::sprm(Sprm & rSprm)
{
#ifdef DEBUG_DMAPPER_TABLE_HANDLER
    dmapper_logger->startElement("tablemanager.sprm");
    string sSprm = rSprm.toString();
    dmapper_logger->chars(sSprm);
    dmapper_logger->endElement("tablemanager.sprm");
#endif
    bool bRet = DomainMapperTableManager_Base_t::sprm(rSprm);

    if( !bRet )
    {
        bRet = m_pTablePropsHandler->sprm( rSprm );
    }

    if ( !bRet )
    {
        bRet = true;
        const sal_uInt32 nSprmId = rSprm.getId();
        const Value::Pointer_t pValue = rSprm.getValue();
        const sal_Int32 nIntValue = ((pValue.get() != NULL) ? pValue->getInt() : 0);
        switch ( nSprmId )
        {
            case 0xf661: //sprmTTRLeft left table indent
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */

            case 0xf614: // sprmTTPreferredWidth - preferred table width
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */

            case NS_ooxml::LN_CT_TblPrBase_tblW:  //90722;
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */

            case NS_ooxml::LN_CT_TblPrBase_tblInd: //90725
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */
            {
                //contains unit and value 
                writerfilter::Reference<Properties>::Pointer_t pProperties = rSprm.getProps();
                if( pProperties.get())
                {   //contains attributes x2902 (LN_unit) and x17e2 (LN_trleft)
                    MeasureHandlerPtr pMeasureHandler( new MeasureHandler );
                    pProperties->resolve(*pMeasureHandler);
                    TablePropertyMapPtr pPropMap( new TablePropertyMap );
                    if( nSprmId == 0xf661 || nSprmId == sal_uInt32(NS_ooxml::LN_CT_TblPrBase_tblInd ))
                    {
                        pPropMap->setValue( TablePropertyMap::LEFT_MARGIN, pMeasureHandler->getMeasureValue() );
                    }
                    else
                    {
                        m_nTableWidthOfCurrentTable.top() = pMeasureHandler->getMeasureValue();
                        if( m_nTableWidthOfCurrentTable.top() > 0 )
                            pPropMap->setValue( TablePropertyMap::TABLE_WIDTH, m_nTableWidthOfCurrentTable.top() );
                    }
#ifdef DEBUG_DMAPPER_TABLE_HANDLER
                    dmapper_logger->addTag(pPropMap->toTag());
#endif
                    insertTableProps(pPropMap);
                }
            }
            break;

            case 0x3404:// sprmTTableHeader
            case NS_ooxml::LN_CT_TrPrBase_tblHeader:
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */
                // if nIntValue == 1 then the row is a repeated header line
                // to prevent later rows from increasing the repeating m_nHeaderRepeat is set to NULL when repeating stops
                if( nIntValue > 0 && m_nCurrentHeaderRepeatCount.top() >= 0 )
                {
                    ++(m_nCurrentHeaderRepeatCount.top());
                    TablePropertyMapPtr pPropMap( new TablePropertyMap );
                    pPropMap->Insert( PROP_HEADER_ROW_COUNT, false, uno::makeAny( m_nCurrentHeaderRepeatCount.top() ));
                    insertTableProps(pPropMap);
                }
                else
                    m_nCurrentHeaderRepeatCount.top() = -1;
            break;  

            case 0xd608: // TDefTable
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */
            {    
                writerfilter::Reference<Properties>::Pointer_t pProperties = rSprm.getProps();
                if( pProperties.get())
                {
                    TDefTableHandlerPtr pTDefTableHandler( new TDefTableHandler(m_bOOXML) );
                    pProperties->resolve( *pTDefTableHandler );
                    
                    TablePropertyMapPtr pRowPropMap( new TablePropertyMap );
                    pRowPropMap->insert( pTDefTableHandler->getRowProperties() );
                    insertRowProps( pRowPropMap );
                    if( m_nTableWidthOfCurrentTable.top() == 0 )
                    {
                        m_nTableWidthOfCurrentTable.top() = pTDefTableHandler->getTableWidth();
                        if( m_nTableWidthOfCurrentTable.top() > 0 )
                        {
                            TablePropertyMapPtr pPropMap( new TablePropertyMap );
                            pPropMap->setValue( TablePropertyMap::TABLE_WIDTH, m_nTableWidthOfCurrentTable.top() );
                            insertTableProps(pPropMap);
                        }
                    }
                    for( size_t nCell = 0; nCell < pTDefTableHandler->getCellCount(); ++nCell )
                    {
                        TablePropertyMapPtr pCellPropMap( new TablePropertyMap );
                        pTDefTableHandler->fillCellProperties( nCell, pCellPropMap );
                        cellPropsByCell( nCell, pCellPropMap );
                    }
                }    
            }
            break;

            case 0xD605: // sprmTTableBorders
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */
            {
                writerfilter::Reference<Properties>::Pointer_t pProperties = rSprm.getProps();
                if( pProperties.get())
                {
                    BorderHandlerPtr pBorderHandler( new BorderHandler(m_bOOXML) );
                    pProperties->resolve(*pBorderHandler);
                    TablePropertyMapPtr pCellPropMap( new TablePropertyMap() );
                    pCellPropMap->insert( pBorderHandler->getProperties() );
                    cellPropsByCell( m_nCurrentCellBorderIndex.top(), pCellPropMap );
                    ++(m_nCurrentCellBorderIndex.top());
                }
            }
            break;

            case 0xd632 : //sprmTNewSpacing
                /* WRITERFILTERSTATUS: done: 0, planned: 2, spent: 0 */

            case 0xd634 : //sprmTNewSpacing
                /* WRITERFILTERSTATUS: done: 0, planned: 2, spent: 0 */
                //TODO: sprms contain default (TNew) and actual border spacing of cells - not resolvable yet
            break;

            case 0xd613: //sprmTGridLineProps
                /* WRITERFILTERSTATUS: done: 0, planned: 2, spent: 0 */
                // TODO: needs a handler 
                /*contains: 
                 GridLineProps">
                    rtf:LINEPROPSTOP
                    rtf:LINEPROPSLEFT
                    rtf:LINEPROPSBOTTOM
                    rtf:LINEPROPSRIGHT
                    rtf:LINEPROPSHORIZONTAL
                    rtf:LINEPROPSVERTICAL
                        rtf:LINECOLOR
                        rtf:LINEWIDTH
                        rtf:LINETYPE
                
                */
            break;

            case 0x740a : //sprmTTlp
                /* WRITERFILTERSTATUS: done: 0, planned: 2, spent: 0 */
                //TODO: Table look specifier
            break;

            case 0x6816 : //unknown
            case 0x3466 : //unknown
            case 0x3615 : //unknown
            case 0x646b : //unknown - expandable sprm - see ww8scan.cxx
            case 0x7479 : //unknown
            case 0xf617 : //unknown
            case 0xf618 : //unknown
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */
                bRet = false;
            break;

            case NS_ooxml::LN_CT_TblPrBase_tblStyle: //table style name
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */
            {    
                TablePropertyMapPtr pPropMap( new TablePropertyMap );
                pPropMap->Insert( META_PROP_TABLE_STYLE_NAME, false, uno::makeAny( pValue->getString() ));
                insertTableProps(pPropMap);
            }
            break;

            case NS_ooxml::LN_CT_TblGridBase_gridCol:
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */
            {   
                getCurrentGrid()->push_back( ConversionHelper::convertTwipToMM100( nIntValue ) );
            }    
            break;

            case NS_ooxml::LN_CT_TcPrBase_vMerge : //vertical merge
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */
            {    
                // values can be: LN_Value_ST_Merge_restart, LN_Value_ST_Merge_continue, in reality the second one is a 0
                TablePropertyMapPtr pMergeProps( new TablePropertyMap );
                pMergeProps->Insert( PROP_VERTICAL_MERGE, false, uno::makeAny( bool( sal::static_int_cast<Id>(nIntValue) == NS_ooxml::LN_Value_ST_Merge_restart )) );
                cellProps( pMergeProps);
            }
            break;

            case NS_ooxml::LN_CT_TcPrBase_gridSpan: //number of grid positions spanned by this cell
                /* WRITERFILTERSTATUS: done: 100, planned: 2, spent: 0 */
            {    
#if DEBUG_DMAPPER_TABLE_HANDLER
                dmapper_logger->startElement("tablemanager.GridSpan");
                dmapper_logger->attribute("gridSpan", nIntValue);
                dmapper_logger->endElement("tablemanager.GridSpan");
#endif
                m_nGridSpanOfCurrentCell = nIntValue;
            }
            break;
            case NS_ooxml::LN_CT_TblPrBase_tblLook: 
                /* WRITERFILTERSTATUS: done: 0, planned: 2, spent: 0 */
                break; //todo: table look specifier

            case NS_ooxml::LN_CT_TcPrBase_tcW: 
                /* WRITERFILTERSTATUS: done: 100, planned: 0.5, spent: 0 */
                break; //fixed column width is not supported

            case NS_ooxml::LN_CT_TrPrBase_cnfStyle:
                /* WRITERFILTERSTATUS: done: 100, planned: 0.5, spent: 0 */
                {
                    TablePropertyMapPtr pProps( new TablePropertyMap );
                    pProps->Insert( PROP_CNF_STYLE, true, uno::makeAny( pValue->getString( ) ) );
                    insertRowProps( pProps );
                }
                break;

            case NS_ooxml::LN_CT_PPrBase_cnfStyle:
                /* WRITERFILTERSTATUS: done: 0, planned: 0.5, spent: 0 */
                // TODO cnfStyle on a paragraph
                break;

            case NS_ooxml::LN_CT_TcPrBase_cnfStyle:
                /* WRITERFILTERSTATUS: done: 100, planned: 0.5, spent: 0 */
                {
                    TablePropertyMapPtr pProps( new TablePropertyMap );
                    pProps->Insert( PROP_CNF_STYLE, true, uno::makeAny( pValue->getString( ) ) );
                    cellProps( pProps );
                }
                break;

            default:
                bRet = false;
                
#ifdef DEBUG_DOMAINMAPPER
                dmapper_logger->element("TableManager.unhandled");
#endif
        }
    }
    return bRet;
}


void DomainMapperTableManager::pushStackOfMembers()
{
    m_nCellCounterForCurrentRow.push( 0 );
    m_nCurrentCellBorderIndex.push( 0 );
    m_nCurrentHeaderRepeatCount.push( 0 );
    m_nTableWidthOfCurrentTable.push( 0 );

    IntVectorPtr pNewGrid( new vector<sal_Int32> );
    IntVectorPtr pNewSpans( new vector<sal_Int32> );
    m_aTableGrid.push( pNewGrid );
    m_aGridSpans.push( pNewSpans );
}


void DomainMapperTableManager::popStackOfMembers()
{
    if (!m_nCellCounterForCurrentRow.empty()) {
        m_nCellCounterForCurrentRow.pop();
    }
    if (!m_nCurrentCellBorderIndex.empty()) {
        m_nCurrentCellBorderIndex.pop();
    }
    if (!m_nCurrentHeaderRepeatCount.empty()) {
        m_nCurrentHeaderRepeatCount.pop();
    }
    if (!m_nTableWidthOfCurrentTable.empty()) {
        m_nTableWidthOfCurrentTable.pop();
    }

    if (!m_aTableGrid.empty()) {
        m_aTableGrid.pop();
    }
    if (!m_aGridSpans.empty()) {
        m_aGridSpans.pop();
    }
}


boost::shared_ptr< vector<sal_Int32> > DomainMapperTableManager::getCurrentGrid()
{
    return m_aTableGrid.top();
}

boost::shared_ptr< vector< sal_Int32 > > DomainMapperTableManager::getCurrentSpans()
{
    return m_aGridSpans.top();
}


void DomainMapperTableManager::SetStyleProperties(
    PropertyMapPtr pProperties )
{
    m_pStyleProps = pProperties;
    if ( m_pStyleProps.get() )
    {
        pushStackOfMembers();
    }
    else
    {
        popStackOfMembers();
    }
};


void DomainMapperTableManager::startLevel()
{
    DomainMapperTableManager_Base_t::startLevel();

    pushStackOfMembers();
}

void DomainMapperTableManager::endLevel()
{
    popStackOfMembers();

    DomainMapperTableManager_Base_t::endLevel();

#ifdef DEBUG_DMAPPER_TABLE_HANDLER
    dmapper_logger->startElement("dmappertablemanager.endLevel");
    PropertyMapPtr pProps = getTableProps();
    if (pProps.get() != NULL)
        dmapper_logger->addTag(getTableProps()->toTag());
        
    dmapper_logger->endElement("dmappertablemanager.endLevel");
#endif    
}

/*-- 02.05.2007 14:36:26---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapperTableManager::endOfCellAction()
{
#ifdef DEBUG_DMAPPER_TABLE_HANDLER
    dmapper_logger->element("endOFCellAction");
#endif

    getCurrentSpans()->push_back( m_nGridSpanOfCurrentCell );
    m_nGridSpanOfCurrentCell = 1;
    ++(m_nCellCounterForCurrentRow.top());
}
/*-- 02.05.2007 14:36:26---------------------------------------------------

  -----------------------------------------------------------------------*/
void DomainMapperTableManager::endOfRowAction()
{
#ifdef DEBUG_DMAPPER_TABLE_HANDLER
    dmapper_logger->startElement("endOfRowAction");
#endif
    
    IntVectorPtr pTableGrid = getCurrentGrid( );
    if( m_nTableWidthOfCurrentTable.top() == 0
        && pTableGrid->size() > 0 )
    {
        ::std::vector<sal_Int32>::const_iterator aCellIter = pTableGrid->begin();

#ifdef DEBUG_DMAPPER_TABLE_HANDLER
        dmapper_logger->startElement("tableWidth");
#endif

        while( aCellIter != pTableGrid->end() )
        {
#ifdef DEBUG_DMAPPER_TABLE_HANDLER
            dmapper_logger->startElement("col");
            dmapper_logger->attribute("width", *aCellIter);
            dmapper_logger->endElement("col");
#endif

             m_nTableWidthOfCurrentTable.top() += *aCellIter++;
        }

        if( m_nTableWidthOfCurrentTable.top() > 0)
        {
            TablePropertyMapPtr pPropMap( new TablePropertyMap );
            pPropMap->setValue( TablePropertyMap::TABLE_WIDTH, m_nTableWidthOfCurrentTable.top() );
            insertTableProps(pPropMap);
        }

#ifdef DEBUG_DMAPPER_TABLE_HANDLER
        dmapper_logger->endElement("tableWidth");
#endif
    }

    IntVectorPtr pCurrentSpans = getCurrentSpans( );
    if( pCurrentSpans->size() < m_nCellCounterForCurrentRow.top() )
    {
        //fill missing elements with '1'
        pCurrentSpans->insert( pCurrentSpans->end( ), m_nCellCounterForCurrentRow.top() - pCurrentSpans->size(), 1 );
    }    

#ifdef DEBUG_DMAPPER_TABLE_HANDLER
    dmapper_logger->startElement("gridSpans");
    {
        ::std::vector<sal_Int32>::const_iterator aGridSpanIter = pCurrentSpans->begin();
        ::std::vector<sal_Int32>::const_iterator aGridSpanIterEnd = pCurrentSpans->end();
        
        while (aGridSpanIter != aGridSpanIterEnd)
        {
            dmapper_logger->startElement("gridSpan");
            dmapper_logger->attribute("span", *aGridSpanIter);
            dmapper_logger->endElement("gridSpan");
        
            aGridSpanIter++;
        }
    }
    dmapper_logger->endElement("gridSpans");
#endif

    //calculate number of used grids - it has to match the size of m_aTableGrid
    size_t nGrids = 0;
    ::std::vector<sal_Int32>::const_iterator aGridSpanIter = pCurrentSpans->begin();
    for( ; aGridSpanIter != pCurrentSpans->end(); ++aGridSpanIter)
        nGrids += *aGridSpanIter;

    if( pTableGrid->size() == nGrids )
    {
        //determine table width 
        double nFullWidth = m_nTableWidthOfCurrentTable.top();
        //the positions have to be distibuted in a range of 10000 
        const double nFullWidthRelative = 10000.;
        uno::Sequence< text::TableColumnSeparator > aSeparators( m_nCellCounterForCurrentRow.top() - 1 );
        text::TableColumnSeparator* pSeparators = aSeparators.getArray();
        sal_Int16 nLastRelPos = 0;
        sal_uInt32 nBorderGridIndex = 0;

        ::std::vector< sal_Int32 >::const_iterator aSpansIter = pCurrentSpans->begin( );
        for( sal_uInt32 nBorder = 0; nBorder < m_nCellCounterForCurrentRow.top() - 1; ++nBorder )
        {
            sal_Int32 nGridCount = *aSpansIter;
            double fGridWidth = 0.;
            do
            {
                fGridWidth += (*pTableGrid.get())[nBorderGridIndex++];
            }while( --nGridCount );
            
            sal_Int16 nRelPos = 
                sal::static_int_cast< sal_Int16 >(fGridWidth * nFullWidthRelative / nFullWidth );

            pSeparators[nBorder].Position =  nRelPos + nLastRelPos;
            pSeparators[nBorder].IsVisible = sal_True;
            nLastRelPos = nLastRelPos + nRelPos;
            aSpansIter++;
        }
        TablePropertyMapPtr pPropMap( new TablePropertyMap );
        pPropMap->Insert( PROP_TABLE_COLUMN_SEPARATORS, false, uno::makeAny( aSeparators ) );

#ifdef DEBUG_DMAPPER_TABLE_HANDLER
        dmapper_logger->startElement("rowProperties");
        dmapper_logger->addTag(pPropMap->toTag());
        dmapper_logger->endElement("rowProperties");
#endif
        insertRowProps(pPropMap);
    }

    m_nCellCounterForCurrentRow.top() = 0;
    m_nCurrentCellBorderIndex.top() = 0;
    pCurrentSpans->clear();

#ifdef DEBUG_DMAPPER_TABLE_HANDLER
    dmapper_logger->endElement("endOfRowAction");
#endif
}


}}
