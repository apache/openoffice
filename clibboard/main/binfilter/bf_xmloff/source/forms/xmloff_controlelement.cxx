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



#ifndef _XMLOFF_FORMS_CONTROLELEMENT_HXX_
#include "controlelement.hxx"
#endif
namespace binfilter {

//.........................................................................
namespace xmloff
{
//.........................................................................

	//=====================================================================
	//= OControlElement
	//=====================================================================
	//---------------------------------------------------------------------
	const sal_Char* OControlElement::getElementName(ElementType _eType)
	{
		switch (_eType)
		{
			case TEXT: return "text";
			case TEXT_AREA: return "textarea";
			case PASSWORD: return "password";
			case FILE: return "file";
			case FORMATTED_TEXT: return "formatted-text";
			case FIXED_TEXT: return "fixed-text";
			case COMBOBOX: return "combobox";
			case LISTBOX: return "listbox";
			case BUTTON: return "button";
			case IMAGE: return "image";
			case CHECKBOX: return "checkbox";
			case RADIO: return "radio";
			case FRAME: return "frame";
			case IMAGE_FRAME: return "image-frame";
			case HIDDEN: return "hidden";
			case GRID: return "grid";

			default: return "generic-control";
		}
	}

//.........................................................................
}	// namespace xmloff
//.........................................................................


}//end of namespace binfilter
