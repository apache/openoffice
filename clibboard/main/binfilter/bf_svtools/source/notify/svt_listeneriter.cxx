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

#ifndef _DEBUG_HXX
#include <tools/debug.hxx>
#endif

#include "listenerbase.hxx"
#include "listeneriter.hxx"
#include "broadcast.hxx"
#include "listener.hxx"

namespace binfilter
{

SvtListenerIter* SvtListenerIter::pListenerIters = 0;

SvtListenerIter::SvtListenerIter( SvtBroadcaster& rBrdcst )
	: rRoot( rBrdcst )
{
	// hinten einketten!
	pNxtIter = 0;
	if( pListenerIters )
	{
		SvtListenerIter* pTmp = pListenerIters;
		while( pTmp->pNxtIter )
			pTmp = pTmp->pNxtIter;
		pTmp->pNxtIter = this;
	}
	else
		pListenerIters = this;

	pAkt = rRoot.pRoot;
	pDelNext = pAkt;
}



SvtListenerIter::~SvtListenerIter()
{
	if( pListenerIters )
	{
		if( pListenerIters == this )
			pListenerIters = pNxtIter;
		else
		{
			SvtListenerIter* pTmp = pListenerIters;
			while( pTmp->pNxtIter != this )
				if( 0 == ( pTmp = pTmp->pNxtIter ) )
					return ;
			pTmp->pNxtIter = pNxtIter;
		}
	}
}

void SvtListenerIter::RemoveListener( SvtListenerBase& rDel,
										SvtListenerBase* pNext )
{
	// Update the ListenerIter
	SvtListenerIter* pTmp = pListenerIters;
	while( pTmp )
	{
		if( pTmp->pAkt == &rDel || pTmp->pDelNext == &rDel )
			pTmp->pDelNext = pNext;
		pTmp = pTmp->pNxtIter;
	}
}

SvtListener* SvtListenerIter::GoNext()
{
	if( pDelNext == pAkt )
	{
		pAkt = pAkt->GetRight();
		pDelNext = pAkt;
	}
	else
		pAkt = pDelNext;
	return pAkt ? pAkt->GetListener() : 0;
}

SvtListener* SvtListenerIter::GoStart() 		// zum Anfang des Baums
{
	pAkt = rRoot.pRoot;
	if( pAkt )
		while( pAkt->GetLeft() )
			pAkt = pAkt->GetLeft();
	pDelNext = pAkt;
	return pAkt ? pAkt->GetListener() : 0;
}

}
