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

package complex.connectivity;

import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sdb.CommandType;
import com.sun.star.sdbc.SQLException;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.Date;
import connectivity.tools.CsvDatabase;
import connectivity.tools.RowSet;
import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import org.openoffice.test.Argument;
import org.openoffice.test.OfficeConnection;

public class FlatFileAccess
{
    public FlatFileAccess()
    {
    }

    @BeforeClass
    public static void beforeClass() throws Exception
    {
        connection.setUp();
    }

    @AfterClass
    public static void afterClass() throws Exception
    {
        connection.tearDown();
    }

    @Before
    public void before() throws Exception
    {
        XMultiServiceFactory xMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, connection.getComponentContext().getServiceManager());
        m_database = new CsvDatabase( xMSF );

        // proper settings
        final XPropertySet dataSourceSettings = m_database.getDataSource().geSettings();
        dataSourceSettings.setPropertyValue( "Extension", "csv" );
        dataSourceSettings.setPropertyValue( "HeaderLine", Boolean.TRUE );
        dataSourceSettings.setPropertyValue( "FieldDelimiter", " " );
        m_database.store();

        // write the table(s) for our test
        final String tableLocation = m_database.getTableFileLocation().getAbsolutePath();
        final PrintWriter tableWriter = new PrintWriter( new FileOutputStream( tableLocation + File.separatorChar + "dates.csv", false ) );
        tableWriter.println( "ID date" );
        tableWriter.println( "1 2013-01-01" );
        tableWriter.println( "2 2012-02-02" );
        tableWriter.println( "3 2011-03-03" );
        tableWriter.close();
    }

    private class EqualityDate extends Date
    {
        EqualityDate( short i_day, short i_month, short i_year )
        {
            super( i_day, i_month, i_year );
        }

        EqualityDate( Date i_date )
        {
            super( i_date.Day, i_date.Month, i_date.Year );
        }

        @Override
        public boolean equals( Object i_compare )
        {
            if ( !( i_compare instanceof Date ) )
                return false;
            return  Day   == ((Date)i_compare).Day
                &&  Month == ((Date)i_compare).Month
                &&  Year  == ((Date)i_compare).Year;
        }
    }

    /**
     * ensures simple SELECTs from our table(s) work, and deliver the expected results
     */
    @Test
    public void testBasicAccess()
    {
        testRowSetResults(
            "SELECT * FROM \"dates\"",
            new RowSetIntGetter(1),
            new Integer[] { 1, 2, 3 },
            "simple select", "wrong IDs"
        );

        testRowSetResults(
            "SELECT * FROM \"dates\"",
            new RowSetDateGetter( 2 ),
            new EqualityDate[] { new EqualityDate( (short)1, (short)1, (short)2013 ),
                                 new EqualityDate( (short)2, (short)2, (short)2012 ),
                                 new EqualityDate( (short)3, (short)3, (short)2011 )
            },
            "simple select", "wrong dates"
        );
        testRowSetResults(
            "SELECT \"date\", \"ID\" FROM \"dates\" ORDER BY \"ID\" DESC",
            new RowSetIntGetter( 2 ),
            new Integer[] { 3, 2, 1 },
            "explicit column selection, sorted by IDs", "wrong IDs"
        );
        testRowSetResults(
            "SELECT * FROM \"dates\" ORDER BY \"date\"",
            new RowSetIntGetter( 1 ),
            new Integer[] { 3, 2, 1 },
            "sorted by date", "wrong IDs"
        );
    }

    /**
     * ensures the basic functionality for selecting calendar functions from a CSV table - this is a prerequisite for
     * later tests.
     */
    @Test
    public void testCalendarFunctions()
    {
        // simple check for proper results of the calendar functions (DATE/MONTH)
        // The * at the first position is crucial here - there was code which wrongly calculated
        // column positions of function columns when * was present in the statement
        testRowSetResults(
            "SELECT \"dates\".*, YEAR( \"date\" ) FROM \"dates\"",
            new RowSetIntGetter( 3 ),
            new Integer[] { 2013, 2012, 2011 },
            "YEAR function", "wrong calculated years"
        );
        testRowSetResults(
            "SELECT \"dates\".*, MONTH( \"date\" ) FROM \"dates\"",
            new RowSetIntGetter( 3 ),
            new Integer[] { 1, 2, 3 },
            "MONTH function", "wrong calculated months"
        );
    }

    /**
     * ensures that sorting by a function column works
     */
    @Test
    public void testSortingByFunction()
    {
        // most simple case: select a function, and sort by it
        testRowSetResults(
            "SELECT YEAR( \"date\" ) AS \"year\" FROM \"dates\" ORDER BY \"year\"",
            new RowSetIntGetter(1),
            new Integer[] { 2011, 2012, 2013 },
            "single YEAR selection, sorted by years", "wrong calculated years"
        );
        // somewhat more "difficult" (this used to crash): Select all columns, plus a function, so the calculated
        // column has a position greater than column count
        testRowSetResults(
            "SELECT \"dates\".*, YEAR( \"date\" ) AS \"year\" FROM \"dates\" ORDER BY \"year\" DESC",
            new RowSetIntGetter(3),
            new Integer[] { 2013, 2012, 2011 },
            "extended YEAR selection, sorted by years", "wrong calculated years"
        );
    }

    private interface RowSetValueGetter
    {
        public Object getValue( final RowSet i_rowSet ) throws SQLException;
    }

    private abstract class RowSetColumnValueGetter implements RowSetValueGetter
    {
        RowSetColumnValueGetter( final int i_columnIndex )
        {
            m_columnIndex = i_columnIndex;
        }

        protected final int m_columnIndex;
    }

    private class RowSetIntGetter extends RowSetColumnValueGetter
    {
        RowSetIntGetter( final int i_columnIndex )
        {
            super( i_columnIndex );
        }

        public Object getValue( final RowSet i_rowSet ) throws SQLException
        {
            return i_rowSet.getInt( m_columnIndex );
        }
    }

    private class RowSetDateGetter extends RowSetColumnValueGetter
    {
        RowSetDateGetter( final int i_columnIndex )
        {
            super( i_columnIndex );
        }

        public Object getValue( final RowSet i_rowSet ) throws SQLException
        {
            return i_rowSet.getDate( m_columnIndex );
        }
    }

    private <T> void testRowSetResults( String i_command, RowSetValueGetter i_getter,
        T[] i_expectedValues, String i_context, String i_failureDesc )
    {
        RowSet rowSet = null;
        try
        {
            rowSet = m_database.createRowSet( CommandType.COMMAND, i_command );
            rowSet.execute();

            List< T > values = new ArrayList< T >();
            while ( rowSet.next() )
            {
                values.add( (T)i_getter.getValue( rowSet ) );
            }
            assertEquals( i_context + ": " + i_failureDesc, i_expectedValues, values.toArray() );
        }
        catch( final SQLException e )
        {
            fail( i_context + ": caught an exception: " + e.toString());
        }
        finally
        {
            if ( rowSet != null )
                rowSet.dispose();
        }
    }

    private static final OfficeConnection connection = new OfficeConnection();
    private CsvDatabase m_database = null;
}
