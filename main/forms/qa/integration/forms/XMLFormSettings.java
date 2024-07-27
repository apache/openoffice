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



package integration.forms;

import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.form.binding.IncompatibleTypesException;
import com.sun.star.form.binding.XBindableValue;
import com.sun.star.form.binding.XValueBinding;
import com.sun.star.frame.XStorable;
import com.sun.star.io.IOException;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.CloseVetoException;
import com.sun.star.util.XCloseable;
import com.sun.star.xml.dom.XNode;
import com.sun.star.xsd.DataTypeClass;
import java.io.File;
import org.openoffice.xforms.Instance;
import org.openoffice.xforms.Model;
import org.openoffice.xforms.XMLDocument;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import org.openoffice.test.OfficeConnection;


public class XMLFormSettings
{
    private static final OfficeConnection officeConnection = new OfficeConnection();
    private XMultiServiceFactory    m_orb;
    private XMLDocument             m_document;
    private Model                   m_defaultModel;
    private FormLayer               m_formLayer;
    private XPropertySet            m_stringBinding;
    private XPropertySet            m_booleanBinding;
    private XPropertySet            m_dateBinding;

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

    /** Creates a new instance of XMLFormSettings */
    public XMLFormSettings()
    {
    }
    
    /* ------------------------------------------------------------------ */
    @Before
    public void before() throws java.lang.Exception
    {
        // create the document and assign related members
        m_orb = UnoRuntime.queryInterface(XMultiServiceFactory.class, officeConnection.getComponentContext().getServiceManager());
        m_document = new XMLDocument( m_orb );
        m_formLayer = new FormLayer( m_document );

        // create a simple structure in the DOM tree: an element with two attributes
        String[] modelNames = m_document.getXFormModelNames();
        m_defaultModel = m_document.getXFormModel( modelNames[0] );
        final Instance defaultInstance = m_defaultModel.getDefaultInstance();
        // remove the default root node
        defaultInstance.removeNode( "instanceData" );
        // create test structures
        XNode stringElement = defaultInstance.createElement( "stringElement" );
        XNode booleanAttrib = defaultInstance.createAttribute( stringElement, "booleanAttribute", "true" );
        XNode dateAttrib = defaultInstance.createAttribute( stringElement, "dateAttribute" );

        assertTrue( "booleanAttrib's parent is wrong",
            UnoRuntime.areSame( stringElement, booleanAttrib.getParentNode() ) );
        assertTrue( "dateAttrib's parent is wrong",
            UnoRuntime.areSame( stringElement, dateAttrib.getParentNode() ) );

        // also create bindings for the element and its attributes, of the proper type
        m_stringBinding = m_defaultModel.createBindingForNode( stringElement, DataTypeClass.STRING );
        m_booleanBinding = m_defaultModel.createBindingForNode( booleanAttrib, DataTypeClass.BOOLEAN );
        m_dateBinding = m_defaultModel.createBindingForNode( dateAttrib, DataTypeClass.DATE );

        // TODO: set up the bindings so that the date bindings is relevant if and only if
        // the boolean value is true

        // store the document
        File tempFile = File.createTempFile( "xmlforms", ".odt" );
        tempFile.deleteOnExit();
        String fileURL = tempFile.toURL().toExternalForm();
        XStorable store = (XStorable)UnoRuntime.queryInterface( XStorable.class,
            m_document.getDocument() );
        store.storeAsURL( fileURL, new PropertyValue[] {} );
        assertTrue( "document still modified after saving it", !m_document.isModified() );
    }

    /* ------------------------------------------------------------------ */
    @After
    public void after() throws com.sun.star.uno.Exception, java.lang.Exception
    {
        impl_closeDocument();
    }

    /* ------------------------------------------------------------------ */
    private void impl_closeDocument() throws CloseVetoException
    {
        if ( m_document != null )
        {
            XCloseable closeDoc = (XCloseable)UnoRuntime.queryInterface( XCloseable.class,
                m_document.getDocument() );
            closeDoc.close( true );
        }
    }

