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



$(eval $(call gb_Library_Library,charttools))

$(eval $(call gb_Library_add_precompiled_header,charttools,$(SRCDIR)/charttools/inc/pch/precompiled_charttools))

$(eval $(call gb_Library_set_componentfile,charttools,chart2/source/tools/charttools))

$(eval $(call gb_Library_set_include,charttools,\
	$$(INCLUDE) \
	-I$(SRCDIR)/chart2/inc \
	-I$(SRCDIR)/chart2/inc/pch \
	-I$(SRCDIR)/chart2/source/inc \
	-I$(OUTDIR)/inc \
))

$(eval $(call gb_Library_add_api,charttools, \
        udkapi \
	offapi \
))

$(eval $(call gb_Library_add_defs,charttools,\
	-DOOO_DLLIMPLEMENTATION_CHARTTOOLS \
))

$(eval $(call gb_Library_add_linked_libs,charttools,\
	basegfx \
	comphelper \
	cppu \
	cppuhelper \
	i18nisolang1 \
	sal \
	stl \
	svl \
	tl \
	vcl \
	utl \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,charttools,\
	chart2/source/tools/ErrorBar \
	chart2/source/tools/TrueGuard \
	chart2/source/tools/LifeTime \
	chart2/source/tools/MediaDescriptorHelper \
	chart2/source/tools/ConfigColorScheme \
	chart2/source/tools/ColorPerPointHelper \
	chart2/source/tools/ObjectIdentifier \
	chart2/source/tools/CachedDataSequence \
	chart2/source/tools/CommonConverters \
	chart2/source/tools/DataSeriesHelper \
	chart2/source/tools/DataSource \
	chart2/source/tools/DataSourceHelper \
	chart2/source/tools/DiagramHelper \
	chart2/source/tools/ExponentialRegressionCurveCalculator \
	chart2/source/tools/ImplOPropertySet \
	chart2/source/tools/InternalData \
	chart2/source/tools/InternalDataProvider \
	chart2/source/tools/LabeledDataSequence \
	chart2/source/tools/LinearRegressionCurveCalculator \
	chart2/source/tools/LogarithmicRegressionCurveCalculator \
	chart2/source/tools/MeanValueRegressionCurveCalculator \
	chart2/source/tools/NumberFormatterWrapper \
	chart2/source/tools/OPropertySet \
	chart2/source/tools/WrappedPropertySet \
	chart2/source/tools/WrappedProperty \
	chart2/source/tools/WrappedIgnoreProperty \
	chart2/source/tools/WrappedDefaultProperty \
	chart2/source/tools/WrappedDirectStateProperty \
	chart2/source/tools/PotentialRegressionCurveCalculator \
	chart2/source/tools/RegressionCurveHelper \
	chart2/source/tools/RegressionCurveModel \
	chart2/source/tools/RelativeSizeHelper \
	chart2/source/tools/RelativePositionHelper \
	chart2/source/tools/Scaling \
	chart2/source/tools/SceneProperties \
	chart2/source/tools/ThreeDHelper \
	chart2/source/tools/StatisticsHelper \
	chart2/source/tools/ChartModelHelper \
	chart2/source/tools/ChartViewHelper \
	chart2/source/tools/ChartTypeHelper \
	chart2/source/tools/AxisHelper \
	chart2/source/tools/MutexContainer \
	chart2/source/tools/PropertyHelper \
	chart2/source/tools/FormattedStringHelper \
	chart2/source/tools/TitleHelper \
	chart2/source/tools/LegendHelper \
	chart2/source/tools/CharacterProperties \
	chart2/source/tools/LineProperties \
	chart2/source/tools/FillProperties \
	chart2/source/tools/UserDefinedProperties \
	chart2/source/tools/NameContainer \
	chart2/source/tools/ChartDebugTrace \
	chart2/source/tools/_serviceregistration_tools \
	chart2/source/tools/UncachedDataSequence \
	chart2/source/tools/XMLRangeHelper \
	chart2/source/tools/ModifyListenerHelper \
	chart2/source/tools/ModifyListenerCallBack \
	chart2/source/tools/BaseGFXHelper \
	chart2/source/tools/ControllerLockGuard \
	chart2/source/tools/WeakListenerAdapter \
	chart2/source/tools/ResId \
	chart2/source/tools/RessourceManager \
	chart2/source/tools/RangeHighlighter \
	chart2/source/tools/ReferenceSizeProvider \
	chart2/source/tools/ExplicitCategoriesProvider \
	chart2/source/tools/RegressionCurveCalculator \
	chart2/source/tools/RegressionEquation \
))

# Disabled:
# chart2/source/tools/NamedFillProperties
# chart2/source/tools/NamedLineProperties
# chart2/source/tools/NamedProperties

# vim: set noet sw=4 ts=4:
