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


// __________ Imports __________

import com.sun.star.uno.UnoRuntime;

import java.awt.*;
import javax.swing.*;
import java.lang.String;
import java.awt.event.*;
import java.awt.*;

// __________ Implementation __________

/**
 * Makes it possible to change some states of currently loaded
 * document (e.g. enable/disable menubar, toolbar, objectbar)
 *
 * @author     Andreas Schl&uuml;ns
 * @created    20.06.2002 09:28
 */
public class CustomizeView extends    JPanel
                           implements IShutdownListener
{
    // ____________________
    // const

    /**
     * These const URL's describe feature for toggling some properties of loaded document.
     * Dispatch it with the corresponding parameter to the frame.
     */
    private static final String FEATUREURL_MENUBAR      = "slot:6661"         ;
    private static final String FEATUREURL_TOOLBAR      = "slot:5909"         ;
    private static final String FEATUREURL_OBJECTBAR    = "slot:5905"         ;

    private static final String FEATUREPROP_MENUBAR     = "MenuBarVisible"    ;
    private static final String FEATUREPROP_TOOLBAR     = "ToolBarVisible"    ;
    private static final String FEATUREPROP_OBJECTBAR   = "ObjectBarVisible"  ;

    private static final String ACTION_MENUBAR          = "toogle_menu"       ;
    private static final String ACTION_TOOLBAR          = "toogle_toolbar"    ;
    private static final String ACTION_OBJECTBAR        = "toogle_objectbar"  ;

    private static final String MENUBAR_ON              = "menubar on"        ;
    private static final String TOOLBAR_ON              = "toolbar on"        ;
    private static final String OBJECTBAR_ON            = "objectbar on"      ;

    private static final String MENUBAR_OFF             = "menubar off"       ;
    private static final String TOOLBAR_OFF             = "toolbar off"       ;
    private static final String OBJECTBAR_OFF           = "objectbar off"     ;

    // ____________________
    // member

    /**
     * @member  m_cbMenuBar             reference to checkbox for toggling menubar
     * @member  m_cbToolBar             reference to checkbox for toggling toolbar
     * @member  m_cbObjectBar           reference to checkbox for toggling objectbar
     *
     * @member  m_aMenuBarListener      listener for status events of the menu bar
     * @member  m_aToolBarListener      listener for status events of the tool bar
     * @member  m_aObjectBarListener    listener for status events of the object bar
     */
    private JCheckBox           m_cbMenuBar         ;
    private JCheckBox           m_cbToolBar         ;
    private JCheckBox           m_cbObjectBar       ;

    private StatusListener      m_aMenuBarListener  ;
    private StatusListener      m_aToolBarListener  ;
    private StatusListener      m_aObjectBarListener;

    // ____________________

    /**
     * ctor
     * Create view controls on startup and initialize it.
     * We don't start listening here. see setFrame()!
     */
    CustomizeView()
    {
        this.setLayout(new GridLayout(3,0));

        m_cbMenuBar   = new JCheckBox(MENUBAR_OFF  , false);
        m_cbToolBar   = new JCheckBox(TOOLBAR_OFF  , false);
        m_cbObjectBar = new JCheckBox(OBJECTBAR_OFF, false);

        m_cbMenuBar.setEnabled  (false);
        m_cbToolBar.setEnabled  (false);
        m_cbObjectBar.setEnabled(false);

        m_cbMenuBar.setActionCommand  (ACTION_MENUBAR  );
        m_cbToolBar.setActionCommand  (ACTION_TOOLBAR  );
        m_cbObjectBar.setActionCommand(ACTION_OBJECTBAR);

        this.add(m_cbMenuBar  );
        this.add(m_cbToolBar  );
        this.add(m_cbObjectBar);
    }

    // ____________________

    /**
     * set new frame for this view
     * We start listening for frame action/status and click events instandly.
     * If an event occure we use it to synchronize our controls
     * with states of a (my be) new document view of this frame.
     *
     * @param xFrame
     *          the reference to the frame, which provides the
     *          possibility to get the required status informations
     *
     *          Attention: We don't accept new frames here.
     *          We get one after startup and work with him.
     *          That's it!
     */
    public void setFrame(com.sun.star.frame.XFrame xFrame)
    {
        if (xFrame==null)
            return;

        // be listener for click events
        // They will toogle the UI controls.
        ClickListener aMenuBarHandler   = new ClickListener(FEATUREURL_MENUBAR  ,FEATUREPROP_MENUBAR  ,xFrame);
        ClickListener aToolBarHandler   = new ClickListener(FEATUREURL_TOOLBAR  ,FEATUREPROP_TOOLBAR  ,xFrame);
        ClickListener aObjectBarHandler = new ClickListener(FEATUREURL_OBJECTBAR,FEATUREPROP_OBJECTBAR,xFrame);

        m_cbMenuBar.addActionListener  (aMenuBarHandler  );
        m_cbToolBar.addActionListener  (aToolBarHandler  );
        m_cbObjectBar.addActionListener(aObjectBarHandler);

        // be frame action listener
        // The callback will update listener connections
        // for status updates automatically!
        m_aMenuBarListener   = new StatusListener(m_cbMenuBar  ,MENUBAR_ON  ,MENUBAR_OFF  ,xFrame, FEATUREURL_MENUBAR  );
        m_aToolBarListener   = new StatusListener(m_cbToolBar  ,TOOLBAR_ON  ,TOOLBAR_OFF  ,xFrame, FEATUREURL_TOOLBAR  );
        m_aObjectBarListener = new StatusListener(m_cbObjectBar,OBJECTBAR_ON,OBJECTBAR_OFF,xFrame, FEATUREURL_OBJECTBAR);

        m_aMenuBarListener.startListening();
        m_aToolBarListener.startListening();
        m_aObjectBarListener.startListening();
    }

    // ____________________

    /**
     * react for click events of the used check boxes
     * We use our internal set dispatch objects to
     * call it. This calls toogle the menu/object- or toolbar.
     * Note: Because we are listener status events too - hopefully
     * we get a notification, if toogling was successfully or not.
     * We use this information to update our check boxes again.
     * But such update doesn't force (hopefully) an action event. Otherwise
     * we can produce a never ending recursion!
     *
     * @param aEvent
     *          describes the used check box and his current state
     *          we can use to dispatch the right URL to the office
     */
    class ClickListener implements ActionListener,
                                   com.sun.star.lang.XEventListener
    {
        /// URL, to toogle the requested UI item
        String m_sURL;
        /// name of the property which must be used in combination with the URL
        String m_sProp;
        /// we must use this frame to dispatch a request
        com.sun.star.frame.XFrame m_xFrame;

        //_____________________

        /**
         * ctor
         * It initialize an instance of this clas only.
         */
        ClickListener( String                    sURL   ,
                       String                    sProp  ,
                       com.sun.star.frame.XFrame xFrame )
        {
            m_sURL   = sURL  ;
            m_sProp  = sProp ;
            m_xFrame = xFrame;
        }

        //_____________________

        /**
         * callback for action events
         * Such events occure if somehwere click the
         * JCheckBox control on which we are registered.
         * Such events doesn't occure if we set it programmaticly
         * (e.g. if we get status events to -> see class StatusListener too)
         *
         * @param aEvent
         *          describes the check box and his state
         *          we can use to toogle the requested office
         *          ressource.
         */
        public void actionPerformed(ActionEvent aEvent)
        {
            synchronized(this)
            {
                if (m_xFrame==null)
                    return;
            }

            // define parameters for following dispatch
            boolean bState = ((JCheckBox)aEvent.getSource()).isSelected();

            // prepare the dispatch
            com.sun.star.util.URL aURL = FunctionHelper.parseURL(m_sURL);
            if (aURL==null)
                return;

            com.sun.star.beans.PropertyValue[] lProperties = new com.sun.star.beans.PropertyValue[1];
            lProperties[0]       = new com.sun.star.beans.PropertyValue();
            lProperties[0].Name  = m_sProp;
            lProperties[0].Value = new Boolean(bState);

            // execute (dispatch) it into the frame
            if (m_xFrame==null)
                return;
            FunctionHelper.execute(m_xFrame,aURL,lProperties,null);
        }

        // ____________________

        /**
         * callback for disposing events
         * Internally we save a reference to an office frame.
         * Of course he can die and inform us then. We should react
         * and forget his reference.
         *
         * @param aEvent
         *          describes the source which fire this event
         *          Must be our internal saved frame. Otherwise
         *          somewhere know us without a registration ...
         */
        public void disposing(com.sun.star.lang.EventObject aEvent)
        {
            synchronized(this)
            {
                m_xFrame = null;
            }
        }
    }

    // ____________________

    /**
     * If this java application shutdown - we must cancel all current existing
     * listener connections. Otherwise the office will run into some
     * DisposedExceptions if it tries to use these forgotten listener references.
     * And of course it can die doing that.
     * We are registered at a central object to be informed if the VM will exit.
     * So we can react.
     */
    public void shutdown()
    {
        m_aMenuBarListener.shutdown();
        m_aToolBarListener.shutdown();
        m_aObjectBarListener.shutdown();

        m_aMenuBarListener   = null;
        m_aToolBarListener   = null;
        m_aObjectBarListener = null;
    }
}
