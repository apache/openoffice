# What flex/bison buy you for IDL development
Grammar maintainability — parser.y is ~1500 lines of declarative grammar rules. If you needed to add new IDL constructs (new keywords, new type categories, new annotations), editing a bison grammar is far cleaner than maintaining a hand-written recursive descent parser.

Proven correctness — LALR(1) grammars catch shift/reduce conflicts at grammar-compile time, forcing ambiguities to be resolved explicitly.

# Would you build idlc this way today?
Probably not with flex/bison. Modern alternatives:

| Approach |	Notes | 
| ANTLR4	| Better error recovery, IDE grammars, visitor/ listener pattern, widely supported | 
|Hand-written recursive descent	| What clang/go/rustc do — better error messages, zero build deps, ~2000 lines for a grammar this simple| 
| Skip idlc entirely	| If designing UNO today you'd use protobuf or Cap'n Proto — the entire IDL → RDB → generated headers stack would collapse to protoc| 

The idlc grammar will never be modified during this migration, and realistically never again in AOO's lifetime.

# Verdict for your migration
This is a final state. Generate parser.cxx/scanner.cxx once from Cygwin while it's available, check them in, and remove flex/bison from the dependency graph permanently. That's actually the right long-term answer even from a software architecture standpoint — there's no future investment to protect here.