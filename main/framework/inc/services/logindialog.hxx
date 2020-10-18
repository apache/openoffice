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



#ifndef __FRAMEWORK_LOGINDIALOG_LOGINDIALOG_HXX_
#define __FRAMEWORK_LOGINDIALOG_LOGINDIALOG_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_THREADHELPER_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif
#include <macros/generic.hxx>
#include <macros/debug.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>

#include <services/logindialog.hrc>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/awt/XDialog.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/beans/Property.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/lang/Locale.hpp>
#include <com/sun/star/util/XFlushable.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <tools/config.hxx>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/propshlp.hxx>
#include <vcl/dialog.hxx>
#include <vcl/fixed.hxx>
#include <vcl/edit.hxx>
#include <vcl/combobox.hxx>

#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif

#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#include <vcl/morebtn.hxx>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace framework{

#define	ANY									::com::sun::star::uno::Any
#define	EXCEPTION							::com::sun::star::uno::Exception
#define	ILLEGALARGUMENTEXCEPTION			::com::sun::star::lang::IllegalArgumentException
#define	IPROPERTYARRAYHELPER				::cppu::IPropertyArrayHelper
#define	OBROADCASTHELPER					::cppu::OBroadcastHelper
#define	OPROPERTYSETHELPER					::cppu::OPropertySetHelper
#define	OUSTRING							::rtl::OUString
#define	OWEAKOBJECT							::cppu::OWeakObject
#define	PROPERTY							::com::sun::star::beans::Property
#define	REFERENCE							::com::sun::star::uno::Reference
#define	RUNTIMEEXCEPTION					::com::sun::star::uno::RuntimeException
#define	SEQUENCE							::com::sun::star::uno::Sequence
#define	XDIALOG								::com::sun::star::awt::XDialog
#define	XMULTISERVICEFACTORY				::com::sun::star::lang::XMultiServiceFactory
#define	XPROPERTYSETINFO					::com::sun::star::beans::XPropertySetInfo
#define	XSERVICEINFO						::com::sun::star::lang::XServiceInfo
#define	XTYPEPROVIDER						::com::sun::star::lang::XTypeProvider
#define	PROPERTYVALUE						::com::sun::star::beans::PropertyValue
#define	LOCALE								::com::sun::star::lang::Locale
#define XFLUSHABLE                          ::com::sun::star::util::XFlushable
#define XFLUSHLISTENER                      ::com::sun::star::util::XFlushListener

//_________________________________________________________________________________________________________________
//	exported const
//_________________________________________________________________________________________________________________

#ifdef WNT
	#define	ININAME								DECLARE_ASCII("login.ini")
#elif defined UNIX
	#define	ININAME								DECLARE_ASCII("loginrc")
#else
	#error "name of login profile unknown!"
#endif

#define	UNCPATHSEPERATOR						sal_Unicode(0x002F)
#define	MAX_SERVERHISTORY						10

//	Use follow keys in follow order.
//	[Global]
//	UserName=as
//	ActiveServer=2
//	ConnectionType=https
//	Language=en-US
//	UseProxy=[browser|custom|none]
//	SecurityProxy=so-webcache:3128
//	dialog=[big|small]
//
//	[DefaultPorts]
//	https=8445
//	http=8090
//
//	[ServerHistory]
//	Server_1=localhost
//	Server_2=munch:7202
//	Server_3=www.xxx.com:8000

#define	SECTION_GLOBAL							"Global"
#define	SECTION_DEFAULTPORTS					"DefaultPorts"
#define	SECTION_SERVERHISTORY					"ServerHistory"

#define	SECTION_USERNAME						SECTION_GLOBAL
#define	SECTION_ACTIVESERVER					SECTION_GLOBAL
#define	SECTION_CONNECTIONTYPE					SECTION_GLOBAL
#define	SECTION_LANGUAGE						SECTION_GLOBAL
#define SECTION_SECURITYPROXY					SECTION_GLOBAL
#define SECTION_DIALOG							SECTION_GLOBAL
#define SECTION_HTTP							SECTION_DEFAULTPORTS
#define SECTION_HTTPS							SECTION_DEFAULTPORTS
#define	SECTION_SERVER_X						SECTION_SERVERHISTORY

