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



class ExampleAddInResult implements com.sun.star.sheet.XVolatileResult
{
    private String aName;
    private int nValue;
    private java.util.Vector aListeners = new java.util.Vector();

    public ExampleAddInResult( String aNewName )
    {
        aName = aNewName;
    }

    private com.sun.star.sheet.ResultEvent getResult()
    {
        com.sun.star.sheet.ResultEvent aEvent =
            new com.sun.star.sheet.ResultEvent();
        aEvent.Value = aName + " " + String.valueOf( nValue );
        aEvent.Source = this;
        return aEvent;
    }

    public void addResultListener(com.sun.star.sheet.XResultListener aListener)
    {
        aListeners.addElement( aListener );

        // immediately notify of initial value
        aListener.modified( getResult() );
    }

    public void removeResultListener(com.sun.star.sheet.XResultListener aListener)
    {
        aListeners.removeElement( aListener );
    }

    public void incrementValue()
    {
        ++nValue;
        com.sun.star.sheet.ResultEvent aEvent = getResult();

        java.util.Enumeration aEnum = aListeners.elements();
        while (aEnum.hasMoreElements())
            ((com.sun.star.sheet.XResultListener)aEnum.nextElement()).modified(
                aEvent);
    }
}

class ExampleAddInThread extends Thread
{
    private java.util.Hashtable aCounters;

    public ExampleAddInThread( java.util.Hashtable aResults )
    {
        aCounters = aResults;
    }

    public void run()
    {
        while ( true )
        {
            try
            {
                sleep(1000);
            }
            catch( InterruptedException exception )
            {
            }

            // increment all counters
            java.util.Enumeration aEnum = aCounters.elements();
            while (aEnum.hasMoreElements())
                ((ExampleAddInResult)aEnum.nextElement()).incrementValue();
        }
    }
}

