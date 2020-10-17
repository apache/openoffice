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

import com.sun.star.beans.PropertyValue;
import com.sun.star.document.XExtendedFilterDetection;
import com.sun.star.lib.uno.helper.ComponentBase;

public class FormatDetector extends ComponentBase implements XExtendedFilterDetection {
    public static final String __serviceName = "com.sun.star.frame.ExtendedTypeDetection";
    
    @Override
    public String detect(final PropertyValue[][] descriptor) {
        final MediaDescriptor mediaDescriptor = new MediaDescriptor(descriptor[0]);
        if (mediaDescriptor.getUnpackedValueOrDefault(MediaDescriptor.Aborted, false)) {
            return "";
        }
        return "";
    }
}
