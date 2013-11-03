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
EXECUTE_TARGET := prepare clean rebuild genPO
MODULE_TARGET := $(filter-out $(EXECUTE_TARGET), $(MAKECMDGOALS))
ifeq ($(MODULE_TARGET),)
  MODULE_TARGET := all
endif
SUB_TARGET := $(filter $(EXECUTE_TARGET), $(MAKECMDGOALS))

MAKE_CALL= @cd $@; make $(SUB_TARGET)
NO_CALL= @echo $@ $(SUB_TARGET)

# default target if none is given
all: instsetoo_native

# target to extract messages, called as "make genPO all" or "make genPO"
genPO:


$(EXECUTE_TARGET) : $(MODULE_TARGET)



# ========== special targets (outside main) ==========
# This is temporary and will be removed
l10n : l10ntools
	@cd ../extras/$@; echo $(SUB_TARGET)

# build external libraries, just a ref. was build with bootstrap
ratscan hunspell coinmp apr serf apr-util:
	@cd ../ext_libraries/$@; echo $(SUB_TARGET)



# ========== module targets ==========
apache-commons : tomcat
	$(NO_CALL)

apple_remote : soltools
	$(NO_CALL)

beanshell : solenv
	$(NO_CALL)

boost : solenv
	$(NO_CALL)

cosv : soltools stlport
	$(NO_CALL)

epm : solenv
	$(NO_CALL)

expat : soltools
	$(NO_CALL)

external : soltools
	$(NO_CALL)

extras : solenv
	$(NO_CALL)

graphite : stlport
	$(NO_CALL)

hsqldb : solenv
	$(NO_CALL)

icu : external
	$(NO_CALL)

libtextcat : stlport soltools
	$(NO_CALL)

MathMLDTD : solenv
	$(NO_CALL)

mdds : boost
	$(NO_CALL)

more_fonts : solenv
	$(NO_CALL)

moz : nss
	$(NO_CALL)

mythes : soltools
	$(NO_CALL)

nss : stlport
	$(NO_CALL)

o3tl : sal
	$(NO_CALL)

openssl : external expat
	$(NO_CALL)

python : openssl
	$(NO_CALL)

sal : xml2cmp stlport external boost 
	$(NO_CALL)

sane : solenv
	$(NO_CALL)

saxon : stax
	$(NO_CALL)

solenv :
	$(NO_CALL)

soltools : solenv
	$(NO_CALL)

stax : solenv
	$(NO_CALL)

stlport : boost
	$(NO_CALL)

tomcat : solenv
	$(NO_CALL)

twain : solenv
	$(NO_CALL)

unixODBC : solenv
	$(NO_CALL)

vigra : solenv
	$(NO_CALL)

vos : sal
	$(NO_CALL)

x11_extensions : solenv
	$(NO_CALL)

xml2cmp : soltools stlport
	$(NO_CALL)

xsltml : external
	$(NO_CALL)




# ========== module targets ==========
accessibility :    l10n tools jurt offuh unoil vcl javaunohelper jvmaccess cppu sal toolkit svtools libxslt
	$(NO_CALL)

animations : cppuhelper comphelper offuh libxslt
	$(NO_CALL)

autodoc :  cosv udm sal
	$(NO_CALL)

automation : basic fpicker
	$(NO_CALL)

avmedia :    l10n tools sfx2 libxslt
	$(NO_CALL)

basctl : libxslt l10n svx
	$(NO_CALL)

basebmp : sal vigra basegfx boost
	$(NO_CALL)

basegfx : o3tl sal offuh cppuhelper cppu
	$(NO_CALL)

basic :    l10n offuh oovbaapi svtools xmlscript framework salhelper libxslt
	$(NO_CALL)

bean : javaunohelper jurt ridljar unoil offapi udkapi
	$(NO_CALL)

binaryurp : boost libxslt cppu cppuhelper offuh sal salhelper stlport
	$(NO_CALL)

bridges : cppuhelper jurt jvmaccess salhelper
	$(NO_CALL)

cairo : zlib soltools stlport
	$(NO_CALL)

canvas : javaunohelper comphelper cppuhelper offuh unoil tools svtools vcl basegfx cairo libxslt
	$(NO_CALL)

chart2 :    offapi offuh l10n comphelper cppu cppuhelper sal svtools svx tools vcl toolkit unotools sfx2 libxslt
	$(NO_CALL)

