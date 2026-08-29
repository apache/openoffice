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



#ifndef _TOOLKIT_AWT_VCLXWINDOWS_HXX_
#define _TOOLKIT_AWT_VCLXWINDOWS_HXX_

#include <toolkit/dllapi.h>

#include <com/sun/star/beans/PropertyValues.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/PropertyState.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XFastPropertySet.hpp>
#include <com/sun/star/beans/XVetoableChangeListener.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/beans/XPropertiesChangeListener.hpp>
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#include <com/sun/star/beans/XPropertyAccess.hpp>
#include <com/sun/star/beans/XPropertyContainer.hpp>
#include <com/sun/star/beans/PropertyStateChangeEvent.hpp>
#include <com/sun/star/beans/PropertyChangeEvent.hpp>
#include <com/sun/star/awt/XFileDialog.hpp>
#include <com/sun/star/awt/XTextComponent.hpp>
#include <com/sun/star/awt/XListBox.hpp>
#include <com/sun/star/awt/XProgressMonitor.hpp>
#include <com/sun/star/awt/TextAlign.hpp>
#include <com/sun/star/awt/XScrollBar.hpp>
#include <com/sun/star/awt/XVclContainerPeer.hpp>
#include <com/sun/star/awt/XTabControllerModel.hpp>
#include <com/sun/star/awt/XMessageBox.hpp>
#include <com/sun/star/awt/XTextEditField.hpp>
#include <com/sun/star/awt/Style.hpp>
#include <com/sun/star/awt/XTimeField.hpp>
#include <com/sun/star/awt/XVclWindowPeer.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/XSpinField.hpp>
#include <com/sun/star/awt/XUnoControlContainer.hpp>
#include <com/sun/star/awt/XTextLayoutConstrains.hpp>
#include <com/sun/star/awt/XNumericField.hpp>
#ifndef _COM_SUN_STAR_AWT_XMetricFIELD_HPP_
#include <com/sun/star/awt/XMetricField.hpp>
#endif
#include <com/sun/star/awt/XButton.hpp>
#include <com/sun/star/awt/XToggleButton.hpp>
#include <com/sun/star/awt/XPointer.hpp>
#include <com/sun/star/awt/XTextArea.hpp>
#include <com/sun/star/awt/XImageButton.hpp>
#include <com/sun/star/awt/XFixedHyperlink.hpp>
#include <com/sun/star/awt/XFixedText.hpp>
#include <com/sun/star/awt/XControlContainer.hpp>
#include <com/sun/star/awt/XDialog2.hpp>
#include <com/sun/star/awt/XRadioButton.hpp>
#include <com/sun/star/awt/XCurrencyField.hpp>
#include <com/sun/star/awt/XPatternField.hpp>
#include <com/sun/star/awt/XDateField.hpp>
#include <com/sun/star/awt/XComboBox.hpp>
#include <com/sun/star/awt/XCheckBox.hpp>
#include <com/sun/star/awt/XItemListListener.hpp>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase2.hxx>

#include "toolkit/awt/vclxwindow.hxx"
#include "toolkit/awt/vclxtopwindow.hxx"
#include <cppuhelper/implbase1.hxx>

#include <vcl/pointr.hxx>
#include <vcl/image.hxx>

class Button;
class CheckBox;
class RadioButton;
class ListBox;
class ScrollBar;
class Edit;
class Menu;
class ComboBox;
class FormatterBase;
class SpinField;
class ToolBox;
class VclSimpleEvent;
class VclMenuEvent;

//	----------------------------------------------------
//	class VCLXGraphicControl
//    deriving from VCLXWindow, drawing the graphic which exists as "Graphic" at the model
//	----------------------------------------------------


class TOOLKIT_DLLPUBLIC VCLXGraphicControl : public VCLXWindow
{
private:
    /// the image we currently display
    Image                       maImage;

protected:
    const Image&    GetImage() const { return maImage; }

protected:
	// ::com::sun::star::awt::XWindow
    void SAL_CALL setPosSize( sal_Int32 X, sal_Int32 Y, sal_Int32 Width, sal_Int32 Height, sal_Int16 Flags );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

protected:
    /** forward our bitmap to our window
        @precond
            our mutex is locked
        @precond
            GetWindow is not <NULL/>
        @see GetBitmap
    */
    virtual void    ImplSetNewImage();

public:
    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }

};

