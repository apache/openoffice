"""The bundled CPython's version, for everything that spells it.

It names the python-core-<ver> directory the office installs under program/,
which the launcher (zipcore/python.cxx, via pyversion.hxx), the staging layout
and the generated pythonloader.uno.ini (both //main/staging) must agree on.
It must match the `python` bazel_dep in MODULE.bazel.
"""

PYTHON_VERSION = "3.11.15"
