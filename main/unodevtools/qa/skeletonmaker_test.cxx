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

// Tests for the uno-skeletonmaker generators, driven in-process.
//
// These are NOT ported from upstream - unodevtools has no qa/ of its own.  They
// exist because the tool cannot usefully be tested through its command line:
// skeletonmaker.cxx catches CannotDumpException, prints "Error: <type>", and
// then falls through to `return 0`, so a failed generation is indistinguishable
// from a successful one by exit code.  (UnknownTypeThrows below pins that the
// defect is confined to main() - the library itself does throw.)
//
// Everything here calls the generators directly: they write through a
// std::ostream &, so no subprocess and no golden files are involved.
//
// The type provider is codemaker's RegistryTypeManager, which reads a .rdb
// straight off disk, NOT unodevtools' UnoTypeManager, which bootstraps a
// component context.  The generators take the TypeManager base class, so they
// cannot tell the difference -- and that choice is what keeps this suite off
// the office install entirely.  Every type named below is therefore a udkapi
// type, since //main/udkapi's rdb is the only registry staged.

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "preextstl.h"
#include "gtest/gtest.h"
#include "postextstl.h"

#include "rtl/ustring.hxx"

#include "codemaker/global.hxx"
#include "codemaker/typemanager.hxx"

#include "skeletoncommon.hxx"
#include "skeletoncpp.hxx"
#include "skeletonjava.hxx"

namespace {

// All three live in udkapi, the registry this suite stages:
//   - XServiceInfo returns a string AND a sequence of strings, so one type
//     exercises both interesting parts of the type mapping
//   - XInitialization is the plain-interface case: a class and nothing else
//   - SequenceOutputStream is a NEW-STYLE service, the only kind of input that
//     makes cppcompskeleton emit the component entry points (it guards that
//     block on `serviceobject`)
const char INTERFACE_TYPE[] = "com.sun.star.lang.XServiceInfo";
const char INIT_TYPE[]      = "com.sun.star.lang.XInitialization";
const char SERVICE_TYPE[]   = "com.sun.star.io.SequenceOutputStream";

// Staged beside the exe by data_files, and the test's working directory is that
// staged dir, so a bare name resolves.
const char UDKAPI_RDB[] = "udkapi_idl.rdb";

bool contains(std::string const & haystack, char const * needle)
{
    return haystack.find(needle) != std::string::npos;
}

// True if the generated text carries a DYNAMIC exception specification -
// `throw (<type list>)` or the `SAL_THROW((<type list>))` spelling, which the
// component helper used to emit on _create().  Both are the construct C++17
// removed, and both were swept out of the tree by the throw-spec change.
//
// Deliberately NOT a plain search for "throw (": the EMPTY `throw ()` that the
// class definition puts on acquire()/release(), and the matching
// `SAL_THROW(())`, are a DIFFERENT construct -- MSVC honours it, C++17 keeps it
// as a deprecated spelling of noexcept, and migrating it is a separate change.
// A test that rejected those too would fail the moment a property-helper
// component is generated.
bool hasExceptionSpec(std::string const & s)
{
    static char const * const spellings[] = { "throw", "SAL_THROW" };
    for (int k = 0; k != 2; ++k) {
        std::string const kw(spellings[k]);
        for (std::string::size_type i = s.find(kw); i != std::string::npos;
             i = s.find(kw, i + 1))
        {
            std::string::size_type j = s.find_first_not_of(' ', i + kw.size());
            if (j == std::string::npos || s[j] != '(') {
                continue;           // a throw STATEMENT, not a specification
            }
            j = s.find_first_not_of(' ', j + 1);
            // SAL_THROW takes the list in a SECOND pair of parentheses
            if (k == 1) {
                if (j == std::string::npos || s[j] != '(') {
                    continue;
                }
                j = s.find_first_not_of(' ', j + 1);
            }
            if (j != std::string::npos && s[j] != ')') {
                return true;
            }
        }
    }
    return false;
}

// Reading the registry once for the whole suite rather than per test.
class SkeletonMakerTest : public ::testing::Test
{
protected:
    static RegistryTypeManager * s_manager;