//	----------------------------------------------------
//	class VCLXButton
//	----------------------------------------------------
typedef ::cppu::ImplInheritanceHelper2  <   VCLXGraphicControl
                                        ,   ::com::sun::star::awt::XButton
                                        ,   ::com::sun::star::awt::XToggleButton
                                        >   VCLXButton_Base;
class VCLXButton :public VCLXButton_Base
{
private:
	::rtl::OUString				maActionCommand;
	ActionListenerMultiplexer	maActionListeners;
	ItemListenerMultiplexer     maItemListeners;

protected:
    void            ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > CreateAccessibleContext();

public:
					VCLXButton();
    ~VCLXButton();

	// ::com::sun::star::lang::XComponent
    void SAL_CALL dispose(  );

	// ::com::sun::star::awt::XButton
    void SAL_CALL addActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );
    void SAL_CALL removeActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );
    void SAL_CALL setLabel( const ::rtl::OUString& Label );
    void SAL_CALL setActionCommand( const ::rtl::OUString& Command );

	// ::com::sun::star::awt::XToggleButton
	// ::com::sun::star::awt::XItemEventBroadcaster
    void SAL_CALL addItemListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XItemListener >& l );
    void SAL_CALL removeItemListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XItemListener >& l );

    // ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize(  );
    ::com::sun::star::awt::Size SAL_CALL getPreferredSize(  );
    ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& rNewSize );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXImageControl
//	----------------------------------------------------
class VCLXImageControl : public VCLXGraphicControl
{
public:
					VCLXImageControl();
					~VCLXImageControl();

	// ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize(  );
    ::com::sun::star::awt::Size SAL_CALL getPreferredSize(  );
    ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& rNewSize );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }

protected:
    virtual void    ImplSetNewImage();
};

//	----------------------------------------------------
//	class VCLXCheckBox
//	----------------------------------------------------
class VCLXCheckBox :	public ::com::sun::star::awt::XCheckBox,
						public ::com::sun::star::awt::XButton,
						public VCLXGraphicControl
{
private:
	ActionListenerMultiplexer	maActionListeners;
	::rtl::OUString				maActionCommand;
	ItemListenerMultiplexer     maItemListeners;

protected:
    void    ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > CreateAccessibleContext();

public:
					VCLXCheckBox();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();


	// ::com::sun::star::lang::XComponent
    void SAL_CALL dispose(  );

    // ::com::sun::star::awt::XCheckBox
    void SAL_CALL addItemListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XItemListener >& l );
    void SAL_CALL removeItemListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XItemListener >& l );
    sal_Int16 SAL_CALL getState(  );
    void SAL_CALL setState( sal_Int16 n );
    void SAL_CALL setLabel( const ::rtl::OUString& Label );
    void SAL_CALL enableTriState( sal_Bool b );

	// ::com::sun::star::awt::XButton:
    void SAL_CALL addActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );
    void SAL_CALL removeActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );
    void SAL_CALL setActionCommand( const ::rtl::OUString& Command );

	// ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize(  );
    ::com::sun::star::awt::Size SAL_CALL getPreferredSize(  );
    ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& rNewSize );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXRadioButton
//	----------------------------------------------------
class VCLXRadioButton :	public ::com::sun::star::awt::XRadioButton,
						public ::com::sun::star::awt::XButton,
						public VCLXGraphicControl
{
private:
	ItemListenerMultiplexer     maItemListeners;
	ActionListenerMultiplexer	maActionListeners;
	::rtl::OUString				maActionCommand;

protected:
	void			ImplClickedOrToggled( sal_Bool bToggled );
    void            ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > CreateAccessibleContext();

public:
					VCLXRadioButton();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();

	// ::com::sun::star::lang::XComponent
    void SAL_CALL dispose(  );

	// ::com::sun::star::awt::XRadioButton
    void SAL_CALL addItemListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XItemListener >& l );
    void SAL_CALL removeItemListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XItemListener >& l );
    sal_Bool SAL_CALL getState(  );
    void SAL_CALL setState( sal_Bool b );
    void SAL_CALL setLabel( const ::rtl::OUString& Label );

	// ::com::sun::star::awt::XButton:
    void SAL_CALL addActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );
    void SAL_CALL removeActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );
    void SAL_CALL setActionCommand( const ::rtl::OUString& Command );

	// ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize(  );
    ::com::sun::star::awt::Size SAL_CALL getPreferredSize(  );
    ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& rNewSize );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > getFirstActionListener ();
};

