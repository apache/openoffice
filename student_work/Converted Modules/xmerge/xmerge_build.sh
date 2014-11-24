source ../winenv.set.sh
cd source/${1}
export PATH=$PATH:"/bin/"
C:/Apache-Ant/apache-ant-1.9.2/bin/ant -Dprj=../.. -Dprjname=xmerge -Ddebug=off -Doptimize=on  -Dtarget=htmlsoff -Dsolar.update=on -Dout=../../wntmsci12.pro -Dinpath=wntmsci12.pro -Dproext=".pro"  -Dsolar.bin=../solver/410/wntmsci12.pro/bin -Dsolar.jar=../solver/410/wntmsci12.pro/bin -Dsolar.doc=../solver/410/wntmsci12.pro/doc  -Dcommon.jar=../solver/410/wntmsci12.pro/bin -Dcommon.doc=  -Dsolar.sourceversion=AOO410 -Dsolar.lastminor=m1  -Dsolar.build=9750 -f build.xml  -emacs