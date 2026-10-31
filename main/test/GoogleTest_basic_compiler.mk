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

# This GoogleTest tests the BASIC preprocessor/conditional compilation
# by including the tokenizer, scanner, and parser source files directly.
# The parser (parser.cxx) and its dependencies (symtbl.cxx, codegen.cxx,
# exprtree.cxx, exprnode.cxx, dim.cxx, buffer.cxx, exprgen.cxx, io.cxx,
# loops.cxx, sbcomp.cxx) are compiled directly to test the full
# SbiParser::Parse() flow including #If/#Const/#Else/#ElseIf/#End If.

$(eval $(call gb_GoogleTest_GoogleTest,basic_compiler))

$(eval $(call gb_GoogleTest_add_exception_objects,basic_compiler,\
	test/qa/main \
	test/source/cpp/basictest \
	test/source/basic/stub_getsbdat \
	test/source/basic/sbxmod_override \
	basic/source/comp/scanner \
	basic/source/comp/token \
	basic/source/comp/parser \
	basic/source/comp/symtbl \
	basic/source/comp/codegen \
	basic/source/comp/exprtree \
	basic/source/comp/exprnode \
	basic/source/comp/dim \
	basic/source/comp/buffer \
	basic/source/comp/exprgen \
	basic/source/comp/io \
	basic/source/comp/loops \
	basic/source/comp/sbcomp \
))

$(eval $(call gb_GoogleTest_set_include,basic_compiler,\
	$$(INCLUDE) \
	-I$(SRCDIR)/test/inc \
	-I$(SRCDIR)/basic/inc \
	-I$(SRCDIR)/basic/inc/pch \
	-I$(SRCDIR)/basic/source/inc \
))

$(eval $(call gb_GoogleTest_add_api,basic_compiler,\
	offapi \
	udkapi \
))

$(eval $(call gb_GoogleTest_add_linked_libs,basic_compiler,\
	comphelper \
	cppu \
	cppuhelper \
	gtest \
	sal \
	sb \
	svl \
	sot \
	stl \
	tl \
	utl \
	vcl \
	vos3 \
	$(gb_STDLIBS) \
))

# vim: set noet sw=4 ts=4:
