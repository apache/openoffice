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
#include "precompiled_extensions.hxx"
#include "controlwizard.hxx"
#include <tools/debug.hxx>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#include <com/sun/star/sdb/XQueriesSupplier.hpp>
#include <com/sun/star/sdbc/XPreparedStatement.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/drawing/XDrawView.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/sdb/CommandType.hpp>
#include <com/sun/star/sdbc/SQLWarning.hpp>
#include <com/sun/star/sdb/SQLContext.hpp>
#include <comphelper/types.hxx>
#include <connectivity/dbtools.hxx>
#include <vcl/msgbox.hxx>
#include <comphelper/interaction.hxx>
#include <vcl/stdtext.hxx>
#include <svtools/localresaccess.hxx>
#include <connectivity/conncleanup.hxx>
#include <com/sun/star/sdbc/DataType.hpp>
#include <tools/urlobj.hxx>

//.........................................................................
namespace dbp
{
//.........................................................................

	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::awt;
	using namespace ::com::sun::star::lang;
	using namespace ::com::sun::star::sdb;
	using namespace ::com::sun::star::sdbc;
	using namespace ::com::sun::star::sdbcx;
	using namespace ::com::sun::star::beans;
	using namespace ::com::sun::star::container;
	using namespace ::com::sun::star::drawing;
	using namespace ::com::sun::star::frame;
	using namespace ::com::sun::star::sheet;
	using namespace ::com::sun::star::form;
	using namespace ::com::sun::star::task;
	using namespace ::svt;
	using namespace ::comphelper;
	using namespace ::dbtools;

	//=====================================================================
	//= OAccessRegulator
	//=====================================================================
	struct OAccessRegulator
	{
		friend class OControlWizardPage;

	protected:
		OAccessRegulator() { }
	};

	//=====================================================================
	//= OControlWizardPage
	//=====================================================================
	//---------------------------------------------------------------------
	OControlWizardPage::OControlWizardPage( OControlWizard* _pParent, const ResId& _rResId )
		:OControlWizardPage_Base( _pParent, _rResId )
		,m_pFormSettingsSeparator(NULL)
		,m_pFormDatasourceLabel(NULL)
		,m_pFormDatasource(NULL)
		,m_pFormContentTypeLabel(NULL)
		,m_pFormContentType(NULL)
		,m_pFormTableLabel(NULL)
		,m_pFormTable(NULL)
	{
	}

	//---------------------------------------------------------------------
	OControlWizardPage::~OControlWizardPage()
	{
		delete m_pFormSettingsSeparator;
		delete m_pFormDatasourceLabel;
		delete m_pFormDatasource;
		delete m_pFormContentTypeLabel;
		delete m_pFormContentType;
		delete m_pFormTableLabel;
		delete m_pFormTable;
	}

	//---------------------------------------------------------------------
	OControlWizard* OControlWizardPage::getDialog()
	{
		return static_cast< OControlWizard* >(GetParent());
	}

	//---------------------------------------------------------------------
	const OControlWizard* OControlWizardPage::getDialog() const
	{
		return static_cast< OControlWizard* >(GetParent());
	}

	//---------------------------------------------------------------------
	sal_Bool OControlWizardPage::updateContext()
	{
		return getDialog()->updateContext(OAccessRegulator());
	}

	//---------------------------------------------------------------------
	Reference< XConnection > OControlWizardPage::getFormConnection() const
	{
		return getDialog()->getFormConnection(OAccessRegulator());
	}

	//---------------------------------------------------------------------
	void OControlWizardPage::setFormConnection( const Reference< XConnection >& _rxConn, sal_Bool _bAutoDispose )
	{
		getDialog()->setFormConnection( OAccessRegulator(), _rxConn, _bAutoDispose );
	}

	//---------------------------------------------------------------------
	const OControlWizardContext& OControlWizardPage::getContext()
	{
		return getDialog()->getContext();
	}