//	----------------------------------------------------
//	class VCLXMessageBox
//	----------------------------------------------------
class VCLXMessageBox :	public ::com::sun::star::awt::XMessageBox,
						public VCLXTopWindow
{
public:
						VCLXMessageBox();
						~VCLXMessageBox();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();


    // ::com::sun::star::awt::XMessageBox
    void SAL_CALL setCaptionText( const ::rtl::OUString& aText );
    ::rtl::OUString SAL_CALL getCaptionText(  );
    void SAL_CALL setMessageText( const ::rtl::OUString& aText );
    ::rtl::OUString SAL_CALL getMessageText(  );
    sal_Int16 SAL_CALL execute(  );

    // ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize();

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};



//	----------------------------------------------------
//	class VCLXDialog
//	----------------------------------------------------
class TOOLKIT_DLLPUBLIC VCLXDialog :	public ::com::sun::star::awt::XDialog2,
					public VCLXTopWindow
{
public:
						VCLXDialog();
						~VCLXDialog();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();

    // ::com::sun::star::awt::XDialog2
    virtual void SAL_CALL endDialog( ::sal_Int32 Result );
    virtual void SAL_CALL setHelpId( const rtl::OUString& Id );

    // ::com::sun::star::awt::XDialog
    void SAL_CALL setTitle( const ::rtl::OUString& Title );
    ::rtl::OUString SAL_CALL getTitle(  );
    sal_Int16 SAL_CALL execute(  );
    void SAL_CALL endExecute(  );

	// ::com::sun::star::awt::XView
    void SAL_CALL draw( sal_Int32 nX, sal_Int32 nY );

	// ::com::sun::star::awt::XDevice,
	::com::sun::star::awt::DeviceInfo SAL_CALL getInfo();

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }

	// ::com::sun::star::awt::XVclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );

};

//	----------------------------------------------------
//	class VCLXTabPage
//	----------------------------------------------------
class VCLXTabPage :	public VCLXContainer
{
public:
						VCLXTabPage();
						~VCLXTabPage();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();

	// ::com::sun::star::awt::XView
    void SAL_CALL draw( sal_Int32 nX, sal_Int32 nY );

	// ::com::sun::star::awt::XDevice,
	::com::sun::star::awt::DeviceInfo SAL_CALL getInfo();

	// ::com::sun::star::awt::XVclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );

};

//  ----------------------------------------------------
//  class VCLXFixedHyperlink
//  ----------------------------------------------------
class TOOLKIT_DLLPUBLIC VCLXFixedHyperlink :
    public ::com::sun::star::awt::XFixedHyperlink,
    public VCLXWindow
{
private:
    ActionListenerMultiplexer   maActionListeners;

protected:
    void                        ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >
                                CreateAccessibleContext();

public:
    VCLXFixedHyperlink();
    ~VCLXFixedHyperlink();

    // ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any                  SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
    void                                        SAL_CALL acquire() throw()  { OWeakObject::acquire(); }
    void                                        SAL_CALL release() throw()  { OWeakObject::release(); }

    // ::com::sun::star::lang::XComponent
    void SAL_CALL dispose(  );

    // ::com::sun::star::lang::XTypeProvider
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >  SAL_CALL getTypes();
    ::com::sun::star::uno::Sequence< sal_Int8 >                     SAL_CALL getImplementationId();

    // ::com::sun::star::awt::XFixedHyperlink
    void SAL_CALL setText( const ::rtl::OUString& Text );
    ::rtl::OUString SAL_CALL getText(  );
    void SAL_CALL setURL( const ::rtl::OUString& URL );
    ::rtl::OUString SAL_CALL getURL(  );
    void SAL_CALL setAlignment( sal_Int16 nAlign );
    sal_Int16 SAL_CALL getAlignment(  );
    void SAL_CALL addActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );
    void SAL_CALL removeActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );

    // ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize(  );
    ::com::sun::star::awt::Size SAL_CALL getPreferredSize(  );
    ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& rNewSize );

    // ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXFixedText
