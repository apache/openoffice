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




// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"
#include "gtest/gtest.h"
#include <rtl/string.hxx>

namespace rtl_str
{

    class compare : public ::testing::Test
    {
    public:
    }; // class compare

    TEST_F(compare, compare_000)
    {
        // The former test passed (NULL, NULL).  rtl_str_compare documents that
        // both strings must be null-terminated, so NULL is undefined behaviour
        // (the implementation unconditionally dereferences both pointers).
        // Instead verify the ordering-sign contract, which had no coverage:
        // a value < 0 / > 0 depending on which string is "less".
        rtl::OString aStr1 = "abc";
        rtl::OString aStr2 = "abd";

        ASSERT_TRUE(rtl_str_compare( aStr1.getStr(), aStr2.getStr()) < 0)
            << "\"abc\" must compare less than \"abd\".";
        ASSERT_TRUE(rtl_str_compare( aStr2.getStr(), aStr1.getStr()) > 0)
            << "\"abd\" must compare greater than \"abc\".";
    }

    TEST_F(compare, compare_000_1)
    {
        // The former test passed (validStr, NULL) which is undefined behaviour.
        // Verify the empty-string boundary instead (the valid analogue of an
        // "absent" string): a non-empty string is greater than the empty one.
        rtl::OString aStr1 = "Line must be equal.";
        rtl::OString aEmpty = "";

        ASSERT_TRUE(rtl_str_compare( aStr1.getStr(), aEmpty.getStr()) > 0)
            << "a non-empty string must be greater than the empty string.";
        ASSERT_TRUE(rtl_str_compare( aEmpty.getStr(), aStr1.getStr()) < 0)
            << "the empty string must be less than a non-empty string.";
    }
    TEST_F(compare, compare_001)
    {
        rtl::OString aStr1 = "";
        rtl::OString aStr2 = "";

        sal_Int32 nValue = rtl_str_compare( aStr1.getStr(), aStr2.getStr());
        ASSERT_TRUE(nValue == 0) << "compare failed, strings are equal.";
    }

    TEST_F(compare, compare_002)
    {
        rtl::OString aStr1 = "Line must be equal.";
        rtl::OString aStr2 = "Line must be equal.";

        sal_Int32 nValue = rtl_str_compare( aStr1.getStr(), aStr2.getStr());
        ASSERT_TRUE(nValue == 0) << "compare failed, strings are equal.";
    }

    TEST_F(compare, compare_003)
    {
        rtl::OString aStr1 = "Line must differ.";
        rtl::OString aStr2 = "Line foo bar, ok, differ.";

        sal_Int32 nValue = rtl_str_compare( aStr1.getStr(), aStr2.getStr());
        ASSERT_TRUE(nValue != 0) << "compare failed, strings differ.";
    }

    class compareIgnoreAsciiCase : public ::testing::Test
    {
    public:
    }; // class compareIgnoreAsciiCase

    TEST_F(compareIgnoreAsciiCase, compare_000)
    {
        // Former test passed (NULL, NULL) -> undefined behaviour.  Verify the
        // case-insensitive ordering-sign contract instead.
        rtl::OString aStr1 = "abc";
        rtl::OString aStr2 = "ABD";

        ASSERT_TRUE(rtl_str_compareIgnoreAsciiCase( aStr1.getStr(), aStr2.getStr()) < 0)
            << "\"abc\" must compare less than \"ABD\" ignoring case.";
        ASSERT_TRUE(rtl_str_compareIgnoreAsciiCase( aStr2.getStr(), aStr1.getStr()) > 0)
            << "\"ABD\" must compare greater than \"abc\" ignoring case.";
    }

    TEST_F(compareIgnoreAsciiCase, compare_000_1)
    {
        // Former test passed (validStr, NULL) -> undefined behaviour.
        // Verify the empty-string boundary instead.
        rtl::OString aStr1 = "Line must be equal.";
        rtl::OString aEmpty = "";

        ASSERT_TRUE(rtl_str_compareIgnoreAsciiCase( aStr1.getStr(), aEmpty.getStr()) > 0)
            << "a non-empty string must be greater than the empty string.";
    }
    TEST_F(compareIgnoreAsciiCase, compare_001)
    {
        rtl::OString aStr1 = "";
        rtl::OString aStr2 = "";

        sal_Int32 nValue = rtl_str_compareIgnoreAsciiCase( aStr1.getStr(), aStr2.getStr());
        ASSERT_TRUE(nValue == 0) << "compare failed, strings are equal.";
    }

