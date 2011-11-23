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


#ifndef _SVT_LISTENER_HXX
#define _SVT_LISTENER_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif

namespace binfilter
{

class SvtBroadcaster;
class SfxHint;
class SvtListenerBase;

//-------------------------------------------------------------------------

class  SvtListener
{
	friend class SvtListenerBase;
	SvtListenerBase *pBrdCastLst;

	const SvtListener&	operator=(const SvtListener &); // n.i., ist verboten

public:
	TYPEINFO();

						SvtListener();
						SvtListener( const SvtListener &rCopy );
	virtual 			~SvtListener();

	BOOL				StartListening( SvtBroadcaster& rBroadcaster );
	BOOL				EndListening( SvtBroadcaster& rBroadcaster );
	void				EndListeningAll();
#ifdef DBG_UTIL
	BOOL				IsListening( SvtBroadcaster& rBroadcaster ) const;
#endif
	BOOL 				HasBroadcaster() const { return 0 != pBrdCastLst; }

	virtual void		Notify( SvtBroadcaster& rBC, const SfxHint& rHint );
};

}

#endif
