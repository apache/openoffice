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



#ifndef _SD_IMAPINFO_HXX
#define _SD_IMAPINFO_HXX

#ifndef _SD_SDIOCMPT_HXX
#include "sdiocmpt.hxx"
#endif
#ifndef _SD_GLOB_HXX
#include "glob.hxx"
#endif
#ifndef _SVDOBJ_HXX //autogen
#include <bf_svx/svdobj.hxx>
#endif
#ifndef _IMAP_HXX //autogen
#include <bf_svtools/imap.hxx>
#endif
#include "bf_so3/staticbaseurl.hxx"
namespace binfilter {


/*************************************************************************
|*
|*
|*
\************************************************************************/

class SdIMapInfo : public SdrObjUserData, public SfxListener
{

	ImageMap		aImageMap;

public:
					SdIMapInfo() :
						SdrObjUserData( SdUDInventor, SD_IMAPINFO_ID, 0 ) {};

					SdIMapInfo( const ImageMap& rImageMap ) :
						SdrObjUserData( SdUDInventor, SD_IMAPINFO_ID, 0 ),
						aImageMap( rImageMap ) {};

					SdIMapInfo( const SdIMapInfo& rIMapInfo ) :
						SdrObjUserData( SdUDInventor, SD_IMAPINFO_ID, 0 ),
						aImageMap( rIMapInfo.aImageMap ) {};

	virtual 		~SdIMapInfo() {};

	virtual SdrObjUserData* Clone( SdrObject* pObj ) const { return new SdIMapInfo( *this ); }

	virtual void WriteData( SvStream& rOStm );
	virtual void ReadData( SvStream& rIStm );

	void			SetImageMap( const ImageMap& rIMap ) { aImageMap = rIMap; }
	const ImageMap& GetImageMap() const { return aImageMap; }
};


/*************************************************************************
|*
|*
|*
\************************************************************************/

inline void SdIMapInfo::WriteData( SvStream& rOStm )
{
	SdrObjUserData::WriteData( rOStm );

	SdIOCompat aIO( rOStm, STREAM_WRITE, 1 );

    aImageMap.Write(
        rOStm, ::binfilter::StaticBaseUrl::GetBaseURL(INetURLObject::NO_DECODE));
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

inline void SdIMapInfo::ReadData( SvStream& rIStm )
{
	SdrObjUserData::ReadData( rIStm );

	SdIOCompat aIO( rIStm, STREAM_READ );

    aImageMap.Read(
        rIStm, ::binfilter::StaticBaseUrl::GetBaseURL(INetURLObject::NO_DECODE));
}


} //namespace binfilter
#endif		// _SD_IMAPINFO_HXX


