###############################################################
#
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#
###############################################################


# ========== target function modifiers  ==========
# target modification, to allow "make prepare" instead of "make prepare all"
MODULE_TARGET := $(filter-out prepare clean genPO, $(MAKECMDGOALS))
ifeq ($(MODULE_TARGET),)
  MODULE_TARGET := all
endif
SUB_TARGET := $(filter prepare clean genPO, $(MAKECMDGOALS))

MAKE_CALL= @echo $@ $(SUB_TARGET)

all: instsetoo_native

prepare clean genPO: $(MODULE_TARGET)



# ========== special targets (outside main) ==========
l10n : l10ntools
	$(MAKE_CALL) ../extras

ratscan hunspell coinmp apr serf apr-util:
	$(MAKE_CALL) ../ext_libraries



# ========== module targets ==========
afms : solenv
	$(MAKE_CALL)

accessibility :    l10n tools jurt offuh unoil vcl javaunohelper jvmaccess cppu sal toolkit svtools libxslt
	$(MAKE_CALL)

animations : cppuhelper comphelper offuh libxslt
	$(MAKE_CALL)

apache-commons : tomcat
	$(MAKE_CALL)

apple_remote : soltools
	$(MAKE_CALL)

autodoc :  cosv udm sal
	$(MAKE_CALL)

automation : basic fpicker
	$(MAKE_CALL)

avmedia :    l10n tools sfx2 libxslt
	$(MAKE_CALL)

basctl : libxslt l10n svx
	$(MAKE_CALL)

basebmp : sal vigra basegfx boost
	$(MAKE_CALL)

basegfx : o3tl sal offuh cppuhelper cppu
	$(MAKE_CALL)

basic :    l10n offuh oovbaapi svtools xmlscript framework salhelper libxslt
	$(MAKE_CALL)

bean : javaunohelper jurt ridljar unoil offapi udkapi
	$(MAKE_CALL)

beanshell : solenv
	$(MAKE_CALL)

binaryurp : boost libxslt cppu cppuhelper offuh sal salhelper stlport
	$(MAKE_CALL)

boost : solenv
	$(MAKE_CALL)

bridges : cppuhelper jurt jvmaccess salhelper
	$(MAKE_CALL)

cairo : zlib soltools stlport
	$(MAKE_CALL)

canvas : javaunohelper comphelper cppuhelper offuh unoil tools svtools vcl basegfx cairo libxslt
	$(MAKE_CALL)

chart2 :    offapi offuh l10n comphelper cppu cppuhelper sal svtools svx tools vcl toolkit unotools sfx2 libxslt
	$(MAKE_CALL)

cli_ure :       cppu cppuhelper sal codemaker stoc udkapi bridges
	$(MAKE_CALL)

codemaker : udkapi
	$(MAKE_CALL)

comphelper : cppuhelper ucbhelper offuh vos salhelper libxslt
	$(MAKE_CALL)

configmgr : boost libxslt comphelper cppu cppuhelper offuh sal salhelper stlport xmlreader
	$(MAKE_CALL)

connectivity    :    shell  l10n comphelper moz  svl unixODBC unoil javaunohelper hsqldb qadevOOo officecfg nss libxslt
	$(MAKE_CALL)

cosv : soltools stlport
	$(MAKE_CALL)

cppcanvas : comphelper cppuhelper offuh tools vcl basegfx canvas
	$(MAKE_CALL)

cppuhelper : boost libxslt codemaker cppu offuh
	$(MAKE_CALL)

cppu : sal salhelper offuh boost
	$(MAKE_CALL)

cpputools : cppuhelper offuh libxml2
	$(MAKE_CALL)

crashrep :    l10n sal sysui tools shell
	$(MAKE_CALL)

cui : svx sax sot
	$(MAKE_CALL)

curl : stlport external
	$(MAKE_CALL)

dbaccess        :       l10n boost connectivity svx stoc qadevOOo xmlscript libxslt
	$(MAKE_CALL)

desktop :    l10n sfx2 stoc sysui  boost svx xmlhelp sal unoil officecfg offuh filter libxslt
	$(MAKE_CALL)

drawinglayer : sal vcl basegfx offuh cppuhelper cppu svtools avmedia canvas cppcanvas
	$(MAKE_CALL)

dtrans : unotools offapi offuh rdbmaker vos stoc libxslt
	$(MAKE_CALL)

editeng : l10n svtools xmloff linguistic
	$(MAKE_CALL)

embeddedobj : offuh sal cppu cppuhelper comphelper tools unotools vos libxslt
	$(MAKE_CALL)

