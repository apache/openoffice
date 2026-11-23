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

# The pdfi helper protocol

`pdfimport.uno.dll` does not parse PDF itself.  It spawns a **helper process**,
feeds it a file name, and reads back a stream of drawing commands which it
replays into `pdfi::ContentSink` (`inc/contentsink.hxx`).  Everything above the
sink — the document tree in `tree/`, the ODF emitters in `odf/` and `sax/` — is
backend-agnostic and never sees a PDF.

This file is the contract.  Anything that implements it is a usable backend;
see `inc/pdfbackend.hxx` for how one is selected at run time.

## Why a separate process

Originally: a licence firewall.  The only backend was `xpdfimport`, which links
poppler (GPLv2) and therefore may not be linked into an Apache-licensed
`soffice.bin`.

It is kept for a better reason.  A PDF is untrusted input and the parsing code
is where the memory-safety defects live; a crash or a corrupted heap in the
helper costs one document, not the user's session.  The split is now a
containment boundary, not a workaround, and new backends keep it even when
their licence would permit linking.

## Invocation

    <program> [prefix args...] [-f <binary-file>] <pdf-system-path>

* The PDF path is a **native system path**, not a file URL, and is always last.
* `-f <path>` redirects the binary channel (v1 only, see below).
* `-opw <pw>` / `-upw <pw>` supply owner / user passwords on the command line.
  Prefer stdin; these exist because `xpdfimport` accepted them.

**stdin** — the helper reads exactly one line before anything else: the open
password, possibly empty.  It must tolerate EOF.  The password is *always*
written by the caller, so a helper that does not read it will make the caller
block on a full pipe.

**exit status** — `0` on success.  Any other value, or a helper that dies, is
reported to the user as a failed import.

## Framing

Two framings exist.  A backend declares which it speaks; the caller adapts.

**v1 — `BINARY_ON_STDERR`** (`xpdfimport`).  Text commands on stdout, binary
payloads on stderr.  Both processes must touch the same pipe at the same time
or they deadlock, so the helper has to flush one stream before writing to the
other.  This is the framing described by the `SYNC STREAMS` comment in
`xpdfwrapper/pdfioutdev_gpl.cxx`; it exists only because that helper already
used stderr for binary before anyone needed stderr for anything else.

**v2 — `BINARY_INLINE`** (everything new).  One stream.  A binary payload
follows immediately after the newline of the command line that announced its
length, then the next command line follows immediately after the payload's last
byte.  No second pipe, no flush discipline, nothing to deadlock — and **stderr
is left free for diagnostics**, which matters for any helper on a runtime that
writes to stderr on its own account (a JVM emits warnings and stack traces
there and cannot be reliably stopped from doing so).

A payload's length is always stated on the line before it, so v1 and v2 differ
only in *which handle* the reader takes those bytes from.

## Coordinate spaces and units

This is the part of the contract that is easiest to get wrong, because the
protocol is deliberately **not** in a single space: the consumer transforms some
values and uses others as they arrive.  Both of the geometry bugs found while
bringing up the PDFBox backend were violations of this section, and neither
looked like a geometry bug from the outside -- one made all text invisible
rather than misplaced.

**Device space** is 1/7200 inch (`PDFI_OUTDEV_RESOLUTION`, `inc/pdfihelper.hxx`),
y **down**, origin at the top-left of the cropped and rotated page.  A PDF point
is 1/72 inch, so one point is exactly **100 device units**.

**User space** is PDF default user space for the page: points, y **up**, origin
at the crop box's lower-left corner.

| Value | Space | Why |
| --- | --- | --- |
| path points, clip paths | user | `pdfiprocessor.cxx` multiplies them by the declared CTM |
| **glyph rectangles** (`drawChar`) | user | same -- `drawCharGlyphs` calls `calcTransformedRectBounds` with the CTM |
| hyperlink rectangles (`drawLink`) | device | stored as page coordinates unchanged |
| page size (`startPage`) | device | used as the page extent directly |
| font size (`updateFont`) | device | `drawtreevisiting.cxx` converts back with `size*72/7200` |
| font matrix (`drawChar`) | none -- normalised | applied about the glyph origin, *in addition* to the font size |
| declared CTM (`updateCtm`) | user to device | see below |

