# gbuild's main/solenv/gbuild/AllLangResTarget.mk contains these 5 classes:
# 1. SrsPartMergeTarget (used in 2 modules)
# 2. SrsPartTarget (used in 2 modules)
# 3. SrsTarget (used in many modules)
# 4. ResTarget (unused in modules)
# 5. AllLangResTarget (used in many modules)

#
#
#                          AllLangResTarget(name)
#                      (meta-target; delivers an empty timestamp file)
#                            ^                         ^
#                           /                           \                                           For each lang, makes a ResTarget.
#                          /                             \                                          The ResTarget and its image list
#                         /                               \                                         are the ONLY deliverables.
#      ResTarget(nameen-US,name,en-US)                      ResTarget(nameen-GB,name,en-GB)
#      $(WORKDIR)/ResTarget/$(resName).res                  $(WORKDIR)/ResTarget/$(resName).res
#      $(WORKDIR)/ResTarget/nameen-US.res                   $(WORKDIR)/ResTarget/nameen-GB.res
#        ^                                                      ^                                   For each file added though
#        | rsc                                                  |                                   gb_AllLangResTarget_add_srs(), makes a
#        |                                                      |                                   SrsTarget per ResTarget (realistically,
#        |                                                      |                                   exactly 1 SrsTarget per ResTarget):
#      SrsTarget                                             SrsTarget
#      $(WORKDIR)/SrsTarget/$(srsName).srs
#      $(WORKDIR)/SrsTarget/uui/res.srs
#        ^
#        |                                                                                          For each file added through
#        | concatenate                                                                              gb_SrsTarget_add_files(), makes an
#        +----------+                                                                               SrsPartTarget:
#        |           \
#        |            \
#      SrcPartTarget   SrcPartTarget
#                      $(WORKDIR)/SrsPartTarget/$(srsPartName)
#                      $(WORKDIR)/SrsPartTarget/uui/source/ids.src
#                        ^                   ^
#                        | rsc               | rsc
#                        |                   |
# (when not translating) |                   | (when translating)                                   We only make a SrsPartMergeTarget
#                        |                   |                                                      when translating, and it's exactly
#                        |                SrcPartMergeTarget                                        one per SrcPartTarget.
#                        |                $(WORKDIR)/SrsPartMergeTarget/$(1)
#                        |                $(WORKDIR)/SrsPartMergeTarget/uui/source/ids.src
#                        |                 ^
#                        |                /
#                        |               / transex3
#                        |              / (when translating)
#                      $(srsPartName)  /
#                      uuid/source/ids.src
#
#
#



# ====================
# 5. AllLangResTarget
# ====================
# This is just there to create a number of ResTarget objects,
# one per language, and forward various calls to each one
# of those (eg. add_srs() is called on every lang's ResTarget).
# 
# (extensions/pre_SharedLangList.mk)
if WITH_LANG == 'All':
    gb_WITH_LANG = $(completelangiso) from $(SOLARENV)/inc/langlist.mk
else:
    gb_WITH_LANG = $(WITH_LANG)
gb_AllLangResTarget_LANGS = unique union of (en-US, gb_WITH_LANG)

# # UNUSED
# #def gb_AllLangResTarget_set_langs(langs):
# #    gb_AllLangResTarget_LANGS = langs
# # 
def gb_AllLangResTarget_get_target(name):
    return $(WORKDIR)/AllLangRes/$(name)

def gb_AllLangResTarget_AllLangResTarget(name):
    for lang in gb_AllLangResTarget_LANGS:
        gb_ResTarget_ResTarget($name$lang, name, lang)
    register the target gb_AllLangResTarget_get_target(name)

def gb_AllLangResTarget_add_srs(name, srs):
    for s in srs:
        gb_AllLangResTarget_add_one_srs(name, s)

def gb_AllLangResTarget_add_one_srs(name, srs):
    for lang in gb_AllLangResTarget_LANGS:
        gb_ResTarget_add_one_srs($(name)$(lang), srs)

def gb_AllLangResTarget_set_reslocation(name, location):
    for lang in gb_AllLangResTarget_LANGS:
        gb_ResTarget_set_reslocation($(name)$(lang), location)

def gb_AllLangResTarget_set_imagelocations(name, location):
    for lang in gb_AllLangResTarget_LANGS:
        gb_ResTarget_set_imagelocations($(name)$(lang), location)

# ============================
#  4. ResTarget
# ============================
# Generates $(WORKDIR)/ResTarget/$(resName).res
# by compiling a number of .srs

def gb_ResTarget_get_target(resName):
    return $(WORKDIR)/ResTarget/$(resName).res

