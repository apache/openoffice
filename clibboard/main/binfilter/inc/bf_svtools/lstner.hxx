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


#ifndef _SFXLSTNER_HXX
#define _SFXLSTNER_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif

#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif

namespace binfilter
{

class SfxBroadcaster;
class SfxHint;

SV_DECL_PTRARR( SfxBroadcasterArr_Impl, SfxBroadcaster*, 0, 2 )

#define SFX_NOTIFY( rBC, rBCT, rHint, rHintT ) \
		Notify( rBC, rHint )

//-------------------------------------------------------------------------

class  SfxListener
{
	::binfilter::SfxBroadcasterArr_Impl aBCs;

private:
	const SfxListener&	operator=(const SfxListener &); // n.i., ist verboten

public:
	TYPEINFO();

						SfxListener();
						SfxListener( const SfxListener &rCopy );
	virtual 			~SfxListener();

	BOOL				StartListening( SfxBroadcaster& rBroadcaster, BOOL bPreventDups = FALSE );
	BOOL				EndListening( SfxBroadcaster& rBroadcaster, BOOL bAllDups = FALSE );
	void				EndListeningAll();
	BOOL				IsListening( SfxBroadcaster& rBroadcaster ) const;

	USHORT				GetBroadcasterCount() const
						{ return aBCs.Count(); }
	SfxBroadcaster* 	GetBroadcasterJOE( USHORT nNo ) const
						{ return (SfxBroadcaster*) aBCs.GetObject(nNo); }

	virtual void		Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

#ifndef _NOTIFY_HXX
	void RemoveBroadcaster_Impl( SfxBroadcaster& rBC );
#endif
};

}

#endif