### The declared CTM

`updateCtm` declares the transform the consumer applies to path and glyph
coordinates.  What it should contain depends on what your engine already did:

* If the engine reports path points with the content stream's own CTM **already
  applied** -- PDFBox's `PDFGraphicsStreamEngine` does -- then declare the page
  transform alone, and `q`/`Q` never has to be tracked.
* If the engine reports raw operands -- poppler's `GfxPath` does -- then declare
  the page transform composed with the current content CTM.

Either way the product reaching the consumer is the same.  Images are the
exception: `drawImage` places the unit square, so its CTM is always the page
transform composed with the image's own matrix.

### The page transform

With crop box `(x0,y0)-(x1,y1)` in points, `S = 100`, and `/Rotate` normalised to
0/90/180/270, as `(a,b,c,d,e,f)` mapping `(x,y)` to `(ax+cy+e, bx+dy+f)`:

    0     ( S,  0,  0, -S,  -S*x0,  S*y1 )
    90    ( 0,  S,  S,  0,  -S*y0, -S*x0 )
    180   (-S,  0,  0,  S,   S*x1, -S*y0 )
    270   ( 0, -S, -S,  0,   S*y1,  S*x1 )

Page size is `(x1-x0)*S` by `(y1-y0)*S`, swapped for 90 and 270.

`/Rotate` belongs **here**, not left to the consumer.  Do not assume a library
applies it: PDFBox does not, and a page processed at 0, 90 and 270 yields
byte-identical coordinates, so this fails silently and every landscape or
scanned document imports sideways.

### The font matrix

Send the text matrix divided by its own vertical scale, so that its vertical
scale becomes unity.  It carries orientation, shear and the horizontal/vertical
aspect ratio, and **no overall scale**.

The consumer applies it as a transform about each glyph's origin *and*
separately sizes the text from `updateFont`.  Sending a matrix that still
carries the type scale therefore applies that scale twice.  Documents that put
their scale in `Tm` with `Tf 1` are common, and poppler gets this wrong -- it
sends `getTransformedFontSize()`, which already includes `Tm`'s scale, next to
the raw `Tm`.

### Image encodings

`PNG`, `JPEG`, `PBM` and `PPM` are all accepted, and a backend should send
whichever the image already is rather than normalising to one.  Re-encoding a
lossy image losslessly is the expensive mistake: it is bit-exact about content
that was never crisp, and it multiplied one photographic document by three.
Send the stored bytes unchanged when they are already a usable JPEG, stay lossy
when the source was lossy, and keep PNG for anything stored losslessly.

### Capturing a stream for inspection

The stream is binary.  Redirect it with a shell that will not transcode:
PowerShell's `>` re-encodes as UTF-8 with a BOM and rewrites LF as CRLF, which
silently inflates every payload and makes a correct backend look like it has
regressed.  Use `bash` redirection, or have the helper write the file itself.

### Numbers

C locale, `printf` `%f`, always a dot.  A helper that emits `1,5` for `1.5` has
its coordinates read as `1.0` by `rtl::OString::toDouble` and silently displaces
the whole page.

### Worked example

A4 landscape, crop box `(0,0)-(794,595)`, `/Rotate 0`, 14pt text with `Tm` the
identity whose first glyph sits at user `(39.7, 40.2)`:

    startPage 79400 59500                     794*100 by 595*100
    updateCtm 100 0 0 -100 0 59500            S, 0, 0, -S, -S*x0, S*y1
    updateFont <id> 1 0 0 0 1400 0 <name>     14pt * 100
    drawChar 39.7 <top> <right> <bottom> 1 0 0 1 T

Note the two orders of magnitude between `updateCtm`/`updateFont` and
`drawChar`: that contrast is the check.  Glyph coordinates in the tens of
thousands mean device space has leaked into a user-space field, and the symptom
is text frames metres wide translated far off the canvas -- invisible, not
merely misplaced.

## Grammar

One command per line, tokens separated by single spaces, `\n`-terminated.
Numbers are C `printf` `%d` / `%f` / `%lld` in the **C locale** — a helper that
formats `1,5` for `1.5` will silently produce a corrupt document.