    TEST_F(compareIgnoreAsciiCase, compare_002)
    {
        rtl::OString aStr1 = "Line must be equal.";
        rtl::OString aStr2 = "Line must be equal.";

        sal_Int32 nValue = rtl_str_compareIgnoreAsciiCase( aStr1.getStr(), aStr2.getStr());
        ASSERT_TRUE(nValue == 0) << "compare failed, strings are equal.";
    }

    TEST_F(compareIgnoreAsciiCase, compare_002_1)
    {
        rtl::OString aStr1 = "Line must be equal.";
        rtl::OString aStr2 = "LINE MUST BE EQUAL.";

        sal_Int32 nValue = rtl_str_compareIgnoreAsciiCase( aStr1.getStr(), aStr2.getStr());
        ASSERT_TRUE(nValue == 0) << "compare failed, strings are equal (if case insensitive).";
    }

    TEST_F(compareIgnoreAsciiCase, compare_003)
    {
        rtl::OString aStr1 = "Line must differ.";
        rtl::OString aStr2 = "Line foo bar, ok, differ.";

        sal_Int32 nValue = rtl_str_compareIgnoreAsciiCase( aStr1.getStr(), aStr2.getStr());
        ASSERT_TRUE(nValue != 0) << "compare failed, strings differ.";
    }
// -----------------------------------------------------------------------------

    class shortenedCompareIgnoreAsciiCase_WithLength : public ::testing::Test
    {
    public:
    }; // class compare

    TEST_F(shortenedCompareIgnoreAsciiCase_WithLength, compare_000)
    {
        // The _WithLength variant is length-bounded, so NULL data with length 0
        // is well-defined (the loop body never runs) and must return 0.
        // Keep the NULL+0 call as a regression guard, but actually assert it.
        sal_Int32 nValue = rtl_str_shortenedCompareIgnoreAsciiCase_WithLength( NULL, 0, NULL, 0, 0);
        ASSERT_TRUE(nValue == 0) << "zero-length comparison must return 0, even for NULL data.";
    }

    TEST_F(shortenedCompareIgnoreAsciiCase_WithLength, compare_000_1)
    {
        // First string has data, second is a zero-length string (NULL data, len 0).
        // Nothing is dereferenced; the function returns the length difference.
        rtl::OString aStr1 = "Line must be equal.";
        sal_Int32 nValue = rtl_str_shortenedCompareIgnoreAsciiCase_WithLength( aStr1.getStr(), aStr1.getLength(), NULL, 0, 1);
        ASSERT_TRUE(nValue == aStr1.getLength())
            << "comparing against a zero-length string must yield the length difference.";
    }
    TEST_F(shortenedCompareIgnoreAsciiCase_WithLength, compare_001)
    {
        rtl::OString aStr1 = "";
        rtl::OString aStr2 = "";

        sal_Int32 nValue = rtl_str_shortenedCompareIgnoreAsciiCase_WithLength( aStr1.getStr(), aStr1.getLength(), aStr2.getStr(), aStr2.getLength(), aStr1.getLength());
        ASSERT_TRUE(nValue == 0) << "compare failed, strings are equal.";
    }

    TEST_F(shortenedCompareIgnoreAsciiCase_WithLength, compare_002)
    {
        rtl::OString aStr1 = "Line must be equal.";
        rtl::OString aStr2 = "Line must be equal.";

        sal_Int32 nValue = rtl_str_shortenedCompareIgnoreAsciiCase_WithLength( aStr1.getStr(), aStr1.getLength(),
                                                                               aStr2.getStr(), aStr2.getLength(),
                                                                               aStr1.getLength());
        ASSERT_TRUE(nValue == 0) << "compare failed, strings are equal.";
    }

