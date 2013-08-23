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

PRJ=../../..
PRJNAME=helpcontent2
TARGET=helper

LANGUAGE_FILELIST=auxiliary/sbasic.tree,\
text/sbasic/guide/create_dialog.xhp,\
text/sbasic/guide/insert_control.xhp,\
text/sbasic/guide/sample_code.xhp,\
text/sbasic/guide/show_dialog.xhp,\
text/sbasic/guide/translation.xhp,\
text/sbasic/shared/00000002.xhp,\
text/sbasic/shared/00000003.xhp,\
text/sbasic/shared/01000000.xhp,\
text/sbasic/shared/01010210.xhp,\
text/sbasic/shared/01020000.xhp,\
text/sbasic/shared/01020100.xhp,\
text/sbasic/shared/01020200.xhp \
text/sbasic/shared/01020300.xhp \
text/sbasic/shared/01020500.xhp \
text/sbasic/shared/01030000.xhp \
text/sbasic/shared/01030100.xhp \
text/sbasic/shared/01030200.xhp \
text/sbasic/shared/01030300.xhp \
text/sbasic/shared/01030400.xhp \
text/sbasic/shared/01040000.xhp \
text/sbasic/shared/01050000.xhp \
text/sbasic/shared/01050100.xhp \
text/sbasic/shared/01050200.xhp \
text/sbasic/shared/01050300.xhp \
text/sbasic/shared/01/06130000.xhp \
text/sbasic/shared/01/06130100.xhp \
text/sbasic/shared/01/06130500.xhp \
text/sbasic/shared/01170100.xhp \
text/sbasic/shared/01170101.xhp \
text/sbasic/shared/01170103.xhp \
text/sbasic/shared/02/11010000.xhp \
text/sbasic/shared/02/11020000.xhp \
text/sbasic/shared/02/11030000.xhp \
text/sbasic/shared/02/11040000.xhp \
text/sbasic/shared/02/11050000.xhp \
text/sbasic/shared/02/11060000.xhp \
text/sbasic/shared/02/11070000.xhp \
text/sbasic/shared/02/11080000.xhp \
text/sbasic/shared/02/11090000.xhp \
text/sbasic/shared/02/11100000.xhp \
text/sbasic/shared/02/11110000.xhp \
text/sbasic/shared/02/11120000.xhp \
text/sbasic/shared/02/11140000.xhp \
text/sbasic/shared/02/11150000.xhp \
text/sbasic/shared/02/11160000.xhp \
text/sbasic/shared/02/11170000.xhp \
text/sbasic/shared/02/11180000.xhp \
text/sbasic/shared/02/11190000.xhp \
text/sbasic/shared/02/20000000.xhp \
text/sbasic/shared/03000000.xhp \
text/sbasic/shared/03010000.xhp \
text/sbasic/shared/03010100.xhp \
text/sbasic/shared/03010101.xhp \
text/sbasic/shared/03010102.xhp \
text/sbasic/shared/03010103.xhp \
text/sbasic/shared/03010200.xhp \
text/sbasic/shared/03010201.xhp \
text/sbasic/shared/03010300.xhp \
text/sbasic/shared/03010301.xhp \
text/sbasic/shared/03010302.xhp \
text/sbasic/shared/03010303.xhp \
text/sbasic/shared/03010304.xhp \
text/sbasic/shared/03010305.xhp \
text/sbasic/shared/03020000.xhp \
text/sbasic/shared/03020100.xhp \
text/sbasic/shared/03020101.xhp \
text/sbasic/shared/03020102.xhp \
text/sbasic/shared/03020103.xhp \
text/sbasic/shared/03020104.xhp \
text/sbasic/shared/03020200.xhp \
text/sbasic/shared/03020201.xhp \
text/sbasic/shared/03020202.xhp \
text/sbasic/shared/03020203.xhp \
text/sbasic/shared/03020204.xhp \
text/sbasic/shared/03020205.xhp \
text/sbasic/shared/03020301.xhp \
text/sbasic/shared/03020302.xhp \
text/sbasic/shared/03020303.xhp \
text/sbasic/shared/03020304.xhp \
text/sbasic/shared/03020305.xhp \
text/sbasic/shared/03020400.xhp \
text/sbasic/shared/03020401.xhp \
text/sbasic/shared/03020402.xhp \
text/sbasic/shared/03020403.xhp \
text/sbasic/shared/03020404.xhp \
text/sbasic/shared/03020405.xhp \
text/sbasic/shared/03020406.xhp \
text/sbasic/shared/03020407.xhp \
text/sbasic/shared/03020408.xhp \
text/sbasic/shared/03020409.xhp \
text/sbasic/shared/03020410.xhp \
text/sbasic/shared/03020411.xhp \
text/sbasic/shared/03020412.xhp \
text/sbasic/shared/03020413.xhp \
text/sbasic/shared/03020414.xhp \
text/sbasic/shared/03020415.xhp \
text/sbasic/shared/03030000.xhp \
text/sbasic/shared/03030100.xhp \
text/sbasic/shared/03030101.xhp \
text/sbasic/shared/03030102.xhp \
text/sbasic/shared/03030103.xhp \
text/sbasic/shared/03030104.xhp \
text/sbasic/shared/03030105.xhp \
text/sbasic/shared/03030106.xhp \
text/sbasic/shared/03030107.xhp \
text/sbasic/shared/03030108.xhp \
text/sbasic/shared/03030110.xhp \
text/sbasic/shared/03030120.xhp \
text/sbasic/shared/03030130.xhp \
text/sbasic/shared/03030200.xhp \
text/sbasic/shared/03030201.xhp \
text/sbasic/shared/03030202.xhp \
text/sbasic/shared/03030203.xhp \
text/sbasic/shared/03030204.xhp \
text/sbasic/shared/03030205.xhp \
text/sbasic/shared/03030206.xhp \
text/sbasic/shared/03030300.xhp \
text/sbasic/shared/03030301.xhp \
text/sbasic/shared/03030302.xhp \
text/sbasic/shared/03030303.xhp \
text/sbasic/shared/03050000.xhp \
text/sbasic/shared/03050100.xhp \
text/sbasic/shared/03050200.xhp \
text/sbasic/shared/03050300.xhp \
text/sbasic/shared/03050500.xhp \
text/sbasic/shared/03060000.xhp \
text/sbasic/shared/03060100.xhp \
text/sbasic/shared/03060200.xhp \
text/sbasic/shared/03060300.xhp \
text/sbasic/shared/03060400.xhp \
text/sbasic/shared/03060500.xhp \
text/sbasic/shared/03060600.xhp \
text/sbasic/shared/03070000.xhp \
text/sbasic/shared/03070100.xhp \
text/sbasic/shared/03070200.xhp \
text/sbasic/shared/03070300.xhp \
text/sbasic/shared/03070400.xhp \
text/sbasic/shared/03070500.xhp \
text/sbasic/shared/03070600.xhp \
text/sbasic/shared/03080000.xhp \
text/sbasic/shared/03080100.xhp \
text/sbasic/shared/03080101.xhp \
text/sbasic/shared/03080102.xhp \
text/sbasic/shared/03080103.xhp \
text/sbasic/shared/03080104.xhp \
text/sbasic/shared/03080200.xhp \
text/sbasic/shared/03080201.xhp \
text/sbasic/shared/03080202.xhp \
text/sbasic/shared/03080300.xhp \
text/sbasic/shared/03080301.xhp \
text/sbasic/shared/03080302.xhp \
text/sbasic/shared/03080400.xhp \
text/sbasic/shared/03080401.xhp \
text/sbasic/shared/03080500.xhp \
text/sbasic/shared/03080501.xhp \
text/sbasic/shared/03080502.xhp \
text/sbasic/shared/03080600.xhp \
text/sbasic/shared/03080601.xhp \
text/sbasic/shared/03080700.xhp \
text/sbasic/shared/03080701.xhp \
text/sbasic/shared/03080800.xhp \
text/sbasic/shared/03080801.xhp \
text/sbasic/shared/03080802.xhp \
text/sbasic/shared/03090000.xhp \
text/sbasic/shared/03090100.xhp \
text/sbasic/shared/03090101.xhp \
text/sbasic/shared/03090102.xhp \
text/sbasic/shared/03090103.xhp \
text/sbasic/shared/03090200.xhp \
text/sbasic/shared/03090201.xhp \
text/sbasic/shared/03090202.xhp \
text/sbasic/shared/03090203.xhp \
text/sbasic/shared/03090300.xhp \
text/sbasic/shared/03090301.xhp \
text/sbasic/shared/03090302.xhp \
text/sbasic/shared/03090303.xhp \
text/sbasic/shared/03090400.xhp \
text/sbasic/shared/03090401.xhp \
text/sbasic/shared/03090402.xhp \
text/sbasic/shared/03090403.xhp \
text/sbasic/shared/03090404.xhp \
text/sbasic/shared/03090405.xhp \
text/sbasic/shared/03090406.xhp \
text/sbasic/shared/03090407.xhp \
text/sbasic/shared/03090408.xhp \
text/sbasic/shared/03090409.xhp \
text/sbasic/shared/03090410.xhp \
text/sbasic/shared/03090411.xhp \
text/sbasic/shared/03090412.xhp \
text/sbasic/shared/03100000.xhp \
text/sbasic/shared/03100050.xhp \
text/sbasic/shared/03100060.xhp \
text/sbasic/shared/03100070.xhp \
text/sbasic/shared/03100080.xhp \
text/sbasic/shared/03100100.xhp \
text/sbasic/shared/03100300.xhp \
text/sbasic/shared/03100400.xhp \
text/sbasic/shared/03100500.xhp \
text/sbasic/shared/03100600.xhp \
text/sbasic/shared/03100700.xhp \
text/sbasic/shared/03100900.xhp \
text/sbasic/shared/03101000.xhp \
text/sbasic/shared/03101100.xhp \
text/sbasic/shared/03101110.xhp \
text/sbasic/shared/03101120.xhp \
text/sbasic/shared/03101130.xhp \
text/sbasic/shared/03101140.xhp \
text/sbasic/shared/03101300.xhp \
text/sbasic/shared/03101400.xhp \
text/sbasic/shared/03101500.xhp \
text/sbasic/shared/03101600.xhp \
text/sbasic/shared/03101700.xhp \
text/sbasic/shared/03102000.xhp \
text/sbasic/shared/03102100.xhp \
text/sbasic/shared/03102101.xhp \
text/sbasic/shared/03102200.xhp \
text/sbasic/shared/03102300.xhp \
text/sbasic/shared/03102400.xhp \
text/sbasic/shared/03102450.xhp \
text/sbasic/shared/03102600.xhp \
text/sbasic/shared/03102700.xhp \
text/sbasic/shared/03102800.xhp \
text/sbasic/shared/03102900.xhp \
text/sbasic/shared/03103000.xhp \
text/sbasic/shared/03103100.xhp \
text/sbasic/shared/03103200.xhp \
text/sbasic/shared/03103300.xhp \
text/sbasic/shared/03103400.xhp \
text/sbasic/shared/03103450.xhp \
text/sbasic/shared/03103500.xhp \
text/sbasic/shared/03103600.xhp \
text/sbasic/shared/03103700.xhp \
text/sbasic/shared/03103800.xhp \
text/sbasic/shared/03103900.xhp \
text/sbasic/shared/03104000.xhp \
text/sbasic/shared/03104100.xhp \
text/sbasic/shared/03104200.xhp \
text/sbasic/shared/03104300.xhp \
text/sbasic/shared/03104400.xhp \
text/sbasic/shared/03104500.xhp \
text/sbasic/shared/03104600.xhp \
text/sbasic/shared/03104700.xhp \
text/sbasic/shared/03110000.xhp \
text/sbasic/shared/03110100.xhp \
text/sbasic/shared/03120000.xhp \
text/sbasic/shared/03120100.xhp \
text/sbasic/shared/03120101.xhp \
text/sbasic/shared/03120102.xhp \
text/sbasic/shared/03120103.xhp \
text/sbasic/shared/03120104.xhp \
text/sbasic/shared/03120105.xhp \
text/sbasic/shared/03120200.xhp \
text/sbasic/shared/03120201.xhp \
text/sbasic/shared/03120202.xhp \
text/sbasic/shared/03120300.xhp \
text/sbasic/shared/03120301.xhp \
text/sbasic/shared/03120302.xhp \
text/sbasic/shared/03120303.xhp \
text/sbasic/shared/03120304.xhp \
text/sbasic/shared/03120305.xhp \
text/sbasic/shared/03120306.xhp \
text/sbasic/shared/03120307.xhp \
text/sbasic/shared/03120308.xhp \
text/sbasic/shared/03120309.xhp \
text/sbasic/shared/03120310.xhp \
text/sbasic/shared/03120311.xhp \
text/sbasic/shared/03120312.xhp \
text/sbasic/shared/03120313.xhp \
text/sbasic/shared/03120314.xhp \
text/sbasic/shared/03120315.xhp \
text/sbasic/shared/03120400.xhp \
text/sbasic/shared/03120401.xhp \
text/sbasic/shared/03120402.xhp \
text/sbasic/shared/03120403.xhp \
text/sbasic/shared/03130000.xhp \
text/sbasic/shared/03130100.xhp \
text/sbasic/shared/03130500.xhp \
text/sbasic/shared/03130600.xhp \
text/sbasic/shared/03130700.xhp \
text/sbasic/shared/03130800.xhp \
text/sbasic/shared/03131000.xhp \
text/sbasic/shared/03131300.xhp \
text/sbasic/shared/03131400.xhp \
text/sbasic/shared/03131500.xhp \
text/sbasic/shared/03131600.xhp \
text/sbasic/shared/03131700.xhp \
text/sbasic/shared/03131800.xhp \
text/sbasic/shared/03131900.xhp \
text/sbasic/shared/03132000.xhp \
text/sbasic/shared/03132100.xhp \
text/sbasic/shared/03132200.xhp \
text/sbasic/shared/03132300.xhp \
text/sbasic/shared/03132400.xhp \
text/sbasic/shared/03132500.xhp \
text/sbasic/shared/05060700.xhp \
text/sbasic/shared/keys.xhp \
text/sbasic/shared/main0211.xhp \
text/sbasic/shared/main0601.xhp



.INCLUDE :	settings.mk

.INCLUDE :	target.mk
