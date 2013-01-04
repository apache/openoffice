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


#ifndef SC_EEIMPORT_HXX
#define SC_EEIMPORT_HXX

#include <bf_svtools/bf_solar.h>

#include "global.hxx"
class SvStream;
class Table;
namespace binfilter {

class ScDocument;
class ScEEParser;
class ScTabEditEngine;

struct ScEEParseEntry;

class ScEEImport
{
protected:
	ScRange 			aRange;
	ScDocument*			pDoc;
	ScEEParser*			pParser;
	ScTabEditEngine*	pEngine;
	Table*				pRowHeights;

	BOOL				GraphicSize( USHORT nCol, USHORT nRow, USHORT nTab,
							ScEEParseEntry* );
	void				InsertGraphic( USHORT nCol, USHORT nRow, USHORT nTab,
							ScEEParseEntry* );

public:
						ScEEImport( ScDocument* pDoc, const ScRange& rRange );
	virtual				~ScEEImport();

	ULONG				Read( SvStream& rStream );
	ScRange				GetRange()		{ return aRange; }
	virtual void		WriteToDocument( BOOL bSizeColsRows = FALSE,
										double nOutputFactor = 1.0 );
};



} //namespace binfilter
#endif
