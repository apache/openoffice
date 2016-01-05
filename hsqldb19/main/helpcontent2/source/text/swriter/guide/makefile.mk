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
PRJ		= ..$/..$/..$/..
# same for all makefiles in "helpcontent2"
PRJNAME = helpcontent2
# edit to match the current package
PACKAGE = text/swriter/guide
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_swriter_guide
# edit to match the current module
MODULE  = swriter

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   anchor_object.xhp \
   arrange_chapters.xhp \
   auto_numbering.xhp \
   auto_off.xhp \
   auto_spellcheck.xhp \
   autocorr_except.xhp \
   autotext.xhp \
   background.xhp \
   border_object.xhp \
   border_page.xhp \
   borders.xhp \
   calculate.xhp \
   calculate_clipboard.xhp \
   calculate_intable.xhp \
   calculate_intext.xhp \
   calculate_intext2.xhp \
   calculate_multitable.xhp \
   captions.xhp \
   captions_numbers.xhp \
   change_header.xhp \
   chapter_numbering.xhp \
   conditional_text.xhp \
   conditional_text2.xhp \
   delete_from_dict.xhp \
   dragdroptext.xhp \
   even_odd_sdw.xhp \
   field_convert.xhp \
   fields.xhp \
   fields_date.xhp \
   fields_enter.xhp \
   fields_userdata.xhp \
   finding.xhp \
   footer_nextpage.xhp \
   footer_pagenumber.xhp \
   footnote_usage.xhp \
   footnote_with_line.xhp \
   form_letters_main.xhp \
   globaldoc.xhp \
   globaldoc_howtos.xhp \
   header_footer.xhp \
   header_pagestyles.xhp \
   header_with_chapter.xhp \
   header_with_line.xhp \
   hidden_text.xhp \
   hidden_text_display.xhp \
   hyperlinks.xhp \
   hyphen_prevent.xhp \
   indenting.xhp \
   indices_delete.xhp \
   indices_edit.xhp \
   indices_enter.xhp \
   indices_form.xhp \
   indices_index.xhp \
   indices_literature.xhp \
   indices_multidoc.xhp \
   indices_toc.xhp \
   indices_userdef.xhp \
   insert_beforetable.xhp \
   insert_graphic.xhp \
   insert_graphic_dialog.xhp \
   insert_graphic_fromchart.xhp \
   insert_graphic_fromdraw.xhp \
   insert_graphic_gallery.xhp \
   insert_graphic_scan.xhp \
   insert_line.xhp \
   insert_tab_innumbering.xhp \
   join_numbered_lists.xhp \
   jump2statusbar.xhp \
   keyboard.xhp \
   load_styles.xhp \
   main.xhp \
   navigator.xhp \
   nonprintable_text.xhp \
   number_date_conv.xhp \
   number_sequence.xhp \
   numbering_lines.xhp \
   numbering_paras.xhp \
   page_break.xhp \
   pagebackground.xhp \
   pagenumbers.xhp \
   pageorientation.xhp \
   pagestyles.xhp \
   print_brochure.xhp \
   print_preview.xhp \
   print_small.xhp \
   printer_tray.xhp \
   printing_order.xhp \
   protection.xhp \
   references.xhp \
   references_modify.xhp \
   registertrue.xhp \
   removing_line_breaks.xhp \
   reset_format.xhp \
   resize_navigator.xhp \
   ruler.xhp \
   search_regexp.xhp \
   section_edit.xhp \
   section_insert.xhp \
   sections.xhp \
   send2html.xhp \
   shortcut_writing.xhp \
   smarttags.xhp \
   spellcheck_dialog.xhp \
   stylist_fillformat.xhp \
   stylist_fromselect.xhp \
   stylist_update.xhp \
   subscript.xhp \
   table_cellmerge.xhp \
   table_cells.xhp \
   table_delete.xhp \
   table_insert.xhp \
   table_repeat_multiple_headers.xhp \
   table_select.xhp \
   table_sizing.xhp \
   tablemode.xhp \
   template_create.xhp \
   template_default.xhp \
   templates_styles.xhp \
   text_animation.xhp \
   text_capital.xhp \
   text_centervert.xhp \
   text_direct_cursor.xhp \
   text_emphasize.xhp \
   text_frame.xhp \
   text_nav_keyb.xhp \
   text_rotate.xhp \
   textdoc_inframe.xhp \
   using_hyphen.xhp \
   using_numbered_lists.xhp \
   using_numbered_lists2.xhp \
   using_numbering.xhp \
   using_thesaurus.xhp \
   word_completion.xhp \
   word_completion_adjust.xhp \
   words_count.xhp \
   wrap.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