	//---------------------------------------------------------------------
	void OControlWizardPage::fillListBox(ListBox& _rList, const Sequence< ::rtl::OUString >& _rItems, sal_Bool _bClear)
	{
		if (_bClear)
			_rList.Clear();
		const ::rtl::OUString* pItems = _rItems.getConstArray();
		const ::rtl::OUString* pEnd = pItems + _rItems.getLength();
		::svt::WizardTypes::WizardState nPos;
		sal_Int32 nIndex = 0;
		for (;pItems < pEnd; ++pItems, ++nIndex)
		{
			nPos = _rList.InsertEntry(*pItems);
			_rList.SetEntryData(nPos, reinterpret_cast<void*>(nIndex));
		}
	}

	//---------------------------------------------------------------------
	void OControlWizardPage::fillListBox(ComboBox& _rList, const Sequence< ::rtl::OUString >& _rItems, sal_Bool _bClear)
	{
		if (_bClear)
			_rList.Clear();
		const ::rtl::OUString* pItems = _rItems.getConstArray();
		const ::rtl::OUString* pEnd = pItems + _rItems.getLength();
		::svt::WizardTypes::WizardState nPos;
		sal_Int32 nIndex = 0;
		for (;pItems < pEnd; ++pItems)
		{
			nPos = _rList.InsertEntry(*pItems);
			_rList.SetEntryData(nPos, reinterpret_cast<void*>(nIndex));
		}
	}

	//---------------------------------------------------------------------
	void OControlWizardPage::enableFormDatasourceDisplay()
	{
		if (m_pFormSettingsSeparator)
			// nothing to do
			return;

		ModuleRes aModuleRes(RID_PAGE_FORM_DATASOURCE_STATUS);
		OLocalResourceAccess aLocalControls(aModuleRes, RSC_TABPAGE);

		m_pFormSettingsSeparator	= new FixedLine(this,  ModuleRes(FL_FORMSETINGS));
		m_pFormDatasourceLabel		= new FixedText(this,  ModuleRes(FT_FORMDATASOURCELABEL));
		m_pFormDatasource			= new FixedText(this,  ModuleRes(FT_FORMDATASOURCE));
		m_pFormContentTypeLabel		= new FixedText(this,  ModuleRes(FT_FORMCONTENTTYPELABEL));
		m_pFormContentType			= new FixedText(this,  ModuleRes(FT_FORMCONTENTTYPE));
		m_pFormTableLabel			= new FixedText(this,  ModuleRes(FT_FORMTABLELABEL));
		m_pFormTable				= new FixedText(this,  ModuleRes(FT_FORMTABLE));

		const OControlWizardContext& rContext = getContext();
		if ( rContext.bEmbedded )
		{
			m_pFormDatasourceLabel->Hide();
			m_pFormDatasource->Hide();
			m_pFormContentTypeLabel->SetPosPixel(m_pFormDatasourceLabel->GetPosPixel());
			m_pFormContentType->SetPosPixel(m_pFormDatasource->GetPosPixel());
			m_pFormTableLabel->SetPosPixel(::Point(m_pFormDatasourceLabel->GetPosPixel().X(),m_pFormTableLabel->GetPosPixel().Y()));
			m_pFormTable->SetPosPixel(::Point(m_pFormDatasource->GetPosPixel().X(),m_pFormTable->GetPosPixel().Y()));
		}
	}

	//---------------------------------------------------------------------
	void OControlWizardPage::adjustControlForNoDSDisplay(Control* _pControl, sal_Bool _bConstLowerDistance)
	{
		::Size aDistanceToMove = LogicToPixel( ::Size( 0, 37 ), MAP_APPFONT );

		::Point aPos = _pControl->GetPosPixel();
		aPos.Y() -= aDistanceToMove.Height();
		_pControl->SetPosPixel(aPos);

		if (_bConstLowerDistance)
		{
			::Size aSize = _pControl->GetSizePixel();
			aSize.Height() += aDistanceToMove.Height();
			_pControl->SetSizePixel(aSize);
		}
	}

