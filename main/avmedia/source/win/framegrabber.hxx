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



#ifndef _FRAMEGRABBER_HXX
#define _FRAMEGRABBER_HXX

#include "wincommon.hxx"

#ifndef _COM_SUN_STAR_MEDIA_XFRAMEGRABBER_HDL_
#include "com/sun/star/media/XFrameGrabber.hdl"
#endif

struct IMediaDet;

namespace avmedia { namespace win {

// ----------------
// - FrameGrabber -
// ----------------

class FrameGrabber : public ::cppu::WeakImplHelper2 < ::com::sun::star::media::XFrameGrabber,
                                                      ::com::sun::star::lang::XServiceInfo >
{
public:

            FrameGrabber( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext );
            ~FrameGrabber();

    bool    create( const ::rtl::OUString& rURL );

    // XFrameGrabber
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > SAL_CALL grabFrame( double fMediaTime );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

private:

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >    mxContext;
    ::rtl::OUString                                                                     maURL;

    IMediaDet* implCreateMediaDet( const ::rtl::OUString& rURL ) const;
};

} // namespace win
} // namespace avmedia

#endif // _FRAMEGRABBER_HXX