def gb_ResTarget_ResTarget(resName, allLangResName, lang):
    on gb_ResTarget_get_target(resName), set:
        RESLOCATION = allLangResName
        LIBRARY = allLangResName
        LANGUAGE = lang
    add dependencies and deliverables

def gb_ResTarget_add_file(resName, filename):
    gb_ResTarget_get_target(resName) depends on filename

def gb_ResTarget_add_one_srs(resName, srsName):
    gb_ResTarget_add_file(resName, gb_SrsTarget_get_target(srsName))

def gb_ResTarget_set_reslocation(resName, location):
    on gb_ResTarget_get_target(resName), set RESLOCATION=location

def gb_ResTarget_set_imagelocation(resName, location):
    on gb_ResTarget_get_target(resName), set IMAGELOCATIONS=location

# A SUPER LONG COMPLEX RULE TO GENERATE ANY gb_ResTarget_get_target()
# FROM ITS DEPENDENCIES USING rsc

# =============================
# 3. SrsTarget
# =============================
# Generates $(WORKDIR)/SrsTarget/$(srsName).srs
# by catting together its compiled .src dependencies
def gb_SrsTarget_get_target(srsName):
    return $(WORKDIR)/SrsTarget/$(srsName).srs

gb_SrsTarget_DEFAULTDEFS = gb_GLOBALDEFS

def gb_SrsTarget_SrsTarget(srsName):
    on gb_SrsTarget_get_target(srsName), set:
        DEFS = $(gb_SrsTarget_DEFAULTDEFS)
        INCLUDE = $(SOLARINC)
        PARTS =
    some dependencies
 
def gb_SrsTarget_set_include(srsName, includes):
    on gb_SrsTarget_get_target(srsName) set INCLUDE = includes

# file is something like uui/source/ids.src
def gb_SrsTarget_add_file(srsName, file):
    gb_SrsPartTarget_SrsPartTarget(file)
    on gb_SrsPartTarget_get_target(file), add file to PARTS, make us depend on it
    dependencies

# # UNUSED
# # def gb_SrsTarget_set_defs

# RULE to generate gb_SrsTarget_get_target():
#     cat together dependencies (file contents) into target file
# 
# ====================================
# 2. SrsPartTarget
# ====================================
# Generates $(WORKDIR)/SrsPartTarget/$(srsPartName)[.src]
# by compiling $(srsPartName)[.src]

def gb_SrsPartTarget_get_target(srsPartName):
    return $(WORKDIR)/SrsPartTarget/$(srsPartName)

# srsPartName is something like uui/source/ids.src
def gb_SrsPartTarget_SrsPartTarget(srsPartName):
    if $(gb_WITH_LANG) is empty:
        on gb_SrsPartTarget_get_target(srsPartName), set MERGEDFILE=
    else:
        on gb_SrsPartTarget_get_target(srsPartName), set:
            MERGEDFILE = gb_SrsPartMergeTarget_get_target(srsPartName)
            depend on gb_SrsPartMergeTarget_get_target(srsPartName)
            on gb_SrsPartMergeTarget_get_target(srsPartName):
                set SDF = $(realpath $(gb_SrsPartMergeTarget_SDFLOCATION)/$(dir $(1))localize.sdf)
                make gb_SrsPartMergeTarget_get_target(srsPartName) depend on that location

# RULE to generate gb_SrsPartTarget_get_target() from dependencies or MERGEFILE
# $$(call gb_SrsPartTarget_get_target,%) : $(1)/% $$(gb_Helper_MISCDUMMY) | $$(gb_SrsPartTarget_RSCTARGET)
# So:
#   $(WORKDIR)/SrsPartTarget/$(srsPartName) solver/450/unxfbsdx/workdir/SrsPartTarget/uui/source/ids.src
#   depends on
#                            $(srsPartName)                                           uui/source/ids.src
# and an rsc command generates the target from the source.  
# If MERGEFILE is set, it is used, otherwise the dependency is used.

# =====================
# 1. SrsPartMergeTarget
# =====================
# Only used when MERGEFILE was set in gb_SrsPartTarget_SrsPartTarget
# 
#

gb_SrsPartMergeTarget_get_target = $(WORKDIR)/SrsPartMergeTarget/$(1)
gb_SrsPartMergeTarget_SDFLOCATION := $(LOCDIR)/l10n/$(INPATH)/misc/sdf

# runs rules on each repo

def gb_SrsPartMergeTarget__rules():
    if SDF is set:
        gb_SrsPartMergeTarget__command()
    else:
        copy sources to target

def gb_SrsPartMergeTarget__command():
    calls gb_SrsPartMergeTarget_TRANSEXCOMMAND on various args


# NOTE: SCons' CPPSUFFIXES could scan .src files for dependencies