    TEST_F(shortenedCompareIgnoreAsciiCase_WithLength, compare_002_1)
    {
        rtl::OString aStr1 = "Line must be equal.";
        rtl::OString aStr2 = "LINE MUST BE EQUAL.";

        sal_Int32 nValue = rtl_str_shortenedCompareIgnoreAsciiCase_WithLength( aStr1.getStr(), aStr1.getLength(),
                                                                               aStr2.getStr(), aStr2.getLength(),
                                                                               aStr1.getLength());
        ASSERT_TRUE(nValue == 0) << "compare failed, strings are equal (if case insensitive).";
    }

    TEST_F(shortenedCompareIgnoreAsciiCase_WithLength, compare_003)
    {
        rtl::OString aStr1 = "Line must differ.";
        rtl::OString aStr2 = "Line foo bar, ok, differ.";

        sal_Int32 nValue = rtl_str_shortenedCompareIgnoreAsciiCase_WithLength( aStr1.getStr(), aStr1.getLength(),
                                                                               aStr2.getStr(), aStr2.getLength(),
                                                                               5);
        ASSERT_TRUE(nValue == 0) << "compare failed, strings are equal first 5 characters.";
    }

    TEST_F(shortenedCompareIgnoreAsciiCase_WithLength, compare_004)
    {
        rtl::OString aStr1 = "Line must differ.";
        rtl::OString aStr2 = "Line foo bar, ok, differ.";

        sal_Int32 nValue = rtl_str_shortenedCompareIgnoreAsciiCase_WithLength( aStr1.getStr(), aStr1.getLength(),
                                                                               aStr2.getStr(), aStr2.getLength(),
                                                                               aStr1.getLength());
        ASSERT_TRUE(nValue != 0) << "compare failed, strings differ.";
    }

// -----------------------------------------------------------------------------

    class hashCode : public ::testing::Test
    {
    public:
    }; // class compare

    TEST_F(hashCode, hashCode_000)
    {
        // Former test passed NULL -> getLength(NULL) dereferences NULL (UB).
        // Verify the empty-string boundary: hashCode("") is defined and 0.
        rtl::OString aStr1 = "";
        sal_Int32 nHashCode = rtl_str_hashCode( aStr1.getStr() );
        ASSERT_TRUE(nHashCode == 0) << "the hashCode of an empty string must be 0.";
    }

    TEST_F(hashCode, hashCode_001)
    {
        rtl::OString aStr1 = "Line for a hashCode.";
        sal_Int32 nHashCode = rtl_str_hashCode( aStr1.getStr() );
        printf("hashcode: %d\n", nHashCode);
        // ASSERT_TRUE(nValue == 0) << "failed.";
    }

    TEST_F(hashCode, hashCode_002)
    {
        rtl::OString aStr1 = "Line for a hashCode.";
        sal_Int32 nHashCode1 = rtl_str_hashCode( aStr1.getStr() );

        rtl::OString aStr2 = "Line for a hashCode.";
        sal_Int32 nHashCode2 = rtl_str_hashCode( aStr2.getStr() );

        ASSERT_TRUE(nHashCode1 == nHashCode2) << "hashcodes must be equal.";
    }

    TEST_F(hashCode, hashCode_003)
    {
        rtl::OString aStr1 = "Line for a hashCode.";
        sal_Int32 nHashCode1 = rtl_str_hashCode( aStr1.getStr() );

        rtl::OString aStr2 = "Line for an other hashcode.";
        sal_Int32 nHashCode2 = rtl_str_hashCode( aStr2.getStr() );

        ASSERT_TRUE(nHashCode1 != nHashCode2) << "hashcodes must differ.";
    }

// -----------------------------------------------------------------------------

    class indexOfChar : public ::testing::Test
    {
    public:
    }; // class compare

    TEST_F(indexOfChar, indexOfChar_000)
    {
        // Former test passed NULL -> while(*pStr) dereferences NULL (UB).
        // Verify the empty-string boundary: nothing is ever found -> -1.
        rtl::OString aStr1 = "";
        sal_Int32 nIndex = rtl_str_indexOfChar( aStr1.getStr(), 'x' );
        ASSERT_TRUE(nIndex == -1) << "searching an empty string must return -1.";
    }

