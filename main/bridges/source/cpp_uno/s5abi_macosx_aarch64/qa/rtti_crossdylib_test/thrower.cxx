// Cross-image ABI harness: synthesizes RTTI for a whole exception chain
// (neither type dlsym-able, mirroring how the real bridge is used) and
// throws it from a separate dylib.  Not wired into gbuild; build and run
// manually:
//
//   clang++ -std=c++11 -arch arm64 -dynamiclib -o libthrower.dylib thrower.cxx \
//           -install_name @rpath/libthrower.dylib
//   clang++ -std=c++11 -arch arm64 -o catcher catcher.cxx -L. -lthrower \
//           -Wl,-rpath,@executable_path
//   ./catcher
//
// Expected: "RESULT: caught as EXACT derived type  (WORKS)".
#include "uno_like.hxx"
#include <typeinfo>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
extern "C" void* __cxa_allocate_exception(size_t) throw();
extern "C" void __cxa_throw(void*, std::type_info*, void (*)(void*)) __attribute__((noreturn));
// vtable donors: real, compiler-emitted, in THIS dylib
namespace { struct DonorBase { int a; }; struct DonorDerived : DonorBase { int b; }; }
struct ClassTi { void const* vptr; uintptr_t name; };
struct SiTi    { void const* vptr; uintptr_t name; void const* base; };
static const uintptr_t NONUNIQUE = 1ULL << 63;
static uintptr_t mkname(const char* s){ return (uintptr_t)strdup(s) | NONUNIQUE; }
static std::type_info* synth_base() {
    static ClassTi* t = 0;
    if (!t) { t = (ClassTi*)calloc(1,sizeof(ClassTi));
        t->vptr = ((ClassTi const*)&typeid(DonorBase))->vptr;
        t->name = mkname("N3com3sun4star3uno9ExceptionE"); }
    return (std::type_info*)t;
}
static std::type_info* synth_derived() {
    static SiTi* t = 0;
    if (!t) { t = (SiTi*)calloc(1,sizeof(SiTi));
        t->vptr = ((SiTi const*)&typeid(DonorDerived))->vptr;
        t->name = mkname("N3com3sun4star3ucb31InteractiveAugmentedIOExceptionE");
        t->base = synth_base(); }
    return (std::type_info*)t;
}
static void nodel(void*) {}
extern "C" __attribute__((visibility("default"))) void throw_synth() {
    void* e = __cxa_allocate_exception(sizeof(com::sun::star::ucb::InteractiveAugmentedIOException));
    memset(e,0,sizeof(com::sun::star::ucb::InteractiveAugmentedIOException));
    __cxa_throw(e, synth_derived(), nodel);
}
