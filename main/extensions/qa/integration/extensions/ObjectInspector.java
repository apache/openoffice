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



package integration.extensions;

import com.sun.star.uno.XComponentContext;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.lang.XMultiServiceFactory;

import com.sun.star.frame.*;
import com.sun.star.inspection.*;
import com.sun.star.beans.*;

import integration.extensions.Frame;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import org.openoffice.test.OfficeConnection;

public class ObjectInspector
{
    private static final OfficeConnection officeConnection = new OfficeConnection();
    private XComponentContext       m_context;
    private XMultiServiceFactory    m_orb;
    private Frame                   m_desktop;

    final private String    m_inspectorFrameName = new String( "ObjectInspector" );

    @BeforeClass
    public static void beforeClass() throws Exception
    {
        officeConnection.setUp();
    }

    @AfterClass
    public static void afterClass() throws Exception
    {
        officeConnection.tearDown();
    }

    /** Creates a new instance of ValueBinding */
    public ObjectInspector()
    {
    }

    /* ------------------------------------------------------------------ */
    @Before
    public void before() throws com.sun.star.uno.Exception, java.lang.Exception
    {
        m_orb = UnoRuntime.queryInterface(XMultiServiceFactory.class, officeConnection.getComponentContext().getServiceManager());
        m_context = (XComponentContext)UnoRuntime.queryInterface( XComponentContext.class,
                ((XPropertySet)UnoRuntime.queryInterface( XPropertySet.class, m_orb )).getPropertyValue( "DefaultContext" ) );
        m_desktop = new Frame( m_orb.createInstance( "com.sun.star.frame.Desktop" ) );
    }

    /* ------------------------------------------------------------------ */
    @After
    public void after() throws com.sun.star.uno.Exception, java.lang.Exception
    {
        closeExistentInspector();
    }

    /* ------------------------------------------------------------------ */
    @Test
    public void interactiveObjectInspector() throws com.sun.star.uno.Exception, java.lang.Exception
    {
        closeExistentInspector();

        // the to-be-inspected object
        XFrame inspectee = m_desktop.getActiveFrame();

        // the inspector
        XObjectInspector inspector = createObjectInspector();

        // do inspect
        inspector.inspect( new Object[] { inspectee } );

        ConsoleWait keyWaiter = new ConsoleWait( inspector );
        keyWaiter.waitForUserInput();
    }

    /* ------------------------------------------------------------------ */
    private XObjectInspector createObjectInspector() throws com.sun.star.uno.Exception
    {
        com.sun.star.awt.XWindow floatingWindow = createFloatingWindow();

        Frame inspectorFrame = new Frame( m_orb.createInstance( "com.sun.star.frame.Frame" ) );
        inspectorFrame.setName( m_inspectorFrameName );
        inspectorFrame.initialize( floatingWindow );
        m_desktop.getFrames().append( inspectorFrame.getXFrame() );

        // handler factories:
        Object[] handlerFactories = new Object[] {
            "com.sun.star.inspection.GenericPropertyHandler",
            new ComponentFactory( ServicesHandler.class ),
            new ComponentFactory( MethodHandler.class )
        };
        // a model
        XObjectInspectorModel model = ObjectInspectorModel.createWithHandlerFactoriesAndHelpSection(
            m_context, handlerFactories, 4, 4 );

	// create the ObjectInspector
        XObjectInspector inspector = com.sun.star.inspection.ObjectInspector.createWithModel(
            m_context, model );

        // add an observer which will emit help texts
        new HelpTextProvider( inspector.getInspectorUI() );

        // plug it into the frame
        inspector.attachFrame( inspectorFrame.getXFrame() );

        // make the window visible
	floatingWindow.setVisible( true );

        // outta here
        return inspector;
    }

    /* ------------------------------------------------------------------ */
    private void closeExistentInspector()
    {
        Frame existentInspectorFrame = new Frame( m_desktop.findFrame( m_inspectorFrameName, 255 ) );
        if ( existentInspectorFrame != null )
        {
            try
            {
                existentInspectorFrame.close( true );
            }
            catch( com.sun.star.util.CloseVetoException e )
            {
                fail( "could not close the existent inspector frame" );
            }
        }
    }

    /* ------------------------------------------------------------------ */
    private com.sun.star.awt.XWindow createFloatingWindow() throws com.sun.star.uno.Exception
    {
        com.sun.star.awt.XToolkit toolkit = (com.sun.star.awt.XToolkit)UnoRuntime.queryInterface(
                com.sun.star.awt.XToolkit.class, m_orb.createInstance( "com.sun.star.awt.Toolkit" ) );

        com.sun.star.awt.WindowDescriptor windowDescriptor = new com.sun.star.awt.WindowDescriptor();
        windowDescriptor.Type = com.sun.star.awt.WindowClass.TOP;
        windowDescriptor.WindowServiceName = "modelessdialog";  // "floatingwindow" would need a parent
        windowDescriptor.ParentIndex       =  -1;
        //windowDescriptor.Parent           =  null;

        windowDescriptor.Bounds              = new com.sun.star.awt.Rectangle( 500, 100,  400, 600 );
        windowDescriptor.WindowAttributes    =  com.sun.star.awt.WindowAttribute.BORDER
                                            +   com.sun.star.awt.WindowAttribute.MOVEABLE
                                            +   com.sun.star.awt.WindowAttribute.SIZEABLE
                                            +   com.sun.star.awt.WindowAttribute.CLOSEABLE
                                            +   com.sun.star.awt.VclWindowPeerAttribute.CLIPCHILDREN;

	return (com.sun.star.awt.XWindow)UnoRuntime.queryInterface( com.sun.star.awt.XWindow.class,
                toolkit.createWindow( windowDescriptor ) );
    }
}
