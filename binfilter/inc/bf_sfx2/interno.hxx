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


#ifndef _SFX_INTERNO_HXX
#define _SFX_INTERNO_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _IPOBJ_HXX //autogen
#include <bf_so3/ipobj.hxx>
#endif
#include "bf_sfx2/objsh.hxx"
#include "bf_sfx2/sfx.hrc"

namespace binfilter {

struct SfxInPlaceObject_Impl;
class SfxObjectShell;
class INote;


//=========================================================================

class SfxInPlaceObject : public SvInPlaceObject
{
	SfxObjectShell* 	pObjShell;
	INote*              pNote;
	BOOL                bTriggerLinkTimer : 1;
	BOOL				bDisableViewScaling : 1;

#if _SOLAR__PRIVATE
private:

public:
#endif
                        SO2_DECL_STANDARD_CLASS(SfxInPlaceObject)
public:
	SfxObjectShell*     GetObjectShell() const { return pObjShell; }

protected:
    void                SetShell( SfxObjectShell *pObjSh );

	virtual void        FillClass( SvGlobalName * pClassName,
								   ULONG * pClipFormat,
								   String * pAppName,
								   String * pLongUserName,
                                   String * pUserName,
                                   long nVersion = SOFFICE_FILEFORMAT_CURRENT
	) const;

	virtual BOOL        InitNew( SvStorage * );         // Rekursiv
	virtual BOOL        Load( SvStorage * );            // Rekursiv
	virtual BOOL        Save();                         // Rekursiv
	virtual BOOL        SaveAs( SvStorage * pNewStg );  // Rekursiv
	virtual void        HandsOff();                     // Rekursiv
	virtual BOOL        SaveCompleted( SvStorage * );   // Rekursiv

	virtual ErrCode		Verb( long nVerbPos,
							SvEmbeddedClient *pCaller,
							Window *pWin,
							const Rectangle *pWorkAreaPixel );
	virtual BOOL		Verb( long nVerbPos, const Rectangle *pWorkAreaPixel );
	virtual void        Embed( BOOL bEmbedded );
	virtual void        Open( BOOL bOpen );
	virtual void        InPlaceActivate( BOOL );
	virtual void		DocumentNameChanged( const String & rDocName );

	virtual 			~SfxInPlaceObject();

public:
						SfxInPlaceObject();

	void                SetTriggerLinkTimer( BOOL bSet )
						{ bTriggerLinkTimer = bSet; }
	void                UpdateLinks();

	virtual void        SetVisArea( const Rectangle & rVisArea );

	void				SetModified( BOOL bSet );
	void				DisableViewScaling( BOOL bSet )
						{ bDisableViewScaling = bSet; }
	BOOL				IsViewScalingDisabled()
						{ return bDisableViewScaling; }
};

#ifndef SFX_DECL_INTERNALOBJECT_DEFINED
#define SFX_DECL_INTERNALOBJECT_DEFINED
SO2_DECL_REF(SfxInPlaceObject)
#endif
SO2_IMPL_REF(SfxInPlaceObject)

#if _SOLAR__PRIVATE
class SfxForceLinkTimer_Impl
{
	SfxInPlaceObject *pInObj;

public:
	SfxForceLinkTimer_Impl( SfxObjectShell *pObj );
	~SfxForceLinkTimer_Impl()
	{ if( pInObj ) pInObj->SetTriggerLinkTimer( TRUE ); }
};
#endif


}//end of namespace binfilter
#endif

