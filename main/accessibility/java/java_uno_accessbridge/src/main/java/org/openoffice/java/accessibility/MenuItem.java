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


package org.openoffice.java.accessibility;

import com.sun.star.accessibility.XAccessible;
import com.sun.star.accessibility.XAccessibleContext;
import com.sun.star.accessibility.XAccessibleEventListener;


class MenuItem extends ToggleButton {
    public MenuItem(XAccessible xAccessible, XAccessibleContext xAccessibleContext) {
        super(xAccessible, xAccessibleContext);
    }

    protected class AccessibleMenuItemListener extends AccessibleUNOComponentListener {
    
        protected AccessibleMenuItemListener() {
        }
        
        protected void setComponentState(short state, boolean enable) {
                
            // #i56538# menu items in Java 1.5 are ARMED, not SELECTED
            if( state == com.sun.star.accessibility.AccessibleStateType.SELECTED )
                fireStatePropertyChange(javax.accessibility.AccessibleState.ARMED, enable);
            else
                super.setComponentState(state, enable);
        }        
    };

    protected XAccessibleEventListener createEventListener() {
        return new AccessibleMenuItemListener();
    }
    
    /** Creates the AccessibleContext associated with this object */
    public javax.accessibility.AccessibleContext createAccessibleContext() {
        return new AccessibleMenuItem();
    }

    protected class AccessibleMenuItem extends AccessibleToggleButton {
        /** Gets the role of this object */
        public javax.accessibility.AccessibleRole getAccessibleRole() {
            return javax.accessibility.AccessibleRole.MENU_ITEM;
        }

        /** Gets the 0-based index of this object in its accessible parent */
        public int getAccessibleIndexInParent() {
            if (getAccessibleParent() instanceof Menu) {
                return ((Menu) getAccessibleParent()).indexOf(MenuItem.this);
            } else {
                return super.getAccessibleIndexInParent();
            }
        }
        
        /**
        * Gets the current state set of this object.
        *
        * @return an instance of <code>AccessibleStateSet</code>
        *    containing the current state set of the object
        * @see javax.accessibility.AccessibleState
        */
        public javax.accessibility.AccessibleStateSet getAccessibleStateSet() {
            javax.accessibility.AccessibleStateSet stateSet = super.getAccessibleStateSet();
            
            // #i56538# menu items in Java do not have SELECTABLE ..
            stateSet.remove(javax.accessibility.AccessibleState.SELECTABLE);
            
            // .. and also ARMED instead of SELECTED
            if( stateSet.remove(javax.accessibility.AccessibleState.SELECTED) )
                stateSet.add(javax.accessibility.AccessibleState.ARMED);
            
            return stateSet;
        }
        
    }
}
