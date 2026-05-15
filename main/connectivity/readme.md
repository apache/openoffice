# connectivity — Bazel Migration

## Targets

| Target | Output | Description |
|--------|--------|-------------|
| `//main/connectivity:dbtools` | `dbtools.dll` | Core database tools library (SQL parser, schema classes, common utilities) |
| `//main/connectivity:sdbc2` | `sdbc2.dll` | SDBC driver manager UNO component |
| `//main/connectivity:dbpool2` | `dbpool2.dll` | Connection pool UNO component |

## Source layout

- `source/commontools/` — 40 .cxx files: shared DB utilities, metadata helpers, FValue, parameters
- `source/sdbcx/` — 12 .cxx files: schema/catalog/table/key/index/view classes
- `source/parse/` — SQL parser (bison + flex generated) + iterator/node types
- `source/simpledbt/` — simplified dbtools interface classes
- `source/resource/` — shared resource bundle helper
- `source/manager/` — SDBC driver manager (`sdbc2.dll`)
- `source/cpool/` — connection pool (`dbpool2.dll`)

## SQL parser pre-generation

`sqlbison.cxx` / `sqlbison.hxx` and `sqlflex.cxx` are pre-generated from the
grammar sources and committed to the tree. To regenerate (requires Cygwin bison/flex):

```bash
cd main/connectivity/source/parse
bison -v -d -l -pSQLyy -o sqlbison.cxx sqlbison.y
flex  -i -8 -PSQLyy -L -o sqlflex.cxx sqlflex.l
```

`wrap_sqlbison.cxx` and `wrap_sqlflex.cxx` `#include` the generated `.cxx`
files as textual fragments (declared via `_parse_generated` cc_library).

## Key build notes

- `OOO_DLLIMPLEMENTATION_DBTOOLS` enables `SAL_DLLPUBLIC_EXPORT` in public headers
- `BOOST_SPIRIT_USE_OLD_NAMESPACE` required by `RowFunctionParser.cxx` (boost::spirit classic)
- `CONN_SHARED_RESOURCE_FILE=cnr` names the shared resource bundle
- `/Imain/soltools/winunistd` needed because flex-generated `sqlflex.cxx` includes `<unistd.h>`
- stlport required by `sdbc2` for `std::unary_compose` (SGI extension)
- `_HAS_ITERATOR_DEBUGGING=0` on `sdbc2`: `mdrivermanager.cxx` uses a heterogeneous
  comparator with `std::equal_range`; MSVC debug STL validates the range by calling
  `comp(DriverAccess, DriverAccess)` which the comparator doesn't provide — semantically
  correct C++ but the MSVC debug probe trips on it
- Drivers (ado, odbc, dbase, calc, flat, hsqldb, jdbc, mysql, …) are not yet migrated
