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


#ifndef _ROWFRM_HXX
#define _ROWFRM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVMEMPOOL_HXX //autogen
#include <tools/mempool.hxx>
#endif

#include "layfrm.hxx"
namespace binfilter {

class SwTableLine;
class SwBorderAttrs;

class SwRowFrm: public SwLayoutFrm
{
	virtual void Format( const SwBorderAttrs *pAttrs = 0 );
		//Aendern nur die Framesize, nicht die PrtArea-SSize
    virtual SwTwips ShrinkFrm( SwTwips, SZPTR
							   BOOL bTst = FALSE, BOOL bInfo = FALSE );
    virtual SwTwips GrowFrm  ( SwTwips, SZPTR
							   BOOL bTst = FALSE, BOOL bInfo = FALSE );

	const SwTableLine *pTabLine;

protected:
	virtual void MakeAll();

public:
	SwRowFrm( const SwTableLine & );
	~SwRowFrm();

	virtual	void  Modify( SfxPoolItem*, SfxPoolItem* );

	//Zum Anmelden der Flys nachdem eine Zeile erzeugt _und_ eingefuegt wurde.
	//Muss vom Erzeuger gerufen werden, denn erst nach dem Konstruieren wird
	//Das Teil gepastet; mithin ist auch erst dann die Seite zum Anmelden der
	//Flys erreichbar.
	void RegistFlys( SwPageFrm *pPage = 0 );

	const SwTableLine *GetTabLine() const { return pTabLine; }

	//Passt die Zellen auf die aktuelle Hoehe an, invalidiert die Zellen
	//wenn die Direction nicht der Hoehe entspricht.
	void AdjustCells( const SwTwips nHeight, const BOOL bHeight );
	DECL_FIXEDMEMPOOL_NEWDEL(SwRowFrm)
};

} //namespace binfilter
#endif
