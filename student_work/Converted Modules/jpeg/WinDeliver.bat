mkdir  ..\solver\410\wntmsci12.pro\inc\external 
mkdir  ..\solver\410\wntmsci12.pro\inc\external\jpeg 
xcopy "wntmsci12.pro\slb\jpeglib.lib" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\slb\jpeglib.lib" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\lib\libjpeglib.a" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\lib\libjpeglib.a" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\inc\jerror.h" "..\solver\410\wntmsci12.pro\inc\external\jpeg\" /D /Y /c
xcopy "wntmsci12.pro\inc\jpeglib.h" "..\solver\410\wntmsci12.pro\inc\external\jpeg\" /D /Y /c
xcopy "wntmsci12.pro\inc\jconfig.h" "..\solver\410\wntmsci12.pro\inc\external\jpeg\" /D /Y /c
xcopy "wntmsci12.pro\inc\jmorecfg.h" "..\solver\410\wntmsci12.pro\inc\external\jpeg\" /D /Y /c
echo nul > wntmsci12.pro\misc\build\so_installed_jpeg