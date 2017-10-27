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

package org.apache.openoffice.comp.sdbc.dbtools.sdbcx.descriptors;

import org.apache.openoffice.comp.sdbc.dbtools.comphelper.PropertySetAdapter.PropertyGetter;
import org.apache.openoffice.comp.sdbc.dbtools.comphelper.PropertySetAdapter.PropertySetter;
import org.apache.openoffice.comp.sdbc.dbtools.sdbcx.OContainer;
import org.apache.openoffice.comp.sdbc.dbtools.sdbcx.ODescriptor;
import org.apache.openoffice.comp.sdbc.dbtools.util.PropertyIds;

import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XNameAccess;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.sdbcx.XColumnsSupplier;
import com.sun.star.sdbcx.XKeysSupplier;
import com.sun.star.uno.Type;

public class SdbcxTableDescriptor extends ODescriptor implements XColumnsSupplier, XKeysSupplier, XServiceInfo {

    private static final String[] services = {
            "com.sun.star.sdbcx.TableDescriptor"
    };
    
    protected String catalogName;
    protected String schemaName;
    protected String description;
    
    private OContainer columns;
    private OContainer keys;
    
    public SdbcxTableDescriptor(boolean isCaseSensitive) {
        super("", isCaseSensitive, false);
        columns = new SdbcxColumnDescriptorContainer(this, isCaseSensitive());
        keys = new SdbcxKeyDescriptorContainer(this, isCaseSensitive());
        registerProperties();
    }
    
    private void registerProperties() {
        registerProperty(PropertyIds.CATALOGNAME.name, PropertyIds.CATALOGNAME.id, Type.STRING, (short)0,
                new PropertyGetter() {
                    @Override
                    public Object getValue() {
                        return catalogName;
                        
                    }
                },
                new PropertySetter() {
                    @Override
                    public void setValue(Object value) {
                        catalogName = (String) value;
                    }
                });
        registerProperty(PropertyIds.SCHEMANAME.name, PropertyIds.SCHEMANAME.id, Type.STRING, (short)0,
                new PropertyGetter() {
                    @Override
                    public Object getValue() {
                        return schemaName;
                        
                    }
                },
                new PropertySetter() {
                    @Override
                    public void setValue(Object value) {
                        schemaName = (String) value;
                    }
                });
        registerProperty(PropertyIds.DESCRIPTION.name, PropertyIds.DESCRIPTION.id, Type.STRING, (short)0,
                new PropertyGetter() {
                    @Override
                    public Object getValue() {
                        return description;
                        
                    }
                },
                new PropertySetter() {
                    @Override
                    public void setValue(Object value) {
                        description = (String) value;
                    }
                });
    }
    
    @Override
    public XNameAccess getColumns() {
        return columns;
    }
    
    @Override
    public XIndexAccess getKeys() {
        return keys;
    }
    
    // XServiceInfo
    
    public String getImplementationName() {
        return getClass().getName();
    }
    
    @Override
    public String[] getSupportedServiceNames() {
        return services.clone();
    }
    
    @Override
    public boolean supportsService(String serviceName) {
        for (String service : getSupportedServiceNames()) {
            if (service.equals(serviceName)) {
                return true;
            }
        }
        return false;
    }
}
