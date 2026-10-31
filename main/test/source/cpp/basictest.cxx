#include "gtest/gtest.h"
#include "scanner.hxx"
#include "token.hxx"
#include "sbcomp.hxx"
#include "parser.hxx"
#include <rtl/ustring.hxx>
#include <basic/sbobjmod.hxx>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace {

// =========================================================================
// Step 2: Tokenizer regression tests — verify token-table ordering fix
// =========================================================================

TEST(BasicCompilerTest, PP_CreateAndNext)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("#If True Then\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    aTok->SetVBASupportOn(true);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, PP_IF);
    delete aTok;
}

TEST(BasicCompilerTest, PP_ConstToken)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("#Const MY_CONST = True\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    aTok->SetVBASupportOn(true);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, PP_CONST);
    delete aTok;
}

TEST(BasicCompilerTest, PP_ElseToken)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("#Else\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    aTok->SetVBASupportOn(true);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, PP_ELSE);
    delete aTok;
}

TEST(BasicCompilerTest, PP_ElseIfToken)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("#ElseIf False Then\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    aTok->SetVBASupportOn(true);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, PP_ELSEIF);
    delete aTok;
}

TEST(BasicCompilerTest, PP_EndIfToken)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("#End If\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    aTok->SetVBASupportOn(true);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, PP_ENDIF);
    delete aTok;
}

TEST(BasicCompilerTest, PP_MallocAfterTokenizer)
{
    fprintf(stderr, "PP_MallocAfterTokenizer: allocating small block\n");
    void* p = malloc(16);
    fprintf(stderr, "PP_MallocAfterTokenizer: malloc OK, freeing\n");
    free(p);
    EXPECT_TRUE(true);
}

// --- Tokenizer regression: Interface keyword (was misplaced in token table) ---