    TEST_F(indexOfChar, indexOfChar_001)
    {
        rtl::OString aStr1 = "Line for a indexOfChar.";

        sal_Int32 nIndex = rtl_str_indexOfChar( aStr1.getStr(), 'L' );
        ASSERT_TRUE(nIndex == 0) << "index is wrong.";

        /* sal_Int32 */ nIndex = rtl_str_indexOfChar( aStr1.getStr(), 'i' );
        ASSERT_TRUE(nIndex == 1) << "index is wrong.";

        /* sal_Int32 */ nIndex = rtl_str_indexOfChar( aStr1.getStr(), 'n' );
        ASSERT_TRUE(nIndex == 2) << "index is wrong.";

        /* sal_Int32 */ nIndex = rtl_str_indexOfChar( aStr1.getStr(), 'e' );
        ASSERT_TRUE(nIndex == 3) << "index is wrong.";
    }

    TEST_F(indexOfChar, indexOfChar_002)
    {
        rtl::OString aStr1 = "Line for a indexOfChar.";
        sal_Int32 nIndex = rtl_str_indexOfChar( aStr1.getStr(), 'y' );

        ASSERT_TRUE(nIndex == -1) << "index is wrong.";
    }

// -----------------------------------------------------------------------------
    class lastIndexOfChar : public ::testing::Test
    {
    public:
    }; // class lastIndexOfChar

    TEST_F(lastIndexOfChar, lastIndexOfChar_000)
    {
        // Former test passed NULL -> getLength(NULL) dereferences NULL (UB).
        // Verify the empty-string boundary instead.
        rtl::OString aStr1 = "";
        sal_Int32 nIndex = rtl_str_lastIndexOfChar( aStr1.getStr(), 'x' );
        ASSERT_TRUE(nIndex == -1) << "searching an empty string must return -1.";
    }

    TEST_F(lastIndexOfChar, lastIndexOfChar_001)
    {
        rtl::OString aStr1 = "Line for a lastIndexOfChar.";

        sal_Int32 nIndex = rtl_str_lastIndexOfChar( aStr1.getStr(), 'C' );
        ASSERT_TRUE(nIndex == 22) << "index is wrong.";

        /* sal_Int32 */ nIndex = rtl_str_lastIndexOfChar( aStr1.getStr(), 'h' );
        ASSERT_TRUE(nIndex == 23) << "index is wrong.";

        /* sal_Int32 */ nIndex = rtl_str_lastIndexOfChar( aStr1.getStr(), 'a' );
        ASSERT_TRUE(nIndex == 24) << "index is wrong.";

        /* sal_Int32 */ nIndex = rtl_str_lastIndexOfChar( aStr1.getStr(), 'r' );
        ASSERT_TRUE(nIndex == 25) << "index is wrong.";
    }

    TEST_F(lastIndexOfChar, lastIndexOfChar_002)
    {
        rtl::OString aStr1 = "Line for a lastIndexOfChar.";
        sal_Int32 nIndex = rtl_str_lastIndexOfChar( aStr1.getStr(), 'y' );

        ASSERT_TRUE(nIndex == -1) << "index is wrong.";
    }

// -----------------------------------------------------------------------------

    class indexOfStr : public ::testing::Test
    {
    public:
    }; // class compare

    TEST_F(indexOfStr, indexOfStr_000)
    {
        // Former test passed (NULL, NULL) -> getLength(NULL) dereferences NULL (UB).
        // Verify the empty-haystack boundary: nothing is found -> -1.
        rtl::OString aStr1 = "";
        sal_Int32 nIndex = rtl_str_indexOfStr( aStr1.getStr(), "x" );
        ASSERT_TRUE(nIndex == -1) << "searching in an empty string must return -1.";
    }

    TEST_F(indexOfStr, indexOfStr_000_1)
    {
        // Former test passed a NULL needle -> getLength(NULL) (UB).
        // Verify the empty-needle boundary: an empty search string is never found.
        rtl::OString aStr1 = "Line for a indexOfStr.";
        sal_Int32 nIndex = rtl_str_indexOfStr( aStr1.getStr(), "" );
        ASSERT_TRUE(nIndex == -1) << "an empty search string is never found -> -1.";
    }

