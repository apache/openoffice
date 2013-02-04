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



#ifndef _SVX_XDASH_HXX
#define _SVX_XDASH_HXX

//-------------
// class XDash
//-------------

#ifndef _XENUM_HXX //autogen
#include <bf_svx/xenum.hxx>
#endif
namespace binfilter {

class XDash
{
protected:
	XDashStyle  eDash;
	USHORT      nDots;
	ULONG       nDotLen;
	USHORT      nDashes;
	ULONG       nDashLen;
	ULONG       nDistance;

public:
		  XDash(XDashStyle eDash = XDASH_RECT,
				USHORT nDots = 1, ULONG nDotLen = 20,
				USHORT nDashes = 1, ULONG nDashLen = 20, ULONG nDistance = 20);

	int operator==(const XDash& rDash) const;

	void SetDashStyle(XDashStyle eNewStyle) { eDash = eNewStyle; }
	void SetDots(USHORT nNewDots)           { nDots = nNewDots; }
	void SetDotLen(ULONG nNewDotLen)        { nDotLen = nNewDotLen; }
	void SetDashes(USHORT nNewDashes)       { nDashes = nNewDashes; }
	void SetDashLen(ULONG nNewDashLen)      { nDashLen = nNewDashLen; }
	void SetDistance(ULONG nNewDistance)    { nDistance = nNewDistance; }

	XDashStyle  GetDashStyle() const        { return eDash; }
	USHORT      GetDots() const             { return nDots; }
	ULONG       GetDotLen() const           { return nDotLen; }
	USHORT      GetDashes() const           { return nDashes; }
	ULONG       GetDashLen() const          { return nDashLen; }
	ULONG       GetDistance() const         { return nDistance; }

};

}//end of namespace binfilter
#endif