	//---------------------------------------------------------------------
	void OControlWizardPage::initializePage()
	{
		if (m_pFormDatasource && m_pFormContentTypeLabel && m_pFormTable)
		{
			const OControlWizardContext& rContext = getContext();
			::rtl::OUString sDataSource;
			::rtl::OUString sCommand;
			sal_Int32 nCommandType = CommandType::COMMAND;
			try
			{
				rContext.xForm->getPropertyValue(::rtl::OUString::createFromAscii("DataSourceName")) >>= sDataSource;
				rContext.xForm->getPropertyValue(::rtl::OUString::createFromAscii("Command")) >>= sCommand;
				rContext.xForm->getPropertyValue(::rtl::OUString::createFromAscii("CommandType")) >>= nCommandType;
			}
			catch(const Exception&)
			{
				DBG_ERROR("OControlWizardPage::initializePage: caught an exception!");
			}

			INetURLObject aURL( sDataSource );
			if( aURL.GetProtocol() != INET_PROT_NOT_VALID )
				sDataSource = aURL.GetName(INetURLObject::DECODE_WITH_CHARSET);
			m_pFormDatasource->SetText(sDataSource);
			m_pFormTable->SetText(sCommand);

			::svt::WizardTypes::WizardState nCommandTypeResourceId = 0;
			switch (nCommandType)
			{
				case CommandType::TABLE:
					nCommandTypeResourceId = RID_STR_TYPE_TABLE;
					break;

				case CommandType::QUERY:
					nCommandTypeResourceId = RID_STR_TYPE_QUERY;
					break;

				default:
					nCommandTypeResourceId = RID_STR_TYPE_COMMAND;
					break;
			}
			m_pFormContentType->SetText(String(ModuleRes(nCommandTypeResourceId)));
		}

		OControlWizardPage_Base::initializePage();
	}

	//=====================================================================
	//= OControlWizard
	//=====================================================================
	//---------------------------------------------------------------------
	OControlWizard::OControlWizard( Window* _pParent, const ResId& _rId,
			const Reference< XPropertySet >& _rxObjectModel, const Reference< XMultiServiceFactory >& _rxORB )
		:OWizardMachine(_pParent, _rId, WZB_CANCEL | WZB_PREVIOUS | WZB_NEXT | WZB_FINISH)
		,m_xORB(_rxORB)
	{
		m_aContext.xObjectModel = _rxObjectModel;
		initContext();

		SetPageSizePixel(LogicToPixel(::Size(WINDOW_SIZE_X, WINDOW_SIZE_Y), MAP_APPFONT));
		ShowButtonFixedLine(sal_True);
		defaultButton(WZB_NEXT);
		enableButtons(WZB_FINISH, sal_False);
	}

	//---------------------------------------------------------------------
	OControlWizard::~OControlWizard()
	{
	}

	//---------------------------------------------------------------------
	short OControlWizard::Execute()
	{
		// get the class id of the control we're dealing with
		sal_Int16 nClassId = FormComponentType::CONTROL;
		try
		{
			getContext().xObjectModel->getPropertyValue(::rtl::OUString::createFromAscii("ClassId")) >>= nClassId;
		}
		catch(Exception&)
		{
			DBG_ERROR("OControlWizard::activate: could not obtain the class id!");
		}
		if (!approveControl(nClassId))
		{
			// TODO: MessageBox or exception
			return RET_CANCEL;
		}

		ActivatePage();

		return OControlWizard_Base::Execute();
	}

	//---------------------------------------------------------------------
	void OControlWizard::ActivatePage()
	{
		OControlWizard_Base::ActivatePage();
	}

	//---------------------------------------------------------------------
	void OControlWizard::implDetermineShape()
	{
		Reference< XIndexAccess > xPageObjects(m_aContext.xDrawPage, UNO_QUERY);
		DBG_ASSERT(xPageObjects.is(), "OControlWizard::implDetermineShape: invalid page!");

		// for comparing the model
		Reference< XControlModel > xModelCompare(m_aContext.xObjectModel, UNO_QUERY);

		if (xPageObjects.is())
		{
			// loop through all objects of the page
			sal_Int32 nObjects = xPageObjects->getCount();
			Reference< XControlShape > xControlShape;
			Reference< XControlModel > xControlModel;
			for (sal_Int32 i=0; i<nObjects; ++i)
			{
				if (xPageObjects->getByIndex(i) >>= xControlShape)
				{	// it _is_ a control shape
					xControlModel = xControlShape->getControl();
					DBG_ASSERT(xControlModel.is(), "OControlWizard::implDetermineShape: control shape without model!");
					if (xModelCompare.get() == xControlModel.get())
					{
						m_aContext.xObjectShape = xControlShape;
						break;
					}
				}
			}
		}
	}

