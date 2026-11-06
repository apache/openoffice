<!--
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
-->

# DRAFT: Apache OpenOffice: ODF 1.4 known limitations

> **DRAFT. Work in progress, for discussion only.** This document has not been reviewed or approved by the Apache OpenOffice project and may change or be withdrawn. It is not an announcement, and it makes no claim that Apache OpenOffice conforms to ODF 1.4.

This lists the known deviations of Apache OpenOffice from ODF 1.4 in its "ODF 1.4" save mode (the producer) and when it reads ODF 1.4 documents (the consumer). It accompanies the implementation-defined values document. The "ODF 1.2" and "ODF 1.3" limitations lists describe those modes; their checks were not repeated item by item for the 1.4 mode, and the 1.3 reader limitations apply here too. For the 1.4 mode, a corpus of 411 test packages saved in that mode validates with no error against the 1.4 schema.

## Not supported

- XAdES signatures, and OpenPGP key transport in the manifest (`manifest:encrypted-key`, `keyinfo`, `PGPData`, `PGPKeyID`, `PGPKeyPacket`, `PGPAlgorithm`, `CipherData`, `CipherValue`).
- Documents newer than ODF 1.4 trigger the "created with a newer version" prompt; ODF 1.3 and 1.4 documents do not.

## Producer: the "ODF 1.4" save mode

- Signatures carry no `dsig:version`. The attribute is required by the 1.2, 1.3 and 1.4 signature schemas, and AOO has not written it in any mode.
- Formula cells with an error result are written with `office:value-type="string"`. ODF 1.4 adds the value type `error`; it is not used because the 1.4 text names an `office:error` attribute for it that does not exist, and older readers do not know the value type.
- `draw:handle-position` and `draw:handle-polar`, which ODF 1.4 deprecates, are still written next to the new handle attributes so that older versions keep custom-shape handles.
- `draw:decorative` is kept as a property of shapes in Draw, Impress and Calc, of groups, and of Writer text frames, graphics and drawing shapes, and is written in the 1.4 modes only. There is no user interface for it, it has no effect on accessibility or tagged PDF, changing it cannot be undone, and deleting a Writer text frame and undoing the deletion, or copying and pasting the frame, loses it, as it loses the frame's title. OLE objects, Calc form controls and charts were not tried.
- Paragraph fills are not written in the "ODF 1.4" mode. Header and footer fills and Impress table cell fills are written as `style:background-fill`.
- The vertical and horizontal text alignment of an Impress table cell has no counterpart in ODF 1.4 and is lost on save. The writing mode of a table cell is not saved in any mode, and cell borders are written as paragraph borders.
- `form:input-required` is not written in the 1.3 and earlier strict modes, so a control set to false reloads as true.
- A stock chart whose series has a role with no range may lose its link to the sheet on reload, because `table:cell-range-address` is no longer written on `chart:plot-area`. This comes from reading the code; no such chart could be built in the user interface.

## Producer: the "ODF 1.4 Extended" save mode

This mode is not part of any claim. For the 411-package corpus its output has no error against the 1.4 schema once foreign elements and attributes are removed. Beyond the limits above:

- Paragraph fills are written as `loext:graphic-properties`.
- The "snap to characters" grid option is written as `style:layout-grid-snap-to`, and the XForms list source of a combobox as `ooo:xforms-list-source`. Older versions lose these.
- `text:p` inside `draw:object` is still written.
- Untouched embedded objects are re-exported as in the strict formats: more work on each save, and their stored form changes.

## Reader: ODF 1.4 features that are not interpreted

These are read without error and have no effect:

- `style:margin-gutter`, `text:relative-tab-stop-position` on a paragraph style, `draw:allow-overlap`, `draw:extrusion-metal-type`, `svg:rx` and `svg:ry` on text boxes and annotations, `chart:major-origin` and `chart:minor-logarithmic`, and `office:annotation` in `draw:page`.
- `style:background-fill` on paragraphs.
- `number:num-list-format` and `style:num-list-format-name`: the list keeps its default label.
- `table:table` inside a shape: the document loads, but the table is not shown or kept.
- Colour filter conditions (`text-color`, `background-color` on `table:filter-condition`) are dropped; the other conditions of the filter are kept.

## Reader: read with limits

- Error cells (`office:value-type="error"`) are read as cells holding that error. They are saved as strings with the error text and no formula.
- `fo:break-before` with `even-page` or `odd-page` becomes an ordinary page break.
- `draw:extrusion-specularity` above 100% is stored unchanged. Its rendering was not checked.
- `style:repeat="scale"` is read as `stretch`: the image fills the shape without keeping its aspect ratio.
- `style:writing-mode` with `sideways-rl` or `sideways-lr` falls back to the mode of the page, and `style:vertical-rel` with `page-content-bottom` or `page-content-top` falls back to the paragraph area. AOO has neither.

## Formulas

- `INDEX` on a row vector with only a row argument returns the column, as in other spreadsheets. ODF 1.4 removed that rule and calls the call an error.
- `CONVERT` with `atm` uses 101324.996583 Pa; the ODF 1.4 table gives 1.0132510E+5, a relative difference of 1.0e-6.
- A trailing empty parameter before the closing bracket, as in `NPER(r;p;pv;;)`, is an error for every function except `IF`. An empty parameter in the middle works.
- `DATE` gives an error for dates before 15 October 1582. ODF 1.4 requires support only from 1583 and makes earlier dates implementation-defined.

## Not verified

Builds on Linux x86-64, FreeBSD and Windows; the translated option-page strings; rendering of the shapes and fills listed above.
