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



package ifc.util;

import lib.MultiPropertyTest;
import util.utils;

import com.sun.star.lang.XMultiServiceFactory;

/**
 * Testing <code>com.sun.star.util.PathSettings</code>
 * service properties :
 * <ul>
 *  <li><code> Addin         </code></li>
 *  <li><code> AutoCorrect   </code></li>
 *  <li><code> AutoText      </code></li>
 *  <li><code> Backup        </code></li>
 *  <li><code> Basic         </code></li>
 *  <li><code> Bitmap        </code></li>
 *  <li><code> Config        </code></li>
 *  <li><code> Dictionary    </code></li>
 *  <li><code> Favorites     </code></li>
 *  <li><code> Filter        </code></li>
 *  <li><code> Gallery       </code></li>
 *  <li><code> Graphic       </code></li>
 *  <li><code> Help          </code></li>
 *  <li><code> Linguistic    </code></li>
 *  <li><code> Module        </code></li>
 *  <li><code> Palette       </code></li>
 *  <li><code> Plugin        </code></li>
 *  <li><code> Storage       </code></li>
 *  <li><code> Temp          </code></li>
 *  <li><code> Template      </code></li>
 *  <li><code> UIConfig      </code></li>
 *  <li><code> UserConfig    </code></li>
 *  <li><code> UserDictionary</code></li>
 *  <li><code> Work          </code></li>
 * </ul> <p>
 *
 * Properties testing is automated by <code>lib.MultiPropertyTest</code>.
 * @see com.sun.star.util.PathSettings
 */
public class _PathSettings extends MultiPropertyTest {

    /**
     * This path list could be empty after SO installation.
     * And the URL is validated to match the pattern 'file:///*'
     * that's why the custom changing of this property required.
     */
    public void _UIConfig() {
        testProperty("UIConfig", new PropertyTester() {
            public Object getNewValue(String propName, Object oldVal) {
                String path = (String) oldVal;
                if (path == null) path = "";
                if (path.length() > 0 && !path.endsWith(";")) {
                    path += ";";
                }
                path += utils.getOfficeTemp((XMultiServiceFactory)tParam.getMSF());
                return path;
            }
        });
    }
}

