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



#ifndef _PARALIST_HXX
#define _PARALIST_HXX

//STRIP008 class Paragraph;

#include <bf_svtools/bf_solar.h>

#ifndef _LIST_HXX
#include <tools/list.hxx>
#endif

#ifndef _LINK_HXX
#include <tools/link.hxx>
#endif
namespace binfilter {
class Paragraph;

class ParagraphList : private List
{
private:
	Link			aVisibleStateChangedHdl;

public:
	void			Clear( BOOL bDestroyParagraphs );

	ULONG			GetParagraphCount() const			{ return List::Count(); }
	Paragraph*		GetParagraph( ULONG nPos ) const 	{ return (Paragraph*)List::GetObject( nPos ); }

	ULONG			GetAbsPos( Paragraph* pParent ) const { return List::GetPos( pParent ); }

	void			Insert( Paragraph* pPara, ULONG nAbsPos = LIST_APPEND ) { List::Insert( pPara, nAbsPos ); }
	void			Remove( ULONG nPara ) { List::Remove( nPara ); }


/*NBFF*/ 	Paragraph*		GetParent( Paragraph* pParagraph, USHORT& rRelPos ) const;


	void			SetVisibleStateChangedHdl( const Link& rLink ) { aVisibleStateChangedHdl = rLink; }
	Link			GetVisibleStateChangedHdl() const { return aVisibleStateChangedHdl; }
};

}//end of namespace binfilter
#endif
