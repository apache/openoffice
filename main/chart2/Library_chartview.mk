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



$(eval $(call gb_Library_Library,chartview))

$(eval $(call gb_Library_add_precompiled_header,chartview,$(SRCDIR)/chart2/inc/pch/precompiled_chartview))

$(eval $(call gb_Library_set_componentfile,chartview,chart2/source/view/chartview))

$(eval $(call gb_Library_set_include,chartview,\
	$$(INCLUDE) \
	-I$(SRCDIR)/chart2/inc \
	-I$(SRCDIR)/chart2/inc/pch \
	-I$(SRCDIR)/chart2/source/inc \
	-I$(SRCDIR)/chart2/source/view/inc \
	-I$(OUTDIR)/inc \
))

$(eval $(call gb_Library_add_api,chartview, \
        udkapi \
	offapi \
))

$(eval $(call gb_Library_add_defs,chartview,\
	-DOOO_DLLIMPLEMENTATION_CHARTVIEW \
))

$(eval $(call gb_Library_add_linked_libs,chartview,\
	basegfx \
	charttools \
	comphelper \
	cppu \
	cppuhelper \
	editeng \
	sal \
	sfx \
	stl \
	svl \
	svt \
	svxcore \
	tl \
	utl \
	vcl \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,chartview,\
	chart2/source/view/axes/VAxisOrGridBase \
	chart2/source/view/axes/VAxisBase \
	chart2/source/view/axes/DateHelper \
	chart2/source/view/axes/DateScaling \
	chart2/source/view/axes/Tickmarks \
	chart2/source/view/axes/Tickmarks_Equidistant \
	chart2/source/view/axes/Tickmarks_Dates \
	chart2/source/view/axes/MinimumAndMaximumSupplier \
	chart2/source/view/axes/ScaleAutomatism \
	chart2/source/view/axes/VAxisProperties \
	chart2/source/view/axes/VCartesianAxis \
	chart2/source/view/axes/VCartesianGrid \
	chart2/source/view/axes/VCartesianCoordinateSystem \
	chart2/source/view/axes/VPolarAxis \
	chart2/source/view/axes/VPolarAngleAxis \
	chart2/source/view/axes/VPolarRadiusAxis \
	chart2/source/view/axes/VPolarGrid \
	chart2/source/view/axes/VPolarCoordinateSystem \
	chart2/source/view/axes/VCoordinateSystem \
	chart2/source/view/charttypes/Splines \
	chart2/source/view/charttypes/CategoryPositionHelper \
	chart2/source/view/charttypes/BarPositionHelper \
	chart2/source/view/charttypes/VSeriesPlotter \
	chart2/source/view/charttypes/BarChart \
	chart2/source/view/charttypes/PieChart \
	chart2/source/view/charttypes/AreaChart \
	chart2/source/view/charttypes/CandleStickChart \
	chart2/source/view/charttypes/BubbleChart \
	chart2/source/view/diagram/VDiagram \
	chart2/source/view/main/ChartItemPool \
	chart2/source/view/main/DrawModelWrapper \
	chart2/source/view/main/PropertyMapper \
	chart2/source/view/main/Stripe \
	chart2/source/view/main/VLineProperties \
	chart2/source/view/main/ShapeFactory \
	chart2/source/view/main/VLegendSymbolFactory \
	chart2/source/view/main/DataPointSymbolSupplier \
	chart2/source/view/main/Linear3DTransformation \
	chart2/source/view/main/VPolarTransformation \
	chart2/source/view/main/Clipping \
	chart2/source/view/main/PlottingPositionHelper \
	chart2/source/view/main/LabelPositionHelper \
	chart2/source/view/main/PolarLabelPositionHelper \
	chart2/source/view/main/PlotterBase \
	chart2/source/view/main/VDataSeries \
	chart2/source/view/main/VLegend \
	chart2/source/view/main/VTitle \
	chart2/source/view/main/ChartView \
	chart2/source/view/main/_serviceregistration_view \
))

# vim: set noet sw=4 ts=4:

