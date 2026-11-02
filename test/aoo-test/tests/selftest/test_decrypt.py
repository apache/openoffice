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

"""decrypt.py: the Blowfish, AES, SHA-1 and PBKDF2 known-answer tests."""
import pytest

from aootest.checkers import decrypt

pytestmark = pytest.mark.selftest

ANSWERS = decrypt.known_answers()


@pytest.mark.parametrize("got, want", [pytest.param(g, w, id=n) for n, g, w in ANSWERS])
def test_known_answer(got, want):
    assert got == want


def test_known_answers_are_not_empty():
    assert len(ANSWERS) >= 9
