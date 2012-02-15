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


#ifndef INCLUDED_SVTOOLS_SAVEOPT_HXX
#define INCLUDED_SVTOOLS_SAVEOPT_HXX

#include <sal/types.h>
#include <bf_svtools/options.hxx>

namespace binfilter
{

struct SvtLoadSaveOptions_Impl;
class  SvtSaveOptions: public Options
{
    SvtLoadSaveOptions_Impl*    pImp;

public:

    enum EOption
    {
        E_AUTOSAVETIME,
        E_USEUSERDATA,
        E_BACKUP,
        E_AUTOSAVE,
        E_AUTOSAVEPROMPT,
        E_DOCINFSAVE,
        E_SAVEWORKINGSET,
        E_SAVEDOCWINS,
        E_SAVEDOCVIEW,
        E_SAVERELINET,
        E_SAVERELFSYS,
        E_SAVEUNPACKED,
        E_DOPRETTYPRINTING,
        E_WARNALIENFORMAT,
        E_LOADDOCPRINTER
    };
                            SvtSaveOptions();
                            virtual ~SvtSaveOptions();

    sal_Bool                IsBackup() const;

    sal_Bool                IsSaveDocView() const;

    sal_Bool                IsSaveRelINet() const;

    sal_Bool                IsSaveRelFSys() const;

	sal_Bool				IsSaveUnpacked() const;

    sal_Bool                IsLoadUserSettings() const;

	sal_Bool				IsPrettyPrinting( ) const;
};

}
#endif

