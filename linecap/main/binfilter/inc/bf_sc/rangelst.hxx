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



#ifndef SC_RANGELST_HXX
#define SC_RANGELST_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
namespace binfilter {

class ScDocument;

typedef ScRange* ScRangePtr;
DECLARE_LIST( ScRangeListBase, ScRangePtr )
class ScRangeList : public ScRangeListBase, public SvRefBase
{
public:
					ScRangeList() {}
					ScRangeList( const ScRangeList& rList );
	virtual 		~ScRangeList();
	ScRangeList&	operator=(const ScRangeList& rList);
	void			RemoveAll();
	void			Append( const ScRange& rRange )
					{
						ScRangePtr pR = new ScRange( rRange );
						Insert( pR, LIST_APPEND );
					}
	USHORT			Parse( const String&, ScDocument* = NULL,
							USHORT nMask = SCA_VALID );
	void 			Format( String&, USHORT nFlags = 0, ScDocument* = NULL ) const;
	void			Join( const ScRange&, BOOL bIsInList = FALSE );
	BOOL 			UpdateReference( UpdateRefMode, ScDocument*,
									const ScRange& rWhere,
									short nDx, short nDy, short nDz );
/*N*/ 	BOOL			Load( SvStream&, USHORT nVer );
/*N*/ 	BOOL			Store( SvStream& ) const;
};
SV_DECL_IMPL_REF( ScRangeList );


// RangePairList: erster Range (aRange[0]) eigentlicher Range, zweiter
// Range (aRange[1]) Daten zu diesem Range, z.B. Rows eines ColName
DECLARE_LIST( ScRangePairListBase, ScRangePair* )
class ScRangePairList : public ScRangePairListBase, public SvRefBase
{
#if defined( ICC ) && defined( OS2 )
#endif
private:

public:
	virtual 		~ScRangePairList();
	ScRangePairList*	Clone() const;
	void			Append( const ScRangePair& rRangePair )
					{
						ScRangePair* pR = new ScRangePair( rRangePair );
						Insert( pR, LIST_APPEND );
					}
	void			Join( const ScRangePair&, BOOL bIsInList = FALSE );
	BOOL 			UpdateReference( UpdateRefMode, ScDocument*,
									const ScRange& rWhere,
									short nDx, short nDy, short nDz );
	ScRangePair*	Find( const ScRange& ) const;
/*N*/ 	BOOL			Load( SvStream&, USHORT nVer );
/*N*/ 	BOOL			Store( SvStream& ) const;
};
SV_DECL_IMPL_REF( ScRangePairList );

#if defined( ICC ) && defined( SC_RANGELST_CXX ) && defined( OS2 )
#endif


} //namespace binfilter
#endif
