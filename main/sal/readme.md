# code documentation
## typesconfig is Unix only
typesconfig.c is a probe program that runs on Unix to detect platform type sizes and generate typesizes.h.

## workben/ — developer test tools
Clipboard test utilities (testcopy, testpaste, testviewer). These are manual developer tools, not part of the build.

## test/ and qa/ — automated tests
These become cc_test targets (Bazel's test rule), not cc_library. Leave them until sal.dll itself builds correctly. Add them last.