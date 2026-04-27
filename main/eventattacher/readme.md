# eventattacher — Bazel Migration Notes

## Output
- `evtatt.dll` — UNO EventAttacher service (`com.sun.star.script.EventAttacher`)

## Build target
```
bazel build //main/eventattacher:evtatt
```

## Structure
Single source file: `source/eventattacher.cxx`. No exported headers, no local includes.

## Key decisions
- `cppuhelper_headers` transitively provides sal, cppu, and udkapi deps — only `offapi_idl_headers` added explicitly for `com/sun/star/script/*`.
- DEF file at `util/evtatt.def` exports only the two symbols present in source (`component_getImplementationEnvironment`, `component_getFactory`); no `component_canUnload` as source does not implement it.
- `/Zc:wchar_t-` added as the component uses OUString and UNO sequences.
