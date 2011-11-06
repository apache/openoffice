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



#ifndef SC_UNDOBASE_HXX
#define SC_UNDOBASE_HXX

#ifndef _UNDO_HXX //autogen
#include <bf_svtools/undo.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
namespace binfilter {

class ScDocument;
class ScDocShell;
class SdrUndoAction;
class ScRefUndoData;

//----------------------------------------------------------------------------

class ScSimpleUndo: public SfxUndoAction
{
public:
					ScSimpleUndo( ScDocShell* pDocSh );
	virtual 		~ScSimpleUndo();

	virtual BOOL	Merge( SfxUndoAction *pNextAction );

protected:
	ScDocShell*		pDocShell;
	SfxUndoAction*	pDetectiveUndo;

};

//----------------------------------------------------------------------------

enum ScBlockUndoMode { SC_UNDO_SIMPLE, SC_UNDO_MANUALHEIGHT, SC_UNDO_AUTOHEIGHT };

class ScBlockUndo: public ScSimpleUndo
{
public:
					ScBlockUndo( ScDocShell* pDocSh, const ScRange& rRange,
								 ScBlockUndoMode eBlockMode );
	virtual 		~ScBlockUndo();

protected:
	ScRange			aBlockRange;
	SdrUndoAction*	pDrawUndo;
	ScBlockUndoMode	eMode;

//	void			BeginRedo();

};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

class ScUndoWrapper: public SfxUndoAction           // for manual merging of actions
{
    SfxUndoAction*  pWrappedUndo;

public:
                            TYPEINFO();
                            ScUndoWrapper( SfxUndoAction* pUndo );
    virtual                 ~ScUndoWrapper();

    SfxUndoAction*          GetWrappedUndo()        { return pWrappedUndo; }
    void                    ForgetWrappedUndo();

    virtual BOOL            IsLinked();
    virtual void            SetLinked( BOOL bIsLinked );
    virtual void            Undo();
    virtual void            Redo();
    virtual void            Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL            CanRepeat(SfxRepeatTarget& rTarget) const;
    virtual BOOL            Merge( SfxUndoAction *pNextAction );
    virtual String          GetComment() const;
    virtual String          GetRepeatComment(SfxRepeatTarget&) const;
    virtual USHORT          GetId() const;
};


} //namespace binfilter
#endif

