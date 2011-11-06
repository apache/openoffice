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



#ifndef _XMLOFF_XMLEVENT_HXX
#include "xmlevent.hxx"
#endif
namespace binfilter {

//.........................................................................
namespace xmloff
{
//.........................................................................

	//=====================================================================
	//= event translation table
	//=====================================================================
	static const XMLEventNameTranslation aEventTranslations[] =
	{
		{ "XApproveActionListener::approveAction",		"on-approveaction" },
		{ "XActionListener::actionPerformed",			"on-performaction" },
		{ "XChangeListener::changed",					"on-change" },
		{ "XTextListener::textChanged",					"on-textchange" },
		{ "XItemListener::itemStateChanged",			"on-itemstatechange" },
		{ "XFocusListener::focusGained",				"on-focus" },
		{ "XFocusListener::focusLost",					"on-blur" },
		{ "XKeyListener::keyPressed",					"on-keydown" },
		{ "XKeyListener::keyReleased",					"on-keyup" },
		{ "XMouseListener::mouseEntered",				"on-mouseover" },
		{ "XMouseMotionListener::mouseDragged",			"on-mousedrag" },
		{ "XMouseMotionListener::mouseMoved",			"on-mousemove" },
		{ "XMouseListener::mousePressed",				"on-mousedown" },
		{ "XMouseListener::mouseReleased",				"on-mouseup" },
		{ "XMouseListener::mouseExited",				"on-mouseout" },
		{ "XResetListener::approveReset",				"on-approvereset" },
		{ "XResetListener::resetted",					"on-reset" },
		{ "XSubmitListener::approveSubmit",				"on-submit" },
		{ "XUpdateListener::approveUpdate",				"on-approveupdate" },
		{ "XUpdateListener::updated",					"on-update" },
		{ "XLoadListener::loaded",						"on-load" },
		{ "XLoadListener::reloading",					"on-startreload" },
		{ "XLoadListener::reloaded",					"on-reload" },
		{ "XLoadListener::unloading",					"on-startunload" },
		{ "XLoadListener::unloaded",					"on-unload" },
		{ "XConfirmDeleteListener::confirmDelete",		"on-confirmdelete" },
		{ "XRowSetApproveListener::approveRowChange",	"on-approverowchange" },
		{ "XRowSetListener::rowChanged",				"on-rowchange" },
		{ "XRowSetApproveListener::approveCursorMove",	"on-approvecursormove" },
		{ "XRowSetListener::cursorMoved",				"on-cursormove" },
		{ "XDatabaseParameterListener::approveParameter","on-supplyparameter" },
		{ "XSQLErrorListener::errorOccured",			"on-error" },
		{ 0, 0 }
	};

	const XMLEventNameTranslation* g_pFormsEventTranslation = aEventTranslations;

//.........................................................................
}	// namespace xmloff
//.........................................................................


}//end of namespace binfilter
