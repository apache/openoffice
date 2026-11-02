################################################################
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
################################################################

"""Entry point: python -m aootest_worker (run by the Python that can import uno)."""
from aootest_worker.ops import calcval, core, dump, encrypted, generate, roundtrip, smoke
from aootest_worker.proto import serve

serve({**core.OPS, **smoke.OPS, **roundtrip.OPS, **dump.OPS, **calcval.OPS, **encrypted.OPS, **generate.OPS})
