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



#ifndef _SVDHLPLN_HXX
#define _SVDHLPLN_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif


#ifndef _CONTNR_HXX //autogen
#include <tools/contnr.hxx>
#endif
class OutputDevice;
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////

enum SdrHelpLineKind {SDRHELPLINE_POINT,SDRHELPLINE_VERTICAL,SDRHELPLINE_HORIZONTAL};
#define SDRHELPLINE_MIN SDRHELPLINE_POINT
#define SDRHELPLINE_MAX SDRHELPLINE_HORIZONTAL

#define SDRHELPLINE_POINT_PIXELSIZE 15 /* Tatsaechliche Groesse= PIXELSIZE*2+1 */

class SdrHelpLine {
	Point            aPos; // je nach Wert von eKind ist X oder Y evtl. belanglos
	SdrHelpLineKind  eKind;
public:
	SdrHelpLine(SdrHelpLineKind eNewKind=SDRHELPLINE_POINT): eKind(eNewKind) {}
	SdrHelpLine(SdrHelpLineKind eNewKind, const Point& rNewPos): aPos(rNewPos), eKind(eNewKind) {}
	FASTBOOL operator==(const SdrHelpLine& rCmp) const { return aPos==rCmp.aPos && eKind==rCmp.eKind; }
	FASTBOOL operator!=(const SdrHelpLine& rCmp) const { return !operator==(rCmp); }

	void            SetKind(SdrHelpLineKind eNewKind) { eKind=eNewKind; }
	SdrHelpLineKind GetKind() const                   { return eKind; }
	void            SetPos(const Point& rPnt)         { aPos=rPnt; }
	const Point&    GetPos() const                    { return aPos; }

	// OutputDevice wird benoetigt, da Fangpunkte eine feste Pixelgroesse haben

	/* returns true if this and the given help line would be rendered at the same pixel position
		of the given OutputDevice. This can be used to avoid drawing multiple help lines with xor
		on same position which could render them invisible */

	friend SvStream& operator<<(SvStream& rOut, const SdrHelpLine& rHL);
	friend SvStream& operator>>(SvStream& rIn, SdrHelpLine& rHL);
};

#define SDRHELPLINE_NOTFOUND 0xFFFF

class SdrHelpLineList {
	Container aList;
protected:
	SdrHelpLine* GetObject(USHORT i) const { return (SdrHelpLine*)(aList.GetObject(i)); }
public:
	SdrHelpLineList(): aList(1024,4,4) {}
	SdrHelpLineList(const SdrHelpLineList& rSrcList): aList(1024,4,4)      { *this=rSrcList; }
	~SdrHelpLineList()                                                     { Clear(); }
	void               Clear();
	void               operator=(const SdrHelpLineList& rSrcList);
	USHORT             GetCount() const                                    { return USHORT(aList.Count()); }
	void               Insert(const SdrHelpLine& rHL, USHORT nPos=0xFFFF)  { aList.Insert(new SdrHelpLine(rHL),nPos); }
	void               Delete(USHORT nPos)                                 { delete (SdrHelpLine*)aList.Remove(nPos); }
	void               Move(USHORT nPos, USHORT nNewPos)                   { aList.Insert(aList.Remove(nPos),nNewPos); }
	SdrHelpLine&       operator[](USHORT nPos)                             { return *GetObject(nPos); }
	const SdrHelpLine& operator[](USHORT nPos) const                       { return *GetObject(nPos); }
	friend SvStream& operator<<(SvStream& rOut, const SdrHelpLineList& rHLL);
	friend SvStream& operator>>(SvStream& rIn, SdrHelpLineList& rHLL);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_SVDHLPLN_HXX

