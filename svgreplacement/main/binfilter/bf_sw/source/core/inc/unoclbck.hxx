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


#ifndef _UNOCLBCK_HXX
#define _UNOCLBCK_HXX


#ifndef _CALBCK_HXX
#include <calbck.hxx>
#endif
namespace binfilter {

class SwXReferenceMark;
class SwFmtRefMark;
class SwFmtFtn;
class SwXFootnote;
class SwTOXMark;
class SwXDocumentIndexMark;

class SwUnoCallBack : public SwModify
{
public:
	SwUnoCallBack(SwModify *pToRegisterIn);
	virtual ~SwUnoCallBack();

	// returns the API object of a reference mark if available
	SwXReferenceMark* 	GetRefMark(const SwFmtRefMark& rMark);
	SwXFootnote*		GetFootnote(const SwFmtFtn& rMark);
	SwXDocumentIndexMark* GetTOXMark(const SwTOXMark& rMark);
};
} //namespace binfilter
#endif
