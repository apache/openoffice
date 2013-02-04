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



#ifndef _APPLET_HXX
#define _APPLET_HXX

#include <bf_svtools/bf_solar.h>

#include <bf_so3/ipobj.hxx>
#include <bf_svtools/ownlist.hxx>

#ifndef INCLUDED_SO3DLLAPI_H
#include "bf_so3/so3dllapi.h"
#endif
//=========================================================================
class INetURLObject;
class SjApplet;
class RmAppletObject;

namespace binfilter {

struct SvAppletData_Impl;

class SO3_DLLPUBLIC SvAppletObject : public SvInPlaceObject
/*	[Beschreibung]

	Die Klasse AppletObject ist die Abbildung eines InPlace-Objektes
	auf einen Applet-Server. Dieses Objekt kann nur InPlace- und
	nicht UI-Aktiviert werden.

	[Probleme]
	Es handelt sich bei diesem Objekt immer um einen Link. Dieser Link
	kann allerdings InPlace aktiviert werden.
*/
{
friend class SvAppletEnvironment;

private:
	SvAppletData_Impl *	pImpl;

	SO3_DLLPRIVATE void			DataChanged_Impl( BOOL bOnlyEmbedSource );

protected:
	SO3_DLLPRIVATE BOOL			StartApplet();
	SO3_DLLPRIVATE virtual void    FillClass( SvGlobalName * pClassName,
							   ULONG * pFormat,
							   String * pAppName,
							   String * pFullTypeName,
							   String * pShortTypeName,
                               long nFileFormat = SOFFICE_FILEFORMAT_CURRENT ) const;
					// Protokoll
	SO3_DLLPRIVATE virtual void    Open( BOOL bOpen );
	SO3_DLLPRIVATE virtual void    InPlaceActivate( BOOL );
	SO3_DLLPRIVATE virtual ErrCode Verb( long, SvEmbeddedClient * pCallerClient,
						Window * pWin, const Rectangle * pWorkRectPixel );

					// Datenaustausch
	SO3_DLLPRIVATE virtual void    SetVisArea( const Rectangle & rVisArea );
	SO3_DLLPRIVATE virtual void    Draw( OutputDevice *,
						  const JobSetup & rSetup,
						  USHORT nAspect = ASPECT_CONTENT );

					// Laden speichern
	SO3_DLLPRIVATE virtual BOOL    InitNew( SvStorage * );
	SO3_DLLPRIVATE virtual BOOL    Load( SvStorage * );
	SO3_DLLPRIVATE virtual BOOL    Save();
	SO3_DLLPRIVATE virtual BOOL    SaveAs( SvStorage * );
	SO3_DLLPRIVATE virtual void    HandsOff();
	SO3_DLLPRIVATE virtual BOOL    SaveCompleted( SvStorage * );

	SO3_DLLPRIVATE 				~SvAppletObject();
public:
					SO2_DECL_BASIC_CLASS_DLL(SvAppletObject,SOAPP)
					SvAppletObject();

	virtual ULONG	GetMiscStatus() const;
	virtual BOOL	IsLink() const;

					// spezielle Internet Methoden
	void			SetCommandList( const SvCommandList & rList );
	const SvCommandList & GetCommandList() const;
	void			SetClass( const String & rClass );
	const String &	GetClass()const;
	void			SetName( const XubString & rClass );
	const XubString &GetName() const;
	void			SetCodeBase( const XubString & );
	const XubString & GetCodeBase() const;
	void			SetMayScript( BOOL bMayScript );
	BOOL			IsMayScript() const;
};

SO2_DECL_IMPL_REF(SvAppletObject)

}

#endif // _Applet_HXX
