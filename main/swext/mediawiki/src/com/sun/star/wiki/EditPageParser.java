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

import javax.swing.text.html.*;
import javax.swing.text.MutableAttributeSet;

public class EditPageParser extends HTMLEditorKit.ParserCallback
{

    protected String m_sMainURL = "";

    private boolean m_bInHead = false;
    private boolean m_bAPIFound = false;

    /** Creates a new instance of WikiHTMLParser */
    public EditPageParser()
    {
    }

    public void handleEndTag( HTML.Tag t,int pos )
    {
        if ( t == HTML.Tag.HEAD )
        {
            m_bInHead = false;
        }
    }

    public void handleSimpleTag( HTML.Tag t, MutableAttributeSet a,int pos )
    {
        if ( t == HTML.Tag.LINK && m_bInHead && !m_bAPIFound )
        {
            String sHref = ( String ) a.getAttribute( HTML.Attribute.HREF );
            if ( sHref != null )
            {
                // MediaWiki advertises its API with <link rel="EditURI" href=".../api.php?action=rsd">
                String sRel = ( String ) a.getAttribute( HTML.Attribute.REL );
                int nAPIStart = sHref.indexOf( "api.php" );
                if ( sRel != null && sRel.equalsIgnoreCase( "EditURI" ) && nAPIStart >= 0 )
                {
                    m_sMainURL = sHref.substring( 0, nAPIStart );
                    m_bAPIFound = true;
                    return;
                }

                int nIndexStart = sHref.indexOf( "index.php" );
                // otherwise get the main URL from the first header-link with index.php
                // the link with "action=edit" inside is preferable
                if ( nIndexStart>= 0
                  && ( m_sMainURL.length() == 0 || sHref.indexOf( "action=edit" ) >= 0 ) )
                {
                    m_sMainURL = sHref.substring( 0, nIndexStart );
                }
            }
        }
    }

    public void handleStartTag( HTML.Tag t, MutableAttributeSet a,int pos )
    {
        if ( t == HTML.Tag.HEAD )
        {
            m_bInHead = true;
        }
    }
}