    static void SetUpTestCase()
    {
        s_manager = new RegistryTypeManager();
        StringVector registries;
        registries.push_back(::rtl::OString(UDKAPI_RDB));
        ASSERT_TRUE(s_manager->init(registries))
            << "cannot open " << UDKAPI_RDB << " - is it staged?";
        // The registry root, NOT a detail: idl_pipeline.bzl merges the .urd
        // files under the key "UCR" and runs cppumaker with -BUCR, so every
        // type sits at UCR/com/sun/star/...  searchTypeKey() composes
        // base + "/" + name, so leaving the default "/" looks for
        // "//com/sun/star/..." and every single lookup misses -- which surfaces
        // as CannotDumpException from the generator, not as a failed init().
        s_manager->setBase("UCR");
    }

    static void TearDownTestCase()
    {
        delete s_manager;
        s_manager = 0;
    }

    static skeletonmaker::ProgramOptions cppOptions()
    {
        skeletonmaker::ProgramOptions o;
        o.language = 2; // C++
        return o;
    }

    static skeletonmaker::ProgramOptions javaOptions()
    {
        skeletonmaker::ProgramOptions o;
        o.language = 1; // Java
        o.java5 = true;
        return o;
    }

    // Where generateSkeleton() is allowed to write.  $(SCRATCH) is a fresh,
    // writable, per-run directory; the staged dir itself is build output and
    // must be treated as read-only.  Plain getenv(), so it is a native path
    // with single backslashes and no bootstrap expansion.
    static ::rtl::OString scratchDir()
    {
        char const * p = getenv("SKELETONMAKER_OUT");
        EXPECT_TRUE(p != 0) << "SKELETONMAKER_OUT unset - see the env attr";
        return ::rtl::OString(p == 0 ? "." : p);
    }

    static std::string readFile(::rtl::OString const & path)
    {
        std::ifstream in(path.getStr(), std::ios_base::binary);
        if (!in.is_open()) {
            return std::string();
        }
        std::ostringstream buf;
        buf << in.rdbuf();
        return buf.str();
    }