public class ExampleAddIn
{
    static public class _ExampleAddIn extends com.sun.star.lib.uno.helper.WeakBase
           implements org.openoffice.sheet.addin.XExampleAddIn,
                      com.sun.star.sheet.XAddIn,
                      com.sun.star.lang.XServiceName,
                      com.sun.star.lang.XServiceInfo
    {
        static private final String aExampleService = "org.openoffice.sheet.addin.ExampleAddIn";
        static private final String aAddInService = "com.sun.star.sheet.AddIn";
        static private final String aImplName = _ExampleAddIn.class.getName();

        private static final short FUNCTION_INVALID   = -1;
        private static final short FUNCTION_INCREMENT = 0;
        private static final short FUNCTION_COUNTER   = 1;

        private static final String[] aFunctionNames =
        {
            "getIncremented",
            "getCounter"
        };
        private static final String[] aDisplayFunctionNames =
        {
            "Increment",
            "Counter"
        };
        private static final String[] aDescriptions =
        {
            "Increments a value",
            "Returns a counter"
        };
        private static final String[] aFirstArgumentNames =
        {
            "Value",
            "Name"
        };
        private static final String[] aFirstArgumentDescriptions =
        {
            "The value that is incremented",
            "The name of the counter"
        };

        private com.sun.star.lang.Locale aFuncLocale;
        private java.util.Hashtable aResults;

        public _ExampleAddIn( com.sun.star.lang.XMultiServiceFactory xFactory )
        {
        }

        private int getFunctionID( String aProgrammaticFunctionName )
        {
            for ( int i = 0; i < aFunctionNames.length; i++ )
                if ( aProgrammaticFunctionName.equals(aFunctionNames[i]) )
                    return i;
            return FUNCTION_INVALID;
        }

        //  XExampleAddIn

        public int getIncremented( int nValue )
        {
            return nValue + 1;
        }

        public com.sun.star.sheet.XVolatileResult getCounter(String aName)
        {
            if ( aResults == null )
            {
                // create the table of results, and start a thread to increment
                // all counters
                aResults = new java.util.Hashtable();
                ExampleAddInThread aThread = new ExampleAddInThread( aResults );
                aThread.start();
            }

            ExampleAddInResult aResult = (ExampleAddInResult) aResults.get(aName);
            if ( aResult == null )
            {
                aResult = new ExampleAddInResult(aName);
                aResults.put( aName, aResult );
            }
            return aResult;
        }

        //  XAddIn

        public String getProgrammaticFuntionName(String aDisplayName)
        {
            for ( int i = 0; i < aFunctionNames.length; i++ )
                if ( aDisplayName.equals(aDisplayFunctionNames[i]) )
                    return aFunctionNames[i];
            return "";
        }

        public String getDisplayFunctionName(String aProgrammaticName)
        {
            int nFunction = getFunctionID( aProgrammaticName );
            return ( nFunction == FUNCTION_INVALID ) ? "" :
                aDisplayFunctionNames[nFunction];
        }

        public String getFunctionDescription(String aProgrammaticName)
        {
            int nFunction = getFunctionID( aProgrammaticName );
            return ( nFunction == FUNCTION_INVALID ) ? "" :
                aDescriptions[nFunction];
        }

        public String getDisplayArgumentName(String aProgrammaticFunctionName,
                                             int nArgument)
        {
            //  both functions in this example only have a first argument
            int nFunction = getFunctionID( aProgrammaticFunctionName );
            return ( nFunction == FUNCTION_INVALID || nArgument != 0) ? "" :
                aFirstArgumentNames[nFunction];
        }

        public String getArgumentDescription(String aProgrammaticFunctionName,
                                             int nArgument )
        {
            //  both functions in this example only have a first argument
            int nFunction = getFunctionID( aProgrammaticFunctionName );
            return ( nFunction == FUNCTION_INVALID || nArgument != 0) ? "" :
                aFirstArgumentDescriptions[nFunction];
        }

        public String getProgrammaticCategoryName(String aProgrammaticFunctionName)
        {
            return( "Add-In" );
        }

        public String getDisplayCategoryName(String aProgrammaticFunctionName)
        {
            return( "Add-In" );
        }

        //  XLocalizable

        public void setLocale( com.sun.star.lang.Locale aLocale )
        {
            // the locale is stored and used for getLocale, but otherwise
            // ignored in this example
            aFuncLocale = aLocale;
        }

        public com.sun.star.lang.Locale getLocale()
        {
            return aFuncLocale;
        }

        //  XServiceName

        public String getServiceName()
        {
            return aExampleService;
        }

        //  XServiceInfo

        public String getImplementationName()
        {
            return aImplName;
        }

        public String[] getSupportedServiceNames()
        {
            String [] aSupportedServices = new String[ 2 ];
            aSupportedServices[ 0 ] = aExampleService;
            aSupportedServices[ 1 ] = aAddInService;
            return aSupportedServices;
        }

        public boolean supportsService( String aService )
        {
            return (aService.equals( aExampleService ) ||
                    aService.equals( aAddInService ) );
        }

    }


    public static com.sun.star.lang.XSingleServiceFactory __getServiceFactory(
        String implName,
        com.sun.star.lang.XMultiServiceFactory multiFactory,
        com.sun.star.registry.XRegistryKey regKey)
    {
        com.sun.star.lang.XSingleServiceFactory xSingleServiceFactory = null;
        if ( implName.equals(_ExampleAddIn.aImplName) )
            xSingleServiceFactory =
                com.sun.star.comp.loader.FactoryHelper.getServiceFactory(
                    _ExampleAddIn.class, _ExampleAddIn.aExampleService,
                    multiFactory, regKey);
        return xSingleServiceFactory;
    }

    // This method not longer necessary since OOo 3.4 where the component registration
    // was changed to passive component registration. For more details see
    // https://wiki.openoffice.org/wiki/Passive_Component_Registration

//     public static boolean __writeRegistryServiceInfo(
//         com.sun.star.registry.XRegistryKey regKey)
//     {
//         //  register for both the base AddIn and the own service
//         return com.sun.star.comp.loader.FactoryHelper.writeRegistryServiceInfo(
//                     _ExampleAddIn.aImplName, _ExampleAddIn.aExampleService, regKey)
//             && com.sun.star.comp.loader.FactoryHelper.writeRegistryServiceInfo(
//                     _ExampleAddIn.aImplName, _ExampleAddIn.aAddInService, regKey);
//     }
}

