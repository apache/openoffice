# langs.bzl — OOo language / locale registry
#
# Maps the old alllangiso variable to Bazel build flags.
# Each locale becomes //build:lang_<code>  (hyphens → underscores).
#
# en-US is always ON (build_setting_default = True).
# All others default OFF; opt in via user.bazelrc:
#
#   build --//build:lang_de=True
#   build --//build:lang_fr=True
#   build --//build:lang_zh_CN=True
#
# Or on the command line:
#   bazel build --//build:lang_de=True //main/officecfg:officecfg

# Complete set of locales shipped with Apache OpenOffice 4.x
OOO_LANGS = [
    "en-US",
    "af", "ar", "bg", "bn", "br", "bs",
    "ca", "cs", "cy",
    "da", "de", "dz",
    "el", "en-GB", "eo", "es", "et", "eu",
    "fa", "fi", "fr",
    "ga", "gl", "gu",
    "he", "hi", "hr", "hu",
    "id", "it",
    "ja",
    "km", "ko", "ku",
    "lt", "lv",
    "mk", "ml", "mn", "mr",
    "nb", "ne", "nl", "nn", "nr", "nso",
    "oc", "or",
    "pa-IN", "pl", "pt", "pt-BR",
    "ro", "ru", "rw",
    "sh", "si", "sk", "sl", "sq", "sr", "ss", "st", "sv", "sw",
    "ta", "te", "tg", "th", "tn", "tr", "ts",
    "uk", "ur", "uz",
    "ve", "vi",
    "xh",
    "zh-CN", "zh-TW",
    "zu",
]

def lang_id(lang):
    """Normalise a locale code to a valid Bazel identifier (hyphens → underscores)."""
    return lang.replace("-", "_")
