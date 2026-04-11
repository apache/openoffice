All 5 io DLLs built successfully on the first try (after the transient PDB race on streams).

Key insight: /Zc:wchar_t- is needed for any module that uses Sequence<sal_Unicode> — without it VS2008 treats wchar_t as a native type, which breaks the UNO type-system overload resolution.