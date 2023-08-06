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



#ifndef OOX_XLS_SCENARIOBUFFER_HXX
#define OOX_XLS_SCENARIOBUFFER_HXX

#include <com/sun/star/table/CellAddress.hpp>
#include "oox/helper/refmap.hxx"
#include "oox/helper/refvector.hxx"
#include "oox/xls/workbookhelper.hxx"

namespace oox {
namespace xls {

// ============================================================================

struct ScenarioCellModel
{
    ::com::sun::star::table::CellAddress maPos;
    ::rtl::OUString     maValue;
    sal_Int32           mnNumFmtId;
    bool                mbDeleted;

    explicit            ScenarioCellModel();
};

// ----------------------------------------------------------------------------

struct ScenarioModel
{
    ::rtl::OUString     maName;             // Name of the scenario.
    ::rtl::OUString     maComment;          // Comment.
    ::rtl::OUString     maUser;             // Name of user created the scenario.
    bool                mbLocked;           // True = input cell values locked.
    bool                mbHidden;           // True = scenario is hidden.

    explicit            ScenarioModel();
};

// ----------------------------------------------------------------------------

class Scenario : public WorkbookHelper
{
public:
    explicit            Scenario( const WorkbookHelper& rHelper, sal_Int16 nSheet );

    /** Imports a scenario definition from a scenario element. */
    void                importScenario( const AttributeList& rAttribs );
    /** Imports a new cell for this scenario from a inputCells element. */
    void                importInputCells( const AttributeList& rAttribs );

    /** Imports a scenario definition from a SCENARIO record. */
    void                importScenario( SequenceInputStream& rStrm );
    /** Imports a new cell for this scenario from a INPUTCELLS record. */
    void                importInputCells( SequenceInputStream& rStrm );

    /** Imports a scenario definition from a SCENARIO record. */
    void                importScenario( BiffInputStream& rStrm );

    /** Creates the scenario in the Calc document. */
    void                finalizeImport();

private:
    typedef ::std::vector< ScenarioCellModel > ScenarioCellVector;

    ScenarioCellVector  maCells;            // Scenario cells.
    ScenarioModel       maModel;            // Scenario model data.
    sal_Int16           mnSheet;            // Index of the sheet this scenario is based on.
};

// ============================================================================

struct SheetScenariosModel
{
    sal_Int32           mnCurrent;          // Selected scenario.
    sal_Int32           mnShown;            // Visible scenario.

    explicit            SheetScenariosModel();
};

// ----------------------------------------------------------------------------

class SheetScenarios : public WorkbookHelper
{
public:
    explicit            SheetScenarios( const WorkbookHelper& rHelper, sal_Int16 nSheet );

    /** Imports sheet scenario settings from a scenarios element. */
    void                importScenarios( const AttributeList& rAttribs );
    /** Imports sheet scenario settings from a SCENARIOS record. */
    void                importScenarios( SequenceInputStream& rStrm );
    /** Imports sheet scenario settings from a SCENARIOS record. */
    void                importScenarios( BiffInputStream& rStrm );

    /** Creates and returns a new scenario in this collection. */
    Scenario&           createScenario();

    /** Creates all scenarios in the Calc sheet. */
    void                finalizeImport();

private:
    typedef RefVector< Scenario > ScenarioVector;
    ScenarioVector      maScenarios;
    SheetScenariosModel maModel;
    sal_Int16           mnSheet;
};

// ============================================================================

class ScenarioBuffer : public WorkbookHelper
{
public:
    explicit            ScenarioBuffer( const WorkbookHelper& rHelper );

    /** Creates and returns a scenario collection for the passed sheet. */
    SheetScenarios&     createSheetScenarios( sal_Int16 nSheet );

    /** Creates all scenarios in the Calc document. */
    void                finalizeImport();

private:
    typedef RefMap< sal_Int16, SheetScenarios, ::std::greater< sal_Int16 > > SheetScenariosMap;
    SheetScenariosMap   maSheetScenarios;
};

// ============================================================================

} // namespace xls
} // namespace oox

#endif

/* vim: set noet sw=4 ts=4: */
