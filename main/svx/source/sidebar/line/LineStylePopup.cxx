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

#include "LineStylePopup.hxx"
#include "LineStyleControl.hxx"


namespace svx { namespace sidebar {

LineStylePopup::LineStylePopup (
    Window* pParent,
    const ::boost::function<PopupControl*(PopupContainer*)>& rControlCreator)
    : Popup(
        pParent,
        rControlCreator, 
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Style")))
{
}




LineStylePopup::~LineStylePopup (void)
{
}




void LineStylePopup::CreateContainerAndControl (void)
{

    Popup::CreateContainerAndControl();

    LineStyleControl* pControl = dynamic_cast<LineStyleControl*>(mpControl.get());
    if (pControl != NULL)
    {
        Size aSize = pControl->GetSizePixel();
        Size aSizeVS(182, 23*12);

        pControl->GetValueSet().SetSizePixel(aSizeVS);
        aSize.setWidth (aSize.Width() + aSizeVS.Width());
        aSize.setHeight (aSize.Height() + aSizeVS.Height());
        pControl->SetSizePixel(aSize);
        pControl->GetPB().SetSizePixel(
            Size(
                aSizeVS.Width() - 15 ,
                pControl->GetPB().GetSizePixel().Height()) );
        pControl->GetPB().SetPosPixel(
            Point(
                pControl->GetPB().GetPosPixel().X(),
                pControl->GetPB().GetPosPixel().Y() + aSizeVS.Height()));
    }
}




void LineStylePopup::SetStyleSelect (
	XLineStyleItem* pStyleItem,
    XLineDashItem* pDashItem,
    const bool bIsStyleItemAvailable,
    const bool bIsDashItemAvailable)
{
    ProvideContainerAndControl();

    LineStyleControl* pControl = dynamic_cast<LineStyleControl*>(mpControl.get());
    if (pControl != NULL)
        pControl->SetStyleSelect(pStyleItem, pDashItem, bIsStyleItemAvailable, bIsDashItemAvailable);
}


} } // end of namespace svx::sidebar
