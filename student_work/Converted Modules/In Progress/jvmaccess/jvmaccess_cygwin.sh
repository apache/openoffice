source ../winenv.set.sh
cat util/msvc_win32_intel.map | awk -f ../solenv/bin/getcsym.awk > wntmsci12.pro/misc/msvc_win32_intel_jvmaccess3MSC.dxp
cat wntmsci12.pro/misc/msvc_win32_intel_jvmaccess3MSC.dxp >> wntmsci12.pro/misc/jvmaccess3MSC.def.tmpfile
rc -DWIN32 -I. -Iwntmsci12.pro/inc/jvmaccess -Iinc -Iinc/pch -Iinc -IWIN/inc -Iwntmsci12.pro/inc -I. -I../solver/410/wntmsci12.pro/inc/stl -I../solver/410/wntmsci12.pro/inc/external -I${1} -I../solver/410/wntmsci12.pro/inc -I../solenv/wntmsci12/inc -I../solenv/inc -I../res -I../solver/410/wntmsci12.pro/inc/offuh -I. -Ires -I.  wntmsci12.pro/misc/jvmaccess3MSC_def.rc
cat wntmsci12.pro/misc/jvmaccess3MSC_def.res > wntmsci12.pro/misc/jvmaccess3MSC.res