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

# usage: decrypt.py <in-dir> <out-dir> <password>      (decrypt.py --selftest runs the cipher known-answer tests)
# Writes a plain copy of every package in <in-dir> into <out-dir> for the schema checkers: encrypted
# streams are decrypted and inflated, the manifest loses its encryption-data elements and the manifest:size
# of those entries. Do not run pkgcheck.py on the copies. A checksum mismatch (wrong password) or any other
# failure writes nothing for that package and makes the exit status 1.
# Implements what main/package writes and reads: PBKDF2-HMAC-SHA1 over the start key (SHA1 or SHA256 of the
# UTF-8 password), Blowfish with 64 bit feedback (rtl_cipher_CFB over 8 byte blocks) or AES-CBC with W3C padding.
import sys, os, io, re, zlib, zipfile, hashlib, base64, struct
from xml.etree import ElementTree as ET

M = "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0"
Q = "{%s}" % M
ET.register_namespace("manifest", M)
MASK = 0xFFFFFFFF


class DecryptError(Exception):
    pass


def pi_hex_words(n):
    bits = 32 * n + 64
    one = 1 << (bits + 16)

    def arctan_inv(x):
        x2, term, total, k = x * x, one // x, 0, 0
        while term:
            total += term // (2 * k + 1) if k % 2 == 0 else -(term // (2 * k + 1))
            term //= x2
            k += 1
        return total
    pi = 16 * arctan_inv(5) - 4 * arctan_inv(239)
    frac = (pi - (3 * one)) >> 16
    frac >>= 64
    return [(frac >> (32 * (n - 1 - i))) & MASK for i in range(n)]


_BF_INIT = []


class Blowfish:
    def __init__(self, key):
        if not _BF_INIT:
            _BF_INIT.extend(pi_hex_words(18 + 1024))
            assert _BF_INIT[0] == 0x243F6A88 and _BF_INIT[18] == 0xD1310BA6 and _BF_INIT[-1] == 0x3AC372E6
        self.P = _BF_INIT[:18]
        self.S = [_BF_INIT[18 + 256 * i:18 + 256 * (i + 1)] for i in range(4)]
        k = 0
        for i in range(18):
            d = 0
            for _ in range(4):
                d = (d << 8) | key[k]
                k = (k + 1) % len(key)
            self.P[i] ^= d
        l = r = 0
        for i in range(0, 18, 2):
            l, r = self.encrypt(l, r)
            self.P[i], self.P[i + 1] = l, r
        for i in range(4):
            for j in range(0, 256, 2):
                l, r = self.encrypt(l, r)
                self.S[i][j], self.S[i][j + 1] = l, r

    def encrypt(self, l, r):
        P, S0, S1, S2, S3 = self.P, *self.S
        for i in range(16):
            l ^= P[i]
            r ^= ((((S0[l >> 24] + S1[(l >> 16) & 255]) & MASK) ^ S2[(l >> 8) & 255]) + S3[l & 255]) & MASK
            l, r = r, l
        l, r = r, l
        return l ^ P[17], r ^ P[16]

    def encrypt_block(self, b):
        return struct.pack(">II", *self.encrypt(*struct.unpack(">II", b)))


def blowfish_cfb64_decrypt(key, iv, data):
    bf = Blowfish(key)
    reg = (iv + bytes(8))[:8]
    out = bytearray()
    for i in range(0, len(data), 8):
        ct = data[i:i + 8]
        ks = bf.encrypt_block(reg)
        out += bytes(a ^ b for a, b in zip(ct, ks))
        reg = ct
    return bytes(out)


def _aes_tables():
    def xt(a):
        a <<= 1
        return (a ^ 0x11B) & 255 if a & 256 else a

    def mul(a, b):
        r = 0
        while b:
            if b & 1:
                r ^= a
            a, b = xt(a), b >> 1
        return r
    inv = [0] * 256
    for a in range(1, 256):
        for b in range(1, 256):
            if mul(a, b) == 1:
                inv[a] = b
                break
    sbox = []
    for a in range(256):
        x = inv[a]
        s = x ^ (x << 1 | x >> 7) & 255 ^ (x << 2 | x >> 6) & 255 ^ (x << 3 | x >> 5) & 255 ^ (x << 4 | x >> 4) & 255 ^ 0x63
        sbox.append(s & 255)
    isbox = [0] * 256
    for i, s in enumerate(sbox):
        isbox[s] = i
    return sbox, isbox, {n: [mul(a, n) for a in range(256)] for n in (2, 9, 11, 13, 14)}