embedserv : offuh sal cppu cppuhelper comphelper libxslt
	$(MAKE_CALL)

epm : solenv
	$(MAKE_CALL)

eventattacher   :       offapi cppuhelper vos libxslt
	$(MAKE_CALL)

expat : soltools
	$(MAKE_CALL)

extras : solenv
	$(MAKE_CALL)

extensions :  officecfg  l10n rdbmaker svx sane twain offuh stoc zlib curl libxslt
	$(MAKE_CALL)

external :       soltools
	$(MAKE_CALL)

fileaccess : unotools rdbmaker tools ucbhelper libxslt
	$(MAKE_CALL)

filter  :    l10n svtools unotools xmloff cppu tools cppuhelper sal svx javaunohelper jvmaccess canvas saxon libxslt basegfx
	$(MAKE_CALL)

forms :    l10n oovbaapi svx sfx2 qadevOOo libxslt
	$(MAKE_CALL)

formula    :   boost libxslt comphelper svx
	$(MAKE_CALL)

fpicker : libxslt l10n rdbmaker svtools
	$(MAKE_CALL)

framework : libxslt l10n svtools
	$(MAKE_CALL)

graphite : stlport solenv
	$(MAKE_CALL)

helpcontent2 :    l10n xmlhelp
	$(MAKE_CALL)

hsqldb : solenv
	$(MAKE_CALL)

hwpfilter : offuh cppuhelper comphelper zlib libxslt
	$(MAKE_CALL)

hyphen : soltools
	$(MAKE_CALL)

i18npool    :   bridges sax stoc comphelper icu i18nutil libxslt
	$(MAKE_CALL)

i18nutil    :   sal cppu offuh
	$(MAKE_CALL)

icc : solenv stlport external sal
	$(MAKE_CALL)

icu : solenv external
	$(MAKE_CALL)

idlc : ucpp registry
	$(MAKE_CALL)

idl : tools
	$(MAKE_CALL)

instsetoo_native : l10n postprocess packimages ratscan
	$(MAKE_CALL)

io : libxslt rdbmaker
	$(MAKE_CALL)

javainstaller2    :    l10n setup_native
	$(MAKE_CALL)

javaunohelper : libxslt bridges cppuhelper jurt ridljar unoil
	$(MAKE_CALL)

jpeg :  soltools
	$(MAKE_CALL)

jurt : ridljar sal
	$(MAKE_CALL)

jvmaccess : offuh ridljar cppu sal salhelper
	$(MAKE_CALL)

jvmfwk : cppu cppuhelper sal libxml2
	$(MAKE_CALL)

l10ntools : tools libxslt lucene python
	$(MAKE_CALL)

libpng : zlib soltools
	$(MAKE_CALL)

libtextcat      :       stlport soltools solenv
	$(MAKE_CALL)

libxml2 : soltools
	$(MAKE_CALL)

libxmlsec   : stlport soltools libxml2 moz 
	$(MAKE_CALL)

libxslt : soltools libxml2
	$(MAKE_CALL)

lingucomponent  :      linguistic libtextcat svl hyphen hunspell mythes libxslt
	$(MAKE_CALL)

linguistic  :   svl xmloff ucbhelper vos comphelper icu libxslt
	$(MAKE_CALL)

lucene  :  soltools expat
	$(MAKE_CALL)

MathMLDTD : solenv
	$(MAKE_CALL)

mdds : solenv boost
	$(MAKE_CALL)

migrationanalysis   :   soltools shell
	$(MAKE_CALL)

more_fonts : solenv
	$(MAKE_CALL)

moz : solenv  nss
	$(MAKE_CALL)

mysqlc : solenv offuh cppu soltools cppuhelper readlicense_oo
	$(MAKE_CALL)

mythes  :       soltools solenv
	$(MAKE_CALL)

nss : solenv stlport
	$(MAKE_CALL)

o3tl : sal boost
	$(MAKE_CALL)

odk     :      cppuhelper jurt javaunohelper udkapi ridljar offapi autodoc unodevtools external
	$(MAKE_CALL)

offapi : udkapi idlc solenv
	$(MAKE_CALL)

officecfg   :    l10n soltools solenv libxslt
	$(MAKE_CALL)

offuh : offapi oovbaapi codemaker
	$(MAKE_CALL)

oldep : svtools
	$(MAKE_CALL)

oovbaapi : offapi solenv
	$(MAKE_CALL)

