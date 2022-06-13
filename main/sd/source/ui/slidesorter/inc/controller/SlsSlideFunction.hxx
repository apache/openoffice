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



#ifndef SD_SLIDESORTER_SLIDE_FUNCTION_HXX
#define SD_SLIDESORTER_SLIDE_FUNCTION_HXX

#include "fupoor.hxx"

class SdDrawDocument;

namespace sd { namespace slidesorter {
class SlideSorter;
} }


namespace sd { namespace slidesorter { namespace controller {

class SlideSorterController;


/** Base class for functions of the slide sorter.
*/
class SlideFunction
    : public FuPoor
{
public:
	TYPEINFO();

	virtual sal_Bool MouseMove (const MouseEvent& rMEvt);
	virtual sal_Bool MouseButtonUp (const MouseEvent& rMEvt);
	virtual sal_Bool MouseButtonDown (const MouseEvent& rMEvt);

    /** Called from ForceScroll() before the actual scrolling.
    */
	virtual void ScrollStart (void);

    /** Called from ForceScroll() after the actual scrolling.
    */
	virtual void ScrollEnd (void);

protected:
	SlideFunction (
        SlideSorter& rSlideSorter,
        SfxRequest& rRequest);
};

} } } // end of namespace ::sd::slidesorter::controller

#endif