_AES = []


def aes_schedule(key):
    if not _AES:
        _AES.extend(_aes_tables())
    sbox, mt = _AES[0], _AES[2]
    if len(key) not in (16, 24, 32):
        raise DecryptError("AES key size %d" % len(key))
    nk = len(key) // 4
    nr = nk + 6
    w = [list(key[4 * i:4 * i + 4]) for i in range(nk)]
    rcon = 1
    for i in range(nk, 4 * (nr + 1)):
        t = list(w[i - 1])
        if i % nk == 0:
            t = [sbox[b] for b in t[1:] + t[:1]]
            t[0] ^= rcon
            rcon = mt[2][rcon]
        elif nk > 6 and i % nk == 4:
            t = [sbox[b] for b in t]
        w.append([a ^ b for a, b in zip(w[i - nk], t)])
    return [sum(w[4 * r:4 * r + 4], []) for r in range(nr + 1)]


def aes_decrypt_block(rk, blk):
    isbox, mt = _AES[1], _AES[2]
    m9, m11, m13, m14 = mt[9], mt[11], mt[13], mt[14]
    nr = len(rk) - 1
    s = [a ^ b for a, b in zip(blk, rk[nr])]
    for r in range(nr - 1, -1, -1):
        s = [s[0], s[13], s[10], s[7], s[4], s[1], s[14], s[11], s[8], s[5], s[2], s[15], s[12], s[9], s[6], s[3]]
        s = [isbox[b] ^ k for b, k in zip(s, rk[r])]
        if r:
            t = []
            for c in range(0, 16, 4):
                a0, a1, a2, a3 = s[c:c + 4]
                t += [m14[a0] ^ m11[a1] ^ m13[a2] ^ m9[a3], m9[a0] ^ m14[a1] ^ m11[a2] ^ m13[a3],
                      m13[a0] ^ m9[a1] ^ m14[a2] ^ m11[a3], m11[a0] ^ m13[a1] ^ m9[a2] ^ m14[a3]]
            s = t
    return bytes(s)


def aes_cbc_decrypt(key, iv, data):
    rk = aes_schedule(key)
    if len(data) % 16 or not data:
        raise DecryptError("AES data is not a multiple of 16 bytes")
    out, prev = bytearray(), iv
    for off in range(0, len(data), 16):
        blk = data[off:off + 16]
        out += bytes(a ^ b for a, b in zip(aes_decrypt_block(rk, blk), prev))
        prev = blk
    n = out[-1]
    if not 1 <= n <= 16:
        raise DecryptError("bad AES padding byte %d" % n)
    return bytes(out[:-n])


def sha1_rtl(data):
    """SHA-1 as main/sal rtl_digest_SHA1 computes it: input lengths of 52..55 mod 64 get one extra padding block."""
    h = [0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0]
    r = len(data) % 64
    m = data + b"\x80" + bytes((55 - r) % 64) + (bytes(64) if 52 <= r <= 55 else b"") + struct.pack(">Q", len(data) * 8)

    def rol(x, n):
        return ((x << n) | (x >> (32 - n))) & MASK
    for o in range(0, len(m), 64):
        w = list(struct.unpack(">16I", m[o:o + 64]))
        for i in range(16, 80):
            w.append(rol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1))
        a, b, c, d, e = h
        for i in range(80):
            if i < 20:
                f, k = (b & c) | (~b & d), 0x5A827999
            elif i < 40:
                f, k = b ^ c ^ d, 0x6ED9EBA1
            elif i < 60:
                f, k = (b & c) | (b & d) | (c & d), 0x8F1BBCDC
            else:
                f, k = b ^ c ^ d, 0xCA62C1D6
            a, b, c, d, e = (rol(a, 5) + f + e + k + w[i]) & MASK, a, rol(b, 30), c, d
        h = [(x + y) & MASK for x, y in zip(h, (a, b, c, d, e))]
    return struct.pack(">5I", *h)


def b64(v, what):
    try:
        return base64.b64decode(v or "", validate=True)
    except Exception:
        raise DecryptError("%s is not base64: %r" % (what, v))


