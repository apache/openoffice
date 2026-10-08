"""The program/resource/ bundles: rsc_res target -> installed name prefix.

Shared by //main/staging (which installs each .res as <prefix>.res) and
//main/packimages (which builds images*.zip from each bundle's .ilst), so the
images an install can load and the bundles it ships cannot drift apart.
"""

RES_BUNDLES = {
    "//main/accessibility:accessibility_res":      "accen-US",
    "//main/dbaccess:adabasui_res":                "adabasuien-US",
    "//main/scaddins:analysis_res":                "analysisen-US",
    "//main/avmedia:avmedia_res":                  "avmediaen-US",
    "//main/basctl:basctl_res":                    "basctlen-US",
    "//main/basic:basic_res":                      "sben-US",
    "//main/chart2:chart2_res":                    "chartcontrolleren-US",
    # Three SEPARATE bundles, looked up by name at runtime, not one merged
    # .res: cnr is the shared driver strings (dbtools is compiled with
    # CONN_SHARED_RESOURCE_FILE=cnr), sdbcl is what JDriver.cxx's
    # ResourceBasedEventLogger("sdbcl", "org.openoffice.sdbc.jdbcBridge")
    # resolves, and sdberr holds the SQL error messages.  connectivity's
    # fourth reslib, hsqldb, is deliberately absent — it exists only to pull
    # its two images into images.zip and upstream never installs it
    # (//main/packimages takes its image list directly).
    "//main/connectivity:connectivity_res":        "cnren-US",
    "//main/connectivity:connectivity_log_res":    "sdbclen-US",
    "//main/connectivity:connectivity_err_res":    "sdberren-US",
    "//main/cui:cui_res":                          "cuien-US",
    "//main/scaddins:date_res":                    "dateen-US",
    "//main/dbaccess:dba_res":                     "dbaen-US",
    "//main/dbaccess:dbmm_res":                    "dbmmen-US",
    "//main/dbaccess:dbui_res":                    "dbuien-US",
    "//main/desktop:desktop_res":                  "dkten-US",
    "//main/desktop:deployment_res":               "deploymenten-US",
    "//main/desktop:deploymentgui_res":            "deploymentguien-US",
    "//main/editeng:editeng_res":                  "editengen-US",
    "//main/extensions:bib_res":                   "biben-US",
    "//main/extensions:dbp_res":                   "dbpen-US",
    "//main/extensions:abp_res":                   "abpen-US",
    "//main/extensions:preload_res":               "preloaden-US",
    "//main/extensions:pcr_res":                   "pcren-US",
    "//main/extensions:upd_res":                   "upden-US",
    "//main/extensions:updchkui_res":              "updchken-US",
    "//main/filter:flash_res":                     "flashen-US",
    "//main/forms:forms_res":                      "frmen-US",
    "//main/formula:formula_res":                  "foren-US",
    "//main/fpicker:fpicker_res":                  "fps_officeen-US",
    "//main/framework:framework_res":              "fween-US",
    "//main/svx:gal_res":                          "galen-US",
    "//main/filter:graphicfilter_eps_res":         "epsen-US",
    "//main/sdext:minimizer_res":                  "minimizeren-US",
    "//main/svx:ofa_res":                          "ofaen-US",
    "//main/filter:pdf_res":                       "pdffiltreen-US",
    "//main/sdext:presenter_res":                  "PresenterScreenen-US",
    "//main/reportdesign:rpt_res":                 "rpten-US",
    "//main/reportdesign:rptui_res":               "rptuien-US",
    "//main/sc:sc_res":                            "scen-US",
    "//main/sccomp:sccomp_res":                    "solveren-US",
    "//main/scripting:scripting_res":              "scriptframeen-US",
    "//main/sd:sd_res":                            "sden-US",
    "//main/dbaccess:sdbt_res":                    "sdbten-US",
    "//main/sfx2:sfx2_res":                        "sfxen-US",
    "//main/starmath:starmath_res":                "smen-US",
    "//main/svl:svl_res":                          "svlen-US",
    "//main/svtools:svtools_res":                  "svten-US",
    "//main/svx:svx_res":                          "svxen-US",
    "//main/sw:sw_res":                            "swen-US",
    "//main/filter:t602_res":                      "t602filter680en-US",
    "//main/svx:textconversiondlgs_res":           "textconversiondlgsen-US",
    "//main/uui:uui_res":                          "uuien-US",
    "//main/vcl:vcl_res":                          "vclen-US",
    # wizards — seven bundles, and the bundle NAME is the lookup key, not a
    # filename convention: BASIC asks by prefix (Tools/Misc.xba
    # InitResources("Euro Converter","eur")) and Java by module argument
    # (new Resource(xMSF,"Database","dbw")).  dbw serves ALL the Java
    # wizards, not just the database ones; wzi holds Images rather than
    # strings, reached as "private:resource/wzi/image/<id>".
    "//main/wizards:cal_res":                      "calen-US",
    "//main/wizards:dbw_res":                      "dbwen-US",
    "//main/wizards:eur_res":                      "euren-US",
    "//main/wizards:imp_res":                      "impen-US",
    "//main/wizards:tpl_res":                      "tplen-US",
    "//main/wizards:wwz_res":                      "wwzen-US",
    "//main/wizards:wzi_res":                      "wzien-US",
    "//main/xmlsecurity:xmlsecurity_res":          "xmlsecen-US",
    "//main/filter:xsltdialog_res":                "xsltdlgen-US",
}
