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

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#include "doc.hxx"
#include "pagefrm.hxx"
#include "cntfrm.hxx"
#include "pam.hxx"

#ifndef _NODE_HXX //autogen
#include <node.hxx>
#endif
#include "pageiter.hxx"
#include "txtfrm.hxx"
namespace binfilter {




SwPageIter::SwPageIter( const SwDoc &rDoc, const SwPosition &rStartPos )
    : rPDoc( rDoc ), pPage(0)
{
    Seek( rStartPos );
}



BOOL SwPageIter::NextPage()
{
    if( IsEnd() )
        return FALSE;
    pPage = (SwPageFrm*)pPage->GetNext();
    return TRUE;
}



const SwPageDesc* SwPageIter::GetPageDesc() const
{
    return  ( IsEnd() )? 0 : pPage->GetPageDesc();
}



BOOL SwPageIter::Seek( const SwPosition &rPos )
{
    const SwTxtFrm *pTxt = (SwTxtFrm*)rPDoc.GetNodes()[rPos.nNode.GetIndex()]->
                                                GetCntntNode()->GetFrm();
    if ( !pTxt )
        return FALSE;

    pTxt = pTxt->GetFrmAtPos( rPos );
    pPage = pTxt->FindPageFrm();
    return TRUE;
}



BOOL SwPageIter::GetPosition( SwPosition &rPos ) const
{
    if( IsEnd() )
        return FALSE;

    const SwCntntFrm *pCnt = pPage->FindFirstBodyCntnt();
    if ( !pCnt )
        return FALSE;

    pCnt = ((SwTxtFrm*)pCnt)->GetFrmAtPos( rPos );
    if ( !pCnt )
        return FALSE;

    rPos.nNode = *pCnt->GetNode();
    rPos.nContent.Assign( rPos.nNode.GetNode().GetCntntNode(),
                            ((SwTxtFrm*)pCnt)->GetOfst() );

    return TRUE;
}


}