oox : vos cppu cppuhelper comphelper sal offapi sax basegfx xmlscript tools vcl boost openssl libxslt
	$(MAKE_CALL)

openssl  :  soltools external expat
	$(MAKE_CALL)

package : cppu cppuhelper comphelper ucbhelper sal sax zlib libxslt

packimages : postprocess

padmin : vcl svtools

postprocess     :      svgio accessibility automation basctl bean chart2 configmgr crashrep coinmp cui dbaccess desktop dtrans embeddedobj embedserv epm eventattacher extensions extras fileaccess filter forms fpicker helpcontent2 hwpfilter io javainstaller2 lingucomponent MathMLDTD odk officecfg package padmin psprint_config remotebridges sc scaddins sccomp scp2 scripting sd setup_native slideshow starmath sw sysui testtools ucb UnoControls unoxml ure wizards xmerge xmlsecurity more_fonts pyuno readlicense_oo unodevtools reportbuilder reportdesign sdext swext smoketestdoc uui writerfilter oox mysqlc libxslt
	$(MAKE_CALL)

psprint_config : soltools afms
	$(MAKE_CALL)

python :  solenv openssl
	$(MAKE_CALL)

pyuno : stoc cpputools cppuhelper bridges tools python libxslt
	$(MAKE_CALL)

qadevOOo    : javaunohelper jurt ridljar unoil
	$(MAKE_CALL)

rdbmaker : cppuhelper
	$(MAKE_CALL)

readlicense_oo :    l10n solenv libxslt
	$(MAKE_CALL)

redland : stlport soltools libxml2 libxslt openssl
	$(MAKE_CALL)

registry : salhelper store
	$(MAKE_CALL)

remotebridges : libxslt cppu cppuhelper offuh rdbmaker sal
	$(MAKE_CALL)

reportbuilder    :   boost wizards readlicense_oo apache-commons officecfg
	$(MAKE_CALL)

reportdesign    :   l10n boost comphelper dbaccess formula libxslt
	$(MAKE_CALL)

rhino : soltools
	$(MAKE_CALL)

ridljar : codemaker solenv udkapi
	$(MAKE_CALL)

rsc : tools
	$(MAKE_CALL)

salhelper : sal
	$(MAKE_CALL)

sal : xml2cmp stlport external boost 
	$(MAKE_CALL)

sane : solenv
	$(MAKE_CALL)

sax : offapi cppuhelper expat comphelper libxslt
	$(MAKE_CALL)

saxon : solenv stax
	$(MAKE_CALL)

scaddins : libxslt l10n vcl
	$(MAKE_CALL)

sccomp :    l10n offuh comphelper coinmp tools rsc libxslt
	$(MAKE_CALL)

scp2    :    l10n i18npool setup_native l10ntools python icu redland libxslt apr apr-util serf
	$(MAKE_CALL)

sc      :       filter l10n vbahelper oovbaapi svx uui stoc boost formula mdds oox libxslt
	$(MAKE_CALL)

scripting : oovbaapi vbahelper bridges rdbmaker vcl xmlscript basic sfx2 rhino beanshell javaunohelper libxslt
	$(MAKE_CALL)

sdext : l10n offuh comphelper unotools readlicense_oo cppu tools cppuhelper sal javaunohelper officecfg libxslt xmlhelp
	$(MAKE_CALL)

sd      :    filter l10n animations svx sfx2 stoc canvas libxslt
	$(MAKE_CALL)

setup_native : l10n l10ntools soltools sal xml2cmp
	$(MAKE_CALL)

sfx2    :    l10n idl basic xmlscript framework readlicense_oo shell setup_native sax libxml2 libxslt
	$(MAKE_CALL)

shell :    l10n offuh rdbmaker tools sal expat libxslt
	$(MAKE_CALL)

slideshow : tools sal cppu vcl comphelper cppuhelper offuh cppcanvas basegfx svtools unotools toolkit animations avmedia libxslt
	$(MAKE_CALL)

smoketestdoc : soltools codemaker unoil javaunohelper
	$(MAKE_CALL)

smoketestoo_native     :       instsetoo_native test smoketestdoc
	$(MAKE_CALL)

solenv :
	$(MAKE_CALL)

soltools : solenv
	$(MAKE_CALL)

sot : tools ucbhelper unotools
	$(MAKE_CALL)

splitbuild     :      postprocess
	$(MAKE_CALL)

starmath : libxslt l10n svx
	$(MAKE_CALL)

stax : solenv
	$(MAKE_CALL)

stlport :    solenv boost
	$(MAKE_CALL)

