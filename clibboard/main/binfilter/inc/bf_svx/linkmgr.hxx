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


#ifndef _SVXLINKMGR_HXX
#define _SVXLINKMGR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _LINKMGR_HXX
#include <bf_so3/linkmgr.hxx>
#endif
class Graphic;
class Size;
namespace binfilter {


// Damit der Link ueber den Status der zu ladenen Grafik informierten werden
// verschickt das FileObject ein SvData, mit der FormatId
// "RegisterStatusInfoId" und ein einem String als Datentraeger. Dieser
// enthaelt den folgenden enum.
enum LinkState
{
	STATE_LOAD_OK,
	STATE_LOAD_ERROR,
	STATE_LOAD_ABORT
};

class SvxLinkManager : public ::binfilter::SvLinkManager
{
	SvxLinkManager( const SvxLinkManager& );
	SvxLinkManager& operator=( const SvxLinkManager& );

public:
	SvxLinkManager( SvPersist * pCacheCont );

	// den Link mit einem PseudoObject verbinden und in die Liste eintragen
	BOOL InsertFileLink( ::binfilter::SvBaseLink&,
						USHORT nFileType,
						const String& rTxt,
						const String* pFilterNm = 0,
						const String* pRange = 0 );

			// falls am Link schon alles eingestellt ist !

		// erfrage die Strings fuer den Dialog
	virtual BOOL GetDisplayNames( const ::binfilter::SvBaseLink*,
									String* pType,
									String* pFile = 0,
									String* pLink = 0,
									String* pFilter = 0 ) const;

	virtual ::binfilter::SvLinkSourceRef CreateObj( ::binfilter::SvBaseLink * );

	// eine Uebertragung wird abgebrochen, also alle DownloadMedien canceln
	// (ist zur Zeit nur fuer die FileLinks interressant!)
	void CancelTransfers();


	// um Status Informationen aus dem FileObject an den BaseLink zu
	// senden, gibt es eine eigene ClipBoardId. Das SvData-Object hat
	// dann die entsprechenden Informationen als String.
	// Wird zur Zeit fuer FileObject in Verbindung mit JavaScript benoetigt
	// - das braucht Informationen ueber Load/Abort/Error
	static ULONG  RegisterStatusInfoId();

	// if the mimetype says graphic/bitmap/gdimetafile then get the
	// graphic from the Any. Return says no errors
	static BOOL GetGraphicFromAny( const String& rMimeType,
								const ::com::sun::star::uno::Any & rValue,
								Graphic& rGrf );

};

}//end of namespace binfilter
#endif

