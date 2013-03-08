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

#ifndef _SVX_SIDEBAR_VALUESETWITHTEXT_CONTROL_HXX_
#define _SVX_SIDEBAR_VALUESETWITHTEXT_CONTROL_HXX_

#include "svx/svxdllapi.h"

#include <svtools/valueset.hxx>

#include <vcl/image.hxx>

#include <vector>

namespace svx { namespace sidebar {

/** Specialization of class <ValueSet>.
    This specialization allows is a one-columned ValueSet which allow
    items containing an image and a text or a text and a second text.

    Especially, used for sidebar related controls.
*/
class SVX_DLLPUBLIC ValueSetWithTextControl : public ValueSet
{
public:
    // control type of specialized <ValueSet>:
    // - image + text
    // - text + text
    enum tControlType
    {
        IMAGE_TEXT,
        TEXT_TEXT
    };

    ValueSetWithTextControl(
        const tControlType eControlType,
        Window* pParent,
        const ResId& rResId);

    virtual ~ValueSetWithTextControl(void);

    // add item for control type IMAGE_TEXT
    // if control type does not match IMAGE_TEXT no item is added.
    // @param pSelectedItemImage
    // selection item image is optional. if not provided, it is the same as the image item
    // @param pItemHelpText
    // help text is optional. if not provided, it is the same as the item text
    void AddItem(
        const Image& rItemImage,
        const Image* pSelectedItemImage,
        const XubString& rItemText,
        const XubString* pItemHelpText );

    // add item for control type TEXT_TEXT
    // if control type does not match TEXT_TEXT no item is added.
    // @param pItemHelpText
    // help text is optional. if not provided, it is the same as the item text
    void AddItem(
        const XubString& rItemText,
        const XubString& rItemText2,
        const XubString* pItemHelpText );

    virtual void UserDraw( const UserDrawEvent& rUDEvt );

private:
    struct ValueSetWithTextItem
    {
        Image maItemImage;
        Image maSelectedItemImage;
        XubString maItemText;
        XubString maItemText2;
    };

    typedef ::std::vector< ValueSetWithTextItem > tItemList;

    const tControlType meControlType;
    tItemList maItems;
};

} } // end of namespace svx::sidebar

#endif
