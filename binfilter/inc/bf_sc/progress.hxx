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



#ifndef SC_PROGRESS_HXX
#define SC_PROGRESS_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFX_PROGRESS_HXX //autogen
#include <bf_sfx2/progress.hxx>
#endif
namespace binfilter {

class ScDocument;

class ScProgress
{
private:
	static	SfxProgress*	pGlobalProgress;
	static	ULONG			nGlobalRange;
	static	ULONG			nGlobalPercent;
	static	BOOL			bGlobalNoUserBreak;
	static	ScProgress*		pInterpretProgress;
	static	ScProgress*		pOldInterpretProgress;
	static	ULONG			nInterpretProgress;
	static	BOOL			bAllowInterpretProgress;
	static	ScDocument*		pInterpretDoc;
	static	BOOL			bIdleWasDisabled;

			SfxProgress*	pProgress;

							// not implemented
							ScProgress( const ScProgress& );
			ScProgress&		operator=( const ScProgress& );

	static	void			CalcGlobalPercent( ULONG nVal )
								{
									nGlobalPercent = nGlobalRange ?
										nVal * 100 / nGlobalRange : 0;
								}

public:
	static	SfxProgress*	GetGlobalSfxProgress() { return pGlobalProgress; }
	static	BOOL			IsUserBreak() { return !bGlobalNoUserBreak; }
	static	ScProgress*		GetInterpretProgress() { return pInterpretProgress; }
	static	BOOL			GetAllowInterpret() { return bAllowInterpretProgress; }
	static	ULONG			GetInterpretCount() { return nInterpretProgress; }
	static	ULONG			GetGlobalRange()	{ return nGlobalRange; }
	static	ULONG			GetGlobalPercent()	{ return nGlobalPercent; }

							ScProgress( SfxObjectShell* pObjSh,
										 const String& rText,
										 ULONG nRange, BOOL bAllDocs = FALSE,
										 BOOL bWait = TRUE );
							~ScProgress();

#ifdef SC_PROGRESS_CXX
							// nur fuer DummyInterpret, sonst nie benutzen!!!
							ScProgress();
#endif
							// kann NULL sein!
			SfxProgress*	GetSfxProgress() const { return pProgress; }

			BOOL			SetState( ULONG nVal, ULONG nNewRange = 0 )
								{
									if ( pProgress )
									{
										if ( nNewRange )
											nGlobalRange = nNewRange;
										CalcGlobalPercent( nVal );
										if ( !pProgress->SetState( nVal, nNewRange ) )
											bGlobalNoUserBreak = FALSE;
										return bGlobalNoUserBreak;
									}
									return TRUE;
								}
			BOOL			SetStateCountDown( ULONG nVal )
								{
									if ( pProgress )
									{
										CalcGlobalPercent( nGlobalRange - nVal );
										if ( !pProgress->SetState( nGlobalRange - nVal ) )
											bGlobalNoUserBreak = FALSE;
										return bGlobalNoUserBreak;
									}
									return TRUE;
								}
			BOOL			SetStateOnPercent( ULONG nVal )
								{	// nur wenn Prozent mehr als vorher
									if ( nGlobalRange && (nVal * 100 /
											nGlobalRange) > nGlobalPercent )
										return SetState( nVal );
									return TRUE;
								}
			BOOL			SetStateCountDownOnPercent( ULONG nVal )
								{	// nur wenn Prozent mehr als vorher
									if ( nGlobalRange &&
											((nGlobalRange - nVal) * 100 /
											nGlobalRange) > nGlobalPercent )
										return SetStateCountDown( nVal );
									return TRUE;
								}
			ULONG			GetState()
								{
									if ( pProgress )
										return pProgress->GetState();
									return 0;
								}
};


} //namespace binfilter
#endif