def entry_params(fe):
    ed = fe.find(Q + "encryption-data")
    alg, kd, sk = ed.find(Q + "algorithm"), ed.find(Q + "key-derivation"), ed.find(Q + "start-key-generation")
    if alg is None or kd is None:
        raise DecryptError("encryption-data lacks algorithm or key-derivation")
    p = {"checksum": b64(ed.get(Q + "checksum"), "checksum") if ed.get(Q + "checksum") else None}
    ctype = ed.get(Q + "checksum-type")
    p["check"] = {"SHA1/1K": "sha1", M + "#sha1-1k": "sha1", M + "#sha256-1k": "sha256"}.get(ctype)
    if p["check"] is None:
        raise DecryptError("checksum-type %r" % ctype)
    name = alg.get(Q + "algorithm-name")
    keylen = {"http://www.w3.org/2001/04/xmlenc#aes128-cbc": 16, "http://www.w3.org/2001/04/xmlenc#aes192-cbc": 24,
              "http://www.w3.org/2001/04/xmlenc#aes256-cbc": 32}.get(name)
    if name in ("Blowfish CFB", M + "#blowfish"):
        p["cipher"], keylen = "blowfish", 16
    elif keylen:
        p["cipher"] = "aes"
    else:
        raise DecryptError("algorithm-name %r" % name)
    p["iv"] = b64(alg.get(Q + "initialisation-vector"), "initialisation-vector")
    if kd.get(Q + "key-derivation-name") not in ("PBKDF2", M + "#pbkdf2"):
        raise DecryptError("key-derivation-name %r" % kd.get(Q + "key-derivation-name"))
    p["salt"] = b64(kd.get(Q + "salt"), "salt")
    p["iter"] = int(kd.get(Q + "iteration-count"))
    p["keylen"] = int(kd.get(Q + "key-size") or keylen)
    sname = sk.get(Q + "start-key-generation-name") if sk is not None else "SHA1"
    p["start"] = {"SHA1": "sha1", "http://www.w3.org/2000/09/xmldsig#sha1": "sha1",
                  "http://www.w3.org/2000/09/xmldsig#sha256": "sha256"}.get(sname)
    if p["start"] is None:
        raise DecryptError("start-key-generation-name %r" % sname)
    return p


def raw_entry(path, zi):
    with open(path, "rb") as f:
        f.seek(zi.header_offset)
        h = f.read(30)
        if h[:4] != b"PK\x03\x04":
            raise DecryptError("no local header for %s" % zi.filename)
        n, e = struct.unpack("<HH", h[26:30])
        f.seek(zi.header_offset + 30 + n + e)
        return f.read(zi.compress_size)


def decrypt_entry(raw, p, password, name, notes):
    start = hashlib.new(p["start"], password.encode("utf-8")).digest()
    key = hashlib.pbkdf2_hmac("sha1", start, p["salt"], p["iter"], p["keylen"])
    try:
        plain = blowfish_cfb64_decrypt(key, p["iv"], raw) if p["cipher"] == "blowfish" else aes_cbc_decrypt(key, p["iv"], raw)
    except DecryptError as e:
        raise DecryptError("%s: %s (wrong password?)" % (name, e))
    if p["checksum"] is not None and hashlib.new(p["check"], plain[:1024]).digest() != p["checksum"]:
        if p["check"] == "sha1" and sha1_rtl(plain[:1024]) == p["checksum"]:
            notes.append("%s: checksum is the non-standard SHA-1 of main/sal (hashed length %d, mod 64 = %d)"
                         % (name, len(plain[:1024]), len(plain[:1024]) % 64))
        else:
            raise DecryptError("%s: checksum mismatch (wrong password?)" % name)
    try:
        d = zlib.decompressobj(-15)
        out = d.decompress(plain) + d.flush()
    except zlib.error as e:
        raise DecryptError("%s: decrypted data does not inflate (%s)" % (name, e))
    if not d.eof:
        raise DecryptError("%s: deflate stream is truncated" % name)
    return out


