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

"""fetch.py: checksum-verified downloads into a cache, offline mode, and the lookup in tools.py. No network: the downloader is faked."""
import hashlib
import io
import json
import re

import pytest

from aootest import fetch, tools

pytestmark = pytest.mark.selftest

PAYLOAD = b"<grammar/>"
ITEM = {"name": "x", "group": "g", "url": "https://example.invalid/x.rng", "sha256": hashlib.sha256(PAYLOAD).hexdigest(), "path": "g/x.rng", "license": "test"}


class Opener:
    def __init__(self, data=PAYLOAD):
        self.data, self.calls = data, []

    def __call__(self, req):
        self.calls.append(req.full_url)
        return io.BytesIO(self.data)


def test_download_verify_and_reuse(tmp_path):
    op = Opener()
    assert fetch.fetch(cache=tmp_path, opener=op, items=[ITEM]) == {"x": "downloaded"}
    assert (tmp_path / "g/x.rng").read_bytes() == PAYLOAD
    assert fetch.fetch(cache=tmp_path, opener=op, items=[ITEM]) == {"x": "present"}
    assert len(op.calls) == 1


def test_checksum_mismatch_leaves_nothing_behind(tmp_path):
    res = fetch.fetch(cache=tmp_path, opener=Opener(b"tampered"), items=[ITEM])
    assert res["x"].startswith("failed: checksum mismatch")
    assert list((tmp_path / "g").iterdir()) == []


def test_corrupt_cached_file_is_replaced_only_by_a_verified_download(tmp_path):
    (tmp_path / "g").mkdir()
    (tmp_path / "g/x.rng").write_bytes(b"corrupt")
    assert fetch.status(ITEM, tmp_path) == "bad"
    assert fetch.fetch(cache=tmp_path, opener=Opener(b"still wrong"), items=[ITEM])["x"].startswith("failed")
    assert (tmp_path / "g/x.rng").read_bytes() == b"corrupt"
    assert fetch.fetch(cache=tmp_path, opener=Opener(), items=[ITEM]) == {"x": "downloaded"}
    assert fetch.status(ITEM, tmp_path) == "ok"


def test_offline_downloads_nothing(tmp_path):
    op = Opener()
    assert fetch.fetch(offline=True, cache=tmp_path, opener=op, items=[ITEM]) == {"x": "missing"}
    assert op.calls == []


def test_network_error_is_reported_not_raised(tmp_path):
    def broken(req):
        raise OSError("no route")
    assert fetch.fetch(cache=tmp_path, opener=broken, items=[ITEM]) == {"x": "failed: no route"}


def test_only_https_is_fetched(tmp_path):
    http = dict(ITEM, url="http://example.invalid/x.rng")
    op = Opener()
    assert fetch.fetch(cache=tmp_path, opener=op, items=[http])["x"].startswith("failed: only https")
    assert op.calls == []


def test_cache_dir_override(monkeypatch, tmp_path):
    monkeypatch.setenv("AOO_TEST_CACHE", str(tmp_path))
    assert fetch.cache_dir() == tmp_path


def test_fetch_json_is_well_formed():
    items = fetch.load_items()
    assert {it["group"] for it in items} == {"schemas", "jing", "odfvalidator"}
    assert len({it["name"] for it in items}) == len({it["path"] for it in items}) == len(items)
    for it in items:
        assert it["url"].startswith("https://")
        assert re.fullmatch(r"[0-9a-f]{64}", it["sha256"])
        assert not it["path"].startswith("/") and ".." not in it["path"].split("/")
        assert it["license"]
    assert sum(it["group"] == "schemas" for it in items) == 8


def test_tools_prefer_the_environment_then_the_cache(monkeypatch, tmp_path):
    jar = tmp_path / "cache" / "jing" / "jing-20181222.jar"
    jar.parent.mkdir(parents=True)
    monkeypatch.setenv("AOO_TEST_CACHE", str(tmp_path / "cache"))
    monkeypatch.delenv("AOO_JING", raising=False)
    assert tools.jing() is None                       # nothing cached yet
    jar.write_bytes(b"not the pinned jar")
    assert tools.jing() is None                       # a file with the wrong checksum is not used
    pinned = next(it for it in fetch.load_items() if it["name"] == "jing")
    monkeypatch.setattr(fetch, "load_items", lambda path=None: [dict(pinned, sha256=hashlib.sha256(b"not the pinned jar").hexdigest())])
    assert tools.jing() == str(jar)                   # verified cache hit
    own = tmp_path / "own.jar"
    own.write_bytes(b"x")
    monkeypatch.setenv("AOO_JING", str(own))
    assert tools.jing() == str(own)                   # the environment wins
