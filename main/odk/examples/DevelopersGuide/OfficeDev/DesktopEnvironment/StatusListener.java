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

import java.lang.String;
import java.awt.Component;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import java.util.Vector;

// __________ Implementation __________

/**
 * reacts for status events we listen for
 * We listen for status events to update our UI.
 * To know which event must be used for which UI control
 * we use a special class to do that. Otherwise we have
 * to guess it ...
 *
 * Further we are frame action listener too.
 * So we can update our status listener connections and
 * internal holded dispatch object automatically.
 *
 * Another reason for such extra class for listening:
 * Most listener callbacks are asynchronous [oneway] requests.
 * And it's not allowed to call back synchronously there.
 * So we must start threads for updating something internally.
 *
 * @author     Andreas Schl&uuml;ns
 * @created    15.07.2002 12:36
 */
class StatusListener implements com.sun.star.frame.XStatusListener,
                                com.sun.star.frame.XFrameActionListener,
                                IShutdownListener,
                                IOnewayLink
{
    //_____________________

    /**
     * @member m_rControl           reference to the UI control, which should be updated
     * @member m_sTrueText          this text will be shown at the used UI control as description for an enabled status
     * @member m_sFalseText         this text will be shown at the used UI control as description for an disabled status
     * @member m_xDispatch          if we listen for status events, we must hold the dispatch object alive!
     * @member m_xFrame             reference to the frame, which can provide new dispatch objects if it's necessary to update it
     * @member m_aURL               and of course we must be registered for a special URL
     * @member m_bIsActionListener  indicates if we are currently registered as a listener for frame action events or not
     * @member m_bIsStatusListener  indicates if we are currently registered as a listener for status events or not
     * @member m_bDead              there exist more then one way to finish an object of this class - we must know it sometimes
     */
    private Component                    m_rControl         ;
    private String                       m_sTrueText        ;
    private String                       m_sFalseText       ;
    private com.sun.star.frame.XDispatch m_xDispatch        ;
    private com.sun.star.frame.XFrame    m_xFrame           ;
    private com.sun.star.util.URL        m_aURL             ;
    private boolean                      m_bIsActionListener;
    private boolean                      m_bIsStatusListener;
    private boolean                      m_bDead            ;

    //_____________________

    /**
     * ctor
     * It initialize an instance of this class only.
     * We sett all necessary informations on our internal member - that's it
     */
    StatusListener( /*IN*/ Component                   rControl   ,
                    /*IN*/ String                      sTrueText  ,
                    /*IN*/ String                      sFalseText ,
                    /*IN*/ com.sun.star.frame.XFrame   xFrame     ,
                    /*IN*/ String                      sURL       )
    {
        m_rControl          = rControl   ;
        m_sTrueText         = sTrueText  ;
        m_sFalseText        = sFalseText ;
        m_xFrame            = xFrame     ;
        m_bIsStatusListener = false      ;
        m_bIsActionListener = false      ;
        m_bDead             = false      ;
        // to be perform - we parse the given URL  one times only
        // and use it till we die ...
        m_aURL = FunctionHelper.parseURL(sURL);
    }

    //_____________________

    /**
     * start working as frame action listener really.
     * In case we get such frame action, it indicates that we should
     * update our internal saved dispatch object on which we listen
     * for status events. So we can do it automatically. The outside code
     * mustn't check such things. We can work with one frame,
     * till it die. It doesn't matter if he will be used for different
     * load/save or any other requests. We will be up to date every time.
     */
    public void startListening()
    {
        com.sun.star.frame.XFrame xFrame = null;
        synchronized(this)
        {
            if (m_bDead)
                return;
            if (m_xFrame==null)
                return;
            if (m_bIsActionListener==true)
                return;
            xFrame = m_xFrame;
        }
        xFrame.addFrameActionListener(this);
        synchronized(this)
        {
            m_bIsActionListener=true;
        }
    }

    //_____________________

    /**
     * In case we got an oneway listener callback - we had to use the office
     * asynchronous then. This method is the callback from the started thread
     * (started inside the original oneway method). We found all parameters of
     * the original request packed inside a vector. Here we unpack it and
     * call the right internal helper method, which implements the right
     * funtionality.
     *
     * @seealso frameAction()
     * @seealso statusChanged()
     *
     * @param nRequest
     *          indicates, which was the original request (identifies the
     *          original called method)
     *
     * @param lParams
     *          the vector with all packed parameters of the original request
     */
    public void execOneway(/*IN*/  int nRequest,/*IN*/  Vector lParams )
    {
        synchronized(this)
        {
            if (m_bDead)
                return;
        }
        // was it frameAction()?
        if (nRequest==OnewayExecutor.REQUEST_FRAMEACTION)
        {
            com.sun.star.frame.FrameActionEvent[] lOutAction   = new com.sun.star.frame.FrameActionEvent[1];
            Vector[]                              lInParams    = new Vector[1];
                                                  lInParams[0] = lParams;

            OnewayExecutor.codeFrameAction( OnewayExecutor.DECODE_PARAMS ,
                                            lInParams                    ,
                                            lOutAction                   );
            impl_frameAction(lOutAction[0]);
        }
    }

    //_____________________

    /**
     * This is the callback method for such frame action events, we listen for.
     * Because it's a oneway method we start a thread as reaction. This thread call
     * us back and we can do necessary things there.
     * But we shouldn't start such action - if it's not really necessary.
     * So we check before, if we are intereested on this event really.
     *
     * @see impl_frameAction()
     *
     * @param aEvent
     *          describes the action, which triggered this event
     */
    public /*ONEWAY*/ void frameAction(/*IN*/ com.sun.star.frame.FrameActionEvent aEvent)
    {
        synchronized(this)
        {
            if (m_bDead)
                return;
        }
        boolean bHandle = false;
        switch(aEvent.Action.getValue())
        {
            case com.sun.star.frame.FrameAction.COMPONENT_ATTACHED_value   : bHandle=true; break;
            case com.sun.star.frame.FrameAction.COMPONENT_DETACHING_value  : bHandle=true; break;
            case com.sun.star.frame.FrameAction.COMPONENT_REATTACHED_value : bHandle=true; break;
            case com.sun.star.frame.FrameAction.CONTEXT_CHANGED_value      : bHandle=true; break;
        }

        if (! bHandle)
            return;

        Vector[]                              lOutParams   = new Vector[1];
        com.sun.star.frame.FrameActionEvent[] lInAction    = new com.sun.star.frame.FrameActionEvent[1];
                                              lInAction[0] = aEvent;

        OnewayExecutor.codeFrameAction( OnewayExecutor.ENCODE_PARAMS ,
                                        lOutParams                   ,
                                        lInAction                    );
        OnewayExecutor aExecutor = new OnewayExecutor( (IOnewayLink)this                  ,
                                                       OnewayExecutor.REQUEST_FRAMEACTION ,
                                                       lOutParams[0]                      );
        aExecutor.start();
    }

    //_____________________

    /**
     * This is the callback method for the status we listen for an wish to show it
     * on our UI control. Of yourse it's a oneway method ... but we doesn't call back
     * to the office synchronously here. We update our UI only. So we don't leave this
     * java process. In such case it's not necessary to use threads to decouple it.
     * Do it here and now ...
     *
     * @param aEvent
     *          describes the status, we can use to update our UI control
     */
    public /*ONEWAY*/ void statusChanged(/*IN*/ com.sun.star.frame.FeatureStateEvent aEvent)
    {
        synchronized(this)
        {
            if (m_bDead)
                return;

            // enable/dsiable th control.
            // Means: If the feature isn't available currently - we can't show an status really here.
            // Then we should colorize it gray ...
            m_rControl.setEnabled(aEvent.IsEnabled);

            // Only if status is enabled we can look for his value!
            if (aEvent.IsEnabled)
            {
                // look for the right type ofthe UI control
                // Following actions depend on it.

                //.............................................................
                // it's a check box
                if (m_rControl instanceof JCheckBox)
                {
                    JCheckBox aBox = (JCheckBox)m_rControl;

                    // State must be a boolean value too. Otherwise must
                    // ignore this event.
                    if ( ! (aEvent.State instanceof Boolean ) )
                        return;

                    boolean bState = ((Boolean)(aEvent.State)).booleanValue();
                    aBox.setSelected(bState);
                    if (bState)
                        aBox.setText(m_sTrueText);
                    else
                        aBox.setText(m_sFalseText);
                }
                else
                //.............................................................
                // it's a label
                if (m_rControl instanceof JLabel)
                {
                    JLabel aLabel = (JLabel)m_rControl;

                    // Detect type of state value
                    // and set it on internal well known UI control
                    // But do it only, if value really change.
                    if(aEvent.State instanceof String)
                    {
                        String sState = (String)aEvent.State;
                        aLabel.setText(sState);
                    }
                    else
                    if(aEvent.State instanceof Boolean)
                    {
                        boolean bState = ((Boolean)aEvent.State).booleanValue();
                        if (bState)
                            aLabel.setText(m_sTrueText);
                        else
                            aLabel.setText(m_sFalseText);
                    }
                    else
                    if(aEvent.State instanceof Float)
                    {
                        String sState = ((Float)aEvent.State).toString();
                        aLabel.setText(sState);
                    }
                }
            }
        }
    }

    //_____________________

    /**
     * Internal call back for frame action events, triggered by the used
     * OnewayExecutor thread we started in frameAction().
     * We use it to update internal saved dispatch object and the corresponding
     * listener connection for status events.
     *
     * @param aEvent
     *          describes the action
     */
    public void impl_frameAction(/*IN*/ com.sun.star.frame.FrameActionEvent aEvent)
    {
        synchronized(this)
        {
            if (m_bDead)
                return;
        }
        // Don't look for ignoring actions - it was done already inside original frameAction() call!
        // deregistration as status listener will be done here every time - but registration only, if necessary!
        boolean bRegister = false;
        switch(aEvent.Action.getValue())
        {
            case com.sun.star.frame.FrameAction.COMPONENT_ATTACHED_value   : bRegister=true ; break;
            case com.sun.star.frame.FrameAction.COMPONENT_DETACHING_value  : bRegister=false; break;
            case com.sun.star.frame.FrameAction.COMPONENT_REATTACHED_value : bRegister=true ; break;
            case com.sun.star.frame.FrameAction.CONTEXT_CHANGED_value      : bRegister=true ; break;
        }

        boolean                      bIsStatusListener = false;
        com.sun.star.frame.XFrame    xFrame            = null ;
        com.sun.star.frame.XDispatch xDispatch         = null ;
        com.sun.star.util.URL        aURL              = null ;
        synchronized(this)
        {
            bIsStatusListener   = m_bIsStatusListener;
            m_bIsStatusListener = false;

            xDispatch   = m_xDispatch;
            m_xDispatch = null;

            aURL        = m_aURL;
            xFrame      = m_xFrame;
        }

        if (bIsStatusListener)
            xDispatch.removeStatusListener(this,aURL);
        xDispatch = null;

        if (! bRegister)
            return;

        com.sun.star.frame.XDispatchProvider xProvider = (com.sun.star.frame.XDispatchProvider)UnoRuntime.queryInterface(
            com.sun.star.frame.XDispatchProvider.class,
            xFrame);

        if (xProvider==null)
            return;

        xDispatch = xProvider.queryDispatch(aURL,"",0);

        if (xDispatch==null)
            return;

        xDispatch.addStatusListener(this,aURL);
        synchronized(this)
        {
            m_xDispatch         = xDispatch;
            m_bIsStatusListener = true;
        }
    }

    // ____________________

    /**
     * callback for disposing events
     * Our dispatch or frame object inform us about his following dead ...
     * So we must forget his reference. But it's not necessary to
     * remove listener connections here. Because the broadcaster
     * forget us automatically. The only thing we have to do: release
     * his reference and let him die!
     *
     * @param aEvent
     *          describes the source which fire this event
     *          Must be our internal saved dispatch or frame. Otherwise
     *          somewhere know us without a registration ...
     */
    public /*ONEWAY*/ void disposing(/*IN*/ com.sun.star.lang.EventObject aEvent)
    {
        synchronized(this)
        {
            if (m_bDead)
                return;
            if (m_xFrame!=null && UnoRuntime.areSame(aEvent.Source,m_xFrame))
            {
                m_bIsActionListener = false;
                m_xFrame            = null ;
            }
            else
            if (m_xDispatch!=null && UnoRuntime.areSame(aEvent.Source,m_xDispatch))
            {
                m_bIsStatusListener = false;
                m_xDispatch         = null ;
                m_aURL              = null ;
            }
        }
        shutdown();
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
        boolean                      bIsActionListener = false;
        boolean                      bIsStatusListener = false;
        com.sun.star.frame.XFrame    xFrame            = null ;
        com.sun.star.frame.XDispatch xDispatch         = null ;
        com.sun.star.util.URL        aURL              = null ;
        synchronized(this)
        {
            // don't react a second time here!
            if (m_bDead)
                return;
            m_bDead = true;

            bIsActionListener   = m_bIsActionListener;
            m_bIsActionListener = false;

            bIsStatusListener   = m_bIsStatusListener;
            m_bIsStatusListener = false;

            xFrame              = m_xFrame;
            m_xFrame            = null;

            xDispatch           = m_xDispatch;
            m_xDispatch         = null;

            aURL                = m_aURL;
            m_aURL              = null;
        }

        if (bIsStatusListener)
            xDispatch.removeStatusListener(this,aURL);
        xDispatch = null ;
        aURL      = null ;

        if (bIsActionListener)
            xFrame.removeFrameActionListener(this);
        xFrame = null ;
    }
}
