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

"""Fixtures for the checker self-tests: negative-control packages built in a temp dir, and the checkers run over them once per session.

Tests that need java, jing or the OASIS schemas skip with a reason when they are not available (AOO_JING, AOO_SCHEMA_DIR,
AOO_ODFVALIDATOR, AOO_JAVA, or the cache that `python -m aootest.fetch` fills).
"""
import pytest

from aootest import controls, tools
from aootest.checkers import doctypecheck, idcheck, pkgcheck, validate


@pytest.fixture(scope="session")
def jing_cmd():
    cmd = tools.jing_command()
    if cmd is None:
        pytest.skip("needs java and a jing jar (set AOO_JING or run: python -m aootest.fetch)")
    return cmd


@pytest.fixture(scope="session")
def schema_dir():
    d = tools.schema_dir()
    if d is None:
        pytest.skip("needs the OASIS ODF schemas (set AOO_SCHEMA_DIR or run: python -m aootest.fetch)")
    return d


@pytest.fixture(scope="session")
def odf_controls(tmp_path_factory):
    d = tmp_path_factory.mktemp("control")
    controls.make_odf_controls(d)
    return d


@pytest.fixture(scope="session")
def val(odf_controls, jing_cmd, schema_dir, tmp_path_factory):
    return validate.check(odf_controls, tmp_path_factory.mktemp("validate"), jing_cmd, schema_dir)


@pytest.fixture(scope="session")
def errs(val):
    """errs(version, package) -> the messages jing reported for that package in that schema version."""
    return lambda v, pkg: [e[3] for e in val["errors"][v] if e[0].startswith(pkg + "/")]


@pytest.fixture(scope="session")
def ids(odf_controls):
    return idcheck.check(odf_controls)


@pytest.fixture(scope="session")
def pkg_result(jing_cmd, schema_dir, tmp_path_factory):
    d = tmp_path_factory.mktemp("control-pkg")
    controls.make_pkg_controls(d)
    return pkgcheck.check(d, tmp_path_factory.mktemp("pkgcheck"), jing_cmd, schema_dir)


@pytest.fixture(scope="session")
def doctype_result(tmp_path_factory):
    d = tmp_path_factory.mktemp("control-doctype")
    controls.make_doctype_controls(d)
    return doctypecheck.check(d)
