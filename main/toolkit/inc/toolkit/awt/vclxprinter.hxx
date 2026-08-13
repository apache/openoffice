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



#ifndef _TOOLKIT_AWT_VCLXPRINTER_HXX_
#define _TOOLKIT_AWT_VCLXPRINTER_HXX_


#include <com/sun/star/awt/XPrinterPropertySet.hpp>
#include <com/sun/star/awt/XPrinter.hpp>
#include <com/sun/star/awt/XPrinterServer.hpp>
#include <com/sun/star/awt/XInfoPrinter.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <cppuhelper/weak.hxx>
#include <osl/mutex.hxx>

#include <toolkit/helper/mutexandbroadcasthelper.hxx>
#include <cppuhelper/propshlp.hxx>
#include <cppuhelper/implbase1.hxx>
#include <comphelper/uno3.hxx>

#include "vcl/oldprintadaptor.hxx"

// Fuer den Drucker relevante Properties:
/*
     sal_Bool    Horizontal
     sal_uInt16    CopyCount;
     sal_Bool    Collate;
     String    FormDescriptor;
     sal_uInt16    Orientation;    // PORTRAIT, LANDSCAPE
*/

//    ----------------------------------------------------
//    class VCLXPrinterPropertySet
//    ----------------------------------------------------

typedef ::cppu::WeakImplHelper1 <   ::com::sun::star::awt::XPrinterPropertySet
                                >   VCLXPrinterPropertySet_Base;
class VCLXPrinterPropertySet    :public VCLXPrinterPropertySet_Base
                                ,public MutexAndBroadcastHelper
                                ,public ::cppu::OPropertySetHelper
{
protected:
    boost::shared_ptr<Printer>                      mpPrinter;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XDevice >    mxPrnDevice;

    sal_Int16                    mnOrientation;
    sal_Bool                    mbHorizontal;
public:
    VCLXPrinterPropertySet( const String& rPrinterName );
    virtual ~VCLXPrinterPropertySet();

    Printer*                    GetPrinter() const { return mpPrinter.get(); }
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XDevice >    GetDevice();

    // ::com::sun::star::uno::XInterface
    DECLARE_XINTERFACE();

    // ::com::sun::star::lang::XTypeProvider
    DECLARE_XTYPEPROVIDER();

    // ::com::sun::star::beans::XPropertySet
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    void SAL_CALL setPropertyValue( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Any& aValue ) { OPropertySetHelper::setPropertyValue( rPropertyName, aValue ); }
    ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& rPropertyName ) { return OPropertySetHelper::getPropertyValue( rPropertyName ); }
    void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& rxListener ) { OPropertySetHelper::addPropertyChangeListener( rPropertyName, rxListener ); }
    void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& rxListener ) { OPropertySetHelper::removePropertyChangeListener( rPropertyName, rxListener ); }
    void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& rxListener ) { OPropertySetHelper::addVetoableChangeListener( rPropertyName, rxListener ); }
    void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& rxListener ) { OPropertySetHelper::removeVetoableChangeListener( rPropertyName, rxListener ); }

    // ::cppu::OPropertySetHelper
    ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();
    sal_Bool SAL_CALL convertFastPropertyValue( ::com::sun::star::uno::Any & rConvertedValue, ::com::sun::star::uno::Any & rOldValue, sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue );
    void SAL_CALL setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue );
    using cppu::OPropertySetHelper::getFastPropertyValue;
    void SAL_CALL getFastPropertyValue( ::com::sun::star::uno::Any& rValue, sal_Int32 nHandle ) const;

    // ::com::sun::star::awt::XPrinterPropertySet
    void SAL_CALL setHorizontal( sal_Bool bHorizontal );
    ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getFormDescriptions(  );
    void SAL_CALL selectForm( const ::rtl::OUString& aFormDescription );
    ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getBinarySetup(  );
    void SAL_CALL setBinarySetup( const ::com::sun::star::uno::Sequence< sal_Int8 >& data );
};

//    ----------------------------------------------------
//    class VCLXPrinter
//    ----------------------------------------------------

typedef ::cppu::ImplInheritanceHelper1  <   VCLXPrinterPropertySet
                                        ,   ::com::sun::star::awt::XPrinter
                                        >   VCLXPrinter_Base;
class VCLXPrinter:    public VCLXPrinter_Base
{
    boost::shared_ptr<vcl::OldStylePrintAdaptor>    mpListener;
    JobSetup                                        maInitJobSetup;
public:
    VCLXPrinter( const String& rPrinterName );
    ~VCLXPrinter();

