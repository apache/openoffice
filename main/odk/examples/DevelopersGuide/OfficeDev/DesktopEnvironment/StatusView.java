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

// __________ Implementation __________

/**
 * Implement a view to show status informations
 * of currently loaded document of a document view.
 * It use separate listener threads to get this informations
 * and actualize it automatically if frame broadcast changes of
 * his contained document.
 * Threads are necessary to prevent this view against deadlocks.
 * These deadlocks can occur if a listener will be notified
 * by the office in an "oneway" method and try to call back
 * to the office by using a synchronous method.
 * UNO must guarantee order of all these calls ... and if
 * the source of arrived event holds a mutex and our synchronous
 * call needs this mutex too => a deadlock occur.
 * Why? UNO had created a new thread for our synchronous call
 * inside the office process and so exist different threads
 * for this constellation.
 *
 * @author     Andreas Schl&uuml;ns
 * @created    20.06.2002 15:08
 */
public class StatusView extends    JPanel
                        implements IShutdownListener
{
    // ____________________

    /**
     * const
     * These URL's describe available feature states.
     */
    public static final String FEATUREURL_FONT      = "slot:10007";
    public static final String FEATUREURL_SIZE      = "slot:10015";
    public static final String FEATUREURL_BOLD      = "slot:10009";
    public static final String FEATUREURL_ITALIC    = "slot:10008";
    public static final String FEATUREURL_UNDERLINE = "slot:10014";

    // ____________________

    /**
     * const
     * These values are used to show current state of showed feature.
     */
    public static final String FONT_OFF         = "unknown" ;
    public static final String SIZE_OFF         = "0.0"     ;
    public static final String BOLD_OFF         = "-"       ;
    public static final String ITALIC_OFF       = "-"       ;
    public static final String UNDERLINE_OFF    = "-"       ;

    public static final String FONT_ON          = ""        ;
    public static final String SIZE_ON          = ""        ;
    public static final String BOLD_ON          = "X"       ;
    public static final String ITALIC_ON        = "X"       ;
    public static final String UNDERLINE_ON     = "X"       ;

    // ____________________

    /**
     * @member  mlaFontValue            shows status of font name
     * @member  mlaSizeValue            shows status of font size
     * @member  mlaBoldValue            shows status of font style bold
     * @member  mlaUnderlineValue       shows status of font style underline
     * @member  mlaItalicValue          shows status of font style italic
     *
     * @member  maFontListener          threadsafe(!) helper to listen for status event which describe font name
     * @member  maSizeListener          threadsafe(!) helper to listen for status event which describe font size
     * @member  maBoldListener          threadsafe(!) helper to listen for status event which describe font style bold
     * @member  maUnderlineListener     threadsafe(!) helper to listen for status event which describe font style underline
     * @member  maItalicListener        threadsafe(!) helper to listen for status event which describe font style italic
     */
    private JLabel              m_laFontValue            ;
    private JLabel              m_laSizeValue            ;
    private JLabel              m_laBoldValue            ;
    private JLabel              m_laUnderlineValue       ;
    private JLabel              m_laItalicValue          ;

    private StatusListener      m_aFontListener          ;
    private StatusListener      m_aSizeListener          ;
    private StatusListener      m_aBoldListener          ;
    private StatusListener      m_aUnderlineListener     ;
    private StatusListener      m_aItalicListener        ;

    // ____________________

    /**
     * ctor
     * Create view controls on startup and initialize it with default values.
     * Filling of view items can be done by special set-methods.
     * We don't start listening here! see setFrame() for that ...
     */
    StatusView()
    {
        this.setLayout(new GridBagLayout());

        GridBagConstraints aConstraint = new GridBagConstraints();
        aConstraint.anchor = GridBagConstraints.NORTHWEST;
        aConstraint.insets = new Insets(2,2,2,2);
        aConstraint.gridy  = 0;
        aConstraint.gridx  = 0;

        JLabel laFont      = new JLabel("Font"     );
        JLabel laSize      = new JLabel("Size"     );
        JLabel laBold      = new JLabel("Bold"     );
        JLabel laUnderline = new JLabel("Underline");
        JLabel laItalic    = new JLabel("Italic"   );

        m_laFontValue       = new JLabel();
        m_laSizeValue       = new JLabel();
        m_laBoldValue       = new JLabel();
        m_laUnderlineValue  = new JLabel();
        m_laItalicValue     = new JLabel();

        aConstraint.gridx = 0;
        this.add( laFont, aConstraint );
        aConstraint.gridx = 1;
        this.add( m_laFontValue, aConstraint );

        ++aConstraint.gridy;

        aConstraint.gridx = 0;
        this.add( laSize, aConstraint );
        aConstraint.gridx = 1;
        this.add( m_laSizeValue, aConstraint );

        ++aConstraint.gridy;

        aConstraint.gridx = 0;
        this.add( laSize, aConstraint );
        aConstraint.gridx = 1;
        this.add( m_laSizeValue, aConstraint );

        ++aConstraint.gridy;

        aConstraint.gridx = 0;
        this.add( laBold, aConstraint );
        aConstraint.gridx = 1;
        this.add( m_laBoldValue, aConstraint );

        ++aConstraint.gridy;

        aConstraint.gridx = 0;
        this.add( laUnderline, aConstraint );
        aConstraint.gridx = 1;
        this.add( m_laUnderlineValue, aConstraint );

        ++aConstraint.gridy;

        aConstraint.gridx = 0;
        this.add( laItalic, aConstraint );
        aConstraint.gridx = 1;
        this.add( m_laItalicValue, aConstraint );

        m_laFontValue.setEnabled     (false);
        m_laSizeValue.setEnabled     (false);
        m_laBoldValue.setEnabled     (false);
        m_laItalicValue.setEnabled   (false);
        m_laUnderlineValue.setEnabled(false);

        m_laFontValue.setText     (FONT_OFF     );
        m_laSizeValue.setText     (SIZE_OFF     );
        m_laBoldValue.setText     (BOLD_OFF     );
        m_laItalicValue.setText   (ITALIC_OFF   );
        m_laUnderlineValue.setText(UNDERLINE_OFF);
    }

    // ____________________

    /**
     * Set new frame for this view and start listening for events immediately.
     * We create one status listener for every control we wish to update.
     * And because the environment of the frame can be changed - these
     * listener refresh himself internally for frame action events too.
     * So we register it as such frame action listener only here.
     * Rest is done automatically ...
     *
     * @param xFrame
     *          will be used as source of possible status events
     */
    public void setFrame(com.sun.star.frame.XFrame xFrame)
    {
        if (xFrame==null)
            return;

        // create some listener on given frame for available status events
        // Created listener instances will register herself on this frame and
        // show her received informations automatically on set UI controls.
        m_aFontListener      = new StatusListener(m_laFontValue     ,FONT_ON     ,FONT_OFF     ,xFrame, FEATUREURL_FONT     );
        m_aSizeListener      = new StatusListener(m_laSizeValue     ,SIZE_ON     ,SIZE_OFF     ,xFrame, FEATUREURL_SIZE     );
        m_aBoldListener      = new StatusListener(m_laBoldValue     ,BOLD_ON     ,BOLD_OFF     ,xFrame, FEATUREURL_BOLD     );
        m_aItalicListener    = new StatusListener(m_laItalicValue   ,ITALIC_ON   ,ITALIC_OFF   ,xFrame, FEATUREURL_ITALIC   );
        m_aUnderlineListener = new StatusListener(m_laUnderlineValue,UNDERLINE_ON,UNDERLINE_OFF,xFrame, FEATUREURL_UNDERLINE);

        m_aFontListener.startListening();
        m_aSizeListener.startListening();
        m_aBoldListener.startListening();
        m_aItalicListener.startListening();
        m_aUnderlineListener.startListening();
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
        m_aFontListener.shutdown();
        m_aSizeListener.shutdown();
        m_aBoldListener.shutdown();
        m_aItalicListener.shutdown();
        m_aUnderlineListener.shutdown();

        m_aFontListener      = null;
        m_aSizeListener      = null;
        m_aBoldListener      = null;
        m_aItalicListener    = null;
        m_aUnderlineListener = null;
    }
}
