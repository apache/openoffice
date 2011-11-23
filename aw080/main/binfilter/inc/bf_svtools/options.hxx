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



#ifndef INCLUDED_SVTOOLS_OPTIONS_HXX
#define INCLUDED_SVTOOLS_OPTIONS_HXX

#ifndef _SAL_CONFIG_H_
#include "sal/config.h"
#endif

namespace binfilter
{

// A base class for the various option classes supported by
// svtools/source/config/itemholderbase.hxx (which must be public, as it is
// shared between svl and svt):
class  Options {
public:
    Options();

    virtual ~Options() = 0;

private:
     Options(Options &); // not defined
     void operator =(Options &); // not defined
};

}

#endif
