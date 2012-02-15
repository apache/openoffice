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


#ifndef _SWDDETBL_HXX
#define _SWDDETBL_HXX

#include <bf_svtools/bf_solar.h>

#include "swtable.hxx"
namespace binfilter {

class SwDDEFieldType;

class SwDDETable : public SwTable
{
public:
	TYPEINFO();
	// Constructor movet alle Lines/Boxen aus der SwTable zu sich.
	// Die SwTable ist danach Leer und muss geloescht werden.
	SwDDETable( SwTable& rTable, SwDDEFieldType* pDDEType, 
		BOOL bUpdate = TRUE ):SwTable( rTable ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 BOOL bUpdate = TRUE );
	SwDDEFieldType* GetDDEFldType();
	inline const SwDDEFieldType* GetDDEFldType() const;
};


// ----------- Inlines -----------------------------

inline const SwDDEFieldType* SwDDETable::GetDDEFldType() const
{
	return ((SwDDETable*)this)->GetDDEFldType();
}

} //namespace binfilter
#endif
