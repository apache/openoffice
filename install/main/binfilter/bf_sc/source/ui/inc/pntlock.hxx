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



#ifndef SC_PNTLOCK_HXX
#define SC_PNTLOCK_HXX

#ifndef SC_RANGELST_HXX
#include "rangelst.hxx"
#endif
namespace binfilter {

class ScPaintLockData
{
private:
	ScRangeListRef	xRangeList;
	USHORT			nMode;
	USHORT			nLevel;
	USHORT			nDocLevel;
	USHORT			nParts;
	BOOL			bModified;

public:
					ScPaintLockData(USHORT nNewMode);
					~ScPaintLockData();

	void			AddRange( const ScRange& rRange, USHORT nP );

	void			SetModified()	{ bModified = TRUE; }
	void			IncLevel(sal_Bool bDoc)
						{ if (bDoc) ++nDocLevel; else ++nLevel; }
	void			DecLevel(sal_Bool bDoc)
						{ if (bDoc) --nDocLevel; else --nLevel; }

	const ScRangeListRef&	GetRangeList() const			{ return xRangeList; }
	USHORT					GetParts() const				{ return nParts; }
	USHORT					GetLevel(sal_Bool bDoc) const
								{ return bDoc ? nDocLevel : nLevel; }
	BOOL					GetModified() const				{ return bModified; }

					// fuer Wiederherstellen nach Reset
	void			SetLevel(USHORT nNew, sal_Bool bDoc)
						{ if (bDoc) nDocLevel = nNew; else nLevel = nNew; }
};

} //namespace binfilter
#endif

