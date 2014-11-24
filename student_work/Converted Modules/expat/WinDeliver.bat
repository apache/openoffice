mkdir  ..\solver\410\wntmsci12.pro\inc\external\expat 
mkdir  ..\solver\410\wntmsci12.pro\lib\x64 
xcopy "wntmsci12.pro\misc\build\expat-2.1.0\lib\expat.h" "..\solver\410\wntmsci12.pro\inc\external\" /D /Y /c
xcopy "wntmsci12.pro\misc\build\expat-2.1.0\lib\expat_external.h" "..\solver\410\wntmsci12.pro\inc\external\" /D /Y /c
xcopy "wntmsci12.pro\slb\expat_xmltok.lib" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\slb\expat_xmlparse.lib" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\slb\ascii_expat_xmlparse.lib" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\lib\libexpat_xmltok.a" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\lib\libexpat_xmlparse.a" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\lib\libascii_expat_xmlparse.a" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\misc\_ooo_st_expat_xmltok.pdb" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\misc\_ooo_st_expat_xmlparse.pdb" "..\solver\410\wntmsci12.pro\lib\" /D /Y /c
xcopy "wntmsci12.pro\slb\x64\expat_xmltok.lib" "..\solver\410\wntmsci12.pro\lib\x64\" /D /Y /c
xcopy "wntmsci12.pro\slb\x64\expat_xmlparse.lib" "..\solver\410\wntmsci12.pro\lib\x64\" /D /Y /c
