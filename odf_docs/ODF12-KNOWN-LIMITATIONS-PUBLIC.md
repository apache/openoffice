# DRAFT: Apache OpenOffice: ODF 1.2 known limitations

> **DRAFT. Work in progress, for discussion only.** This document has not been reviewed or approved by the Apache OpenOffice project and may change or be withdrawn. It is not an announcement, and it makes no claim that Apache OpenOffice conforms to ODF 1.2.

This lists the known deviations of Apache OpenOffice from ODF 1.2 in its "ODF 1.2" save mode (the producer) and when it reads ODF documents (the consumer). It accompanies the implementation-defined values document. Entries marked "runtime-checked" were reproduced with a running office; the others come from schema validation of generated documents, or from reading the code and the specification.

## Producer: the "ODF 1.2" save mode

- `anim:animateMotion` is written without `smil:attributeName`.
- As-character Writer lines are written with `svg:y`, and `draw:line` without `svg:x1` and `svg:y1`.
- `presentation:object` has vertical title and outline layouts whose value is not in the 1.2 schema.
- Date and numeric `table:data-pilot-groups` are written without a `table:data-pilot-group` child, which the schema requires.
- `table:selected-page` is required for an unselected page field; the schema has no "all" value.
- `grouped-by` and `step` are missing on named and numeric-range pivot groups.
- `table:id` is written on `table:movement-cut-off`, because the importer needs it to link the move.
- `table:refresh-delay` on a database range is written as a duration, where the schema says boolean.
- `draw:name` is written on `dr3d:scene`.
- Base: `db:apply-filter`, `db:help-message`, `db:type-name` and `db:default-value` are written in AOO's own form.
- `text:select-page="current"` is written.
- A database-range export still writes a `form:connection-resource` child under `table:database-source-table` (the schema says it is empty) and omits `table:database-name` when it is empty (the schema requires it).
- Graphic and OLE frames can still carry `fo:min-width` on `draw:frame`.
- `style:layout-grid-standard-mode` is written only in the default page layout, as the specification says, so a per-page-style value does not round trip.
- A formula that cannot be parsed is kept and written as typed. A cell with `=a0` (a name that looks like a cell reference, with no row 0) is saved as `of:=a0`, which Part 2 5.11 does not allow. There is no valid spelling for it.
- The concordance file of an alphabetical index is written in the system text encoding, not UTF-8. It is written only from a dialog.
- The extrusion attributes `draw:extrusion-first-light-harsh` and `draw:extrusion-second-light-harsh` are written from the light properties, but the renderer ignores them: harsh and soft lights look the same (runtime-checked).

## Consumer

- The XML parser is not namespace-aware. An unprefixed attribute takes the default namespace, a prefix rebound to another URI is ignored, and an undeclared prefix is not an error (runtime-checked).
- `xml:id` is not whitespace-normalised, an invalid value is silently dropped, and a duplicate keeps the first one without an error. Only `content.xml` and `styles.xml` are registered (runtime-checked).
- `text:start-value` of `text:notes-configuration` is read as the 0-based offset, where §19.868.5 defines the first note number (default 1). A file with `start-value="1"` loads with its first note numbered 2. AOO's own files round trip (runtime-checked).
- Levels missing from a `text:list-style` keep the application defaults instead of copying the next lower level (§16.30).
- The `svg:d` of a connector that is glued at either end is replaced by AOO's own route on load.
- A `text:list` without a style and without a surrounding list style does not use the list style named by its paragraph styles (§5.3.2, second rule): it gets a default bullet rule.

## Compatibility notes

- Protection keys are written as SHA-256 over the UTF-8 password with `...xmldsig#sha256`. Older versions of OpenOffice cannot unprotect a sheet saved this way (checked for Calc; for Writer sections from the code). A legacy key (SHA-1 over UTF-16LE) loaded from a file is written back unchanged, without the attribute, until the password is entered again. Writer keeps only the hash, so an ODF 1.0 or 1.1 save also writes the 32-byte key.

## Not verified

Builds on Linux, FreeBSD and Windows; older versions of OpenOffice as readers of the changes made for the "ODF 1.2" mode; the save-time cost of re-exporting embedded objects in that mode.
