# Patches for libxslt 1.1.35

Place any `.patch` files here and reference them in `source.json` under the `patches` key.

Example source.json with patches:
```json
{
  "integrity": "sha256-mB2b2GJANBtAlP6f8EEuJT0oiNfVqGNHq5qFUZi3oeQ=",
  "strip_prefix": "libxslt-1.1.35",
  "type": "archive",
  "url": "https://download.gnome.org/sources/libxslt/1.1/libxslt-1.1.35.tar.xz",
  "patches": {
    "fix_build.patch": "sha256-XXXX..."
  },
  "patch_strip": 1
}
```
