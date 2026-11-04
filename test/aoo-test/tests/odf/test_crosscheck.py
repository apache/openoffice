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

"""A second validator over the saved output: odfvalidator (a different engine, with its own error recovery) must agree with
jing about which streams of a document have errors. Strict ODF 1.2 output is compared in conformance mode, 1.2 extended
output in extended mode. Error counts are not comparable between the engines, so only whether a stream has errors is."""
import pytest

from aootest import documents

pytestmark = [pytest.mark.slow, pytest.mark.needs_java]

MODES = [("strict", "conformance"), ("ext", "extended")]


@pytest.mark.parametrize("mode,flavour", MODES)
def test_validators_agree(saved, mode, flavour, source):
    run, r = saved(mode, source)
    assert r["out"], "not saved: %s" % r["detail"]
    res = run.crosscheck()
    if res is None:
        pytest.skip("needs java, jing, odfvalidator and the ODF schemas (python -m aootest.fetch)")
    name = documents.out_name(source)
    res = res[flavour]
    disagree = [k for k in res["only_jing"] + res["only_odfvalidator"] if k[0].split("/", 1)[0] == name]
    assert name not in res["odfvalidator_crashed_on"], "odfvalidator crashed on %s" % name
    assert not disagree, "only one validator reports errors: %s" % disagree[:3]
