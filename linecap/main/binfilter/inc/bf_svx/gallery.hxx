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



#ifndef _GALLERY_HXX_
#define _GALLERY_HXX_

#include <bf_svtools/bf_solar.h>
class List;
class Graphic;
class Bitmap;
namespace binfilter {

// -----------
// - Defines -
// -----------

#define SGA_FORMAT_NONE		0x00000000L
#define	SGA_FORMAT_STRING	0x00000001L
#define	SGA_FORMAT_GRAPHIC	0x00000010L
#define	SGA_FORMAT_SOUND	0x00000100L
#define SGA_FORMAT_OLE          0x00001000L
#define	SGA_FORMAT_SVDRAW	0x00010000L
#define	SGA_FORMAT_ALL		0xFFFFFFFFL

#define SVX_GALLERY() (GalleryExplorer::GetGallery())

// Defines for preinstalled themes
#define GALLERY_THEME_3D                1
#define GALLERY_THEME_ANIMATIONS	2
#define GALLERY_THEME_BULLETS		3
#define GALLERY_THEME_OFFICE            4
#define GALLERY_THEME_FLAGS             5
#define GALLERY_THEME_FLOWCHARTS	6
#define GALLERY_THEME_EMOTICONS         7
#define GALLERY_THEME_PHOTOS		8
#define GALLERY_THEME_BACKGROUNDS	9
#define GALLERY_THEME_HOMEPAGE		10
#define GALLERY_THEME_INTERACTION	11
#define GALLERY_THEME_MAPS              12
#define GALLERY_THEME_PEOPLE            13
#define GALLERY_THEME_SURFACES		14
#define GALLERY_THEME_HTMLBUTTONS	15
#define GALLERY_THEME_POWERPOINT	16
#define GALLERY_THEME_RULERS		17
#define GALLERY_THEME_SOUNDS		18
#define GALLERY_THEME_SYMBOLS		19
#define GALLERY_THEME_MYTHEME		20
#define GALLERY_THEME_USERSOUNDS	21
#define GALLERY_THEME_ARROWS            22
#define GALLERY_THEME_BALLOONS          23
#define GALLERY_THEME_KEYBOARD          24
#define GALLERY_THEME_TIME              25
#define GALLERY_THEME_PRESENTATION      26
#define GALLERY_THEME_CALENDAR          27
#define GALLERY_THEME_NAVIGATION        28
#define GALLERY_THEME_COMMUNICATION     29
#define GALLERY_THEME_FINANCES          30
#define GALLERY_THEME_COMPUTER          31
#define GALLERY_THEME_CLIMA             32
#define GALLERY_THEME_EDUCATION         33
#define GALLERY_THEME_TROUBLE           34
#define GALLERY_THEME_SCREENBEANS       35

#define GALLERY_THEME_DUMMY5            36

// -------------------
// - GalleryExplorer -
// -------------------

class Gallery;
class VCDrawModel;
class FmFormModel;
class FmFormModel;


}//end of namespace binfilter
#endif // _GALLERY_HXX_