	//---------------------------------------------------------------------
	void OControlWizard::implDetermineForm()
	{
		Reference< XChild > xModelAsChild(m_aContext.xObjectModel, UNO_QUERY);
		Reference< XInterface > xControlParent;
		if (xModelAsChild.is())
			xControlParent = xModelAsChild->getParent();

		m_aContext.xForm = Reference< XPropertySet >(xControlParent, UNO_QUERY);
		m_aContext.xRowSet = Reference< XRowSet >(xControlParent, UNO_QUERY);
		DBG_ASSERT(m_aContext.xForm.is() && m_aContext.xRowSet.is(),
			"OControlWizard::implDetermineForm: missing some interfaces of the control parent!");

	}

	//---------------------------------------------------------------------
	void OControlWizard::implDeterminePage()
	{
		try
		{
			// get the document model
			Reference< XChild > xControlAsChild(m_aContext.xObjectModel, UNO_QUERY);
			Reference< XChild > xModelSearch(xControlAsChild->getParent(), UNO_QUERY);

			Reference< XModel > xModel(xModelSearch, UNO_QUERY);
			while (xModelSearch.is() && !xModel.is())
			{
				xModelSearch = Reference< XChild >(xModelSearch->getParent(), UNO_QUERY);
				xModel = Reference< XModel >(xModelSearch, UNO_QUERY);
			}

			Reference< XDrawPage > xPage;
			if (xModel.is())
			{
				m_aContext.xDocumentModel = xModel;

				Reference< XDrawPageSupplier > xPageSupp(xModel, UNO_QUERY);
				if (xPageSupp.is())
				{	// it's a document with only one page -> Writer
					xPage = xPageSupp->getDrawPage();
				}
				else
				{
					// get the controller currently working on this model
					Reference< XController > xController = xModel->getCurrentController();
					DBG_ASSERT(xController.is(), "OControlWizard::implDeterminePage: no current controller!");

					// maybe it's a spreadsheet
					Reference< XSpreadsheetView > xView(xController, UNO_QUERY);
					if (xView.is())
					{	// okay, it is one
						Reference< XSpreadsheet > xSheet = xView->getActiveSheet();
						xPageSupp = Reference< XDrawPageSupplier >(xSheet, UNO_QUERY);
						DBG_ASSERT(xPageSupp.is(), "OControlWizard::implDeterminePage: a spreadsheet which is no page supplier!");
						if (xPageSupp.is())
							xPage = xPageSupp->getDrawPage();
					}
					else
					{	// can be a Draw/Impress doc only
						Reference< XDrawView > xDrawView(xController, UNO_QUERY);
						DBG_ASSERT(xDrawView.is(), "OControlWizard::implDeterminePage: no alternatives left ... can't determine the page!");
						if (xDrawView.is())
							xPage = xDrawView->getCurrentPage();
					}
				}
			}
			else
			{
				DBG_ASSERT(xPage.is(), "OControlWizard::implDeterminePage: can't determine the page (no model)!");
			}
			m_aContext.xDrawPage = xPage;
		}
		catch(Exception&)
		{
			DBG_ERROR("OControlWizard::implDeterminePage: caught an exception!");
		}
	}

	//---------------------------------------------------------------------
	void OControlWizard::implGetDSContext()
	{
		Reference< XMultiServiceFactory > xORB = getServiceFactory();
		try
		{
			DBG_ASSERT(xORB.is(), "OControlWizard::implGetDSContext: invalid service factory!");

			Reference< XInterface > xContext;
			if (xORB.is())
				xContext = xORB->createInstance(::rtl::OUString::createFromAscii("com.sun.star.sdb.DatabaseContext"));
			DBG_ASSERT(xContext.is(), "OControlWizard::implGetDSContext: invalid database context!");

			m_aContext.xDatasourceContext = Reference< XNameAccess >(xContext, UNO_QUERY);
			DBG_ASSERT(m_aContext.xDatasourceContext.is() || !xContext.is(), "OControlWizard::implGetDSContext: invalid database context (missing the XNameAccess)!");
		}
		catch(Exception&)
		{
			DBG_ERROR("OControlWizard::implGetDSContext: invalid database context!");
		}
	}

