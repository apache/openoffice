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



#ifndef _TOOLKIT_AWT_VCLXTOPWINDOW_HXX_
#define _TOOLKIT_AWT_VCLXTOPWINDOW_HXX_

#include <com/sun/star/awt/XSystemDependentWindowPeer.hpp>
#include <com/sun/star/awt/XTopWindow2.hpp>
#include <com/sun/star/awt/XMenuBar.hpp>
#include <cppuhelper/weak.hxx>
#include <osl/mutex.hxx>

#include <cppuhelper/implbase1.hxx>

#include <toolkit/awt/vclxcontainer.hxx>

typedef ::cppu::ImplHelper1 <   ::com::sun::star::awt::XTopWindow2
                            >   VCLXTopWindow_XBase;
typedef ::cppu::ImplHelper1 <   ::com::sun::star::awt::XSystemDependentWindowPeer
                            >   VCLXTopWindow_SBase;

class TOOLKIT_DLLPUBLIC VCLXTopWindow_Base  :public VCLXTopWindow_XBase
                                            ,public VCLXTopWindow_SBase
{
private:
    const bool  m_bWHWND;

protected:
  	::com::sun::star::uno::Reference< ::com::sun::star::awt::XMenuBar> mxMenuBar;

    bool isSystemDependentWindowPeer() const { return m_bWHWND; }

	virtual ::vos::IMutex& GetMutexImpl() = 0;
    virtual Window* GetWindowImpl() = 0;
	virtual ::cppu::OInterfaceContainerHelper& GetTopWindowListenersImpl() = 0;

    VCLXTopWindow_Base( const bool _bSupportSystemWindowPeer );

public:
    virtual ~VCLXTopWindow_Base();

    // XInterface equivalents
    ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
    // XTypeProvider equivalents
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();

    // ::com::sun::star::awt::XSystemDependentWindowPeer
    ::com::sun::star::uno::Any SAL_CALL getWindowHandle( const ::com::sun::star::uno::Sequence< sal_Int8 >& ProcessId, sal_Int16 SystemType );

    // ::com::sun::star::awt::XTopWindow
    void SAL_CALL addTopWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTopWindowListener >& rxListener );
    void SAL_CALL removeTopWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTopWindowListener >& rxListener );
    void SAL_CALL toFront();
    void SAL_CALL toBack();
    void SAL_CALL setMenuBar( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMenuBar >& xMenu );

    // XTopWindow2
    virtual ::sal_Bool SAL_CALL getIsMaximized();
    virtual void SAL_CALL setIsMaximized( ::sal_Bool _ismaximized );
    virtual ::sal_Bool SAL_CALL getIsMinimized();
    virtual void SAL_CALL setIsMinimized( ::sal_Bool _isminimized );
    virtual ::sal_Int32 SAL_CALL getDisplay();
    virtual void SAL_CALL setDisplay( ::sal_Int32 _display );
};

//	----------------------------------------------------
//	class VCLXTopWindow
//	----------------------------------------------------

class TOOLKIT_DLLPUBLIC VCLXTopWindow: public VCLXTopWindow_Base,
                     public VCLXContainer
{
protected:
    virtual vos::IMutex& GetMutexImpl();
    virtual Window* GetWindowImpl();
    virtual ::cppu::OInterfaceContainerHelper& GetTopWindowListenersImpl();

public:
	VCLXTopWindow(bool bWHWND = false);
	~VCLXTopWindow();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any	SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void						SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void						SAL_CALL release() throw()	{ OWeakObject::release(); }

	// ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};




#endif // _TOOLKIT_AWT_VCLXTOPWINDOW_HXX_
