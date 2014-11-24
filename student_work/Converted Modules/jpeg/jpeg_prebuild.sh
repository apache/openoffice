source ../winenv.set.sh
if [ -d ./wntmsci12.pro/misc/build/jpeg-8d ] ; then echo "moving" && mv ./wntmsci12.pro/misc/build/jpeg-8d ./wntmsci12.pro/misc/build/jpeg-8d_removeme ; fi
rm -rf ./wntmsci12.pro/misc/build/jpeg-8d_removeme
cd ./wntmsci12.pro/misc/build && ( gzip -d -c C:/steve/TestArea/ext_sources/52654eb3b2e60c35731ea8fc87f1bd29-jpeg-8d.tar.gz  | tar -xvf -) && C:/cygwin/bin/touch.exe so_unpacked_jpeg
cd ../../../
echo dummy > ./wntmsci12.pro/misc/build/jpeg-8d/makefile.mk.dummy
C:/cygwin/bin/touch.exe ./wntmsci12.pro/misc/build/jpeg-8d/makefile.mk.dummy
echo dummy > ./wntmsci12.pro/misc/build/jpeg-8d/makefile.mk
C:/cygwin/bin/touch.exe ./wntmsci12.pro/misc/build/jpeg-8d/makefile.mk
echo dummy > ./wntmsci12.pro/misc/build/jpeg-8d/jconfig.h.dummy
C:/cygwin/bin/touch.exe ./wntmsci12.pro/misc/build/jpeg-8d/jconfig.h.dummy
echo dummy > ./wntmsci12.pro/misc/build/jpeg-8d/jconfig.h
C:/cygwin/bin/touch.exe ./wntmsci12.pro/misc/build/jpeg-8d/jconfig.h
cd ./wntmsci12.pro/misc/build && cat ../../..//jpeg-8d.patch | patch --binary  -p2 && C:/cygwin/bin/touch.exe so_patched_jpeg
cd ../../../
sleep 2
C:/cygwin/bin/touch.exe ./wntmsci12.pro/misc/build/so_patched_jpeg
C:/cygwin/bin/touch.exe ./wntmsci12.pro/misc/build/so_configured_jpeg
cp ./wntmsci12.pro/misc/build/jpeg-8d/jconfig.h ./wntmsci12.pro/misc/build/jpeg-8d/jerror.h ./wntmsci12.pro/misc/build/jpeg-8d/jmorecfg.h ./wntmsci12.pro/misc/build/jpeg-8d/jpegint.h ./wntmsci12.pro/misc/build/jpeg-8d/jpeglib.h ./wntmsci12.pro/inc