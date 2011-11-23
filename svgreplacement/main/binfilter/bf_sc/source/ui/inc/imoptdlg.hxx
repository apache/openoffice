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



#ifndef SC_IMOPTDLG_HXX
#define SC_IMOPTDLG_HXX

#ifndef _SV_DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif

#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _SV_COMBOBOX_HXX //autogen
#include <vcl/combobox.hxx>
#endif

#ifndef _SV_LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

#ifndef _SV_BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _SVX_TXENCBOX_HXX
#include <bf_svx/txencbox.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
namespace binfilter {

//===================================================================

class ScImportOptions;
class ScDelimiterTable;


//------------------------------------------------------------------------

class ScImportOptions
{
public:
		ScImportOptions()
            : nFieldSepCode(0),nTextSepCode(0),eCharSet(RTL_TEXTENCODING_DONTKNOW),bFixedWidth(FALSE)
		{}

		ScImportOptions( sal_Unicode nFieldSep, sal_Unicode nTextSep, const String& rStr )
            : nFieldSepCode(nFieldSep),nTextSepCode(nTextSep),aStrFont(rStr),bFixedWidth(FALSE)
		{ eCharSet = ScGlobal::GetCharsetValue(aStrFont); }

		ScImportOptions( sal_Unicode nFieldSep, sal_Unicode nTextSep, rtl_TextEncoding nEnc )
            : nFieldSepCode(nFieldSep),nTextSepCode(nTextSep),bFixedWidth(FALSE)
		{ SetTextEncoding( nEnc ); }

		ScImportOptions( const ScImportOptions& rCpy )
			: nFieldSepCode	(rCpy.nFieldSepCode),
			  nTextSepCode	(rCpy.nTextSepCode),
			  aStrFont		(rCpy.aStrFont),
              eCharSet      (rCpy.eCharSet),
              bFixedWidth   (rCpy.bFixedWidth)
		{}

	ScImportOptions& operator=( const ScImportOptions& rCpy )
						{
							nFieldSepCode = rCpy.nFieldSepCode;
							nTextSepCode  = rCpy.nTextSepCode;
							aStrFont	  = rCpy.aStrFont;
							eCharSet	  = rCpy.eCharSet;
                            bFixedWidth   = rCpy.bFixedWidth;
							return *this;
						}

	BOOL			 operator==( const ScImportOptions& rCmp )
						{
							return
								   nFieldSepCode == rCmp.nFieldSepCode
								&& nTextSepCode  == rCmp.nTextSepCode
								&& eCharSet		 == rCmp.eCharSet
                                && aStrFont      == rCmp.aStrFont
                                && bFixedWidth   == rCmp.bFixedWidth;
						}
	void	SetTextEncoding( rtl_TextEncoding nEnc );

    sal_Unicode nFieldSepCode;
    sal_Unicode nTextSepCode;
    String      aStrFont;
    CharSet     eCharSet;
    BOOL        bFixedWidth;
};


} //namespace binfilter
#endif // SC_IMOPTDLG_HXX



