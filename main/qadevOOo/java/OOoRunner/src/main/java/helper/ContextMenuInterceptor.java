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




package helper;

import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexContainer;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.ui.ActionTriggerSeparatorType;
import com.sun.star.ui.ContextMenuInterceptorAction;
import com.sun.star.ui.XContextMenuInterceptor;
import com.sun.star.uno.UnoRuntime;

public class ContextMenuInterceptor implements XContextMenuInterceptor {
    
    public ContextMenuInterceptorAction notifyContextMenuExecute(
            com.sun.star.ui.ContextMenuExecuteEvent aEvent ) throws RuntimeException {
        try {
            // Retrieve context menu container and query for service factory to
            // create sub menus, menu entries and separators
            XIndexContainer xContextMenu = aEvent.ActionTriggerContainer;
            XMultiServiceFactory xMenuElementFactory =
                    (XMultiServiceFactory)UnoRuntime.queryInterface(
                    XMultiServiceFactory.class, xContextMenu );
            
            if ( xMenuElementFactory != null ) {
                
                // create root menu entry for sub menu and sub menu
                XPropertySet xRootMenuEntry =
                        (XPropertySet)UnoRuntime.queryInterface(
                        XPropertySet.class,
                        xMenuElementFactory.createInstance("com.sun.star.ui.ActionTrigger" ));
                
                // create a line separator for our new help sub menu
                XPropertySet xSeparator =
                        (XPropertySet)UnoRuntime.queryInterface(
                        XPropertySet.class,
                        xMenuElementFactory.createInstance("com.sun.star.ui.ActionTriggerSeparator" ) );
                Short aSeparatorType = new Short( ActionTriggerSeparatorType.LINE );
                xSeparator.setPropertyValue( "SeparatorType", (Object)aSeparatorType );
                
                // query sub menu for index container to get access
                XIndexContainer xSubMenuContainer =
                        (XIndexContainer)UnoRuntime.queryInterface(
                        XIndexContainer.class,
                        xMenuElementFactory.createInstance("com.sun.star.ui.ActionTriggerContainer" ));
                
                // initialize root menu entry "Help"
                xRootMenuEntry.setPropertyValue( "Text", new String( "Help" ));
                xRootMenuEntry.setPropertyValue( "CommandURL", new String( "slot:5410" ));
                xRootMenuEntry.setPropertyValue( "HelpURL", new String( "5410" ));
                xRootMenuEntry.setPropertyValue( "SubContainer", (Object)xSubMenuContainer );
                
                // create menu entries for the new sub menu
                // initialize help/content menu entry
                // entry "Content"
                XPropertySet xMenuEntry = (XPropertySet)UnoRuntime.queryInterface(
                        XPropertySet.class, xMenuElementFactory.createInstance(
                        "com.sun.star.ui.ActionTrigger" ));
                xMenuEntry.setPropertyValue( "Text", new String( "Content" ));
                xMenuEntry.setPropertyValue( "CommandURL", new String( "slot:5401" ));
                xMenuEntry.setPropertyValue( "HelpURL", new String( "5401" ));
                
                // insert menu entry to sub menu
                xSubMenuContainer.insertByIndex( 0, (Object)xMenuEntry );
                
                // initialize help/help agent
                // entry "Help Agent"
                xMenuEntry = (XPropertySet)UnoRuntime.queryInterface(
                        XPropertySet.class,
                        xMenuElementFactory.createInstance("com.sun.star.ui.ActionTrigger" ));
                xMenuEntry.setPropertyValue( "Text", new String( "Help Agent" ));
                xMenuEntry.setPropertyValue( "CommandURL", new String( "slot:5962" ));
                xMenuEntry.setPropertyValue( "HelpURL", new String( "5962" ));
                
                // insert menu entry to sub menu
                xSubMenuContainer.insertByIndex( 1, (Object)xMenuEntry );
                // initialize help/tips
                // entry "Tips"
                xMenuEntry = (XPropertySet)UnoRuntime.queryInterface(
                        XPropertySet.class,
                        xMenuElementFactory.createInstance("com.sun.star.ui.ActionTrigger" ));
                xMenuEntry.setPropertyValue( "Text", new String( "Tips" ));
                xMenuEntry.setPropertyValue( "CommandURL", new String( "slot:5404" ));
                xMenuEntry.setPropertyValue( "HelpURL", new String( "5404" ));
                
                // insert menu entry to sub menu
                xSubMenuContainer.insertByIndex( 2, (Object)xMenuEntry );
                
                // add separator into the given context menu
                xContextMenu.insertByIndex( 1, (Object)xSeparator );
                
                // add new sub menu into the given context menu
                xContextMenu.insertByIndex( 1, (Object)xRootMenuEntry );
                
                // The controller should execute the modified context menu and stop notifying other
                // interceptors.
                return ContextMenuInterceptorAction.EXECUTE_MODIFIED ;
            }
        } catch ( UnknownPropertyException ex ) {
            // do something useful
            // we used a unknown property
        } catch ( IndexOutOfBoundsException ex ) {
            // do something useful
            // we used an invalid index for accessing a container
        } catch ( Exception ex ) {
            // something strange has happened!
        } catch ( Throwable ex ) {
            // catch java exceptions and do something useful
        }
        
        return ContextMenuInterceptorAction.IGNORED;
    }
}