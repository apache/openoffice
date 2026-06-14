# Migrated Packages.
These packages have been migrated.
```
sal           ✅  — main/sal/readme.md
salhelper     ✅
store         ✅
registry      ✅
ucpp          ✅
idlc          ✅  (+ regmerge, cppumaker) — main/idlc/readme.md
udkapi        ✅  (.idl → .urd → .rdb → .hdl/.hpp via idl_pipeline.bzl)
cppu          ✅  (cppu3, purpenvhelper3MSC, affine/unsafe/log bridge DLLs)
cppuhelper    ✅  (cppuhelper3MSC.dll)
bridges       ✅  (msci_uno.dll) — main/bridges/readme.md
rdbmaker      ✅  (rdbmaker.exe) — main/rdbmaker/readme.md
xmlreader     ✅  (xmlreader.dll) — main/xmlreader/readme.md
stoc          ✅  (bootstrap.uno.dll, stocservices.uno.dll) — main/stoc/readme.md
jvmfwk        ✅  (jvmfwk.dll) — main/jvmfwk/readme.md
io            ✅  (streams.dll, acceptor.dll, connector.dll, textinstream.dll, textoutstream.dll) — main/io/readme.md
o3tl          ✅  (header-only)
vos           ✅  (vos3.dll)
libxslt       ✅  (@libxslt//:libxslt + :libexslt)
offapi        ✅  (offapi_idl.rdb + offapi_idl_inc/ — 3718 IDL files)
binaryurp     ✅  (binaryurp.dll)
remotebridges ✅  (uuresolver.dll)
cpputools     ✅  (regcomp.exe, regsingleton.exe, sp2bv.exe, uno.exe)
i18nutil      ✅  (i18nutil.dll)
ucbhelper     ✅  (ucbhelper.dll)
comphelper    ✅  (comphelp.dll) — main/comphelper/readme.md
configmgr     ✅  (configmgr.uno.dll) — main/configmgr/readme.md
expat         ✅  — ext_libraries/modules/expat/readme.md
sax           ✅  (sax.dll, expwrap.dll, fastsax.dll) — main/sax/readme.md
zlib          ✅  — ext_libraries/modules/zlib/readme.md
package       ✅  (package2.dll, xstor.dll) — main/package/readme.md
i18npool      ✅  (i18nisolang1.dll, i18npaper.dll, i18npool.dll, i18nsearch.dll) — main/i18npool/readme.md
python        ✅  (python27.dll) — ext_libraries/modules/python/readme.md
pyuno         ✅  (pyuno.dll, pythonloader.dll) — main/pyuno/readme.md
            │
            ▼
── Layer 1 (all deps done) ──────────────────────────────────────────────
basegfx       ✅  (o3tl, sal, offapi, cppu, cppuhelper) — main/basegfx/readme.md
codemaker     ✅  (registry) — main/codemaker/BUILD.bazel
oovbaapi      ✅  (offapi) — main/oovbaapi/BUILD.bazel
external      ✅  (pre-built DLL/lib filegroups) — main/external/BUILD.bazel
── Layer 2 ──────────────────────────────────────────────────────────────
tools         ✅  (tl.dll, ooopathutils.lib) — main/tools/BUILD.bazel
basebmp       ✅  (basegfx, sal + boost, vigra ext) — main/basebmp/BUILD.bazel
ridljar       ✅  (codemaker, udkapi — Java) — main/ridljar/readme.md
── Layer 3 ──────────────────────────────────────────────────────────────
rsc           ✅  (rscpp, rsc, rsc2) — main/rsc/readme.md
unotools      ✅  (utl.dll) — main/unotools/readme.md
xmlscript     ✅  (xcr.dll) — main/xmlscript/BUILD.bazel
shell         ✅  (syssh.uno, localebe1.uno, wininetbe1.uno, shlxthdl, ooofilt, propertyhdl, lngconvex, regsvrex) — main/shell/BUILD.bazel
l10ntools     ✅  (transex3, helpex, ulfex, gsiconv, gsicheck, cfgex, xrmex, localize_sl, HelpLinker, helplinker.dll) — main/l10ntools/BUILD.bazel
icc           ✅  (create_sRGB_profile.exe → sRGB-IEC61966-2.1.hxx, srgb_icc) — main\icc\readme.md
jurt          ✅  (ridljar, sal — Java; jpipe.dll + jpipx.dll native pipe JNI) — main/jurt/readme.md
jvmaccess     ✅  (jvmaccess.dll) — main/jvmaccess/readme.md
── Layer 4 ──────────────────────────────────────────────────────────────
sot           ✅  (tools, unotools, ucbhelper) — main/sot/readme.md
unoil         ✅  (codemaker, ridljar, offapi — Java) — main/unoil/readme.md
idl           ✅  (svidl.exe) — main/idl/readme.md
── Layer 5 ──────────────────────────────────────────────────────────────
svl           ✅  (svl.dll, fsstorage.dll, passwordcontainer.dll) — main/svl/readme.md
javaunohelper ✅  (juh.jar, juh.dll, juhx.dll) — main/javaunohelper/readme.md
── Layer 6 ──────────────────────────────────────────────────────────────
vcl           ✅  (rsc, sot, unotools, svl, basegfx, basebmp, tools, shell, l10ntools,
                   icc + boost, icu, graphite ext) — main/vcl/readme.md
xmloff        ✅  (xo.dll, xof.dll) — main/xmloff/readme.md
── Layer 7 ──────────────────────────────────────────────────────────────
linguistic    ✅  (svl, xmloff, ucbhelper, vos, comphelper + icu ext) — main/linguistic/readme.md
toolkit       ✅  (ootk.dll) — main/toolkit/readme.md
svtools       ✅  (svt.dll, hatchwindowfactory.dll) — main/svtools/readme.md
officecfg     ✅  (xsltproc pipeline: schema_trim + alllang, per-locale zips) — main/officecfg/BUILD.bazel
── Layer 8 ──────────────────────────────────────────────────────────────
framework     ✅  (fwi.dll, fwe.dll, fwk.dll, fwl.dll, fwm.dll) — main/framework/readme.md
editeng       ✅  (editeng.dll) — main/editeng/readme.md
canvas        ✅  (canvastools, canvasfactory, nullcanvas, simplecanvas, vclcanvas, directx9canvas, gdipluscanvas) — main/canvas/readme.md
basic         ✅  (oovbaapi, svtools, xmlscript, framework, salhelper)
connectivity  ✅  (dbtools.dll, sdbc2.dll, dbpool2.dll) — main/connectivity/readme.md
── Layer 9 ──────────────────────────────────────────────────────────────
sfx2          ✅  (basic, xmlscript, framework, shell, sax + libxml2 ext) — main/sfx2/readme.md
cppcanvas     ✅  (cppcanvas.dll) — main/cppcanvas/readme.md
── Layer 10 ─────────────────────────────────────────────────────────────
avmedia       ✅  (avmedia.dll, avmediawin.dll) — main/avmedia/readme.md
── Layer 11 ─────────────────────────────────────────────────────────────
drawinglayer  ✅  (vcl, svtools, basegfx, avmedia, canvas, cppcanvas)
── Layer 12 ─────────────────────────────────────────────────────────────
svx           ✅  (svxcore.dll, svx.dll, textconversiondlgs.dll) — main/svx/readme.md
msfilter      ✅  (msfilter.dll — prereq for oox export) — main/filter/BUILD.bazel
oox           ✅  (oox.dll) — main/oox/readme.md
── Layer 13 ─────────────────────────────────────────────────────────────
writerfilter  ✅  (writerfilter.dll, writerfilter_debug.dll) — main/writerfilter/readme.md
filter        ✅  (xmlfa, xfld, svgfilter, pdffilter, flash, placeware, t602filter,
                   filterconfig1, xsltfilter, xsltdlg, filtertracer + 22 graphic
                   filter DLLs) — main/filter/readme.md
vbahelper     ✅  (vbahelper.dll, msforms.dll) — main/vbahelper/readme.md
── Layer 14 — pre-app infrastructure ─────────────────────────────────────
ucb           ✅  (ucb1, cached1, srtrs1, ucpexpand1, ucpext, ucpfile1, ucphier1,
                   ucppkg1, ucptdoc1, ucpftp1, ucpdav1) — main/ucb/readme.md
unoxml        ✅  (unoxml.dll — DOM/XPath/Events; unordf.dll — RDF/metadata librdf_Repository) — main/unoxml/readme.md
              RDF stack (native-cc, static): redland (librdf) + raptor2 + rasqal — rdf-readme.md
              build-side complete 2026-06-14; save *runtime* path still open (see rdf-readme.md)
eventattacher ✅  (evtatt.dll) — main/eventattacher/readme.md
UnoControls   ✅  (ctl.dll) — main/UnoControls/readme.md
dtrans        ✅  (dtrans.dll, mcnttype.dll, dnd.dll, ftransl.dll, sysdtrans.dll) — main/dtrans/readme.md
svgio         ✅  (svgio.dll) — main/svgio/readme.md
lingucomponent ✅ (spell.dll, hyphen.dll, lnth.dll, guesslang.dll) — main/lingucomponent/readme.md
fpicker       ✅  (fps.dll, fop.dll, fps_office.dll, fpicker.dll) — main/fpicker/readme.md
mysqlc        ✅  (mysqlc.uno.dll — MySQL/MariaDB SDBC driver via mysqlcppconn+mariadb-connector-c) — main/mysqlc/readme.md
── Layer 15 ─────────────────────────────────────────────────────────────
formula       ✅  (for.dll, forui.dll) — main/formula/readme.md
animations    ✅  (animcore.dll) — main/animations/readme.md
chart2        ✅  (charttools, chartmodel, chartview, chartcontroller) — main/chart2/readme.md
embeddedobj   ✅  (embobj.dll, emboleobj.dll) — main/embeddedobj/readme.md
xmlsecurity   ✅  (xsec_fw.dll, xsec_xmlsec.dll, xmlsecurity.dll) — main/xmlsecurity/readme.md
uui           ✅  (uui.dll) — main/uui/readme.md
fileaccess    ✅  (fileacc.dll) — main/fileaccess/readme.md
accessibility ✅  (acc.dll — VCL + svt a11y factories) — main/accessibility/readme.md
── Layer 16 ─────────────────────────────────────────────────────────────
forms         ✅  (frm.dll — form controls) — main/forms/readme.md
scripting     ✅  (scriptframe.dll, basprov.uno.dll, dlgprov.uno.dll, vbaevents.uno.dll, stringresource.uno.dll, protocolhandler.dll) — main/scripting/readme.md
embedserv     ✅  (emser.dll, inprocserv.dll — ATL required; skipped unless --//build:atl_available=True) — main/embedserv/readme.md
winaccessibility ✅  (UAccCOM.dll + winaccessibility.dll — ATL + MIDL required; skipped unless --//build:atl_available=True) — main/winaccessibility/readme.md
── Layer 17 ─────────────────────────────────────────────────────────────
cui           ✅  (cui.dll) — main/cui/readme.md
── Layer 18 — sw ─────────────────────────────────────────────────────────
sw            ✅  (swd.dll + sw.dll + swui.dll + msword.dll + vbaswobj.dll) — main/sw/README.md
── Layer 19 ─────────────────────────────────────────────────────────────
basctl        ✅  (basic, sfx2, svx — Basic IDE) — main/basctl/readme.md
sd            ✅  (sdd.dll, sd.dll, sdui.dll, sdfilt.dll) — main/sd/readme.md
── Layer 20 ─────────────────────────────────────────────────────────────
slideshow     ✅  (slideshow.dll — Impress slideshow engine) — main/slideshow/readme.md
sdext         ✅  (PresentationMinimizer.uno.dll, PresenterScreen.uno.dll, pdfimport.uno.dll) — main/sdext/readme.md
sc            ✅  (sc.dll, scd.dll, scfilt.dll, scui.dll, vbaobj.dll) — main/sc/readme.md
── Layer 21 ─────────────────────────────────────────────────────────────
scaddins      ✅  (analysis.dll, date.dll) — main/scaddins/readme.md
sccomp        ✅  (solver.dll — CoinMP/COIN-OR linear solver) — main/sccomp/readme.md
dbaccess      ✅  (dba.dll, dbu.dll, dbaxml.dll, sdbt.dll, dbmm.dll, adabasui.dll) — main/dbaccess/readme.md
── Layer 22 ─────────────────────────────────────────────────────────────
reportdesign  ✅  (rpt.dll, rptui.dll, rptxml.dll) — main/reportdesign/readme.md
starmath      ✅  (sm.dll, smd.dll — Math formula editor) — main/starmath/readme.md
── Layer 23 — desktop ────────────────────────────────────────────────────
desktop       ✅  (soffice.exe + all launcher EXEs + 9 DLLs) — main/desktop/readme.md
── Layer 24 — staging ────────────────────────────────────────────
staging       ✅  (bazel build //main/staging:install → runnable install layout) — main/staging/BUILD.bazel
── Already done, not yet listed ─────────────────────────────────────────
ooxml         ✅  (done with oox migration; openssl_shim removed, now uses @openssl//:ssl)
stlport       ✅  (boost hash_map shim, referenced as dep)
── Cross-cutting infrastructure ─────────────────────────────────────
rsc pipeline   ✅  (rscpp + rsc2 called directly as Bazel actions; rsc.exe launcher
                   not used; -lip= + -subimages= required for image lookup; normal
                   mode writes .res only; 31 modules wired up) — build/rules/rsc_pipeline.bzl
── Deployment local ─────────────────────────────────────
postprocess   ✅  (services.rdb, ooo_services_rdb, uiconfig.zip, 20 .xcd packages via fcfg_merge+pack_registry) — main/postprocess/readme.md