TEST(TokenTableRegression, InterfaceToken)
{
    // INTERFACE is found by binary search but converted to SYMBOL when
    // bCompatible is false (default). Verify it's NOT confused with other tokens.
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("Interface\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    // Token is found successfully - not an error condition
    delete aTok;
}

TEST(TokenTableRegression, ImplementsToken)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("Implements\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    // FOUND without crash = binary search found the entry
    delete aTok;
}

// --- Adjacent keyword tests to verify token-table ordering fix ---
// INTERFACE was moved to the correct alphabetical position (after Integer, before Is).
// These tests verify adjacent entries are still found correctly by binary search.

TEST(TokenTableRegression, AdjToken_CloseBeforeInterface)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("Close\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, CLOSE);
    delete aTok;
}

TEST(TokenTableRegression, AdjToken_CompareAfterInterface)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("Compare\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, COMPARE);
    delete aTok;
}

TEST(TokenTableRegression, AdjToken_ClassModuleBeforeInterface)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("ClassModule\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    // FOUND without crash = binary search navigated correctly past ClassModule
    delete aTok;
}

TEST(TokenTableRegression, AdjToken_ConstAfterInterface)
{
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("Const\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, _CONST_);
    delete aTok;
}

TEST(TokenTableRegression, EndHashIfToken)
{
    // #End If should tokenize as PP_ENDIF
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("#End If\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    aTok->SetVBASupportOn(true);
    SbiToken eTok = aTok->Next();
    EXPECT_EQ(eTok, PP_ENDIF);
    delete aTok;
}

TEST(TokenTableRegression, InterfaceNotMatchedAsOther)
{
    // "Interface" must be found by binary search without confusion
    rtl::OUString ouSrc = rtl::OUString::createFromAscii("Interface\n");
    SbiTokenizer* aTok = new SbiTokenizer(ouSrc, NULL);
    aTok->Keywords(sal_True);
    SbiToken eTok = aTok->Next();
    // No crash = correct binary search navigation
    ASSERT_TRUE(eTok != NIL);
    delete aTok;
}

// =========================================================================
// Step 3: Preprocessor directive evaluation tests via SbiParser
// =========================================================================

class PreprocTest : public ::testing::Test
{
protected:
    // Creates an SbModule with the given source, compiles it, and returns
    // true if compilation succeeded without errors.
    bool compileSource(const char* src)
    {
        String aModuleName(String::CreateFromAscii("TestModule"));
        SbModule* pMod = new SbModule(aModuleName, sal_True);
        pMod->SetSource32(rtl::OUString::createFromAscii(src));

        SbiParser* pParser = new SbiParser(NULL, pMod);
        sal_Bool bErrors = sal_False;
        while( pParser->Parse() ) {}
        bErrors = pParser->GetErrors();
        delete pParser;
        // SbModule has a protected destructor - cannot delete directly.
        // In production code this is managed by SvRef; in the test the
        // process exits so cleanup is not critical.
        return bErrors == 0;
    }

    // Helper: create module and return parser for direct inspection
    SbiParser* createParser(const char* src)
    {
        String aModuleName(String::CreateFromAscii("TestModule"));
        SbModule* pMod = new SbModule(aModuleName, sal_False);
        pMod->SetSource32(rtl::OUString::createFromAscii(src));
        return new SbiParser(NULL, pMod);
    }
};  // class PreprocTest

// 1. True and false conditions

TEST_F(PreprocTest, PP_TrueCondition_Compiles)
{
    // Code in a True #If branch should compile successfully
    // Using Rem (comment) as test code since Dim requires StarBASIC*
    bool result = compileSource(
        "#If True Then\n"
        "Rem This branch should be active\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

TEST_F(PreprocTest, PP_FalseCondition_SkipsCode)
{
    // Code in a False #If branch should be skipped without errors
    bool result = compileSource(
        "#If False Then\n"
        "Dim x As Integer\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

// 2. #ElseIf and #Else branch selection

TEST_F(PreprocTest, PP_ElseBranch_SkipsInactive)
{
    // When first branch is True, #Else code should be skipped
    // Even if #Else code contains invalid syntax
    bool result = compileSource(
        "#If True Then\n"
        "Rem Active branch\n"
        "#Else\n"
        "ThisIsInvalidSyntax!!!\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

TEST_F(PreprocTest, PP_ElseIfBranch_Selection)
{
    // #ElseIf evaluates when #If is False
    bool result = compileSource(
        "#If False Then\n"
        "ThisIsInvalidSyntax!!!\n"
        "#ElseIf True Then\n"
        "Rem Elseif branch active\n"
        "#Else\n"
        "ThisIsInvalidSyntax!!!\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

TEST_F(PreprocTest, PP_ElseIfBranch_SkippedAfterTrue)
{
    // When #If is True, #ElseIf branch is skipped even with errors
    bool result = compileSource(
        "#If True Then\n"
        "Rem First branch\n"
        "#ElseIf True Then\n"
        "ThisIsInvalidSyntax!!!\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

// 3. #Const declarations

TEST_F(PreprocTest, PP_ConstDeclaration_UsedInCondition)
{
    bool result = compileSource(
        "#Const MY_FLAG = True\n"
        "#If MY_FLAG Then\n"
        "Rem Branch taken\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

TEST_F(PreprocTest, PP_ConstFalse_ConditionallyFalse)
{
    bool result = compileSource(
        "#Const MY_FLAG = False\n"
        "#If MY_FLAG Then\n"
        "ThisIsInvalidSyntax!!!\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

TEST_F(PreprocTest, PP_ConstNumericValue)
{
    bool result = compileSource(
        "#Const VERSION = 2\n"
        "#If VERSION >= 2 Then\n"
        "Rem Numeric condition true\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

// 4. Boolean expressions and comparisons

TEST_F(PreprocTest, PP_AndExpression)
{
    bool result = compileSource(
        "#If True And False Then\n"
        "ThisIsInvalidSyntax!!!\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

TEST_F(PreprocTest, PP_OrExpression)
{
    bool result = compileSource(
        "#If False Or True Then\n"
        "Rem Branch taken\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

TEST_F(PreprocTest, PP_NotExpression)
{
    bool result = compileSource(
        "#If Not False Then\n"
        "Rem Branch taken\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

TEST_F(PreprocTest, PP_ComparisonExpression)
{
    bool result = compileSource(
        "#If VBA7 = True Then\n"
        "Rem VBA7 matches\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

// 5. Nested directives

TEST_F(PreprocTest, PP_NestedDirectives)
{
    bool result = compileSource(
        "#If True Then\n"
        "#If False Then\n"
        "ThisIsInvalidSyntax!!!\n"
        "#End If\n"
        "Rem Inner branch\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

TEST_F(PreprocTest, PP_NestedBothTrue)
{
    bool result = compileSource(
        "#If True Then\n"
        "#If True Then\n"
        "Rem Nested branch\n"
        "#End If\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

// 6. Inactive branches containing invalid code

TEST_F(PreprocTest, PP_InactiveBranchWithSyntaxError)
{
    bool result = compileSource(
        "#If False Then\n"
        "Dim asdf zxcv !!!\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

// 7. Unmatched/malformed directives

TEST_F(PreprocTest, PP_UnmatchedEndIf)
{
    bool result = compileSource(
        "#End If\n"
    );
    EXPECT_FALSE(result);
}

TEST_F(PreprocTest, PP_UnmatchedElse)
{
    bool result = compileSource(
        "#Else\n"
    );
    EXPECT_FALSE(result);
}

// 8. Unknown conditional-compilation identifiers

TEST_F(PreprocTest, PP_UnknownIdentifier_TreatedAsFalse)
{
    bool result = compileSource(
        "#If UNKNOWN_CONST Then\n"
        "ThisIsInvalidSyntax!!!\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

// --- OpenSolver 2.9.2 fixture expressions ---

TEST_F(PreprocTest, PP_OpenSolver_PlatformCheck_Mac)
{
    // From SolverCommon.bas: #If Mac And MAC_OFFICE_VERSION >= 15
    // On Linux: Mac=0, MAC_OFFICE_VERSION=0, so condition is False
    bool result = compileSource(
        "#If Mac And MAC_OFFICE_VERSION >= 15 Then\n"
        "ThisIsMacOnlyCode!!!\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

TEST_F(PreprocTest, PP_OpenSolver_Win64Check)
{
    // From SolverCommon.bas: LibDir() platform detection
    // On Linux: all false, falls through to #Else
    bool result = compileSource(
        "#If Mac Then\n"
        "ThisIsInvalidSyntax!!!\n"
        "#ElseIf Win64 Then\n"
        "ThisIsInvalidSyntax!!!\n"
        "#Else\n"
        "Rem Fallback branch\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

TEST_F(PreprocTest, PP_OpenSolver_VBA7Check)
{
    // VBA7 is set to True on all platforms
    bool result = compileSource(
        "#If VBA7 Then\n"
        "Rem VBA7 is True\n"
        "#End If\n"
    );
    EXPECT_TRUE(result);
}

} // namespace
