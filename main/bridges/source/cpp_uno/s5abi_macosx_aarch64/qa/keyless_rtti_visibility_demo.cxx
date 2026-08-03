// Demonstrates why arm64 Darwin needs the RTTI synthesis in except.cxx: a
// class with no key function (every UNO exception, since they declare no
// virtual functions) gets hidden ("weak private external") typeinfo on
// arm64 regardless of visibility attributes, whereas the same class gets
// exported ("weak external") typeinfo on x86_64.  Compare:
//
//   clang++ -arch arm64  -c keyless_rtti_visibility_demo.cxx && nm -m keyless_rtti_visibility_demo.o | grep __ZTI
//   clang++ -arch x86_64 -c keyless_rtti_visibility_demo.cxx && nm -m keyless_rtti_visibility_demo.o | grep __ZTI
//
// A/B (keyless) are hidden on arm64 and exported on x86_64; C/D (have a key
// function) are exported on both, with or without the visibility attribute.
struct A { int x; };
struct __attribute__((visibility("default"))) B { int x; };
struct C { int x; virtual void key(); };
void C::key() {}
struct __attribute__((visibility("default"))) D { int x; virtual void key(); };
void D::key() {}
#include <typeinfo>
const void* refs[] = { &typeid(A), &typeid(B), &typeid(C), &typeid(D) };