    /* ------------------------------------------------------------------ */
    private static void impl_bind( XPropertySet _control, XPropertySet _binding ) throws IncompatibleTypesException
    {
        XBindableValue bindableControl = (XBindableValue)UnoRuntime.queryInterface(
            XBindableValue.class, _control );
        XValueBinding binding = (XValueBinding)UnoRuntime.queryInterface(
            XValueBinding.class, _binding );
        bindableControl.setValueBinding( binding );
    }

    /* ------------------------------------------------------------------ */
    /** checks if master-detail relationships including multiple keys work
     */
    @Test
    public void checkExternalData() throws com.sun.star.uno.Exception, java.lang.Exception
    {
        // some controls
        XPropertySet stringControl = m_formLayer.createLabeledControl(
            "DatabaseTextField", "Task", 10, 10, 6 );
        impl_bind( stringControl, m_stringBinding );

        XPropertySet booleanControl = m_formLayer.createControlAndShape(
            "DatabaseCheckBox", 35, 18, 25, 6 );
        booleanControl.setPropertyValue( "Label", "has due date" );
        impl_bind( booleanControl, m_booleanBinding );

        XPropertySet dateControl = m_formLayer.createControlAndShape(
            "DatabaseDateField", 40, 26, 25, 6 );
        dateControl.setPropertyValue( "Dropdown", new Boolean( true ) );
        impl_bind( dateControl, m_dateBinding );

        m_document.getCurrentView( ).toggleFormDesignMode( );

        // ensure the model is set up as containing "document-internal" data
        m_defaultModel.setIsDocumentInternalData( true );
        assertTrue( "setting up the document to contain 'internal data' failed",
            m_defaultModel.getIsDocumentInternalData() );
        impl_storeDocument();

        // okay, here we go ...
        // what this particular test is about is to check whether we can set up the model
        // so that any changes to any controls bound to any data in this model actually marks
        // the containing document as modified
        m_formLayer.userTextInput( stringControl, "don't break this test" );
        assertTrue( "model data changed, but document is not modified",
            m_document.isModified() );

        // TODO: do this with the other control/binding types, too

        // no the other way round: set up the model to contain "document-external" data
        m_defaultModel.setIsDocumentInternalData( false );
        assertTrue( "setting up the document to contain 'internal data' failed",
            !m_defaultModel.getIsDocumentInternalData() );
        impl_storeDocument();

        // and check that now, changes in the controls / model data are not reflected in
        // document's modified state
        m_formLayer.userTextInput( stringControl, "(or any other test, that is)" );
        assertTrue( "model data changed, but document is modified",
            !m_document.isModified() );

        // .................................................................
        // finally, check whether the flag survives loading and saving
        Model internalDataModel = m_document.addXFormModel( "internalData" );
        internalDataModel.setIsDocumentInternalData( true );
        Model externalDataModel = m_document.addXFormModel( "externalData" );
        externalDataModel.setIsDocumentInternalData( false );

        impl_storeDocument();
        m_document.reload();

        internalDataModel = m_document.getXFormModel( "internalData" );
        externalDataModel = m_document.getXFormModel( "externalData" );

        assertTrue( "setting up a model to contain 'internal data' did not survive reloading",
            internalDataModel.getIsDocumentInternalData() );
        assertTrue( "setting up a model to contain 'external data' did not survive reloading",
            !externalDataModel.getIsDocumentInternalData() );
    }

    /* ------------------------------------------------------------------ */
    /** stores our document
     * @throws com.sun.star.io.IOException
     */
    private void impl_storeDocument() throws IOException
    {
        XStorable store = (XStorable)UnoRuntime.queryInterface( XStorable.class,
            m_document.getDocument() );
        store.store();
        assertTrue( "document still modified after saving it", !m_document.isModified() );
    }
}
