# embeddedobj — Bazel migration

Produces two UNO component DLLs:

| Target | DLL | Purpose |
|--------|-----|---------|
| `embobj` | embobj.dll | Generic OLE/embedded object factories (OOo-native) |
| `emboleobj` | emboleobj.dll | Windows MS-OLE embedded object implementation |

## embobj.dll
Sources from `source/commonembedding/` and `source/general/`.  
Registers: `EmbeddedObjectCreator`, `OOoEmbeddedObjectFactory`, `OOoSpecialEmbeddedObjectFactory`.  
Deps: sal, cppu, cppuhelper, comphelper.

## emboleobj.dll
Sources from `source/msole/` (Windows-only files all included).  
Registers: `MSOLEObjectSystemCreator`, `OLEEmbeddedObjectFactory`.  
Deps: sal, cppu, cppuhelper, comphelper, tl, utl, vos3 + gdi32/ole32/oleaut32/user32/uuid.

## DEF files
Both DLLs export the standard two-symbol UNO component interface:
`component_getFactory`, `component_getImplementationEnvironment`.