    TEST_F(indexOfStr, indexOfStr_001)
    {
        rtl::OString aStr1 = "Line for a indexOfStr.";

        sal_Int32 nIndex = rtl_str_indexOfStr( aStr1.getStr(), "Line" );
        ASSERT_TRUE(nIndex == 0) << "index is wrong.";

        /* sal_Int32 */ nIndex = rtl_str_indexOfStr( aStr1.getStr(), "for" );
        ASSERT_TRUE(nIndex == 5) << "index is wrong.";

        /* sal_Int32 */ nIndex = rtl_str_indexOfStr( aStr1.getStr(), "a" );
        ASSERT_TRUE(nIndex == 9) << "index is wrong.";

        /* sal_Int32 */ nIndex = rtl_str_indexOfStr( aStr1.getStr(), "a index" );
        ASSERT_TRUE(nIndex ==9) << "index is wrong.";
    }

    TEST_F(indexOfStr, indexOfStr_002)
    {
        rtl::OString aStr1 = "Line for a indexOfStr.";
        sal_Int32 nIndex = rtl_str_indexOfStr( aStr1.getStr(), "not exist" );

        ASSERT_TRUE(nIndex == -1) << "index is wrong.";
    }

// -----------------------------------------------------------------------------


    class lastIndexOfStr : public ::testing::Test
    {
    public:
    }; // class lastIndexOfStr

    TEST_F(lastIndexOfStr, lastIndexOfStr_000)
    {
        // Former test passed (NULL, NULL) -> getLength(NULL) dereferences NULL (UB).
        // Verify the empty-haystack boundary instead.
        rtl::OString aStr1 = "";
        sal_Int32 nIndex = rtl_str_lastIndexOfStr( aStr1.getStr(), "Line" );
        ASSERT_TRUE(nIndex == -1) << "searching in an empty string must return -1.";
    }

    TEST_F(lastIndexOfStr, lastIndexOfStr_000_1)
    {
        // Former test passed a NULL needle -> getLength(NULL) (UB).
        // Verify the empty-needle boundary: an empty search string is never found.
        rtl::OString aStr1 = "Line for a lastIndexOfStr.";
        sal_Int32 nIndex = rtl_str_lastIndexOfStr( aStr1.getStr(), "" );
        ASSERT_TRUE(nIndex == -1) << "an empty search string is never found -> -1.";
    }

    TEST_F(lastIndexOfStr, lastIndexOfStr_001)
    {
        rtl::OString aStr1 = "Line for a lastIndexOfStr.";
        rtl::OString aSearchStr = "Index";

        sal_Int32 nIndex = rtl_str_lastIndexOfStr( aStr1.getStr(), aSearchStr.getStr() );
        ASSERT_TRUE(nIndex == 15) << "index is wrong.";

        /* rtl::OString */ aSearchStr = "Line";
        /* sal_Int32 */ nIndex = rtl_str_lastIndexOfStr( aStr1.getStr(), aSearchStr.getStr() );
        ASSERT_TRUE(nIndex == 0) << "index is wrong.";

        /* rtl::OString */ aSearchStr = "";
        /* sal_Int32 */ nIndex = rtl_str_lastIndexOfStr( aStr1.getStr(), aSearchStr.getStr() );
        ASSERT_TRUE(nIndex == -1) << "index is wrong.";
    }

    TEST_F(lastIndexOfStr, lastIndexOfStr_002)
    {
        rtl::OString aStr1 = "Line for a lastIndexOfStr.";
        rtl::OString aSearchStr = "foo";
        sal_Int32 nIndex = rtl_str_lastIndexOfStr( aStr1.getStr(), aSearchStr.getStr() );

        ASSERT_TRUE(nIndex == -1) << "index is wrong.";
    }

    TEST_F(lastIndexOfStr, lastIndexOfStr_003)
    {
        rtl::OString aStr1 = "Line for a lastIndexOfStr.";
        rtl::OString aSearchStr = "O";
        sal_Int32 nIndex = rtl_str_lastIndexOfStr( aStr1.getStr(), aSearchStr.getStr() );

        ASSERT_TRUE(nIndex == 20) << "index is wrong.";
    }

// -----------------------------------------------------------------------------

