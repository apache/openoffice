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



#ifndef _SVDPOEV_HXX
#define _SVDPOEV_HXX

#include <bf_svtools/bf_solar.h>

// HACK to avoid too deep includes and to have some
// levels free in svdmark itself (MS compiler include depth limit)
#ifndef _SVDHDL_HXX
#include <bf_svx/svdhdl.hxx>
#endif

#include <bf_svx/svdedtv.hxx>
namespace binfilter {

//************************************************************
//   Defines
//************************************************************

enum SdrPathSmoothKind  {SDRPATHSMOOTH_DONTCARE,   // nur fuer Statusabfrage
						 SDRPATHSMOOTH_ANGULAR,    // Eckig
						 SDRPATHSMOOTH_ASYMMETRIC, // unsymmetrisch, normales Smooth
						 SDRPATHSMOOTH_SYMMETRIC}; // symmetrisch

enum SdrPathSegmentKind {SDRPATHSEGMENT_DONTCARE,  // nur fuer Statusabfrage
						 SDRPATHSEGMENT_LINE,      // gerader Streckenabschnitt
						 SDRPATHSEGMENT_CURVE,     // Kurvenabschnitt (Bezier)
						 SDRPATHSEGMENT_TOGGLE};   // nur fuer Set: Toggle

enum SdrObjClosedKind   {SDROBJCLOSED_DONTCARE,    // nur fuer Statusabfrage
						 SDROBJCLOSED_OPEN,        // Objekte geoeffnet (Linie, Polyline, ...)
						 SDROBJCLOSED_CLOSED,      // Objekte geschlossen (Polygon, ...)
						 SDROBJCLOSED_TOGGLE};     // nur fuer Set: Toggle (not implemented yet)

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// @@@@@   @@@@  @@  @@  @@  @@@@@ @@@@@  @@ @@@@@@  @@ @@ @@ @@@@@ @@   @@
// @@  @@ @@  @@ @@  @@  @@  @@    @@  @@ @@   @@    @@ @@ @@ @@    @@   @@
// @@  @@ @@  @@ @@  @@  @@  @@    @@  @@ @@   @@    @@ @@ @@ @@    @@ @ @@
// @@@@@  @@  @@ @@   @@@@   @@@@  @@  @@ @@   @@    @@@@@ @@ @@@@  @@@@@@@
// @@     @@  @@ @@    @@    @@    @@  @@ @@   @@     @@@  @@ @@    @@@@@@@
// @@     @@  @@ @@    @@    @@    @@  @@ @@   @@     @@@  @@ @@    @@@ @@@
// @@      @@@@  @@@@@ @@    @@@@@ @@@@@  @@   @@      @   @@ @@@@@ @@   @@
//
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class SdrPolyEditView: public SdrEditView
{
	friend class				SdrEditView;

protected:
	BOOL						bSetMarkedPointsSmoothPossible : 1;
	BOOL						bSetMarkedSegmentsKindPossible : 1;

	SdrPathSmoothKind			eMarkedPointsSmooth;
	SdrPathSegmentKind			eMarkedSegmentsKind;

private:
	void ImpClearVars();
	void ImpResetPolyPossibilityFlags();
	void ImpCheckPolyPossibilities();

	// Markierte Punkte kopieren und anstelle der alten markieren
	// ist noch nicht implementiert!
	typedef void (*PPolyTrFunc)(Point&, Point*, Point*, const void*, const void*, const void*, const void*, const void*);

public:
	SdrPolyEditView(SdrModel* pModel1, OutputDevice* pOut=NULL);
	virtual ~SdrPolyEditView();

	BOOL IsSetMarkedPointsSmoothPossible() const { ForcePossibilities(); return bSetMarkedPointsSmoothPossible; }
	SdrPathSmoothKind GetMarkedPointsSmooth() const { ForcePossibilities(); return eMarkedPointsSmooth; }

	// Ein PolySegment kann eine Strecke oder eine Bezierkurve sein.
	BOOL IsSetMarkedSegmentsKindPossible() const { ForcePossibilities(); return bSetMarkedSegmentsKindPossible; }
	SdrPathSegmentKind GetMarkedSegmentsKind() const { ForcePossibilities(); return eMarkedSegmentsKind; }

	// Moeglicherweise ist das Obj hinterher geloescht:
	BOOL IsDeleteMarkedPointsPossible() const { return HasMarkedPoints(); }


	// Hierbei entstehen eventuell beliebig viele neue Objekte:

	// Alle markierten Polylines werden zu Polygonen, alle offenen
	// Bezierkurven zu geschlossenen.
};

}//end of namespace binfilter
#endif //_SVDPOEV_HXX

