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



#ifndef SC_AREALINK_HXX
#define SC_AREALINK_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
#ifndef SC_REFRESHTIMER_HXX
#include "refreshtimer.hxx"
#endif

#ifndef _LNKBASE_HXX //autogen
#include <bf_so3/lnkbase.hxx>
#endif
namespace binfilter {

class ScDocShell;
class SfxObjectShell;

class ScAreaLink : public ::binfilter::SvBaseLink, public ScRefreshTimer
{
private:
	ScDocShell*	pDocShell;		// Container
	String		aFileName;
	String		aFilterName;
	String		aOptions;
	String		aSourceArea;
	ScRange		aDestArea;
	BOOL		bAddUndo;
	BOOL		bInCreate;
	BOOL		bDoInsert;		// wird fuer das erste Update auf FALSE gesetzt

	BOOL		FindExtRange( ScRange& rRange, ScDocument* pSrcDoc, const String& rAreaName );

public:
	TYPEINFO();
	ScAreaLink( SfxObjectShell* pShell, const String& rFile,
					const String& rFilter, const String& rOpt,
					const String& rArea, const ScRange& rDest, ULONG nRefresh );
	virtual ~ScAreaLink();

	virtual void DataChanged( const String& rMimeType,
							  const ::com::sun::star::uno::Any & rValue );


	BOOL	Refresh( const String& rNewFile, const String& rNewFilter,
					const String& rNewArea, ULONG nNewRefresh );

	void	SetInCreate(BOOL bSet)					{ bInCreate = bSet; }
	void	SetDoInsert(BOOL bSet)					{ bDoInsert = bSet; }
	void	SetDestArea(const ScRange& rNew);


	const String&	GetFile() const			{ return aFileName;		}
	const String&	GetFilter() const		{ return aFilterName;	}
	const String&	GetOptions() const		{ return aOptions;		}
	const String&	GetSource() const		{ return aSourceArea;	}
	const ScRange&	GetDestArea() const		{ return aDestArea;		}

	DECL_LINK( RefreshHdl, ScAreaLink* );

};


} //namespace binfilter
#endif