//	----------------------------------------------------
class VCLXFixedText :	public ::com::sun::star::awt::XFixedText,
						public VCLXWindow
{
protected:
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > CreateAccessibleContext();

public:
						VCLXFixedText();
						~VCLXFixedText();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();

	// ::com::sun::star::awt::XFixedText
    void SAL_CALL setText( const ::rtl::OUString& Text );
    ::rtl::OUString SAL_CALL getText(  );
    void SAL_CALL setAlignment( sal_Int16 nAlign );
    sal_Int16 SAL_CALL getAlignment(  );

	// ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize(  );
    ::com::sun::star::awt::Size SAL_CALL getPreferredSize(  );
    ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& rNewSize );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXScrollBar
//	----------------------------------------------------
class VCLXScrollBar :	public ::com::sun::star::awt::XScrollBar,
						public VCLXWindow
{
private:
	AdjustmentListenerMultiplexer maAdjustmentListeners;

protected:
    void            ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > CreateAccessibleContext();

public:
					VCLXScrollBar();
	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();


	// ::com::sun::star::lang::XComponent
    void SAL_CALL dispose(  );

	// ::com::sun::star::awt::XScrollbar
    void SAL_CALL addAdjustmentListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XAdjustmentListener >& l );
    void SAL_CALL removeAdjustmentListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XAdjustmentListener >& l );
    void SAL_CALL setValue( sal_Int32 n );
    void SAL_CALL setValues( sal_Int32 nValue, sal_Int32 nVisible, sal_Int32 nMax );
    sal_Int32 SAL_CALL getValue(  );
    void SAL_CALL setMaximum( sal_Int32 n );
    sal_Int32 SAL_CALL getMaximum(  );
    void SAL_CALL setLineIncrement( sal_Int32 n );
    sal_Int32 SAL_CALL getLineIncrement(  );
    void SAL_CALL setBlockIncrement( sal_Int32 n );
    sal_Int32 SAL_CALL getBlockIncrement(  );
    void SAL_CALL setVisibleSize( sal_Int32 n );
    sal_Int32 SAL_CALL getVisibleSize(  );
    void SAL_CALL setOrientation( sal_Int32 n );
    sal_Int32 SAL_CALL getOrientation(  );

    // why isn't this part of the XScrollbar?
    void SAL_CALL setMinimum( sal_Int32 n );
    sal_Int32 SAL_CALL getMinimum(  );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    // ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize();
    static ::com::sun::star::awt::Size SAL_CALL implGetMinimumSize( Window* p );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXEdit
//	----------------------------------------------------
class TOOLKIT_DLLPUBLIC VCLXEdit :	public ::com::sun::star::awt::XTextComponent,
					public ::com::sun::star::awt::XTextEditField,
					public ::com::sun::star::awt::XTextLayoutConstrains,
					public VCLXWindow
{
private:
	TextListenerMultiplexer maTextListeners;

protected:
    void            ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > CreateAccessibleContext();


public:
					VCLXEdit();

	TextListenerMultiplexer&	GetTextListeners() { return maTextListeners; }

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();


	// ::com::sun::star::lang::XComponent
    void SAL_CALL dispose(  );

	// ::com::sun::star::awt::XTextComponent
    void SAL_CALL addTextListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTextListener >& l );
    void SAL_CALL removeTextListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTextListener >& l );
    void SAL_CALL setText( const ::rtl::OUString& aText );
    void SAL_CALL insertText( const ::com::sun::star::awt::Selection& Sel, const ::rtl::OUString& Text );
    ::rtl::OUString SAL_CALL getText(  );
    ::rtl::OUString SAL_CALL getSelectedText(  );
    void SAL_CALL setSelection( const ::com::sun::star::awt::Selection& aSelection );
    ::com::sun::star::awt::Selection SAL_CALL getSelection(  );
    sal_Bool SAL_CALL isEditable(  );
    void SAL_CALL setEditable( sal_Bool bEditable );
    void SAL_CALL setMaxTextLen( sal_Int16 nLen );
    sal_Int16 SAL_CALL getMaxTextLen(  );

	// ::com::sun::star::awt::XTextEditField:
    void SAL_CALL setEchoChar( sal_Unicode cEcho );

	// ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize(  );
    ::com::sun::star::awt::Size SAL_CALL getPreferredSize(  );
    ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& rNewSize );

	// ::com::sun::star::awt::XTextLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize( sal_Int16 nCols, sal_Int16 nLines );
    void SAL_CALL getColumnsAndLines( sal_Int16& nCols, sal_Int16& nLines );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXListBox
//	----------------------------------------------------
typedef ::cppu::ImplInheritanceHelper3  <   VCLXWindow
                                        ,   ::com::sun::star::awt::XListBox
                                        ,   ::com::sun::star::awt::XTextLayoutConstrains
                                        ,   ::com::sun::star::awt::XItemListListener
                                        >   VCLXListBox_Base;
class VCLXListBox  : public VCLXListBox_Base
{
private:
	ActionListenerMultiplexer	maActionListeners;
	ItemListenerMultiplexer		maItemListeners;

protected:
    virtual void	ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >
					CreateAccessibleContext();
    void            ImplCallItemListeners();

public:
						VCLXListBox();

	// ::com::sun::star::lang::XComponent
    void SAL_CALL dispose(  );

    // ::com::sun::star::awt::XListBox
    void SAL_CALL addItemListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XItemListener >& l );
    void SAL_CALL removeItemListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XItemListener >& l );
    void SAL_CALL addActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );
    void SAL_CALL removeActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );
    void SAL_CALL addItem( const ::rtl::OUString& aItem, sal_Int16 nPos );
    void SAL_CALL addItems( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aItems, sal_Int16 nPos );
    void SAL_CALL removeItems( sal_Int16 nPos, sal_Int16 nCount );
    sal_Int16 SAL_CALL getItemCount(  );
    ::rtl::OUString SAL_CALL getItem( sal_Int16 nPos );
    ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getItems(  );
    sal_Int16 SAL_CALL getSelectedItemPos(  );
    ::com::sun::star::uno::Sequence< sal_Int16 > SAL_CALL getSelectedItemsPos(  );
    ::rtl::OUString SAL_CALL getSelectedItem(  );
    ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSelectedItems(  );
    void SAL_CALL selectItemPos( sal_Int16 nPos, sal_Bool bSelect );
    void SAL_CALL selectItemsPos( const ::com::sun::star::uno::Sequence< sal_Int16 >& aPositions, sal_Bool bSelect );
    void SAL_CALL selectItem( const ::rtl::OUString& aItem, sal_Bool bSelect );
    sal_Bool SAL_CALL isMutipleMode(  );
    void SAL_CALL setMultipleMode( sal_Bool bMulti );
    sal_Int16 SAL_CALL getDropDownLineCount(  );
    void SAL_CALL setDropDownLineCount( sal_Int16 nLines );
    void SAL_CALL makeVisible( sal_Int16 nEntry );

	// ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize(  );
    ::com::sun::star::awt::Size SAL_CALL getPreferredSize(  );
    ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& rNewSize );

	// ::com::sun::star::awt::XTextLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize( sal_Int16 nCols, sal_Int16 nLines );
    void SAL_CALL getColumnsAndLines( sal_Int16& nCols, sal_Int16& nLines );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    // XItemListListener
    virtual void SAL_CALL listItemInserted( const ::com::sun::star::awt::ItemListEvent& Event );
    virtual void SAL_CALL listItemRemoved( const ::com::sun::star::awt::ItemListEvent& Event );
    virtual void SAL_CALL listItemModified( const ::com::sun::star::awt::ItemListEvent& Event );
    virtual void SAL_CALL allItemsRemoved( const ::com::sun::star::lang::EventObject& Event );
    virtual void SAL_CALL itemListChanged( const ::com::sun::star::lang::EventObject& Event );

    // XEventListener
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& i_rEvent );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXComboBox
//	----------------------------------------------------
typedef ::cppu::ImplInheritanceHelper2  <   VCLXEdit
                                        ,   ::com::sun::star::awt::XComboBox
                                        ,   ::com::sun::star::awt::XItemListListener > VCLXComboBox_Base;
