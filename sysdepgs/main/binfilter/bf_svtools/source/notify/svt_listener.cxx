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



// MARKER(update_precomp.py): autogen include statement, do not remove

#ifndef GCC
#endif

#ifndef DEBUG_HXX
#include <tools/debug.hxx>
#endif

#include "broadcast.hxx"
#include "listener.hxx"
#include "listenerbase.hxx"
#include "listeneriter.hxx"

namespace binfilter
{

//====================================================================
TYPEINIT0(SvtListener);

//====================================================================
// simple ctor of class SvtListener

SvtListener::SvtListener()
	: pBrdCastLst( 0 )
{
}
//--------------------------------------------------------------------

// copy ctor of class SvtListener

SvtListener::SvtListener( const SvtListener &rListener )
	: pBrdCastLst( 0 )
{
	SvtListenerBase* pLst = rListener.pBrdCastLst;
	while( pLst )
	{
		new SvtListenerBase( *this, *pLst->GetBroadcaster() );
		pLst = pLst->GetNext();
	}
}
//--------------------------------------------------------------------

// unregisteres the SvtListener from its SvtBroadcasters

SvtListener::~SvtListener()
{
	EndListeningAll();
}

//--------------------------------------------------------------------

// registeres at a specific SvtBroadcaster

BOOL SvtListener::StartListening( SvtBroadcaster& rBroadcaster )
{
	const SvtListenerBase* pLst = pBrdCastLst;
	while( pLst )
	{
		if( &rBroadcaster == pLst->GetBroadcaster() )
		{
			// double, than return
			return FALSE;
		}
		pLst = pLst->GetNext();
	}
	new SvtListenerBase( *this, rBroadcaster );
	return TRUE;
}

//--------------------------------------------------------------------

// unregisteres at a specific SvtBroadcaster

BOOL SvtListener::EndListening( SvtBroadcaster& rBroadcaster )
{
	SvtListenerBase *pLst = pBrdCastLst, *pPrev = pLst;
	while( pLst )
	{
		if( &rBroadcaster == pLst->GetBroadcaster() )
		{
			if( pBrdCastLst == pLst )
				pBrdCastLst = pLst->GetNext();
			else
				pPrev->SetNext( pLst->GetNext() );

			delete pLst;
			return TRUE;
		}
		pPrev = pLst;
		pLst = pLst->GetNext();
	}
	return FALSE;
}

//--------------------------------------------------------------------

// unregisteres all Broadcasters

void SvtListener::EndListeningAll()
{
	SvtListenerBase *pLst = pBrdCastLst;
	while( pLst )
	{
		SvtListenerBase *pDel = pLst;
		pLst = pLst->GetNext();

		delete pDel;
	}
	pBrdCastLst = 0;
}

#ifdef DBG_UTIL

BOOL SvtListener::IsListening( SvtBroadcaster& rBroadcaster ) const
{
	const SvtListenerBase *pLst = pBrdCastLst;
	while( pLst )
	{
		if( &rBroadcaster == pLst->GetBroadcaster() )
			break;
		pLst = pLst->GetNext();
	}
	return 0 != pLst;
}

#endif

//--------------------------------------------------------------------

// base implementation of notification handler

void SvtListener::Notify( SvtBroadcaster&
#ifdef DBG_UTIL
rBC
#endif
, const SfxHint& )
{
	DBG_ASSERT( IsListening( rBC ),
				"notification from unregistered broadcaster" );
}


}
