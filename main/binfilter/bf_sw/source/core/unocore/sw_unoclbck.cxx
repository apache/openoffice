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




#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _UNOIDX_HXX
#include <unoidx.hxx>
#endif
#ifndef _UNOCLBCK_HXX
#include <unoclbck.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
namespace binfilter {

/* -----------------------------06.01.00 13:51--------------------------------

 ---------------------------------------------------------------------------*/
SwUnoCallBack::SwUnoCallBack(SwModify *pToRegisterIn)	:
	SwModify(pToRegisterIn)
{
}
/* -----------------------------06.01.00 13:51--------------------------------

 ---------------------------------------------------------------------------*/
SwUnoCallBack::~SwUnoCallBack()
{
}
/* -----------------------------01.09.00 12:03--------------------------------

 ---------------------------------------------------------------------------*/
SwXReferenceMark* SwUnoCallBack::GetRefMark(const SwFmtRefMark& rMark)
{
	SwClientIter aIter( *this );
	SwXReferenceMark* pxRefMark = (SwXReferenceMark*)aIter.First( TYPE( SwXReferenceMark ));
	while(pxRefMark)
	{
		SwDoc* pDoc = pxRefMark->GetDoc();
		if(pDoc)
		{
			const SwFmtRefMark*	pFmt = pDoc->GetRefMark(pxRefMark->GetMarkName());
			if(pFmt == &rMark)
				return pxRefMark;
		}
		pxRefMark = (SwXReferenceMark*)aIter.Next( );
	}
	return 0;
}
/* -----------------------------05.09.00 12:38--------------------------------

 ---------------------------------------------------------------------------*/
SwXFootnote* SwUnoCallBack::GetFootnote(const SwFmtFtn& rMark)
{
	SwClientIter aIter( *this );
	SwXFootnote* pxFootnote = (SwXFootnote*)aIter.First( TYPE( SwXFootnote ));
	while(pxFootnote)
	{
		SwDoc* pDoc = pxFootnote->GetDoc();
		if(pDoc)
		{
			const SwFmtFtn* pFtn = pxFootnote->FindFmt();
			if(pFtn == &rMark)
				return pxFootnote;
		}
		pxFootnote = (SwXFootnote*)aIter.Next( );
	}
	return 0;
}

/* -----------------------------27.11.00 17:15--------------------------------

 ---------------------------------------------------------------------------*/
SwXDocumentIndexMark* SwUnoCallBack::GetTOXMark(const SwTOXMark& rMark)
{
	SwClientIter aIter( *this );
	SwXDocumentIndexMark* pxIndexMark = (SwXDocumentIndexMark*)aIter.First( TYPE( SwXDocumentIndexMark ));
	while(pxIndexMark)
	{
		const SwTOXMark* pMark = pxIndexMark->GetTOXMark();
		if(pMark == &rMark)
			return pxIndexMark;

		pxIndexMark = (SwXDocumentIndexMark*)aIter.Next( );
	}
	return 0;
}

}
