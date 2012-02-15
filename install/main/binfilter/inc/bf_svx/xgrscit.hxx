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



#ifndef _SVX_XGRSCIT_HXX
#define _SVX_XGRSCIT_HXX

#ifndef _SFXINTITEM_HXX //autogen
#include <bf_svtools/intitem.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* GradientenSchritte-Item fuer Gradienten-Fuellungen
|*
\************************************************************************/

class XGradientStepCountItem: public SfxUInt16Item
{
public:
							TYPEINFO();
							XGradientStepCountItem( USHORT nStepCount = 0 );
							XGradientStepCountItem( SvStream& rIn );
	virtual SfxPoolItem*    Clone( SfxItemPool* pPool = NULL ) const;
	virtual SfxPoolItem*    Create( SvStream& rIn, USHORT nVer ) const;
};

}//end of namespace binfilter
#endif
