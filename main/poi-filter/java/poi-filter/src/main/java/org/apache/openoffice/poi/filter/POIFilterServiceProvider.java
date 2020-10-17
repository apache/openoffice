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

package org.apache.openoffice.poi.filter;

import com.sun.star.lib.uno.helper.Factory;;
import com.sun.star.lang.XSingleComponentFactory;
import com.sun.star.registry.XRegistryKey;

public class POIFilterServiceProvider {
    
    public static XSingleComponentFactory __getComponentFactory(String implName) {
        
        System.out.println("Asked to create " + implName);

        XSingleComponentFactory xSingleComponentFactory = null;
//        if (implName.equals( FormatDetector.class.getName()) )
//            xSingleComponentFactory = Factory.createComponentFactory( FormatDetector.class,
//                   FormatDetector.__serviceName, multiFactory, regKey); 
        if (implName.equals(ExcelFilter.class.getName()))
            xSingleComponentFactory = Factory.createComponentFactory( ExcelFilter.class,
                    ExcelFilter.class.getName(), ExcelFilter.getSupportedServiceNamesStatic());
        System.out.println("Returning " + xSingleComponentFactory.toString());
        return xSingleComponentFactory;
    }

    public static boolean __writeRegistryServiceInfo(XRegistryKey regKey) {
        boolean registered = true;
        registered |= Factory.writeRegistryServiceInfo(
            ExcelFilter.class.getName(),
            ExcelFilter.getSupportedServiceNamesStatic(),
            regKey);
        return registered;
    }
}
