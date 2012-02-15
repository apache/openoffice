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



#ifndef _EDITUND2_HXX
#define _EDITUND2_HXX

//#include <tools.hxx>

#ifndef _UNDO_HXX
#include <bf_svtools/undo.hxx>
#endif
namespace binfilter {

class ImpEditEngine;

class EditUndoManager : public SfxUndoManager
{
private:
	ImpEditEngine*	pImpEE;
public:
					EditUndoManager( ImpEditEngine* pImpEE );

};

// -----------------------------------------------------------------------
// EditUndo
// ------------------------------------------------------------------------
class EditUndo : public SfxUndoAction
{
private:
	USHORT 			nId;
	ImpEditEngine*	pImpEE;

public:
					EditUndo( USHORT nI, ImpEditEngine* pImpEE );
	virtual 		~EditUndo();

	ImpEditEngine*	GetImpEditEngine() const	{ return pImpEE; }

	virtual void	Undo() 		= 0;
	virtual void	Redo()		= 0;
	virtual void 	Repeat()	= 0;

	virtual String	GetComment() const;
	virtual USHORT	GetId() const;
};

}//end of namespace binfilter
#endif	// _EDITUND2_HXX

