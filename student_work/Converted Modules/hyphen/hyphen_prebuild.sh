source ../winenv.set.sh
if [ -d ./wntmsci12.pro/misc/build/hyphen-2.7.1 ] ; then echo "moving" && mv ./wntmsci12.pro/misc/build/hyphen-2.7.1 ./wntmsci12.pro/misc/build/hyphen-2.7.1_removeme ; fi
rm -rf ./wntmsci12.pro/misc/build/hyphen-2.7.1_removeme
mkdir -p wntmsci12.pro/misc/build/hyphen-2.7.1
echo dummy > wntmsci12.pro/misc/build/hyphen-2.7.1/makefile.mk.dummy
echo dummy > wntmsci12.pro/misc/build/hyphen-2.7.1/makefile.mk
cd ./wntmsci12.pro/misc/build && ( gzip -d -c C:/steve/TestArea/ext_sources/48a9f787f43a09c0a9b7b00cd1fddbbf-hyphen-2.7.1.tar.gz  | tar -xvf -) && C:/cygwin/bin/touch.exe so_unpacked_hyphen
cd ../../../
C:/cygwin/bin/touch.exe wntmsci12.pro/misc/build/hyphen-2.7.1/makefile.mk.dummy
C:/cygwin/bin/touch.exe wntmsci12.pro/misc/build/hyphen-2.7.1/makefile.mk
cd ./wntmsci12.pro/misc/build && cat ../../..//hyphen-2.7.1.patch ../../..//hyphen-2.7.1-read-charset.patch | patch --binary  -p2 && C:/cygwin/bin/touch.exe so_patched_hyphen
cd ../../../
sleep 2
C:/cygwin/bin/touch.exe ./wntmsci12.pro/misc/build/so_patched_hyphen
C:/cygwin/bin/touch.exe ./wntmsci12.pro/misc/build/so_configured_hyphen