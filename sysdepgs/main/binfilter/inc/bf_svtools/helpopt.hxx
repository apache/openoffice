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


#ifndef INCLUDED_SVTOOLS_HELPOPT_HXX
#define INCLUDED_SVTOOLS_HELPOPT_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef INCLUDED_LIST
#include <list>
#define INCLUDED_LIST
#endif

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef INCLUDED_SVTOOLS_OPTIONS_HXX
#include <bf_svtools/options.hxx>
#endif

namespace binfilter
{

typedef std::list< sal_Int32 > IdList;

class SvtHelpOptions_Impl;

class  SvtHelpOptions: public Options
{
    SvtHelpOptions_Impl*    pImp;

public:
                    SvtHelpOptions();
                    virtual ~SvtHelpOptions();
};

}

#endif

