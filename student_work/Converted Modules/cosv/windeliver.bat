mkdir "..\solver\410\wntmsci12.pro\inc\cosv"
mkdir "..\solver\410\wntmsci12.pro\inc\cosv\tpl"
xcopy "wntmsci12.pro\lib\cosv.lib" "..\solver\410\wntmsci12.pro\lib\cosv.lib" /D /Y
xcopy "inc\cosv\*.hxx" "..\solver\410\wntmsci12.pro\inc\cosv\*.hxx" /D /Y
xcopy "inc\cosv\*.h" "..\solver\410\wntmsci12.pro\inc\cosv\*.h" /D /Y
xcopy inc\cosv\tpl\*.hxx ..\solver\410\wntmsci12.pro\inc\cosv\tpl\*.hxx /D /Y
xcopy "inc\cosv\tpl\*.h" "..\solver\410\wntmsci12.pro\inc\cosv\tpl\*.h" /D /Y