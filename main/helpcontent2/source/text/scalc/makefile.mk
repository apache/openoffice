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



# edit to match directory level 
PRJ		= ..$/..$/..
# same for all makefiles in "helpcontent2"
PRJNAME = help_scalc
# edit to match the current package
PACKAGE = text/scalc
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_scalc
# edit to match the current module
MODULE  = scalc

# --- Settings -----------------------------------------------------
LANGUAGE_FILELIST=../../auxiliary/scalc.tree \
00/00000004.xhp \
00/00000402.xhp \
00/00000403.xhp \
00/00000404.xhp \
00/00000405.xhp \
00/00000406.xhp \
00/00000407.xhp \
00/00000412.xhp \
01/01120000.xhp \
01/02110000.xhp \
01/02120000.xhp \
01/02120100.xhp \
01/02140000.xhp \
01/02140100.xhp \
01/02140200.xhp \
01/02140300.xhp \
01/02140400.xhp \
01/02140500.xhp \
01/02140600.xhp \
01/02150000.xhp \
01/02160000.xhp \
01/02170000.xhp \
01/02190000.xhp \
01/02190100.xhp \
01/02190200.xhp \
01/02200000.xhp \
01/02210000.xhp \
01/03070000.xhp \
01/03080000.xhp \
01/03090000.xhp \
01/03100000.xhp \
01/04010000.xhp \
01/04010100.xhp \
01/04010200.xhp \
01/04020000.xhp \
01/04030000.xhp \
01/04040000.xhp \
01/04050000.xhp \
01/04050100.xhp \
01/04060000.xhp \
01/04060100.xhp \
01/04060101.xhp \
01/04060102.xhp \
01/04060103.xhp \
01/04060104.xhp \
01/04060105.xhp \
01/04060106.xhp \
01/04060107.xhp \
01/04060108.xhp \
01/04060109.xhp \
01/04060110.xhp \
01/04060111.xhp \
01/04060112.xhp \
01/04060115.xhp \
01/04060116.xhp \
01/04060118.xhp \
01/04060119.xhp \
01/04060181.xhp \
01/04060182.xhp \
01/04060183.xhp \
01/04060184.xhp \
01/04060185.xhp \
01/04060199.xhp \
01/04070000.xhp \
01/04070100.xhp \
01/04070200.xhp \
01/04070300.xhp \
01/04070400.xhp \
01/04080000.xhp \
01/04090000.xhp \
01/05020000.xhp \
01/05020600.xhp \
01/05030000.xhp \
01/05030200.xhp \
01/05030300.xhp \
01/05030400.xhp \
01/05040000.xhp \
01/05040200.xhp \
01/05050000.xhp \
01/05050100.xhp \
01/05050300.xhp \
01/05060000.xhp \
01/05070000.xhp \
01/05070500.xhp \
01/05080000.xhp \
01/05080100.xhp \
01/05080200.xhp \
01/05080300.xhp \
01/05080400.xhp \
01/05100000.xhp \
01/05110000.xhp \
01/05120000.xhp \
01/06020000.xhp \
01/06030000.xhp \
01/06030100.xhp \
01/06030200.xhp \
01/06030300.xhp \
01/06030400.xhp \
01/06030500.xhp \
01/06030600.xhp \
01/06030700.xhp \
01/06030800.xhp \
01/06030900.xhp \
01/06031000.xhp \
01/06040000.xhp \
01/06050000.xhp \
01/06060000.xhp \
01/06060100.xhp \
01/06060200.xhp \
01/06070000.xhp \
01/06080000.xhp \
01/06130000.xhp \
01/06990000.xhp \
01/07080000.xhp \
01/07090000.xhp \
01/12010000.xhp \
01/12010100.xhp \
01/12020000.xhp \
01/12030000.xhp \
01/12030100.xhp \
01/12030200.xhp \
01/12040000.xhp \
01/12040100.xhp \
01/12040201.xhp \
01/12040300.xhp \
01/12040400.xhp \
01/12040500.xhp \
01/12050000.xhp \
01/12050100.xhp \
01/12050200.xhp \
01/12060000.xhp \
01/12070000.xhp \
01/12070100.xhp \
01/12080000.xhp \
01/12080100.xhp \
01/12080200.xhp \
01/12080300.xhp \
01/12080400.xhp \
01/12080500.xhp \
01/12080600.xhp \
01/12080700.xhp \
01/12090000.xhp \
01/12090100.xhp \
01/12090101.xhp \
01/12090102.xhp \
01/12090103.xhp \
01/12090104.xhp \
01/12090105.xhp \
01/12090106.xhp \
01/12090200.xhp \
01/12090300.xhp \
01/12090400.xhp \
01/12090105.xhp \
01/12090106.xhp \
01/12090200.xhp \
01/12090300.xhp \
01/12090400.xhp \
01/12100000.xhp \
01/12120000.xhp \
01/12120100.xhp \
01/12120200.xhp \
01/12120300.xhp \
01/format_graphic.xhp \
01/func_datevalue.xhp \
01/func_date.xhp \
01/func_days360.xhp \
01/func_days.xhp \
01/func_day.xhp \
01/func_eastersunday.xhp \
01/func_edate.xhp \
01/func_eomonth.xhp \
01/func_hour.xhp \
01/func_minute.xhp \
01/func_month.xhp \
01/func_networkdays.xhp \
01/func_now.xhp \
01/func_second.xhp \
01/func_timevalue.xhp \
01/func_time.xhp \
01/func_today.xhp \
01/func_weekday.xhp \
01/func_weeknumadd.xhp \
01/func_weeknum.xhp \
01/func_workday.xhp \
01/func_yearfrac.xhp \
01/func_year.xhp \
01/solver_options.xhp \
01/solver.xhp \
01/text2columns.xhp \
02/02130000.xhp \
02/02140000.xhp \
02/02150000.xhp \
02/02160000.xhp \
02/02170000.xhp \
02/06010000.xhp \
02/06030000.xhp \
02/06040000.xhp \
02/06050000.xhp \
02/06060000.xhp \
02/06070000.xhp \
02/06080000.xhp \
02/08010000.xhp \
02/08080000.xhp \
02/10050000.xhp \
02/10060000.xhp \
02/18010000.xhp \
02/18020000.xhp \
04/01020000.xhp \
05/02140000.xhp \
05/empty_cells.xhp \
guide/address_auto.xhp \
guide/autofilter.xhp \
guide/autoformat.xhp \
guide/auto_off.xhp \
guide/background.xhp \
guide/borders.xhp \
guide/calc_date.xhp \
guide/calc_series.xhp \
guide/calc_timevalues.xhp \
guide/calculate.xhp \
guide/cellcopy.xhp \
guide/cell_enter.xhp \
guide/cell_protect.xhp \
guide/cellreference_dragdrop.xhp \
guide/cellreferences_url.xhp \
guide/cellreferences.xhp \
guide/cellstyle_by_formula.xhp \
guide/cellstyle_conditional.xhp \
guide/cellstyle_minusvalue.xhp \
guide/cell_unprotect.xhp \
guide/consolidate.xhp \
guide/csv_files.xhp \
guide/csv_formula.xhp \
guide/currency_format.xhp \
guide/database_define.xhp \
guide/database_filter.xhp \
guide/database_sort.xhp \
guide/datapilot_createtable.xhp \
guide/datapilot_deletetable.xhp \
guide/datapilot_edittable.xhp \
guide/datapilot_filtertable.xhp \
guide/datapilot_grouping.xhp \
guide/datapilot_tipps.xhp \
guide/datapilot_updatetable.xhp \
guide/datapilot.xhp \
guide/dbase_files.xhp \
guide/design.xhp \
guide/edit_multitables.xhp \
guide/filters.xhp \
guide/finding.xhp \
guide/format_table.xhp \
guide/format_value_userdef.xhp \
guide/format_value.xhp \
guide/formula_copy.xhp \
guide/formula_enter.xhp \
guide/formulas.xhp \
guide/formula_value.xhp \
guide/fraction_enter.xhp \
guide/goalseek.xhp \
guide/html_doc.xhp \
guide/integer_leading_zero.xhp \
guide/keyboard.xhp \
guide/line_fix.xhp \
guide/main.xhp \
guide/mark_cells.xhp \
guide/matrixformula.xhp \
guide/move_dragdrop.xhp \
guide/multioperation.xhp \
guide/multi_tables.xhp \
guide/multitables.xhp \
guide/note_insert.xhp \
guide/numbers_text.xhp \
guide/print_details.xhp \
guide/print_exact.xhp \
guide/print_landscape.xhp \
guide/printranges.xhp \
guide/print_title_row.xhp \
guide/relativ_absolut_ref.xhp \
guide/rename_table.xhp \
guide/rounding_numbers.xhp \
guide/row_height.xhp \
guide/scenario.xhp \
guide/sorted_list.xhp \
guide/specialfilter.xhp \
guide/super_subscript.xhp \
guide/table_cellmerge.xhp \
guide/table_rotate.xhp \
guide/table_view.xhp \
guide/text_numbers.xhp \
guide/text_rotate.xhp \
guide/text_wrap.xhp \
guide/userdefined_function.xhp \
guide/validity.xhp \
guide/value_with_name.xhp \
guide/webquery.xhp \
guide/year2000.xhp \
main0000.xhp \
main0100.xhp \
main0101.xhp \
main0102.xhp \
main0103.xhp \
main0104.xhp \
main0105.xhp \
main0106.xhp \
main0107.xhp \
main0112.xhp \
main0200.xhp \
main0202.xhp \
main0203.xhp \
main0205.xhp \
main0206.xhp \
main0208.xhp \
main0210.xhp \
main0214.xhp \
main0218.xhp \
main0503.xhp

.IF "$(MAKETARGETS)"!="genPO"
.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   main0000.xhp \
   main0100.xhp \
   main0101.xhp \
   main0102.xhp \
   main0103.xhp \
   main0104.xhp \
   main0105.xhp \
   main0106.xhp \
   main0107.xhp \
   main0112.xhp \
   main0200.xhp \
   main0202.xhp \
   main0203.xhp \
   main0205.xhp \
   main0206.xhp \
   main0208.xhp \
   main0210.xhp \
   main0214.xhp \
   main0218.xhp \
   main0503.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
.ELSE
.INCLUDE :  settings.mk
.INCLUDE :  target.mk
.ENDIF
