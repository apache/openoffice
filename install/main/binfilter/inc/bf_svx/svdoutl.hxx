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



#ifndef _SVDOUTL_HXX
#define _SVDOUTL_HXX

#ifndef _OUTLINER_HXX //autogen
#include <bf_svx/outliner.hxx>
#endif
namespace binfilter {

class SdrTextObj;

class SdrOutliner : public Outliner
{
protected:
	const SdrTextObj* pTextObj;

public:
//    TYPEINFO();

    SdrOutliner( SfxItemPool* pItemPool, USHORT nMode );
    virtual ~SdrOutliner() ;

	void  SetTextObj( const SdrTextObj* pObj );
	void SetTextObjNoInit( const SdrTextObj* pObj );

	const SdrTextObj* GetTextObj() const { return pTextObj; }

	virtual String  CalcFieldValue(const SvxFieldItem& rField, USHORT nPara, USHORT nPos, Color*& rpTxtColor, Color*& rpFldColor);
};

}//end of namespace binfilter
#endif //_SVDOUTL_HXX