#define	KEY_USERNAME							"UserName"
#define	KEY_ACTIVESERVER						"ActiveServer"
#define	KEY_CONNECTIONTYPE						"ConnectionType"
#define	KEY_LANGUAGE							"Language"
#define	KEY_SERVER_X							"Server_"
#define	KEY_SECURITYPROXY						"SecurityProxy"
#define KEY_USESECURITYPROXY					"UseProxy"
#define KEY_DIALOG								"dialog"
#define KEY_HTTP								"http"
#define KEY_HTTPS								"https"

#define	PROPERTYNAME_CONNECTIONTYPE				DECLARE_ASCII("ConnectionType"					)
#define	PROPERTYNAME_LANGUAGE					DECLARE_ASCII("Language"						)
#define PROPERTYNAME_PARENTWINDOW               DECLARE_ASCII("ParentWindow"                    )
#define	PROPERTYNAME_PASSWORD					DECLARE_ASCII("Password"						)
#define PROPERTYNAME_SECURITYPROXY				DECLARE_ASCII("SecurityProxy"					)
#define	PROPERTYNAME_SERVER						DECLARE_ASCII("Server"							)
#define	PROPERTYNAME_SERVERHISTORY				DECLARE_ASCII("ServerHistory"					)
#define	PROPERTYNAME_USERNAME					DECLARE_ASCII("UserName"						)
#define PROPERTYNAME_USEPROXY					DECLARE_ASCII("UseProxy"						)
#define PROPERTYNAME_DIALOG						DECLARE_ASCII("Dialog"							)
#define PROPERTYNAME_HTTP						DECLARE_ASCII("http"							)
#define PROPERTYNAME_HTTPS						DECLARE_ASCII("https"							)

#define	PROPERTYHANDLE_CONNECTIONTYPE			1
#define	PROPERTYHANDLE_LANGUAGE					2
#define PROPERTYHANDLE_PARENTWINDOW             3
#define PROPERTYHANDLE_PASSWORD                 4
#define PROPERTYHANDLE_SERVER                   5
#define PROPERTYHANDLE_SERVERHISTORY            6
#define PROPERTYHANDLE_USERNAME                 7
#define PROPERTYHANDLE_SECURITYPROXY            8
#define PROPERTYHANDLE_USEPROXY		            9
#define PROPERTYHANDLE_DIALOG		            10
#define PROPERTYHANDLE_HTTP						11
#define PROPERTYHANDLE_HTTPS					12

#define PROPERTYCOUNT                           12

//_________________________________________________________________________________________________________________
//	exported definitions
//_________________________________________________________________________________________________________________

struct tIMPL_DialogData
{
	OUSTRING				sUserName				;
	OUSTRING				sPassword				;
	SEQUENCE< OUSTRING >	seqServerList			;
	sal_Int32				nActiveServer			;
	OUSTRING				sConnectionType			;
	LOCALE					aLanguage				;
	sal_Int32				nPortHttp				;
	sal_Int32				nPortHttps				;
    ANY                     aParentWindow           ;
	OUSTRING				sSecurityProxy			;
	OUSTRING				sUseProxy				;
	OUSTRING				sDialog					;
	sal_Bool				bProxyChanged			;

	// default ctor to initialize empty structure.
	tIMPL_DialogData()
		:	sUserName				( OUSTRING()							)
		,	sPassword				( OUSTRING()							)
		,	seqServerList			( SEQUENCE< OUSTRING >()				)
		,	nActiveServer			( 1										)
		,	sConnectionType			( OUSTRING()							)
		,	aLanguage				( OUSTRING(), OUSTRING(), OUSTRING()	)
		,	nPortHttp				( 0										)
		,	nPortHttps				( 0										)
        ,   aParentWindow           (                                       )
		,	sSecurityProxy			( OUSTRING()							)
		,	sUseProxy				( OUSTRING()							)
		,	sDialog					( OUSTRING()							)
		,	bProxyChanged			( sal_False								)
	{
	}