cli_ure :       cppu cppuhelper sal codemaker stoc udkapi bridges
	$(NO_CALL)

codemaker : udkapi
	$(NO_CALL)

comphelper : cppuhelper ucbhelper offuh vos salhelper libxslt
	$(NO_CALL)

configmgr : boost libxslt comphelper cppu cppuhelper offuh sal salhelper stlport xmlreader
	$(NO_CALL)

connectivity    :    shell  l10n comphelper moz  svl unixODBC unoil javaunohelper hsqldb qadevOOo officecfg nss libxslt
	$(NO_CALL)

cppcanvas : comphelper cppuhelper offuh tools vcl basegfx canvas
	$(NO_CALL)

cppuhelper : boost libxslt codemaker cppu offuh
	$(NO_CALL)

cppu : sal salhelper offuh boost
	$(NO_CALL)

cpputools : cppuhelper offuh libxml2
	$(NO_CALL)

crashrep :    l10n sal sysui tools shell
	$(NO_CALL)

cui : svx sax sot
	$(NO_CALL)

curl : stlport external
	$(NO_CALL)

dbaccess        :       l10n boost connectivity svx stoc qadevOOo xmlscript libxslt
	$(NO_CALL)

desktop :    l10n sfx2 stoc sysui  boost svx xmlhelp sal unoil officecfg offuh filter libxslt
	$(NO_CALL)

drawinglayer : sal vcl basegfx offuh cppuhelper cppu svtools avmedia canvas cppcanvas
	$(NO_CALL)

dtrans : unotools offapi offuh rdbmaker vos stoc libxslt
	$(NO_CALL)

editeng : l10n svtools xmloff linguistic
	$(NO_CALL)

embeddedobj : offuh sal cppu cppuhelper comphelper tools unotools vos libxslt
	$(NO_CALL)

embedserv : offuh sal cppu cppuhelper comphelper libxslt
	$(NO_CALL)

eventattacher   :       offapi cppuhelper vos libxslt
	$(NO_CALL)

extensions :  officecfg  l10n rdbmaker svx sane twain offuh stoc zlib curl libxslt
	$(NO_CALL)

fileaccess : unotools rdbmaker tools ucbhelper libxslt
	$(NO_CALL)

filter  :    l10n svtools unotools xmloff cppu tools cppuhelper sal svx javaunohelper jvmaccess canvas saxon libxslt basegfx
	$(NO_CALL)

forms :    l10n oovbaapi svx sfx2 qadevOOo libxslt
	$(NO_CALL)

formula    :   boost libxslt comphelper svx
	$(NO_CALL)

fpicker : libxslt l10n rdbmaker svtools
	$(NO_CALL)

framework : libxslt l10n svtools
	$(NO_CALL)

helpcontent2 :    l10n xmlhelp
	$(NO_CALL)

hwpfilter : offuh cppuhelper comphelper zlib libxslt
	$(NO_CALL)

hyphen : soltools
	$(NO_CALL)

i18npool    :   bridges sax stoc comphelper icu i18nutil libxslt
	$(NO_CALL)

i18nutil    :   sal cppu offuh
	$(NO_CALL)

icc : stlport external sal
	$(NO_CALL)

idlc : ucpp registry
	$(NO_CALL)

idl : tools
	$(NO_CALL)

instsetoo_native : l10n postprocess packimages ratscan
	$(NO_CALL)

io : libxslt rdbmaker
	$(NO_CALL)

javainstaller2    :    l10n setup_native
	$(NO_CALL)

javaunohelper : libxslt bridges cppuhelper jurt ridljar unoil
	$(NO_CALL)

jpeg :  soltools
	$(NO_CALL)

jurt : ridljar sal
	$(NO_CALL)

jvmaccess : offuh ridljar cppu sal salhelper
	$(NO_CALL)

jvmfwk : cppu cppuhelper sal libxml2
	$(NO_CALL)

l10ntools : tools libxslt lucene python
	$(NO_CALL)

libpng : zlib soltools
	$(NO_CALL)

libxml2 : soltools
	$(NO_CALL)

libxmlsec   : stlport soltools libxml2 moz 
	$(NO_CALL)

libxslt : soltools libxml2
	$(NO_CALL)

lingucomponent  :      linguistic libtextcat svl hyphen hunspell mythes libxslt
	$(NO_CALL)

