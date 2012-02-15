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



#ifndef SC_UNDODRAW_HXX
#define SC_UNDODRAW_HXX

#ifndef _UNDO_HXX //autogen
#include <bf_svtools/undo.hxx>
#endif
namespace binfilter {

class ScDocShell;

class ScUndoDraw: public SfxUndoAction
{
	SfxUndoAction*	pDrawUndo;
	ScDocShell*		pDocShell;

public:
							TYPEINFO();
							ScUndoDraw( SfxUndoAction* pUndo, ScDocShell* pDocSh );
	virtual 				~ScUndoDraw();

	SfxUndoAction*			GetDrawUndo()		{ return pDrawUndo; }
	void					ForgetDrawUndo();

};



} //namespace binfilter
#endif

