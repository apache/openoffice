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



#ifndef SC_INPUTOPT_HXX
#define SC_INPUTOPT_HXX


#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif

#include <bf_svtools/bf_solar.h>

class SvStream;

namespace binfilter {

class ScInputOptions
{
private:
	USHORT		nMoveDir;			// enum ScDirection
	BOOL		bMoveSelection;
	BOOL		bEnterEdit;
	BOOL		bExtendFormat;
	BOOL		bRangeFinder;
	BOOL		bExpandRefs;
	BOOL		bMarkHeader;
	BOOL		bUseTabCol;
	BOOL		bTextWysiwyg;
    BOOL        bReplCellsWarn;

public:
				ScInputOptions();
				ScInputOptions( const ScInputOptions& rCpy );
				~ScInputOptions();

	void		SetDefaults();

    void        SetMoveDir(USHORT nNew)         { nMoveDir = nNew;       }
    USHORT      GetMoveDir() const              { return nMoveDir;       }
    void        SetMoveSelection(BOOL bSet)     { bMoveSelection = bSet; }
    BOOL        GetMoveSelection() const        { return bMoveSelection; }
    void        SetEnterEdit(BOOL bSet)         { bEnterEdit = bSet;     }
    BOOL        GetEnterEdit() const            { return bEnterEdit;     }
    void        SetExtendFormat(BOOL bSet)      { bExtendFormat = bSet;  }
    BOOL        GetExtendFormat() const         { return bExtendFormat;  }
    void        SetRangeFinder(BOOL bSet)       { bRangeFinder = bSet;   }
    BOOL        GetRangeFinder() const          { return bRangeFinder;   }
    void        SetExpandRefs(BOOL bSet)        { bExpandRefs = bSet;    }
    BOOL        GetExpandRefs() const           { return bExpandRefs;    }
    void        SetMarkHeader(BOOL bSet)        { bMarkHeader = bSet;    }
    BOOL        GetMarkHeader() const           { return bMarkHeader;    }
    void        SetUseTabCol(BOOL bSet)         { bUseTabCol = bSet;     }
    BOOL        GetUseTabCol() const            { return bUseTabCol;     }
    void        SetTextWysiwyg(BOOL bSet)       { bTextWysiwyg = bSet;   }
    BOOL        GetTextWysiwyg() const          { return bTextWysiwyg;   }
    void        SetReplaceCellsWarn(BOOL bSet)  { bReplCellsWarn = bSet; }
    BOOL        GetReplaceCellsWarn() const     { return bReplCellsWarn; }

	const ScInputOptions&	operator=	( const ScInputOptions& rOpt );
/*N*/ 	friend SvStream& 		operator>>	( SvStream& rStream, ScInputOptions& rOpt );
/*N*/ 	friend SvStream&		operator<<	( SvStream& rStream, const ScInputOptions& rOpt );
};


//==================================================================
// CfgItem fuer Eingabe-Optionen
//==================================================================

class ScInputCfg : public ScInputOptions,
	public ::utl::ConfigItem
{
	::com::sun::star::uno::Sequence<rtl::OUString> GetPropertyNames();

public:
			ScInputCfg();

    virtual void                Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );
    virtual void                Commit();

	void			SetOptions( const ScInputOptions& rNew ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 	void			SetOptions( const ScInputOptions& rNew );
};


} //namespace binfilter
#endif