stoc : rdbmaker cppuhelper cppu jvmaccess sal salhelper jvmfwk xmlreader libxslt
	$(MAKE_CALL)

store : sal
	$(MAKE_CALL)

svgio : sal basegfx drawinglayer cppuhelper cppu svtools
	$(MAKE_CALL)

svl : l10n rsc offuh ucbhelper unotools cppu cppuhelper comphelper sal sot  libxslt
	$(MAKE_CALL)

svtools : l10n svl offuh toolkit ucbhelper unotools jpeg cppu cppuhelper comphelper sal sot jvmfwk libxslt
	$(MAKE_CALL)

svx : sfx2 l10n oovbaapi connectivity xmloff linguistic jvmfwk avmedia drawinglayer editeng libxslt
	$(MAKE_CALL)

swext :   officecfg l10n javaunohelper ridljar unoil jurt apache-commons readlicense_oo xsltml xmlhelp
	$(MAKE_CALL)

sw      :    filter l10n connectivity vbahelper svx stoc writerfilter libxslt
	$(MAKE_CALL)

sysui : l10n offapi xml2cmp rdbmaker l10ntools setup_native
	$(MAKE_CALL)

testgraphical     :       instsetoo_native qadevOOo
	$(MAKE_CALL)

testtools : cpputools io remotebridges stoc javaunohelper pyuno cli_ure offapi ure libxslt
	$(MAKE_CALL)

tomcat : solenv
	$(MAKE_CALL)

toolkit : libxslt vcl qadevOOo
	$(MAKE_CALL)

tools : cppu external offuh vos zlib expat basegfx comphelper i18npool
	$(MAKE_CALL)

twain : solenv
	$(MAKE_CALL)

ucbhelper   :   offuh sal cppu cppuhelper salhelper
	$(MAKE_CALL)

ucb : cppuhelper curl openssl libxml2 libxslt offapi sal salhelper ucbhelper udkapi comphelper serf tools
	$(MAKE_CALL)

ucpp  :  soltools
	$(MAKE_CALL)

udkapi : idlc
	$(MAKE_CALL)

udm :  cosv
	$(MAKE_CALL)

unixODBC : solenv
	$(MAKE_CALL)

UnoControls : libxslt cppuhelper offuh tools
	$(MAKE_CALL)

unoil : codemaker offapi ridljar solenv cli_ure
	$(MAKE_CALL)

unodevtools : codemaker cppuhelper
	$(MAKE_CALL)

unotools : libxslt comphelper cppuhelper offuh tools ucbhelper
	$(MAKE_CALL)

unoxml : offuh cppuhelper libxml2 libxslt redland sax comphelper ucbhelper
	$(MAKE_CALL)

ure : libxslt binaryurp cli_ure io javaunohelper remotebridges solenv stoc
	$(MAKE_CALL)

uui :    l10n vcl svtools libxslt
	$(MAKE_CALL)

vbahelper : oovbaapi offuh basic sfx2 svx filter cppuhelper vcl comphelper svtools tools sal unotools libxslt
	$(MAKE_CALL)

vcl : l10n apple_remote boost rsc sot ucbhelper unotools icu graphite i18npool i18nutil unoil ridljar x11_extensions offuh basegfx basebmp tools l10ntools icc cpputools shell svl libxslt
	$(MAKE_CALL)

vigra : solenv
	$(MAKE_CALL)

vos : sal solenv
	$(MAKE_CALL)

wizards     :    l10n rsc javaunohelper unoil libxslt
	$(MAKE_CALL)

writerfilter : soltools sal boost cppu cppuhelper ucbhelper tools svx libxslt oox
	$(MAKE_CALL)

writerperfect : sot comphelper xmloff libxslt
	$(MAKE_CALL)

x11_extensions : solenv
	$(MAKE_CALL)

xmerge : solenv unoil javaunohelper libxslt
	$(MAKE_CALL)

xml2cmp : soltools stlport
	$(MAKE_CALL)

xmlhelp :       comphelper ucbhelper libxslt unoil lucene javaunohelper l10ntools
	$(MAKE_CALL)

xmloff : libxslt offapi svl vos
	$(MAKE_CALL)

xmlreader : boost cppu offuh sal stlport
	$(MAKE_CALL)

xmlscript : comphelper tools offapi libxslt
	$(MAKE_CALL)

xmlsecurity :    l10n xmloff unotools offapi unoil svx moz  libxmlsec nss libxslt

xsltml : solenv external
	$(MAKE_CALL)

zlib : soltools
	$(MAKE_CALL)
