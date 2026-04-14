# Notes for xmlreader (done)

- Single DLL: `xmlreader.dll`
- Exports via `OOO_DLLIMPLEMENTATION_XMLREADER` / `SAL_DLLPUBLIC_EXPORT` — no DEF file needed
- Needs cppu_headers even though build.lst only lists `sal` — generated udkapi headers pull in cppu types
