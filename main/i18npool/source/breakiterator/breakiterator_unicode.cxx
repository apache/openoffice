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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_i18npool.hxx"
#include <breakiterator_unicode.hxx>
#include <localedata.hxx>
#include <unicode/uchar.h>
#include <unicode/locid.h>
#include <unicode/rbbi.h>
#include <unicode/udata.h>
#include <rtl/strbuf.hxx>
#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>

U_CDECL_BEGIN
extern const char OpenOffice_dat[];
U_CDECL_END

using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::rtl;

namespace com { namespace sun { namespace star { namespace i18n {

#define ERROR ::com::sun::star::uno::RuntimeException()

//#define ImplementName "com.sun.star.i18n.BreakIterator_Unicode";


BreakIterator_Unicode::BreakIterator_Unicode() :
    cBreakIterator( "com.sun.star.i18n.BreakIterator_Unicode" ),    // implementation name
    wordRule( "word" ),
    lineRule( "line" ),
    result(),
    character(),
    word(),
    sentence(),
    line(),
    icuBI( NULL ),
    aLocale(),
    aBreakType(),
    aWordType()
{
}


BreakIterator_Unicode::~BreakIterator_Unicode()
{
        if (icuBI && icuBI->aBreakIterator) {
            ubrk_close(icuBI->aBreakIterator);
            icuBI->aBreakIterator=NULL;
        }
        if (character.aBreakIterator) ubrk_close(character.aBreakIterator);
        if (word.aBreakIterator) ubrk_close(word.aBreakIterator);
        if (sentence.aBreakIterator) ubrk_close(sentence.aBreakIterator);
        if (line.aBreakIterator) ubrk_close(line.aBreakIterator);
}

static UBreakIterator* open_udata_BreakIterator(UDataMemory *udm, UErrorCode *status)
{
    *status = U_ZERO_ERROR;
    UDataInfo info;
    info.size = sizeof(info);
    udata_getInfo(udm, &info);
    if (  !(
#if defined(OSL_LITENDIAN)
            !info.isBigEndian  &&
#else
            info.isBigEndian &&
#endif
            info.charsetFamily == U_CHARSET_FAMILY &&
            info.dataFormat[0] == 0x42 &&  // dataFormat="Brk "
            info.dataFormat[1] == 0x72 &&
            info.dataFormat[2] == 0x6b &&
            info.dataFormat[3] == 0x20
           )
    ) {
        *status = U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    uint8_t *memory = (uint8_t*) udata_getMemory(udm);
    // FIXME: We have no way to get the real length, without parsing private data out first.
    // But ubrk_openBinaryRules() calls
    //     RuleBasedBreakIterator::RuleBasedBreakIterator(const uint8_t *compiledRules,
    //                 uint32_t       ruleLength,
    //                 UErrorCode     &status)
    // which, ***AT THE TIME OF WRITING***, only does these checks, and nothing else:
    // (compiledRules == nullptr || ruleLength < sizeof(RBBIDataHeader))
    // (data->fLength > ruleLength)
    // both those must be false for this to work, so we just use an arbitrary large number:
    uint32_t length = 1000000000;
    return ubrk_openBinaryRules(memory, length, NULL, 0, status);
}

// loading ICU breakiterator on demand.
void SAL_CALL BreakIterator_Unicode::loadICUBreakIterator(const com::sun::star::lang::Locale& rLocale,
        sal_Int16 rBreakType, sal_Int16 rWordType, const sal_Char *rule, const OUString& rText) throw(uno::RuntimeException)
{
    sal_Bool newBreak = sal_False;
    UErrorCode status = U_ZERO_ERROR;
    sal_Int16 breakType = 0;
    switch (rBreakType) {
        case LOAD_CHARACTER_BREAKITERATOR: icuBI=&character; breakType = 3; break;
        case LOAD_WORD_BREAKITERATOR: icuBI=&word; 
            switch (rWordType) {
                case WordType::ANYWORD_IGNOREWHITESPACES: breakType = 0; rule=wordRule = "edit_word"; break;
                case WordType::DICTIONARY_WORD: breakType = 1; rule=wordRule = "dict_word"; break;
                case WordType::WORD_COUNT: breakType = 2; rule=wordRule = "count_word"; break;
            }
            break;
        case LOAD_SENTENCE_BREAKITERATOR: icuBI=&sentence; breakType = 5; break;
        case LOAD_LINE_BREAKITERATOR: icuBI=&line; breakType = 4; break;
    }
    if (!icuBI->aBreakIterator || rWordType != aWordType ||
            rLocale.Language != aLocale.Language || rLocale.Country != aLocale.Country ||
            rLocale.Variant != aLocale.Variant) {
        if (icuBI->aBreakIterator) {
            ubrk_close(icuBI->aBreakIterator);
            icuBI->aBreakIterator=NULL;
        }
        if (rule) {
            uno::Sequence< OUString > breakRules = LocaleData().getBreakIteratorRules(rLocale);

            status = U_ZERO_ERROR;
            udata_setAppData("OpenOffice", OpenOffice_dat, &status);
            if ( !U_SUCCESS(status) ) throw ERROR;

            UBreakIterator *rbi = NULL;

            if (breakRules.getLength() > breakType && breakRules[breakType].getLength() > 0) {
                rbi = open_udata_BreakIterator(udata_open("OpenOffice", "brk", 
                    OUStringToOString(breakRules[breakType], RTL_TEXTENCODING_ASCII_US).getStr(), &status), &status);
            } else {
                status = U_ZERO_ERROR;
                OStringBuffer aUDName(64);
                aUDName.append(rule);
                aUDName.append('_');
                aUDName.append( OUStringToOString(rLocale.Language, RTL_TEXTENCODING_ASCII_US));
                UDataMemory* pUData = udata_open("OpenOffice", "brk", aUDName.getStr(), &status);
                if( U_SUCCESS(status) )
                    rbi = open_udata_BreakIterator( pUData, &status);
                if (!U_SUCCESS(status) ) {
                    status = U_ZERO_ERROR;
                    pUData = udata_open("OpenOffice", "brk", rule, &status);
                    if( U_SUCCESS(status) )
                        rbi = open_udata_BreakIterator( pUData, &status);
                    if (!U_SUCCESS(status) ) icuBI->aBreakIterator=NULL;
                }
            }
            if (rbi) {
                // This was always a bad idea, as it required calling a private, internal ICU API,
                // and isn't possible since ICU >= 61.1 as per https://unicode-org.atlassian.net/browse/ICU-10688.
                //switch (rBreakType) {
                //    case LOAD_CHARACTER_BREAKITERATOR: rbi->publicSetBreakType(UBRK_CHARACTER); break;
                //    case LOAD_WORD_BREAKITERATOR: rbi->publicSetBreakType(UBRK_WORD); break;
                //    case LOAD_SENTENCE_BREAKITERATOR: rbi->publicSetBreakType(UBRK_SENTENCE); break;
                //    case LOAD_LINE_BREAKITERATOR: rbi->publicSetBreakType(UBRK_LINE); break;
                //}
                icuBI->aBreakIterator = rbi;
            }
        }

        if (!icuBI->aBreakIterator) {
            ::rtl::OUStringBuffer locale;
            if (!rLocale.Language.isEmpty()) {
                locale.append(rLocale.Language);
                locale.appendAscii("_");
                if (!rLocale.Country.isEmpty()) {
                    locale.append(rLocale.Country);
                }
                if (!rLocale.Variant.isEmpty()) {
                    locale.appendAscii("_");
                    locale.append(rLocale.Variant);
                }
            }
            char icuLocale[1024];
            uloc_getName(OUStringToOString(locale.makeStringAndClear(), RTL_TEXTENCODING_ASCII_US).getStr(), icuLocale, sizeof(icuLocale), &status);

            status = U_ZERO_ERROR;
            switch (rBreakType) {
                case LOAD_CHARACTER_BREAKITERATOR:
                    icuBI->aBreakIterator = ubrk_open(UBRK_CHARACTER, icuLocale, NULL, 0, &status);
                    break;
                case LOAD_WORD_BREAKITERATOR:
                    icuBI->aBreakIterator = ubrk_open(UBRK_WORD, icuLocale, NULL, 0, &status);
                    break;
                case LOAD_SENTENCE_BREAKITERATOR:
                    icuBI->aBreakIterator = ubrk_open(UBRK_SENTENCE, icuLocale, NULL, 0, &status);
                    break;
                case LOAD_LINE_BREAKITERATOR:
                    icuBI->aBreakIterator = ubrk_open(UBRK_LINE, icuLocale, NULL, 0, &status);
                    break;
            }
            if ( !U_SUCCESS(status) ) {
                icuBI->aBreakIterator=NULL;
                throw ERROR;
            }
        }
        if (icuBI->aBreakIterator) {
            aLocale=rLocale;
            aWordType=rWordType;
            aBreakType=rBreakType;
            newBreak=sal_True;
        } else {
            throw ERROR;
        }
    }

    if (newBreak || icuBI->aICUText.compareTo(rText)) {	// UChar != sal_Unicode in MinGW
        icuBI->aICUText = rText;
        ubrk_setText(icuBI->aBreakIterator, (const UChar*)icuBI->aICUText.getStr(), icuBI->aICUText.getLength(), &status);
    }
}


sal_Int32 SAL_CALL BreakIterator_Unicode::nextCharacters( const OUString& Text,
        sal_Int32 nStartPos, const lang::Locale &rLocale,
        sal_Int16 nCharacterIteratorMode, sal_Int32 nCount, sal_Int32& nDone )
        throw(uno::RuntimeException)
{
        if (nCharacterIteratorMode == CharacterIteratorMode::SKIPCELL ) { // for CELL mode
            loadICUBreakIterator(rLocale, LOAD_CHARACTER_BREAKITERATOR, 0, "char", Text);
            for (nDone = 0; nDone < nCount; nDone++) {
                nStartPos = ubrk_following(character.aBreakIterator, nStartPos);
                if (nStartPos == UBRK_DONE)
                    return Text.getLength();
            }
        } else { // for CHARACTER mode
            for (nDone = 0; nDone < nCount && nStartPos < Text.getLength(); nDone++)
                Text.iterateCodePoints(&nStartPos, 1);
        }
        return nStartPos;
}

sal_Int32 SAL_CALL BreakIterator_Unicode::previousCharacters( const OUString& Text,
        sal_Int32 nStartPos, const lang::Locale& rLocale,
        sal_Int16 nCharacterIteratorMode, sal_Int32 nCount, sal_Int32& nDone )
        throw(uno::RuntimeException)
{
        if (nCharacterIteratorMode == CharacterIteratorMode::SKIPCELL ) { // for CELL mode
            loadICUBreakIterator(rLocale, LOAD_CHARACTER_BREAKITERATOR, 0, "char", Text);
            for (nDone = 0; nDone < nCount; nDone++) {
                nStartPos = ubrk_preceding(character.aBreakIterator, nStartPos);
                if (nStartPos == UBRK_DONE)
                    return 0;
            }
        } else { // for BS to delete one char and CHARACTER mode.
            for (nDone = 0; nDone < nCount && nStartPos > 0; nDone++)
                Text.iterateCodePoints(&nStartPos, -1);
        }
        return nStartPos;
}


Boundary SAL_CALL BreakIterator_Unicode::nextWord( const OUString& Text, sal_Int32 nStartPos,
    const lang::Locale& rLocale, sal_Int16 rWordType ) throw(uno::RuntimeException)
{
        loadICUBreakIterator(rLocale, LOAD_WORD_BREAKITERATOR, rWordType, NULL, Text);

        result.startPos = ubrk_following(word.aBreakIterator, nStartPos);
        if( result.startPos >= Text.getLength() || result.startPos == UBRK_DONE )
            result.endPos = result.startPos;
        else {
            if ( (rWordType == WordType::ANYWORD_IGNOREWHITESPACES ||
                    rWordType == WordType::DICTIONARY_WORD ) &&
                        u_isWhitespace(Text.iterateCodePoints(&result.startPos, 0)) )
                result.startPos = ubrk_following(word.aBreakIterator, result.startPos);

            result.endPos = ubrk_following(word.aBreakIterator, result.startPos);
            if(result.endPos == UBRK_DONE)
                result.endPos = result.startPos;
        }
        return result;
}


Boundary SAL_CALL BreakIterator_Unicode::previousWord(const OUString& Text, sal_Int32 nStartPos,
        const lang::Locale& rLocale, sal_Int16 rWordType) throw(uno::RuntimeException)
{
        loadICUBreakIterator(rLocale, LOAD_WORD_BREAKITERATOR, rWordType, NULL, Text);

        result.startPos = ubrk_preceding(word.aBreakIterator, nStartPos);
        if( result.startPos < 0 || result.startPos == UBRK_DONE)
            result.endPos = result.startPos;
        else {
            if ( (rWordType == WordType::ANYWORD_IGNOREWHITESPACES ||
                    rWordType == WordType::DICTIONARY_WORD) &&
                        u_isWhitespace(Text.iterateCodePoints(&result.startPos, 0)) )
                result.startPos = ubrk_preceding(word.aBreakIterator, result.startPos);

            result.endPos = ubrk_following(word.aBreakIterator, result.startPos);
            if(result.endPos == UBRK_DONE)
                result.endPos = result.startPos;
        }
        return result;
}


Boundary SAL_CALL BreakIterator_Unicode::getWordBoundary( const OUString& Text, sal_Int32 nPos, const lang::Locale& rLocale,
        sal_Int16 rWordType, sal_Bool bDirection ) throw(uno::RuntimeException)
{
        loadICUBreakIterator(rLocale, LOAD_WORD_BREAKITERATOR, rWordType, NULL, Text);
        sal_Int32 len = Text.getLength();

        if(ubrk_isBoundary(word.aBreakIterator, nPos)) {
            result.startPos = result.endPos = nPos;
            if((bDirection || nPos == 0) && nPos < len) //forward
                result.endPos = ubrk_following(word.aBreakIterator, nPos);
            else
                result.startPos = ubrk_preceding(word.aBreakIterator, nPos);
        } else {
            if(nPos <= 0) {
                result.startPos = 0;
                result.endPos = len ? ubrk_following(word.aBreakIterator, (sal_Int32)0) : 0;
            } else if(nPos >= len) {
                result.startPos = ubrk_preceding(word.aBreakIterator, len);
                result.endPos = len;
            } else {
                result.startPos = ubrk_preceding(word.aBreakIterator, nPos);
                result.endPos = ubrk_following(word.aBreakIterator, nPos);
            }
        }
        if (result.startPos == UBRK_DONE)
            result.startPos = result.endPos;
        else if (result.endPos == UBRK_DONE)
            result.endPos = result.startPos;

        return result;
}


sal_Int32 SAL_CALL BreakIterator_Unicode::beginOfSentence( const OUString& Text, sal_Int32 nStartPos,
        const lang::Locale &rLocale ) throw(uno::RuntimeException)
{
        loadICUBreakIterator(rLocale, LOAD_SENTENCE_BREAKITERATOR, 0, "sent", Text);

        sal_Int32 len = Text.getLength();
        if (len > 0 && nStartPos == len)
            Text.iterateCodePoints(&nStartPos, -1); // issue #i27703# treat end position as part of last sentence
        if (!ubrk_isBoundary(sentence.aBreakIterator, nStartPos))
            nStartPos = ubrk_preceding(sentence.aBreakIterator, nStartPos);

        // skip preceding space.
        sal_uInt32 ch = Text.iterateCodePoints(&nStartPos, 1);
        while (nStartPos < len && u_isWhitespace(ch)) ch = Text.iterateCodePoints(&nStartPos, 1);
		Text.iterateCodePoints(&nStartPos, -1);

        return nStartPos;
}

sal_Int32 SAL_CALL BreakIterator_Unicode::endOfSentence( const OUString& Text, sal_Int32 nStartPos,
        const lang::Locale &rLocale ) throw(uno::RuntimeException)
{
        loadICUBreakIterator(rLocale, LOAD_SENTENCE_BREAKITERATOR, 0, "sent", Text);

        sal_Int32 len = Text.getLength();
        if (len > 0 && nStartPos == len)
            Text.iterateCodePoints(&nStartPos, -1); // issue #i27703# treat end position as part of last sentence
        nStartPos = ubrk_following(sentence.aBreakIterator, nStartPos);

        sal_Int32 nPos=nStartPos;
        while (nPos > 0 && u_isWhitespace(Text.iterateCodePoints(&nPos, -1))) nStartPos=nPos;

        return nStartPos;
}

LineBreakResults SAL_CALL BreakIterator_Unicode::getLineBreak(
        const OUString& Text, sal_Int32 nStartPos,
        const lang::Locale& rLocale, sal_Int32 nMinBreakPos,
        const LineBreakHyphenationOptions& hOptions,
        const LineBreakUserOptions& /*rOptions*/ ) throw(uno::RuntimeException)
{
        LineBreakResults lbr;

        if (nStartPos >= Text.getLength()) {
            lbr.breakIndex = Text.getLength();
            lbr.breakType = BreakType::WORDBOUNDARY;
            return lbr;
        }

        loadICUBreakIterator(rLocale, LOAD_LINE_BREAKITERATOR, 0, lineRule, Text);

        sal_Bool GlueSpace=sal_True;
        while (GlueSpace) {
            if (ubrk_preceding(line.aBreakIterator, nStartPos + 1) == nStartPos) { //Line boundary break
                lbr.breakIndex = nStartPos;
                lbr.breakType = BreakType::WORDBOUNDARY;
            } else if (hOptions.rHyphenator.is()) { //Hyphenation break
                Boundary wBoundary = getWordBoundary( Text, nStartPos, rLocale,
                                                WordType::DICTIONARY_WORD, false);
                uno::Reference< linguistic2::XHyphenatedWord > aHyphenatedWord;
                aHyphenatedWord = hOptions.rHyphenator->hyphenate(Text.copy(wBoundary.startPos, 
                    wBoundary.endPos - wBoundary.startPos), rLocale, 
                    (sal_Int16) (hOptions.hyphenIndex - wBoundary.startPos), hOptions.aHyphenationOptions);
                if (aHyphenatedWord.is()) {
                    lbr.rHyphenatedWord = aHyphenatedWord;
                    if(wBoundary.startPos + aHyphenatedWord->getHyphenationPos() + 1 < nMinBreakPos )
                        lbr.breakIndex = -1;
                    else
                        lbr.breakIndex = wBoundary.startPos; //aHyphenatedWord->getHyphenationPos();
                    lbr.breakType = BreakType::HYPHENATION;
                } else {
                    lbr.breakIndex = ubrk_preceding(line.aBreakIterator, nStartPos);
                    lbr.breakType = BreakType::WORDBOUNDARY;
                }
            } else { //word boundary break
                lbr.breakIndex = ubrk_preceding(line.aBreakIterator, nStartPos);
                lbr.breakType = BreakType::WORDBOUNDARY;
            }

#define WJ 0x2060   // Word Joiner
            GlueSpace=sal_False;
            if (lbr.breakType == BreakType::WORDBOUNDARY) {
                nStartPos = lbr.breakIndex;
                if (Text[nStartPos--] == WJ)
                    GlueSpace=sal_True;
                while (nStartPos >= 0 &&
                    (u_isWhitespace(Text.iterateCodePoints(&nStartPos, 0)) || Text[nStartPos] == WJ)) {
                    if (Text[nStartPos--] == WJ)
                        GlueSpace=sal_True;
                }
                if (GlueSpace && nStartPos < 0)  {
                    lbr.breakIndex = 0;
                    break;
                }
            }
        }

        return lbr;
}



OUString SAL_CALL
BreakIterator_Unicode::getImplementationName(void) throw( uno::RuntimeException )
{
        return OUString::createFromAscii(cBreakIterator);
}

sal_Bool SAL_CALL
BreakIterator_Unicode::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
        return !rServiceName.compareToAscii(cBreakIterator);
}

uno::Sequence< OUString > SAL_CALL
BreakIterator_Unicode::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
        uno::Sequence< OUString > aRet(1);
        aRet[0] = OUString::createFromAscii(cBreakIterator);
        return aRet;
}

} } } }
