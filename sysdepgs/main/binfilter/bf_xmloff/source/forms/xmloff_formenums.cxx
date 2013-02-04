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



#ifndef _XMLOFF_FORMENUMS_HXX_
#include "formenums.hxx"
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _COM_SUN_STAR_FORM_FORMSUBMITENCODING_HPP_
#include <com/sun/star/form/FormSubmitEncoding.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_FORMSUBMITMETHOD_HPP_
#include <com/sun/star/form/FormSubmitMethod.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_COMMANDTYPE_HPP_
#include <com/sun/star/sdb/CommandType.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_NAVIGATIONBARMODE_HPP_
#include <com/sun/star/form/NavigationBarMode.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_TABULATORCYCLE_HPP_
#include <com/sun/star/form/TabulatorCycle.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_FORMBUTTONTYPE_HPP_
#include <com/sun/star/form/FormButtonType.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_LISTSOURCETYPE_HPP_
#include <com/sun/star/form/ListSourceType.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_TEXTALIGN_HPP_
#include <com/sun/star/awt/TextAlign.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTEMPHASISMARK_HPP_
#include <com/sun/star/awt/FontEmphasisMark.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTRELIEF_HPP_
#include <com/sun/star/awt/FontRelief.hpp>
#endif
#include <tools/wintypes.hxx>		// for check states
namespace binfilter {

//.........................................................................
namespace xmloff
{
//.........................................................................

	using namespace ::com::sun::star::form;
	using namespace ::com::sun::star::sdb;
	using namespace ::com::sun::star::awt;
	using namespace ::binfilter::xmloff::token;

	const SvXMLEnumMapEntry*	OEnumMapper::s_pEnumMap[OEnumMapper::KNOWN_ENUM_PROPERTIES] =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	};

