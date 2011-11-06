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



#ifndef _XMLOFF_XMLNMSPE_HXX
#define _XMLOFF_XMLNMSPE_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
namespace binfilter {

#define XML_NAMESPACE( prefix, key ) \
const sal_uInt16 XML_NAMESPACE_##prefix			= key; \
const sal_uInt16 XML_NAMESPACE_##prefix##_IDX	= key;

#define XML_OLD_NAMESPACE( prefix, index ) \
const sal_uInt16 XML_OLD_NAMESPACE_##prefix##_IDX = \
	(_XML_OLD_NAMESPACE_BASE+index);

// current namespaces
// These namespaces have the same index in the namespace table as prefix used.
// If a namespace is added, _XML_OLD_NAMESPACE_BASE has to be adjusted!
XML_NAMESPACE( OFFICE, 			0U )
XML_NAMESPACE( STYLE, 			1U )
XML_NAMESPACE( TEXT	, 			2U )
XML_NAMESPACE( TABLE, 			3U )
XML_NAMESPACE( DRAW	, 			4U )
XML_NAMESPACE( FO	, 			5U )
XML_NAMESPACE( XLINK, 			6U )
XML_NAMESPACE( DC	, 			7U )
XML_NAMESPACE( META	, 			8U )
XML_NAMESPACE( NUMBER, 			9U )
XML_NAMESPACE( PRESENTATION, 	10U )
XML_NAMESPACE( SVG, 			11U )
XML_NAMESPACE( CHART, 			12U )
XML_NAMESPACE( DR3D, 			13U )
XML_NAMESPACE( MATH, 			14U )
XML_NAMESPACE( FORM, 			15U )
XML_NAMESPACE( SCRIPT, 			16U )
XML_NAMESPACE( BLOCKLIST, 		17U )
XML_NAMESPACE( FRAMEWORK, 		18U )
XML_NAMESPACE( CONFIG,			19U )

#define _XML_OLD_NAMESPACE_BASE 20U

// namespaces used in the technical preview (SO 5.2)
XML_OLD_NAMESPACE( FO,		0U )
XML_OLD_NAMESPACE( XLINK,	1U )

XML_OLD_NAMESPACE( OFFICE,	2U )
XML_OLD_NAMESPACE( STYLE,	3U )
XML_OLD_NAMESPACE( TEXT,	4U )
XML_OLD_NAMESPACE( TABLE,	5U )
XML_OLD_NAMESPACE( META,	6U )

}//end of namespace binfilter
#endif	//  _XMLOFF_XMLNMSPE_HXX
