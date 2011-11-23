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



#ifndef _SVX_UNOEDHLP_HXX
#define _SVX_UNOEDHLP_HXX

#include <memory>
#include <bf_svtools/bf_solar.h>
#include <tools/gen.hxx> 

namespace binfilter {

struct EENotify;
class EditEngine;

/** Helper class for common functionality in edit sources
 */
class SvxEditSourceHelper
{
public:
    /** Calculate attribute run for EditEngines

    	Please note that the range returned is half-open: [nStartIndex,nEndIndex)

    	@param nStartIndex 
        Herein, the start index of the range of similar attributes is returned

    	@param nEndIndex 
        Herein, the end index (exclusive) of the range of similar attributes is returned

        @param rEE
        The EditEngine to query for attributes

        @param nPara
        The paragraph the following index value is to be interpreted in

        @param nIndex
        The character index from which the range of similar attributed characters is requested

        @return sal_True, if the range has been successfully determined
     */
    static sal_Bool GetAttributeRun( USHORT& nStartIndex, USHORT& nEndIndex, const EditEngine& rEE, USHORT nPara, USHORT nIndex );

    /** Convert point from edit engine to user coordinate space

    	As the edit engine internally keeps vertical text unrotated,
    	all internal edit engine methods return their stuff unrotated,
    	too. This method rotates and shifts given point appropriately,
    	if vertical writing is on.

        @param rPoint
        Point to transform

        @param rEESize
        Paper size of the edit engine

        @param  bIsVertical
        Whether output text is vertical or not

        @return the possibly transformed point
     */
    static Point EEToUserSpace( const Point& rPoint, const Size& rEESize, bool bIsVertical );

    /** Convert point from user to edit engine coordinate space

    	As the edit engine internally keeps vertical text unrotated,
    	all internal edit engine methods return their stuff unrotated,
    	too. This method rotates and shifts given point appropriately,
    	if vertical writing is on.

        @param rPoint
        Point to transform

        @param rEESize
        Paper size of the edit engine

        @param  bIsVertical
        Whether output text is vertical or not

        @return the possibly transformed point
     */
    static Point UserSpaceToEE( const Point& rPoint, const Size& rEESize, bool bIsVertical );

    /** Convert rect from edit engine to user coordinate space

    	As the edit engine internally keeps vertical text unrotated,
    	all internal edit engine methods return their stuff unrotated,
    	too. This method rotates and shifts given rect appropriately,
    	if vertical writing is on.

        @param rRect
        Rectangle to transform

        @param rEESize
        Paper size of the edit engine

        @param  bIsVertical
        Whether output text is vertical or not

        @return the possibly transformed rect
     */
    static Rectangle EEToUserSpace( const Rectangle& rRect, const Size& rEESize, bool bIsVertical );
};

}//end of namespace binfilter
#endif

