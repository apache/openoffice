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



#ifndef SVTOOLS_INC_IMAGERESOURCEACCESS_HXX
#define SVTOOLS_INC_IMAGERESOURCEACCESS_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
/** === end UNO includes === **/

class SvStream;
//........................................................................
namespace binfilter
{
//........................................................................

    //====================================================================
    //= ImageResourceAccess
    //====================================================================
    /** helper class for obtaining streams (which also can be used with the ImageProducer)
        from a resource
    */
    class ImageResourceAccess
    {
    private:
        ImageResourceAccess();    // never implemented

    public:
        /** determines whether the given URL denotes an image within a resource
        */
         static  bool        isImageResourceURL( const ::rtl::OUString& _rURL );

        /** for a given URL of an image within a resource, this method retrieves
            an SvStream for this image.

            This method works for arbitrary URLs denoting an image, since the
            <type scope="com::sun::star::graphics">GraphicsProvider</type> service is used
            to resolve the URL. However, obtaining the stream is expensive (since
            the image must be copied), so you are strongly encouraged to only use it
            when you know that the image is small enough.
        */
         static  SvStream*   getImageStream(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB,
                    const ::rtl::OUString& _rImageResourceURL
                );
    };

//........................................................................
}
//........................................................................

#endif // DBA14_SVTOOLS_INC_IMAGERESOURCEACCESS_HXX

