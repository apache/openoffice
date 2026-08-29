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




#include <com/sun/star/awt/grid/XGridColumn.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/style/HorizontalAlignment.hpp>

#include <cppuhelper/basemutex.hxx>
#include <cppuhelper/compbase3.hxx>
#include <comphelper/componentguard.hxx>
#include <rtl/ref.hxx>
#include <toolkit/helper/mutexandbroadcasthelper.hxx>

#include <vector>

namespace toolkit
{

typedef ::cppu::WeakComponentImplHelper3    <   ::com::sun::star::awt::grid::XGridColumn
                                            ,   ::com::sun::star::lang::XServiceInfo
                                            ,   ::com::sun::star::lang::XUnoTunnel
                                            >   GridColumn_Base;
class GridColumn    :public ::cppu::BaseMutex
                    ,public GridColumn_Base
{
public:
	GridColumn();
    GridColumn( GridColumn const & i_copySource );
	virtual ~GridColumn();

	// ::com::sun::star::awt::grid::XGridColumn
	virtual ::com::sun::star::uno::Any SAL_CALL getIdentifier();
	virtual void SAL_CALL setIdentifier(const ::com::sun::star::uno::Any & value);
	virtual ::sal_Int32 SAL_CALL getColumnWidth();
	virtual void SAL_CALL setColumnWidth(::sal_Int32 the_value);
	virtual ::sal_Int32 SAL_CALL getMaxWidth();
	virtual void SAL_CALL setMaxWidth(::sal_Int32 the_value);
	virtual ::sal_Int32 SAL_CALL getMinWidth();
	virtual void SAL_CALL setMinWidth(::sal_Int32 the_value);
	virtual ::sal_Bool SAL_CALL getResizeable();
	virtual void SAL_CALL setResizeable(::sal_Bool the_value);
    virtual ::sal_Int32 SAL_CALL getFlexibility();
    virtual void SAL_CALL setFlexibility( ::sal_Int32 _flexibility );
	virtual ::rtl::OUString SAL_CALL getTitle();
	virtual void SAL_CALL setTitle(const ::rtl::OUString & value);
	virtual ::rtl::OUString SAL_CALL getHelpText();
	virtual void SAL_CALL setHelpText(const ::rtl::OUString & value);
    virtual ::sal_Int32 SAL_CALL getIndex();
    virtual ::sal_Int32 SAL_CALL getDataColumnIndex();
    virtual void SAL_CALL setDataColumnIndex( ::sal_Int32 i_dataColumnIndex );
	virtual ::com::sun::star::style::HorizontalAlignment SAL_CALL getHorizontalAlign();
	virtual void SAL_CALL setHorizontalAlign(::com::sun::star::style::HorizontalAlignment align);
    virtual void SAL_CALL addGridColumnListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::grid::XGridColumnListener >& xListener );
	virtual void SAL_CALL removeGridColumnListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::grid::XGridColumnListener >& xListener );

    // OComponentHelper
    virtual void SAL_CALL disposing();

    // XCloneable (base of XGridColumn)
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone(  );

    // XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(  );
	virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    // XUnoTunnel and friends
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& i_identifier );
	static ::com::sun::star::uno::Sequence< sal_Int8 > getUnoTunnelId() throw();
	static GridColumn* getImplementation( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& i_component );

    // attribute access
    void setIndex( sal_Int32 const i_index );

private:
    void broadcast_changed(
            sal_Char const * const i_asciiAttributeName,
            ::com::sun::star::uno::Any i_oldValue,
            ::com::sun::star::uno::Any i_newValue,
            ::comphelper::ComponentGuard& i_Guard
        );

    template< class TYPE >
    void impl_set( TYPE & io_attribute, TYPE const & i_newValue, sal_Char const * i_attributeName )
    {
        ::comphelper::ComponentGuard aGuard( *this, rBHelper );
        if ( io_attribute == i_newValue )
            return;

        TYPE const aOldValue( io_attribute );
	    io_attribute = i_newValue;
        broadcast_changed( i_attributeName, ::com::sun::star::uno::makeAny( aOldValue ), ::com::sun::star::uno::makeAny( io_attribute ), aGuard );
    }

    ::com::sun::star::uno::Any                      m_aIdentifier;
	sal_Int32                                       m_nIndex;
    sal_Int32                                       m_nDataColumnIndex;
	sal_Int32                                       m_nColumnWidth;
	sal_Int32                                       m_nMaxWidth;
	sal_Int32                                       m_nMinWidth;
    sal_Int32                                       m_nFlexibility;
	sal_Bool                                        m_bResizeable;
	::rtl::OUString                                 m_sTitle;
	::rtl::OUString                                 m_sHelpText;
	::com::sun::star::style::HorizontalAlignment    m_eHorizontalAlign;
};

}