    class replaceChar : public ::testing::Test
    {
    public:
    }; // class replaceChar

    TEST_F(replaceChar, replaceChar_000)
    {
        // Former test passed NULL -> while(*pStr) dereferences NULL (UB).
        // Verify the empty-string boundary: replacing in "" is a no-op.
        sal_Char pStr[] = "";
        rtl_str_replaceChar( pStr, 'a', 'b' );
        ASSERT_TRUE(pStr[0] == 0) << "replacing in an empty string must leave it empty.";
    }

    TEST_F(replaceChar, replaceChar_001)
    {
        rtl::OString aStr1 = "replace char.";
        rtl::OString aShouldStr1 = "ruplacu char.";

        sal_Char* pStr = (sal_Char*) malloc(aStr1.getLength() + 1);
        ASSERT_TRUE(pStr != NULL) << "can't get memory for test";
        strcpy(pStr, aStr1.getStr());

        rtl_str_replaceChar( pStr, 'e', 'u' );

        ASSERT_TRUE(aShouldStr1.equals(rtl::OString(pStr)) == sal_True) << "replace failed";
        free(pStr);
    }

// -----------------------------------------------------------------------------

    class replaceChar_WithLength : public ::testing::Test
    {
    public:
    }; // class replaceChar

    TEST_F(replaceChar_WithLength, replaceChar_WithLength_000)
    {
        // Length-bounded: NULL data with length 0 never dereferences -> no-op.
        // Keep the NULL+0 call as a regression guard for that tolerance.
        rtl_str_replaceChar_WithLength( NULL, 0, 0, 0 );
        SUCCEED() << "NULL data with zero length must be tolerated (no dereference).";
    }

    TEST_F(replaceChar_WithLength, replaceChar_WithLength_000_1)
    {
        // Former test passed (NULL, 1, ...) -> the loop runs once and
        // dereferences NULL (UB).  Verify the length bound instead: only the
        // first nLen characters are touched, the rest are left intact.
        sal_Char pStr[] = "aaaa";
        rtl_str_replaceChar_WithLength( pStr, 2, 'a', 'b' );
        ASSERT_TRUE(rtl::OString(pStr).equals(rtl::OString("bbaa")) == sal_True)
            << "only the first nLen characters must be replaced.";
    }
    TEST_F(replaceChar_WithLength, replaceChar_WithLength_001)
    {
        rtl::OString aStr1 = "replace char.";
        rtl::OString aShouldStr1 = "ruplace char.";

        sal_Char* pStr = (sal_Char*) malloc(aStr1.getLength() + 1);
        ASSERT_TRUE(pStr != NULL) << "can't get memory for test";
        strcpy(pStr, aStr1.getStr());

        rtl_str_replaceChar_WithLength( pStr, 6, 'e', 'u' );

        ASSERT_TRUE(aShouldStr1.equals(rtl::OString(pStr)) == sal_True) << "replace failed";
        free(pStr);
    }

// -----------------------------------------------------------------------------

    class toAsciiLowerCase : public ::testing::Test
    {
    public:
    }; // class replaceChar

    TEST_F(toAsciiLowerCase, toAsciiLowerCase_000)
    {
        // Former test passed NULL -> while(*pStr) dereferences NULL (UB).
        // Verify the empty-string boundary: lowercasing "" is a no-op.
        sal_Char pStr[] = "";
        rtl_str_toAsciiLowerCase( pStr );
        ASSERT_TRUE(pStr[0] == 0) << "lowercasing an empty string must leave it empty.";
    }

    TEST_F(toAsciiLowerCase, toAsciiLowerCase_001)
    {
        rtl::OString aStr1 = "CHANGE THIS TO ASCII LOWER CASE.";
        rtl::OString aShouldStr1 = "change this to ascii lower case.";

        sal_Char* pStr = (sal_Char*) malloc(aStr1.getLength() + 1);
        ASSERT_TRUE(pStr != NULL) << "can't get memory for test";
        strcpy(pStr, aStr1.getStr());

        rtl_str_toAsciiLowerCase( pStr );

        ASSERT_TRUE(aShouldStr1.equals(rtl::OString(pStr)) == sal_True) << "failed";
        free(pStr);
    }

