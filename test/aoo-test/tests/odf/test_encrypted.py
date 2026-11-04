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

"""Password-protected documents. The office saves documents of each application with a password, under each combination of the
UseSHA1InODF12 and UseBlowfishInODF12 settings, in ODF 1.2 strict and extended mode. The ported decryptor (aootest.checkers.decrypt,
which reimplements PBKDF2, Blowfish and AES independently of AOO) must then read them: the manifest names the algorithms the setting
asks for, the right password decrypts every entry and passes the checksum, a wrong one is rejected, and the decrypted content equals
the same document saved without a password, apart from the cached value of date and time fields."""
import zipfile

import pytest

from aootest import stability
from aootest.checkers import decrypt

pytestmark = pytest.mark.slow

PASSWORD = "aootest-Password"
MODES = {"strict": 4, "ext": 3}
# variant -> (UseSHA1InODF12, UseBlowfishInODF12)
VARIANTS = {"default": (True, True), "aes": (False, False), "sha1aes": (True, False), "sha256bf": (False, True)}
FULL = [("odt", False), ("ods", False), ("odp", False), ("odg", False), ("odt", True), ("ods", True), ("odp", True), ("odg", True)]
BASIC = [("odt", False), ("odt", True)]
CASES = [(m, v, k, o) for m in MODES for v in VARIANTS for k, o in (FULL if v in ("default", "aes") else BASIC)]
IDS = ["%s-%s|%s%s" % (m, v, k, "-objects" if o else "") for m, v, k, o in CASES]


@pytest.fixture(scope="module")
def encrypted_runs(aoo_offices, tmp_path_factory):
    """{(mode, variant, kind, objects): paths}, saving each document once, on first use."""
    work, done = tmp_path_factory.mktemp("encrypted"), {}

    def get(mode, variant, kind, objects):
        key = (mode, variant, kind, objects)
        if key not in done:
            sha1, blowfish = VARIANTS[variant]
            session = aoo_offices.get(MODES[mode], {"UseSHA1InODF12": sha1, "UseBlowfishInODF12": blowfish}, "%s_%s" % (mode, variant))
            d = work / ("%s-%s" % (mode, variant)) / ("%s%s" % (kind, "-objects" if objects else ""))
            d.mkdir(parents=True)
            dst, twin = d / ("enc." + kind), d / ("plain." + kind)
            session.call("make_encrypted", {"kind": kind, "dst": str(dst), "twin": str(twin), "password": PASSWORD,
                                            "with_objects": objects}, timeout=300)
            done[key] = {"dir": d, "enc": dst, "twin": twin}
        return done[key]
    return get


def encrypted_entries(path):
    """{entry name: manifest file-entry} of the encrypted entries of a package."""
    with zipfile.ZipFile(path) as z:
        mani = decrypt.ET.fromstring(z.read("META-INF/manifest.xml"))
    return {fe.get(decrypt.Q + "full-path"): fe for fe in mani.findall(decrypt.Q + "file-entry")
            if fe.find(decrypt.Q + "encryption-data") is not None}


@pytest.mark.parametrize("mode,variant,kind,objects", CASES, ids=IDS)
class TestEncrypted:
    def test_manifest_names_the_algorithms_of_the_setting(self, encrypted_runs, need_mode, mode, variant, kind, objects):
        need_mode(MODES[mode])
        entries = encrypted_entries(encrypted_runs(mode, variant, kind, objects)["enc"])
        assert entries, "nothing in the package is encrypted"
        assert "mimetype" not in entries and "META-INF/manifest.xml" not in entries
        sha1, blowfish = VARIANTS[variant]
        for name, fe in entries.items():
            p = decrypt.entry_params(fe)
            assert (p["start"], p["cipher"]) == ("sha1" if sha1 else "sha256", "blowfish" if blowfish else "aes"), name

    def test_right_password_decrypts(self, encrypted_runs, need_mode, mode, variant, kind, objects):
        need_mode(MODES[mode])
        r = encrypted_runs(mode, variant, kind, objects)
        n = decrypt.decrypt_package(str(r["enc"]), str(r["dir"] / "decrypted.zip"), PASSWORD, [])
        assert n == len(encrypted_entries(r["enc"])) > 0

    def test_wrong_password_is_rejected(self, encrypted_runs, need_mode, mode, variant, kind, objects):
        need_mode(MODES[mode])
        r = encrypted_runs(mode, variant, kind, objects)
        with pytest.raises(decrypt.DecryptError):
            decrypt.decrypt_package(str(r["enc"]), str(r["dir"] / "wrong.zip"), "not-" + PASSWORD, [])

    def test_decrypted_content_equals_the_unencrypted_twin(self, encrypted_runs, need_mode, mode, variant, kind, objects):
        need_mode(MODES[mode])
        r = encrypted_runs(mode, variant, kind, objects)
        decrypt.decrypt_package(str(r["enc"]), str(r["dir"] / "decrypted.zip"), PASSWORD, [])
        with zipfile.ZipFile(r["dir"] / "decrypted.zip") as a, zipfile.ZipFile(r["twin"]) as b:
            for part in ("content.xml", "styles.xml"):
                assert stability.normalize(a.read(part).decode("utf-8")) == stability.normalize(b.read(part).decode("utf-8")), part