    // ::com::sun::star::beans::XPropertySet
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) { return VCLXPrinterPropertySet::getPropertySetInfo(); }
    void SAL_CALL setPropertyValue( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Any& aValue ) { VCLXPrinterPropertySet::setPropertyValue( rPropertyName, aValue ); }
    ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& rPropertyName ) { return VCLXPrinterPropertySet::getPropertyValue( rPropertyName ); }
    void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& rxListener ) { VCLXPrinterPropertySet::addPropertyChangeListener( rPropertyName, rxListener ); }
    void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& rxListener ) { VCLXPrinterPropertySet::removePropertyChangeListener( rPropertyName, rxListener ); }
    void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& rxListener ) { VCLXPrinterPropertySet::addVetoableChangeListener( rPropertyName, rxListener ); }
    void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& rxListener ) { VCLXPrinterPropertySet::removeVetoableChangeListener( rPropertyName, rxListener ); }

    // ::com::sun::star::awt::XPrinterPropertySet
    void SAL_CALL setHorizontal( sal_Bool bHorizontal ) { VCLXPrinterPropertySet::setHorizontal( bHorizontal ); }
    ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getFormDescriptions(  ) { return VCLXPrinterPropertySet::getFormDescriptions(); }
    void SAL_CALL selectForm( const ::rtl::OUString& aFormDescription ) { VCLXPrinterPropertySet::selectForm( aFormDescription ); }
    ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getBinarySetup(  ) { return VCLXPrinterPropertySet::getBinarySetup(); }
    void SAL_CALL setBinarySetup( const ::com::sun::star::uno::Sequence< sal_Int8 >& data ) { VCLXPrinterPropertySet::setBinarySetup( data ); }

    // ::com::sun::star::awt::XPrinter
    sal_Bool SAL_CALL start( const ::rtl::OUString& nJobName, sal_Int16 nCopies, sal_Bool nCollate );
    void SAL_CALL end(  );
    void SAL_CALL terminate(  );
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XDevice > SAL_CALL startPage(  );
    void SAL_CALL endPage(  );
};

//    ----------------------------------------------------
//    class VCLXInfoPrinter
//    ----------------------------------------------------

typedef ::cppu::ImplInheritanceHelper1  <   VCLXPrinterPropertySet
                                        ,   ::com::sun::star::awt::XInfoPrinter
                                        >   VCLXInfoPrinter_Base;
class VCLXInfoPrinter :  public VCLXInfoPrinter_Base
{
public:
    VCLXInfoPrinter( const String& rPrinterName );
    ~VCLXInfoPrinter();

    // ::com::sun::star::beans::XPropertySet
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) { return VCLXPrinterPropertySet::getPropertySetInfo(); }
    void SAL_CALL setPropertyValue( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Any& aValue ) { VCLXPrinterPropertySet::setPropertyValue( rPropertyName, aValue ); }
    ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& rPropertyName ) { return VCLXPrinterPropertySet::getPropertyValue( rPropertyName ); }
    void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& rxListener ) { VCLXPrinterPropertySet::addPropertyChangeListener( rPropertyName, rxListener ); }
    void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& rxListener ) { VCLXPrinterPropertySet::removePropertyChangeListener( rPropertyName, rxListener ); }
    void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& rxListener ) { VCLXPrinterPropertySet::addVetoableChangeListener( rPropertyName, rxListener ); }
    void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& rxListener ) { VCLXPrinterPropertySet::removeVetoableChangeListener( rPropertyName, rxListener ); }

    // ::com::sun::star::awt::XPrinterPropertySet
    void SAL_CALL setHorizontal( sal_Bool bHorizontal ) { VCLXPrinterPropertySet::setHorizontal( bHorizontal ); }
    ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getFormDescriptions(  ) { return VCLXPrinterPropertySet::getFormDescriptions(); }
    void SAL_CALL selectForm( const ::rtl::OUString& aFormDescription ) { VCLXPrinterPropertySet::selectForm( aFormDescription ); }
    ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getBinarySetup(  ) { return VCLXPrinterPropertySet::getBinarySetup(); }
    void SAL_CALL setBinarySetup( const ::com::sun::star::uno::Sequence< sal_Int8 >& data ) { VCLXPrinterPropertySet::setBinarySetup( data ); }

    // ::com::sun::star::awt::XInfoPrinter
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XDevice > SAL_CALL createDevice(  );
};

//    ----------------------------------------------------
//    class VCLXPrinterServer
//    ----------------------------------------------------

typedef ::cppu::WeakImplHelper1 <   ::com::sun::star::awt::XPrinterServer
                                >   VCLXPrinterServer_Base;
class VCLXPrinterServer : public VCLXPrinterServer_Base
{
public:
    // ::com::sun::star::awt::XPrinterServer
    ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getPrinterNames(  );
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XPrinter > SAL_CALL createPrinter( const ::rtl::OUString& printerName );
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XInfoPrinter > SAL_CALL createInfoPrinter( const ::rtl::OUString& printerName );
};




#endif // _TOOLKIT_AWT_VCLXPRINTER_HXX_
