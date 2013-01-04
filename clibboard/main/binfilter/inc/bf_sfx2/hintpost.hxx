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


#ifndef _SFXHINTPOST_HXX
#define _SFXHINTPOST_HXX

#ifndef _SFXGENLINK_HXX
#include <bf_sfx2/genlink.hxx>
#endif

#ifndef _REF_HXX //autogen
#include <tools/ref.hxx>
#endif
namespace binfilter {

class SfxHint;

//===================================================================


//-------------------------------------------------------------------

class SfxHintPoster: public SvRefBase

/*	[Beschreibung]

	Mit Instanzen dieser Klasse k"onnen eindeutige Events per PostUserEvent
	"uber die StarView-Application verschickt werden. Wenn das User-Event
	ausgel"ost wird, wird der Handler <Event()> gerufen, dessen
	Basisimplementierung den mit <SetEventHdl()> angegbenen Link ruft.

	Die Instanz wird via Ref-Count mindestens solange gehalten, wie
	ein ggf. abgeschicktes Event noch nicht angekommen ist. Sollte das
	Ziel vorher sterben, ist die Verbindung zuvor mit 'SetEventHdl(GenLink())'
	zu kappen.
*/

{
	ULONG			nId;
	GenLink 		aLink;

private:
#if _SOLAR__PRIVATE
					DECL_LINK( DoEvent_Impl, SfxHint * );
#endif

protected:
	virtual         ~SfxHintPoster();
	virtual void	Event( SfxHint* pPostedHint );
};

//-------------------------------------------------------------------

SV_DECL_IMPL_REF(SfxHintPoster);

}//end of namespace binfilter
#endif
