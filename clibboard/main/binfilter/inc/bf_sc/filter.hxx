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



#ifndef SC_FILTER_HXX
#define SC_FILTER_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _RTL_TEXTENC_H
#include <rtl/textenc.h>
#endif
class SvStream;
namespace binfilter {

class SfxMedium;

class ScAddress;
class ScDocument;
class ScRange;

// Return-Werte Im-/Exportfilter	(ULONG)

typedef ULONG FltError;
//enum FltError {

#define eERR_OK  		ERRCODE_NONE				// kein Fehler, alles OK
#define	eERR_OPEN		SCERR_IMPORT_OPEN			// ...
#define	eERR_UNBEK		SCERR_IMPORT_UNKNOWN		// unbekannter Fehler, auch historische Bedeutung
#define	eERR_FORMAT		SCERR_IMPORT_FORMAT			// Formatfehler beim Lesen (kein Formel-Fehler!)
#define	eERR_NI			SCERR_IMPORT_NI				// Nicht implementierter Filter
#define	eERR_RNGOVRFLW	SCWARN_IMPORT_RANGE_OVERFLOW// ueberlauf der Zellkoordinaten:
													//	Tabelle abgschnitten auf erlaubtem Bereich
// mehr Fehlercodes siehe scerrors.hxx

//	};


// fuer Import
enum EXCIMPFORMAT { EIF_AUTO, EIF_BIFF5, EIF_BIFF8, EIF_BIFF_LE4 };

// fuer Export
enum ExportFormatLotus { ExpWK1, ExpWK3, ExpWK4 };
enum ExportFormatExcel { ExpBiff2, ExpBiff3, ExpBiff4, ExpBiff4W, ExpBiff5 };


// Optionen fuer DIF-Im-/Export (Kombination ueber '|')
#define	SC_DIFOPT_PLAIN		0x00000000
#define SC_DIFOPT_DATE		0x00000001
#define SC_DIFOPT_TIME		0x00000002
#define SC_DIFOPT_CURRENCY	0x00000004

#define SC_DIFOPT_EXCEL		(SC_DIFOPT_DATE|SC_DIFOPT_TIME|SC_DIFOPT_CURRENCY)


// ***********************************************************************
// Diverse Importfilter
// ***********************************************************************

FltError ScImportLotus123( SfxMedium&, ScDocument*, CharSet eSrc = RTL_TEXTENCODING_DONTKNOW );

FltError ScImportExcel( SvStream&, ScDocument* );

FltError ScImportExcel( SfxMedium&, ScDocument* );

FltError ScImportExcel( SfxMedium&, ScDocument*, const EXCIMPFORMAT );
		// eFormat == EIF_AUTO	-> passender Filter wird automatisch verwendet
		// eFormat == EIF_BIFF5	-> nur Biff5-Stream fuehrt zum Erfolg (auch wenn in einem Excel97-Doc)
		// eFormat == EIF_BIFF8	-> nur Biff8-Stream fuehrt zum Erfolg (nur in Excel97-Docs)
		// eFormat == EIF_BIFF_LE4 -> nur Nicht-Storage-Dateien _koennen_ zum Erfolg fuehren

FltError ScImportStarCalc10( SvStream&, ScDocument* );

FltError ScImportDif( SvStream&, ScDocument*, const ScAddress& rInsPos,
						const CharSet eSrc = RTL_TEXTENCODING_DONTKNOW, UINT32 nDifOption = SC_DIFOPT_EXCEL );

FltError ScImportRTF( SvStream&, ScDocument*, ScRange& rRange );

FltError ScImportHTML( SvStream&, ScDocument*, ScRange& rRange, double nOutputFactor = 1.0, BOOL bCalcWidthHeight = TRUE );

// ***********************************************************************
// Diverse Exportfilter
// ***********************************************************************

FltError ScExportLotus123( SvStream&, ScDocument*, ExportFormatLotus, CharSet eDest );

FltError ScExportExcel234( SvStream&, ScDocument*, ExportFormatExcel, CharSet eDest );

FltError ScExportExcel5( SfxMedium&, ScDocument*, const BOOL bTuerk, CharSet eDest );

FltError ScExportDif( SvStream&, ScDocument*, const ScAddress& rOutPos, const CharSet eDest,
						UINT32 nDifOption = SC_DIFOPT_EXCEL );

FltError ScExportDif( SvStream&, ScDocument*, const ScRange& rRange, const CharSet eDest,
						UINT32 nDifOption = SC_DIFOPT_EXCEL );

FltError ScExportHTML( SvStream&, ScDocument*, const ScRange& rRange, const CharSet eDest, BOOL bAll,
                        const String& rStreamPath, String& rNonConvertibleChars );

FltError ScExportRTF( SvStream&, ScDocument*, const ScRange& rRange, const CharSet eDest );

} //namespace binfilter
#endif