linguistic  :   svl xmloff ucbhelper vos comphelper icu libxslt
	$(NO_CALL)

lucene  :  soltools expat
	$(NO_CALL)

migrationanalysis   :   soltools shell
	$(NO_CALL)

mysqlc : solenv offuh cppu soltools cppuhelper readlicense_oo
	$(NO_CALL)

odk     :      cppuhelper jurt javaunohelper udkapi ridljar offapi autodoc unodevtools external
	$(NO_CALL)

offapi : udkapi idlc solenv
	$(NO_CALL)

officecfg   :    l10n soltools solenv libxslt
	$(NO_CALL)

offuh : offapi oovbaapi codemaker
	$(NO_CALL)

oldep : svtools
	$(NO_CALL)

oovbaapi : offapi solenv
	$(NO_CALL)

oox : vos cppu cppuhelper comphelper sal offapi sax basegfx xmlscript tools vcl boost openssl libxslt
	$(NO_CALL)

package : cppu cppuhelper comphelper ucbhelper sal sax zlib libxslt

packimages : postprocess

padmin : vcl svtools

postprocess     :      svgio accessibility automation basctl bean chart2 configmgr crashrep coinmp cui dbaccess desktop dtrans embeddedobj embedserv epm eventattacher extensions extras fileaccess filter forms fpicker helpcontent2 hwpfilter io javainstaller2 lingucomponent MathMLDTD odk officecfg package padmin psprint_config remotebridges sc scaddins sccomp scp2 scripting sd setup_native slideshow starmath sw sysui testtools ucb UnoControls unoxml ure wizards xmerge xmlsecurity more_fonts pyuno readlicense_oo unodevtools reportbuilder reportdesign sdext swext smoketestdoc uui writerfilter oox mysqlc libxslt
	$(NO_CALL)

psprint_config : soltools afms
	$(NO_CALL)

pyuno : stoc cpputools cppuhelper bridges tools python libxslt
	$(NO_CALL)

qadevOOo    : javaunohelper jurt ridljar unoil
	$(NO_CALL)

rdbmaker : cppuhelper
	$(NO_CALL)

readlicense_oo :    l10n solenv libxslt
	$(NO_CALL)

redland : stlport soltools libxml2 libxslt openssl
	$(NO_CALL)

registry : salhelper store
	$(NO_CALL)

remotebridges : libxslt cppu cppuhelper offuh rdbmaker sal
	$(NO_CALL)

reportbuilder    :   boost wizards readlicense_oo apache-commons officecfg
	$(NO_CALL)

reportdesign    :   l10n boost comphelper dbaccess formula libxslt
	$(NO_CALL)

rhino : soltools
	$(NO_CALL)

ridljar : codemaker solenv udkapi
	$(NO_CALL)

rsc : tools
	$(NO_CALL)

salhelper : sal
	$(NO_CALL)

sax : offapi cppuhelper expat comphelper libxslt
	$(NO_CALL)

scaddins : libxslt l10n vcl
	$(NO_CALL)

sccomp :    l10n offuh comphelper coinmp tools rsc libxslt
	$(NO_CALL)

scp2    :    l10n i18npool setup_native l10ntools python icu redland libxslt apr apr-util serf
	$(NO_CALL)

sc      :       filter l10n vbahelper oovbaapi svx uui stoc boost formula mdds oox libxslt
	$(NO_CALL)

scripting : oovbaapi vbahelper bridges rdbmaker vcl xmlscript basic sfx2 rhino beanshell javaunohelper libxslt
	$(NO_CALL)

sdext : l10n offuh comphelper unotools readlicense_oo cppu tools cppuhelper sal javaunohelper officecfg libxslt xmlhelp
	$(NO_CALL)

sd      :    filter l10n animations svx sfx2 stoc canvas libxslt
	$(NO_CALL)

setup_native : l10n l10ntools soltools sal xml2cmp
	$(NO_CALL)

sfx2    :    l10n idl basic xmlscript framework readlicense_oo shell setup_native sax libxml2 libxslt
	$(NO_CALL)

shell :    l10n offuh rdbmaker tools sal expat libxslt
	$(NO_CALL)

slideshow : tools sal cppu vcl comphelper cppuhelper offuh cppcanvas basegfx svtools unotools toolkit animations avmedia libxslt
	$(NO_CALL)