	//---------------------------------------------------------------------
	Reference< XConnection > OControlWizard::getFormConnection(const OAccessRegulator&) const
	{
		return getFormConnection();
	}
	//---------------------------------------------------------------------
	Reference< XConnection > OControlWizard::getFormConnection() const
	{
		Reference< XConnection > xConn;
		try
		{
			if ( !::dbtools::isEmbeddedInDatabase(m_aContext.xForm,xConn) )
				m_aContext.xForm->getPropertyValue(::rtl::OUString::createFromAscii("ActiveConnection")) >>= xConn;
		}
		catch(const Exception&)
		{
			DBG_ERROR("OControlWizard::getFormConnection: caught an exception!");
		}
		return xConn;
	}

	//---------------------------------------------------------------------
	void OControlWizard::setFormConnection( const OAccessRegulator& _rAccess, const Reference< XConnection >& _rxConn, sal_Bool _bAutoDispose )
	{
		try
		{
			Reference< XConnection > xOldConn = getFormConnection(_rAccess);
			if (xOldConn.get() == _rxConn.get())
				return;

			disposeComponent(xOldConn);

			// set the new connection
			if ( _bAutoDispose )
			{
				// for this, use a AutoDisposer (so the conn is cleaned up when the form dies or gets another connection)
				Reference< XRowSet > xFormRowSet( m_aContext.xForm, UNO_QUERY );
				OAutoConnectionDisposer* pAutoDispose = new OAutoConnectionDisposer( xFormRowSet, _rxConn );
				Reference< XPropertyChangeListener > xEnsureDelete( pAutoDispose );
			}
			else
			{
				m_aContext.xForm->setPropertyValue( ::rtl::OUString::createFromAscii("ActiveConnection"), makeAny( _rxConn ) );
			}
		}
		catch(const Exception&)
		{
			DBG_ERROR("OControlWizard::setFormConnection: caught an exception!");
		}
	}

