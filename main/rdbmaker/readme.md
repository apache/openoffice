They're genuinely different implementations — rdbmaker has an older fork of the codemaker library:

Aspect	rdbmaker/inc/codemaker/	codemaker/inc/codemaker/
Files	dependency, global, options, registry, typemanager	More files: unotype, dependencies, generatedtypeset, etc.
typemanager.hxx	Old-style: TypeReader (deprecated), m_pMergedRegistry, init(bMerge, regFiles)	New-style: typereg::Reader, getTypeKeys(), init(regFiles, extraFiles)
API shape	Older, simpler interface	Extended with isBaseType(), extra-type support, getTypeClass(RegistryKey&)
rdbmaker predates the codemaker refactor — it carries its own frozen copy of the old codemaker/ API. They cannot share headers; rdbmaker must use its own /Imain/rdbmaker/inc.

The BUILD.bazel is correct — //main/codemaker:codemaker_headers is intentionally not in the deps.