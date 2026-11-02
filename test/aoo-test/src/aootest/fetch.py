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

"""Downloads the items listed in data/fetch.json (OASIS schemas, jing, odfvalidator) into a per-user cache.

Every file is checked against its pinned sha256 before it is moved into place, so a cached file is always a verified one.
Nothing third-party is committed. Command line: python -m aootest.fetch [--offline] [--list] [name ...]
"""
import hashlib
import json
import os
import sys
import tempfile
import urllib.request
from pathlib import Path

from aootest.paths import DATA


def cache_dir():
    """AOO_TEST_CACHE, else the usual per-user cache directory of the platform."""
    env = os.environ.get("AOO_TEST_CACHE")
    if env:
        return Path(env)
    if sys.platform == "win32":
        return Path(os.environ.get("LOCALAPPDATA") or Path.home() / "AppData" / "Local") / "aootest" / "Cache"
    if sys.platform == "darwin":
        return Path.home() / "Library" / "Caches" / "aootest"
    return Path(os.environ.get("XDG_CACHE_HOME") or Path.home() / ".cache") / "aootest"


def load_items(path=None):
    """The entries of fetch.json: dicts with name, group, url, sha256, path (relative to the cache), license."""
    return json.loads(Path(path or DATA / "fetch.json").read_text(encoding="utf-8"))["items"]


def sha256_of(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for block in iter(lambda: f.read(1 << 20), b""):
            h.update(block)
    return h.hexdigest()


def status(item, cache=None):
    """'ok' (present and matching), 'bad' (present, wrong checksum) or 'missing'."""
    p = Path(cache or cache_dir()) / item["path"]
    if not p.is_file():
        return "missing"
    return "ok" if sha256_of(p) == item["sha256"] else "bad"


def cached_path(name, cache=None, items=None):
    """Path of a verified cached item, or None."""
    for it in items or load_items():
        if it["name"] == name:
            return Path(cache or cache_dir()) / it["path"] if status(it, cache) == "ok" else None
    raise KeyError(name)


def group_dir(group, cache=None, items=None):
    """The directory holding a group (all files in one directory) when every item of it is verified, else None."""
    its = [it for it in (items or load_items()) if it["group"] == group]
    if not its or any(status(it, cache) != "ok" for it in its):
        return None
    dirs = {(Path(cache or cache_dir()) / it["path"]).parent for it in its}
    return dirs.pop() if len(dirs) == 1 else None


def _download(item, cache, opener):
    url = item["url"]
    if not url.startswith("https://"):
        raise ValueError("only https URLs are fetched: " + url)
    dest = Path(cache) / item["path"]
    dest.parent.mkdir(parents=True, exist_ok=True)
    fd, tmp = tempfile.mkstemp(dir=dest.parent, prefix=dest.name + ".part")
    try:
        h = hashlib.sha256()
        with os.fdopen(fd, "wb") as out, opener(urllib.request.Request(url, headers={"User-Agent": "aootest-fetch"})) as src:
            for block in iter(lambda: src.read(1 << 20), b""):
                h.update(block)
                out.write(block)
        if h.hexdigest() != item["sha256"]:
            raise ValueError("checksum mismatch for %s: got %s" % (url, h.hexdigest()))
        os.replace(tmp, dest)
    except BaseException:
        if os.path.exists(tmp):
            os.unlink(tmp)
        raise


def fetch(names=None, offline=False, cache=None, opener=None, items=None):
    """Makes the named items (default: all) present. Returns {name: 'present' | 'downloaded' | 'missing' | 'failed: reason'}.
    With offline=True nothing is downloaded and absent items are reported 'missing'. A file with the wrong checksum is
    replaced only by a download that verifies."""
    cache = Path(cache or cache_dir())
    opener = opener or (lambda req: urllib.request.urlopen(req, timeout=60))
    result = {}
    for it in items or load_items():
        if names and it["name"] not in names:
            continue
        if status(it, cache) == "ok":
            result[it["name"]] = "present"
        elif offline:
            result[it["name"]] = "missing"
        else:
            try:
                _download(it, cache, opener)
                result[it["name"]] = "downloaded"
            except Exception as e:
                result[it["name"]] = "failed: %s" % e
    return result


def main(argv=None):
    argv = sys.argv[1:] if argv is None else list(argv)
    offline, listing = "--offline" in argv, "--list" in argv
    names = [a for a in argv if not a.startswith("--")]
    unknown = set(names) - {it["name"] for it in load_items()}
    if unknown:
        print("unknown item: " + ", ".join(sorted(unknown)), file=sys.stderr)
        return 2
    if listing:
        for it in load_items():
            print("%-9s %-8s %s" % (status(it), it["group"], it["name"]))
        return 0
    res = fetch(names or None, offline=offline)
    for n, s in res.items():
        print("%-12s %s" % (s.split(":")[0], n) + (s[s.index(":"):] if s.startswith("failed") else ""))
    print("cache: %s" % cache_dir())
    return 0 if all(s in ("present", "downloaded") for s in res.values()) else 1


if __name__ == "__main__":
    sys.exit(main())
