# javaunohelper — Bazel migration

## Outputs

| Target | File | Description |
|--------|------|-------------|
| `:juh_jar` | juh.jar | Java UNO helpers, adapters, bootstrap |
| `:juh` | juh.dll | Preload DLL — thin JNI forwarder loaded by Java |
| `:juhx` | juhx.dll | Real JNI implementation (loaded by juh.dll at runtime) |

## Architecture

`juh.dll` is the DLL loaded by Java via `System.loadLibrary("juh")`. On the first JNI call it lazily loads `juhx.dll` from the same directory via `osl_loadModuleRelative` and resolves the 4 exported JNI symbols by name. This indirection allows juhx.dll to carry the heavy dependencies (cppu, cppuhelper, jvmaccess) while juh.dll only needs sal.

Both DLLs export the same 4 JNI entry points defined in `util/juh.def` / `util/juhx.def`:
- `Java_com_sun_star_comp_helper_SharedLibraryLoader_component_1writeInfo`
- `Java_com_sun_star_comp_helper_SharedLibraryLoader_component_1getFactory`
- `Java_com_sun_star_comp_helper_RegistryServiceFactory_createRegistryServiceFactory`
- `Java_com_sun_star_comp_helper_Bootstrap_cppuhelper_1bootstrap`

## Key flags

- `/Zc:wchar_t-` — required: sal_Unicode is `unsigned short`, not native `wchar_t`
- `SOLAR_JAVA` — activates JVM/JNI code paths in sal/cppu headers
- `CPPU_ENV=msci` — selects the MSVC C++ UNO environment

## Dependencies

**juh.jar**: `ridljar:ridl`, `jurt:jurt`, `unoil:unoil`

**juhx.dll**: sal, cppu, cppuhelper, salhelper, jvmaccess, udkapi headers
