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


#ifndef _UIVWIMP_HXX
#define _UIVWIMP_HXX

#ifndef _SWVIEW_HXX //autogen
#include <view.hxx>
#endif

#include <bf_sfx2/objsh.hxx>

#ifndef _COM_SUN_STAR_VIEW_XSELECTIONSUPPLIER_HPP_
#include <com/sun/star/view/XSelectionSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_DATATRANSFER_CLIPBOARD_XCLIPBOARDLISTENER_HPP_
#include <com/sun/star/datatransfer/clipboard/XClipboardListener.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>	// helper for implementations
#endif
#ifndef _SWUNODEF_HXX
#include <swunodef.hxx>
#endif
#ifndef _CPPUHELPER_WEAKREF_HXX_ 
#include <cppuhelper/weakref.hxx>
#endif
namespace com{ namespace sun{ namespace star {
	namespace frame {
		class XDispatchProviderInterceptor;
	}
    namespace lang {
        class XUnoTunnel;
    }
}}}
namespace binfilter {

class SwXTextView;
class SfxRequest;

class SwView_Impl
{
	STAR_REFERENCE( frame::XDispatchProviderInterceptor )	xDisProvInterceptor;
	STAR_REFERENCE( view::XSelectionSupplier ) 				*pxXTextView;		// UNO object
    ::com::sun::star::uno::WeakReference< ::com::sun::star::lang::XUnoTunnel > xTransferable;
    
    SfxObjectShellRef           xTmpSelDocSh;
    SvEmbeddedObjectRef         aEmbeddedObjRef;
    
    SwView* pView;
	ShellModes 					eShellMode;

public:
	SwView_Impl(SwView* pShell);
	~SwView_Impl();

	void							SetShellMode(ShellModes eSet);

	::com::sun::star::view::XSelectionSupplier* GetUNOObject();
	SwXTextView*					GetUNOObject_Impl();
    void                            Invalidate();

	ShellModes						GetShellMode() {return eShellMode;}


	void 							AddClipboardListener();

    SfxObjectShellRef &             GetTmpSelectionDoc()    { return xTmpSelDocSh; }
    SvEmbeddedObjectRef &           GetEmbeddedObjRef()     { return (SvEmbeddedObjectRef&)(long&)aEmbeddedObjRef; }

};
} //namespace binfilter
#endif