    class toAsciiLowerCase_WithLength : public ::testing::Test
    {
    public:
    }; // class replaceChar

    TEST_F(toAsciiLowerCase_WithLength, toAsciiLowerCase_WithLength_000)
    {
        // Length-bounded: NULL data with length 0 never dereferences -> no-op.
        rtl_str_toAsciiLowerCase_WithLength( NULL, 0 );
        SUCCEED() << "NULL data with zero length must be tolerated (no dereference).";
    }

    TEST_F(toAsciiLowerCase_WithLength, toAsciiLowerCase_WithLength_001)
    {
        rtl::OString aStr1 = "CHANGE THIS TO ASCII LOWER CASE.";
        rtl::OString aShouldStr1 = "change thiS TO ASCII LOWER CASE.";

        sal_Char* pStr = (sal_Char*) malloc(aStr1.getLength() + 1);
        ASSERT_TRUE(pStr != NULL) << "can't get memory for test";
        strcpy(pStr, aStr1.getStr());

        rtl_str_toAsciiLowerCase_WithLength( pStr, 10 );

        printf("Lowercase with length: '%s'\n", pStr);
        ASSERT_TRUE(aShouldStr1.equals(rtl::OString(pStr)) == sal_True) << "failed";
        free(pStr);
    }

// -----------------------------------------------------------------------------

    class toAsciiUpperCase : public ::testing::Test
    {
    public:
    }; // class replaceChar

    TEST_F(toAsciiUpperCase, toAsciiUpperCase_000)
    {
        // Former test passed NULL -> while(*pStr) dereferences NULL (UB).
        // Verify the empty-string boundary: uppercasing "" is a no-op.
        sal_Char pStr[] = "";
        rtl_str_toAsciiUpperCase( pStr );
        ASSERT_TRUE(pStr[0] == 0) << "uppercasing an empty string must leave it empty.";
    }

    TEST_F(toAsciiUpperCase, toAsciiUpperCase_001)
    {
        rtl::OString aStr1 = "change this to ascii upper case.";
        rtl::OString aShouldStr1 = "CHANGE THIS TO ASCII UPPER CASE.";

        sal_Char* pStr = (sal_Char*) malloc(aStr1.getLength() + 1);
        ASSERT_TRUE(pStr != NULL) << "can't get memory for test";
        strcpy(pStr, aStr1.getStr());

        rtl_str_toAsciiUpperCase( pStr );

        ASSERT_TRUE(aShouldStr1.equals(rtl::OString(pStr)) == sal_True) << "failed";
        free(pStr);
    }

    class toAsciiUpperCase_WithLength : public ::testing::Test
    {
    public:
    }; // class replaceChar

    TEST_F(toAsciiUpperCase_WithLength, toAsciiUpperCase_WithLength_000)
    {
        // Length-bounded: NULL data with length 0 never dereferences -> no-op.
        rtl_str_toAsciiUpperCase_WithLength( NULL, 0 );
        SUCCEED() << "NULL data with zero length must be tolerated (no dereference).";
    }

    TEST_F(toAsciiUpperCase_WithLength, toAsciiUpperCase_WithLength_001)
    {
        rtl::OString aStr1 = "change this to ascii lower case.";
        rtl::OString aShouldStr1 = "CHANGE THIs to ascii lower case.";

        sal_Char* pStr = (sal_Char*) malloc(aStr1.getLength() + 1);
        ASSERT_TRUE(pStr != NULL) << "can't get memory for test";

        strcpy(pStr, aStr1.getStr());
        rtl_str_toAsciiUpperCase_WithLength( pStr, 10 );

        printf("Uppercase with length: '%s'\n", aStr1.getStr());
        ASSERT_TRUE(aShouldStr1.equals(rtl::OString(pStr)) == sal_True) << "failed";
        free(pStr);
    }

    // -----------------------------------------------------------------------------

    class trim_WithLength : public ::testing::Test
    {
      public:
    };

