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



#ifndef _SVDXCGV_HXX
#define _SVDXCGV_HXX

#include <bf_svtools/bf_solar.h>

// HACK to avoid too deep includes and to have some
// levels free in svdmark itself (MS compiler include depth limit)
#ifndef _SVDHDL_HXX
#include <bf_svx/svdhdl.hxx>
#endif

#include <bf_svx/svdedxv.hxx>

#ifndef _GDIMTF_HXX //autogen
#include <vcl/gdimtf.hxx>
#endif
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@@ @@   @@  @@@@  @@  @@  @@@@  @@  @@  @@@@  @@@@@  @@ @@ @@ @@@@@ @@   @@
//  @@    @@@ @@@ @@  @@ @@  @@ @@  @@ @@@ @@ @@  @@ @@     @@ @@ @@ @@    @@   @@
//  @@     @@@@@  @@     @@  @@ @@  @@ @@@@@@ @@     @@     @@ @@ @@ @@    @@ @ @@
//  @@@@    @@@   @@     @@@@@@ @@@@@@ @@@@@@ @@ @@@ @@@@   @@@@@ @@ @@@@  @@@@@@@
//  @@     @@@@@  @@     @@  @@ @@  @@ @@ @@@ @@  @@ @@      @@@  @@ @@    @@@@@@@
//  @@    @@@ @@@ @@  @@ @@  @@ @@  @@ @@  @@ @@  @@ @@      @@@  @@ @@    @@@ @@@
//  @@@@@ @@   @@  @@@@  @@  @@ @@  @@ @@  @@  @@@@@ @@@@@    @   @@ @@@@@ @@   @@
//
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class SdrExchangeView: public SdrObjEditView
{
	friend class SdrPageView;

protected:

 //STRIP001    void                ImpGetPasteObjList(Point& rPos, SdrObjList*& rpLst);

	// liefert True, wenn rPt geaendert wurde

public:

    SdrExchangeView(SdrModel* pModel1, OutputDevice* pOut=NULL);

	// Alle markierten Objekte auf dem angegebenen OutputDevice ausgeben.

	// Z.B. fuer's Clipboard, Drag&Drop, ...
	// Alle markierten Objekte in ein Metafile stecken. Z.Zt. noch etwas
	// buggee (Offset..., Fremdgrafikobjekte (SdrGrafObj), Virtuelle
	// Objektkopien (SdrVirtObj) mit Ankerpos<>(0,0)).
	virtual GDIMetaFile GetMarkedObjMetaFile(BOOL bNoVDevIfOneMtfMarked=FALSE) const;

	// Alle markierten Objekte auf eine Bitmap malen. Diese hat die Farbtiefe
	// und Aufloesung des Bildschirms.

	// Alle markierten Objekte in ein neues Model kopieren. Dieses neue Model
	// hat dann genau eine Page. Das Flag PageNotValid an diesem Model ist
	// gesetzt. Daran ist zu erkennen, dass nur die Objekte der Page Gueltikeit
	// haben, die Page sebst jedoch nicht (Seitengroesse, Raender). Das neue
	// Model wird auf dem Heap erzeugt und wird an den Aufrufer dieser Methode
	// uebergeben. Dieser hat es dann spaeter zu entsorgen.
	// Beim einfuegen der markierten Objekte in die eine Page des neuen Model
	// findet ein Merging der seitenlokalen Layer statt. Sollte kein Platz mehr
	// fuer weitere seitenlokale Layer sein, wird den entsprechenden Objekten
	// der Default-Layer zugewiesen (Layer 0, (dokumentglobaler Standardlayer).

	GDIMetaFile     GetAllMarkedMetaFile(BOOL bNoVDevIfOneMtfMarked=FALSE) const { return GetMarkedObjMetaFile(bNoVDevIfOneMtfMarked); }


	// Bestimmung des View-Mittelpunktes, z.B. zum Pasten

	// Bei allen Paste-Methoden werden die neuen Draw-Objekte markiert.
	// Wird der Parameter bAddMark auf TRUE gesetzt, so werden die neuen
	// DrawObjekte zu einer bereits bestehenden Selektion "hinzumarkiert".
	// Dieser Fall ist fuer Drag&Drop mit mehreren Items gedacht.
	// Die Methoden mit Point-Parameter fuegen neue Objekte zentriert an
	// dieser Position ein, die anderen zentriert am 1.OutputDevice der View.
	// Ist der Parameter pPg gesetzt, werden die Objekte and dieser Seite
	// eingefuegt. Die Positionierung (rPos bzw. Zentrierung) bezieht sich
	// dann nichtmehr auf die View sondern auf die Page.
	// Hinweis: SdrObjList ist Basisklasse von SdrPage.
	// Die Methoden liefern TRUE, wenn die Objekte erfolgreich erzeugt und
	// eingefuegt wurden. Bei pLst=FALSE und kein TextEdit aktiv kann man
	// sich dann auch darauf verlassen, dass diese an der View markiert sind.
	// Andernfalls erfolgt die Markierung nur, wenn pLst z.Zt. auch an der
	// View angezeigt wird.
	// Gueltige Werte fuer nOptions sind SDRINSERT_DONTMARK und
	// SDRINSERT_ADDMARK (siehe svdedtv.hxx).
};

}//end of namespace binfilter
#endif //_SVDXCGV_HXX
