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



#ifndef SC_TABLINK_HXX
#define SC_TABLINK_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SC_REFRESHTIMER_HXX
#include "refreshtimer.hxx"
#endif

#ifndef _EMBOBJ_HXX 
#include <bf_so3/embobj.hxx>
#endif

#ifndef _LNKBASE_HXX //autogen
#include <bf_so3/lnkbase.hxx>
#endif
namespace binfilter {

#ifndef SO2_DECL_SVEMBEDDEDOBJECT_DEFINED
#define SO2_DECL_SVEMBEDDEDOBJECT_DEFINED
SO2_DECL_REF(SvEmbeddedObject)
#endif

class ScDocShell;
class SfxObjectShell;

class ScTableLink : public ::binfilter::SvBaseLink, public ScRefreshTimer
{
private:
	ScDocShell*	pDocShell;		// Container
	String		aFileName;
	String		aFilterName;
	String		aOptions;
	BOOL		bInCreate;
	BOOL		bInEdit;
	BOOL		bAddUndo;
	BOOL		bDoPaint;

public:
	TYPEINFO();
	ScTableLink( ScDocShell* pDocSh, const String& rFile,
					const String& rFilter, const String& rOpt, ULONG nRefresh );
	ScTableLink( SfxObjectShell* pShell, const String& rFile,
					const String& rFilter, const String& rOpt, ULONG nRefresh );
	virtual ~ScTableLink();
	virtual void DataChanged( const String& rMimeType,
							  const ::com::sun::star::uno::Any & rValue );


	BOOL	Refresh(const String& rNewFile, const String& rNewFilter,
					const String* pNewOptions /* = NULL */, ULONG nNewRefresh );
	void	SetInCreate(BOOL bSet)		{ bInCreate = bSet; }
	void	SetAddUndo(BOOL bSet)		{ bAddUndo = bSet; }
	void	SetPaint(BOOL bSet)			{ bDoPaint = bSet; }

	const String& GetFileName() const	{ return aFileName; }
	const String& GetFilterName() const	{ return aFilterName; }
	const String& GetOptions() const	{ return aOptions; }


	DECL_LINK( RefreshHdl, ScTableLink* );

};


class ScDocument;
class SfxMedium;

class ScDocumentLoader
{
private:
	ScDocShell*			pDocShell;
	SvEmbeddedObjectRef aRef;
	SfxMedium*			pMedium;

public:
						ScDocumentLoader( const String& rFileName,
											String& rFilterName, String& rOptions,
											UINT32 nRekCnt = 0, BOOL bWithInteraction = FALSE );
						~ScDocumentLoader();
	ScDocument*			GetDocument();
	ScDocShell*			GetDocShell()		{ return pDocShell; }
	BOOL				IsError() const;

	static String		GetOptions( SfxMedium& rMedium );
	static void			GetFilterName( const String& rFileName,
										String& rFilter, String& rOptions,
										BOOL bWithContent = FALSE );
	static void			RemoveAppPrefix( String& rFilterName );
};

} //namespace binfilter
#endif

