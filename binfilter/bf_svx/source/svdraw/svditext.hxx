/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef _SVDITEXT_HXX
#define _SVDITEXT_HXX

// SvxItem-Mapping. Wird benoetigt um die SvxItem-Header erfolgreich zu includen

namespace binfilter {

//      textitem.hxx        editdata.hxx
#define ITEMID_COLOR        EE_CHAR_COLOR
#define ITEMID_FONT         EE_CHAR_FONTINFO
#define ITEMID_FONTHEIGHT   EE_CHAR_FONTHEIGHT
#define ITEMID_CHARSCALE_W	EE_CHAR_FONTWIDTH
#define ITEMID_WEIGHT       EE_CHAR_WEIGHT
#define ITEMID_UNDERLINE    EE_CHAR_UNDERLINE
#define ITEMID_CROSSEDOUT   EE_CHAR_STRIKEOUT
#define ITEMID_POSTURE      EE_CHAR_ITALIC
#define ITEMID_CONTOUR      EE_CHAR_OUTLINE
#define ITEMID_SHADOWED     EE_CHAR_SHADOW
#define ITEMID_ESCAPEMENT   EE_CHAR_ESCAPEMENT
#define ITEMID_AUTOKERN     EE_CHAR_PAIRKERNING
#define ITEMID_WORDLINEMODE EE_CHAR_WLM

//      paraitem.hxx       editdata.hxx
#define ITEMID_ADJUST      EE_PARA_JUST
#define ITEMID_FIELD       EE_FEATURE_FIELD

}//end of namespace binfilter
#endif  // _SVDITEXT_HXX

