<!--
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
-->

# Slideshow

## Bazel Migration

### Deliverables

| Target | Output | Description |
|--------|--------|-------------|
| `//main/slideshow:slideshow` | `slideshow.dll` | Impress slideshow engine UNO component |
| `//main/slideshow:slideshow_implib` | `slideshow.lib` | Import library |

OGLTrans.dll (OpenGL transitions) is not built — it uses Linux-only `GL/glx.h` headers and is only enabled with `ENABLE_OPENGL=TRUE`.

### Key notes

- 95 source files across `source/engine/` subdirectories (transitions, activities, animationnodes, shapes, slide, engine root)
- `BOOST_SPIRIT_USE_OLD_NAMESPACE` required for `smilfunctionparser.cxx` (uses `boost::spirit` classic)
- `stlport` required for `hash_map` (eventmultiplexer.cxx, layermanager.hxx)
- Per-subdirectory `/I` copts needed — source files use sibling `#include "foo.hxx"` includes
- `animations_headers` dep needed for `animations/animationnodehelper.hxx`
- DEF exports: `component_getFactory` + `component_getImplementationEnvironment` (via `COMPHELPER_SERVICEDECL_EXPORTS1`)
