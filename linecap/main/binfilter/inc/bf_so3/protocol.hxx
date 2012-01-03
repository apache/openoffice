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



#ifndef _PROTOCOL_HXX
#define _PROTOCOL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _ERRCODE_HXX //autogen
#include <tools/errcode.hxx>
#endif
#ifndef _TOOLS_STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef INCLUDED_SO3DLLAPI_H
#include "bf_so3/so3dllapi.h"
#endif

/*************************************************************************
*************************************************************************/
class  String;

namespace binfilter {

class  SvEmbeddedObject;
class  SvInPlaceObject;
class  SvEmbeddedClient;
class  SvInPlaceClient;
class  SoDll;
class  ImplSvEditObjectProtocol;

class SO3_DLLPUBLIC SvEditObjectProtocol
{
friend class SoDll;
private:
	ImplSvEditObjectProtocol * pImp;
	SO3_DLLPRIVATE static void         Imp_DeleteDefault();
	SO3_DLLPRIVATE BOOL                Release();
public:
	SvEditObjectProtocol();
	SvEditObjectProtocol( const SvEditObjectProtocol & );
	SO3_DLLPRIVATE SvEditObjectProtocol & operator = ( const SvEditObjectProtocol & );
	~SvEditObjectProtocol();

	// Objekte
	SvEmbeddedObject * GetObj() const;
	SvInPlaceObject *  GetIPObj() const;
	SvEmbeddedClient * GetClient() const;
	SvInPlaceClient *  GetIPClient() const;

	// Zustandsabfragen
	BOOL IsConnect() const;
	BOOL IsOpen() const;
	BOOL IsEmbed() const;
	BOOL IsPlugIn() const;
	BOOL IsInPlaceActive() const;
	BOOL IsUIActive() const;

	// Protokolle
	ErrCode EmbedProtocol();
	ErrCode	PlugInProtocol();
	ErrCode	IPProtocol();
	ErrCode UIProtocol();

	// Protokoll zurueckfahren
	BOOL Reset();
	BOOL Reset2Connect();
	BOOL Reset2Open();
	BOOL Reset2Embed();
	BOOL Reset2PlugIn();
	BOOL Reset2InPlaceActive();
	BOOL Reset2UIActive();

	// Einzelschritte der Protokolle
	void Opened( BOOL bOpen );
	void Embedded( BOOL bEmbed );
	void PlugIn( BOOL bPlugIn );
	void InPlaceActivate( BOOL bActivate );
	void UIActivate( BOOL bActivate );
	void SetInClosed( BOOL bInClosed );
};

}

#endif // _PROTOCOL_HXX
