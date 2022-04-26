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

#include "log_module.hxx"
#include "methodguard.hxx"
#include "loghandler.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/logging/XConsoleHandler.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/logging/LogLevel.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/ucb/AlreadyInitializedException.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
/** === end UNO includes === **/

#include <tools/diagnose_ex.h>

#include <comphelper/componentcontext.hxx>

#include <cppuhelper/compbase3.hxx>
#include <cppuhelper/basemutex.hxx>

#include <rtl/strbuf.hxx>
#include <osl/process.h>
#include <osl/socket.hxx>
#include <osl/time.h>

#include <stdio.h>

//........................................................................
namespace logging
{
//........................................................................

	/** === begin UNO using === **/
    using ::com::sun::star::logging::XLogHandler;
    using ::com::sun::star::lang::XServiceInfo;
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::logging::XLogFormatter;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::logging::LogRecord;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::uno::XInterface;
    using ::com::sun::star::lang::XInitialization;
    using ::com::sun::star::ucb::AlreadyInitializedException;
    using ::com::sun::star::lang::IllegalArgumentException;
    using ::com::sun::star::beans::NamedValue;
	/** === end UNO using === **/
    namespace LogLevel = ::com::sun::star::logging::LogLevel;

	//====================================================================
	//= SyslogHandler - declaration
	//====================================================================
	//--------------------------------------------------------------------
    typedef ::cppu::WeakComponentImplHelper3    <   XLogHandler
                                                ,   XServiceInfo
                                                ,   XInitialization
                                                >   SyslogHandler_Base;
    class SyslogHandler    :public ::cppu::BaseMutex
                            ,public SyslogHandler_Base
	{
    private:
        ::comphelper::ComponentContext  m_aContext;
        LogHandlerHelper                m_aHandlerHelper;
        ::osl::SocketAddr               m_aSocketAddress;
        ::osl::DatagramSocket           m_aSocket;

    protected:
        SyslogHandler( const Reference< XComponentContext >& _rxContext );
        virtual ~SyslogHandler();

        // XLogHandler
        virtual ::rtl::OUString SAL_CALL getEncoding() throw (RuntimeException);
        virtual void SAL_CALL setEncoding( const ::rtl::OUString& _encoding ) throw (RuntimeException);
        virtual Reference< XLogFormatter > SAL_CALL getFormatter() throw (RuntimeException);
        virtual void SAL_CALL setFormatter( const Reference< XLogFormatter >& _formatter ) throw (RuntimeException);
        virtual ::sal_Int32 SAL_CALL getLevel() throw (RuntimeException);
        virtual void SAL_CALL setLevel( ::sal_Int32 _level ) throw (RuntimeException);
        virtual void SAL_CALL flush(  ) throw (RuntimeException);
        virtual ::sal_Bool SAL_CALL publish( const LogRecord& Record ) throw (RuntimeException);

        // XInitialization
        virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

        // XServiceInfo
		virtual ::rtl::OUString SAL_CALL getImplementationName() throw(RuntimeException);
        virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& _rServiceName ) throw(RuntimeException);
        virtual Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw(RuntimeException);

        // OComponentHelper
        virtual void SAL_CALL disposing();

    public:
        // XServiceInfo - static version
		static ::rtl::OUString SAL_CALL getImplementationName_static();
        static Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_static();
        static Reference< XInterface > Create( const Reference< XComponentContext >& _rxContext );

