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


#ifndef _FLTINI_HXX
#define _FLTINI_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif
namespace binfilter {

class SwNumRuleTbl;
class SwDoc;
class SwTxtNode;
class SwNodeNum;
class SwNumRule;
class SwNodeIndex;


// die speziellen Reader

class Sw6Reader: public Reader
{
    virtual ULONG Read( SwDoc &,SwPaM &,const String &);
};

 class W4WReader: public StgReader
 {
    String sVersion;
    USHORT nFilter;
    BOOL bStorageFlag;
    virtual ULONG Read(SwDoc &,SwPaM &,const String &);
    // wir wollen die Streams / Storages nicht geoeffnet haben
    virtual int SetStrmStgPtr();
 public:
    W4WReader() : StgReader(), nFilter(0), bStorageFlag(FALSE) {}
    virtual int GetReaderType();
    virtual void SetFltName( const String& rFltName );

    USHORT GetFilter() const { return nFilter; }
    const String& GetVersion() const { return sVersion; }
 };

#ifdef DEBUG_SH

class Internal_W4WReader: public W4WReader
{
	virtual ULONG Read(SwDoc &,SwPaM &,const String &);
};

#endif

class ExcelReader : public StgReader
{
	virtual ULONG Read(SwDoc &,SwPaM &,const String &);
public:
	virtual int GetReaderType();
};

class LotusReader : public Reader
{
	CharSet eCodeSet;
	virtual ULONG Read(SwDoc &,SwPaM &,const String &);
public:
};




// die speziellen Writer

#if 0
/*?*/ WriterRef GetW4WWriter( const String& );
WriterRef GetStgWriter( const String& );
WriterRef GetWWWriter( const String& );
/*?*/ WriterRef GetWW8Writer( const String& );
#if !( !defined(DBG_UTIL) || defined(MAC) || defined(PM2) )
/*?*/ WriterRef GetDebugWriter( const String& );
/*?*/ WriterRef GetUndoWriter( const String& );
#endif
#else
/*?*/ void GetW4WWriter( const String&, WriterRef& );
void GetStgWriter( const String&, WriterRef& );
void GetWWWriter( const String&, WriterRef& );
#if !( !defined(DBG_UTIL) || defined(MAC) || defined(PM2) )
#endif
#endif


// JP 17.03.99 - 63049
// Umsetzen der LRSpaces im aktuell importierten Doc. Die Fremd-Filter
// liefern immer absolute Werte fuer die Ebenen einer NumRule. Wir
// verarbeiten jetzt aber relative Werte bezogen auf das LR-Space-Item.
// Das hat zur Folge, das bei allen Absaetzen die EInzuege der NumRule vom
// Absatz-Einzug abgezogen werden muss.
class SwRelNumRuleSpaces
{
    SwNumRuleTbl* pNumRuleTbl;  // Liste aller benannten NumRules
    BOOL bNewDoc;

    void SetNumLSpace( SwTxtNode& rNd, const SwNumRule& rRule );

public:
    SwRelNumRuleSpaces( SwDoc& rDoc, BOOL bNewDoc );
    ~SwRelNumRuleSpaces();

    void SetNumRelSpaces( SwDoc& rDoc );
    void SetOultineRelSpaces( const SwNodeIndex& rStt,
                                const SwNodeIndex& rEnd );
};

#define SW_SV_BRUSH_25     		0
#define SW_SV_BRUSH_50          1
#define SW_SV_BRUSH_75          2
#define SW_SV_BRUSH_NULL        3
#define SW_SV_BRUSH_SOLID       4
#define SW_SV_BRUSH_INVALID     5

Color	ConvertBrushStyle(const Color& rCol, const Color& rFillCol, BYTE nStyle);

// Get size of fly (if 'automatic' in WW) and check if not too small
void	CalculateFlySize( SfxItemSet& rFlySet, SwNodeIndex& rAnchor,
						  SwTwips nPageWidth );

} //namespace binfilter
#endif
