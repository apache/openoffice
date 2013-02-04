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


#ifndef INCLUDED_SVTOOLS_UNDOOPT_HXX
#define INCLUDED_SVTOOLS_UNDOOPT_HXX

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

namespace binfilter
{

class SvtUndoOptions_Impl;

class  SvtUndoOptions:
    public Options, public SfxBroadcaster, public SfxListener
{
    SvtUndoOptions_Impl*    pImp;

public:
                            SvtUndoOptions();
                            virtual ~SvtUndoOptions();

    virtual void            Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
};

}

#endif

