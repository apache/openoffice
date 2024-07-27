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

import com.sun.star.uno.UnoRuntime;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.beans.XPropertySet;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XText;
import com.sun.star.text.XTextTable;
import com.sun.star.text.XTextCursor;
import com.sun.star.form.binding.XValueBinding;
import com.sun.star.form.binding.XBindableValue;

import integration.forms.DocumentHelper;
import integration.forms.TableCellTextBinding;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import org.openoffice.test.OfficeConnection;


public class ValueBinding extends integration.forms.TestCase
{
    private static final OfficeConnection officeConnection = new OfficeConnection();
    private XMultiServiceFactory    m_orb = null;

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
    public ValueBinding()
    {
        super( DocumentType.WRITER );
    }

    /* ------------------------------------------------------------------ */
    @Before
    public void before() throws com.sun.star.uno.Exception, java.lang.Exception
    {
        m_orb = UnoRuntime.queryInterface(XMultiServiceFactory.class, officeConnection.getComponentContext().getServiceManager());
        prepareDocument();
    }

    /* ------------------------------------------------------------------ */
    @After
    public void after() throws com.sun.star.uno.Exception, java.lang.Exception
    {
        super.waitForUserInput();
        super.closeDocument();
    }

    /* ------------------------------------------------------------------ */
    @Test
    public void checkBindingProperties() throws com.sun.star.uno.Exception, java.lang.Exception
    {
    }

    /* ------------------------------------------------------------------ */
    protected void prepareDocument() throws com.sun.star.uno.Exception, java.lang.Exception
    {
        super.prepareDocument(m_orb);

        // insert a table with exactly one cell. The content of this table will be synced with
        // the content of a form control
        XTextDocument textDoc = (XTextDocument)UnoRuntime.queryInterface( XTextDocument.class,  m_document.getDocument() );
        XText documentText = textDoc.getText();
        XTextCursor textCursor = documentText.createTextCursor();

        XTextTable table = (XTextTable)UnoRuntime.queryInterface( XTextTable.class,
            m_document.createInstance( "com.sun.star.text.TextTable" )
        );
        table.initialize( 1, 1 );
        documentText.insertTextContent( textCursor, table, false );

        // insert our sample control
        XPropertySet textControl = m_formLayer.insertControlLine( "DatabaseTextField", "Test", "", 10 );

        // create a value binding for the first cell of the table
        XValueBinding cellBinding = new TableCellTextBinding( table.getCellByName( "A1" ) );
        // and bind it to the control
        XBindableValue bindable = (XBindableValue)UnoRuntime.queryInterface(
            XBindableValue.class, textControl
        );
        bindable.setValueBinding( cellBinding );

        // toggle the view to alive mode
        m_document.getCurrentView( ).toggleFormDesignMode( );
    }
 }
