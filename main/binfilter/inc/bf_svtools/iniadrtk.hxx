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



#ifndef SVTOOLS_INIADRTK_HXX
#define SVTOOLS_INIADRTK_HXX

class String;

namespace binfilter
{

//============================================================================
enum AddressToken
{
	ADDRESS_COMPANY,
	ADDRESS_STREET,
	ADDRESS_COUNTRY,
	ADDRESS_PLZ,
	ADDRESS_CITY,
	ADDRESS_TITLE,
	ADDRESS_POSITION,
	ADDRESS_TEL_PRIVATE,
	ADDRESS_TEL_COMPANY,
	ADDRESS_FAX,
	ADDRESS_EMAIL,
	ADDRESS_STATE,
	ADDRESS_FATHERSNAME,
	ADDRESS_APARTMENT
};

}

#endif // SVTOOLS_INIADRTK_HXX

