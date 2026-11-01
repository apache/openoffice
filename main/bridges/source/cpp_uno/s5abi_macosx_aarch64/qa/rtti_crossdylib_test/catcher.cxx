// See thrower.cxx for build/run instructions and expected output.
#include "uno_like.hxx"
#include <cstdio>
extern "C" void throw_synth();
int main() {
    try { throw_synth(); }
    catch (com::sun::star::ucb::InteractiveAugmentedIOException&) { printf("RESULT: caught as EXACT derived type  (WORKS)\n"); return 0; }
    catch (com::sun::star::uno::Exception&) { printf("RESULT: caught as BASE css::uno::Exception (WORKS)\n"); return 0; }
    catch (...) { printf("RESULT: caught only by catch(...)  (BROKEN)\n"); return 1; }
}