    // generateSkeleton() derives its filename from implname, mirroring
    // getOutputStream(): the dots become directory separators.
    static ::rtl::OString outPath(char const * implname, char const * ext)
    {
        ::rtl::OString p(scratchDir());
        p += ::rtl::OString("/") + ::rtl::OString(implname).replace('.', '/')
             + ::rtl::OString(ext);
        return p;
    }
};

RegistryTypeManager * SkeletonMakerTest::s_manager = 0;

// 1. The C++ type mapping: an interface's direct methods, fully qualified.
TEST_F(SkeletonMakerTest, DumpCppInterface)
{
    std::ostringstream o;
    skeletonmaker::ProgramOptions options(cppOptions());
    skeletonmaker::cpp::generateDocumentation(
        o, options, *s_manager, INTERFACE_TYPE, ::rtl::OString());

    std::string const s(o.str());
    EXPECT_TRUE(contains(s, "SAL_CALL getImplementationName")) << s;
    EXPECT_TRUE(contains(s, "SAL_CALL supportsService")) << s;
    // UNO string maps to rtl::OUString, sequence<string> to Sequence< OUString >,
    // and the default is the long namespace form.
    EXPECT_TRUE(contains(s, "::rtl::OUString")) << s;
    EXPECT_TRUE(contains(s, "Sequence< ::rtl::OUString >")) << s;
    EXPECT_TRUE(contains(s, "::com::sun::star::uno::Sequence")) << s;
    // No dynamic exception specification: C++17 removed the construct, so the
    // generator must not hand one to somebody starting a new component.
    EXPECT_FALSE(hasExceptionSpec(s)) << s;
}

// 2. -sn/--shortnames swaps the namespace form, and must do so everywhere:
//    a half-applied abbreviation still compiles but is not what was asked for.
TEST_F(SkeletonMakerTest, DumpCppShortNames)
{
    std::ostringstream o;
    skeletonmaker::ProgramOptions options(cppOptions());
    options.shortnames = true;
    skeletonmaker::cpp::generateDocumentation(
        o, options, *s_manager, INTERFACE_TYPE, ::rtl::OString());

    std::string const s(o.str());
    EXPECT_TRUE(contains(s, "css::uno::")) << s;
    EXPECT_FALSE(contains(s, "::com::sun::star::uno::Sequence")) << s;
}

// 3. The same type through the Java mapping - a different generator, and the
//    proof that commonjava is doing something rather than merely linking.
TEST_F(SkeletonMakerTest, DumpJavaInterface)
{
    std::ostringstream o;
    skeletonmaker::ProgramOptions options(javaOptions());
    skeletonmaker::java::generateDocumentation(
        o, options, *s_manager, INTERFACE_TYPE, ::rtl::OString());

    std::string const s(o.str());
    EXPECT_TRUE(contains(s, "getImplementationName")) << s;
    // Java has no SAL_CALL and no rtl::OUString; string maps to String.
    EXPECT_FALSE(contains(s, "SAL_CALL")) << s;
    EXPECT_FALSE(contains(s, "rtl::OUString")) << s;
}

// 4. A whole C++ component from a bare interface: a class, and deliberately
//    NO component entry points (see ComponentCppServiceHasFactory).
TEST_F(SkeletonMakerTest, ComponentCppInterface)
{
    skeletonmaker::ProgramOptions options(cppOptions());
    options.shortnames = true;
    options.implname = "MyComp";
    options.outputpath = scratchDir();

    std::vector< ::rtl::OString > types;
    types.push_back(::rtl::OString(INIT_TYPE));
    skeletonmaker::cpp::generateSkeleton(
        options, *s_manager, types, ::rtl::OString());

    std::string const s(readFile(outPath("MyComp", ".cxx")));
    ASSERT_FALSE(s.empty()) << "no MyComp.cxx written";
    EXPECT_TRUE(contains(s, "WeakImplHelper1")) << s;
    EXPECT_TRUE(contains(s, "XInitialization")) << s;
    EXPECT_TRUE(contains(s, "SAL_CALL initialize")) << s;
    EXPECT_FALSE(hasExceptionSpec(s)) << s;
    EXPECT_FALSE(contains(s, "component_getFactory")) << s;
}

// 5. The same call for a new-style SERVICE, which is the only input that
//    reaches the factory/entry-point block.
TEST_F(SkeletonMakerTest, ComponentCppServiceHasFactory)
{
    skeletonmaker::ProgramOptions options(cppOptions());
    options.shortnames = true;
    options.implname = "MySvc";
    options.outputpath = scratchDir();

    std::vector< ::rtl::OString > types;
    types.push_back(::rtl::OString(SERVICE_TYPE));
    skeletonmaker::cpp::generateSkeleton(
        options, *s_manager, types, ::rtl::OString());

    std::string const s(readFile(outPath("MySvc", ".cxx")));
    ASSERT_FALSE(s.empty()) << "no MySvc.cxx written";
    EXPECT_TRUE(contains(s, "component_getFactory")) << s;
    EXPECT_TRUE(contains(s, "component_getImplementationEnvironment")) << s;
    EXPECT_TRUE(contains(s, "_getSupportedServiceNames")) << s;
    // The widest reach into cppcompskeleton the suite has: XServiceInfo bodies,
    // the class definition and the component entry points all in one file.
    EXPECT_FALSE(hasExceptionSpec(s)) << s;
}

// 6. The Java component writer, including the package-to-directory split that
//    getOutputStream() does on the implementation name.
TEST_F(SkeletonMakerTest, ComponentJavaInterface)
{
    skeletonmaker::ProgramOptions options(javaOptions());
    options.implname = "org.example.MyComp";
    options.outputpath = scratchDir();

    std::vector< ::rtl::OString > types;
    types.push_back(::rtl::OString(INIT_TYPE));
    skeletonmaker::java::generateSkeleton(
        options, *s_manager, types, ::rtl::OString());

    std::string const s(readFile(outPath("org.example.MyComp", ".java")));
    ASSERT_FALSE(s.empty()) << "no org/example/MyComp.java written";
    EXPECT_TRUE(contains(s, "package org.example")) << s;
    EXPECT_TRUE(contains(s, "XInitialization")) << s;
    EXPECT_TRUE(contains(s, "class MyComp")) << s;
}

// 7. An unresolvable type must be REPORTED, not silently generated as an empty
//    skeleton.  The library gets this right; only main() then discards it and
//    returns 0 (readme.md, "a failed dump still exits 0").  If that is ever
//    fixed upstream, this test still passes - it pins the library's contract,
//    which the fix would not change.
TEST_F(SkeletonMakerTest, UnknownTypeThrows)
{
    std::ostringstream o;
    skeletonmaker::ProgramOptions options(cppOptions());
    EXPECT_THROW(
        skeletonmaker::cpp::generateDocumentation(
            o, options, *s_manager, "com.sun.star.no.Such.Type",
            ::rtl::OString()),
        CannotDumpException);
}

}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
