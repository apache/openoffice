"""VS2008 and Windows SDK path constants — single source of truth.

Update these when VS or the SDK is installed at a different location.
All other build files load from here so only this file needs changing.
"""

VS  = "C:\\Program Files (x86)\\Microsoft Visual Studio 9.0"
VC  = VS + "\\VC"
SDK = "C:\\Program Files\\Microsoft SDKs\\Windows\\v7.0"

# Non-redistributable debug CRT (MSVCR90D.dll, MSVCP90D.dll).
# Only present on machines with a full VS2008 installation.
# Required at runtime when --compilation_mode=dbg is active (/MDd).
DEBUG_CRT_DIR = VC + "\\redist\\Debug_NonRedist\\x86\\Microsoft.VC90.DebugCRT"
