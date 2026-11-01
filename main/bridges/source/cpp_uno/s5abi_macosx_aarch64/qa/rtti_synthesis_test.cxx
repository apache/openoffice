// Standalone ABI harness for the RTTI synthesis in ../except.cxx and
// ../share.hxx.  Not wired into gbuild; compile and run manually:
//
//   clang++ -std=c++11 -arch arm64 -o rtti_synthesis_test rtti_synthesis_test.cxx
//   ./rtti_synthesis_test
//
// Expected output: test 3 and both variants of test 2 report CAUGHT; only
// test 1 (the naive hand-built type_info, i.e. the approach the
// s5abi_macosx_x86-64 bridge uses) reports the catch(...) fallback.
//
// Does libc++abi on arm64 Darwin catch an exception thrown with a hand-built
// type_info?  Mirrors what bridges/.../s5abi_macosx_x86-64/except.cxx does.
#include <cxxabi.h>
#include <typeinfo>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdint>

// ---- UNO-like plain structs (no virtual functions), as css::uno::Exception is
struct UnoException { char* Message; void* Context; };
struct UnoRealDerived : UnoException { int extra; };   // typeid -> real __si_class_type_info

// ---- AOO's stand-ins, copied verbatim in shape from s5abi share.hxx ----
namespace fake {
class __class_type_info : public std::type_info {
public:
    explicit __class_type_info(const char* n) : std::type_info(n) {}
};
class __si_class_type_info : public __class_type_info {
    const __class_type_info* mpBaseType;
public:
    __si_class_type_info(const char* n, __class_type_info* b)
        : __class_type_info(n), mpBaseType(b) {}
};
}

// Itanium layout of __cxxabiv1::__si_class_type_info
struct SiLayout {
    void const* vptr;
    char const* name;
    void const* base;
};

extern "C" void* __cxa_allocate_exception(size_t) throw();
extern "C" void __cxa_throw(void*, std::type_info*, void (*)(void*)) __attribute__((noreturn));

static void nodelete(void*) {}

static void report(const char* what, int r) {
    printf("%-46s : %s\n", what,
           r == 1 ? "CAUGHT as UnoException&  (WORKS)"
                  : r == 2 ? "caught only by catch(...)  (BROKEN)"
                           : "??");
}

// Test 1: naive hand-built type_info (foreign vtable) -- what AOO x86-64 does
static int test_naive() {
    static fake::__si_class_type_info* ti =
        new fake::__si_class_type_info(
            strdup("N3com3sun4star3ucb31InteractiveAugmentedIOExceptionE"),
            (fake::__class_type_info*)const_cast<std::type_info*>(&typeid(UnoException)));
    void* e = __cxa_allocate_exception(sizeof(UnoException));
    memset(e, 0, sizeof(UnoException));
    try {
        __cxa_throw(e, (std::type_info*)ti, nodelete);
    } catch (UnoException&) { return 1; }
      catch (...)           { return 2; }
    return 0;
}

// Test 2: vtable borrowed from a real compiler-emitted __si_class_type_info
static int test_vtable_borrow(bool set_nonunique_bit) {
    SiLayout const* model =
        reinterpret_cast<SiLayout const*>(&typeid(UnoRealDerived));
    SiLayout* ti = (SiLayout*)calloc(1, sizeof(SiLayout));
    ti->vptr = model->vptr;
    char const* nm = strdup("N3com3sun4star3ucb31InteractiveAugmentedIOExceptionE");
    if (set_nonunique_bit)
        nm = (char const*)((uintptr_t)nm | (1ULL << 63));
    ti->name = nm;
    ti->base = &typeid(UnoException);
    void* e = __cxa_allocate_exception(sizeof(UnoException));
    memset(e, 0, sizeof(UnoException));
    try {
        __cxa_throw(e, (std::type_info*)ti, nodelete);
    } catch (UnoException&) { return 1; }
      catch (...)           { return 2; }
    return 0;
}

// Test 3: sanity -- a genuinely compiler-emitted derived type
static int test_real() {
    void* e = __cxa_allocate_exception(sizeof(UnoRealDerived));
    memset(e, 0, sizeof(UnoRealDerived));
    try {
        __cxa_throw(e, (std::type_info*)&typeid(UnoRealDerived), nodelete);
    } catch (UnoException&) { return 1; }
      catch (...)           { return 2; }
    return 0;
}

int main() {
    printf("_LIBCPP_TYPEINFO_COMPARISON_IMPLEMENTATION-relevant arch: %s\n",
#if defined(__APPLE__) && defined(__LP64__) && !defined(__x86_64__)
           "arm64 Apple -> NonUniqueARMRTTIBit (impl 3)");
#else
           "other");
#endif
    SiLayout const* m = reinterpret_cast<SiLayout const*>(&typeid(UnoRealDerived));
    printf("real typeid(UnoRealDerived).name ptr = %p  high bit=%d  str=%s\n",
           (void*)m->name, (int)(((uintptr_t)m->name) >> 63),
           typeid(UnoRealDerived).name());
    printf("\n");
    report("3: real compiler-emitted typeinfo (control)", test_real());
    report("1: naive hand-built (AOO x86-64 approach)",   test_naive());
    report("2a: borrowed vtable, plain name",             test_vtable_borrow(false));
    report("2b: borrowed vtable, non-unique bit set",     test_vtable_borrow(true));
    return 0;
}
