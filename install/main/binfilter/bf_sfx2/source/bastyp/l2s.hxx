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


#ifndef _SFXL2L_HXX
#define _SFXL2L_HXX
namespace binfilter {

inline int L2S( StreamOpenMode eMode )
{
	int nMode = 0;
	switch ( eMode )
	{
		case BF_READ:   nMode = O_RDONLY; break;
		case BF_WRITE:  nMode = O_RDWR;   break;
		case BF_CREATE: nMode = O_CREAT;  break;
		case BF_TRUNC:  nMode = O_TRUNC;  break;
	}
	return nMode;
}

}//end of namespace binfilter
#endif
