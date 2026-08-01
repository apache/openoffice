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

// Regression test for WW8PLCF::IsValidLength, the length guard shared by
// WW8PLCF::ReadPLCF and the WW8PLCFspecial constructor (ww8scan.cxx).

#include "gtest/gtest.h"

#include "ww8scan.hxx"     // WW8PLCF::IsValidLength (inline, no filter link)
#include "ww8struc.hxx"    // WW8_CP_MAX
#include <sal/types.h>

// Non-positive count must be rejected.
TEST(WW8Plcf, IsValidLengthRejectsNegativeAndZero)
{
    EXPECT_FALSE( WW8PLCF::IsValidLength( -1 ) );          // 0xFFFFFFFF narrowed
    EXPECT_FALSE( WW8PLCF::IsValidLength( 0 ) );
    EXPECT_FALSE( WW8PLCF::IsValidLength( -64 ) );
    EXPECT_FALSE( WW8PLCF::IsValidLength( SAL_MIN_INT32 ) );
}

// Lengths close to the maximum must be rejected too
TEST(WW8Plcf, IsValidLengthRejectsOverflowingUpperRange)
{
    EXPECT_FALSE( WW8PLCF::IsValidLength( WW8_CP_MAX ) );
    EXPECT_FALSE( WW8PLCF::IsValidLength( WW8_CP_MAX - 1 ) );
    EXPECT_FALSE( WW8PLCF::IsValidLength( WW8_CP_MAX - 3 ) );
}

// Genuine lengths are accepted
TEST(WW8Plcf, IsValidLengthAcceptsBenignLengths)
{
    EXPECT_TRUE( WW8PLCF::IsValidLength( 1 ) );
    EXPECT_TRUE( WW8PLCF::IsValidLength( 12 ) );
    EXPECT_TRUE( WW8PLCF::IsValidLength( 64 ) );
    EXPECT_TRUE( WW8PLCF::IsValidLength( WW8_CP_MAX - 4 ) );   // largest accepted
}

// For every accepted length, the element count both readers allocate is
// large enough to hold the nPLCF bytes the subsequent Read copies in.
TEST(WW8Plcf, AcceptedLengthSizesBufferToHoldRead)
{
    const sal_Int32 aLens[] = { 1, 3, 4, 5, 12, 64, 4096, WW8_CP_MAX - 4 };
    for ( size_t i = 0; i < sizeof(aLens) / sizeof(aLens[0]); ++i )
    {
        const sal_Int32 nPLCF = aLens[i];
        ASSERT_TRUE( WW8PLCF::IsValidLength( nPLCF ) );
        // Done in 64-bit to model the allocation without itself overflowing.
        const sal_Int64 nElems         = ( static_cast<sal_Int64>(nPLCF) + 3 ) / 4;
        const sal_Int64 nCapacityBytes = nElems * 4;   // sizeof(WW8_CP) == 4
        EXPECT_GT( nElems, 0 );
        EXPECT_GE( nCapacityBytes, static_cast<sal_Int64>(nPLCF) );
    }
}

int main( int argc, char** argv )
{
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
