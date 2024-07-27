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

import com.sun.star.awt.XListBox;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XChild;
import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XNameAccess;
import com.sun.star.form.ListSourceType;
import com.sun.star.form.runtime.FormFeature;
import com.sun.star.form.runtime.XFormController;
import com.sun.star.form.runtime.XFormOperations;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sdb.CommandType;
import com.sun.star.sdbc.SQLException;
import com.sun.star.sdbc.XParameters;
import com.sun.star.sdbc.XPreparedStatement;
import com.sun.star.uno.Exception;
import com.sun.star.uno.UnoRuntime;
import connectivity.tools.HsqlColumnDescriptor;
import connectivity.tools.HsqlDatabase;
import connectivity.tools.HsqlTableDescriptor;
import connectivity.tools.sdb.Connection;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import org.openoffice.test.OfficeConnection;


public class ListBox extends TestCase
{
    private static final OfficeConnection officeConnection = new OfficeConnection();
    private XMultiServiceFactory    m_orb = null;
    HsqlDatabase    m_database = null;
    private final String    m_foreignKeyTableName = "foreign_keys";

    @BeforeClass
    public static void beforeClass() throws java.lang.Exception
    {
        officeConnection.setUp();
    }

    @AfterClass
    public static void afterClass() throws java.lang.Exception
    {
        officeConnection.tearDown();
    }

    public ListBox()
    {
        super( DocumentType.WRITER );
    }

    /* ------------------------------------------------------------------ */
    @Test
    public void checkForeignKeys() throws com.sun.star.uno.Exception, java.lang.Exception
    {
        try
        {
            // create the form document
            prepareDocument();

            final XIndexAccess formsCollection = UnoRuntime.queryInterface( XIndexAccess.class,
                m_document.getFormComponentTreeRoot() );
            final XNameAccess form = UnoRuntime.queryInterface( XNameAccess.class, formsCollection.getByIndex(0) );

            final DocumentViewHelper view = m_document.getCurrentView();
            final XFormController formController = view.getFormController( form );
            final XFormOperations formOperations = formController.getFormOperations();

            // move through all records, and check that the display values in the list boxes are as expected
            final String[][] fieldTypesDefinitions = impl_getFieldTypeDefinitions();
            final String[] fieldTypes = fieldTypesDefinitions[0];

            final String[] displayValues = impl_getDisplayValues();

            formOperations.execute( FormFeature.MoveToFirst );
            for ( int row=0; row<2; ++row )
            {
                StringBuffer failedFieldTypes = new StringBuffer();
                for ( int i=0; i<fieldTypes.length; ++i )
                {
                    final String columnFKName = fieldTypes[i] + "_fk";
                    Object listBoxModel = form.getByName( columnFKName );
                    XListBox listBoxControl = UnoRuntime.queryInterface(  XListBox.class,
                        view.getControl( listBoxModel ) );
                    if ( !listBoxControl.getSelectedItem().equals( displayValues[row] ) )
                    {
                        if ( failedFieldTypes.length() > 0 )
                            failedFieldTypes.append( ", " );
                        failedFieldTypes.append( fieldTypes[i] );
                    }
                }
                /*assertTrue( "The following field types do not work when used as bound list box fields: " + failedFieldTypes.toString() +
                        " (row " + row + ")", failedFieldTypes.length() == 0 );*/

                formOperations.execute( FormFeature.MoveToNext );
            }

        }
        finally
        {
            closeDocument();
        }
    }

    /* ------------------------------------------------------------------ */
    @Before
    public void before() throws Exception, java.lang.Exception
    {
        m_orb = UnoRuntime.queryInterface(XMultiServiceFactory.class, officeConnection.getComponentContext().getServiceManager());
        impl_createDatabase();
    }

    /* ------------------------------------------------------------------ */
    protected void prepareDocument() throws com.sun.star.uno.Exception, java.lang.Exception
    {
        super.prepareDocument(m_orb);
        impl_createForm();
    }

    /* ------------------------------------------------------------------ */
    private String[][] impl_getFieldTypeDefinitions()
    {
        return new String[][] {
            new String[] {
                "bigint", "boolean", "date", "decimal", "double", "float", "numeric", "time", "timestamp", "tinyint", "varchar"
            },
            new String[] {
                null, null, null, "(10,2)", null, null, "(10,2)", null, null, null, "(50)"
            }
        };
    }

    /* ------------------------------------------------------------------ */
    private String[] impl_getTypedValue( final String _asType, final int _rowNum ) throws SQLException
    {
        Map< String, String[] > valueMap = new HashMap< String, String[] >();
        valueMap.put( "bigint",     new String[] { "1111111111", "222222222" } );
        valueMap.put( "boolean",    new String[] { "false", "true" } );
        valueMap.put( "date",       new String[] { "2001-01-01", "2002-02-02" } );
        valueMap.put( "decimal",    new String[] { "1.11", "2.22" } );
        valueMap.put( "double",     new String[] { "1.11", "2.22" } );
        valueMap.put( "float",      new String[] { "1.11", "2.22" } );
        valueMap.put( "numeric",    new String[] { "1.11", "2.22" } );
        valueMap.put( "time",       new String[] { "01:01:01", "02:02:02" } );
        valueMap.put( "timestamp",  new String[] { "2001-01-01 01:01:01", "2002-02-02 02:02:02" } );
        valueMap.put( "tinyint",    new String[] { "1", "2" } );
        valueMap.put( "varchar",    new String[] { "first", "second" } );

        return valueMap.get( _asType );
    }