    TEST_F(trim_WithLength, trim_WithLength_000)
    {
        // Length-bounded: NULL data with length 0 never dereferences and
        // returns the resulting length 0.
        sal_Int32 nLen = rtl_str_trim_WithLength(NULL, 0);
        ASSERT_TRUE(nLen == 0) << "trimming a zero-length string must return 0 (and not GPF).";
    }

    TEST_F(trim_WithLength, trim_WithLength_000_1)
    {
        char pStr[] = { "  trim this" };
        rtl_str_trim_WithLength( pStr, 0 );
    }

    TEST_F(trim_WithLength, trim_WithLength_001)
    {
        char const *pStr = "  trim this";
        sal_Char *pStr2 = (sal_Char*)malloc(strlen(pStr) + 1);
        if (pStr2)
        {
            strcpy(pStr2, pStr);
            rtl_str_trim_WithLength( pStr2, 2 );

            ASSERT_TRUE(strlen(pStr2) == 0) << "string should be empty";
            free(pStr2);
        }
    }

    TEST_F(trim_WithLength, trim_WithLength_002)
    {
        char const *pStr = "trim this";
        sal_Char *pStr2 = (sal_Char*)malloc(strlen(pStr) + 1);
        if (pStr2)
        {
            strcpy(pStr2, pStr);
            rtl_str_trim_WithLength( pStr2, 5 );

            ASSERT_TRUE(strlen(pStr2) == 4) << "string should contain 'trim'";
            free(pStr2);
        }
    }

    TEST_F(trim_WithLength, trim_WithLength_003)
    {
        char const *pStr = "     trim   this";
        sal_Char *pStr2 = (sal_Char*)malloc(strlen(pStr) + 1);
        if (pStr2)
        {
            strcpy(pStr2, pStr);
            rtl_str_trim_WithLength( pStr2, 11 );

            ASSERT_TRUE(strlen(pStr2) == 4) << "string should contain 'trim'";
            free(pStr2);
        }
    }

    TEST_F(trim_WithLength, trim_WithLength_004)
    {
        char const *pStr = "\r\n\t \n\r    trim  \n this";
        sal_Char *pStr2 = (sal_Char*)malloc(strlen(pStr) + 1);
        if (pStr2)
        {
            strcpy(pStr2, pStr);
            rtl_str_trim_WithLength( pStr2, 17 );

            ASSERT_TRUE(strlen(pStr2) == 4) << "string should contain 'trim'";
            free(pStr2);
        }
    }

    TEST_F(trim_WithLength, trim_WithLength_005)
    {
        char const *pStr = "\r\n\t \n\r    trim \t this \n\r\t\t     ";
        sal_Char *pStr2 = (sal_Char*)malloc(strlen(pStr) + 1);
        if (pStr2)
        {
            strcpy(pStr2, pStr);
            rtl_str_trim_WithLength( pStr2, strlen(pStr2) );
            ASSERT_TRUE(strlen(pStr2) == 11) << "string should contain 'trim'";
            free(pStr2);
        }
    }

    // -----------------------------------------------------------------------------

    class valueOfChar : public ::testing::Test
    {
      public:
    };

    TEST_F(valueOfChar, valueOfChar_000)
    {
        // Former test passed NULL -> the function writes through NULL (UB);
        // the "should not GPF" comment was wrong, it always did.
        // Verify the documented behaviour on a real buffer: writes the char
        // and a terminating NUL, returns 1.
        sal_Char pStr[RTL_STR_MAX_VALUEOFCHAR];
        sal_Int32 nLen = rtl_str_valueOfChar(pStr, 'Z');
        ASSERT_TRUE(nLen == 1) << "valueOfChar must return 1.";
        ASSERT_TRUE(pStr[0] == 'Z') << "valueOfChar must write the character.";
        ASSERT_TRUE(pStr[1] == 0) << "valueOfChar must NUL-terminate.";
    }
    TEST_F(valueOfChar, valueOfChar_001)
    {
        sal_Char *pStr = (sal_Char*)malloc(RTL_STR_MAX_VALUEOFCHAR);
        if (pStr)
        {
            rtl_str_valueOfChar(pStr, 'A');

            ASSERT_TRUE(pStr[0] == 'A') << "string should contain 'A'";
            free(pStr);
        }
    }

} // namespace rtl_str

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
