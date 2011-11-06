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



#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _TOOLS_RCID_H
#include <tools/rcid.h>
#endif

#ifndef MATH_COMMAND_HXX
#include "command.hxx"
#endif
namespace binfilter {

SmCommandDesc::SmCommandDesc(const ResId& rResId) :
	Resource(rResId)
{
	if (IsAvailableRes(ResId(1).SetRT(RSC_STRING)))
		pSample = new String(ResId(1));
	else
		pSample = new String;

	if (IsAvailableRes(ResId(2).SetRT(RSC_STRING)))
		pCommand = new String(ResId(2));
	else
		pCommand = new String;

	if (IsAvailableRes(ResId(3).SetRT(RSC_STRING)))
		pText = new String(ResId(3));
	else
		pText = new String("sorry, no help available");

	if (IsAvailableRes(ResId(4).SetRT(RSC_STRING)))
		pHelp = new String(ResId(4));
	else
		pHelp = new String("sorry, no help available");

	if (IsAvailableRes(ResId(1).SetRT(RSC_BITMAP)))
		pGraphic =	new Bitmap(ResId(1));
	else
		pGraphic = new Bitmap();

	FreeResource();
}

SmCommandDesc::~SmCommandDesc()
{
	if (pText)		delete pText;
	if (pHelp)		delete pHelp;
	if (pSample)	delete pSample;
	if (pCommand)	delete pCommand;
	if (pGraphic)	delete pGraphic;
}


}