class VCLXComboBox :	public VCLXComboBox_Base
{
private:
	ActionListenerMultiplexer	maActionListeners;
	ItemListenerMultiplexer		maItemListeners;

protected:
    void            ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > CreateAccessibleContext();


public:
						VCLXComboBox();
    ~VCLXComboBox();

 	// ::com::sun::star::lang::XComponent
    void SAL_CALL dispose(  );

	// ::com::sun::star::awt::XComboBox
    void SAL_CALL addItemListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XItemListener >& l );
    void SAL_CALL removeItemListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XItemListener >& l );
    void SAL_CALL addActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );
    void SAL_CALL removeActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& l );
    void SAL_CALL addItem( const ::rtl::OUString& aItem, sal_Int16 nPos );
    void SAL_CALL addItems( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aItems, sal_Int16 nPos );
    void SAL_CALL removeItems( sal_Int16 nPos, sal_Int16 nCount );
    sal_Int16 SAL_CALL getItemCount(  );
    ::rtl::OUString SAL_CALL getItem( sal_Int16 nPos );
    ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getItems(  );
    sal_Int16 SAL_CALL getDropDownLineCount(  );
    void SAL_CALL setDropDownLineCount( sal_Int16 nLines );

	// ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize(  );
    ::com::sun::star::awt::Size SAL_CALL getPreferredSize(  );
    ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& rNewSize );

	// ::com::sun::star::awt::XTextLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize( sal_Int16 nCols, sal_Int16 nLines );
    void SAL_CALL getColumnsAndLines( sal_Int16& nCols, sal_Int16& nLines );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    // XItemListListener
    virtual void SAL_CALL listItemInserted( const ::com::sun::star::awt::ItemListEvent& Event );
    virtual void SAL_CALL listItemRemoved( const ::com::sun::star::awt::ItemListEvent& Event );
    virtual void SAL_CALL listItemModified( const ::com::sun::star::awt::ItemListEvent& Event );
    virtual void SAL_CALL allItemsRemoved( const ::com::sun::star::lang::EventObject& Event );
    virtual void SAL_CALL itemListChanged( const ::com::sun::star::lang::EventObject& Event );
    // XEventListener
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& i_rEvent );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXSpinField
//	----------------------------------------------------
class TOOLKIT_DLLPUBLIC VCLXSpinField :	public ::com::sun::star::awt::XSpinField,
						public VCLXEdit
{
private:
	SpinListenerMultiplexer	maSpinListeners;

protected:
    void            ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );

public:
					VCLXSpinField();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();


	// ::com::sun::star::awt::XSpinField
    void SAL_CALL addSpinListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XSpinListener >& l );
    void SAL_CALL removeSpinListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XSpinListener >& l );
    void SAL_CALL up(  );
    void SAL_CALL down(  );
    void SAL_CALL first(  );
    void SAL_CALL last(  );
    void SAL_CALL enableRepeat( sal_Bool bRepeat );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXFormattedSpinField
//	----------------------------------------------------
class VCLXFormattedSpinField : public VCLXSpinField
{
private:
	FormatterBase*	mpFormatter;

protected:
	FormatterBase*	GetFormatter() const { return GetWindow() ? mpFormatter : NULL; }

public:
					VCLXFormattedSpinField();
					~VCLXFormattedSpinField();

	void			SetFormatter( FormatterBase* pFormatter ) { mpFormatter = pFormatter; }

