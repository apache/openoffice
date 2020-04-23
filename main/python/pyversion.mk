# *************************************************************
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
# *************************************************************
# when you want to change the python version, you must update the d.lst
# in the python project accordingly !!!
PYMAJOR=2
PYMINOR=7
PYMICRO=18
PYVERSION=$(PYMAJOR).$(PYMINOR).$(PYMICRO)

ifeq ($(GUI),UNX)
ifeq ($(OS),MACOSX)
PY_FULL_DLL_NAME=libpython$(PYMAJOR).$(PYMINOR).dylib
else
PY_FULL_DLL_NAME=libpython$(PYMAJOR).$(PYMINOR).so.1.0
endif
PYTHONLIB=-lpython$(PYMAJOR).$(PYMINOR)
else ifeq ($(GUI),OS2)
PY_FULL_DLL_NAME=python$(PYMAJOR)$(PYMINOR).dll
PYTHONLIB=python$(PYMAJOR)$(PYMINOR).lib
else
ifeq ($(COM),GCC)
PY_FULL_DLL_NAME=libpython$(PYMAJOR).$(PYMINOR).dll
PYTHONLIB=-lpython$(PYMAJOR).$(PYMINOR)
else
PY_FULL_DLL_NAME=python$(PYMAJOR)$(PYMINOR).dll
PYTHONLIB=python$(PYMAJOR)$(PYMINOR).lib
endif
endif
