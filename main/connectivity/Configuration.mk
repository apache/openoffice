#**************************************************************
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
#**************************************************************

#########
# Inputs:
#########

PACKAGE := org.openoffice.Office.DataAccess
COMPONENT_CONFIG_DATA := \
	$(SRCDIR)/source/drivers/ado/ado.xcu \
	$(SRCDIR)/source/drivers/adabas/adabas.xcu \
	$(SRCDIR)/source/drivers/mysql/mysql.xcu \
	$(SRCDIR)/source/drivers/hsqldb/hsqldb.xcu \
	$(SRCDIR)/source/drivers/macab/macab.xcu \
	$(SRCDIR)/source/drivers/evoab2/evoab2.xcu \
	$(SRCDIR)/source/drivers/jdbc/jdbc.xcu \
	$(SRCDIR)/source/drivers/dbase/dbase.xcu \
	$(SRCDIR)/source/drivers/flat/flat.xcu \
	$(SRCDIR)/source/drivers/calc/calc.xcu \
	$(SRCDIR)/source/drivers/odbc/odbc.xcu

LOCALIZEDFILES = $(COMPONENT_CONFIG_DATA)
XCUFILES = $(COMPONENT_CONFIG_DATA)



##############################################
# Processing and output.
# From dmake's main/solenv/inc/tg_config.mk:
##############################################

PACKAGEDIR = $(subst .,/,$(PACKAGE))
XSLTPACKAGEDIR = $(PACKAGEDIR)

XCSROOTURL = $(WORKDIR)/CustomTarget/connectivity
ifeq ($(GUI),WNT)
XCSROOTURL = file:///$(subst \,/,$(XCSROOTURL))
endif
ifeq ($(GUI),OS2)
XCSROOTURL = file:///$(subst \,/,$(XCSROOTURL))
endif

XCU_DEFAULT = $(foreach file,$(XCUFILES),\
              $(WORKDIR)/CustomTarget/connectivity/registry/data/$(PACKAGEDIR)/$(file))
XCU_LANG = $(foreach file,$(LOCALIZEDFILES),\
           $(foreach lang,$(gb_AllLangResTarget_LANGS),\
           $(WORKDIR)/CustomTarget/connectivity/registry/res/$(lang)/$(PACKAGEDIR)/$(file)))

$(XCU_DEFAULT) : $(OUTPUTDIR)/xml/registry/component-update.dtd \
                 $(OUTPUTDIR)/xml/processing/data_val.xsl \
                 $(OUTPUTDIR)/xml/processing/alllang.xsl

$(WORKDIR)/CustomTarget/connectivity/registry/data/$(PACKAGEDIR)/%.xcu : %.xcu
	mkdir -p $(dir $@) && \
	$(gb_XSLTPROC) --nonet -o $*.val \
		--stringparam xcs $(WORKDIR)/CustomTarget/connectivity/registry/schema/$(XSLTPACKAGEDIR)/$*.xcs \
		--stringparam schemaRoot $(XCSROOTURL)/registry/schema \
		--path $(OUTPUTDIR)/xml/registry \
		$(OUTPUTDIR)/xml/processing/data_val.xsl $< && \
	$(gb_XSLTPROC) --nonet -o $@ \
		--stringparam xcs $(WORKDIR)/CustomTarget/connectivity/registry/schema/$(XSLTPACKAGEDIR)/$*.xcs \
		--stringparam schemaRoot $(XCSROOTURL)/registry/schema \
		--path $(OUTPUTDIR)/xml/registry \
		$(OUTPUTDIR)/xml/processing/alllang.xsl $< && \
	rm $*.val


# vim: set noet sw=4 ts=4:
