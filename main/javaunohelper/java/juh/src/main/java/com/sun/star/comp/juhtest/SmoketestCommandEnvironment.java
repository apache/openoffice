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


package com.sun.star.comp.juhtest;

import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lib.uno.helper.WeakBase;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.ucb.XCommandEnvironment;

/** This service is for use by the smoketest which checks the installation of
 * extensions. The service provides the XCommandEnvironment interface, which
 * is needed for adding extensions.
 */
public class SmoketestCommandEnvironment extends WeakBase
    implements XServiceInfo, XCommandEnvironment {

    static private final String __serviceName =
    "com.sun.star.deployment.test.SmoketestCommandEnvironment";
    
    private XComponentContext m_cmpCtx;
    private XMultiComponentFactory m_xMCF;
    

    public SmoketestCommandEnvironment(XComponentContext xCompContext) {
        try {
            m_cmpCtx = xCompContext;
            m_xMCF = m_cmpCtx.getServiceManager();                
        }
        catch( Exception e ) {
            e.printStackTrace();
        }
    }

    public static String[] getServiceNames() {
        String[] sSupportedServiceNames = { __serviceName};
        return sSupportedServiceNames;
    }

    //XServiceInfo -------------------------------------------------------------
    public String[] getSupportedServiceNames() {
        return getServiceNames();
    }

    
    public boolean supportsService( String sServiceName ) {
        boolean bSupported = false;
        if (sServiceName.equals(__serviceName))
            bSupported = true;
        return bSupported;
    }
    
    public String getImplementationName() {
        return  SmoketestCommandEnvironment.class.getName();
    }
    
    //XCommandEnvironment ================================================
    public com.sun.star.task.XInteractionHandler getInteractionHandler()
    {
        return new InteractionImpl();
    }
    
    public com.sun.star.ucb.XProgressHandler getProgressHandler()
    {
        return new ProgressImpl();
    }
}




class InteractionImpl implements com.sun.star.task.XInteractionHandler
{
    public void handle( com.sun.star.task.XInteractionRequest xRequest )
    {
        Object request = xRequest.getRequest();

        boolean approve = true;
        boolean abort = false;
//             Object install_Exception =
//                 AnyConverter.toObject(
//                     com.sun.star.deployment.InstallException.class, request);
//             if (install_Exception != null)
//             {
//                 approve = true;
//             }

        com.sun.star.task.XInteractionContinuation[] conts = xRequest.getContinuations();
        for (int i = 0; i < conts.length; i++)
        {
            if (approve)
            {
                com.sun.star.task.XInteractionApprove xApprove =
                    UnoRuntime.queryInterface(com.sun.star.task.XInteractionApprove.class, conts[i]);
                if (xApprove != null)
                    xApprove.select();
                //don't query again for ongoing extensions
                approve = false;
            }
            else if (abort)
            {
                com.sun.star.task.XInteractionAbort xAbort =
                    UnoRuntime.queryInterface(com.sun.star.task.XInteractionAbort.class, conts[i]);
                if (xAbort != null)
                    xAbort.select();
                //don't query again for ongoing extensions
                abort = false;
            }
        }   
    }
}

class ProgressImpl implements com.sun.star.ucb.XProgressHandler
{
    public void push(Object status)
    {
    }

    public void update(Object status)
    {
    }

    public void pop()
    {
    }
}
