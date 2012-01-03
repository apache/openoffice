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


#ifndef _SFX_SAVEOPT_HXX
#define _SFX_SAVEOPT_HXX

#include <bf_svtools/bf_solar.h>


#include <tools/wintypes.hxx>
#ifndef _SV_FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
// -----------------------------------------------------------------------
#include "bf_sfx2/cfgitem.hxx"
namespace binfilter {

struct SfxOptions_Impl;

// -----------------------------------------------------------------------
class SfxOptions : public SfxConfigItem
{
#if _SOLAR__PRIVATE
	struct SfxOptions_Impl
	{
		SfxMapUnit	eUserMetric;
		BOOL		bSaveDocView:1,
					bSaveRelINet:1,
					bSaveRelFSys:1,
					bAutoHelpAgent:1,
					bSaveGrfCompr:1,
					bSaveGrfOrig:1,
					bWelcomeScreen:1;
		String		aRet;
	};
#endif

	SfxOptions_Impl*pImp;
	String			aDocMgrConfig;
	USHORT			nAutoSaveTime;
	USHORT			nUndoActions;
	FieldUnit  		eMetric;
	BOOL			bBackup:1,
					bAutoSave:1,
					bDocInfSave:1,
					bIndepGrfFmt:1,
					bAutoSavePrompt:1,
					bSaveWorkingSet:1,
					bHelpBalloons:1,
					bHelpTips:1;

private:
	void			SetMetric_Impl( FieldUnit e, BOOL bLoading );

protected:
	virtual int 	Load(SvStream&);
	virtual BOOL	Store(SvStream&);
	virtual	void	UseDefault();

public:
					SfxOptions();
					~SfxOptions();

	void			SetBackup( BOOL b ) 		{ bBackup = b;SetDefault(FALSE);}
	BOOL			IsBackup() const			{ return bBackup;		  }
	void			SetAutoSave( BOOL b )		{ bAutoSave = b;	SetDefault(FALSE);	  }
	BOOL			IsAutoSave() const			{ return bAutoSave; 	  }
	void			SetAutoSavePrompt( BOOL b ) { bAutoSavePrompt = b;SetDefault(FALSE);  }
	BOOL			IsAutoSavePrompt() const	{ return bAutoSavePrompt; }
	void			SetAutoSaveTime( USHORT n ) { nAutoSaveTime = n;SetDefault(FALSE);  }
	USHORT			GetAutoSaveTime() const 	{ return nAutoSaveTime;   }
	void			SetDocInfoSave(BOOL b)		{ bDocInfSave = b;	 SetDefault(FALSE); }
	BOOL			IsDocInfoSave()	const		{ return bDocInfSave;     }

	void			SetSaveOriginalGraphics(BOOL b);
	BOOL			IsSaveOriginalGraphics() const;
	void			SetSaveGraphicsCompressed(BOOL b);
	BOOL			IsSaveGraphicsCompressed() const;

	void			SetIndepGrfFmt(BOOL b)		{ bIndepGrfFmt = b;	  SetDefault(FALSE);}
	BOOL			IsIndepGrfFmt()	const		{ return bIndepGrfFmt;    }
	void			SetSaveWorkingSet( BOOL b )	{ bSaveWorkingSet = b;SetDefault(FALSE);}
	BOOL			IsSaveWorkingSet() const	{ return bSaveWorkingSet;		  }
	void			SetSaveDocWins( BOOL b );
	void			SetSaveDocView( BOOL b );
	BOOL			IsSaveDocView() const;
	void			SetAutoHelpAgent( BOOL b );
	BOOL			IsAutoHelpAgent() const;
	void			SetWelcomeScreen( BOOL b );
	BOOL			IsWelcomeScreen() const;

	void			SetMetric(FieldUnit e);
	FieldUnit		GetMetric() const			{ return eMetric; }
	SfxMapUnit		GetUserMetric() const;

	void			SetUndoCount(USHORT n)		{ nUndoActions = n; SetDefault(FALSE);}
	USHORT 			GetUndoCount() const		{ return nUndoActions; }

	void			SetHelpBalloons( BOOL b )	{ bHelpBalloons = b; SetDefault(FALSE); }
	BOOL			IsHelpBalloons() const		{ return bHelpBalloons; }
	void			SetHelpTips( BOOL b )		{ bHelpTips = b; SetDefault(FALSE); }
	BOOL			IsHelpTips() const			{ return bHelpTips; }

	const String&	GetDocumentManagerConfig() const
					{ return aDocMgrConfig; }
	void            SetDocumentManagerConfig( const String &rConfig )
					{ aDocMgrConfig = rConfig; SetDefault(FALSE); }

	void			SetSaveRelINet( BOOL b );
	BOOL			IsSaveRelINet() const;
	void			SetSaveRelFSys( BOOL b );
	BOOL			IsSaveRelFSys() const;
};

}//end of namespace binfilter
#endif