    /* ------------------------------------------------------------------ */
    private String[] impl_getDisplayValues()
    {
        return new String[] { "one", "two" };
    }

    /* ------------------------------------------------------------------ */
    private void impl_createDatabase() throws java.lang.Exception
    {
        try
        {
            m_database = new HsqlDatabase( m_orb );
            Connection connection = m_database.defaultConnection();
            System.out.println( m_database.getDocumentURL() );

            final String[][] fieldDefinitions = impl_getFieldTypeDefinitions();
            final String[] keyTypes = fieldDefinitions[0];
            final String[] keyCreationArgs = fieldDefinitions[1];

            Vector< HsqlColumnDescriptor > foreignKeyColumns = new Vector< HsqlColumnDescriptor >();
            foreignKeyColumns.add( new HsqlColumnDescriptor( "ID", "integer", HsqlColumnDescriptor.PRIMARY ) );

            Vector< String[] > foreignKeyValues = new Vector< String[] >();

            StringBuffer foreignKeyInsertSQL = new StringBuffer();
            foreignKeyInsertSQL.append( "INSERT INTO \"" + m_foreignKeyTableName + "\" VALUES (?" );

            final String[] displayValues = impl_getDisplayValues();

            for ( int i=0; i<keyTypes.length; ++i )
            {
                final String tableName = keyTypes[i] + "_pk";
                final String columnPKName = keyTypes[i] + "_pk";
                final String columnFKName = keyTypes[i] + "_fk";
                final String columnType = keyTypes[i] + ( keyCreationArgs[i] != null ? keyCreationArgs[i] : "" );
                m_database.createTable( new HsqlTableDescriptor( tableName,
                    new HsqlColumnDescriptor[] {
                        new HsqlColumnDescriptor( columnPKName, columnType, HsqlColumnDescriptor.PRIMARY ),
                        new HsqlColumnDescriptor( "content", "varchar(50)" )
                    }
                ) );

                // insert a few rows
                StringBuffer sql = new StringBuffer();
                sql.append( "INSERT INTO \"" );
                sql.append( tableName );
                sql.append( "\" VALUES (?, ?)");
                XPreparedStatement statement = connection.prepareStatement( sql.toString() );
                XParameters statementParameters = UnoRuntime.queryInterface( XParameters.class, statement );

                final String[] keyValues = impl_getTypedValue( keyTypes[i], 0 );

                for ( int row=0; row<displayValues.length; ++row )
                {
                    statementParameters.setString( 1, keyValues[row] );
                    statementParameters.setString( 2, displayValues[row] );
                    statement.execute();
                }

                // remember a column descriptor for later creation of the table with the foreign keys
                foreignKeyColumns.add(  new HsqlColumnDescriptor( columnFKName, columnType, HsqlColumnDescriptor.REQUIRED,
                    tableName, columnPKName ) );

                // remember the data to fill into this table
                foreignKeyValues.add( keyValues );
                foreignKeyInsertSQL.append( ", ?" );
            }

            // create the table taking all those foreign keys
            m_database.createTable( new HsqlTableDescriptor( m_foreignKeyTableName, foreignKeyColumns.toArray( new HsqlColumnDescriptor[0] ) ) );
            // fill in some data
            foreignKeyInsertSQL.append( ")" );
            XPreparedStatement statement = connection.prepareStatement( foreignKeyInsertSQL.toString() );
            XParameters statementParameters = UnoRuntime.queryInterface( XParameters.class, statement );
            for ( int row=0; row<2; ++row )
            {
                statementParameters.setInt( 1, row );
                for ( int i=0; i<keyTypes.length; ++i )
                {
                    statementParameters.setString( i+2, foreignKeyValues.get(i)[row] );
                }
                statement.execute();
            }

            m_database.defaultConnection().refreshTables();
        }
        finally
        {
            if ( m_database != null )
                m_database.store();
        }
    }

    /* ------------------------------------------------------------------ */
    private void impl_createForm() throws java.lang.Exception
    {
        // a single control for the ID field
        XPropertySet controlModel = m_formLayer.insertControlLine( "DatabaseNumericField", "ID", null, 10, 10, 6 );
        // bind the form to the foreign_keys table
        XPropertySet form = dbfTools.queryPropertySet( dbfTools.getParent( controlModel, XChild.class ) );
        form.setPropertyValue( "Command", m_foreignKeyTableName );
        form.setPropertyValue( "CommandType", CommandType.TABLE );
        form.setPropertyValue( "DataSourceName", m_database.getDocumentURL() );

        // create list boxes for the different foreign keys
        final String[][] fieldDefinitions = impl_getFieldTypeDefinitions();
        final String[] fieldTypes = fieldDefinitions[0];
        for ( int i=0; i<fieldTypes.length; ++i )
        {
            final String tableName = fieldTypes[i] + "_pk";
            final String columnFKName = fieldTypes[i] + "_fk";
            final String columnPKName = fieldTypes[i] + "_pk";
            XPropertySet listBoxModel = m_formLayer.insertControlLine( "DatabaseListBox", columnFKName, null, 10, 20 + 10*i, 6 );
            listBoxModel.setPropertyValue( "Dropdown", new Boolean( true ) );
            listBoxModel.setPropertyValue( "ListSourceType", ListSourceType.SQL );
            listBoxModel.setPropertyValue( "ListSource", new String[] { "SELECT \"content\", \"" + columnPKName +
                "\" FROM \"" + tableName + "\"" } );
            listBoxModel.setPropertyValue( "BoundColumn", new Short( (short)1 ) );
        }

        m_document.getCurrentView().toggleFormDesignMode();
    }
 }
