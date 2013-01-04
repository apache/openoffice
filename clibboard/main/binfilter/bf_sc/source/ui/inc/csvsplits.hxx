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



// ============================================================================

#ifndef _SC_CSVSPLITS_HXX
#define _SC_CSVSPLITS_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#include <vector>
namespace binfilter {


// ============================================================================

/** Constant for an invalid vector index. */
const sal_uInt32 CSV_VEC_NOTFOUND   = ~0UL;
/** Constant for an invalid ruler position. */
const sal_Int32 CSV_POS_INVALID     = -1;


// ----------------------------------------------------------------------------

/** A vector of column splits that supports inserting, removing and moving splits. */
class ScCsvSplits
{
private:
    typedef ::std::vector< sal_Int32 >      ScSplitVector;
    typedef ScSplitVector::iterator         iterator;
    typedef ScSplitVector::const_iterator   const_iterator;

    ScSplitVector               maVec;          /// The split containter.

};


// ============================================================================

} //namespace binfilter
#endif

