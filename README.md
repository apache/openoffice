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

# Apache OpenOffice<sup>®</sup>

The Apache OpenOffice project (AOO) provides a full featured office productivity suite based on open standards.
It is the continuation of the OpenOffice.org project.

For detailed information about Apache OpenOffice please visit the [Project Website](https://openoffice.apache.org/).

Apache OpenOffice is a very large project with many components and mainly implemented in C++ but also in Java, Perl, Python and other languages.

Currently supported platforms include:

- Microsoft Windows
- macOS (OS X)
- Linux variants
- FreeBSD
- OS/2

OpenOffice is used by millions of users worldwide and with more than 395 million downloads it is one of the most successful open source projects.

# Getting the latest sources via Git

You can always checkout the latest source via Git using the following command:

```
git clone https://gitbox.apache.org/repos/asf/openoffice.git aoo
```

For further information visit [https://openoffice.apache.org/source.html](https://openoffice.apache.org/source.html).

# Building Apache OpenOffice

Apache OpenOffice is a big project and depends on several other external libraries.\
The list of prerequisites varies for the different platforms.

A comprehensive and complete building guide can be found in the [Project Wiki](https://wiki.openoffice.org/wiki/Documentation/Building_Guide_AOO).

With having all prerequisites in place you can simply run

```
cd aoo/main
autoconf
./configure <configure_switches>
./bootstrap
source *.Set.sh
cd instsetoo_native
build --all
```

Note that building OpenOffice can take several hours.

The default build will produce a setup version (e.g. setup program on Windows, dmg on macOS, rpm and deb packages on Linux) and an archived version.\
The output can be found in the <output> directory in instsetoo_native/<output_dir>/Apache_OpenOffice/...