	// copy ctor to initialize structure with values from another one.
	tIMPL_DialogData( const tIMPL_DialogData& aCopyDataSet )
		:	sUserName				( aCopyDataSet.sUserName				)
		,	sPassword				( aCopyDataSet.sPassword				)
		,	seqServerList			( aCopyDataSet.seqServerList			)
		,	nActiveServer			( aCopyDataSet.nActiveServer			)
		,	sConnectionType			( aCopyDataSet.sConnectionType			)
		,	aLanguage				( aCopyDataSet.aLanguage				)
		,	nPortHttp				( aCopyDataSet.nPortHttp				)
		,	nPortHttps				( aCopyDataSet.nPortHttps				)
        ,   aParentWindow           ( aCopyDataSet.aParentWindow            )
		,	sSecurityProxy			( aCopyDataSet.sSecurityProxy			)
		,	sUseProxy				( aCopyDataSet.sUseProxy				)
		,	sDialog					( aCopyDataSet.sDialog					)
		,	bProxyChanged			( aCopyDataSet.bProxyChanged			)
	{
	}

	// assignment operator to cop values from another struct to this one.
	tIMPL_DialogData& operator=( const tIMPL_DialogData& aCopyDataSet )
	{
		sUserName				= aCopyDataSet.sUserName				;
		sPassword				= aCopyDataSet.sPassword				;
		seqServerList			= aCopyDataSet.seqServerList			;
		nActiveServer			= aCopyDataSet.nActiveServer			;
		sConnectionType			= aCopyDataSet.sConnectionType			;
		aLanguage				= aCopyDataSet.aLanguage				;
		nPortHttp				= aCopyDataSet.nPortHttp				;
		nPortHttps				= aCopyDataSet.nPortHttps				;
        aParentWindow           = aCopyDataSet.aParentWindow            ;
		sSecurityProxy			= aCopyDataSet.sSecurityProxy           ;
		sUseProxy				= aCopyDataSet.sUseProxy				;
		sDialog					= aCopyDataSet.sDialog					;
		bProxyChanged			= aCopyDataSet.bProxyChanged            ;
		return *this;
	}
};

/*-************************************************************************************************************//**
	@short		implements an "private inline" dialog class used by follow class LoginDialog to show the dialog
	@descr		This is a VCL- modal dialog and not threadsafe! We use it as private definition in the context of login dialog only!

	@implements	-

	@base		ModalDialog
*//*-*************************************************************************************************************/

