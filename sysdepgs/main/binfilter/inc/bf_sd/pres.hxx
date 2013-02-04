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


#ifndef _PRESENTATION_HXX
#define _PRESENTATION_HXX
namespace binfilter {

enum AutoLayout
{
	AUTOLAYOUT_TITLE,
	AUTOLAYOUT_ENUM,
	AUTOLAYOUT_CHART,
	AUTOLAYOUT_2TEXT,
	AUTOLAYOUT_TEXTCHART,
	AUTOLAYOUT_ORG,
	AUTOLAYOUT_TEXTCLIP,
	AUTOLAYOUT_CHARTTEXT,
	AUTOLAYOUT_TAB,
	AUTOLAYOUT_CLIPTEXT,
	AUTOLAYOUT_TEXTOBJ,
	AUTOLAYOUT_OBJ,
	AUTOLAYOUT_TEXT2OBJ,
	AUTOLAYOUT_OBJTEXT,
	AUTOLAYOUT_OBJOVERTEXT,
	AUTOLAYOUT_2OBJTEXT,
	AUTOLAYOUT_2OBJOVERTEXT,
	AUTOLAYOUT_TEXTOVEROBJ,
	AUTOLAYOUT_4OBJ,
	AUTOLAYOUT_ONLY_TITLE,
	AUTOLAYOUT_NONE,
	AUTOLAYOUT_NOTES,
	AUTOLAYOUT_HANDOUT1,
	AUTOLAYOUT_HANDOUT2,
	AUTOLAYOUT_HANDOUT3,
	AUTOLAYOUT_HANDOUT4,
	AUTOLAYOUT_HANDOUT6,
	AUTOLAYOUT_VERTICAL_TITLE_TEXT_CHART,
	AUTOLAYOUT_VERTICAL_TITLE_VERTICAL_OUTLINE,
	AUTOLAYOUT_TITLE_VERTICAL_OUTLINE,
	AUTOLAYOUT_TITLE_VERTICAL_OUTLINE_CLIPART
};

enum PageKind
{
	PK_STANDARD,
	PK_NOTES,
	PK_HANDOUT
};

enum EditMode
{
	EM_PAGE,
	EM_MASTERPAGE
};

enum DocumentType
{
	DOCUMENT_TYPE_IMPRESS,
	DOCUMENT_TYPE_DRAW
};

enum NavigatorDragType
{
	NAVIGATOR_DRAGTYPE_NONE,
	NAVIGATOR_DRAGTYPE_URL,
	NAVIGATOR_DRAGTYPE_LINK,
	NAVIGATOR_DRAGTYPE_EMBEDDED
};
#define NAVIGATOR_DRAGTYPE_COUNT 4

} //namespace binfilter
#endif	// _PRESENTATION_HXX

