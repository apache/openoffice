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


#ifndef _SVTOOLS_CTLOPTIONS_HXX
#define _SVTOOLS_CTLOPTIONS_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _SFXBRDCST_HXX
#include <bf_svtools/brdcst.hxx>
#endif
#ifndef _SFXLSTNER_HXX
#include <bf_svtools/lstner.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_OPTIONS_HXX
#include <bf_svtools/options.hxx>
#endif

namespace binfilter {

class SvtCTLOptions_Impl;

// class SvtCTLOptions --------------------------------------------------------

class  SvtCTLOptions:
    public Options, public SfxBroadcaster, public SfxListener
{
private:
    SvtCTLOptions_Impl*    m_pImp;

public:

    // bDontLoad is for referencing purposes only
    SvtCTLOptions( sal_Bool bDontLoad = sal_False );
    virtual ~SvtCTLOptions();

    virtual void        Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    sal_Bool		IsCTLFontEnabled() const;

    sal_Bool		IsCTLSequenceChecking() const;

    enum CursorMovement
	{
		MOVEMENT_LOGICAL = 0,
		MOVEMENT_VISUAL
	};

	enum TextNumerals
	{
		NUMERALS_ARABIC = 0,
		NUMERALS_HINDI,
		NUMERALS_SYSTEM
	};
    TextNumerals	GetCTLTextNumerals() const;

    enum EOption
    {
        E_CTLFONT,
        E_CTLSEQUENCECHECKING,
        E_CTLCURSORMOVEMENT,
        E_CTLTEXTNUMERALS,
        E_CTLSEQUENCECHECKINGRESTRICTED,
        E_CTLSEQUENCECHECKINGTYPEANDREPLACE
    };
};

}

#endif // _SVTOOLS_CTLOPTIONS_HXX