	//---------------------------------------------------------------------
	sal_Bool OControlWizard::updateContext(const OAccessRegulator&)
	{
		return initContext();
	}
	//---------------------------------------------------------------------
	Reference< XInteractionHandler > OControlWizard::getInteractionHandler(Window* _pWindow) const
	{
		const ::rtl::OUString sInteractionHandlerServiceName = ::rtl::OUString::createFromAscii("com.sun.star.task.InteractionHandler");
		Reference< XInteractionHandler > xHandler;
		try
		{
			if (getServiceFactory().is())
				xHandler = Reference< XInteractionHandler >(getServiceFactory()->createInstance(sInteractionHandlerServiceName), UNO_QUERY);
		}
		catch(Exception&) { }
		if (!xHandler.is())
			ShowServiceNotAvailableError(_pWindow, sInteractionHandlerServiceName, sal_True);
		return xHandler;
	}
	//---------------------------------------------------------------------
	sal_Bool OControlWizard::initContext()
	{
		DBG_ASSERT(m_aContext.xObjectModel.is(), "OGroupBoxWizard::initContext: have no control model to work with!");
		if (!m_aContext.xObjectModel.is())
			return sal_False;

		// reset the context
		m_aContext.xForm.clear();
		m_aContext.xRowSet.clear();
		m_aContext.xDocumentModel.clear();
		m_aContext.xDrawPage.clear();
		m_aContext.xObjectShape.clear();
		m_aContext.aFieldNames.realloc(0);

		m_aContext.xObjectContainer.clear();
		m_aContext.aTypes.clear();
		m_aContext.bEmbedded = sal_False;

		Any aSQLException;
		Reference< XPreparedStatement > xStatement;
		try
		{
			// get the datasource context
			implGetDSContext();

			// first, determine the form the control belongs to
			implDetermineForm();

			// need the page, too
			implDeterminePage();

			// the shape of the control
			implDetermineShape();

			// get the columns of the object the settings refer to
			Reference< XNameAccess > xColumns;

			if (m_aContext.xForm.is())
			{
				// collect some properties of the form
				::rtl::OUString sObjectName = ::comphelper::getString(m_aContext.xForm->getPropertyValue(::rtl::OUString::createFromAscii("Command")));
				sal_Int32 nObjectType = ::comphelper::getINT32(m_aContext.xForm->getPropertyValue(::rtl::OUString::createFromAscii("CommandType")));

				// calculate the connection the rowset is working with
				Reference< XConnection > xConnection;
				m_aContext.bEmbedded = ::dbtools::isEmbeddedInDatabase( m_aContext.xForm, xConnection );
				if ( !m_aContext.bEmbedded )
					xConnection = ::dbtools::connectRowset( m_aContext.xRowSet, getServiceFactory(), sal_True );

				// get the fields
				if (xConnection.is())
				{
					switch (nObjectType)
					{
						case 0:
						{
							Reference< XTablesSupplier > xSupplyTables(xConnection, UNO_QUERY);
							if (xSupplyTables.is() && xSupplyTables->getTables().is() && xSupplyTables->getTables()->hasByName(sObjectName))
							{
								Reference< XColumnsSupplier > xSupplyColumns;
								m_aContext.xObjectContainer = xSupplyTables->getTables();
								m_aContext.xObjectContainer->getByName(sObjectName) >>= xSupplyColumns;
								DBG_ASSERT(xSupplyColumns.is(), "OControlWizard::initContext: invalid table columns!");
								xColumns = xSupplyColumns->getColumns();
							}
						}
						break;
						case 1:
						{
							Reference< XQueriesSupplier > xSupplyQueries(xConnection, UNO_QUERY);
							if (xSupplyQueries.is() && xSupplyQueries->getQueries().is() && xSupplyQueries->getQueries()->hasByName(sObjectName))
							{
								Reference< XColumnsSupplier > xSupplyColumns;
								m_aContext.xObjectContainer = xSupplyQueries->getQueries();
								m_aContext.xObjectContainer->getByName(sObjectName) >>= xSupplyColumns;
								DBG_ASSERT(xSupplyColumns.is(), "OControlWizard::initContext: invalid query columns!");
								xColumns = xSupplyColumns->getColumns();
							}
						}
						break;
						default:
						{
							xStatement = xConnection->prepareStatement(sObjectName);

							// not interested in any results, only in the fields
							Reference< XPropertySet > xStatementProps(xStatement, UNO_QUERY);
							xStatementProps->setPropertyValue(::rtl::OUString::createFromAscii("MaxRows"), makeAny(sal_Int32(0)));

							// TODO: think about handling local SQLExceptions here ...
							Reference< XColumnsSupplier > xSupplyCols(xStatement->executeQuery(), UNO_QUERY);
							if (xSupplyCols.is())
								xColumns = xSupplyCols->getColumns();
						}
					}
				}
			}

			if (xColumns.is())
			{
				m_aContext.aFieldNames = xColumns->getElementNames();
				static const ::rtl::OUString s_sFieldTypeProperty	= ::rtl::OUString::createFromAscii("Type");
				const ::rtl::OUString* pBegin = m_aContext.aFieldNames.getConstArray();
				const ::rtl::OUString* pEnd	  = pBegin + m_aContext.aFieldNames.getLength();
				for(;pBegin != pEnd;++pBegin)
				{
					sal_Int32 nFieldType = DataType::OTHER;
					try
					{
						Reference< XPropertySet > xColumn;
						xColumns->getByName(*pBegin) >>= xColumn;
						xColumn->getPropertyValue(s_sFieldTypeProperty) >>= nFieldType;
					}
					catch(Exception&)
					{
						DBG_ERROR("OControlWizard::initContext: unexpected exception while gathering column information!");
					}
					m_aContext.aTypes.insert(OControlWizardContext::TNameTypeMap::value_type(*pBegin,nFieldType));
				}
			}
		}
		catch(SQLContext& e) { aSQLException <<= e; }
		catch(SQLWarning& e) { aSQLException <<= e; }
		catch(SQLException& e) { aSQLException <<= e; }
		catch(Exception&)
		{
			DBG_ERROR("OControlWizard::initContext: could not retrieve the control context (caught an exception)!");
		}

		::comphelper::disposeComponent(xStatement);

		if (aSQLException.hasValue())
		{	// an SQLException (or derive) was thrown ...

			// prepend an extra SQLContext explaining what we were doing
			SQLContext aContext;
			aContext.Message = String(ModuleRes(RID_STR_COULDNOTOPENTABLE));
			aContext.NextException = aSQLException;

			// create an interaction handler to display this exception
			Reference< XInteractionHandler > xHandler = getInteractionHandler(this);
			if ( !xHandler.is() )
				return sal_False;

			Reference< XInteractionRequest > xRequest = new OInteractionRequest(makeAny(aContext));
			try
			{
				xHandler->handle(xRequest);
			}
			catch(Exception&) { }
			return sal_False;
		}

		return 0 != m_aContext.aFieldNames.getLength();
	}

