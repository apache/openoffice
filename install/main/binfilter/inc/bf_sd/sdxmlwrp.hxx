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


#ifndef _SDXMLWRP_HXX
#define _SDXMLWRP_HXX

#include "sdfilter.hxx"
namespace binfilter {

// ---------------
// - SdXMLFilter -
// ---------------

enum SdXMLFilterMode
{
	SDXMLMODE_Normal,			// standard load and save of the complete document
	SDXMLMODE_Preview,			// only for import, only the first draw page and its master page is loaded
	SDXMLMODE_Organizer			// only for import, only the styles are loaded
};

class SdXMLFilter : public SdFilter
{
private:
	SdXMLFilterMode meFilterMode;

public:
	
							SdXMLFilter( SfxMedium& rMedium, SdDrawDocShell& rDocShell, sal_Bool bShowProgress, SdXMLFilterMode eFilterMode = SDXMLMODE_Normal );
							~SdXMLFilter();

	virtual sal_Bool		Import();
	virtual sal_Bool		Export();
};

} //namespace binfilter
#endif	// _SDXMLWRP_HXX
