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

"""Parameters shared by the tests that run over the source documents."""
import pytest

from aootest import documents

PLACEHOLDER = documents.Source("(no office)", None, "odt", None)


def pytest_generate_tests(metafunc):
    """The documents depend on the office named on the command line, so `source` is parametrized here. Without an office there
    is one placeholder, and the office fixtures skip it with the reason."""
    if "source" in metafunc.fixturenames:
        found = documents.from_config(metafunc.config) or [PLACEHOLDER]
        metafunc.parametrize("source", found, ids=[s.id for s in found])


@pytest.fixture
def saved(odf_runs, need_mode):
    """saved(mode, source) -> (ModeRun, result of saving `source` in `mode`); skips when the build lacks the mode."""
    def get(mode, source):
        need_mode(mode)
        run = odf_runs(mode)
        return run, run.saved()[source.id]
    return get
