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


#ifndef _HFFRM_HXX
#define _HFFRM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVMEMPOOL_HXX //autogen
#include <tools/mempool.hxx>
#endif

#include "layfrm.hxx"
namespace binfilter {


class SwHeadFootFrm : public SwLayoutFrm
{
protected:
    void FormatSize(SwTwips nUL, const SwBorderAttrs * pAttrs);
    void FormatPrt(SwTwips & nUL, const SwBorderAttrs * pAttrs);
    inline BOOL GetEatSpacing() const; // in hffrm.cxx
public:
    SwHeadFootFrm(SwFrmFmt * pFrm, USHORT aType);
	virtual void Format( const SwBorderAttrs *pAttrs = 0 );
    virtual SwTwips GrowFrm( SwTwips, 
                             BOOL bTst = FALSE, BOOL bInfo = FALSE );
    virtual SwTwips ShrinkFrm( SwTwips, 
                               BOOL bTst = FALSE, BOOL bInfo = FALSE );
};

class SwHeaderFrm: public SwHeadFootFrm
{
public:
	SwHeaderFrm( SwFrmFmt* pFrm ) : SwHeadFootFrm(pFrm, FRMC_HEADER) {};

	DECL_FIXEDMEMPOOL_NEWDEL(SwHeaderFrm)
};

class SwFooterFrm: public SwHeadFootFrm
{
public:
	SwFooterFrm( SwFrmFmt* pFrm ) : SwHeadFootFrm(pFrm, FRMC_FOOTER) {};

	DECL_FIXEDMEMPOOL_NEWDEL(SwFooterFrm)
};

} //namespace binfilter
#endif	//_HFFRM_HXX