Two commands (`drawChar`, `updateFont`) end in a free-text field that consumes
the rest of the line; in those, `\n`, `\r` and `\` are escaped as `\n`, `\r`,
`\`.  Text is UTF-8.

### Document and page

| Command | Arguments |
| --- | --- |
| `setPageNum` | `<int nPages>` — total page count, sent once, before any page |
| `startPage` | `<double width> <double height>` |
| `endPage` | — |

### Graphics state

| Command | Arguments |
| --- | --- |
| `saveState` / `restoreState` | — |
| `updateCtm` | `<a> <b> <c> <d> <e> <f>` — PDF matrix order |
| `updateFillColor` / `updateStrokeColor` | `<r> <g> <b> <a>`, each 0..1 |
| `updateLineWidth` | `<double>` |
| `updateLineCap` | `0` butt, `1` round, `2` square |
| `updateLineJoin` | `0` miter, `1` round, `2` bevel |
| `updateMiterLimit` | `<double>` |
| `updateFlatness` | `<int>` |
| `updateLineDash` | *(empty = solid)*, else `<phase> <n> <d1> … <dn>` |
| `setTextRenderMode` | `<int>` — PDF Tr operator |

### Paths

`strokePath`, `fillPath`, `eoFillPath`, `clipPath`, `eoClipPath` all take the
same trailing path description:

    subpath <int closed> <x> <y> <int isControlPoint> ... [subpath ...]

Points with `isControlPoint = 1` come in pairs and, with the following ordinary
point, form one cubic Bézier segment.

`fillPath` and `eoFillPath` are additionally stroked by the caller when the
filled area is very small, so hairline table rules survive.

### Text

| Command | Arguments |
| --- | --- |
| `updateFont` | `<int64 id> <int embedded> <int bold> <int italic> <int underline> <double size> <int fileLen> <name…>` |
| `drawChar` | `<x1> <y1> <x2> <y2> <m00> <m01> <m10> <m11> <text…>` |
| `endTextObject` | — |

`updateFont` assigns a **stable numeric id** to a font; the caller caches it, so
the same id must not be re-described with a different `fileLen`.  If `fileLen`
is non-zero, that many bytes of the embedded font file follow on the binary
channel and the caller runs them through `com.sun.star.awt.FontIdentificator`
to recover the real family name.  Send `0` and a good name if the font is not
embedded.

### Images

    drawImage           <w> <h> <maskColors> <image>
    drawMask            <w> <h> <invert>     <image>
    drawMaskedImage     <w> <h> <mw> <mh> <invert> <image> <mask>
    drawSoftMaskedImage <w> <h> <mw> <mh>    <image> <mask>

where each `<image>` is the two tokens

    <format> <byteCount>

with `<format>` one of `PNG`, `JPEG`, `PBM`, `PPM`.  The payloads follow, in the
order the tokens appeared, after the end of the line.  For `drawImage` a
non-zero `maskColors` means a further `maskColors` bytes follow the image
payload: the first half are per-component minima, the second half maxima.

### Links

    drawLink <x1> <y1> <x2> <y2> <uri…>

## Writing a backend

Implement the grammar **and the coordinate section above**, pick v2, and add one
entry to the candidate table in `wrapper/pdfbackend.cxx`.  Nothing above
`ContentSink` changes, and nothing links against your engine: the abstraction
here is the process boundary, not a C++ interface, which is what keeps
`pdfimport.uno.dll` free of every engine's headers, libraries and licence.

The grammar is the easy half.  Everything that went wrong bringing up the first
non-poppler backend was semantic -- right tokens, wrong space -- so before
trusting a new helper, check it against the invariants that hold for any
backend:

* page size equals crop-box points times 100, with width and height swapped for
  `/Rotate` 90 and 270;
* every path and glyph coordinate falls inside the crop box (they are user
  space, so they are bounded by it);
* the font matrix has vertical scale 1;
* every announced payload length is consumed exactly, and the stream ends
  cleanly;
* no number contains a comma.

`PDFI_BACKEND` pins one engine and fails rather than falling through to another,
so the same document can be run through two backends and the output compared.

Set `PDFI_BACKEND` in the bootstrap environment to force a specific one —
`PDFI_BACKEND=xpdf` pins the legacy helper, which is how two backends are
compared on the same document.
