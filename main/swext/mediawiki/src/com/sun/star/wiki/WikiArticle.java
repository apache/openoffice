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



package com.sun.star.wiki;

import java.net.URL;
import java.util.Hashtable;
import java.util.concurrent.Callable;
import com.sun.star.uno.XComponentContext;
import org.w3c.dom.Document;
import org.w3c.dom.Element;


public class WikiArticle
{
    private XComponentContext m_xContext;

    private boolean m_bNoArticle = true;

    protected String m_sWikiUser;
    protected String m_sWikiPass;

    protected String m_sTitle = "";

    private URL m_aMainURL;
    private WikiSession m_aSession;


    /** Creates a new instance of WikiArticle */
    public WikiArticle( XComponentContext xContext, String sTitle, Hashtable wikiSettings, boolean bLogin, WikiPropDialog aPropDialog )
        throws java.net.MalformedURLException, com.sun.star.uno.Exception, java.io.IOException, WikiCancelException
    {
        m_xContext = xContext;

        String sMainUrl = (String) wikiSettings.get("Url");
        m_sWikiUser = (String) wikiSettings.get("Username");
        m_sWikiPass = (String) wikiSettings.get("Password");
        m_sTitle = sTitle;

        m_aMainURL = new URL( sMainUrl );

        if ( bLogin )
        {
            final XComponentContext xDialogContext = xContext;
            final Hashtable aDialogSettings = wikiSettings;
            Callable<Boolean> aAskForLogin = new Callable<Boolean>()
            {
                public Boolean call()
                {
                    WikiEditSettingDialog aDialog = new WikiEditSettingDialog( xDialogContext, "vnd.sun.star.script:WikiEditor.EditSetting?location=application", aDialogSettings, false );
                    try
                    {
                        return Boolean.valueOf( aDialog.show() );
                    }
                    finally
                    {
                        aDialog.DisposeDialog();
                    }
                }
            };

            while( !Login() )
            {
                if ( aPropDialog != null )
                    aPropDialog.SetThrobberActive( false );

                if ( MainThreadDialogExecutor.Run( xContext, aAskForLogin ) )
                {
                    m_sWikiUser = (String) wikiSettings.get("Username");
                    m_sWikiPass = (String) wikiSettings.get("Password");
                }
                else
                    throw new WikiCancelException();

                if ( aPropDialog != null )
                {
                    aPropDialog.SetThrobberActive( true );
                    Thread.yield();
                }
            }
        }

        InitArticleExistence();
    }

    public String GetMainURL()
    {
        return m_aMainURL.toString();
    }

    public String GetTitle()
    {
        return m_sTitle;
    }

    private void InitArticleExistence()
        throws java.io.IOException, WikiCancelException
    {
        if ( m_aSession != null )
        {
            Document aResult = m_aSession.CallAPI( new String[][] {
                    { "action", "query" }, { "prop", "info" }, { "titles", m_sTitle } } );
            Element aPage = WikiSession.GetElement( aResult, "page" );
            m_bNoArticle = ( aPage == null || aPage.hasAttribute( "missing" ) || aPage.hasAttribute( "invalid" ) );
        }
    }

    protected boolean setArticle( String sWikiCode, String sWikiComment, boolean bMinorEdit )
        throws java.io.IOException, WikiCancelException
    {
        boolean bResult = false;

        if ( m_aSession != null && sWikiCode != null && sWikiComment != null )
        {
            String sToken = m_aSession.GetToken( "csrf" );
            if ( sToken == null )
                return false;

            Document aResult = m_aSession.CallAPI( new String[][] {
                    { "action", "edit" },
                    { "title", m_sTitle },
                    { "text", sWikiCode },
                    { "summary", sWikiComment },
                    { bMinorEdit ? "minor" : "notminor", "1" },
                    // fail instead of silently saving as an anonymous edit if the session was lost
                    { "assert", "user" },
                    // MediaWiki recommends sending the token last
                    { "token", sToken } } );

            Element aEdit = WikiSession.GetElement( aResult, "edit" );
            bResult = ( aEdit != null && "Success".equals( aEdit.getAttribute( "result" ) ) );
        }

        return bResult;
    }

    protected boolean Login()
        throws java.io.IOException, WikiCancelException
    {
        m_aSession = Helper.Login( m_aMainURL, m_sWikiUser, m_sWikiPass, m_xContext );
        return ( m_aSession != null );
    }

    protected boolean NotExist()
    {
        return m_bNoArticle;
    }
}