class cIMPL_Dialog	:	public ModalDialog
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------

	public:

		/*-****************************************************************************************************//**
			@short		default ctor
			@descr		This ctor initialize the dialog, load ressources but not set values on edits or check boxes!
						These is implemented by setValues() on the same class.
						You must give us a language identifier to describe which ressource should be used!

			@seealso	method setValues()

            @param      "aLanguage" , identifier to describe ressource language
            @param      "pParent"   , parent window handle for dialog! If is it NULL -> no parent exist ...
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

        cIMPL_Dialog( ::com::sun::star::lang::Locale aLocale, Window* pParent );

		/*-****************************************************************************************************//**
			@short		default dtor
			@descr		This dtor deinitialize the dialog and free all used ressources.
						But you can't get the values of the dialog. Use getValues() to do this.

			@seealso	method getValues()

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		~cIMPL_Dialog();

		/*-****************************************************************************************************//**
			@short		set new values on dialog to show
			@descr		Use this to initialize the dialg with new values for showing before execute.

			@seealso	method getValues()

			@param		"aDataSet"; struct of variables to set it on dialog controls
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		void setValues( const tIMPL_DialogData& aDataSet );

		/*-****************************************************************************************************//**
			@short		get current values from dialog controls
			@descr		Use this if you will get all values of dialog after execute.

			@seealso	method setValues()

			@param		"aDataSet"; struct of variables filled by dialog
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		tIMPL_DialogData getValues();

		/*-****************************************************************************************************/
		/* handler
		*/

		DECL_LINK( ClickHdl, void* );

	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------

	private:
		void			setCustomSettings();

		void			showDialogExpanded();
		void			showDialogCollapsed();

		/*-****************************************************************************************************//**
			@short		get a host and port from a concated string form <host>:<port>

			@param		"aProxyHostPort" ; a string with the following format <host>:<port>
			@param		"aHost"			 ; a host string
			@param		"aPort"			 ; a port string
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		void			getProxyHostPort( const OUSTRING& aProxyHostPort, OUSTRING& aHost, OUSTRING& aPort );

		/*-****************************************************************************************************//**
			@short		get a ressource for given id from right ressource file
			@descr		This dialog need his own ressource. We can't use the global ressource manager!
						We must use our own.
						You must give us the ressource language. If no right ressource could be found -
						any  existing one is used automatically!

			@seealso	method setValues()

			@param		"nId"		; id to convert it in right ressource id
			@param		"aLanguage"	; type of ressource language
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		static ResId impl_getResId(	sal_uInt16		nId			,
									::com::sun::star::lang::Locale	aLocale	);

	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------

	private:

		FixedImage			m_imageHeader				;
		FixedText			m_textLoginText				;
		FixedText       	m_textUserName				;
		Edit            	m_editUserName				;
		FixedText       	m_textPassword				;
		Edit            	m_editPassword				;
		FixedLine			m_fixedLineServer			;
		FixedText       	m_textServer				;
		ComboBox        	m_comboServer				;
		FixedLine			m_fixedLineProxySettings	;
		RadioButton			m_radioNoProxy				;
		RadioButton			m_radioBrowserProxy			;
		RadioButton			m_radioCustomProxy			;
		FixedText			m_textSecurityProxy			;
		FixedText			m_textSecurityProxyHost		;
		Edit				m_editSecurityProxyHost		;
		FixedText			m_textSecurityProxyPort		;
		Edit				m_editSecurityProxyPort		;
		FixedLine			m_fixedLineButtons			;
		OKButton        	m_buttonOK					;
		CancelButton    	m_buttonCancel				;
		PushButton			m_buttonAdditionalSettings	;
		Size				m_expandedDialogSize		;
		Size				m_collapsedDialogSize		;
		Point				m_expOKButtonPos			;
		Point				m_expCancelButtonPos		;
		Point				m_expAdditionalButtonPos	;
		Point				m_colOKButtonPos			;
		Point				m_colCancelButtonPos		;
		Point				m_colAdditionalButtonPos	;
		OUSTRING			m_colButtonAddText			;
		OUSTRING			m_expButtonAddText			;
		tIMPL_DialogData	m_aDataSet					;
};

/*-************************************************************************************************************//**
	@short

	@descr		-

	@implements	XInterface
				XTypeProvider
				XServiceInfo
				XDialog

    @base       ThreadHelpBase
				OWeakObject
*//*-*************************************************************************************************************/

