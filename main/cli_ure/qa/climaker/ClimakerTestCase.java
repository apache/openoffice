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


package climaker;

import org.junit.Test;
import static org.junit.Assert.*;


public class ClimakerTestCase
{
    @Test
    public void checkGeneratedCLITypes() throws Exception
    {
        String testProgram = System.getProperty("cli_ure_test");
        if (testProgram == null || testProgram.length() == 0)
            fail("Check the make file. Java must be called with -Dcli_ure_test=pathtoexe");
        Process proc = null;
        proc = Runtime.getRuntime().exec(testProgram);
        Reader outReader = new Reader(proc.getInputStream());
        Reader errReader = new Reader(proc.getErrorStream());
        proc.waitFor();
        int retVal = proc.exitValue();
        assertTrue("Tests for generated CLI code failed.", retVal == 0);
    }
}


/*  This reads reads from an InputStream and discards the data.
 */
class Reader extends Thread
{
    java.io.InputStream is;
    public Reader(java.io.InputStream stream)
    {
        is = stream;
        start();
    }

    public void run()
    {
        try
        {
            byte[] buf = new byte[1024];
            while (-1 != is.read(buf));
        }
        catch (java.io.IOException exc)
        {
        }
    }
}
