"""import_lib_only: hand out a DLL's import library and nothing else.

//main/staging's collect_files_aspect follows deps/srcs/data/exports/
additional_linker_inputs on purpose: a DLL whose import library you link is a
DLL you must ship.  The jawt stub is the one exception — the real jawt.dll
belongs to the JVM, and a stub staged into program/ beside officebean.dll
would WIN the import resolution (Java loads officebean.dll by full path with
the altered search order, so its own directory is searched first) and make
JAWT_GetAWT return false.  Holding the DLL target in an attribute the aspect
does not traverse makes the stub unreachable from staging by construction,
rather than by a filter someone has to remember.
"""

def _import_lib_only_impl(ctx):
    libs = ctx.attr.dll[OutputGroupInfo].interface_library
    return [DefaultInfo(files = libs)]

import_lib_only = rule(
    implementation = _import_lib_only_impl,
    attrs = {
        # Deliberately NOT named deps/srcs/data/exports/additional_linker_inputs.
        "dll": attr.label(mandatory = True),
    },
)
