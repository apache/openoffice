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


#ifndef _SFXMETRICITEM_HXX
#define _SFXMETRICITEM_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _SFXINTITEM_HXX
#include <bf_svtools/intitem.hxx>
#endif

namespace binfilter
{

DBG_NAMEEX_VISIBILITY(SfxMetricItem, )

// -----------------------------------------------------------------------

class  SfxMetricItem: public SfxInt32Item
{
public:
							 TYPEINFO();
							 SfxMetricItem( USHORT nWhich = 0, UINT32 nValue = 0 );
							 SfxMetricItem( USHORT nWhich, SvStream & );
							 SfxMetricItem( const SfxMetricItem& );
							 ~SfxMetricItem() {
								 DBG_DTOR(SfxMetricItem, 0); }

	virtual int				 ScaleMetrics( long lMult, long lDiv );
	virtual	int				 HasMetrics() const;

};

}

#endif