	//---------------------------------------------------------------------
	void OControlWizard::commitControlSettings(OControlWizardSettings* _pSettings)
	{
		DBG_ASSERT(m_aContext.xObjectModel.is(), "OControlWizard::commitControlSettings: have no control model to work with!");
		if (!m_aContext.xObjectModel.is())
			return;

		// the only thing we have at the moment is the label
		try
		{
			::rtl::OUString sLabelPropertyName = ::rtl::OUString::createFromAscii("Label");
			Reference< XPropertySetInfo > xInfo = m_aContext.xObjectModel->getPropertySetInfo();
			if (xInfo.is() && xInfo->hasPropertyByName(sLabelPropertyName))
			{
				::rtl::OUString sControlLabel(_pSettings->sControlLabel);
				m_aContext.xObjectModel->setPropertyValue(
					::rtl::OUString::createFromAscii("Label"),
					makeAny(sControlLabel)
				);
			}
		}
		catch(Exception&)
		{
			DBG_ERROR("OControlWizard::commitControlSettings: could not commit the basic control settings!");
		}
	}

	//---------------------------------------------------------------------
	void OControlWizard::initControlSettings(OControlWizardSettings* _pSettings)
	{
		DBG_ASSERT(m_aContext.xObjectModel.is(), "OControlWizard::initControlSettings: have no control model to work with!");
		if (!m_aContext.xObjectModel.is())
			return;

		// initialize some settings from the control model given
		try
		{
			::rtl::OUString sLabelPropertyName = ::rtl::OUString::createFromAscii("Label");
			Reference< XPropertySetInfo > xInfo = m_aContext.xObjectModel->getPropertySetInfo();
			if (xInfo.is() && xInfo->hasPropertyByName(sLabelPropertyName))
			{
				::rtl::OUString sControlLabel;
				m_aContext.xObjectModel->getPropertyValue(sLabelPropertyName) >>= sControlLabel;
				_pSettings->sControlLabel = sControlLabel;
			}
		}
		catch(Exception&)
		{
			DBG_ERROR("OControlWizard::initControlSettings: could not retrieve the basic control settings!");
		}
	}

	//---------------------------------------------------------------------
	sal_Bool OControlWizard::needDatasourceSelection()
	{
		// let me see...
		return (0 == getContext().aFieldNames.getLength());
			// if we got fields, the data source is valid ...
//		try
//		{
//			// first, we need a valid data source name
//			::rtl::OUString sDataSourceName;
//			m_aContext.xForm->getPropertyValue(::rtl::OUString::createFromAscii("DataSourceName")) >>= sDataSourceName;
//			if (m_aContext.xDatasourceContext.is() && m_aContext.xDatasourceContext->hasByName(sDataSourceName))
//			{	// at least the data source name is valid ...
//				// then, a CommandType "table" would be nice ...
//				sal_Int32 nCommandType = CommandType::COMMAND;
//				m_aContext.xForm->getPropertyValue(::rtl::OUString::createFromAscii("CommandType")) >>= nCommandType;
//				if (CommandType::TABLE == nCommandType)
//				{	// okay ....
//					// now the table itself should be valid
//					::rtl::OUString sTableName;
//					m_aContext.xForm->getPropertyValue(::rtl::OUString::createFromAscii("Command")) >>= sTableName;
//					if (m_aContext.xObjectContainer.is() && m_aContext.xObjectContainer->hasByName(sTableName))
//						return sal_False;
//				}
//			}
//		}
//		catch(Exception&)
//		{
//			DBG_ERROR("OControlWizard::needDatasourceSelection: caught an exception while checking the form settings!");
//		}
//		return sal_True;
	}

//.........................................................................
}	// namespace dbp
//.........................................................................

