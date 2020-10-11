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



#ifndef INCLUDED_SVTOOLS_ACCELERATOREXECUTE_HXX
#define INCLUDED_SVTOOLS_ACCELERATOREXECUTE_HXX

//===============================================
// includes

#include "svtools/svtdllapi.h"

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

#ifndef __COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef __COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

#ifndef __COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif

#ifndef __com_SUN_STAR_UI_XACCELERATORCONFIGURATION_HPP_
#include <com/sun/star/ui/XAcceleratorConfiguration.hpp>
#endif

#ifndef __COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif
#include <com/sun/star/util/URL.hpp>

#ifndef __COM_SUN_STAR_AWT_KEYEVENT_HPP_
#include <com/sun/star/awt/KeyEvent.hpp>
#endif
#include <vcl/keycod.hxx>
#include <vcl/evntpost.hxx>
#include <osl/mutex.h>

//===============================================
// namespace

#ifdef css
    #error "Conflict on using css as namespace alias!"
#else
    #define css ::com::sun::star
#endif

namespace svt
{

//===============================================
// definitions

struct TMutexInit
{
    ::osl::Mutex m_aLock;
};    
    
//===============================================
/**
    @descr  implements a helper, which can be used to
            convert vcl key codes into awt key codes ...
            and reverse.
            
            Further such key code can be triggered.
            Doing so different accelerator
            configurations are merged together; a suitable
            command registered for the given key code is searched
            and will be dispatched.

    @attention

            Because exceution of an accelerator command can be dangerous
            (in case it force an office shutdown for key "ALT+F4"!)
            all internal dispatches are done asynchronous.
            Menas that the trigger call doesn't wait till the dispatch
            is finished. You can call very often. All requests will be
            queued internal and dispatched ASAP.

            Of course this queue will be stopped if the environment
            will be destructed ...
 */
class SVT_DLLPUBLIC AcceleratorExecute : private TMutexInit
{
    //-------------------------------------------
    // const, types
    private:

        /** @deprecated
                replaced by internal class AsyncAccelExec ...
                remove this resource here if we go forwards to next major */
        typedef ::std::vector< ::std::pair< css::util::URL, css::uno::Reference< css::frame::XDispatch > > > TCommandQueue;

    //-------------------------------------------
    // member
    private:

        /** TODO document me */
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xSMGR;

        /** TODO document me */
        css::uno::Reference< css::util::XURLTransformer > m_xURLParser;

        /** TODO document me */
        css::uno::Reference< css::frame::XDispatchProvider > m_xDispatcher;

        /** TODO document me */
        css::uno::Reference< css::ui::XAcceleratorConfiguration > m_xGlobalCfg;
        css::uno::Reference< css::ui::XAcceleratorConfiguration > m_xModuleCfg;
        css::uno::Reference< css::ui::XAcceleratorConfiguration > m_xDocCfg;

        /** @deprecated
                replaced by internal class AsyncAccelExec ...
                remove this resource here if we go forwards to next major */
        TCommandQueue m_lCommandQueue;

        /** @deprecated
                replaced by internal class AsyncAccelExec ...
                remove this resource here if we go forwards to next major */
        ::vcl::EventPoster m_aAsyncCallback;

    //-------------------------------------------
    // interface
    public:

        //---------------------------------------
        /** @short  factory method to create new accelerator
                    helper instance.

            @descr  Such helper instance must be initialized at first.
                    So it can know its environment (global/module or
                    document specific).

                    Afterwards it can be used to execute incoming
                    accelerator requests.

                    The "end of life" of such helper can be reached as follow:

                    - delete the object
                      => If it stands currently in its execute method, they will
                         be finished. All further queued requests will be removed
                         and further not executed!

                    - "let it stay alone"
                      => All currently queued events will be finished. The
                         helper kills itself afterwards. A shutdown of the
                         environment will be recognized ... The helper stop its
                         work immediately then!
         */
        static AcceleratorExecute* createAcceleratorHelper();

        //---------------------------------------
        /** @short  fight against inlining ... */
        virtual ~AcceleratorExecute();

        //---------------------------------------
        /** @short  init this instance.

            @descr  It must be called as first method after creation.
                    And further it can be called more than once ...
                    but at least it should be used one time only.
                    Otherwise nobody can say, which asynchronous
                    executions will be used inside the old and which one
                    will be used inside the new environment.

            @param  xSMGR
                    reference to an uno service manager.

            @param  xEnv
                    if it points to a valid frame it will be used
                    to execute the dispatch there. Further the frame
                    is used to locate the right module configuration
                    and use it merged together with the document and
                    the global configuration.

                    If this parameter is set to NULL, the global configuration
                    is used only. Further the global Desktop instance is
                    used for dispatch.
         */
        virtual void init(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR,
                          const css::uno::Reference< css::frame::XFrame >&              xEnv );

        //---------------------------------------
        /** @short  trigger this accelerator.

            @descr  The internal configuartions are used to find
                    as suitable command for this key code.
                    This command will be queued and executed later
                    asynchronous.

            @param  aKey
                    specify the accelerator for execute.

            @return [sal_Bool]
                    sal_True if this key is configured and match to a command.
                    Attention: This state does not mean the success state
                    of the corresponding execute. Because its done asynchronous!
         */
        virtual sal_Bool execute(const KeyCode&            aKey);
        virtual sal_Bool execute(const css::awt::KeyEvent& aKey);

        /** search the command for the given key event.
        *
        * @param aKey The key event
        * @return The command or an empty string if the key event could not be found.
        */
        ::rtl::OUString  findCommand(const ::com::sun::star::awt::KeyEvent& aKey);
        //---------------------------------------
        /** TODO document me */
        static css::awt::KeyEvent st_VCLKey2AWTKey(const KeyCode&            aKey);
        static KeyCode            st_AWTKey2VCLKey(const css::awt::KeyEvent& aKey);

        //---------------------------------------
        /** TODO document me */
        static css::uno::Reference< css::ui::XAcceleratorConfiguration > st_openGlobalConfig(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR);

        //---------------------------------------
        /** TODO document me */
        static css::uno::Reference< css::ui::XAcceleratorConfiguration > st_openModuleConfig(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR ,
                                                                                              const css::uno::Reference< css::frame::XFrame >&              xFrame);

        //---------------------------------------
        /** TODO document me */
        static css::uno::Reference< css::ui::XAcceleratorConfiguration > st_openDocConfig(const css::uno::Reference< css::frame::XModel >& xModel);

    //-------------------------------------------
    // internal
    private:

        //---------------------------------------
        /** @short  allow creation of instances of this class
                    by using our factory only!
         */
        SVT_DLLPRIVATE AcceleratorExecute();

        AcceleratorExecute(const AcceleratorExecute& rCopy);
        void operator=(const AcceleratorExecute&) {};
        //---------------------------------------
        /** TODO document me */
        SVT_DLLPRIVATE ::rtl::OUString impl_ts_findCommand(const css::awt::KeyEvent& aKey);

        //---------------------------------------
        /** TODO document me */
        SVT_DLLPRIVATE css::uno::Reference< css::util::XURLTransformer > impl_ts_getURLParser();

        //---------------------------------------
        /** @deprecated
                replaced by internal class AsyncAccelExec ...
                remove this resource here if we go forwards to next major */
        DECL_DLLPRIVATE_LINK(impl_ts_asyncCallback, void*);
};

} // namespace svt

#undef css

#endif // INCLUDED_SVTOOLS_ACCELERATOREXECUTE_HXX