    public:
        typedef ComponentMethodGuard< SyslogHandler > MethodGuard;
        void    enterMethod( MethodGuard::Access );
        void    leaveMethod( MethodGuard::Access );
	};

    //====================================================================
	//= SyslogHandler - implementation
	//====================================================================
	//--------------------------------------------------------------------
    SyslogHandler::SyslogHandler( const Reference< XComponentContext >& _rxContext )
        :SyslogHandler_Base( m_aMutex )
        ,m_aContext( _rxContext )
        ,m_aHandlerHelper( _rxContext, m_aMutex, rBHelper )
        ,m_aSocketAddress()
        ,m_aSocket()
    {
    }

    //--------------------------------------------------------------------
    SyslogHandler::~SyslogHandler()
    {
        if ( !rBHelper.bDisposed )
        {
            acquire();
            dispose();
        }
    }

    //--------------------------------------------------------------------
    void SAL_CALL SyslogHandler::disposing()
    {
        m_aHandlerHelper.setFormatter( NULL );
    }

    //--------------------------------------------------------------------
    void SyslogHandler::enterMethod( MethodGuard::Access )
    {
        m_aHandlerHelper.enterMethod();
    }

    //--------------------------------------------------------------------
    void SyslogHandler::leaveMethod( MethodGuard::Access )
    {
        m_aMutex.release();
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL SyslogHandler::getEncoding() throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        ::rtl::OUString sEncoding;
        OSL_VERIFY( m_aHandlerHelper.getEncoding( sEncoding ) );
        return sEncoding;
    }
    
    //--------------------------------------------------------------------
    void SAL_CALL SyslogHandler::setEncoding( const ::rtl::OUString& _rEncoding ) throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        OSL_VERIFY( m_aHandlerHelper.setEncoding( _rEncoding ) );
    }
    
    //--------------------------------------------------------------------
    Reference< XLogFormatter > SAL_CALL SyslogHandler::getFormatter() throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        return m_aHandlerHelper.getFormatter();
    }
    
    //--------------------------------------------------------------------
    void SAL_CALL SyslogHandler::setFormatter( const Reference< XLogFormatter >& _rxFormatter ) throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        m_aHandlerHelper.setFormatter( _rxFormatter );
    }
    
    //--------------------------------------------------------------------
    ::sal_Int32 SAL_CALL SyslogHandler::getLevel() throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        return m_aHandlerHelper.getLevel();
    }
    
    //--------------------------------------------------------------------
    void SAL_CALL SyslogHandler::setLevel( ::sal_Int32 _nLevel ) throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        m_aHandlerHelper.setLevel( _nLevel );
    }
    
    //--------------------------------------------------------------------
    void SAL_CALL SyslogHandler::flush(  ) throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        fflush( stdout );
        fflush( stderr );
    }
    
    //--------------------------------------------------------------------
    ::sal_Bool SAL_CALL SyslogHandler::publish( const LogRecord& _rRecord ) throw (RuntimeException)
    {
        MethodGuard aGuard( *this );

        ::rtl::OString sEntry;
        if ( !m_aHandlerHelper.formatForPublishing( _rRecord, sEntry ) )
            return sal_False;

        ::rtl::OStringBuffer buffer;
        // PRI
        const sal_Int32 facility = 1; // USER
        sal_Int32 severity;
        switch ( _rRecord.Level )
        {
            case LogLevel::SEVERE:
                severity = 3; // error
                break;
            case LogLevel::WARNING:
                severity = 4; // warning
                break;
            case LogLevel::INFO:
            case LogLevel::CONFIG:
                severity = 5; // notice
                break;
            default:
                severity = 7; // debug
        }
        buffer.append( '<' );
        buffer.append( ( facility * 8 ) + severity );
        buffer.append( '>' );
        
        // VERSION
        buffer.append( '1' );
        buffer.append( ' ' );        
        
        // TIMESTAMP
        char timestampBuffer[256];
        snprintf( timestampBuffer, sizeof( timestampBuffer ),
            "%04i-%02i-%02iT%02i:%02i:%02i.%02iZ",
            (int)_rRecord.LogTime.Year, (int)_rRecord.LogTime.Month, (int)_rRecord.LogTime.Day,
            (int)_rRecord.LogTime.Hours, (int)_rRecord.LogTime.Minutes, (int)_rRecord.LogTime.Seconds,
            (int)_rRecord.LogTime.HundredthSeconds );
        buffer.append( timestampBuffer );
        buffer.append( ' ' );
        
        // HOSTNAME
        ::rtl::OUString hostname = ::osl::SocketAddr::getLocalHostname( 0 );
        if ( !hostname.isEmpty() )
            buffer.append( ::rtl::OUStringToOString( hostname, RTL_TEXTENCODING_UTF8 ) );
        else
            buffer.append( '-' );
        buffer.append( ' ' );
        
        // APP-NAME
        buffer.append( "soffice" );
        buffer.append( ' ' );
        
        // PROC-ID
        oslProcessInfo pInfo;
        pInfo.Size = sizeof(oslProcessInfo);
        oslProcessError prerr = osl_getProcessInfo( NULL, osl_Process_IDENTIFIER, &pInfo );
        if ( prerr == osl_Process_E_None )
            buffer.append( (sal_Int64) pInfo.Ident );
        else
            buffer.append( '-' );
        buffer.append( ' ' );
        
        // MESSAGE-ID
        buffer.append( '-' );
        buffer.append( ' ' );
        
        // STRUCTURED DATA
        buffer.append( '-' );
        buffer.append( ' ' );
        
        // MESSAGE
        buffer.append( sEntry );
        
        sal_Int32 sockRes = m_aSocket.sendTo( m_aSocketAddress, buffer.getStr(), buffer.getLength() );

        return sockRes == osl_Socket_MsgNormal;
    }

    //--------------------------------------------------------------------
    void SAL_CALL SyslogHandler::initialize( const Sequence< Any >& _rArguments ) throw (Exception, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if ( m_aHandlerHelper.getIsInitialized() )
            throw AlreadyInitializedException();

        if ( _rArguments.getLength() != 1 )
            throw IllegalArgumentException( ::rtl::OUString(), *this, 1 );

        Sequence< NamedValue > aSettings;
        if ( !( _rArguments[0] >>= aSettings ) )
            throw IllegalArgumentException( ::rtl::OUString(), *this, 1 );

        // createWithSettings( [in] sequence< ::com::sun::star::beans::NamedValue > Settings )
        ::comphelper::NamedValueCollection aTypedSettings( aSettings );
        m_aHandlerHelper.initFromSettings( aTypedSettings );

        ::rtl::OUString host;
        if ( !aTypedSettings.get_ensureType( "Host", host ) )
            throw IllegalArgumentException( ::rtl::OUString(), *this, 1 );
        sal_Int32 port;
        if ( !aTypedSettings.get_ensureType( "Port", port ) )
            throw IllegalArgumentException( ::rtl::OUString(), *this, 1 );
        ::osl::SocketAddr address( host, port );
        if ( address.is() )
            m_aSocketAddress = address;
        else
            throw IllegalArgumentException( ::rtl::OUString(), *this, 1 );

        m_aHandlerHelper.setIsInitialized();
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL SyslogHandler::getImplementationName() throw(RuntimeException)
    {
        return getImplementationName_static();
    }
    
    //--------------------------------------------------------------------
    ::sal_Bool SAL_CALL SyslogHandler::supportsService( const ::rtl::OUString& _rServiceName ) throw(RuntimeException)
    {
        const Sequence< ::rtl::OUString > aServiceNames( getSupportedServiceNames() );
        for (   const ::rtl::OUString* pServiceNames = aServiceNames.getConstArray();
                pServiceNames != aServiceNames.getConstArray() + aServiceNames.getLength();
                ++pServiceNames
            )
            if ( _rServiceName == *pServiceNames )
                return sal_True;
        return sal_False;
    }
    
    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL SyslogHandler::getSupportedServiceNames() throw(RuntimeException)
    {
        return getSupportedServiceNames_static();
    }
    
    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL SyslogHandler::getImplementationName_static()
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.extensions.SyslogHandler" ) );
    }
    
    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL SyslogHandler::getSupportedServiceNames_static()
    {
        Sequence< ::rtl::OUString > aServiceNames(1);
        aServiceNames[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.logging.SyslogHandler" ) );
        return aServiceNames;
    }

    //--------------------------------------------------------------------
    Reference< XInterface > SyslogHandler::Create( const Reference< XComponentContext >& _rxContext )
    {
        return *( new SyslogHandler( _rxContext ) );
    }

    //--------------------------------------------------------------------
    void createRegistryInfo_SyslogHandler()
    {
        static OAutoRegistration< SyslogHandler > aAutoRegistration;
    }

//........................................................................
} // namespace logging
//........................................................................
