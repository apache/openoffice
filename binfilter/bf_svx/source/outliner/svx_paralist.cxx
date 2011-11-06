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

#include <paralist.hxx>
#include <outliner.hxx>		// nur wegen Paragraph, muss geaendert werden!
#include <numdef.hxx>
namespace binfilter {

/*N*/ DBG_NAME(Paragraph)

/*N*/ Paragraph::Paragraph( USHORT nDDepth )
/*N*/ 	: aBulSize( -1, -1)
/*N*/ {
/*N*/ 	DBG_CTOR( Paragraph, 0 );
/*N*/ 
/*N*/     DBG_ASSERT( ( nDDepth < SVX_MAX_NUM ) || ( nDDepth == 0xFFFF ), "Paragraph-CTOR: nDepth invalid!" );
/*N*/ 
/*N*/ 	nDepth = nDDepth;
/*N*/ 	nFlags = 0;
/*N*/ 	bVisible = TRUE;
/*N*/ }


/*N*/ Paragraph::~Paragraph()
/*N*/ {
/*N*/ 	DBG_DTOR( Paragraph, 0 );
/*N*/ }

/*N*/ void ParagraphList::Clear( BOOL bDestroyParagraphs )
/*N*/ {
/*N*/ 	if ( bDestroyParagraphs )
/*N*/ 	{
/*N*/ 		for ( ULONG n = GetParagraphCount(); n; )
/*N*/ 		{
/*N*/ 			Paragraph* pPara = GetParagraph( --n );
/*N*/ 			delete pPara;
/*N*/ 		}
/*N*/ 	}
/*N*/ 	List::Clear();
/*N*/ }









/*NBFF*/ Paragraph* ParagraphList::GetParent( Paragraph* pParagraph, USHORT& rRelPos ) const
/*NBFF*/ {
/*NBFF*/ 	rRelPos = 0;
/*NBFF*/ 	ULONG n = GetAbsPos( pParagraph );
/*NBFF*/ 	Paragraph* pPrev = GetParagraph( --n );
/*NBFF*/ 	while ( pPrev && ( pPrev->GetDepth() >= pParagraph->GetDepth() ) )
/*NBFF*/ 	{
/*NBFF*/ 		if ( pPrev->GetDepth() == pParagraph->GetDepth() )
/*NBFF*/ 			rRelPos++;
/*NBFF*/ 		pPrev = GetParagraph( --n );
/*NBFF*/ 	}
/*NBFF*/ 
/*NBFF*/ 	return pPrev;
/*NBFF*/ }



}
