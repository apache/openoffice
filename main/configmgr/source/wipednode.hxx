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



#ifndef INCLUDED_CONFIGMGR_SOURCE_WIPEDNODE_HXX
#define INCLUDED_CONFIGMGR_SOURCE_WIPEDNODE_HXX

#include "sal/config.h"

#include "rtl/ref.hxx"

#include "node.hxx"

namespace rtl { class OUString; }

namespace configmgr {

class Components;

/**
 * A configuration element that is going to disappear.
 *
 * These objects are a placeholder for elements that are pending removal
 * and must completely disappear from the configuration registry.
 */
class WipedNode: public Node {
public:
    WipedNode(int layer);

    virtual rtl::Reference< Node > clone(bool keepTemplateName) const;

private:
    WipedNode(WipedNode const & other);

    virtual ~WipedNode();

    virtual Kind kind() const;

};

}

#endif
