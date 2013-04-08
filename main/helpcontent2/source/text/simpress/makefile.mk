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



# --- Define module and files to be handled ------------------------
MODULE   = help_simpress
XHPFILES = \
        text/simpress/main0000.xhp   \
        text/simpress/main0100.xhp   \
        text/simpress/main0101.xhp   \
        text/simpress/main0102.xhp   \
        text/simpress/main0103.xhp   \
        text/simpress/main0104.xhp   \
        text/simpress/main0105.xhp   \
        text/simpress/main0106.xhp   \
        text/simpress/main0107.xhp   \
        text/simpress/main0113.xhp   \
        text/simpress/main0114.xhp   \
        text/simpress/main0200.xhp   \
        text/simpress/main0202.xhp   \
        text/simpress/main0203.xhp   \
        text/simpress/main0204.xhp   \
        text/simpress/main0206.xhp   \
        text/simpress/main0209.xhp   \
        text/simpress/main0210.xhp   \
        text/simpress/main0211.xhp   \
        text/simpress/main0212.xhp   \
        text/simpress/main0213.xhp   \
        text/simpress/main0214.xhp   \
        text/simpress/main0503.xhp   \
        text/simpress/00/00000004.xhp   \
        text/simpress/00/00000401.xhp   \
        text/simpress/00/00000402.xhp   \
        text/simpress/00/00000403.xhp   \
        text/simpress/00/00000404.xhp   \
        text/simpress/00/00000405.xhp   \
        text/simpress/00/00000406.xhp   \
        text/simpress/00/00000407.xhp   \
        text/simpress/00/00000413.xhp   \
        text/simpress/01/01170000.xhp   \
        text/simpress/01/01180000.xhp   \
        text/simpress/01/01180001.xhp   \
        text/simpress/01/01180002.xhp   \
        text/simpress/01/02110000.xhp   \
        text/simpress/01/02120000.xhp   \
        text/simpress/01/02130000.xhp   \
        text/simpress/01/02140000.xhp   \
        text/simpress/01/02150000.xhp   \
        text/simpress/01/02160000.xhp   \
        text/simpress/01/03050000.xhp   \
        text/simpress/01/03060000.xhp   \
        text/simpress/01/03070000.xhp   \
        text/simpress/01/03080000.xhp   \
        text/simpress/01/03090000.xhp   \
        text/simpress/01/03100000.xhp   \
        text/simpress/01/03110000.xhp   \
        text/simpress/01/03120000.xhp   \
        text/simpress/01/03130000.xhp   \
        text/simpress/01/03150000.xhp   \
        text/simpress/01/03150100.xhp   \
        text/simpress/01/03150300.xhp   \
        text/simpress/01/03151000.xhp   \
        text/simpress/01/03151100.xhp   \
        text/simpress/01/03151200.xhp   \
        text/simpress/01/03152000.xhp   \
        text/simpress/01/03180000.xhp   \
        text/simpress/01/04010000.xhp   \
        text/simpress/01/04020000.xhp   \
        text/simpress/01/04030000m.xhp   \
        text/simpress/01/04030000.xhp   \
        text/simpress/01/04030100.xhp   \
        text/simpress/01/04040000m.xhp   \
        text/simpress/01/04080100.xhp   \
        text/simpress/01/04110000.xhp   \
        text/simpress/01/04110100.xhp   \
        text/simpress/01/04110200.xhp   \
        text/simpress/01/04120000.xhp   \
        text/simpress/01/04130000.xhp   \
        text/simpress/01/04140000.xhp   \
        text/simpress/01/04990000.xhp   \
        text/simpress/01/04990100.xhp   \
        text/simpress/01/04990200.xhp   \
        text/simpress/01/04990300.xhp   \
        text/simpress/01/04990400.xhp   \
        text/simpress/01/04990500.xhp   \
        text/simpress/01/04990600.xhp   \
        text/simpress/01/04990700.xhp   \
        text/simpress/01/05090000m.xhp   \
        text/simpress/01/05100000.xhp   \
        text/simpress/01/05110500m.xhp   \
        text/simpress/01/05120000.xhp   \
        text/simpress/01/05120100.xhp   \
        text/simpress/01/05120500m.xhp   \
        text/simpress/01/05130000.xhp   \
        text/simpress/01/05140000.xhp   \
        text/simpress/01/05150000.xhp   \
        text/simpress/01/05170000.xhp   \
        text/simpress/01/05250000.xhp   \
        text/simpress/01/05250500.xhp   \
        text/simpress/01/05250600.xhp   \
        text/simpress/01/05250700.xhp   \
        text/simpress/01/06030000.xhp   \
        text/simpress/01/06040000.xhp   \
        text/simpress/01/06050000.xhp   \
        text/simpress/01/06060000.xhp   \
        text/simpress/01/06070000.xhp   \
        text/simpress/01/06080000.xhp   \
        text/simpress/01/06100000.xhp   \
        text/simpress/01/06100100.xhp   \
        text/simpress/01/13050000.xhp   \
        text/simpress/01/13050100.xhp   \
        text/simpress/01/13050200.xhp   \
        text/simpress/01/13050300.xhp   \
        text/simpress/01/13050400.xhp   \
        text/simpress/01/13050500.xhp   \
        text/simpress/01/13050600.xhp   \
        text/simpress/01/13050700.xhp   \
        text/simpress/01/13140000.xhp   \
        text/simpress/01/13150000.xhp   \
        text/simpress/01/13160000.xhp   \
        text/simpress/01/13170000.xhp   \
        text/simpress/01/13180000.xhp   \
        text/simpress/01/13180100.xhp   \
        text/simpress/01/13180200.xhp   \
        text/simpress/01/13180300.xhp   \
        text/simpress/01/animationeffect.xhp   \
        text/simpress/01/effectoptionseffect.xhp   \
        text/simpress/01/effectoptionstext.xhp   \
        text/simpress/01/effectoptionstiming.xhp   \
        text/simpress/01/effectoptions.xhp   \
        text/simpress/01/slidesorter.xhp   \
        text/simpress/01/taskpanel.xhp   \
        text/simpress/02/04010000.xhp   \
        text/simpress/02/04020000.xhp   \
        text/simpress/02/04030000.xhp   \
        text/simpress/02/04040000.xhp   \
        text/simpress/02/04060000.xhp   \
        text/simpress/02/04070000.xhp   \
        text/simpress/02/08020000.xhp   \
        text/simpress/02/08060000.xhp   \
        text/simpress/02/10020000.xhp   \
        text/simpress/02/10030000.xhp   \
        text/simpress/02/10030200.xhp   \
        text/simpress/02/10050000.xhp   \
        text/simpress/02/10060000.xhp   \
        text/simpress/02/10070000.xhp   \
        text/simpress/02/10080000.xhp   \
        text/simpress/02/10090000.xhp   \
        text/simpress/02/10100000.xhp   \
        text/simpress/02/10110000.xhp   \
        text/simpress/02/10120000.xhp   \
        text/simpress/02/10130000.xhp   \
        text/simpress/02/11060000.xhp   \
        text/simpress/02/11070000.xhp   \
        text/simpress/02/11080000.xhp   \
        text/simpress/02/11090000.xhp   \
        text/simpress/02/11100000.xhp   \
        text/simpress/02/11110000.xhp   \
        text/simpress/02/13010000.xhp   \
        text/simpress/02/13020000.xhp   \
        text/simpress/02/13030000.xhp   \
        text/simpress/02/13040000.xhp   \
        text/simpress/02/13050000.xhp   \
        text/simpress/02/13060000.xhp   \
        text/simpress/02/13070000.xhp   \
        text/simpress/02/13080000.xhp   \
        text/simpress/02/13090000.xhp   \
        text/simpress/02/13100000.xhp   \
        text/simpress/02/13140000.xhp   \
        text/simpress/02/13150000.xhp   \
        text/simpress/02/13160000.xhp   \
        text/simpress/02/13170000.xhp   \
        text/simpress/02/13180000.xhp   \
        text/simpress/02/13190000.xhp   \
        text/simpress/04/01020000.xhp   \
        text/simpress/guide/3d_create.xhp   \
        text/simpress/guide/animated_gif_create.xhp   \
        text/simpress/guide/animated_gif_save.xhp   \
        text/simpress/guide/animated_objects.xhp   \
        text/simpress/guide/animated_slidechange.xhp   \
        text/simpress/guide/arrange_slides.xhp   \
        text/simpress/guide/background.xhp   \
        text/simpress/guide/change_scale.xhp   \
        text/simpress/guide/footer.xhp   \
        text/simpress/guide/gluepoints.xhp   \
        text/simpress/guide/html_export.xhp   \
        text/simpress/guide/html_import.xhp   \
        text/simpress/guide/individual.xhp   \
        text/simpress/guide/keyboard.xhp   \
        text/simpress/guide/layer_move.xhp   \
        text/simpress/guide/layer_new.xhp   \
        text/simpress/guide/layers.xhp   \
        text/simpress/guide/layer_tipps.xhp   \
        text/simpress/guide/line_arrow_styles.xhp   \
        text/simpress/guide/line_draw.xhp   \
        text/simpress/guide/line_edit.xhp   \
        text/simpress/guide/main.xhp   \
        text/simpress/guide/masterpage.xhp   \
        text/simpress/guide/move_object.xhp   \
        text/simpress/guide/orgchart.xhp   \
        text/simpress/guide/page_copy.xhp   \
        text/simpress/guide/palette_files.xhp   \
        text/simpress/guide/printing.xhp   \
        text/simpress/guide/print_tofit.xhp   \
        text/simpress/guide/rehearse_timings.xhp   \
        text/simpress/guide/select_object.xhp   \
        text/simpress/guide/show.xhp   \
        text/simpress/guide/table_insert.xhp   \
        text/simpress/guide/text2curve.xhp   \
        text/simpress/guide/vectorize.xhp   \
        simpress.tree

# --- Targets ------------------------------------------------------
.INCLUDE : ../../../make_helper.mk
