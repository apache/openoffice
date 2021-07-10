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



package com.sun.star.beans;

import java.awt.Component;

import com.sun.star.lang.EventObject;
import com.sun.star.lang.SystemDependent;
import com.sun.star.lang.XEventListener;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.awt.Rectangle;
import com.sun.star.awt.XWindow;
import com.sun.star.awt.XWindowPeer;
import com.sun.star.awt.XVclWindowPeer;
import com.sun.star.awt.XToolkit;
import com.sun.star.awt.WindowDescriptor;
import com.sun.star.awt.WindowAttribute;
import com.sun.star.awt.WindowClass;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;

/**
 * This class represents a local office window.
 * @deprecated
 */
public class LocalOfficeWindow
	extends java.awt.Canvas
	implements OfficeWindow, XEventListener
{
	private transient OfficeConnection	mConnection;
	private transient XWindowPeer		mParentProxy;
	private transient XWindowPeer		mWindow;
	private boolean 			bPeer = false;

	/**
	 * Constructor.
	 *
	 * @param connection The office connection object the window
	 *	belongs to.
	 */
	/* package */ LocalOfficeWindow(OfficeConnection connection)
	{
		mConnection	= connection;
		mConnection.addEventListener((XEventListener)this);
	}

	/**
	 * Retrieves an AWT component object associated with the OfficeWindow.
	 *
	 * @return The AWT component object associated with the OfficeWindow.
	 */
	public Component getAWTComponent()
	{
		return this;
	}

	/**
	 * Retrieves an UNO XWindowPeer object associated with the OfficeWindow.
	 *
	 * @return The UNO XWindowPeer object associated with the OfficeWindow.
	 */
	public XWindowPeer getUNOWindowPeer()
	{
		if (mWindow == null)
			createUNOWindowPeer();
		return mWindow;
	}

	/**
	 * Receives a notification about the connection has been closed.
	 * This method has to set the connection to <code>null</code>.
	 *
	 * @source The event object.
	 */
	public void disposing(EventObject source)
	{
		// the window will be disposed by the framework
		mWindow = null;
		mConnection	= null;
	}

	/**
	* Returns an AWT toolkit.
        */
       private XToolkit queryAWTToolkit()
               throws com.sun.star.uno.Exception
       {
			// Create a UNO toolkit.
			XMultiComponentFactory compfactory;
			XComponentContext xContext = mConnection.getComponentContext();
			if ( xContext != null )
			{
				compfactory     = mConnection.getComponentContext().getServiceManager();
				XMultiServiceFactory    factory;
				factory = (XMultiServiceFactory)UnoRuntime.queryInterface(
						XMultiServiceFactory.class, compfactory);
				Object          object = factory.createInstance( "com.sun.star.awt.Toolkit");
				return (XToolkit)UnoRuntime.queryInterface(XToolkit.class, object);
			}
			else
				return null;
       }

       	/// called when system parent is available, reparents the bean window
	private void aquireSystemWindow()
	{
		if ( !bPeer )
		{
			// set real parent
			XVclWindowPeer xVclWindowPeer = (XVclWindowPeer)UnoRuntime.queryInterface(
                               XVclWindowPeer.class, mWindow);
			xVclWindowPeer.setProperty( "PluginParent", getNativeWindow() );
			bPeer = true;

		       	// show document window
			XWindow aWindow = (XWindow)UnoRuntime.queryInterface(XWindow.class, mWindow);
			aWindow.setVisible( true );
		}
	}

       	/// called when system parent is about to die, reparents the bean window
	private void releaseSystemWindow()
	{
		if ( bPeer )
		{
		       	// hide document window
			XWindow aWindow = (XWindow)UnoRuntime.queryInterface(XWindow.class, mWindow);
			aWindow.setVisible( false );

			// set null parent
			XVclWindowPeer xVclWindowPeer = (XVclWindowPeer)UnoRuntime.queryInterface(
                               XVclWindowPeer.class, mWindow);
			xVclWindowPeer.setProperty( "PluginParent", 0L );
			bPeer = false;
		}
	}

       /// callback handler to get to know when we become visible
        //@deprecated
       class ComponentEventHandler
               extends java.awt.event.ComponentAdapter
       {
		public void componentHidden( java.awt.event.ComponentEvent e)
		{
			// only when we become invisible, we might lose our system window
			CallWatchThread aCallWatchThread = new CallWatchThread( 500 );
			setVisible(false);
			try { aCallWatchThread.cancel(); }
			catch ( java.lang.InterruptedException aExc )
			{} // ignore
		}

		public void componentShown( java.awt.event.ComponentEvent e)
		{
			// only when we become visible, we get a system window
		 	aquireSystemWindow();
		}
	}

	/// Overriding java.awt.Component.setVisible() due to Java bug (no showing event).
	public void setVisible( boolean b )
	{
		super.setVisible(b);

		// Java-Bug: componentShown() is never called :-(
		// is still at least in Java 1.4.1_02
		if ( b )
			aquireSystemWindow();
		else
			releaseSystemWindow();
	}

       /** Factory method for a UNO AWT toolkit window as a child of this Java window.
	*
	*/
       private XWindowPeer createUNOWindowPeer()
       {
		try
		{
			// get this windows native window type
                        int type = getNativeWindowSystemType();

                        // Java AWT windows only have a system window when showing.
                        XWindowPeer parentPeer;
                        if ( isShowing() )
                        {
				// create direct parent relationship
				//setVisible( true );
                                parentPeer = new JavaWindowPeerFake( getNativeWindow(), type);
				bPeer = true;
                        }
                        else
			{
				// no parent yet
				parentPeer = null;
				bPeer = false;
			}

			// create native window (mWindow)
			Rectangle aRect = new Rectangle( 0, 0, 20, 20 );
			WindowDescriptor desc = new WindowDescriptor();
			desc.Type = WindowClass.TOP;
			desc.Parent = parentPeer;
			desc.Bounds = aRect;
			desc.WindowServiceName = "workwindow";
			desc.WindowAttributes = (type == SystemDependent.SYSTEM_WIN32)
				? WindowAttribute.SHOW : 0;
			mWindow	= queryAWTToolkit().createWindow(desc);

			// to get notified when we become visible
			addComponentListener( new ComponentEventHandler() );

			// set initial visibility
						XWindow aWindow = (XWindow)UnoRuntime.queryInterface(XWindow.class, mWindow);
			aWindow.setVisible( bPeer );
		}
		catch (com.sun.star.uno.Exception exp) {
		}

		return mWindow;
	}

	/**
	 * Retrieves a platform dependent system window identifier.
	 *
	 * @return The system window identifier.
	 */
	private native long getNativeWindow();

	/**
	 * Retrieves a platform dependent system window type.
	 *
	 * @return The system window type.
	 */
	private native int getNativeWindowSystemType();

        //---------------------------------------------------------------------------
        /** Helper class to watch calls into AOO with a timeout.
         * @deprecated
         */
        class CallWatchThread extends Thread
        {
                Thread aWatchedThread;
                long nTimeout;

                CallWatchThread( long nTimeout )
                {
                        this.aWatchedThread = Thread.currentThread();
                        this.nTimeout = nTimeout;
                        start();
                }

                void cancel()
                        throws java.lang.InterruptedException
                {
                        Thread aThread = aWatchedThread;
                        aWatchedThread = null;
                        stop();

                        if ( aThread.interrupted() )
                                throw new InterruptedException();
                }

                public void run()
                {
                        while ( aWatchedThread != null )
                        {
                                try { sleep( nTimeout ); }
                                catch ( java.lang.InterruptedException aExc )
                                {}

                                // synchronized
                                {
                                        if ( aWatchedThread != null )
                                        {
                                                aWatchedThread.interrupt();
                                        }
                                }
                        }
                }
        };

}

