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


package clitest;


import java.io.*;

import org.junit.Test;
import static org.junit.Assert.*;


public class CLITest
{
    @Test
    public void runCLITests() throws Exception
    {
        String testProgram = System.getProperty("cli_test", "");
        if (testProgram.length() == 0)
            fail("Check the make file. Java must be called with -Dcli_test=pathtoexe");

        String arg1 = System.getProperty("cli_test_arg", "");
        if (arg1.length() == 0)
            fail("Check the make file. Java must be called with " +
                 "-Dcli_test_arg=path_to_bootstrap_ini");
        String[] cmdarray = new String[] {testProgram, arg1}; 

        Process proc = null;
        Reader outReader;
        Reader errReader;

        proc = Runtime.getRuntime().exec(cmdarray);
        outReader = new Reader(proc.getInputStream());
        errReader = new Reader(proc.getErrorStream());
//       System.out.println("### waiting for " + testProgram);
        proc.waitFor();
        int retVal = proc.exitValue();
        System.out.println("### " + testProgram + " finished with exit code " + retVal);
        assertTrue("CLI test failed.", retVal == 0);
    }
}


/*  This reads reads from an InputStream and discards the data.
 */
class Reader extends Thread
{
    InputStream is;
    public Reader(InputStream stream)
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
