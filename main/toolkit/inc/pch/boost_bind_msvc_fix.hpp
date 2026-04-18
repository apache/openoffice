#ifndef TOOLKIT_BOOST_BIND_MSVC_FIX_HPP
#define TOOLKIT_BOOST_BIND_MSVC_FIX_HPP

// MSVC 9.0 (VS2008) encodes calling convention into member-function-pointer
// types for template argument deduction.  SAL_CALL = __cdecl, but boost::bind
// only generates __cdecl mf-wrapper variants (mf1_cdecl etc.) when
// BOOST_MEM_FN_ENABLE_CDECL is defined.  Without it, no bind_mf_cc.hpp
// overload matches __cdecl method pointers and the call falls through to the
// generic bind(F,...) path which then fails in result_traits<unspecified,F>.
//
// Defining BOOST_MEM_FN_ENABLE_CDECL before including boost/bind.hpp causes
// boost to emit the __cdecl overloads.  MSVC then allows implicit conversion
// from throw(RuntimeException) to no-throw-spec pointers within the same
// calling convention, so those overloads also cover UNO listener methods.
//
// The original AOO build used MinGW/GCC which ignores both calling convention
// and exception specs in template type matching, so no fix was needed there.

#ifdef _MSC_VER
#  define BOOST_MEM_FN_ENABLE_CDECL
#endif

#endif // TOOLKIT_BOOST_BIND_MSVC_FIX_HPP