smoketestdoc : soltools codemaker unoil javaunohelper
	$(NO_CALL)

smoketestoo_native     :       instsetoo_native test smoketestdoc
	$(NO_CALL)

sot : tools ucbhelper unotools
	$(NO_CALL)

splitbuild     :      postprocess
	$(NO_CALL)

starmath : libxslt l10n svx
	$(NO_CALL)

stoc : rdbmaker cppuhelper cppu jvmaccess sal salhelper jvmfwk xmlreader libxslt
	$(NO_CALL)

store : sal
	$(NO_CALL)

svgio : sal basegfx drawinglayer cppuhelper cppu svtools
	$(NO_CALL)

svl : l10n rsc offuh ucbhelper unotools cppu cppuhelper comphelper sal sot  libxslt
	$(NO_CALL)

svtools : l10n svl offuh toolkit ucbhelper unotools jpeg cppu cppuhelper comphelper sal sot jvmfwk libxslt
	$(NO_CALL)

svx : sfx2 l10n oovbaapi connectivity xmloff linguistic jvmfwk avmedia drawinglayer editeng libxslt
	$(NO_CALL)

swext :   officecfg l10n javaunohelper ridljar unoil jurt apache-commons readlicense_oo xsltml xmlhelp
	$(NO_CALL)

sw      :    filter l10n connectivity vbahelper svx stoc writerfilter libxslt
	$(NO_CALL)

sysui : l10n offapi xml2cmp rdbmaker l10ntools setup_native
	$(NO_CALL)

testgraphical     :       instsetoo_native qadevOOo
	$(NO_CALL)

testtools : cpputools io remotebridges stoc javaunohelper pyuno cli_ure offapi ure libxslt
	$(NO_CALL)

toolkit : libxslt vcl qadevOOo
	$(NO_CALL)

tools : cppu external offuh vos zlib expat basegfx comphelper i18npool
	$(NO_CALL)

ucbhelper   :   offuh sal cppu cppuhelper salhelper
	$(NO_CALL)

ucb : cppuhelper curl openssl libxml2 libxslt offapi sal salhelper ucbhelper udkapi comphelper serf tools
	$(NO_CALL)

ucpp  :  soltools
	$(NO_CALL)

udkapi : idlc
	$(NO_CALL)

udm :  cosv
	$(NO_CALL)

UnoControls : libxslt cppuhelper offuh tools
	$(NO_CALL)

unodevtools : codemaker cppuhelper
	$(NO_CALL)

unoil : codemaker offapi ridljar solenv cli_ure
	$(NO_CALL)

unotools : libxslt comphelper cppuhelper offuh tools ucbhelper
	$(NO_CALL)

unoxml : offuh cppuhelper libxml2 libxslt redland sax comphelper ucbhelper
	$(NO_CALL)

ure : libxslt binaryurp cli_ure io javaunohelper remotebridges solenv stoc
	$(NO_CALL)

uui :    l10n vcl svtools libxslt
	$(NO_CALL)

vbahelper : oovbaapi offuh basic sfx2 svx filter cppuhelper vcl comphelper svtools tools sal unotools libxslt
	$(NO_CALL)

vcl : l10n apple_remote boost rsc sot ucbhelper unotools icu graphite i18npool i18nutil unoil ridljar x11_extensions offuh basegfx basebmp tools l10ntools icc cpputools shell svl libxslt
	$(NO_CALL)

wizards     :    l10n rsc javaunohelper unoil libxslt
	$(NO_CALL)

writerfilter : soltools sal boost cppu cppuhelper ucbhelper tools svx libxslt oox
	$(NO_CALL)

writerperfect : sot comphelper xmloff libxslt
	$(NO_CALL)

xmlhelp :       comphelper ucbhelper libxslt unoil lucene javaunohelper l10ntools
	$(NO_CALL)

xmloff : libxslt offapi svl vos
	$(NO_CALL)

xmerge : solenv unoil javaunohelper libxslt
	$(NO_CALL)

xmlreader : boost cppu offuh sal stlport
	$(NO_CALL)

xmlscript : comphelper tools offapi libxslt
	$(NO_CALL)

xmlsecurity :    l10n xmloff unotools offapi unoil svx moz  libxmlsec nss libxslt

zlib : soltools
	$(NO_CALL)
