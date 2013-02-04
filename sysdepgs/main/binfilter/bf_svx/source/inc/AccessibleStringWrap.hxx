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



#ifndef _ACCESSIBLESTRINGWRAP_HXX
#define _ACCESSIBLESTRINGWRAP_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
class OutputDevice;
class Rectangle;
class Point;
namespace binfilter {


class SvxFont;

//------------------------------------------------------------------------
//
// AccessibleStringWrap declaration
//
//------------------------------------------------------------------------

class AccessibleStringWrap
{
public:

    AccessibleStringWrap( OutputDevice& rDev, SvxFont& rFont, const String& rText ){DBG_BF_ASSERT(0, "STRIP");}//STRIP001 AccessibleStringWrap( OutputDevice& rDev, SvxFont& rFont, const String& rText );

    sal_Bool GetCharacterBounds( sal_Int32 nIndex, Rectangle& rRect ){DBG_BF_ASSERT(0, "STRIP"); return false;}//STRIP001 sal_Bool GetCharacterBounds( sal_Int32 nIndex, Rectangle& rRect );
    sal_Int32 GetIndexAtPoint( const Point& rPoint ){DBG_BF_ASSERT(0, "STRIP"); return 0;}//STRIP001 sal_Int32 GetIndexAtPoint( const Point& rPoint );

private:
    
};

}//end of namespace binfilter
#endif /* _ACCESSIBLESTRINGWRAP_HXX */