	void 			setStrictFormat( sal_Bool bStrict );
    sal_Bool 		isStrictFormat();

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXDateField
//	----------------------------------------------------
class TOOLKIT_DLLPUBLIC VCLXDateField :	public ::com::sun::star::awt::XDateField,
						public VCLXFormattedSpinField
{
protected:
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > CreateAccessibleContext();
public:
					VCLXDateField();
					~VCLXDateField();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();


	// ::com::sun::star::awt::XDateField
    void SAL_CALL setDate( sal_Int32 Date );
    sal_Int32 SAL_CALL getDate(  );
    void SAL_CALL setMin( sal_Int32 Date );
    sal_Int32 SAL_CALL getMin(  );
    void SAL_CALL setMax( sal_Int32 Date );
    sal_Int32 SAL_CALL getMax(  );
    void SAL_CALL setFirst( sal_Int32 Date );
    sal_Int32 SAL_CALL getFirst(  );
    void SAL_CALL setLast( sal_Int32 Date );
    sal_Int32 SAL_CALL getLast(  );
    void SAL_CALL setLongFormat( sal_Bool bLong );
    sal_Bool SAL_CALL isLongFormat(  );
    void SAL_CALL setEmpty(  );
    sal_Bool SAL_CALL isEmpty(  );
    void SAL_CALL setStrictFormat( sal_Bool bStrict );
    sal_Bool SAL_CALL isStrictFormat(  );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXTimeField
//	----------------------------------------------------
class VCLXTimeField :	public ::com::sun::star::awt::XTimeField,
						public VCLXFormattedSpinField
{
protected:
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > CreateAccessibleContext();
public:
					VCLXTimeField();
					~VCLXTimeField();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();

	// ::com::sun::star::awt::XTimeField
    void SAL_CALL setTime( sal_Int32 Time );
    sal_Int32 SAL_CALL getTime(  );
    void SAL_CALL setMin( sal_Int32 Time );
    sal_Int32 SAL_CALL getMin(  );
    void SAL_CALL setMax( sal_Int32 Time );
    sal_Int32 SAL_CALL getMax(  );
    void SAL_CALL setFirst( sal_Int32 Time );
    sal_Int32 SAL_CALL getFirst(  );
    void SAL_CALL setLast( sal_Int32 Time );
    sal_Int32 SAL_CALL getLast(  );
    void SAL_CALL setEmpty(  );
    sal_Bool SAL_CALL isEmpty(  );
    void SAL_CALL setStrictFormat( sal_Bool bStrict );
    sal_Bool SAL_CALL isStrictFormat(  );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXNumericField
//	----------------------------------------------------
class VCLXNumericField :	public ::com::sun::star::awt::XNumericField,
							public VCLXFormattedSpinField
{
public:
					VCLXNumericField();
					~VCLXNumericField();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();

	// ::com::sun::star::awt::XNumericField
    void SAL_CALL setValue( double Value );
    double SAL_CALL getValue(  );
    void SAL_CALL setMin( double Value );
    double SAL_CALL getMin(  );
    void SAL_CALL setMax( double Value );
    double SAL_CALL getMax(  );
    void SAL_CALL setFirst( double Value );
    double SAL_CALL getFirst(  );
    void SAL_CALL setLast( double Value );
    double SAL_CALL getLast(  );
    void SAL_CALL setSpinSize( double Value );
    double SAL_CALL getSpinSize(  );
    void SAL_CALL setDecimalDigits( sal_Int16 nDigits );
    sal_Int16 SAL_CALL getDecimalDigits(  );
    void SAL_CALL setStrictFormat( sal_Bool bStrict );
    sal_Bool SAL_CALL isStrictFormat(  );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXMetricField
//	----------------------------------------------------
class MetricFormatter;
class MetricField;
class VCLXMetricField :	public ::com::sun::star::awt::XMetricField,
                        public VCLXFormattedSpinField
{
    MetricFormatter *GetMetricFormatter();
    MetricField     *GetMetricField();
    void CallListeners();
public:
    VCLXMetricField();
    ~VCLXMetricField();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();

	// ::com::sun::star::awt::XMetricField
    virtual void SAL_CALL setValue( ::sal_Int64 Value, ::sal_Int16 Unit );
    virtual void SAL_CALL setUserValue( ::sal_Int64 Value, ::sal_Int16 Unit );
    virtual ::sal_Int64 SAL_CALL getValue( ::sal_Int16 Unit );
    virtual ::sal_Int64 SAL_CALL getCorrectedValue( ::sal_Int16 Unit );
    virtual void SAL_CALL setMin( ::sal_Int64 Value, ::sal_Int16 Unit );
    virtual ::sal_Int64 SAL_CALL getMin( ::sal_Int16 Unit );
    virtual void SAL_CALL setMax( ::sal_Int64 Value, ::sal_Int16 Unit );
    virtual ::sal_Int64 SAL_CALL getMax( ::sal_Int16 Unit );
    virtual void SAL_CALL setFirst( ::sal_Int64 Value, ::sal_Int16 Unit );
    virtual ::sal_Int64 SAL_CALL getFirst( ::sal_Int16 Unit );
    virtual void SAL_CALL setLast( ::sal_Int64 Value, ::sal_Int16 Unit );
    virtual ::sal_Int64 SAL_CALL getLast( ::sal_Int16 Unit );
    virtual void SAL_CALL setSpinSize( ::sal_Int64 Value );
    virtual ::sal_Int64 SAL_CALL getSpinSize(  );
    virtual void SAL_CALL setDecimalDigits( ::sal_Int16 nDigits );
    virtual ::sal_Int16 SAL_CALL getDecimalDigits(  );
    virtual void SAL_CALL setStrictFormat( ::sal_Bool bStrict );
    virtual ::sal_Bool SAL_CALL isStrictFormat(  );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXCurrencyField
//	----------------------------------------------------
class VCLXCurrencyField :	public ::com::sun::star::awt::XCurrencyField,
							public VCLXFormattedSpinField
{
public:
					VCLXCurrencyField();
					~VCLXCurrencyField();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();


	// ::com::sun::star::awt::XCurrencyField
    void SAL_CALL setValue( double Value );
    double SAL_CALL getValue(  );
    void SAL_CALL setMin( double Value );
    double SAL_CALL getMin(  );
    void SAL_CALL setMax( double Value );
    double SAL_CALL getMax(  );
    void SAL_CALL setFirst( double Value );
    double SAL_CALL getFirst(  );
    void SAL_CALL setLast( double Value );
    double SAL_CALL getLast(  );
    void SAL_CALL setSpinSize( double Value );
    double SAL_CALL getSpinSize(  );
    void SAL_CALL setDecimalDigits( sal_Int16 nDigits );
    sal_Int16 SAL_CALL getDecimalDigits(  );
    void SAL_CALL setStrictFormat( sal_Bool bStrict );
    sal_Bool SAL_CALL isStrictFormat(  );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXPatternField
//	----------------------------------------------------
class VCLXPatternField :	public ::com::sun::star::awt::XPatternField,
							public VCLXFormattedSpinField
{
public:
					VCLXPatternField();
					~VCLXPatternField();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();


	// ::com::sun::star::awt::XPatternField
    void SAL_CALL setMasks( const ::rtl::OUString& EditMask, const ::rtl::OUString& LiteralMask );
    void SAL_CALL getMasks( ::rtl::OUString& EditMask, ::rtl::OUString& LiteralMask );
    void SAL_CALL setString( const ::rtl::OUString& Str );
    ::rtl::OUString SAL_CALL getString(  );
    void SAL_CALL setStrictFormat( sal_Bool bStrict );
    sal_Bool SAL_CALL isStrictFormat(  );

	// ::com::sun::star::awt::VclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds ) { return ImplGetPropertyIds( aIds ); }
};

//	----------------------------------------------------
//	class VCLXToolBox
//	----------------------------------------------------
class VCLXToolBox :	public VCLXWindow
{
private:

protected:
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >
						CreateAccessibleContext();

public:
						VCLXToolBox();
						~VCLXToolBox();
};

#endif // _TOOLKIT_AWT_VCLXWINDOWS_HXX_
