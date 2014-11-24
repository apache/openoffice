source ../winenv.set.sh
cd source/bridge
${ANT} -Dprj=../.. -Dprjname=xmerge -Ddebug=off -Doptimize=on  -Dtarget=XMergeBridge -Dsolar.update=on -Dout=../../wntmsci12.pro -Dinpath=wntmsci12.pro -Dproext=".pro"  -Dsolar.bin=../../../solver/410/wntmsci12.pro/bin -Dsolar.jar=../../../solver/410/wntmsci12.pro/bin -Dsolar.doc=../../../solver/410/wntmsci12.pro/doc  -Dcommon.jar=../../../solver/410/wntmsci12.pro/bin -Dcommon.doc=  -Dsolar.sourceversion=AOO410 -Dsolar.lastminor=m1  -Dsolar.build=9750 -f build.xml  -emacs
../../../solver/410/wntmsci12.pro/bin/xsltproc --nonet --stringparam uri \
'vnd.sun.star.expand:$OOO_BASE_DIR/program/classes/XMergeBridge.jar' -o ../../wntmsci12.pro/misc/XMergeBridge.component \
../../../solenv/bin/createcomponent.xslt XMergeBridge.component