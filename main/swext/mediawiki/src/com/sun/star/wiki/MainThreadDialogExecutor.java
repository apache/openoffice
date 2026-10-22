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



package com.sun.star.wiki;

import com.sun.star.uno.Any;
import com.sun.star.awt.XDialog;
import com.sun.star.awt.XCallback;
import com.sun.star.awt.XRequestCallback;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import java.util.concurrent.Callable;

public class MainThreadDialogExecutor implements XCallback
{
    private Callable<Boolean> m_aTask;
    private XDialog    m_xDialog;
    private boolean    m_bResult = false;
    private boolean    m_bCalled = false;
    private boolean    m_bClose = false;

    /** Runs aTask on the main thread. Dialogs and message boxes must also be created there:
        on macOS a window created on another thread gets no native window and its modal execute never returns. */
    static public boolean Run( XComponentContext xContext, Callable<Boolean> aTask )
    {
        MainThreadDialogExecutor aExecutor = new MainThreadDialogExecutor( aTask );
        return GetCallback( xContext, aExecutor );
    }

    static public boolean Execute( XComponentContext xContext, XDialog xDialog )
    {
        MainThreadDialogExecutor aExecutor = new MainThreadDialogExecutor( xDialog );
        return GetCallback( xContext, aExecutor );
    }

    static public boolean Close( XComponentContext xContext, XDialog xDialog )
    {
        MainThreadDialogExecutor aExecutor = new MainThreadDialogExecutor( xDialog, true );
        return GetCallback( xContext, aExecutor );
    }

    static private boolean GetCallback( XComponentContext xContext, MainThreadDialogExecutor aExecutor )
    {
        try
        {
            if ( aExecutor != null )
            {
                String aThreadName = null;
                Thread aCurThread = Thread.currentThread();
                if ( aCurThread != null )
                    aThreadName = aCurThread.getName();

                if ( aThreadName != null && aThreadName.equals( "com.sun.star.thread.WikiEditorSendingThread" ) )
                {
                    // the main thread should be accessed asynchronously
                    XMultiComponentFactory xFactory = xContext.getServiceManager();
                    if ( xFactory == null )
                        throw new com.sun.star.uno.RuntimeException();

                    XRequestCallback xRequest = UnoRuntime.queryInterface(
                        XRequestCallback.class,
                        xFactory.createInstanceWithContext( "com.sun.star.awt.AsyncCallback", xContext ) );
                    if ( xRequest != null )
                    {
                        xRequest.addCallback( aExecutor, Any.VOID );
                        aExecutor.WaitForCall();
                    }
                }
                else
                {
                    // handle it as a main thread
                    aExecutor.notify( Any.VOID );
                }
            }
        }
        catch( Exception e )
        {
            e.printStackTrace();
        }

        return aExecutor.GetResult();
    }

    private MainThreadDialogExecutor( Callable<Boolean> aTask )
    {
        m_aTask = aTask;
    }

    private MainThreadDialogExecutor( XDialog xDialog )
    {
        m_xDialog = xDialog;
    }

    private MainThreadDialogExecutor( XDialog xDialog, boolean bClose )
    {
        m_xDialog = xDialog;
        m_bClose = true;
        m_bCalled = true; // no waiting, asynchronous closing
    }

    private void WaitForCall()
    {
        boolean bInterrupted = false;
        synchronized( this )
        {
            while( !m_bCalled )
            {
                try
                {
                    wait();
                }
                catch( InterruptedException e )
                {
                    bInterrupted = true;
                }
            }
        }
        if ( bInterrupted )
            Thread.currentThread().interrupt();
    }

    private boolean GetResult()
    {
        return m_bResult;
    }

    public void notify( Object aData )
    {
        try
        {
            CallInMainThread();
        }
        finally
        {
            synchronized( this )
            {
                m_bCalled = true;
                notifyAll();
            }
        }
    }

    private void CallInMainThread()
    {
        if ( m_aTask != null )
        {
            try
            {
                m_bResult = m_aTask.call().booleanValue();
            }
            catch( Exception e )
            {
                e.printStackTrace();
            }
        }
        else if ( m_xDialog != null )
        {
            if ( !m_bClose )
                m_bResult = ( m_xDialog.execute() == 1 );
            else
            {
                try
                {
                    m_xDialog.endExecute();
                }
                catch( Exception e )
                {
                    e.printStackTrace();
                }
                m_bResult = true;
            }
        }
    }
}
