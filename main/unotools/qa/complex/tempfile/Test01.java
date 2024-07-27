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


package complex.tempfile;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.ucb.XSimpleFileAccess;
import com.sun.star.io.*;
import com.sun.star.uno.UnoRuntime;
import java.util.Random;

public class Test01 implements TempFileTest {
    XMultiServiceFactory m_xMSF = null;
    XSimpleFileAccess m_xSFA = null;
    TestHelper m_aTestHelper = null;
    
    public Test01(XMultiServiceFactory xMSF, XSimpleFileAccess xSFA) {
        m_xMSF = xMSF;
        m_xSFA = xSFA;
        m_aTestHelper = new TestHelper( "Test01: ");
    }
    
    public boolean test() {
        XTempFile xTempFile = null;
        XTruncate xTruncate = null;
        String sFileURL = null;
        String sFileName = null;
        //create a temporary file.
        try {
            Object oTempFile = m_xMSF.createInstance( "com.sun.star.io.TempFile" );
            xTempFile = UnoRuntime.queryInterface(XTempFile.class, oTempFile);
            m_aTestHelper.Message( "Tempfile created." );
            xTruncate = UnoRuntime.queryInterface(XTruncate.class, oTempFile);
        } catch( Exception e ) {
            m_aTestHelper.Error( "Cannot create TempFile. exception: " + e );
            return false;
        }
        
        //retrieve the tempfile URL
        if ( xTempFile == null ) {
            m_aTestHelper.Error( "Cannot get XTempFile interface." );
            return false;
        }
        
        try {
            //compare the file name with the name in the URL.
            sFileURL = m_aTestHelper.GetTempFileURL( xTempFile );
            sFileName = m_aTestHelper.GetTempFileName( xTempFile );
            m_aTestHelper.CompareFileNameAndURL( sFileName, sFileURL );
            
            //write to the stream using the service.
            byte pBytesIn[] = new byte[9];
            byte pBytesOut1[][] = new byte [1][9];
            byte pBytesOut2[][] = new byte [1][9];
            Random oRandom = new Random();
            oRandom.nextBytes( pBytesIn );
            m_aTestHelper.WriteBytesWithStream( pBytesIn, xTempFile );
            
            //check the result by reading from the service.
            xTempFile.seek(0);
            m_aTestHelper.ReadBytesWithStream( pBytesOut1, pBytesIn.length + 1, xTempFile );
            for ( int i = 0; i < pBytesIn.length ; i++ ) {
                if ( pBytesOut1[0][i] != pBytesIn[i] ) {
                    m_aTestHelper.Error( "Tempfile outputs false data!" );
                }
            }
            
            //check the result by reading from the file directly.
            m_aTestHelper.ReadDirectlyFromTempFile( pBytesOut2, pBytesIn.length + 1, m_xSFA, sFileURL );
            for ( int i = 0; i < pBytesIn.length; i++ ) {
                if ( pBytesOut2[0][i] != pBytesIn[i] ) {
                    m_aTestHelper.Error( "Tempfile contains false data!" );
                }
            }
            
            //close the object(by closing input and output), check that the file was removed.
            xTempFile.setRemoveFile( false );
            m_aTestHelper.CloseTempFile( xTempFile );
            if( !m_aTestHelper.IfTempFileExists( m_xSFA, sFileURL ) ) {
                m_aTestHelper.Error( "TempFile mistakenly removed. " );
            } else {
                m_aTestHelper.KillTempFile( sFileURL, m_xSFA );
            }
        } catch ( Exception e ) {
            m_aTestHelper.Error( "Exception: " + e );
            return false;
        }
        return true;
    }
}
