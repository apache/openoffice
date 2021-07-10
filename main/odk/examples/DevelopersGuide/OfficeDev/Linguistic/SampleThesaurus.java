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



// uno
import com.sun.star.lib.uno.helper.ComponentBase;
import com.sun.star.uno.UnoRuntime;

// factories
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XSingleServiceFactory;

// supported Interfaces
import com.sun.star.linguistic2.XThesaurus;
import com.sun.star.lang.XInitialization;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.lang.XServiceDisplayName;

// Exceptions
import com.sun.star.uno.Exception;
import com.sun.star.uno.RuntimeException;
import com.sun.star.lang.IllegalArgumentException;

//used Interfaces
import com.sun.star.linguistic2.XMeaning;
import com.sun.star.lang.Locale;
import com.sun.star.lang.XEventListener;
import com.sun.star.lang.EventObject;
import com.sun.star.beans.XPropertySet;
import com.sun.star.beans.PropertyValue;
import com.sun.star.uno.AnyConverter;
import com.sun.star.lang.XTypeProvider;
import com.sun.star.uno.Type;

import java.util.ArrayList;

public class SampleThesaurus extends ComponentBase implements
        XThesaurus,
        XInitialization,
        XServiceDisplayName,
        XServiceInfo
{
    PropChgHelper               aPropChgHelper;
    ArrayList                   aEvtListeners;
    boolean                     bDisposing;

    public SampleThesaurus()
    {
        // names of relevant properties to be used
        String[] aProps = new String[]
            {
                "IsIgnoreControlCharacters",
                "IsUseDictionaryList",
                "IsGermanPreReform",
            };

        // this service has no listeners thus we may use the base class,
        // which is here basically used only to keep track of the
        // property set (and it's lifetime) since it gets used in the
        // 'GetValueToUse' function
        aPropChgHelper = new PropChgHelper( (XThesaurus) this, aProps );

        aEvtListeners  = new ArrayList();
        bDisposing     = false;
    }

    private boolean IsEqual( Locale aLoc1, Locale aLoc2 )
    {
        return aLoc1.Language.equals( aLoc2.Language ) &&
               aLoc1.Country .equals( aLoc2.Country ) &&
               aLoc1.Variant .equals( aLoc2.Variant );
    }

    private boolean GetValueToUse(
            String          aPropName,
            boolean         bDefaultVal,
            PropertyValue[] aProps )
    {
        boolean bRes = bDefaultVal;

        try
        {
            // use temporary value if supplied
            for (int i = 0; i < aProps.length; ++i)
            {
                if (aPropName.equals( aProps[i].Name ))
                {
                    Object aObj = aProps[i].Value;
                    if (AnyConverter.isBoolean( aObj ))
                    {
                        bRes = AnyConverter.toBoolean( aObj );
                        return bRes;
                    }
                }
            }

            // otherwise use value from property set (if available)
            XPropertySet xPropSet = aPropChgHelper.GetPropSet();
            if (xPropSet != null) // should always be the case
            {
                Object aObj = xPropSet.getPropertyValue( aPropName );
                if (AnyConverter.isBoolean( aObj ))
                    bRes = AnyConverter.toBoolean( aObj );
            }
        }
        catch (Exception e) {
            bRes = bDefaultVal;
        }

        return bRes;
    }

    // __________ interface methods __________


    //*****************
    //XSupportedLocales
    //*****************
    public Locale[] getLocales()
        throws com.sun.star.uno.RuntimeException
    {
        Locale aLocales[] =
        {
            new Locale( "en", "US", "" )
        };

        return aLocales;
    }

    public boolean hasLocale( Locale aLocale )
        throws com.sun.star.uno.RuntimeException
    {
        boolean bRes = false;
        if (IsEqual( aLocale, new Locale( "en", "US", "" ) ))
            bRes = true;
        return bRes;
    }

    //**********
    //XThesaurus
    //**********
    public XMeaning[] queryMeanings(
            String aTerm, Locale aLocale,
            PropertyValue[] aProperties )
        throws com.sun.star.lang.IllegalArgumentException,
               com.sun.star.uno.RuntimeException
    {
        if (IsEqual( aLocale, new Locale() ) || aTerm.length() == 0)
            return null;

        // linguistic is currently not allowed to throw exceptions
        // thus we return null which means 'word cannot be looked up'
        if (!hasLocale( aLocale ))
            return null;

        // get values of relevant properties that may be used.
        //! The values for 'IsIgnoreControlCharacters' and 'IsUseDictionaryList'
        //! are handled by the dispatcher! Thus there is no need to access
        //! them here.
        boolean bIsGermanPreReform = GetValueToUse( "IsGermanPreReform", false, aProperties );

        XMeaning[] aRes = null;

        //!! This code needs to be replaced by code calling the actual
        //!! implementation of your thesaurus
        if (aTerm.equals( "house" ) &&
            IsEqual( aLocale, new Locale( "en", "US", "" ) ) )
        {
            aRes = new XMeaning[]
                {
                    new XMeaning_impl( "a building where one lives",
                            new String[]{ "home", "place", "dwelling" } ),
                    new XMeaning_impl( "a group of people sharing common ancestry",
                            new String[]{ "family", "clan", "kindred" } ),
                    new XMeaning_impl( "to provide with lodging",
                            new String[]{ "room", "board", "put up" } )
                };
        }

        return aRes;
    }


    //********************
    // XServiceDisplayName
    //********************
    public String getServiceDisplayName( Locale aLocale )
        throws com.sun.star.uno.RuntimeException
    {
        return "Java Samples";
    }

    //****************
    // XInitialization
    //****************
    public void initialize( Object[] aArguments )
        throws com.sun.star.uno.Exception,
               com.sun.star.uno.RuntimeException
    {
        int nLen = aArguments.length;
        if (2 == nLen)
        {
            XPropertySet xPropSet = (XPropertySet)UnoRuntime.queryInterface(
                                         XPropertySet.class, aArguments[0]);
            // start listening to property changes
            aPropChgHelper.AddAsListenerTo( xPropSet );
        }
    }

    //*************
    // XServiceInfo
    //*************
    public boolean supportsService( String aServiceName )
        throws com.sun.star.uno.RuntimeException
    {
        String[] aServices = getSupportedServiceNames_Static();
        int i, nLength = aServices.length;
        boolean bResult = false;

        for( i = 0; !bResult && i < nLength; ++i )
            bResult = aServiceName.equals( aServices[ i ] );

        return bResult;
    }

    public String getImplementationName()
        throws com.sun.star.uno.RuntimeException
    {
        return _aSvcImplName;
    }

    public String[] getSupportedServiceNames()
        throws com.sun.star.uno.RuntimeException
    {
        return getSupportedServiceNames_Static();
    }

    // __________ static things __________

    public static String _aSvcImplName = SampleThesaurus.class.getName();

    public static String[] getSupportedServiceNames_Static()
    {
        String[] aResult = { "com.sun.star.linguistic2.Thesaurus" };
        return aResult;
    }


    /**
     * Returns a factory for creating the service.
     * This method is called by the <code>JavaLoader</code>
     * <p>
     * @return  returns a <code>XSingleServiceFactory</code> for creating the component
     * @param   implName     the name of the implementation for which a service is desired
     * @param   multiFactory the service manager to be used if needed
     * @param   regKey       the registryKey
     * @see                  com.sun.star.comp.loader.JavaLoader
     */
    public static XSingleServiceFactory __getServiceFactory(
        String aImplName,
        XMultiServiceFactory xMultiFactory,
        com.sun.star.registry.XRegistryKey xRegKey )
    {
        XSingleServiceFactory xSingleServiceFactory = null;
        if( aImplName.equals( _aSvcImplName ) )
        {
            xSingleServiceFactory = new OneInstanceFactory(
                    SampleThesaurus.class, _aSvcImplName,
                    getSupportedServiceNames_Static(),
                    xMultiFactory );
        }
        return xSingleServiceFactory;
    }

    /**
     * Writes the service information into the given registry key.
     * This method is called by the <code>JavaLoader</code>
     * <p>
     * @return  returns true if the operation succeeded
     * @param   xRegKey       the registryKey
     * @see                  com.sun.star.comp.loader.JavaLoader
     */
    // This method not longer necessary since OOo 3.4 where the component registration
    // was changed to passive component registration. For more details see
    // https://wiki.openoffice.org/wiki/Passive_Component_Registration

//     public static boolean __writeRegistryServiceInfo(
//             com.sun.star.registry.XRegistryKey xRegKey )
//     {
//         boolean bResult = true;
//         String[] aServices = getSupportedServiceNames_Static();
//         int i, nLength = aServices.length;
//         for( i = 0; i < nLength; ++i )
//         {
//             bResult = bResult && com.sun.star.comp.loader.FactoryHelper.writeRegistryServiceInfo(
//                 _aSvcImplName, aServices[i], xRegKey );
//         }
//         return bResult;
//     }
}

