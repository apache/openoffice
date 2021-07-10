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
import com.sun.star.linguistic2.XSpellChecker;
import com.sun.star.linguistic2.XLinguServiceEventBroadcaster;
import com.sun.star.lang.XInitialization;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.lang.XServiceDisplayName;

// Exceptions
import com.sun.star.uno.Exception;
import com.sun.star.uno.RuntimeException;
import com.sun.star.lang.IllegalArgumentException;

//used Interfaces
import com.sun.star.linguistic2.XLinguServiceEventListener;
import com.sun.star.linguistic2.XSpellAlternatives;
import com.sun.star.linguistic2.SpellFailure;
import com.sun.star.lang.Locale;
import com.sun.star.lang.XEventListener;
import com.sun.star.lang.EventObject;
import com.sun.star.beans.XPropertySet;
import com.sun.star.beans.PropertyValue;
import com.sun.star.uno.AnyConverter;
import com.sun.star.lang.XTypeProvider;
import com.sun.star.uno.XInterface;
import com.sun.star.uno.Type;

import java.util.ArrayList;

public class SampleSpellChecker extends ComponentBase implements
        XSpellChecker,
        XLinguServiceEventBroadcaster,
        XInitialization,
        XServiceDisplayName,
        XServiceInfo
{
    PropChgHelper_Spell         aPropChgHelper;
    ArrayList                   aEvtListeners;
    boolean                     bDisposing;

    public SampleSpellChecker()
    {
        // names of relevant properties to be used
        String[] aProps = new String[]
            {
                "IsIgnoreControlCharacters",
                "IsUseDictionaryList",
                "IsGermanPreReform",
                "IsSpellUpperCase",
                "IsSpellWithDigits",
                "IsSpellCapitalization"
            };
        aPropChgHelper = new PropChgHelper_Spell( (XSpellChecker) this, aProps );
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

    private boolean IsUpper( String aWord, Locale aLocale )
    {
        java.util.Locale aLang = new java.util.Locale(
                aLocale.Language, aLocale.Country, aLocale.Variant );
        return aWord.equals( aWord.toUpperCase( aLang ) );
    }

    private boolean HasDigits( String aWord )
    {
        int nLen = aWord.length();
        for (int i = 0; i < nLen; ++i)
        {
            if (Character.isDigit( aWord.charAt(i) ))
                return true;
        }
        return false;
    }

    private short GetSpellFailure(
            String aWord,
            Locale aLocale,
            PropertyValue[] aProperties )
    {
        short nRes = -1;

        //!! This code needs to be replaced by code calling the actual
        //!! implementation of your spellchecker
        boolean bIsGermanPreReform = GetValueToUse( "IsGermanPreReform", false, aProperties );
        if (IsEqual( aLocale, new Locale( "de", "DE", "" ) ))
        {
            if (bIsGermanPreReform && aWord.equals( "Schifffahrt" ))
                nRes = SpellFailure.SPELLING_ERROR;
            else if (!bIsGermanPreReform && aWord.equals( "Schiffahrt" ))
                nRes = SpellFailure.SPELLING_ERROR;
        }
        else if (IsEqual( aLocale, new Locale( "en", "US", "" ) ))
        {
            // words with 'u', 'U' and 'arizona' are defined to be incorrect
            boolean bIsValid = !(aWord.indexOf( "u" ) != -1 || aWord.indexOf( "U" ) != -1)
                                 && !aWord.equals( "arizona" );

            if (!bIsValid)
            {
                // default value (no other SpellFailure type is applicable)
                nRes = SpellFailure.SPELLING_ERROR;

                if (aWord.equals( "arizona" ))
                    nRes = SpellFailure.CAPTION_ERROR;
                else if (aWord.equals( "house" ))
                    nRes = SpellFailure.SPELLING_ERROR;
                else if (aWord.equals( "course" ))
                    nRes = SpellFailure.IS_NEGATIVE_WORD;
            }
        }

        return nRes;
    }

    private XSpellAlternatives GetProposals(
            String aWord,
            Locale aLocale,
            PropertyValue[] aProperties )
    {
        short nType = SpellFailure.SPELLING_ERROR;
        String[] aProposals = null;

        // get values of relevant properties that may be used.
        //! The values for 'IsIgnoreControlCharacters' and 'IsUseDictionaryList'
        //! are handled by the dispatcher! Thus there is no need to access
        //! them here.
        boolean bIsGermanPreReform      = GetValueToUse( "IsGermanPreReform", false, aProperties );
        boolean bIsSpellWithDigits      = GetValueToUse( "IsSpellWithDigits", false, aProperties );
        boolean bIsSpellUpperCase       = GetValueToUse( "IsSpellUpperCase", false, aProperties );
        boolean bIsSpellCapitalization  = GetValueToUse( "IsSpellCapitalization", true, aProperties );

        //!! This code needs to be replaced by code calling the actual
        //!! implementation of your spellchecker
        if (IsEqual( aLocale, new Locale( "de", "DE", "" ) ))
        {
            if (bIsGermanPreReform && aWord.equals( "Schifffahrt" ))
            {
                nType = SpellFailure.SPELLING_ERROR;
                aProposals = new String[]{ "Schiffahrt" };
            }
            else if (!bIsGermanPreReform && aWord.equals( "Schiffahrt" ))
            {
                nType = SpellFailure.SPELLING_ERROR;
                aProposals = new String[]{ "Schifffahrt" };
            }
        }
        else if (IsEqual( aLocale, new Locale( "en", "US", "" ) ))
        {
            if (aWord.equals( "arizona" ))
            {
                nType = SpellFailure.CAPTION_ERROR;
                aProposals = new String[]{ "Arizona" };
            }
            else if (aWord.equals( "house" ))
            {
                nType = SpellFailure.SPELLING_ERROR;
                aProposals = new String[]{ "horse", "home" };
            }
            else if (aWord.equals( "course" ))
            {
                nType = SpellFailure.IS_NEGATIVE_WORD;
                aProposals = new String[]{ "line", "plan", "approach" };
            }
        }

        // always return a result if word is incorrect,
        // proposals may be empty though.
        return new XSpellAlternatives_impl( aWord, aLocale,
                                            nType, aProposals );
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
            new Locale( "de", "DE", "" ),
            new Locale( "en", "US", "" )
        };

        return aLocales;
    }

    public boolean hasLocale( Locale aLocale )
        throws com.sun.star.uno.RuntimeException
    {
        boolean bRes = false;
        if ( IsEqual( aLocale, new Locale( "de", "DE", "" ) ) ||
             IsEqual( aLocale, new Locale( "en", "US", "" ) ))
            bRes = true;
        return bRes;
    }


    //*************
    //XSpellChecker
    //*************
    public boolean isValid(
            String aWord, Locale aLocale,
            PropertyValue[] aProperties )
        throws com.sun.star.uno.RuntimeException,
               IllegalArgumentException
    {
        if (IsEqual( aLocale, new Locale() ) || aWord.length() == 0)
            return true;

        // linguistic is currently not allowed to throw exceptions
        // thus we return null which means 'word cannot be spelled'
        if (!hasLocale( aLocale ))
            return true;

        // get values of relevant properties that may be used.
        //! The values for 'IsIgnoreControlCharacters' and 'IsUseDictionaryList'
        //! are handled by the dispatcher! Thus there is no need to access
        //! them here.
        boolean bIsGermanPreReform      = GetValueToUse( "IsGermanPreReform", false, aProperties );
        boolean bIsSpellWithDigits      = GetValueToUse( "IsSpellWithDigits", false, aProperties );
        boolean bIsSpellUpperCase       = GetValueToUse( "IsSpellUpperCase", false, aProperties );
        boolean bIsSpellCapitalization  = GetValueToUse( "IsSpellCapitalization", true, aProperties );

        short nFailure = GetSpellFailure( aWord, aLocale, aProperties );
        if (nFailure != -1)
        {
            // postprocess result for errors that should be ignored
            if (   (!bIsSpellUpperCase  && IsUpper( aWord, aLocale ))
                || (!bIsSpellWithDigits && HasDigits( aWord ))
                || (!bIsSpellCapitalization
                    &&  nFailure == SpellFailure.CAPTION_ERROR)
            )
                nFailure = -1;
        }

        return nFailure == -1;
    }


    public XSpellAlternatives spell(
            String aWord, Locale aLocale,
            PropertyValue[] aProperties )
        throws com.sun.star.uno.RuntimeException,
               IllegalArgumentException
    {
        if (IsEqual( aLocale, new Locale() ) || aWord.length() == 0)
            return null;

        // linguistic is currently not allowed to throw exceptions
        // thus we return null which means 'word cannot be spelled'
        if (!hasLocale( aLocale ))
            return null;

        XSpellAlternatives xRes = null;
        if (!isValid( aWord, aLocale, aProperties ))
        {
            xRes = GetProposals( aWord, aLocale, aProperties );
        }
        return xRes;
    }


    //*****************************
    //XLinguServiceEventBroadcaster
    //*****************************
    public boolean addLinguServiceEventListener (
            XLinguServiceEventListener xLstnr )
        throws com.sun.star.uno.RuntimeException
    {
        boolean bRes = false;
        if (!bDisposing && xLstnr != null)
            bRes = aPropChgHelper.addLinguServiceEventListener( xLstnr );
        return bRes;
    }

    public boolean removeLinguServiceEventListener(
            XLinguServiceEventListener xLstnr )
        throws com.sun.star.uno.RuntimeException
    {
        boolean bRes = false;
        if (!bDisposing && xLstnr != null)
            bRes = aPropChgHelper.removeLinguServiceEventListener( xLstnr );
        return bRes;
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

    public static String _aSvcImplName = SampleSpellChecker.class.getName();

    public static String[] getSupportedServiceNames_Static()
    {
        String[] aResult = { "com.sun.star.linguistic2.SpellChecker" };
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
                    SampleSpellChecker.class, _aSvcImplName,
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

