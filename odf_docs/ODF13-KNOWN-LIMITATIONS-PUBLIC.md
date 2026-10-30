# DRAFT: Apache OpenOffice: ODF 1.3 known limitations

> **DRAFT. Work in progress, for discussion only.** This document has not been reviewed or approved by the Apache OpenOffice project and may change or be withdrawn. It is not an announcement, and it makes no claim that Apache OpenOffice conforms to ODF 1.3.

This lists the known deviations of Apache OpenOffice from ODF 1.3 in its "ODF 1.3" save mode (the producer) and when it reads ODF 1.3 documents (the consumer). It accompanies the implementation-defined values document. The "ODF 1.2" limitations list describes the 1.2 mode; its checks were not repeated item by item for the 1.3 mode. For the 1.3 mode, a corpus of 411 test packages saved in that mode validates with no error against the 1.3 schema.

## Not supported

- XAdES signatures, and OpenPGP key transport in the manifest (`manifest:encrypted-key`, `keyinfo`, `PGPData`, `PGPKeyID`, `PGPKeyPacket`, `PGPAlgorithm`, `CipherData`, `CipherValue`). AES-GCM and Argon2 are not part of ODF 1.3.
- Documents newer than ODF 1.3 trigger the "created with a newer version" prompt; ODF 1.3 documents do not.

## Reader: ODF 1.3 features that are not interpreted

These are read without error and have no effect:

- `number:forced-exponent-sign`, `number:exponent-interval` (engineering notation) and `number:max-denominator-value`.
- `style:header-first` and `style:footer-first`: a first-page header or footer with content is dropped. Neither Writer nor Calc has a model for it.
- `style:page-number="0"`: it is read like `auto`, so a restart of the page numbers at 0 is lost.
- `style:contextual-spacing="true"`, `chart:data-label-series="true"` and the chart regression attributes.
- The text content of `draw:image`; `draw:mime-type` on `draw:image` has no effect.

## Reader: read with limits

- `number:min-decimal-places`: the optional decimals become `#` in the number format, so `0.0##` shows 1.5 as 1.5. In scientific formats the optional digits are still shown as zeros (`0.0##E+00` shows 1.500E+05, not 1.5E+05).
- `number:fill-character`: read into the number format as `*c`, one per format and one character in the Basic Multilingual Plane; further ones are ignored. How the fill appears on screen or in print was not checked.

## Producer: the "ODF 1.3" save mode

- `number:min-decimal-places` is written. The 1.2 modes write `0.0##` as fixed decimals, because they cannot express it.
- `number:fill-character` is not written in any mode: a fill character is lost on save.
- The standard ODF 1.3 forms are written for `chart:coordinate-region`, `table:tab-color`, `meta:creator-initials`, `text:drop-down`, scale-to-X/Y, the chart legend size, chart shapes and index link tokens. `text:help` and `text:hint` on a drop-down are written only in the "ODF 1.3 Extended" mode, in the `textooo:` namespace.
- A formula that cannot be parsed is kept and written as typed, for example `of:=a0` (see the ODF 1.2 list).

## Formulas

- DCOUNT and DCOUNTA accept field 0, meaning the whole database, and ignore empty criteria cells.
- LINEST and LOGEST accept as many observations as parameters plus one, with a constant, which leaves zero degrees of freedom.
- Whitespace between a function name and `(` is written back.
- Operators nested in a scalar parameter of HLOOKUP, VLOOKUP, MATCH, LINEST and LOGEST are evaluated in array mode.
- WEEKDAY keeps its fallback for unspecified return types.
- A missing parameter is 0 when it is the final value of a formula. A missing `IF` branch nested in a larger expression stays empty: `=IF(1;;5)&"x"` gives "x" and `=LEN(IF(1;;5))` gives 0.

## Not verified

Builds on Linux, FreeBSD and Windows; the options dialog in a running office; the function wizard path for `IF(c;)`; the translated option-page strings; the corpus in the 1.0 and 1.1 modes (only the fixtures run there).
