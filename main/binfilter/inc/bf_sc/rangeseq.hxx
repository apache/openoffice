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



#ifndef SC_RANGESEQ_HXX
#define SC_RANGESEQ_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif
namespace binfilter {

class SvNumberFormatter;
class ScDocument;
class ScRange;
class ScMatrix;

class ScRangeToSequence
{
public:
	static BOOL	FillLongArray( ::com::sun::star::uno::Any& rAny,
								ScDocument* pDoc, const ScRange& rRange );
	static BOOL	FillLongArray( ::com::sun::star::uno::Any& rAny,
								const ScMatrix* pMatrix );
	static BOOL	FillDoubleArray( ::com::sun::star::uno::Any& rAny,
								ScDocument* pDoc, const ScRange& rRange );
	static BOOL	FillDoubleArray( ::com::sun::star::uno::Any& rAny,
								const ScMatrix* pMatrix );
	static BOOL	FillStringArray( ::com::sun::star::uno::Any& rAny,
								ScDocument* pDoc, const ScRange& rRange );
	static BOOL	FillStringArray( ::com::sun::star::uno::Any& rAny,
								const ScMatrix* pMatrix, SvNumberFormatter* pFormatter );
	static BOOL	FillMixedArray( ::com::sun::star::uno::Any& rAny,
								ScDocument* pDoc, const ScRange& rRange,
								BOOL bAllowNV = FALSE );
	static BOOL	FillMixedArray( ::com::sun::star::uno::Any& rAny,
								const ScMatrix* pMatrix );
};



} //namespace binfilter
#endif

