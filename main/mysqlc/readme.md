# mysqlc — Bazel migration notes

## Output
- `mysqlc.uno.dll` — UNO SDBC driver for MySQL/MariaDB, registered as `sdbc:mysqlc:*`

## Build target
```
bazel build //main/mysqlc:mysqlc
```

No manual downloads required.  Both external libraries are fetched and built
from source automatically by Bazel.

## External dependencies (fully automated)

### MySQL Connector/C++ 1.1.12
- Registry module: `ext_libraries/modules/mysqlcppconn/1.1.12/`
- Source: `https://github.com/mysql/mysql-connector-cpp/archive/refs/tags/1.1.12.tar.gz`
- Built as a native Bazel `cc_library` (no cmake); avoids `FindMySQL.cmake`
  library-name probing issues when the backing client is MariaDB.

### MariaDB Connector/C 2.3.7
- Registry module: `ext_libraries/modules/mariadb-connector-c/v_2.3.7/`
- Source: `https://github.com/mariadb-corporation/mariadb-connector-c/archive/refs/tags/v_2.3.7.zip`
- Built as a **static** lib via `cmake()` from rules_foreign_cc.
- ABI-compatible drop-in replacement for Oracle's `libmysql.dll`.
- Static link means no extra runtime DLL; mariadb code is embedded in
  `mysqlc.uno.dll` via `mysqlcppconn`.
- Choosing the 2.x series (not 3.x) for VS2008 (MSVC 9) compatibility:
  all variable declarations are at block start (C89 rule).

## Key compile flags
| Flag | Reason |
|------|--------|
| `CPPDBC_EXPORTS` / `CPPCONN_LIB_BUILD` | Symbol visibility for Connector/C++ headers |
| `SYSTEM_MYSQL` | Skips dynamic DLL loading of the MySQL client at runtime; correct for static link |
| `MYSQLC_VERSION_MAJOR/MINOR/MICRO` | Embedded in the driver info string |
| `/Zc:wchar_t-` | sal_Unicode = unsigned short, VS2008 native wchar_t incompatible |
| `snprintf=_snprintf` | VS2008 CRT has only `_snprintf`, not `snprintf` |
| `/NODEFAULTLIB:LIBCMT` | Prevents static CRT from mariadb's cmake build conflicting with /MD |

## DEF exports
`source/mysqlc.def` (converted from `source/mysqlc.map`):
- `component_getImplementationEnvironment`
- `component_writeInfo`
- `component_getFactory`

## stlport headers
`preextstl.h` / `postextstl.h` guard Connector/C++ headers from STLport namespace
leakage.  They live in `main/stlport/systemstlguards/`; the stlport BUILD was
updated to export that path via `includes = ["systemstlguards"]`.

## Overlay traps

### cmake-generated headers (mysqlcppconn)
Four headers that cmake normally generates must be provided as overlays:
- `cppconn/config.h` — feature-detection flags
- `cppconn/version_info.h` — DM version + `MYCPPCONN_STATIC_MYSQL_VERSION_ID=50502`
- `driver/version_info.h` — `MYCPPCONN_STRVERSION` etc. (included as `"version_info.h"`)
- `driver/nativeapi/binding_config.h` — `MYSQLCLIENT_STATIC_BINDING=1`

### libmysql_static_proxy.cpp must be excluded from glob
`mysql_client_api.cpp` does `#include "libmysql_static_proxy.cpp"` at line 85
when `MYSQLCLIENT_STATIC_BINDING=1`.  Compiling it as a separate source too
produces ~60 LNK4006 duplicate-symbol warnings.  Exclude it in the BUILD glob.

### MariaDB missing MySQL 5.6+ enum members
MariaDB 2.3.7 targets MySQL 5.5 compat.  The `mysql_option` enum is missing:
- `MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS` (MySQL 5.6.11) — used with `::` scope
- `MYSQL_OPT_SSL_MODE` (MySQL 5.7.11) — used without version guard

Both must be added to `include/mysql.h` as real enum members (not macros), because
`mysql_native_connection_wrapper.cpp` uses the `::MYSQL_OPT_*` global-scope qualifier
which is satisfied only by a real enum member, not a `#define`.

### MariaDB missing CR_INVALID_BUFFER_USE (errmsg.h)
`mysql_prepared_statement.cpp` uses `CR_INVALID_BUFFER_USE` (MySQL value 2035).
MariaDB's errmsg.h jumps from 2034 to 2036.  Add it via `include/errmsg.h` overlay.

### LIBCMT vs msvcrt conflict
MariaDB's cmake build embeds `LIBCMT` as a default lib in its `.obj` files.
Fix: `/MD` in `CMAKE_C_FLAGS` for the mariadb cmake target, plus
`/NODEFAULTLIB:LIBCMT` in mysqlc's `linkopts`.

# current build
we baked everything into this extension.
## todo
- change the static build into a dynamic one.
Build mysqlcppconn as a shared cc_binary (DLL) instead of a static cc_library — then mariadb is inside that DLL, not mysqlc.dll
Drop SYSTEM_MYSQL and restore MYSQL_LIB="mysqlcppconn.dll" (or whichever name) — then mysqlc.dll dlopens the connector at startup
The extension packages both mysqlc.dll + mysqlcppconn.dll (or libmariadb.dll directly, if using the dynamic mariadb build)
- packaging is to an extension is missing.