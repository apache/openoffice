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


#ifndef _BODYFRM_HXX
#define _BODYFRM_HXX

#ifndef _SVMEMPOOL_HXX //autogen
#include <tools/mempool.hxx>
#endif

#include "layfrm.hxx"
namespace binfilter {

class SwBorderAttrs;

class SwBodyFrm: public SwLayoutFrm
{
protected:
	virtual void Format( const SwBorderAttrs *pAttrs = 0 );

#if defined ( GCC) && defined ( C272 )
	~SwBodyFrm();
#endif
public:
	SwBodyFrm( SwFrmFmt* );

	DECL_FIXEDMEMPOOL_NEWDEL(SwBodyFrm)
};

} //namespace binfilter
#endif	//_BODYFRM_HXX