class LoginDialog	:	public XTYPEPROVIDER				,
						public XSERVICEINFO					,
						public XDIALOG						,
                        public XFLUSHABLE                   ,
                        private ThreadHelpBase              ,   // Order of baseclasses is necessary for right initialization!
						public OBROADCASTHELPER				,
						public OPROPERTYSETHELPER			,
						public OWEAKOBJECT
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------

	public:

		//---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		-
			@descr		-

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

	 	LoginDialog( const REFERENCE< XMULTISERVICEFACTORY >& sFactory );

		/*-****************************************************************************************************//**
			@short		-
			@descr		-

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		virtual	~LoginDialog();

		//---------------------------------------------------------------------------------------------------------
		//	XInterface, XTypeProvider, XServiceInfo
		//---------------------------------------------------------------------------------------------------------

		DECLARE_XINTERFACE
		DECLARE_XTYPEPROVIDER
		DECLARE_XSERVICEINFO

        //---------------------------------------------------------------------------------------------------------
		//	XFlushable
		//---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
			@short		write changed values to configuration
			@descr		Normally the dialog returns with an OK or ERROR value. If OK occur - we flush data
						automaticly. But otherwise we do nothing. If user of this service wish to use property set
						only without any UI(!) - he must call "flush()" explicitly to write data!

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

    	virtual void SAL_CALL flush() throw( RUNTIMEEXCEPTION );
    	virtual void SAL_CALL addFlushListener( const REFERENCE< XFLUSHLISTENER >& xListener ) throw( RUNTIMEEXCEPTION );
    	virtual void SAL_CALL removeFlushListener( const REFERENCE< XFLUSHLISTENER >& xListener ) throw( RUNTIMEEXCEPTION );

		//---------------------------------------------------------------------------------------------------------
		//	XDialog
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		set new title of dialog
			@descr		-

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

    	virtual void SAL_CALL setTitle( const OUSTRING& sTitle ) throw( RUNTIMEEXCEPTION );

		/*-****************************************************************************************************//**
			@short		return the current title of this dialog
			@descr		-

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

    	virtual OUSTRING SAL_CALL getTitle() throw( RUNTIMEEXCEPTION );

		/*-****************************************************************************************************//**
			@short		show the dialog and return user reaction
			@descr		If user close dialog with OK we return 1 else
						user has cancelled this dialog and we return 0.
						You can use this return value directly as boolean.

			@seealso	-

			@param		-
			@return		1; if closed with OK
			@return		0; if cancelled

			@onerror	We return 0(sal_False).
		*//*-*****************************************************************************************************/

    	virtual sal_Int16 SAL_CALL execute() throw( RUNTIMEEXCEPTION );

		/*-****************************************************************************************************//**
			@short		not implemented yet!
			@descr		-

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

    	virtual void SAL_CALL endExecute() throw( RUNTIMEEXCEPTION );

	//-------------------------------------------------------------------------------------------------------------
	//	protected methods
	//-------------------------------------------------------------------------------------------------------------

	protected:

		//---------------------------------------------------------------------------
		//	OPropertySetHelper
		//---------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		try to convert a property value
			@descr		This method is calling from helperclass "OPropertySetHelper".
						Don't use this directly!
						You must try to convert the value of given propertyhandle and
						return results of this operation. This will be use to ask vetoable
						listener. If no listener have a veto, we will change value really!
						( in method setFastPropertyValue_NoBroadcast(...) )

			@seealso	class OPropertySetHelper
			@seealso	method setFastPropertyValue_NoBroadcast()
			@seealso	method impl_tryToChangeProperty()

			@param		"aConvertedValue"	new converted value of property
			@param		"aOldValue"			old value of property
			@param		"nHandle"			handle of property
			@param		"aValue"			new value of property
			@return		sal_True if value will be changed, sal_FALSE otherway

			@onerror	IllegalArgumentException, if you call this with an invalid argument
		*//*-*****************************************************************************************************/

		virtual sal_Bool SAL_CALL convertFastPropertyValue(			ANY&		aConvertedValue	,
								  									ANY&		aOldValue		,
																	sal_Int32	nHandle			,
															const	ANY&		aValue			) throw( ILLEGALARGUMENTEXCEPTION );

		/*-****************************************************************************************************//**
			@short		set value of a transient property
			@descr		This method is calling from helperclass "OPropertySetHelper".
						Don't use this directly!
						Handle and value are valid everyway! You must set the new value only.
						After this, baseclass send messages to all listener automatically.

			@seealso	OPropertySetHelper

			@param      "nHandle"	handle of property to change
			@param      "aValue"	new value of property
			@return		-

			@onerror	An exception is thrown.
		*//*-*****************************************************************************************************/

		virtual void SAL_CALL setFastPropertyValue_NoBroadcast(			sal_Int32	nHandle	,
							  									const	ANY&		aValue	) throw( EXCEPTION );

		/*-****************************************************************************************************//**
			@short		get value of a transient property
			@descr		This method is calling from helperclass "OPropertySetHelper".
						Don't use this directly!

			@seealso	OPropertySetHelper

			@param      "nHandle"	handle of property to change
			@param      "aValue"	current value of property
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		virtual void SAL_CALL getFastPropertyValue(	ANY&		aValue	,
							  						sal_Int32	nHandle	) const;

		/*-****************************************************************************************************//**
			@short      return structure and information about transient properties
			@descr		This method is calling from helperclass "OPropertySetHelper".
						Don't use this directly!

			@seealso	OPropertySetHelper

			@param		-
			@return		structure with property-informations

			@onerror	-
		*//*-*****************************************************************************************************/

		virtual IPROPERTYARRAYHELPER& SAL_CALL getInfoHelper();

		/*-****************************************************************************************************//**
			@short		return propertysetinfo
			@descr		You can call this method to get information about transient properties
						of this object.

			@seealso	OPropertySetHelper
			@seealso	XPropertySet
			@seealso	XMultiPropertySet

			@param		-
			@return		reference to object with information [XPropertySetInfo]

			@onerror	-
		*//*-*****************************************************************************************************/

		virtual REFERENCE< XPROPERTYSETINFO > SAL_CALL getPropertySetInfo() throw (::com::sun::star::uno::RuntimeException);

	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------

	private:

		/*-****************************************************************************************************//**
			@short		return table of all supported properties
			@descr		We need this table to initialize our helper baseclass OPropertySetHelper

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		const SEQUENCE< PROPERTY > impl_getStaticPropertyDescriptor();

		/*-****************************************************************************************************//**
			@short		helper method to check if a property will change his value
			@descr		Is necessary for vetoable listener mechanism of OPropertySethelper.

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		sal_Bool impl_tryToChangeProperty(	const	OUSTRING&				sProperty		,
											const	ANY&					aValue			,
													ANY&					aOldValue		,
													ANY&					aConvertedValue	) throw( ILLEGALARGUMENTEXCEPTION );

		sal_Bool impl_tryToChangeProperty(	const	SEQUENCE< OUSTRING >&	seqProperty		,
											const	ANY&					aValue			,
													ANY&					aOldValue		,
													ANY&					aConvertedValue	) throw( ILLEGALARGUMENTEXCEPTION );

		sal_Bool impl_tryToChangeProperty(	const	sal_Int32&				nProperty		,
											const	ANY&					aValue			,
													ANY&					aOldValue		,
													ANY&					aConvertedValue	) throw( ILLEGALARGUMENTEXCEPTION );

		sal_Bool impl_tryToChangeProperty(	const	LOCALE&					aProperty		,
											const	ANY&					aValue			,
													ANY&					aOldValue		,
													ANY&					aConvertedValue	) throw( ILLEGALARGUMENTEXCEPTION );

        sal_Bool impl_tryToChangeProperty(  const   ANY&                    aProperty       ,
											const	ANY&					aValue			,
													ANY&					aOldValue		,
													ANY&					aConvertedValue	) throw( ILLEGALARGUMENTEXCEPTION );

		/*-****************************************************************************************************//**
			@short		search and open profile
			@descr		This method search and open the ini file. It initialize some member too.

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		void impl_openProfile();

		/*-****************************************************************************************************//**
			@short		close profile and free some member
			@descr		This method close current opened ini file and deinitialize some member too.

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		void impl_closeProfile();

		/*-****************************************************************************************************//**
			@short		write profile and free some member
			@descr		This method writes current settings and deinitialize some member too.

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/
		void impl_writeProfile();

		/*-****************************************************************************************************//**
			@short		check current server history
			@descr		Our current server history implementation can handle 10 elements as maximum.
						If more then 10 elements exist; old ones will be deleted.

			@seealso	-

			@param		"seqHistory"; current history
			@return		Sequence< OUString >; checked and repaired history

			@onerror	-
		*//*-*****************************************************************************************************/

		void impl_addServerToHistory(			SEQUENCE< OUSTRING >&	seqHistory		,
												sal_Int32&				nActiveServer	,
										const	OUSTRING&				sServer			);

		/*-****************************************************************************************************//**
			@short		helper methods to read/write  properties from/to ini file
			@descr		Using of Config-Class isn't easy every time :-(
						That's the reason for these helper. State of operation isn't really important...
						but we assert impossible cases or occurred errors!

			@seealso	-

			@param		-
			@return		-

			@onerror	Assertions are shown.
		*//*-*****************************************************************************************************/

		void					impl_writeUserName				(	const	OUSTRING&				sUserName		);
		void					impl_writeActiveServer			(			sal_Int32				nActiveServer	);
		void					impl_writeServerHistory			(	const	SEQUENCE< OUSTRING >&	lHistory		);
		void					impl_writeConnectionType		(	const	OUSTRING&				sConnectionType	);
		void					impl_writeLanguage				(	const	LOCALE&					aLanguage		);
		void					impl_writePortHttp				(			sal_Int32				nPort			);
		void					impl_writePortHttps				(			sal_Int32				nPort			);
		void					impl_writeSecurityProxy			(	const	OUSTRING&				sSecurityProxy	);
		void					impl_writeUseProxy				(	const	OUSTRING&				sUseProxy		);
		void					impl_writeDialog				(	const	OUSTRING&				sDialog			);

		OUSTRING				impl_readUserName				(													);
		sal_Int32				impl_readActiveServer			(													);
		SEQUENCE< OUSTRING >	impl_readServerHistory			(													);
		OUSTRING				impl_readConnectionType			(													);
		LOCALE					impl_readLanguage				(													);
		sal_Int32				impl_readPortHttp				(													);
		sal_Int32				impl_readPortHttps				(													);
		OUSTRING				impl_readSecurityProxy			(													);
		OUSTRING				impl_readUseProxy				(													);
		OUSTRING				impl_readDialog					(													);

	//-------------------------------------------------------------------------------------------------------------
	//	debug methods
	//	(should be private everyway!)
	//-------------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		debug-method to check incoming parameter of some other mehods of this class
			@descr		The following methods are used to check parameters for other methods
						of this class. The return value is used directly for an ASSERT(...).

			@seealso	ASSERTs in implementation!

			@param		references to checking variables
			@return		sal_False on invalid parameter<BR>
						sal_True  otherway

			@onerror	-
		*//*-*****************************************************************************************************/

	#ifdef ENABLE_ASSERTIONS

	private:

		sal_Bool impldbg_checkParameter_LoginDialog	(	const	REFERENCE< XMULTISERVICEFACTORY >&	xFactory	);
		sal_Bool impldbg_checkParameter_setTitle	(	const	OUSTRING&							sTitle		);

	#endif	// #ifdef ENABLE_ASSERTIONS

	//-------------------------------------------------------------------------------------------------------------
	//	variables
	//	(should be private everyway!)
	//-------------------------------------------------------------------------------------------------------------

	private:

		REFERENCE< XMULTISERVICEFACTORY >		m_xFactory			;	/// reference to factory, which has created this instance
		OUSTRING								m_sININame			;	/// full qualified path to profile UNC-notation
		Config*									m_pINIManager		;	/// manager for full access to ini file
		sal_Bool								m_bInExecuteMode	;	/// protection against setting of properties during showing of dialog
		cIMPL_Dialog*							m_pDialog			;	/// VCL dialog
		tIMPL_DialogData						m_aPropertySet		;

};		//	class LoginDialog

}		//	namespace framework

#endif	//	#ifndef __FRAMEWORK_LOGINDIALOG_LOGINDIALOG_HXX_
