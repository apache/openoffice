#**************************************************************
#
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#
#**************************************************************

# mk file for $(COMEX) == 14 -- the UCRT compiler generation, i.e. VS2015
# (cl 19.00) and every toolset since.
#
# This is included at the END of wntmsci11.mk (x86) and wntmscx.mk (x64), so
# it holds ONLY what differs from VC9 and nothing the two generations share.
# Two things follow from that shape and both are deliberate:
#
#   * a VC9 build is byte-identical.  Nothing here is reached at COMEX 12 --
#     there is no condition inside this file, because the file itself is the
#     condition.
#
#   * the two architectures cannot drift.  Every line below is either
#     architecture-neutral or branches on $(CPU) in one place, rather than
#     being written out twice.

# --- C++ dialect ---------------------------------------------------------

# C++14, not 17.  Three things in this tree depend on the dialect and all
# three say 14:
#
#   * std::tr1.  The MSVC standard library still ships it, but only while
#     _HAS_CXX17 is 0 -- and boost/tr1 and the shims in main/stlport reach for
#     std::tr1 by name.
#   * the empty exception specification.  throw() is deprecated in C++17 and
#     removed in C++20, and there are still thousands of them here; migrating
#     them to noexcept is a separate change.
#   * <hash_map> and <hash_set>, which a modern MSVC turns into a hard #error
#     under C++17.
#
# There is no /std:c++03 to fall back to -- a modern cl rejects it outright
# (D9002) -- so C++14 is the floor whether or not it is chosen.
CFLAGSCXX+=-std:c++14

# Report the real __cplusplus.  Without this MSVC reports 199711L whatever
# /std: says, and the shims in main/stlport/systemstl branch on
# __cplusplus >= 201103L to decide whether <hash_map> can forward to
# <unordered_map>.  Left off, they take the pre-C++11 branch and reach for
# boost/tr1 instead.
CFLAGSCXX+=-Zc:__cplusplus

# --- the CRT -------------------------------------------------------------

# The two halves of the old snprintf shim are NOT symmetric, which is easy to
# get wrong in either direction:
#
#   * snprintf -- the UCRT declares a real, conforming one.  Defining the name
#     away would be a redefinition with different linkage that it refuses to
#     compile (C1189), so the shim is dropped.
#     sal/inc/systools/win32/snprintf.h drops its own declarations on the same
#     condition, and so does uwinapi's implementation of them.
#   * snwprintf -- the UCRT declares no such thing.  The wide C99 name has
#     never existed in any MSVC CRT; only _snwprintf does.  So this half stays,
#     and the bare calls in svx's sendreportw32.cxx and framework's
#     spinfieldtoolbarcontroller.cxx keep compiling.
CDEFS+=-Dsnwprintf=_snwprintf

# stdext::hash_map and stdext::hash_set are deprecated to the point of a hard
# #error.  The tree reaches them through the stlport shims in a great many
# places; these are the stdext containers, not std::unordered_map, so
# replacing them is a refactor and not a build fix.
CDEFS+=-D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

# The CRT split in three when the UCRT arrived: msvcrt.lib is the startup and
# import library, vcruntime.lib covers the compiler runtime, ucrt.lib the C
# library proper.  Normally cl emits /DEFAULTLIB directives naming all three --
# but this build links with -NODEFAULTLIB, so anything not named here is not
# linked.  msvcrt.lib and msvcprt.lib are already in LIBCMT from the base file.
LIBCMT+=vcruntime.lib ucrt.lib

# --- what the modern toolchain removed -----------------------------------

# -Yd (write debug info into every object that uses the precompiled header)
# was removed after VC9.  -Z7 alone already does what this build wants, which
# is debug information inside the object rather than in a separate .pdb that
# every compile would have to serialise on.
CFLAGSENABLESYMBOLS=-Z7

# ATL and MFC moved out of the Platform SDK and into the toolset, and their
# libraries gained the same per-architecture subdirectory the toolset's own
# libraries have.  These are set with *= in the base file, i.e. only if unset,
# so they need an unconditional assignment here rather than an append.
.IF "$(CPU)" == "X"
ATL_LIB:=$(COMPATH)/atlmfc/lib/x64
MFC_LIB:=$(COMPATH)/atlmfc/lib/x64
.ELSE
ATL_LIB:=$(COMPATH)/atlmfc/lib/x86
MFC_LIB:=$(COMPATH)/atlmfc/lib/x86
.ENDIF
ATL_INCLUDE:=$(COMPATH)/atlmfc/include
MFC_INCLUDE:=$(COMPATH)/atlmfc/include

# --- warnings ------------------------------------------------------------

# The base warning list was written against VC9 and is kept as it is; these
# are the ones a modern cl adds in volume, and each is a report about old
# code rather than about a defect this build introduced:
#
#   C4996 -- "was declared deprecated".  The tree calls the POSIX and
#            non-secure CRT names throughout.  _CRT_SECURE_NO_DEPRECATE and
#            _CRT_NONSTDC_NO_DEPRECATE, already set in the base file, silence
#            most but not all of them.
#   C4577 -- "noexcept used with no exception handling mode specified".  This
#            build compiles with -EHa, which does specify one; the warning
#            fires anyway on the empty specifications the tree is still full
#            of.
#   C5040 -- an exception specification on a function pointer type is
#            non-standard.  Same population, same reason.
CFLAGSWARNCXX+=-wd4996 -wd4577 -wd5040
CFLAGSWARNCC+=-wd4996
CFLAGSWALLCC=$(CFLAGSWARNCC)
CFLAGSWALLCXX=$(CFLAGSWARNCXX)
