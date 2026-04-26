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

#ifndef OOX_TOKEN_NAMESPACES_HXX
#define OOX_TOKEN_NAMESPACES_HXX

#include <sal/types.h>

namespace oox {

// ============================================================================

const size_t NMSP_SHIFT = 16;
const sal_Int32 NMSP_xml = 1 << NMSP_SHIFT;
const sal_Int32 NMSP_schema = 2 << NMSP_SHIFT;
const sal_Int32 NMSP_packageContentTypes = 3 << NMSP_SHIFT;
const sal_Int32 NMSP_packageMetaCorePr = 4 << NMSP_SHIFT;
const sal_Int32 NMSP_packageRel = 5 << NMSP_SHIFT;
const sal_Int32 NMSP_officeCustomPr = 6 << NMSP_SHIFT;
const sal_Int32 NMSP_officeDocPropsVT = 7 << NMSP_SHIFT;
const sal_Int32 NMSP_officeExtPr = 8 << NMSP_SHIFT;
const sal_Int32 NMSP_officeMath = 9 << NMSP_SHIFT;
const sal_Int32 NMSP_officeRel = 10 << NMSP_SHIFT;
const sal_Int32 NMSP_officeRelTheme = 11 << NMSP_SHIFT;
const sal_Int32 NMSP_doc = 12 << NMSP_SHIFT;
const sal_Int32 NMSP_xls = 13 << NMSP_SHIFT;
const sal_Int32 NMSP_ppt = 14 << NMSP_SHIFT;
const sal_Int32 NMSP_dml = 15 << NMSP_SHIFT;
const sal_Int32 NMSP_dmlChart = 16 << NMSP_SHIFT;
const sal_Int32 NMSP_dmlChartDr = 17 << NMSP_SHIFT;
const sal_Int32 NMSP_dmlDiagram = 18 << NMSP_SHIFT;
const sal_Int32 NMSP_dmlPicture = 19 << NMSP_SHIFT;
const sal_Int32 NMSP_dmlSpreadDr = 20 << NMSP_SHIFT;
const sal_Int32 NMSP_dmlWordDr = 21 << NMSP_SHIFT;
const sal_Int32 NMSP_vml = 22 << NMSP_SHIFT;
const sal_Int32 NMSP_vmlExcel = 23 << NMSP_SHIFT;
const sal_Int32 NMSP_vmlOffice = 24 << NMSP_SHIFT;
const sal_Int32 NMSP_vmlPowerpoint = 25 << NMSP_SHIFT;
const sal_Int32 NMSP_vmlWord = 26 << NMSP_SHIFT;
const sal_Int32 NMSP_encryption = 27 << NMSP_SHIFT;
const sal_Int32 NMSP_keyEncryptorPassword = 28 << NMSP_SHIFT;
const sal_Int32 NMSP_ax = 29 << NMSP_SHIFT;
const sal_Int32 NMSP_dc = 30 << NMSP_SHIFT;
const sal_Int32 NMSP_dcTerms = 31 << NMSP_SHIFT;
const sal_Int32 NMSP_xm = 32 << NMSP_SHIFT;
const sal_Int32 NMSP_sprm = 33 << NMSP_SHIFT;
const sal_Int32 NMSP_markupCompat = 34 << NMSP_SHIFT;
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

// ============================================================================

const sal_Int32 TOKEN_MASK      = static_cast< sal_Int32 >( (1 << NMSP_SHIFT) - 1 );
const sal_Int32 NMSP_MASK       = static_cast< sal_Int32 >( SAL_MAX_INT16 & ~TOKEN_MASK );

/** Returns the raw token identifier without namespace of the passed token. */
inline sal_Int32 getBaseToken( sal_Int32 nToken ) { return nToken & TOKEN_MASK; }

/** Returns the namespace without token identifier of the passed token. */
inline sal_Int32 getNamespace( sal_Int32 nToken ) { return nToken & NMSP_MASK; }


// defines for tokens with specific namespaces
#define OOX_TOKEN( namespace, token ) (::oox::NMSP_##namespace | ::oox::XML_##token)

#define A_TOKEN( token )        OOX_TOKEN( dml, token )
#define AX_TOKEN( token )       OOX_TOKEN( ax, token )
#define C_TOKEN( token )        OOX_TOKEN( dmlChart, token )
#define CDR_TOKEN( token )      OOX_TOKEN( dmlChartDr, token )
#define DGM_TOKEN( token )      OOX_TOKEN( dmlDiagram, token )
#define ENCRYPTION_TOKEN( token) OOX_TOKEN( encryption, token )
#define KEY_ENCRYPTOR_PASSWORD_TOKEN( token ) OOX_TOKEN( keyEncryptorPassword, token )
#define O_TOKEN( token )        OOX_TOKEN( vmlOffice, token )
#define PC_TOKEN( token )       OOX_TOKEN( packageContentTypes, token )
#define PPT_TOKEN( token )      OOX_TOKEN( ppt, token )
#define PR_TOKEN( token )       OOX_TOKEN( packageRel, token )
#define R_TOKEN( token )        OOX_TOKEN( officeRel, token )
#define VML_TOKEN( token )      OOX_TOKEN( vml, token )
#define VMLX_TOKEN( token )     OOX_TOKEN( vmlExcel, token )
#define XDR_TOKEN( token )      OOX_TOKEN( dmlSpreadDr, token )
#define XLS_TOKEN( token )      OOX_TOKEN( xls, token )
#define XM_TOKEN( token )       OOX_TOKEN( xm, token )
#define XML_TOKEN( token )      OOX_TOKEN( xml, token )

// ============================================================================

} // namespace oox

#endif
