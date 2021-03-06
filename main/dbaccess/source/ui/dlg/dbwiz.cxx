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
#include "precompiled_dbui.hxx"

#ifndef DBAUI_DBWIZ_HXX
#include "dbwiz.hxx"
#endif
#ifndef _DBAUI_DBADMIN_HRC_
#include "dbadmin.hrc"
#endif
#ifndef _DBU_DLG_HRC_
#include "dbu_dlg.hrc"
#endif
#ifndef _DBAUI_DATASOURCEITEMS_HXX_
#include "dsitems.hxx"
#endif
#include "dsnItem.hxx"
#ifndef _SFXSTRITEM_HXX
#include <svl/stritem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX
#include <svl/eitem.hxx>
#endif
#ifndef _SFXINTITEM_HXX
#include <svl/intitem.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef DBACCESS_SHARED_DBUSTRINGS_HRC
#include "dbustrings.hrc"
#endif
#ifndef _DBAUI_ADMINPAGES_HXX_
#include "adminpages.hxx"
#endif
#ifndef _DBAUI_GENERALPAGE_HXX_
#include "generalpage.hxx"
#endif
#ifndef _DBAUI_LOCALRESACCESS_HXX_
#include "localresaccess.hxx"
#endif
#ifndef _DBAUI_STRINGLISTITEM_HXX_
#include "stringlistitem.hxx"
#endif
#ifndef _DBAUI_PROPERTYSETITEM_HXX_
#include "propertysetitem.hxx"
#endif
#ifndef _UNOTOOLS_CONFIGNODE_HXX_
#include <unotools/confignode.hxx>
#endif
#ifndef DBAUI_CONNECTIONPAGE_HXX
#include "ConnectionPage.hxx"
#endif
#ifndef DBAUI_DRIVERSETTINGS_HXX
#include "DriverSettings.hxx"
#endif
#ifndef _DBAUI_DBADMINIMPL_HXX_
#include "DbAdminImpl.hxx"
#endif
#ifndef _DBA_DBACCESS_HELPID_HRC_
#include "dbaccess_helpid.hrc"
#endif

