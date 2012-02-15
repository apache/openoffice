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



#ifndef SC_SCDOCPOL_HXX
#define SC_SCDOCPOL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXITEMPOOL_HXX //autogen
#include <bf_svtools/itempool.hxx>
#endif
namespace binfilter {

class ScStyleSheet;

//------------------------------------------------------------------------

class ScDocumentPool: public SfxItemPool
{
	SfxPoolItem**	ppPoolDefaults;
	SfxItemPool*	pSecondary;
	static USHORT*	pVersionMap1;
	static USHORT*	pVersionMap2;
	static USHORT*	pVersionMap3;
	static USHORT*	pVersionMap4;
	static USHORT*	pVersionMap5;
	static USHORT*	pVersionMap6;
	static USHORT*	pVersionMap7;
	static USHORT*	pVersionMap8;
	static USHORT*	pVersionMap9;

public:
			ScDocumentPool( SfxItemPool* pSecPool = NULL, BOOL bLoadRefCounts = FALSE );
			~ScDocumentPool();


	virtual const SfxPoolItem&	Put( const SfxPoolItem&, USHORT nWhich = 0 );
	virtual void				Remove( const SfxPoolItem& );
	static void					CheckRef( const SfxPoolItem& );
	void						MyLoadCompleted();


	static void	InitVersionMaps();
	static void	DeleteVersionMaps();
};




} //namespace binfilter
#endif
