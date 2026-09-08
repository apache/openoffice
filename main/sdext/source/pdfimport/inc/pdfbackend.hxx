/**************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *************************************************************/

#ifndef INCLUDED_PDFI_PDFBACKEND_HXX
#define INCLUDED_PDFI_PDFBACKEND_HXX

#include "rtl/ustring.hxx"

#include <vector>

namespace com { namespace sun { namespace star {
    namespace uno { class XComponentContext; template <typename> class Reference; }
} } }

namespace pdfi
{
    /** Which handle a helper delivers binary payloads (embedded fonts,
        bitmaps) on.

        Only the framing differs between these; the token grammar is
        identical.  See PROTOCOL.md.
     */
    enum BinaryChannel
    {
        /** v1: text on stdout, binary on stderr.  The two processes must
            alternate between the pipes in lockstep or they deadlock, and
            stderr cannot be used for anything else.  Legacy xpdfimport only.
         */
        BINARY_ON_STDERR,

        /** v2: one stream.  Each payload follows the newline of the line that
            announced its length.  No flush discipline, and stderr stays free
            for diagnostics -- required for any helper whose runtime writes
            there on its own account.
         */
        BINARY_INLINE
    };

    /** A PDF parsing helper: what to spawn, and how it talks back.

        Everything backend-specific lives here.  Adding an engine means adding
        one of these to the table in pdfbackend.cxx -- no caller changes, and
        nothing above ContentSink is aware that there is a choice at all.
     */
    struct PdfBackend
    {
        /** Stable identifier, also what PDFI_BACKEND matches against. */
        rtl::OUString                   aId;

        /** File URL of the program to spawn -- or a bare name, which is
            resolved against PATH.  Empty means unavailable.
         */
        rtl::OUString                   aProgram;

        /** Arguments that precede the PDF path (e.g. -jar <helper>.jar). */
        std::vector< rtl::OUString >    aPrefixArgs;

        BinaryChannel                   eBinaryChannel;

        PdfBackend() : eBinaryChannel( BINARY_ON_STDERR ) {}

        bool isAvailable() const { return aProgram.getLength() != 0; }
    };

    /** Pick the backend to run.

        Candidates are tried in preference order and the first one whose
        program is actually present on disk wins, so a build that ships only
        some of them needs no configuration.  Setting the bootstrap variable
        PDFI_BACKEND to an id restricts the search to that one backend --
        which is how a document is rendered through two engines for
        comparison, and how a test pins the engine it is asserting against.

        @return a backend with isAvailable() false if none was found.
     */
    PdfBackend selectPdfBackend(
        const com::sun::star::uno::Reference<
              com::sun::star::uno::XComponentContext >& xContext );
}

#endif /* INCLUDED_PDFI_PDFBACKEND_HXX */
