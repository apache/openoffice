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

#include "precompiled_svx.hxx"
#include <com/sun/star/ui/ContextChangeEventObject.hpp>
#include <com/sun/star/ui/XContextChangeEventMultiplexer.hpp>
#include <com/sun/star/ui/ContextChangeEventMultiplexer.hpp>
#include <comphelper/processfactory.hxx>
#include "svx/sidebar/ContextChangeEventMultiplexer.hxx"
#include <boost/scoped_ptr.hpp>


using namespace css;
using namespace cssu;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

const ::rtl::OUString ContextChangeEventMultiplexer::Application_Impress (A2S("impress"));

const ::rtl::OUString ContextChangeEventMultiplexer::Context_TextEdit (A2S("text-edit"));
const ::rtl::OUString ContextChangeEventMultiplexer::Context_Default (A2S("default"));


void ContextChangeEventMultiplexer::NotifyContextChange (
    const cssu::Reference<css::frame::XController>& rxController,
    const ::rtl::OUString& rsApplicationName,
    const ::rtl::OUString& rsContextName)
{
    const css::ui::ContextChangeEventObject aEvent(
            rxController,
            rsApplicationName,
            rsContextName);
    
    cssu::Reference<css::ui::XContextChangeEventMultiplexer> xMultiplexer (
        css::ui::ContextChangeEventMultiplexer::get(
            ::comphelper::getProcessComponentContext()));
    if (xMultiplexer.is())
        xMultiplexer->broadcastContextChangeEvent(aEvent, rxController);
}
