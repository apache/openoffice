# Notes for stoc (done)

- javavm/javaloader/jvmfwk/jvmaccess skipped — only C++ URE components needed

## bootstrap.uno.dll
9 sub-components merged into one DLL:
bootstrap, security, servicemanager, simpleregistry, defaultregistry,
implementationregistration, loader, registry_tdprovider, tdmanager

## stocservices.uno.dll
stocservices + typeconv + uriproc

## unistd.h stub
`unistd.h` included unconditionally in implreg.cxx — satisfied by `main/soltools/winunistd/unistd.h` stub.
Add `/Imain/soltools/winunistd` to copts wherever this pattern appears in other modules.

## DEF exports
Standard UNO unloadable component pattern — same for all future component DLLs:
`component_getImplementationEnvironment`, `component_getFactory`, `component_canUnload`