def decrypt_package(src, dst, password, notes):
    """Returns the number of encrypted entries decrypted; 0 means the package was copied unchanged."""
    z = zipfile.ZipFile(src)
    mani = ET.fromstring(z.read("META-INF/manifest.xml"))
    enc = {}
    for fe in mani.findall(Q + "file-entry"):
        if fe.find(Q + "encryption-data") is not None:
            enc[fe.get(Q + "full-path")] = fe
    data = {}
    for zi in z.infolist():
        n = zi.filename
        if n in enc:
            if zi.compress_type != zipfile.ZIP_STORED:
                raise DecryptError("%s: encrypted entry is not STORED" % n)
            plain = decrypt_entry(raw_entry(src, zi), entry_params(enc[n]), password, n, notes)
            size = enc[n].get(Q + "size")
            if size is not None and int(size) != len(plain):
                raise DecryptError("%s: manifest:size %s, decrypted %d bytes" % (n, size, len(plain)))
            data[n] = plain
        else:
            data[n] = z.read(n)
    for fe in enc.values():
        fe.remove(fe.find(Q + "encryption-data"))
        fe.attrib.pop(Q + "size", None)
        fe.text = None
    missing = [p for p in enc if p not in data]
    if missing:
        raise DecryptError("manifest lists encrypted entries not in the zip: %s" % missing)
    if enc:
        data["META-INF/manifest.xml"] = ET.tostring(mani, encoding="UTF-8", xml_declaration=True)
    tmp = dst + ".tmp"
    with zipfile.ZipFile(tmp, "w") as o:
        names = [zi.filename for zi in z.infolist()]
        for n in sorted(names, key=lambda n: n != "mimetype"):
            zi = zipfile.ZipInfo(n, date_time=z.getinfo(n).date_time)
            zi.compress_type = zipfile.ZIP_STORED if n == "mimetype" or n.endswith("/") else zipfile.ZIP_DEFLATED
            zi.external_attr = z.getinfo(n).external_attr
            o.writestr(zi, data[n])
    os.replace(tmp, dst)
    return len(enc)


def selftest():
    bad = 0

    def check(name, got, want):
        nonlocal bad
        print(("ok   " if got == want else "FAIL ") + name)
        bad += got != want
    for k, pt, ct in (("0000000000000000", "0000000000000000", "4ef997456198dd78"),
                      ("ffffffffffffffff", "ffffffffffffffff", "51866fd5b85ecb8a"),
                      ("3000000000000000", "1000000000000001", "7d856f9a613063f2")):
        check("blowfish key " + k, Blowfish(bytes.fromhex(k)).encrypt_block(bytes.fromhex(pt)).hex(), ct)
    pt = "00112233445566778899aabbccddeeff"
    for k, ct in (("000102030405060708090a0b0c0d0e0f", "69c4e0d86a7b0430d8cdb78070b4c55a"),
                  ("000102030405060708090a0b0c0d0e0f1011121314151617", "dda97ca4864cdfe06eaf70a0ec0d7191"),
                  ("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f", "8ea2b7ca516745bfeafc49904b496089")):
        check("aes-%d block" % (len(k) * 2), aes_decrypt_block(aes_schedule(bytes.fromhex(k)), bytes.fromhex(ct)).hex(), pt)
    msgs = [(bytes(range(256)) * 2)[:n] for n in range(0, 300) if not 52 <= n % 64 <= 55]
    check("sha1_rtl equals SHA-1 outside 52..55 mod 64", all(sha1_rtl(m) == hashlib.sha1(m).digest() for m in msgs), True)
    check("sha1_rtl differs from SHA-1 at 52..55 mod 64",
          all(sha1_rtl(bytes(n)) != hashlib.sha1(bytes(n)).digest() for n in (52, 53, 54, 55, 116, 886)), True)
    check("pbkdf2-hmac-sha1 RFC 6070", hashlib.pbkdf2_hmac("sha1", b"password", b"salt", 2, 20).hex(),
          "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957")
    return bad == 0


def main(argv):
    if len(argv) == 2 and argv[1] == "--selftest":
        return 0 if selftest() else 1
    if len(argv) != 4:
        print("usage: decrypt.py <in-dir> <out-dir> <password>", file=sys.stderr)
        return 2
    src, dst, password = argv[1:]
    os.makedirs(dst, exist_ok=True)
    done = copied = 0
    failed, quirks = [], 0
    for name in sorted(os.listdir(src)):
        path = os.path.join(src, name)
        if name.startswith(".") or not os.path.isfile(path) or not zipfile.is_zipfile(path):
            print("skipped (not a zip package): %s" % name)
            continue
        notes = []
        try:
            n = decrypt_package(path, os.path.join(dst, name), password, notes)
        except (DecryptError, zipfile.BadZipFile, KeyError, ET.ParseError, ValueError) as e:
            failed.append(name)
            print("FAILED %s: %s" % (name, e), file=sys.stderr)
            continue
        for note in notes:
            quirks += 1
            print("note %s: %s" % (name, note))
        if n:
            done += 1
            print("decrypted %s (%d encrypted entries)" % (name, n))
        else:
            copied += 1
            print("copied %s (no encrypted entries)" % name)
    print("%d decrypted, %d copied unchanged, %d failed, %d non-standard SHA-1 checksums" % (done, copied, len(failed), quirks))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
