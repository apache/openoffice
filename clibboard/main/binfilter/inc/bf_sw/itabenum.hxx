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


#ifndef _ITABENUM_HXX
#define _ITABENUM_HXX
namespace binfilter {

enum SwTableInsMode
{
	DEFAULT_BORDER     = 0x01,
	HEADLINE           = 0x02,
	REPEAT             = 0x04,
	HEADLINE_REPEAT    = 0x06,	// Headline + Repeat
	SPLIT_LAYOUT       = 0x08,
	HEADLINE_NO_BORDER = HEADLINE_REPEAT|SPLIT_LAYOUT,
	ALL_TBL_INS_ATTR   = DEFAULT_BORDER|HEADLINE_REPEAT|SPLIT_LAYOUT
};


} //namespace binfilter
#endif

