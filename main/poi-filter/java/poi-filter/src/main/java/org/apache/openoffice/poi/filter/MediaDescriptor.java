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

import java.util.HashMap;

import com.sun.star.beans.PropertyValue;

public class MediaDescriptor extends HashMap<String,Object> {
    private static final long serialVersionUID = 1L;
    
    public static final String Aborted = "Aborted";
    public static final String AsTemplate = "AsTemplate";
    public static final String AuthenticationHandler = "AuthenticationHandler";
    public static final String Author = "Author";
    public static final String CharacterSet = "CharacterSet";
    public static final String Comment = "Comment";
    public static final String ComponentData = "ComponentData";
    public static final String DeepDetection = "DeepDetection";
    public static final String DetectService = "DetectService";
    public static final String DocumentBaseURL = "DocumentBaseURL";
    public static final String DocumentService = "DocumentService";
    public static final String DocumentTitle = "DocumentTitle";
    public static final String EncryptionData = "EncryptionData";
    public static final String Extension = "Extension";
    public static final String FileName = "FileName";
    public static final String FilterData = "FilterData";
    public static final String FilterFlags = "FilterFlags";
    public static final String FilterName = "FilterName";
    public static final String FilterOptions = "FilterOptions";
    public static final String Format = "Format";
    public static final String Frame = "Frame";
    public static final String FrameName = "FrameName";
    public static final String Hidden = "Hidden";
    public static final String HierarchicalDocumentName = "HierarchicalDocumentName";
    public static final String InputStream = "InputStream";
    public static final String InteractionHandler = "InteractionHandler";
    public static final String JumpMark = "JumpMark";
    public static final String MacroExecutionMode = "MacroExecutionMode";
    public static final String MediaType = "MediaType";
    public static final String Minimized = "Minimized";
    public static final String Model = "Model";
    public static final String NoAutoSave = "NoAutoSave";
    public static final String OpenFlags = "OpenFlags";
    public static final String OpenNewView = "OpenNewView";
    public static final String OutputStream = "OutputStream";
    public static final String Overwrite = "Overwrite";
    public static final String Password = "Password";
    public static final String Pattern = "Pattern";
    public static final String PosSize = "PosSize";
    public static final String PostData = "PostData";
    public static final String PostString = "PostString";
    public static final String Preview = "Preview";
    public static final String ReadOnly = "ReadOnly";
    public static final String Referer = "Referer";
    public static final String RepairPackage = "RepairPackage";
    public static final String SalvagedFile = "SalvagedFile";
    public static final String Silent = "Silent";
    public static final String StartPresentation = "StartPresentation";
    public static final String StatusIndicator = "StatusIndicator";
    public static final String Stream = "Stream";
    public static final String StreamForOutput = "StreamForOutput";
    public static final String SuggestedSaveAsDir = "SuggestedSaveAsDir";
    public static final String SuggestedSaveAsName = "SuggestedSaveAsName";
    public static final String TemplateName = "TemplateName";
    public static final String TemplateRegionName = "TemplateRegionName";
    public static final String Title = "Title";
    public static final String TypeName = "TypeName";
    public static final String UCBContent = "UCBContent";
    public static final String Unpacked = "Unpacked";
    public static final String UpdateDocMode = "UpdateDocMode";
    public static final String URL = "URL";
    public static final String Version = "Version";
    public static final String ViewControllerName = "ViewControllerName";
    public static final String ViewData = "ViewData";
    public static final String ViewId = "ViewId";
    public static final String ViewOnly = "ViewOnly";

    public MediaDescriptor(final PropertyValue[] properties) {
        super();
        for (final PropertyValue property : properties) {
            put(property.Name, property.Value);
        }
    }
    
    @SuppressWarnings("unchecked")
    public <T> T getUnpackedValueOrDefault(final String key, final T aDefault) {
        final T t = (T) get(key);
        if (t == null) {
            return aDefault;
        }
        return t;
    }
}
