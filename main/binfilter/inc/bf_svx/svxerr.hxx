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


#ifndef _SVXERR_HXX
#define _SVXERR_HXX

// include ---------------------------------------------------------------


// define ----------------------------------------------------------------




#define ERRCODE_SVX_GRAPHIC_WRONG_FILEFORMAT   (8UL | ERRCODE_AREA_SVX | \
												ERRCODE_CLASS_READ )
#define ERRCODE_SVX_FORMS_NOIOSERVICES		   (10UL | ERRCODE_AREA_SVX )
#define ERRCODE_SVX_FORMS_READWRITEFAILED	   (11UL | ERRCODE_AREA_SVX )

#define ERRCODE_SVX_BULLETITEM_NOBULLET        (12UL | ERRCODE_AREA_SVX )


#define ERRCODE_SVX_VBASIC_STORAGE_EXIST       (14UL | ERRCODE_AREA_SVX     \
													 | ERRCODE_WARNING_MASK \
													 | ERRCODE_CLASS_WRITE )

// both codes will be used twice : with ERRCODE_CLASS_READ- and ERRCODE_CLASS_WRITE-bits

// class SvxErrorHandler -------------------------------------------------

#ifndef __RSC

#ifndef _EHDL_HXX //autogen
#include <bf_svtools/ehdl.hxx>
#endif
namespace binfilter {
class SvxErrorHandler : private SfxErrorHandler
{
  public:
	SvxErrorHandler();
};

}//end of namespace binfilter
#endif

#endif