	//---------------------------------------------------------------------
	const SvXMLEnumMapEntry*	OEnumMapper::getEnumMap(EnumProperties _eProperty)
	{
		OSL_ENSURE(_eProperty < KNOWN_ENUM_PROPERTIES, "OEnumMapper::getEnumMap: invalid index (this will crash)!");

		const SvXMLEnumMapEntry*& rReturn = s_pEnumMap[_eProperty];
		if (!rReturn)
		{
			// the map for this property is not initialized yet
		 	switch (_eProperty)
			{
				// FormSubmitEncoding
				case epSubmitEncoding:
				{
					static SvXMLEnumMapEntry aSubmitEncodingMap[] =
					{
						{ XML_APPLICATION_X_WWW_FORM_URLENCODED, FormSubmitEncoding_URL },
						{ XML_MULTIPART_FORMDATA, FormSubmitEncoding_MULTIPART },
						{ XML_APPLICATION_TEXT, FormSubmitEncoding_TEXT },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aSubmitEncodingMap;
				}
				break;
				// FormSubmitMethod
				case epSubmitMethod:
				{
					static SvXMLEnumMapEntry aSubmitMethodMap[] =
					{
						{ XML_GET, FormSubmitMethod_GET },
						{ XML_POST, FormSubmitMethod_POST },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aSubmitMethodMap;
				}
				break;
				// CommandType
				case epCommandType:
				{
					static SvXMLEnumMapEntry aCommandTypeMap[] =
					{
						{ XML_TABLE, CommandType::TABLE },
						{ XML_QUERY, CommandType::QUERY },
						{ XML_COMMAND, CommandType::COMMAND },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aCommandTypeMap;
				}
				break;
				// NavigationBarMode
				case epNavigationType:
				{
					static SvXMLEnumMapEntry aNavigationTypeMap[] =
					{
						{ XML_NONE, NavigationBarMode_NONE },
						{ XML_CURRENT, NavigationBarMode_CURRENT },
						{ XML_PARENT, NavigationBarMode_PARENT },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aNavigationTypeMap;
				};
				break;
				// TabulatorCycle
				case epTabCyle:
				{
					static SvXMLEnumMapEntry aTabulytorCycleMap[] =
					{
						{ XML_RECORDS, TabulatorCycle_RECORDS },
						{ XML_CURRENT, TabulatorCycle_CURRENT },
						{ XML_PAGE, TabulatorCycle_PAGE },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aTabulytorCycleMap;
				};
				break;
				// FormButtonType
				case epButtonType:
				{
					static SvXMLEnumMapEntry aFormButtonTypeMap[] =
					{
						{ XML_PUSH, FormButtonType_PUSH },
						{ XML_SUBMIT, FormButtonType_SUBMIT },
						{ XML_RESET, FormButtonType_RESET },
						{ XML_URL, FormButtonType_URL },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aFormButtonTypeMap;
				};
				break;
				// ListSourceType
				case epListSourceType:
				{
					static SvXMLEnumMapEntry aListSourceTypeMap[] =
					{
						{ XML_VALUE_LIST, ListSourceType_VALUELIST },
						{ XML_TABLE, ListSourceType_TABLE },
						{ XML_QUERY, ListSourceType_QUERY },
						{ XML_SQL, ListSourceType_SQL },
						{ XML_SQL_PASS_THROUGH, ListSourceType_SQLPASSTHROUGH },
						{ XML_TABLE_FIELDS, ListSourceType_TABLEFIELDS },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aListSourceTypeMap;
				};
				break;
				// check state of a checkbox
				case epCheckState:
				{
					static SvXMLEnumMapEntry aCheckStateMap[] =
					{
						{ XML_UNCHECKED, STATE_NOCHECK },
						{ XML_CHECKED, STATE_CHECK },
						{ XML_UNKNOWN, STATE_DONTKNOW },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aCheckStateMap;
				};
				break;
				case epTextAlign:
				{
					static SvXMLEnumMapEntry aTextAlignMap[] =
					{
						{ XML_START,		TextAlign::LEFT },
						{ XML_CENTER,		TextAlign::CENTER },
						{ XML_END,			TextAlign::RIGHT },
						{ XML_JUSTIFY,		-1 },
						{ XML_JUSTIFIED,	-1 },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aTextAlignMap;
				};
				break;
				case epBorderWidth:
				{
					static SvXMLEnumMapEntry aBorderTypeMap[] =
					{
						{ XML_NONE,	    0 },
						{ XML_HIDDEN, 	0 },
						{ XML_SOLID, 	2 },
						{ XML_DOUBLE,	2 },
						{ XML_DOTTED, 	2 },
						{ XML_DASHED, 	2 },
						{ XML_GROOVE, 	1 },
						{ XML_RIDGE,  	1 },
						{ XML_INSET,  	1 },
						{ XML_OUTSET, 	1 },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aBorderTypeMap;
				};
				break;

				case epFontEmphasis:
				{
					static SvXMLEnumMapEntry aFontEmphasisMap[] =
					{
						{ XML_NONE,	    FontEmphasisMark::NONE },
						{ XML_DOT,		FontEmphasisMark::DOT },
						{ XML_CIRCLE,	FontEmphasisMark::CIRCLE },
						{ XML_DISC,	    FontEmphasisMark::DISC },
						{ XML_ACCENT,	FontEmphasisMark::ACCENT },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aFontEmphasisMap;
				}
				break;

				case epFontRelief:
				{
					static SvXMLEnumMapEntry aFontReliefMap[] =
					{
						{ XML_NONE,		FontRelief::NONE },
						{ XML_ENGRAVED,	FontRelief::ENGRAVED },
						{ XML_EMBOSSED,	FontRelief::EMBOSSED },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aFontReliefMap;
				}
				break;

                case epListLinkageType:
				{
					static SvXMLEnumMapEntry aListLinkageMap[] =
					{
						{ XML_SELECTION,            0 },
						{ XML_SELECTION_INDEXES,    1 },
						{ XML_TOKEN_INVALID, 0 }
					};
					rReturn = aListLinkageMap;
				}
				break;
			}
		}

		return rReturn;
	}

//.........................................................................
}	// namespace xmloff
//.........................................................................

}//end of namespace binfilter
