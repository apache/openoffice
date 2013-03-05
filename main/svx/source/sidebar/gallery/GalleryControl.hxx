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

#ifndef SVX_SIDEBAR_GALLERY_CONTROL_HXX
#define SVX_SIDEBAR_GALLERY_CONTROL_HXX

#include <vcl/window.hxx>

#include <boost/scoped_ptr.hpp>

class SfxBindings;

class Gallery;
class GallerySplitter;
class GalleryBrowser1;
class GalleryBrowser2;

namespace svx { namespace sidebar {

class GalleryControl : public Window
{
public:
    GalleryControl (
        SfxBindings* pBindings,
        Window* pParentWindow);
    virtual ~GalleryControl (void);

private:
    Gallery* mpGallery;
    ::boost::scoped_ptr<GallerySplitter> mpSplitter;
    ::boost::scoped_ptr<GalleryBrowser1> mpBrowser1;
    ::boost::scoped_ptr<GalleryBrowser2> mpBrowser2;

    void InitSettings (void);
};

} } // end of namespace svx::sidebar

#endif
