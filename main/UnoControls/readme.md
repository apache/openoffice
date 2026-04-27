# UnoControls — Bazel Migration

## Output
`ctl.dll` — UNO control component (ProgressBar, ProgressMonitor, FrameControl, StatusIndicator)

## Build target
```
bazel build //main/UnoControls:ctl
```

## Dependencies
- `//main/cppuhelper:cppuhelper_headers` — factory/typeprovider headers
- `//main/offapi:offapi_idl_headers` — `com/sun/star/awt`, `com/sun/star/frame`, etc.
- `//main/tools:tools_headers` — `tools/debug.hxx`, `tools/solar.h`
- Implibs: sal, cppu3, cppuhelper3MSC, tl

## Notes
- No .map file existed; `util/ctl.def` exports `component_getImplementationEnvironment` and `component_getFactory`
- Include paths: `main/UnoControls/inc` (public) and `main/UnoControls/source/inc` (private)
- The build.lst listed `libxslt` and `toolkit`/`vcl` as ordering deps; neither is a compile-time dependency of ctl.dll
