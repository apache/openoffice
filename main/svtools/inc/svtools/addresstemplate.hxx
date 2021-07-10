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



#ifndef _SVT_ADDRESSTEMPLATE_HXX_
#define _SVT_ADDRESSTEMPLATE_HXX_

#include "svtools/svtdllapi.h"
#include <vcl/dialog.hxx>
#include <vcl/group.hxx>
#include <vcl/fixed.hxx>
#include <vcl/combobox.hxx>
#include <vcl/button.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/scrbar.hxx>
#include <com/sun/star/container/XNameAccess.hpp>
#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#include <com/sun/star/util/AliasProgrammaticPair.hpp>
#include <com/sun/star/sdbc/XDataSource.hpp>
#include <unotools/configitem.hxx>

// .......................................................................
namespace svt
{
// .......................................................................

	// ===================================================================
	// = AddressBookSourceDialog
	// ===================================================================
	struct AddressBookSourceDialogData;
	class SVT_DLLPUBLIC AddressBookSourceDialog : public ModalDialog
	{
	protected:
		// Controls
		FixedLine		m_aDatasourceFrame;
		FixedText		m_aDatasourceLabel;
		ComboBox		m_aDatasource;
		PushButton		m_aAdministrateDatasources;
		FixedText		m_aTableLabel;
		ComboBox		m_aTable;

		FixedText		m_aFieldsTitle;
		Window			m_aFieldsFrame;	

		ScrollBar		m_aFieldScroller;
		OKButton		m_aOK;
		CancelButton	m_aCancel;
		HelpButton		m_aHelp;

		// string to display for "no selection"
		const String	m_sNoFieldSelection;

		/// the DatabaseContext for selecting data sources
		::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >
						m_xDatabaseContext;
		// the ORB for creating objects
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
						m_xORB;
		::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >
						m_xCurrentDatasourceTables;

		AddressBookSourceDialogData*
						m_pImpl;

	public:
		AddressBookSourceDialog( Window* _pParent,
			const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB );

		/** if you use this ctor, the dialog
            <ul><li>will not store it's data in the configuration (nor initially retrieve it from there)</li>
		        <li>will not allow to change the data source name</li>
		        <li>will not allow to change the table name</li>
		        <li>will not allow to call the data source administration dialog</li>
            </ul>

            @param _rxORB
                a service factory to use for various UNO related needs
            @param _rxTransientDS
                the data source to obtain connections from
            @param _rDataSourceName
                the to-be name of _rxTransientDS. This is only for displaying this
                name to the user, since the dialog completely works on _rxTransientDS,
                and doesn't allow to change this.
            @param _rTable
                the table name to display. It must refer to a valid table, relative to a connection
                obtained from <arg>_rxTransientDS</arg>
        */
		AddressBookSourceDialog( Window* _pParent,
			const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB,
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource >& _rxTransientDS,
            const ::rtl::OUString& _rDataSourceName,
			const ::rtl::OUString& _rTable,
			const ::com::sun::star::uno::Sequence< ::com::sun::star::util::AliasProgrammaticPair >& _rMapping
		);

		~AddressBookSourceDialog();

		// to be used if the object was constructed for editing a field mapping only
		void		getFieldMapping(
			::com::sun::star::uno::Sequence< ::com::sun::star::util::AliasProgrammaticPair >& _rMapping) const;

	protected:
		void	implConstruct();

		// Window overridables
		virtual long		PreNotify( NotifyEvent& _rNEvt );

		// implementations
		void	implScrollFields(sal_Int32 _nPos, sal_Bool _bAdjustFocus, sal_Bool _bAdjustScrollbar);
		void	implSelectField(ListBox* _pBox, const String& _rText);

		void	initializeListBox(ListBox* _pList);
		void	resetTables();
		void	resetFields();

		// fill in the data sources listbox
		void	initializeDatasources();

		// initialize the dialog from the configuration data
		void	loadConfiguration();

		DECL_LINK(OnFieldScroll, ScrollBar*);
		DECL_LINK(OnFieldSelect, ListBox*);
		DECL_LINK(OnAdministrateDatasources, void*);
		DECL_LINK(OnComboGetFocus, ComboBox*);
		DECL_LINK(OnComboLoseFocus, ComboBox*);
		DECL_LINK(OnComboSelect, ComboBox*);
		DECL_LINK(OnOkClicked, Button*);
		DECL_LINK(OnDelayedInitialize, void*);
	};


// .......................................................................
}	// namespace svt
// .......................................................................

#endif // _SVT_ADDRESSTEMPLATE_HXX_

