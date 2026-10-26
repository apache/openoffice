#include "gtest/gtest.h"
#include "scanner.hxx"
#include "token.hxx"
#include <rtl/ustring.hxx>
#include <cstdio>
#include <cstdlib>

namespace {

TEST(BasicCompilerTest, PP_CreateAndNext)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("#If True Then\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, PP_IF);
    delete aTok;
}

TEST(BasicCompilerTest, PP_ConstToken)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("#Const MY_CONST = True\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, PP_CONST);
    delete aTok;
}

TEST(BasicCompilerTest, PP_ElseToken)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("#Else\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, PP_ELSE);
    delete aTok;
}

TEST(BasicCompilerTest, PP_ElseIfToken)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("#ElseIf False Then\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, PP_ELSEIF);
    delete aTok;
}

TEST(BasicCompilerTest, PP_EndIfToken)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("#End If\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, PP_ENDIF);
    delete aTok;
}

TEST(BasicCompilerTest, PP_MallocAfterTokenizer)
{
    // This test runs AFTER all tokenizer tests
    // If heap was corrupted, malloc will crash
    fprintf(stderr, "PP_MallocAfterTokenizer: allocating small block\n");
    void* p = malloc(16);
    fprintf(stderr, "PP_MallocAfterTokenizer: malloc OK, freeing\n");
    free(p);
    EXPECT_TRUE(true);
}

} // namespace
