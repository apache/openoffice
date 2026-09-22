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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svtools.hxx"

#include <com/sun/star/document/XLinkAuthorizer.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include <ucbhelper/contentbroker.hxx>

#include <svtools/linkpolicy.hxx>

using namespace ::com::sun::star;

namespace svt { namespace linkpolicy {

bool mayFollowDocumentLink( ::rtl::OUString const & rUrl )
{
    if ( rUrl.getLength() == 0 )
        return false;

    // These schemes reach process state or the object model; no permission
    // makes them a sensible target for document content.
    if ( isVendorUrl( rUrl ) )
        return false;

    /* Ask the current document, which needs a ServiceManager to reach.
       Because utl::UcbStreamHelper relies on the ::ucbhelper::ContentBroker
       instance, use its ServiceManager. */
    ::ucbhelper::ContentBroker* pBroker = ::ucbhelper::ContentBroker::get();
    if ( pBroker )
    {
        uno::Reference< lang::XMultiServiceFactory > xFactory =
            pBroker->getServiceManager();
        if ( xFactory.is() )
        {
            uno::Any desktop( xFactory->createInstance(
                ::rtl::OUString::createFromAscii( "com.sun.star.frame.Desktop" ) ) );
            uno::Reference< frame::XDesktop > xDesktop( desktop, uno::UNO_QUERY );
            if ( xDesktop.is() )
            {
                uno::Reference< frame::XFrame > xFrame( xDesktop->getCurrentFrame() );
                if ( xFrame.is() )
                {
                    uno::Reference< frame::XController > xController(
                        xFrame->getController() );
                    if ( xController.is() )
                    {
                        uno::Reference< frame::XModel > xModel(
                            xController->getModel() );
                        if ( xModel.is() )
                        {
                            uno::Reference< document::XLinkAuthorizer >
                                xLinkAuthorizer( xModel, uno::UNO_QUERY );
                            if ( xLinkAuthorizer.is() )
                                return xLinkAuthorizer->authorizeLinks( rUrl );
                        }
                    }
                }
            }
        }
    }

    // No document to ask: behave as the linked-graphic loader always has.
    return true;
}

} }