//.........................................................................
namespace dbaui
{
//.........................................................................
using namespace svt;
using namespace com::sun::star::uno;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::lang;
using namespace com::sun::star::util;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;

#define START_PAGE			            0
#define CONNECTION_PAGE		            1
#define ADDITIONAL_PAGE_DBASE     		2
#define ADDITIONAL_PAGE_FLAT            3
#define ADDITIONAL_PAGE_LDAP            4
#define ADDITIONAL_PAGE_ADABAS          5
#define ADDITIONAL_PAGE_MYSQL_JDBC      6
#define ADDITIONAL_PAGE_MYSQL_ODBC	    7
#define ADDITIONAL_PAGE_ORACLE_JDBC     8
#define ADDITIONAL_PAGE_ADO             9
#define ADDITIONAL_PAGE_ODBC           10
#define ADDITIONAL_USERDEFINED         11
#define ADDITIONAL_PAGE_MYSQL_NATIVE   12


DBG_NAME(ODbTypeWizDialog)
//=========================================================================
//= ODbTypeWizDialog
//=========================================================================
//-------------------------------------------------------------------------
ODbTypeWizDialog::ODbTypeWizDialog(Window* _pParent
							   ,SfxItemSet* _pItems
							   ,const Reference< XMultiServiceFactory >& _rxORB
							   ,const ::com::sun::star::uno::Any& _aDataSourceName
							   )
	:OWizardMachine(_pParent, ModuleRes(DLG_DATABASE_TYPE_CHANGE), WZB_NEXT | WZB_PREVIOUS | WZB_FINISH | WZB_CANCEL | WZB_HELP )
	,m_pOutSet(NULL)
	,m_bResetting(sal_False)
	,m_bApplied(sal_False)
	,m_bUIEnabled( sal_True )
{
	DBG_CTOR(ODbTypeWizDialog,NULL);	
	m_pImpl = ::std::auto_ptr<ODbDataSourceAdministrationHelper>(new ODbDataSourceAdministrationHelper(_rxORB,this,this));
	m_pImpl->setDataSourceOrName(_aDataSourceName);
	Reference< XPropertySet > xDatasource = m_pImpl->getCurrentDataSource();
	m_pOutSet = new SfxItemSet( *_pItems->GetPool(), _pItems->GetRanges() );

	m_pImpl->translateProperties(xDatasource, *m_pOutSet);
	m_eType = m_pImpl->getDatasourceType(*m_pOutSet);

	SetPageSizePixel(LogicToPixel(::Size(PAGE_X, PAGE_Y), MAP_APPFONT));
	ShowButtonFixedLine(sal_True);
	defaultButton(WZB_NEXT);
	enableButtons(WZB_FINISH, sal_False);
    enableAutomaticNextButtonState( true );

    m_pPrevPage->SetHelpId(HID_DBWIZ_PREVIOUS);
	m_pNextPage->SetHelpId(HID_DBWIZ_NEXT);
	m_pCancel->SetHelpId(HID_DBWIZ_CANCEL);
	m_pFinish->SetHelpId(HID_DBWIZ_FINISH);
	m_pHelp->SetUniqueId(UID_DBWIZ_HELP);
	// no local resources needed anymore

    DbuTypeCollectionItem* pCollectionItem = PTR_CAST(DbuTypeCollectionItem, _pItems->GetItem(DSID_TYPECOLLECTION));
	m_pCollection = pCollectionItem->getCollection();

	FreeResource();
	ActivatePage();
}

//-------------------------------------------------------------------------
ODbTypeWizDialog::~ODbTypeWizDialog()
{
	DBG_DTOR(ODbTypeWizDialog,NULL);	
	delete m_pOutSet;	
}
//-------------------------------------------------------------------------
IMPL_LINK(ODbTypeWizDialog, OnTypeSelected, OGeneralPage*, _pTabPage)
{
    m_eType = _pTabPage->GetSelectedType();
    const bool bURLRequired = m_pCollection->isConnectionUrlRequired(m_eType);
    enableButtons(WZB_NEXT,bURLRequired);
	enableButtons(WZB_FINISH,!bURLRequired);
	return 1L;
}
//-------------------------------------------------------------------------
WizardTypes::WizardState ODbTypeWizDialog::determineNextState( WizardState _nCurrentState ) const
{
	WizardTypes::WizardState nNextState = WZS_INVALID_STATE;
	switch(_nCurrentState)
	{
		case START_PAGE:
            switch(m_pCollection->determineType(m_eType))
			{
				case  ::dbaccess::DST_MOZILLA:
				case  ::dbaccess::DST_OUTLOOK:
				case  ::dbaccess::DST_OUTLOOKEXP:
				case  ::dbaccess::DST_EVOLUTION:
				case  ::dbaccess::DST_EVOLUTION_GROUPWISE:
				case  ::dbaccess::DST_EVOLUTION_LDAP:
				case  ::dbaccess::DST_KAB:
				case  ::dbaccess::DST_MACAB:
					nNextState = WZS_INVALID_STATE;
					break;
				case  ::dbaccess::DST_MYSQL_NATIVE:
                    nNextState = ADDITIONAL_PAGE_MYSQL_NATIVE;
                    break;
                default:
			        nNextState = CONNECTION_PAGE;
                    break;
            }
			break;
		case CONNECTION_PAGE:
			switch(m_pCollection->determineType(m_eType))
			{
                case  ::dbaccess::DST_MOZILLA:
                case  ::dbaccess::DST_THUNDERBIRD:
				case  ::dbaccess::DST_OUTLOOK:
				case  ::dbaccess::DST_OUTLOOKEXP:
				case  ::dbaccess::DST_EVOLUTION:
				case  ::dbaccess::DST_EVOLUTION_GROUPWISE:
				case  ::dbaccess::DST_EVOLUTION_LDAP:
				case  ::dbaccess::DST_KAB:
				case  ::dbaccess::DST_MACAB:
				case  ::dbaccess::DST_MSACCESS:
                case  ::dbaccess::DST_MSACCESS_2007:
				case  ::dbaccess::DST_JDBC:
				case  ::dbaccess::DST_CALC:
					nNextState = WZS_INVALID_STATE;
					break;
                case  ::dbaccess::DST_DBASE:
                    nNextState = ADDITIONAL_PAGE_DBASE;
                    break;
                case  ::dbaccess::DST_FLAT:
                    nNextState = ADDITIONAL_PAGE_FLAT;
                    break;
                case  ::dbaccess::DST_LDAP:
                    nNextState = ADDITIONAL_PAGE_LDAP;
                    break;
                case  ::dbaccess::DST_ADABAS:
                    nNextState = ADDITIONAL_PAGE_ADABAS;
                    break;
                case  ::dbaccess::DST_MYSQL_JDBC:
                    nNextState = ADDITIONAL_PAGE_MYSQL_JDBC;
                    break;
                case  ::dbaccess::DST_MYSQL_ODBC:
                    nNextState = ADDITIONAL_PAGE_MYSQL_ODBC;
                    break;
                case  ::dbaccess::DST_ORACLE_JDBC:
                    nNextState = ADDITIONAL_PAGE_ORACLE_JDBC;
                    break;
                case  ::dbaccess::DST_ADO:
                    nNextState = ADDITIONAL_PAGE_ADO;
                    break;
                case  ::dbaccess::DST_ODBC:
                    nNextState = ADDITIONAL_PAGE_ODBC;
                    break;
				default:
					nNextState = WZS_INVALID_STATE;
					break;
			}
			break;
	}

	return nNextState;
}

// -----------------------------------------------------------------------------
const SfxItemSet* ODbTypeWizDialog::getOutputSet() const
{
	return m_pOutSet;
}
// -----------------------------------------------------------------------------
SfxItemSet* ODbTypeWizDialog::getWriteOutputSet()
{
	return m_pOutSet;
}
// -----------------------------------------------------------------------------
::std::pair< Reference<XConnection>,sal_Bool> ODbTypeWizDialog::createConnection()
{
	return m_pImpl->createConnection();
}
// -----------------------------------------------------------------------------
Reference< XMultiServiceFactory > ODbTypeWizDialog::getORB() const
{
	return m_pImpl->getORB();
}
// -----------------------------------------------------------------------------
Reference< XDriver > ODbTypeWizDialog::getDriver()
{
	return m_pImpl->getDriver();
}
// -----------------------------------------------------------------------------
::rtl::OUString	ODbTypeWizDialog::getDatasourceType(const SfxItemSet& _rSet) const
{
	return m_pImpl->getDatasourceType(_rSet);
}
// -----------------------------------------------------------------------------
void ODbTypeWizDialog::clearPassword()
{
	m_pImpl->clearPassword();
}

// -----------------------------------------------------------------------------
TabPage* ODbTypeWizDialog::createPage(WizardState _nState)
{
	sal_uInt16 nStringId = STR_PAGETITLE_ADVANCED;
	TabPage* pPage = NULL;
	switch(_nState)
	{
		case START_PAGE: // start state
        {
			pPage = OGeneralPage::Create(this,*m_pOutSet);
            OGeneralPage* pGeneralPage = static_cast< OGeneralPage* >( pPage );
			pGeneralPage->SetTypeSelectHandler( LINK( this, ODbTypeWizDialog, OnTypeSelected));
//            pGeneralPage->SetCreationModeHandler( LINK( this, ODbTypeWizDialog, TODO ) );
			nStringId = STR_PAGETITLE_GENERAL;
        }
		break;
		case CONNECTION_PAGE:
			pPage = OConnectionTabPage::Create(this,*m_pOutSet);
			nStringId = STR_PAGETITLE_CONNECTION;
			break;

        case ADDITIONAL_PAGE_DBASE:
			pPage = ODriversSettings::CreateDbase(this,*m_pOutSet);
            break;
        case ADDITIONAL_PAGE_FLAT:
			pPage = ODriversSettings::CreateText(this,*m_pOutSet);
            break;
        case ADDITIONAL_PAGE_LDAP:
			pPage = ODriversSettings::CreateLDAP(this,*m_pOutSet);
            break;
        case ADDITIONAL_PAGE_ADABAS:
            pPage = ODriversSettings::CreateAdabas(this,*m_pOutSet);
            break;
        case ADDITIONAL_PAGE_MYSQL_JDBC:
            pPage = ODriversSettings::CreateMySQLJDBC(this,*m_pOutSet);
            break;
        case ADDITIONAL_PAGE_MYSQL_NATIVE:
            pPage = ODriversSettings::CreateMySQLNATIVE(this,*m_pOutSet);
            break;
        case ADDITIONAL_PAGE_MYSQL_ODBC:
            pPage = ODriversSettings::CreateMySQLODBC(this,*m_pOutSet);
            break;
        case ADDITIONAL_PAGE_ORACLE_JDBC:
            pPage = ODriversSettings::CreateOracleJDBC(this,*m_pOutSet);
            break;
        case ADDITIONAL_PAGE_ADO:
			pPage = ODriversSettings::CreateAdo(this,*m_pOutSet);
            break;
        case ADDITIONAL_PAGE_ODBC:
			pPage = ODriversSettings::CreateODBC(this,*m_pOutSet);
            break;
        case ADDITIONAL_USERDEFINED:
			pPage = ODriversSettings::CreateUser(this,*m_pOutSet);
			break;
		default:
			OSL_ENSURE(0,"Wrong state!");
			break;
	}

	// register ourself as modified listener
	if ( pPage )
	{		
		static_cast<OGenericAdministrationPage*>(pPage)->SetServiceFactory(m_pImpl->getORB());
		static_cast<OGenericAdministrationPage*>(pPage)->SetAdminDialog(this,this);
		// open our own resource block, as the page titles are strings local to this block
		LocalResourceAccess aDummy(DLG_DATABASE_ADMINISTRATION, RSC_TABDIALOG);

		pPage->SetText(String(ModuleRes(nStringId)));
		defaultButton( _nState == START_PAGE ? WZB_NEXT : WZB_FINISH );
		enableButtons( WZB_FINISH, _nState == START_PAGE ? sal_False : sal_True);
		pPage->Show();
	}
	return pPage;
}
// -----------------------------------------------------------------------------
sal_Bool ODbTypeWizDialog::leaveState(WizardState _nState)
{
	SfxTabPage* pPage = static_cast<SfxTabPage*>(WizardDialog::GetPage(_nState));
	if ( pPage )
		pPage->FillItemSet(*m_pOutSet);
	return sal_True;
}
// -----------------------------------------------------------------------------
void ODbTypeWizDialog::setTitle(const ::rtl::OUString& _sTitle)
{
	SetText(_sTitle);
}
//-------------------------------------------------------------------------
void ODbTypeWizDialog::enableConfirmSettings( bool _bEnable )
{
    enableButtons( WZB_FINISH, _bEnable );
    // TODO:
    // this is hacky. At the moment, this method is used in only one case (#b6532894#).
    // As soon as it is to be used more wide-spread, we should find a proper concept
    // for enabling both the Next and Finish buttons, depending on the current page state.
    // Plus, the concept must also care for the case where those pages are embedded into
    // anormal tab dialog.
}
//-------------------------------------------------------------------------
sal_Bool ODbTypeWizDialog::saveDatasource()
{
	SfxTabPage* pPage = static_cast<SfxTabPage*>(WizardDialog::GetPage(getCurrentState()));
	if ( pPage )
		pPage->FillItemSet(*m_pOutSet);

    DataSourceInfoConverter aConverter(getORB());
    ::rtl::OUString sOldURL;
    if ( m_pImpl->getCurrentDataSource().is() )
        m_pImpl->getCurrentDataSource()->getPropertyValue(PROPERTY_URL) >>= sOldURL;
    aConverter.convert(m_pCollection,sOldURL,m_eType,m_pImpl->getCurrentDataSource());
	return sal_True;
}
// -----------------------------------------------------------------------------
IWizardPageController* ODbTypeWizDialog::getPageController( TabPage* _pCurrentPage ) const
{
	OGenericAdministrationPage* pPage = static_cast<OGenericAdministrationPage*>(_pCurrentPage);
	return pPage;
}
// -----------------------------------------------------------------------------
sal_Bool ODbTypeWizDialog::onFinish()
{
	saveDatasource();
	return m_pImpl->saveChanges(*m_pOutSet) ? OWizardMachine::onFinish() : sal_False;
}
//.........................................................................
}	// namespace dbaui
//.........................................................................
