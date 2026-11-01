#**************************************************************
#
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#
#**************************************************************

# Mac OSX aarch64 (Apple Silicon) specific defines

PROCESSOR_DEFINES=-DAARCH64
BUILD64=1

DLLPOSTFIX=

# Build explicitly for arm64. Use conditional assignment (*=) so the value can
# still be overridden by exporting ARCH_FLAGS in the environment. unxmacc.mk
# also declares ARCH_FLAGS*= (empty), which becomes a no-op once set here.
ARCH_FLAGS*=-arch arm64

# flags to enable build with symbols; required by crashdump feature
.IF "$(ENABLE_SYMBOLS)"=="SMALL"
CFLAGSENABLESYMBOLS=-g1
.ELSE
CFLAGSENABLESYMBOLS=-g
.ENDIF

# Include generic Mac OS X makefile
.INCLUDE : unxmacc.mk
