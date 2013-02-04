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



#ifndef _FRMIDS_HXX
#define	_FRMIDS_HXX

// Aktuelle Version:
namespace binfilter {

#define	FRMVER_NEWFLY	0x0002		// Neue Ablage der absatzgebundenen Flys
#define	FRMVER_EXTINFO	0x0003		// Optionales Extended-Info-Byte
#define FRMVER_NEWALIGN 0x0004		// Alignment wurde Layoutseitig geaendert
									// es muss geeignet invalidiert werden.

#define	FRM_VERSION		0x0004

// ID-Byte als 1. Byte:

#define	FRMID_ROOT		'R'			// Root-Frame
#define	FRMID_PAGE		'P'			// Seite
#define	FRMID_HEADER	'H'			// Kopfzeilen
#define	FRMID_FOOTER	'F'			// Fusszeilen
#define	FRMID_BODY		'B'			// Arbeitsbereich
#define	FRMID_FTNCONT	'C'			// Fussnoten-Container
#define	FRMID_FTN		'N'			// Fussnote
#define	FRMID_TEXT 		'X'			// Text-Frame
#define	FRMID_GRAPHICS	'G'			// Grafik-Frame
#define	FRMID_OLE		'O'			// OLE-Frame (reserviert)
#define	FRMID_ATCNTNT	'A'			// am Inhalt gebundener Rahmen
#define	FRMID_INCNTNT	'I'			// im Inhalt gebundener Rahmen
#define	FRMID_FLYLAY	'L'			// Seiten/spaltengebundener Rahmen
#define	FRMID_COLUMN	'M'			// Spalten-Frame
#define	FRMID_TABLE		'T'			// Tabellen-Frame
#define	FRMID_TABROW	'r'			// Tabellen-Zeile
#define	FRMID_TABCELL	'c'			// Tabellen-Zelle

// Flag-Bits im 2. Byte:

#define	FRMF_PRINT		0x01		// Frame hat eigenen Print-Rahmen
#define	FRMF_HASFLY		0x02		// Frame hat FlyFrames
#define	FRMF_HASLOWER	0x04		// Frame hat Lower Frames
#define	FRMF_HASFOLLOW	0x08		// Frame *hat* Follow-Frames
#define	FRMF_ISFOLLOW	0x10		// Frame *ist* Follow-Frame
#define	FRMF_SECTID1	0x20		// Section-ID ist einstellig
#define	FRMF_NODEID1	0x40		// Node-ID ist einstellig
#define	FRMF_HASID		0x80		// Frame hat eigenen ID

// Zusatz-Bit, vom Reader in nFrmFlags zu setzen

#define	FRMF_INVALID	0x8000		// Frames ungueltig setzen


} //namespace binfilter
#endif
