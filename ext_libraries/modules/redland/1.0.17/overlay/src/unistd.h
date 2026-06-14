/* Dummy unistd.h for the MSVC (Windows) build.
 *
 * The flex-generated lexers (turtle_lexer.h, ntriples, etc.) do an
 * unconditional `#include <unistd.h>`.  MSVC has no unistd.h; the dmake build
 * satisfied this with the empty stub delivered by main/soltools/winunistd on
 * the solver include path (-I$(PRJ)/$(INPATH)/inc).  Cross-repo `-I main/...`
 * is fragile under Bazel, so this overlay reproduces the same empty stub on
 * raptor2's own include path (-Isrc).
 */
