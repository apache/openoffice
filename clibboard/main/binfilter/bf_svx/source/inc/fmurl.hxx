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



#ifndef _SVX_FMURL_HXX
#define _SVX_FMURL_HXX

#ifndef _FM_STATIC_HXX_
#include "fmstatic.hxx"
#endif
namespace binfilter {

namespace svxform
{

	DECLARE_CONSTASCII_USTRING(FMURL_FORMSLOTS_PREFIX);

	DECLARE_CONSTASCII_USTRING(FMURL_RECORD_MOVEFIRST);
	DECLARE_CONSTASCII_USTRING(FMURL_RECORD_MOVEPREV);
	DECLARE_CONSTASCII_USTRING(FMURL_RECORD_MOVENEXT);
	DECLARE_CONSTASCII_USTRING(FMURL_RECORD_MOVELAST);
	DECLARE_CONSTASCII_USTRING(FMURL_RECORD_MOVETONEW);
	DECLARE_CONSTASCII_USTRING(FMURL_RECORD_UNDO);

	DECLARE_CONSTASCII_USTRING(FMURL_CONFIRM_DELETION);

	DECLARE_CONSTASCII_USTRING(FMURL_COMPONENT_FORMGRIDVIEW);
	DECLARE_CONSTASCII_USTRING(FMURL_GRIDVIEW_CLEARVIEW);
	DECLARE_CONSTASCII_USTRING(FMURL_GRIDVIEW_ADDCOLUMN);
	DECLARE_CONSTASCII_USTRING(FMURL_GRIDVIEW_ATTACHTOFORM);

	DECLARE_CONSTASCII_USTRING(FMARG_ATTACHTO_MASTERFORM);
	DECLARE_CONSTASCII_USTRING(FMARG_ADDCOL_COLUMNTYPE);
	DECLARE_CONSTASCII_USTRING(FMARG_ADDCOL_COLUMNPOS);

}	// namespace svxform

}//end of namespace binfilter
#endif // _SVX_FMURL_HXX

