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



#ifndef _WMF_HXX
#define _WMF_HXX

#include <bf_svtools/bf_solar.h>

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _FLTCALL_HXX
#include <bf_svtools/fltcall.hxx>
#endif

namespace binfilter
{

BOOL ConvertWMFToGDIMetaFile( SvStream & rStreamWMF, GDIMetaFile & rGDIMetaFile, FilterConfigItem* pConfigItem = NULL );

 BOOL ReadWindowMetafile( SvStream& rStream, GDIMetaFile& rMTF, FilterConfigItem* pConfigItem );

 BOOL ConvertGDIMetaFileToWMF( const GDIMetaFile & rMTF, SvStream & rTargetStream, FilterConfigItem* pConfigItem = NULL, BOOL bPlaceable = TRUE );

BOOL ConvertGDIMetaFileToEMF( const GDIMetaFile & rMTF, SvStream & rTargetStream, FilterConfigItem* pConfigItem = NULL );

 BOOL WriteWindowMetafileBits( SvStream& rStream, const GDIMetaFile& rMTF );

}

#endif
