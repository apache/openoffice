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



package com.sun.star.comp.ucb;

import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.task.XInteractionAbort;
import com.sun.star.task.XInteractionHandler;
import com.sun.star.task.XInteractionRequest;
import com.sun.star.task.XInteractionContinuation;
import com.sun.star.ucb.Command;
import com.sun.star.ucb.GlobalTransferCommandArgument;
import com.sun.star.ucb.NameClash;
import com.sun.star.ucb.NameClashResolveRequest;
import com.sun.star.ucb.TransferCommandOperation;
import com.sun.star.ucb.XCommandEnvironment;
import com.sun.star.ucb.XCommandProcessor;
import com.sun.star.ucb.XInteractionReplaceExistingData;
import com.sun.star.ucb.XInteractionSupplyName;
import com.sun.star.ucb.XProgressHandler;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.UnoRuntime;
import java.io.PrintWriter;

import org.junit.Test;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import static org.junit.Assert.*;
import org.openoffice.test.OfficeConnection;

public final class GlobalTransfer_Test {
    private static final OfficeConnection officeConnection = new OfficeConnection();

    static private final String fileName
        = "testcase-do-not-remove.sxw";
    static private final String httpSourceDir
        = "http://so-berlin/~webdav/";
    static private final String httpTargetDir
        = "http://so-berlin/~webdav/";
    static private final String fileSourceDir
        = "file:///d:/temp/source/";
    static private final String fileTargetDir
        = "file:///d:/temp/";

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

    @Test
    public void testNameClashASK() throws Exception {
        Object oObj = null;
        try {
           XMultiServiceFactory xMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, officeConnection.getComponentContext().getServiceManager());
           oObj
            = xMSF.createInstance( "com.sun.star.ucb.UniversalContentBroker" );
        }
        catch(com.sun.star.uno.Exception e) {
            e.printStackTrace();

            // After this exception the test has failed and cannot continue.
            fail( "Cannot create service instance: com.sun.star.ucb." +
                    "UniversalContentBroker. message:" + e.getMessage() );
            return;
        }

        if ( oObj == null ) {
            fail( "Cannot create service instance: com.sun.star.ucb." +
                    "UniversalContentBroker");
            return;
        }

        XCommandProcessor xCmdProc
            = (XCommandProcessor)UnoRuntime.queryInterface(
                                               XCommandProcessor.class, oObj );
        assertTrue( "UCB does not implement mandatory interface XCommandProcessor!",
                xCmdProc != null);

        ResourceCopier cp = new ResourceCopier( xCmdProc );

        try {
            cp.copyResource( httpSourceDir, fileTargetDir, fileName );
        }
        catch(com.sun.star.uno.Exception e) {
            e.printStackTrace();

            // After this exception the test has failed and cannot continue.
            fail( "Could not copy resource:" + e.getMessage() );
        }
    }

    private final class ResourceCopier {
        private XCommandProcessor  m_cmdProc = null;
        private CommandEnvironment m_env = new CommandEnvironment();

        ResourceCopier( XCommandProcessor oCmdProc )
        {
            m_cmdProc = oCmdProc;
        }

        public void copyResource(
                String sourceDir, String targetDir, String fileName )
            throws Exception {

            GlobalTransferCommandArgument transferArg
                = new GlobalTransferCommandArgument(
                    TransferCommandOperation.COPY,
                    sourceDir + fileName,
                    targetDir,
                    "",
                    NameClash.ASK );

            Command cmd = new Command( "globalTransfer", -1, transferArg );

            m_cmdProc.execute( cmd, 0, m_env );
        }
    }

    private final class CommandEnvironment implements XCommandEnvironment {
        private final XInteractionHandler m_InteractionHandler
            = new InteractionHadler();

        public XInteractionHandler getInteractionHandler() {
            return m_InteractionHandler;
        }

        public XProgressHandler getProgressHandler() {
            // not needed for the test.
            return null;
        }
    }

    private final class InteractionHadler implements XInteractionHandler {
        public void handle( /*IN*/XInteractionRequest Request ) {

            System.out.println( "Interaction Handler called." );

            try {
                NameClashResolveRequest req = (NameClashResolveRequest)
                    AnyConverter.toObject(
                        NameClashResolveRequest.class, Request.getRequest() );

                System.out.println( "Interaction Handler: NameClashResolveRequest: "
                             + req.ClashingName );

                XInteractionContinuation[] continuations
                    = Request.getContinuations();
                for ( int i = 0; i < continuations.length; ++i ) {
/*
                    XInteractionAbort xAbort
                        = (XInteractionAbort)UnoRuntime.queryInterface(
                            XInteractionAbort.class, continuations[ i ] );
                    if ( xAbort != null ) {
                        System.out.println( "Interaction Handler selects: ABORT" );
                        xAbort.select();
                        return;
                    }
*/
/*
                    XInteractionReplaceExistingData xReplace
                        = (XInteractionReplaceExistingData)
                            UnoRuntime.queryInterface(
                                XInteractionReplaceExistingData.class,
                                continuations[ i ] );
                    if ( xReplace != null ) {
                        System.out.println( "Interaction Handler selects: REPLACE" );
                        xReplace.select();
                        return;
                    }
*/
                    XInteractionSupplyName xSupplyName
                        = (XInteractionSupplyName)
                            UnoRuntime.queryInterface(
                                XInteractionSupplyName.class,
                                continuations[ i ] );
                    if ( xSupplyName != null ) {
                        String newname = "renamed_" + req.ClashingName;
                        System.out.println( "Interaction Handler selects: NEW NAME: "
                                     + newname );
                        xSupplyName.setName( newname );
                        xSupplyName.select();
                        return;
                    }
                }
            }
            catch ( IllegalArgumentException e )
            {
                e.printStackTrace();
            }
        }
    }

}
