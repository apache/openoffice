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

/*
 * Class.java
 *
 * Created on June 23, 2003, 3:19 PM
 */

/**
 *
 * @author  oj93728
 */
package complex.connectivity.hsqldb;
import java.sql.*;
import java.lang.reflect.Method;

import static org.junit.Assert.*;

public class DatabaseMetaData {
    
    private java.sql.DatabaseMetaData m_xMD;
    
    /** Creates a new instance of DatabaseMetaData */
    public DatabaseMetaData(java.sql.DatabaseMetaData _xmd) {
        m_xMD = _xmd;
    }
    
    protected void testMethod(String sName,Class[] params,Object[] objParams,int nCount){
        try {
            System.out.println("test method " + sName);
            
            Method aGet = ((Object)m_xMD).getClass().getDeclaredMethod(sName, params);
            if ( aGet != null ){
                ResultSet rs = (ResultSet)aGet.invoke(m_xMD, objParams);
                ResultSetMetaData rsMD = rs.getMetaData();            

                assertTrue( sName + " returns wrong column count" , rsMD.getColumnCount() == nCount);
            }
            else
                fail( sName + " returns wrong column count");
        } catch( java.lang.NoSuchMethodException ex ) {
            fail("Method " + sName + " could not be found!");
        } catch( java.lang.IllegalAccessException ex ) {
            fail("IllegalAccessException!");
        } catch( SQLException ex ) {
            fail("SQLException occurred: " + ex.getMessage());
        } catch( java.lang.reflect.InvocationTargetException ex ) {
            fail("IllegalAccessException!");
        } finally {
            
        }
    }
    
    public void test(){
        
   //     try {
            
            try{
		ResultSet rs = m_xMD.getTables(null,null,"TESTCASE",null);
		while ( rs.next() )
		{
			String catalog = rs.getString( 1 );
			if ( rs.wasNull() )
				catalog = null;

			String schema = rs.getString( 2 );
			if ( rs.wasNull() )
				schema = null;

			String table = rs.getString( 3 );
			String type = rs.getString( 4 );
			System.out.println("Catalog: " + catalog + " Schema: " + schema + " Table: " + table + " Type: " + type);
			System.out.println("------------------ Columns ------------------");
			ResultSet rsColumns = m_xMD.getColumns(catalog,schema,table,"%");
			while ( rsColumns.next() )
			{
				System.out.println("Column: " + rsColumns.getString( 4 ) + " Type: " + rsColumns.getInt( 5 ) + " TypeName: " + rsColumns.getString( 6 ) );
			}

		}
            } catch(Exception e){
                
            }
            //testMethod("getTypeInfo", zclass,empty,17);
/*
            Class[] zclass = new Class[]{};
            Object[] empty = new Object[]{};
            testMethod("getCatalogs", zclass,empty,1);
            
            testMethod("getSchemas", zclass,empty,2);
            testMethod("getTableTypes", zclass,empty,1);
            Class[] a4 = new Class[4];
            Object[] o4 = new Object[4];
            a4[0] = Class.forName("java.lang.Object");
            a4[1] = Class.forName("java.lang.String");
            a4[2] = Class.forName("java.lang.String");
            a4[3] = Class.forName("java.lang.String");

            o4[0] = null;
            o4[1] = null;
            o4[2] = null;
            o4[3] = "%";
            testMethod("getColumnPrivileges", a4,o4,7);
            testMethod("getColumns", a4,o4,18);
            testMethod("getProcedureColumns", a4,o4,13);
            testMethod("getColumns", a4,o4,18);
        
            Class[] a3 = new Class[3];
            Object[] o3 = new Object[3];
            a3[0] = Class.forName("java.lang.Object");
            a3[1] = Class.forName("java.lang.String");
            a3[2] = Class.forName("java.lang.String");

            o3[0] = null;
            o3[1] = null;
            o3[2] = "%";
            
            testMethod("getExportedKeys", a3,o3,14);
            testMethod("getImportedKeys", a3,o3,14);
            testMethod("getPrimaryKeys", a3,o3,14);
            testMethod("getProcedures", a3,o3,5);
            testMethod("getTablePrivileges", a3,o3,6);
            testMethod("getVersionColumns", a3,o3,7);
            // testMethod("getCrossReference", a3,o3,14);
  
        } catch( java.lang.ClassNotFoundException ex) {
            fail("ClassNotFoundException: " + ex.getMessage());
        }
 */
    }
/*    
    public com.sun.star.sdbc.XResultSet getCrossReference(Object obj, String str, String str2, Object obj3, String str4, String str5) {
    }
    public com.sun.star.sdbc.XResultSet getIndexInfo(Object obj, String str, String str2, boolean param, boolean param4) {
    }
    
    public com.sun.star.sdbc.XResultSet getTables(Object obj, String str, String str2, String[] str3) {
    }
    
    public com.sun.star.sdbc.XResultSet getUDTs(Object obj, String str, String str2, int[] values) {
    }
    
    public com.sun.star.sdbc.XResultSet getBestRowIdentifier(Object obj, String str, String str2, int param, boolean param4) throws com.sun.star.sdbc.SQLException {
    }
 */
}
