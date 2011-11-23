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


#ifndef _SFX_PRINTER_HXX
#define _SFX_PRINTER_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _PRINT_HXX //autogen
#include <vcl/print.hxx>
#endif
namespace binfilter {
class SfxItemSet;

class SfxFont;
class SfxTabPage;

struct SfxPrinter_Impl;

#define SFX_RANGE_NOTSET	((USHORT)0xFFFF)

#define SFX_PRINTER_PRINTER			 1 	// ohne JOBSETUP => temporaer
#define SFX_PRINTER_JOBSETUP    	 2
#define SFX_PRINTER_OPTIONS      	 4
#define SFX_PRINTER_CHG_ORIENTATION  8
#define SFX_PRINTER_CHG_SIZE 		16
#define SFX_PRINTER_ALL         	31

#define SFX_PRINTER_CHG_ORIENTATION_FLAG  3
#define SFX_PRINTER_CHG_SIZE_FLAG 		  4

// class SfxFontSizeInfo -------------------------------------------------


// class SfxFont ---------------------------------------------------------

class SfxFont
{
private:
	String					aName;
	FontFamily				eFamily;
	FontPitch				ePitch;
	CharSet 				eCharSet;

	SfxFont&				operator=(const SfxFont& rFont); // not implemented

public:
	SfxFont( const FontFamily eFam,
			 const String& aName,
			 const FontPitch eFontPitch = PITCH_DONTKNOW,
			 const CharSet eFontCharSet = RTL_TEXTENCODING_DONTKNOW );
	// ZugriffsMethoden:
	inline const String&	GetName() const { return aName; }
	inline FontFamily		GetFamily() const { return eFamily; }
	inline FontPitch		GetPitch() const { return ePitch; }
	inline CharSet			GetCharSet() const { return eCharSet; }
};

// class SfxPrinter ------------------------------------------------------

class SfxPrinter : public Printer
{
private:
	JobSetup				aOrigJobSetup;
	SfxItemSet*				pOptions;
	SfxPrinter_Impl*		pImpl;
	BOOL					bKnown;


public:
							SfxPrinter( SfxItemSet *pTheOptions );
							SfxPrinter( SfxItemSet *pTheOptions,
										const String &rPrinterName );
							SfxPrinter( SfxItemSet *pTheOptions,
										const JobSetup &rTheOrigJobSetup );
							SfxPrinter( SfxItemSet *pTheOptions,
										const String &rPrinterName,
										const JobSetup &rTheOrigJobSetup );
							~SfxPrinter();


	static SfxPrinter*		Create( SvStream &rStream, SfxItemSet *pOptions );
	SvStream&				Store( SvStream &rStream ) const;


	const SfxItemSet&		GetOptions() const { return *pOptions; }
	void					SetOptions( const SfxItemSet &rNewOptions );


	BOOL					IsKnown() const { return bKnown; }
	BOOL					IsOriginal() const { return bKnown; }

	const SfxFont*          GetFontByName( const String &rFontName );

};

}//end of namespace binfilter
#endif
