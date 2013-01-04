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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "errhdl.hxx"			// fuers ASSERT
#include "error.h"				// fuers ASSERT
#include "ndindex.hxx"
namespace binfilter {

#ifdef DBG_UTIL
int SwNodeIndex::nSerial = 0;
#endif


/*N*/ SwNodeRange::SwNodeRange( const SwNodeIndex &rS, const SwNodeIndex &rE )
/*N*/ 	: aStart( rS ), aEnd( rE )
/*N*/ {}

/*N*/ SwNodeRange::SwNodeRange( const SwNodeRange &rRange )
/*N*/ 	: aStart( rRange.aStart ), aEnd( rRange.aEnd )
/*N*/ {}

/*N*/ SwNodeRange::SwNodeRange( const SwNodeIndex& rS, long nSttDiff,
/*N*/ 						  const SwNodeIndex& rE, long nEndDiff )
/*N*/ 	: aStart( rS, nSttDiff ), aEnd( rE, nEndDiff )
/*N*/ {}

/*N*/ SwNodeRange::SwNodeRange( const SwNode& rS, long nSttDiff,
/*N*/ 						  const SwNode& rE, long nEndDiff )
/*N*/ 	: aStart( rS, nSttDiff ), aEnd( rE, nEndDiff )
/*N*/ {}


/*N*/ SwNodeIndex::SwNodeIndex( SwNodes& rNds, ULONG nIdx )
/*N*/ 	: pNd( rNds[ nIdx ] ), pNext( 0 ), pPrev( 0 )
/*N*/ {
/*N*/ 	rNds.RegisterIndex( *this );
/*N*/ 
/*N*/ #ifdef DBG_UTIL
/*N*/ 	MySerial = ++nSerial;		// nur in der nicht PRODUCT-Version
/*N*/ #endif
/*N*/ }


/*N*/ SwNodeIndex::SwNodeIndex( const SwNodeIndex& rIdx, long nDiff )
/*N*/ 	: pNext( 0 ), pPrev( 0 )
/*N*/ {
/*N*/ 	if( nDiff )
/*N*/ 		pNd = rIdx.GetNodes()[ rIdx.GetIndex() + nDiff ];
/*N*/ 	else
/*N*/ 		pNd = rIdx.pNd;
/*N*/ 
/*N*/ 	pNd->GetNodes().RegisterIndex( *this );
/*N*/ #ifdef DBG_UTIL
/*N*/ 	MySerial = ++nSerial;		// nur in der nicht PRODUCT-Version
/*N*/ #endif
/*N*/ }


/*N*/ SwNodeIndex::SwNodeIndex( const SwNode& rNd, long nDiff )
/*N*/ 	: pNext( 0 ), pPrev( 0 )
/*N*/ {
/*N*/ 	if( nDiff )
/*N*/ 		pNd = rNd.GetNodes()[ rNd.GetIndex() + nDiff ];
/*N*/ 	else
/*N*/ 		pNd = (SwNode*)&rNd;
/*N*/ 
/*N*/ 	pNd->GetNodes().RegisterIndex( *this );
/*N*/ #ifdef DBG_UTIL
/*N*/ 	MySerial = ++nSerial;		// nur in der nicht PRODUCT-Version
/*N*/ #endif
/*N*/ }


/*N*/ void SwNodeIndex::Remove()
/*N*/ {
/*N*/ 	pNd->GetNodes().DeRegisterIndex( *this );
/*N*/ }

/*N*/ SwNodeIndex& SwNodeIndex::operator=( const SwNodeIndex& rIdx )
/*N*/ {
/*N*/ 	if( &pNd->GetNodes() != &rIdx.pNd->GetNodes() )
/*N*/ 	{
/*N*/ 		pNd->GetNodes().DeRegisterIndex( *this );
/*N*/ 		pNd = rIdx.pNd;
/*N*/ 		pNd->GetNodes().RegisterIndex( *this );
/*N*/ 	}
/*N*/ 	else
/*N*/ 		pNd = rIdx.pNd;
/*N*/ 	return *this;
/*N*/ }

/*N*/ SwNodeIndex& SwNodeIndex::operator=( const SwNode& rNd )
/*N*/ {
/*N*/ 	if( &pNd->GetNodes() != &rNd.GetNodes() )
/*N*/ 	{
/*?*/ 		pNd->GetNodes().DeRegisterIndex( *this );
/*?*/ 		pNd = (SwNode*)&rNd;
/*?*/ 		pNd->GetNodes().RegisterIndex( *this );
/*N*/ 	}
/*N*/ 	else
/*N*/ 		pNd = (SwNode*)&rNd;
/*N*/ 	return *this;
/*N*/ }


/*N*/ SwNodeIndex& SwNodeIndex::Assign( const SwNode& rNd, long nOffset )
/*N*/ {
/*N*/ 	if( &pNd->GetNodes() != &rNd.GetNodes() )
/*N*/ 	{
/*?*/ 		pNd->GetNodes().DeRegisterIndex( *this );
/*?*/ 		pNd = (SwNode*)&rNd;
/*?*/ 		pNd->GetNodes().RegisterIndex( *this );
/*N*/ 	}
/*N*/ 	else
/*N*/ 		pNd = (SwNode*)&rNd;
/*N*/ 
/*N*/ 	if( nOffset )
/*N*/ 		pNd = pNd->GetNodes()[ pNd->GetIndex() + nOffset ];
/*N*/ 
/*N*/ 	return *this;
/*N*/ }


}
