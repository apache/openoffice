/** @file
    File:       IccTagBasic.cpp

    Contains:   Implementation of the CIccTag class and basic inherited classes

    Version:    V1

    Copyright:  © see ICC Software License
*/

/*
 * The ICC Software License, Version 0.1
 *
 *
 * Copyright (c) 2003-2006 The International Color Consortium. All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        The International Color Consortium (www.color.org)"
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. In the absence of prior written permission, the names "ICC" and "The
 *    International Color Consortium" must not be used to imply that the
 *    ICC organization endorses or promotes products derived from this
 *    software.
 *
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNATIONAL COLOR CONSORTIUM OR
 * ITS CONTRIBUTING MEMBERS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the The International Color Consortium. 
 *
 *
 * Membership in the ICC is encouraged when this software is used for
 * commercial purposes. 
 *
 *  
 * For more information on The International Color Consortium, please
 * see <http://www.color.org/>.
 *  
 * 
 */

////////////////////////////////////////////////////////////////////// 
// HISTORY:
//
// -Initial implementation by Max Derhak 5-15-2003
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
  #pragma warning( disable: 4786) //disable warning in <list.h>
  #include <windows.h>
#endif
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "IccTag.h"
#include "IccUtil.h"
#include "IccProfile.h"
#include "IccTagFactory.h"

#ifndef __min
#include <algorithm>
using std::min;
#define __min min
#endif

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif


/**
 ****************************************************************************
 * Name: CIccTag::CIccTag
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTag::CIccTag()
{
  m_nReserved = 0;
}

/**
 ****************************************************************************
 * Name: CIccTag::CIccTag
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTag::~CIccTag()
{

}

/**
 ****************************************************************************
 * Name: CIccTag::Create
 * 
 * Purpose: This is a static tag creator based upon tag signature type
 * 
 * Args: 
 *  sig = tag type signature
 * 
 * Return: Pointer to Allocated tag 
 *****************************************************************************
 */
CIccTag* CIccTag::Create(icTagTypeSignature sig)
{
  return CIccTagCreator::CreateTag(sig);
}


/**
 ******************************************************************************
 * Name: CIccTag::Validate
 * 
 * Purpose: Check tag data validity.  In base class we only look at the
 *  tag's reserved data value
 * 
 * Args: 
 *  sig = signature of tag being validated,
 *  sReport = String to add report information to
 * 
 * Return: 
 *  icValidateStatusOK if valid, or other error status.
 ******************************************************************************
 */
icValidateStatus CIccTag::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = icValidateOK;
  
  if (m_nReserved != 0) {
    CIccInfo Info;
    sReport += icValidateNonCompliantMsg;
    sReport += Info.GetSigName(sig);
    sReport += " - Reserved Value must be zero.\n";

    rv = icValidateNonCompliant;
  }

  return rv;
}


/**
 ****************************************************************************
 * Name: CIccTagUnknown::CIccTagUnknown
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagUnknown::CIccTagUnknown()
{
  m_nType = icSigUnknownType;
  m_pData = NULL;
}

/**
 ****************************************************************************
 * Name: CIccTagUnknown::CIccTagUnknown
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITU = The CIccTagUnknown object to be copied
 *****************************************************************************
 */
CIccTagUnknown::CIccTagUnknown(const CIccTagUnknown &ITU)
{
  m_nSize = ITU.m_nSize;
  m_nType = ITU.m_nType;

  m_pData = new icUInt8Number[m_nSize];
  memcpy(m_pData, ITU.m_pData, sizeof(m_pData));
}

/**
 ****************************************************************************
 * Name: CIccTagUnknown::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  UnknownTag = The CIccTagUnknown object to be copied
 *****************************************************************************
 */
CIccTagUnknown &CIccTagUnknown::operator=(const CIccTagUnknown &UnknownTag)
{
  if (&UnknownTag == this)
    return *this;

  m_nSize = UnknownTag.m_nSize;
  m_nType = UnknownTag.m_nType;

  if (m_pData)
    delete [] m_pData;
  m_pData = new icUInt8Number[m_nSize];
  memcpy(m_pData, UnknownTag.m_pData, sizeof(m_pData));

  return *this;
}

/**
 ****************************************************************************
 * Name: CIccTagUnknown::~CIccTagUnknown
 * 
 * Purpose: Destructor
 *****************************************************************************
 */
CIccTagUnknown::~CIccTagUnknown()
{
  if (m_pData)
    delete [] m_pData;
}


/**
 ****************************************************************************
 * Name: CIccTagUnknown::Read
 * 
 * Purpose: Read in an unknown tag type into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagUnknown::Read(icUInt32Number size, CIccIO *pIO)
{
  if (m_pData) {
    delete [] m_pData;

  }

  if (size<sizeof(icTagTypeSignature) || !pIO) {
    m_pData = NULL;
    return false;
  }

  if (!pIO->Read32(&m_nType))
    return false;

  m_nSize = size - sizeof(icTagTypeSignature);

  if (m_nSize) {

    m_pData = new icUInt8Number[m_nSize];

    if (pIO->Read8(m_pData, m_nSize) != m_nSize) {
      return false;
    }
  }

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagUnknown::Write
 * 
 * Purpose: Write an unknown tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagUnknown::Write(CIccIO *pIO)
{
  if (!pIO)
   return false;

  if (!pIO->Write32(&m_nType))
   return false;

  if (m_nSize && m_pData) {
   if (pIO->Write8(m_pData, m_nSize) !=m_nSize)
     return false;
  }

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagUnknown::Describe
 * 
 * Purpose: Dump data associated with unknown tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagUnknown::Describe(std::string &sDescription)
{
  icChar buf[128];

  sDescription = "Unknown Tag Type of ";
  sprintf(buf, "%u Bytes.", m_nSize-4);
  sDescription += buf;

  sDescription += "\n\nData Follows:\n";

  icMemDump(sDescription, m_pData+4, m_nSize-4);
}

/**
 ****************************************************************************
 * Name: CIccTagText::CIccTagText
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagText::CIccTagText()
{
  m_szText = (icChar*)malloc(1);
  m_szText[0] = '\0';
  m_nBufSize = 1;
}

/**
 ****************************************************************************
 * Name: CIccTagText::CIccTagText
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITT = The CIccTagText object to be copied
 *****************************************************************************
 */
CIccTagText::CIccTagText(const CIccTagText &ITT)
{
  m_szText = (icChar*)malloc(1);
  m_szText[0] = '\0';
  m_nBufSize = 1;
  SetText(ITT.m_szText);
}

/**
 ****************************************************************************
 * Name: CIccTagText::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  TextTag = The CIccTagText object to be copied
 *****************************************************************************
 */
CIccTagText &CIccTagText::operator=(const CIccTagText &TextTag)
{
  if (&TextTag == this)
    return *this;

  m_szText = (icChar*)malloc(1);
  m_szText[0] = '\0';
  m_nBufSize = 1;
  SetText(TextTag.m_szText);

  return *this;
}

/**
 ****************************************************************************
 * Name: CIccTagText::~CIccTagText
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagText::~CIccTagText()
{
  free(m_szText);
}

/**
 ****************************************************************************
 * Name: CIccTagText::Read
 * 
 * Purpose: Read in a text type tag into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagText::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  if (size<sizeof(icTagTypeSignature) || !pIO) {
    m_szText[0] = '\0';
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  icUInt32Number nSize = size - sizeof(icTagTypeSignature) - sizeof(icUInt32Number);

  icChar *pBuf = GetBuffer(nSize);

  if (nSize) {
    if (pIO->Read8(pBuf, nSize) != nSize) {
      return false;
    }
  }

  Release();

  return true;
}

/**
 ****************************************************************************
 * Name: CIccTagText::Write
 * 
 * Purpose: Write a text type tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagText::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!m_szText)
    return false;

  icUInt32Number nSize = strlen(m_szText)+1;

  if (pIO->Write8(m_szText, nSize) !=nSize)
    return false;

  return true;
}

/**
 ****************************************************************************
 * Name: CIccTagText::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagText::Describe(std::string &sDescription)
{
  sDescription += "\"";
  if (m_szText && *m_szText)
    sDescription += m_szText;

  sDescription += "\"\n";
}


/**
 ****************************************************************************
 * Name: CIccTagText::SetText
 * 
 * Purpose: Allows text data associated with the tag to be set.
 * 
 * Args: 
 *  szText - zero terminated string to put in tag
 *****************************************************************************
 */
void CIccTagText::SetText(const icChar *szText)
{
  if (!szText) 
    SetText("");

  icUInt32Number len=strlen(szText) + 1;
  icChar *szBuf = GetBuffer(len);

  strcpy(szBuf, szText);
  Release();
}

/**
 ****************************************************************************
 * Name: *CIccTagText::operator=
 * 
 * Purpose: Define assignment operator to associate text with tag.
 * 
 * Args: 
 *  szText - zero terminated string to put in the tag
 *
 * Return: A pointer to the string assigned to the tag.
 *****************************************************************************
 */
const icChar *CIccTagText::operator=(const icChar *szText)
{
  SetText(szText);
  return m_szText;
}

/**
 ******************************************************************************
 * Name: CIccTagText::GetBuffer
 * 
 * Purpose: This function allocates room and returns pointer to data buffer to
 *  put string into
 * 
 * Args: 
 *  nSize = Requested size of data buffer.
 * 
 * Return: The character buffer array
 *******************************************************************************
 */
icChar *CIccTagText::GetBuffer(icUInt32Number nSize)
{
  if (m_nBufSize < nSize) {
    m_szText = (icChar*)realloc(m_szText, nSize+1);

    m_szText[nSize] = '\0';

    m_nBufSize = nSize;
  }

  return m_szText;
}

/**
 ****************************************************************************
 * Name: CIccTagText::Release
 * 
 * Purpose: This will resize the buffer to fit the zero terminated string in
 *  the buffer.
 *****************************************************************************
 */
void CIccTagText::Release()
{
  icUInt32Number nSize = strlen(m_szText)+1;

  if (nSize < m_nBufSize-1) {
    m_szText=(icChar*)realloc(m_szText, nSize+1);
    m_nBufSize = nSize+1;
  }
}


/**
******************************************************************************
* Name: CIccTagText::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagText::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (m_nBufSize) {
    switch(sig) {
    case icSigCopyrightTag:
      break;
    case icSigCharTargetTag:
      if (m_nBufSize<7) {
        sReport += icValidateNonCompliantMsg;
        sReport += sSigName;
        sReport += " - Tag must have at least seven text characters.\n";
        rv = icMaxStatus(rv, icValidateNonCompliant);
      }
      break;
    default:
      sReport += icValidateWarningMsg;
      sReport += sSigName;
      sReport += " - Unknown Tag.\n";
      rv = icMaxStatus(rv, icValidateWarning);
    }
    int i;
    for (i=0; m_szText[i] && i<(int)m_nBufSize; i++) {
      if (m_szText[i]&0x80) {
        sReport += icValidateWarning;
        sReport += sSigName;
        sReport += " - Text do not contain 7bit data.\n";
      }
    }
  }
  else {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " - Empty Tag.\n";
    rv = icMaxStatus(rv, icValidateWarning);
  }


  return rv;
}

/**
 ****************************************************************************
 * Name: CIccTagTextDescription::CIccTagTextDescription
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagTextDescription::CIccTagTextDescription()
{
  m_szText = (icChar*)malloc(1);
  m_szText[0] = '\0';
  m_nASCIISize = 1;

  m_uzUnicodeText = (icUInt16Number*)malloc(sizeof(icUInt16Number));
  m_uzUnicodeText[0] = 0;
  m_nUnicodeSize = 1;
  m_nUnicodeLanguageCode = 0;

  m_nScriptSize = 0;
  m_nScriptCode = 0;
  memset(m_szScriptText, 0, sizeof(m_szScriptText));
}

/**
 ****************************************************************************
 * Name: CIccTagTextDescription::CIccTagTextDescription
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITTD = The CIccTagTextDescription object to be copied
 *****************************************************************************
 */
CIccTagTextDescription::CIccTagTextDescription(const CIccTagTextDescription &ITTD)
{
  m_nASCIISize = ITTD.m_nASCIISize;
  m_nUnicodeSize = ITTD.m_nUnicodeSize;
  m_nUnicodeLanguageCode = ITTD.m_nUnicodeLanguageCode;
  m_nScriptSize = ITTD.m_nScriptSize;
  m_nScriptCode = ITTD.m_nScriptCode;

  if (m_nASCIISize) {
    m_szText = (icChar*)malloc(m_nASCIISize * sizeof(icChar));
    memcpy(m_szText, ITTD.m_szText, m_nASCIISize*sizeof(icChar));
  }
  else {
    m_nASCIISize = 1;
    m_szText = (icChar*)calloc(m_nASCIISize, sizeof(icChar));
    m_szText[0] = '\0';
  }

  if (m_nUnicodeSize) {
    m_uzUnicodeText = (icUInt16Number*)malloc((m_nUnicodeSize) * sizeof(icUInt16Number));
    memcpy(m_uzUnicodeText, ITTD.m_uzUnicodeText, m_nUnicodeSize*sizeof(icUInt16Number));
  }
  else {
    m_nUnicodeSize = 1;
    m_uzUnicodeText = (icUInt16Number*)calloc(m_nUnicodeSize, sizeof(icUInt16Number));
    m_uzUnicodeText[0] = 0;
  }

  memcpy(m_szScriptText, ITTD.m_szScriptText, sizeof(m_szScriptText));
}


/**
 ****************************************************************************
 * Name: CIccTagTextDescription::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  TextDescTag = The CIccTagTextDescription object to be copied
 *****************************************************************************
 */
CIccTagTextDescription &CIccTagTextDescription::operator=(const CIccTagTextDescription& TextDescTag)
{
  if (&TextDescTag == this)
    return *this;

  m_nASCIISize = TextDescTag.m_nASCIISize;
  m_nUnicodeSize = TextDescTag.m_nUnicodeSize;
  m_nUnicodeLanguageCode = TextDescTag.m_nUnicodeLanguageCode;
  m_nScriptSize = TextDescTag.m_nScriptSize;
  m_nScriptCode = TextDescTag.m_nScriptCode;

  if (m_szText)
    free(m_szText);
  if (m_nASCIISize) {
    m_szText = (icChar*)calloc(m_nASCIISize, sizeof(icChar));
    memcpy(m_szText, TextDescTag.m_szText, m_nASCIISize*sizeof(icChar));
  } 
  else {
    m_nASCIISize = 1;
    m_szText = (icChar*)calloc(m_nASCIISize, sizeof(icChar));
    m_szText[0] = '\0';
  }

  if (m_uzUnicodeText)
    free(m_uzUnicodeText);
  if (m_nUnicodeSize) {
    m_uzUnicodeText = (icUInt16Number*)calloc(m_nUnicodeSize, sizeof(icUInt16Number));
    memcpy(m_uzUnicodeText, TextDescTag.m_uzUnicodeText, m_nUnicodeSize*sizeof(icUInt16Number));
  }
  else {
    m_nUnicodeSize = 1;
    m_uzUnicodeText = (icUInt16Number*)calloc(m_nUnicodeSize, sizeof(icUInt16Number));
    m_uzUnicodeText[0] = 0;
  }

  memcpy(m_szScriptText, TextDescTag.m_szScriptText, sizeof(m_szScriptText));

  return *this;  
}

/**
 ****************************************************************************
 * Name: CIccTagTextDescription::~CIccTagTextDescription
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagTextDescription::~CIccTagTextDescription()
{
  free(m_szText);
  free(m_uzUnicodeText);
}

/**
 ****************************************************************************
 * Name: CIccTagTextDescription::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagTextDescription::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt32Number nEnd;

  nEnd = pIO->Tell() + size;

  if (size<3*sizeof(icUInt32Number) || !pIO) {
    m_szText[0] = '\0';
    return false;
  }

  icUInt32Number nSize;

  if (!pIO->Read32(&sig) ||
      !pIO->Read32(&m_nReserved) ||
      !pIO->Read32(&nSize))
    return false;

  if (3*sizeof(icUInt32Number) + nSize > size)
    return false;

  icChar *pBuf = GetBuffer(nSize);

  if (nSize) {
    if (pIO->Read8(pBuf, nSize) != nSize) {
      return false;
    }
  }
  else 
    m_szText[0] = '\0';
  
  Release();

  if (pIO->Tell() + 2 * sizeof(icUInt32Number) > nEnd)
    return false;

  if (!pIO->Read32(&m_nUnicodeLanguageCode) ||
      !pIO->Read32(&nSize))
    return false;

  icUInt16Number *pBuf16 = GetUnicodeBuffer(nSize);

  if (nSize) {
    if (pIO->Read16(pBuf16, nSize) != nSize) {
      return false;
    }
  }
  else 
    pBuf16[0] = 0;

  ReleaseUnicode();

  if (pIO->Tell()+3 > (icInt32Number)nEnd)
    return false;

  if (!pIO->Read16(&m_nScriptCode) ||
      !pIO->Read8(&m_nScriptSize))
     return false;
  
  if (pIO->Tell() + m_nScriptSize> (icInt32Number)nEnd ||
      m_nScriptSize > sizeof(m_szScriptText))
    return false;

  if (pIO->Read8(m_szScriptText, 67)!= 67)
    return false;

  return true;
}

/**
 ****************************************************************************
 * Name: CIccTagTextDescription::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagTextDescription::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();
  icUInt32Number zero = 0;

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig) ||
      !pIO->Write32(&m_nReserved) ||
      !pIO->Write32(&m_nASCIISize))
    return false;

  if (m_nASCIISize) {
    if (pIO->Write8(m_szText, m_nASCIISize) != m_nASCIISize)
      return false;
  }

  if (!pIO->Write32(&m_nUnicodeLanguageCode))
    return false;

  if (m_nUnicodeSize > 1) {
    if (!pIO->Write32(&m_nUnicodeSize) ||
        pIO->Write16(m_uzUnicodeText, m_nUnicodeSize) != m_nUnicodeSize)
      return false;
  }
  else {
    if (!pIO->Write32(&zero))
      return false;
  }

  if (!pIO->Write16(&m_nScriptCode) ||
      !pIO->Write8(&m_nScriptSize) ||
      pIO->Write8(m_szScriptText, 67)!= 67)
    return false;

  return true;
}

/**
 ****************************************************************************
 * Name: CIccTagTextDescription::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagTextDescription::Describe(std::string &sDescription)
{
  sDescription += "\"";
  if (m_szText && *m_szText)
    sDescription += m_szText;

  sDescription += "\"\n";
}


/**
 ****************************************************************************
 * Name: CIccTagTextDescription::SetText
 * 
 * Purpose: Allows text data associated with the tag to be set.
 * 
 * Args: 
 *  szText - zero terminated string to put in tag
 *****************************************************************************
 */
void CIccTagTextDescription::SetText(const icChar *szText)
{
  if (!szText) 
    SetText("");

  icUInt32Number len=strlen(szText) + 1;
  icChar *szBuf = GetBuffer(len);

  strcpy(szBuf, szText);
  Release();
}

/**
 ****************************************************************************
 * Name: CIccTagTextDescription::operator=
 * 
 * Purpose: Define assignment operator to associate text with tag.
 * 
 * Args: 
 *  szText - zero terminated string to put in the tag
 *
 * Return: A pointer to the string assigned to the tag.
 *****************************************************************************
 */
const icChar *CIccTagTextDescription::operator=(const icChar *szText)
{
  SetText(szText);
  return m_szText;
}

/**
 ****************************************************************************
 * Name: CIccTagTextDescription::GetBuffer
 * 
 * Purpose: This function allocates room and returns pointer to data buffer to
 *  put string into
 * 
 * Args: 
 *  nSize = Requested size of data buffer.
 * 
 * Return: 
 *****************************************************************************
 */
icChar *CIccTagTextDescription::GetBuffer(icUInt32Number nSize)
{
  if (m_nASCIISize < nSize) {
    m_szText = (icChar*)realloc(m_szText, nSize+1);

    m_szText[nSize] = '\0';

    m_nASCIISize = nSize;
  }

  return m_szText;
}

/**
 ****************************************************************************
 * Name: CIccTagTextDescription::Release
 * 
 * Purpose: This will resize the buffer to fit the zero terminated string in
 *  the buffer.
 *****************************************************************************
 */
void CIccTagTextDescription::Release()
{
  icUInt32Number nSize = strlen(m_szText);

  if (nSize < m_nASCIISize-1) {
    m_szText=(icChar*)realloc(m_szText, nSize+1);
    m_nASCIISize = nSize+1;
  }
}

/**
 ****************************************************************************
 * Name: CIccTagTextDescription::GetUnicodeBuffer
 * 
 * Purpose: This function allocates room and returns pointer to data buffer to
 *  put string into
 * 
 * Args: 
 *  nSize = Requested size of data buffer.
 * 
 * Return: 
 *****************************************************************************
 */
icUInt16Number *CIccTagTextDescription::GetUnicodeBuffer(icUInt32Number nSize)
{
  if (m_nUnicodeSize < nSize) {
    m_uzUnicodeText = (icUInt16Number*)realloc(m_uzUnicodeText, (nSize+1)*sizeof(icUInt16Number));

    m_uzUnicodeText[nSize] = 0;

    m_nUnicodeSize = nSize;
  }

  return m_uzUnicodeText;
}

/**
 ****************************************************************************
 * Name: CIccTagTextDescription::ReleaseUnicode
 * 
 * Purpose: This will resize the buffer to fit the zero terminated string in
 *  the buffer.
 *****************************************************************************
 */
void CIccTagTextDescription::ReleaseUnicode()
{
  int i;
  for (i=0; m_uzUnicodeText[i]; i++);

  icUInt32Number nSize = i+1;

  if (nSize < m_nUnicodeSize-1) {
    m_uzUnicodeText=(icUInt16Number*)realloc(m_uzUnicodeText, (nSize+1)*sizeof(icUInt16Number));
    m_nUnicodeSize = nSize+1;
  }
}


/**
******************************************************************************
* Name: CIccTagTextDescription::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagTextDescription::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (m_nScriptSize>67) {
    sReport += icValidateNonCompliantMsg;
    sReport += sSigName;
    sReport += " - ScriptCode count must not be greater than 67.\n";

    rv =icMaxStatus(rv, icValidateNonCompliant);
  }

  return rv;
}

/**
 ****************************************************************************
 * Name: CIccTagSignature::CIccTagSignature
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagSignature::CIccTagSignature()
{
  m_nSig = 0x3f3f3f3f; //'????';
}



/**
 ****************************************************************************
 * Name: CIccTagSignature::CIccTagSignature
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITS = The CIccTagSignature object to be copied
 *****************************************************************************
 */
CIccTagSignature::CIccTagSignature(const CIccTagSignature &ITS)
{
  m_nSig = ITS.m_nSig;
}



/**
 ****************************************************************************
 * Name: CIccTagSignature::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  SignatureTag = The CIccTagSignature object to be copied
 *****************************************************************************
 */
CIccTagSignature &CIccTagSignature::operator=(const CIccTagSignature &SignatureTag)
{
  if (&SignatureTag == this)
    return *this;

  m_nSig = SignatureTag.m_nSig;

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagSignature::~CIccTagSignature
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagSignature::~CIccTagSignature()
{
}

/**
 ****************************************************************************
 * Name: CIccTagSignature::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagSignature::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  if (sizeof(icTagTypeSignature) + 2*sizeof(icUInt32Number) > size)
    return false;

  if (!pIO) {
    m_nSig = 0x3f3f3f3f; //'????';
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read32(&m_nSig))
    return false;

  return true;
}

/**
 ****************************************************************************
 * Name: CIccTagSignature::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagSignature::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write32(&m_nSig))
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagSignature::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagSignature::Describe(std::string &sDescription)
{
  CIccInfo Fmt;

  sDescription += Fmt.GetSigName(m_nSig);
  sDescription += "\n";
}


/**
******************************************************************************
* Name: CIccTagSignature::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagSignature::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);
  char buf[128];

  if (sig==icSigTechnologyTag) {
    switch(m_nSig) {
    case icSigFilmScanner:
    case icSigDigitalCamera:
    case icSigReflectiveScanner:
    case icSigInkJetPrinter:
    case icSigThermalWaxPrinter:
    case icSigElectrophotographicPrinter:
    case icSigElectrostaticPrinter:
    case icSigDyeSublimationPrinter:
    case icSigPhotographicPaperPrinter:
    case icSigFilmWriter:
    case icSigVideoMonitor:
    case icSigVideoCamera:
    case icSigProjectionTelevision:
    case icSigCRTDisplay:
    case icSigPMDisplay:
    case icSigAMDisplay:
    case icSigPhotoCD:
    case icSigPhotoImageSetter:
    case icSigGravure:
    case icSigOffsetLithography:
    case icSigSilkscreen:
    case icSigFlexography:
    case icSigMotionPictureFilmScanner:
    case icSigMotionPictureFilmRecorder:
    case icSigDigitalMotionPictureCamera:
    case icSigDigitalCinemaProjector:
      break;

    default:
      {
        sReport += icValidateNonCompliantMsg;
        sReport += sSigName;
        sprintf(buf, " - %s: Unknown Technology.\n", Info.GetSigName(m_nSig));
        sReport += buf;
        rv = icMaxStatus(rv, icValidateNonCompliant);
      }
    }
  }
  else if (sig==icSigPerceptualRenderingIntentGamutTag ||
           sig==icSigSaturationRenderingIntentGamutTag) {
    switch(m_nSig) {
    case icSigPerceptualReferenceMediumGamut:
      break;

    default:
      {
        sReport += icValidateNonCompliantMsg;
        sReport += sSigName;
        sprintf(buf, " - %s: Unknown Reference Medium Gamut.\n", Info.GetSigName(m_nSig));
        sReport += buf;
        rv = icMaxStatus(rv, icValidateNonCompliant);
      }
    }
  }
  else if (sig==icSigColorimetricIntentImageStateTag) {
    switch(m_nSig) {
      case icSigSceneColorimetryEstimates:
      case icSigSceneAppearanceEstimates:
      case icSigFocalPlaneColorimetryEstimates:
      case icSigReflectionHardcopyOriginalColorimetry:
      case icSigReflectionPrintOutputColorimetry:
        break;

      default:
      {
        sReport += icValidateNonCompliantMsg;
        sReport += sSigName;
        sprintf(buf, " - %s: Unknown Colorimetric Intent Image State.\n", Info.GetSigName(m_nSig));
        sReport += buf;
        rv = icMaxStatus(rv, icValidateNonCompliant);
      }
    }
  }


  return rv;
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::CIccTagNamedColor2
 * 
 * Purpose: Constructor
 * 
 * Args:
 *  nSize = number of named color entries,
 *  nDeviceCoords = number of device channels
 *****************************************************************************
 */
CIccTagNamedColor2::CIccTagNamedColor2(int nSize/*=1*/, int nDeviceCoords/*=0*/)
{
  m_nSize = nSize;
  m_nDeviceCoords = nDeviceCoords;
  if (m_nSize <1)
    m_nSize = 1;
  if (m_nDeviceCoords<0)
    m_nDeviceCoords = nDeviceCoords = 0;

  if (nDeviceCoords>0)
    nDeviceCoords--;

  m_szPrefix[0] = '\0';
  m_szSufix[0] = '\0';
  m_csPCS = icSigUnknownData;
  m_csDevice = icSigUnknownData;

  m_nColorEntrySize = 32/*rootName*/ + (3/*PCS*/ + 1/*iAny*/ + nDeviceCoords)*sizeof(icFloatNumber);

  m_NamedColor = (SIccNamedColorEntry*)calloc(nSize, m_nColorEntrySize);

  m_NamedLab = NULL;
}


/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::CIccTagNamedColor2
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITNC = The CIccTagNamedColor2 object to be copied
 *****************************************************************************
 */
CIccTagNamedColor2::CIccTagNamedColor2(const CIccTagNamedColor2 &ITNC)
{
  m_nColorEntrySize = ITNC.m_nColorEntrySize;
  m_nVendorFlags = ITNC.m_nVendorFlags;
  m_nDeviceCoords = ITNC.m_nDeviceCoords;
  m_nSize = ITNC.m_nSize;

  m_csPCS = ITNC.m_csPCS;
  m_csDevice = ITNC.m_csDevice;

  memcpy(m_szPrefix, ITNC.m_szPrefix, sizeof(m_szPrefix));
  memcpy(m_szSufix, ITNC.m_szSufix, sizeof(m_szSufix));

  m_NamedColor = (SIccNamedColorEntry*)calloc(m_nSize, m_nColorEntrySize);
  memcpy(m_NamedColor, ITNC.m_NamedColor, sizeof(m_NamedColor));

  m_NamedLab = NULL;
}


/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  NamedColor2Tag = The CIccTagNamedColor2 object to be copied
 *****************************************************************************
 */
CIccTagNamedColor2 &CIccTagNamedColor2::operator=(const CIccTagNamedColor2 &NamedColor2Tag)
{
  if (&NamedColor2Tag == this)
    return *this;

  m_nColorEntrySize = NamedColor2Tag.m_nColorEntrySize;
  m_nVendorFlags = NamedColor2Tag.m_nVendorFlags;
  m_nDeviceCoords = NamedColor2Tag.m_nDeviceCoords;
  m_nSize = NamedColor2Tag.m_nSize;

  m_csPCS = NamedColor2Tag.m_csPCS;
  m_csDevice = NamedColor2Tag.m_csDevice;

  memcpy(m_szPrefix, NamedColor2Tag.m_szPrefix, sizeof(m_szPrefix));
  memcpy(m_szSufix, NamedColor2Tag.m_szSufix, sizeof(m_szSufix));

  if (m_NamedColor)
    free(m_NamedColor);
  m_NamedColor = (SIccNamedColorEntry*)calloc(m_nSize, m_nColorEntrySize);
  memcpy(m_NamedColor, NamedColor2Tag.m_NamedColor, sizeof(m_NamedColor));

  m_NamedLab = NULL;

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::~CIccTagNamedColor2
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagNamedColor2::~CIccTagNamedColor2()
{
  if (m_NamedColor)
    free(m_NamedColor);

  if (m_NamedLab)
    delete [] m_NamedLab;
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::SetSize
 * 
 * Purpose: Sets the size of the named color array.
 * 
 * Args: 
 *  nSize - number of named color entries,
 *  nDeviceCoords - number of device channels
 *****************************************************************************
 */
void CIccTagNamedColor2::SetSize(icUInt32Number nSize, icInt32Number nDeviceCoords/*=-1*/)
{
  if (nSize <1)
    nSize = 1;
  if (nDeviceCoords<0)
    nDeviceCoords = m_nDeviceCoords;

  icInt32Number nNewCoords=nDeviceCoords;

  if (nDeviceCoords>0)
    nDeviceCoords--;

  icUInt32Number nColorEntrySize = 32/*rootName*/ + (3/*PCS*/ + 1/*iAny*/ + nDeviceCoords)*sizeof(icFloatNumber);

  SIccNamedColorEntry* pNamedColor = (SIccNamedColorEntry*)calloc(nSize, nColorEntrySize);

  icUInt32Number i, nCopy = __min(nSize, m_nSize);
  icUInt32Number j, nCoords = __min(nNewCoords, (icInt32Number)m_nDeviceCoords);

  for (i=0; i<nCopy; i++) {
    SIccNamedColorEntry *pFrom = (SIccNamedColorEntry*)((icChar*)m_NamedColor + i*m_nColorEntrySize);
    SIccNamedColorEntry *pTo = (SIccNamedColorEntry*)((icChar*)pNamedColor + i*nColorEntrySize);

    strcpy(pTo->rootName, pFrom->rootName);
    for (j=0; j<3; j++)
      pTo->pcsCoords[j] = pFrom->pcsCoords[j];

    for (j=0; j<nCoords; j++) {
      pTo->deviceCoords[j] = pFrom->pcsCoords[j];
    }
  }
  free(m_NamedColor);

  m_NamedColor = pNamedColor;
  m_nSize = nSize;
  m_nDeviceCoords = nNewCoords;

  ResetPCSCache();
}


/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagNamedColor2::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt32Number nNum, nCoords;

  icUInt32Number nTagHdrSize = sizeof(icTagTypeSignature) + 
                               sizeof(icUInt32Number) + //m_nReserved=0
                               sizeof(icUInt32Number) + //VendorFlags
                               sizeof(icUInt32Number) + //Num Colors
                               sizeof(icUInt32Number) + //Num Device Coords
                               sizeof(m_szPrefix) + 
                               sizeof(m_szSufix); 
  if (nTagHdrSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig) ||
      !pIO->Read32(&m_nReserved) ||
      !pIO->Read32(&m_nVendorFlags) ||
      !pIO->Read32(&nNum) ||
      !pIO->Read32(&nCoords) ||
      pIO->Read8(m_szPrefix, sizeof(m_szPrefix))!=sizeof(m_szPrefix) ||
      pIO->Read8(m_szSufix, sizeof(m_szSufix))!=sizeof(m_szSufix)) {
    return false;
  }

  size -= nTagHdrSize;

  icUInt32Number nCount = size / (32+(3+nCoords)*sizeof(icUInt16Number));

  if (nCount < nNum)
    return false;

  SetSize(nNum, nCoords);

  icUInt32Number i;
  SIccNamedColorEntry *pNamedColor=m_NamedColor;

  for (i=0; i<nNum; i++) {
    if (pIO->Read8(&pNamedColor->rootName, sizeof(pNamedColor->rootName))!=sizeof(pNamedColor->rootName) ||
        pIO->Read16Float(&pNamedColor->pcsCoords, 3)!=3)
      return false;
    if (nCoords) {
      if (pIO->Read16Float(&pNamedColor->deviceCoords, nCoords)!=nCoords)
        return false;
    }
    pNamedColor = (SIccNamedColorEntry*)((icChar*)pNamedColor + m_nColorEntrySize);
  }

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagNamedColor2::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write32(&m_nVendorFlags))
    return false;

  if (!pIO->Write32(&m_nSize))
    return false;

  if (!pIO->Write32(&m_nDeviceCoords))
    return false;

  if (!pIO->Write8(m_szPrefix, sizeof(m_szPrefix)))
    return false;

  if (!pIO->Write8(m_szSufix, sizeof(m_szSufix)))
    return false;

  icUInt32Number i;
  SIccNamedColorEntry *pNamedColor=m_NamedColor;

  for (i=0; i<m_nSize; i++) {
    if (pIO->Write8(&pNamedColor->rootName, sizeof(pNamedColor->rootName))!=sizeof(pNamedColor->rootName) ||
        pIO->Write16Float(&pNamedColor->pcsCoords, 3)!=3)
      return false;
    if (m_nDeviceCoords) {
      if (pIO->Write16Float(&pNamedColor->deviceCoords, m_nDeviceCoords)!=m_nDeviceCoords)
        return false;
    }
    pNamedColor = (SIccNamedColorEntry*)((icChar*)pNamedColor + m_nColorEntrySize);
  }

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagNamedColor2::Describe(std::string &sDescription)
{
  icChar buf[128], szColorVal[40], szColor[40];

  icUInt32Number i, j;
  SIccNamedColorEntry *pNamedColor=m_NamedColor;

  sDescription.reserve(sDescription.size() + m_nSize*79);

  sprintf(buf, "BEGIN_NAMED_COLORS flags=%08x %u %u\n", m_nVendorFlags, m_nSize, m_nDeviceCoords);
  sDescription += buf;

  for (i=0; i<m_nSize; i++) {
    sprintf(buf, "Color[%u]: %s :", i, pNamedColor->rootName);
    sDescription += buf;
    
    icFloatNumber pcsCoord[3];
    for (j=0; j<3; j++)
      pcsCoord[j] = pNamedColor->pcsCoords[j];

    if (m_csPCS==icSigLabData) {
      for (j=0; j<3; j++)
        pcsCoord[j] = (icFloatNumber)(pcsCoord[j] * 65535.0 / 65280.0);
    }

    for (j=0; j<3; j++) {
      icColorIndexName(szColor, m_csPCS, j, 3, "P");
      icColorValue(szColorVal, pcsCoord[j], m_csPCS, j);
      sprintf(buf, " %s=%s", szColor, szColorVal);
      sDescription += buf;
    }
    if (m_nDeviceCoords) {
      sDescription += " :";
      for (j=0; j<m_nDeviceCoords; j++) {
        icColorIndexName(szColor, m_csDevice, j, m_nDeviceCoords, "D");
        icColorValue(szColorVal, pNamedColor->deviceCoords[j], m_csDevice, j);
        sprintf(buf, " %s=%s", szColor, szColorVal);
        sDescription += buf;
      }
    }
    sDescription += "\n";

    pNamedColor = (SIccNamedColorEntry*)((icChar*)pNamedColor + m_nColorEntrySize);
  }
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::SetColorSpaces
 * 
 * Purpose: Set the device and PCS color space of the tag
 * 
 * Args: 
 *  csPCS = PCS color space signature,
 *  csDevice = Device color space signature
 * 
 *****************************************************************************
 */
void CIccTagNamedColor2::SetColorSpaces(icColorSpaceSignature csPCS, icColorSpaceSignature csDevice)
{
   m_csPCS = csPCS;
   m_csDevice = csDevice;
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::FindRootColor
 * 
 * Purpose: Find the root color name
 * 
 * Args: 
 *  szRootColor = string containing the root color name to be found
 * 
 * Return: Index of the named color array where the root color name was found,
 *  if the color was not found -1 is returned
 *****************************************************************************
 */
icInt32Number CIccTagNamedColor2::FindRootColor(const icChar *szRootColor)
{
  for (icUInt32Number i=0; i<m_nSize; i++) {
    if (stricmp(m_NamedColor[i].rootName,szRootColor) == 0)
      return i;
  }

  return -1;
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::ResetPCSCache
 * 
 * Purpose: This function is called if entry values change between calls
 *  to FindPCSColor()
 * 
 *****************************************************************************
 */
void CIccTagNamedColor2::ResetPCSCache()
{
  if (m_NamedLab) {
    delete [] m_NamedLab;
    m_NamedLab = NULL;
  }
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::FindPCSColor
 * 
 * Purpose: Find the PCS color within the specified deltaE
 * 
 * Args: 
 *  pPCS = PCS co-ordinates,
 *  dMinDE = the minimum deltaE (tolerance)
 * 
 * Return: Index of the named color array where the PCS color was found,
 *  if the color was not found within the tolerance -1 is returned
 *****************************************************************************
 */
icInt32Number CIccTagNamedColor2::FindPCSColor(icFloatNumber *pPCS, icFloatNumber dMinDE/*=1000.0*/)
{
  icFloatNumber dCalcDE, dLeastDE;
  icFloatNumber pLabIn[3];
  icFloatNumber *pXYZ, *pLab;
  icInt32Number leastDEindex = -1;
  if (m_csPCS != icSigLabData) {
    pXYZ = pPCS;
    icXyzFromPcs(pXYZ);
    icXYZtoLab(pLabIn,pXYZ);
  }
  else {
    Lab2ToLab4(pLabIn, pPCS);
    icLabFromPcs(pLabIn);
  }

  if (!m_NamedLab) {
    m_NamedLab = new SIccNamedLabEntry[m_nSize];
    if (!m_NamedLab)
      return -1;
    
    if (m_csPCS != icSigLabData) {
      for (icUInt32Number i=0; i<m_nSize; i++) {
        pLab = m_NamedLab[i].lab;
        pXYZ = m_NamedColor[i].pcsCoords;
        icXyzFromPcs(pXYZ);
        icXYZtoLab(pLab, pXYZ);
      }
    }
    else {
      for (icUInt32Number i=0; i<m_nSize; i++) {
        pLab = m_NamedLab[i].lab;
        Lab2ToLab4(pLab, m_NamedColor[i].pcsCoords);
        icLabFromPcs(pLab);
      }
    }
  }

  for (icUInt32Number i=0; i<m_nSize; i++) {
    pLab = m_NamedLab[i].lab;

    dCalcDE = sqrt((pLabIn[0]-pLab[0])*(pLabIn[0]-pLab[0]) +
                   (pLabIn[1]-pLab[1])*(pLabIn[1]-pLab[1]) +
                   (pLabIn[2]-pLab[2])*(pLabIn[2]-pLab[2]));

    if (i==0) {
      dLeastDE = dCalcDE;
      leastDEindex = i;
    }

    if (dCalcDE<dMinDE) {
      if (dCalcDE<dLeastDE) {
        dLeastDE = dCalcDE;
        leastDEindex = i;
      }      
    }
  }

  return leastDEindex;
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::FindColor
 * 
 * Purpose: Find the color with given name
 * 
 * Args: 
 *  szColor = the color name
 * 
 * Return: Index of the named color array where the color name was found,
 *  if the color was not found -1 is returned
 *****************************************************************************
 */
icInt32Number CIccTagNamedColor2::FindColor(const icChar *szColor)
{
  std::string sColorName;
  icInt32Number i, j;

  j = strlen(m_szPrefix);
  if (j != 0) {  
    if (strncmp(szColor, m_szPrefix, j))
      return -1;
  }

  j = strlen(m_szSufix);
  i = strlen(szColor);
  if (j != 0) {
    if (strncmp(szColor+(i-j), m_szSufix, j))
      return -1;    
  }


  for ( i=0; i<(icInt32Number)m_nSize; i++) {
    sColorName = m_szPrefix;
    sColorName += m_NamedColor[i].rootName;
    sColorName += m_szSufix;

    if (strcmp(sColorName.c_str(),szColor) == 0)
      return i;
  }

  return -1;
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::FindColor
 * 
 * Purpose: Find the device color
 * 
 * Args: 
 *  pDevColor = device color co-ordinates
 * 
 * Return: Index of the named color array where the closest device color
 *  was found, if device representation is absent -1 is returned.
 *****************************************************************************
 */
icInt32Number CIccTagNamedColor2::FindDeviceColor(icFloatNumber *pDevColor)
{
  if (!m_nDeviceCoords)
    return -1;
  
  icFloatNumber dCalcDiff=0.0, dLeastDiff;
  icFloatNumber *pDevOut;
  icInt32Number leastDiffindex = -1;


  for (icUInt32Number i=0; i<m_nSize; i++) {
    pDevOut = m_NamedColor[i].deviceCoords;

    for (icUInt32Number j=0; j<m_nDeviceCoords; j++) {
      dCalcDiff += (pDevColor[j]-pDevOut[j])*(pDevColor[j]-pDevOut[j]);
    }
    dCalcDiff = sqrt(dCalcDiff);

    if (i==0) {
      dLeastDiff = dCalcDiff;
      leastDiffindex = i;
    }

    if (dCalcDiff<dLeastDiff) {
      dLeastDiff = dCalcDiff;
      leastDiffindex = i;
    }      

    dCalcDiff = 0.0;
  }

  return leastDiffindex;
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::GetColorName
 * 
 * Purpose: Extracts the color name from the named color array
 * 
 * Args: 
 *  sColorName = string where color name will be stored,
 *  index = array index of the color name
 * 
 * Return: 
 *  true = if the index is within range,
 *  false = index out of range
 *****************************************************************************
 */
bool CIccTagNamedColor2::GetColorName(std::string &sColorName, icInt32Number index)
{
  if (index > (icInt32Number)m_nSize-1)
    return false;

  sColorName += m_szPrefix;
  sColorName += m_NamedColor[index].rootName;
  sColorName += m_szSufix;

  return true;
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::UnitClip
 * 
 * Purpose: Clip number so that its between 0-1
 * 
 * Args: 
 *  v = number to be clipped
 * 
 * Return: Clipped number
 *  
 *****************************************************************************
 */
icFloatNumber CIccTagNamedColor2::UnitClip(icFloatNumber v)
{
  if (v<0)
    v = 0;
  if (v>1.0)
    v = 1.0;

  return v;
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::NegClip
 * 
 * Purpose: Negative numbers are clipped to zero
 * 
 * Args: 
 *  v = number to be clipped
 * 
 * Return: Clipped number
 *  
 *****************************************************************************
 */
icFloatNumber CIccTagNamedColor2::NegClip(icFloatNumber v)
{
  if (v<0)
    v=0;
  
  return v;
}


/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::Lab2ToLab4
 * 
 * Purpose: Convert version 2 Lab to version 4 Lab
 * 
 * Args: 
 *  Dst = array to store version 4 Lab coordinates,
 *  Src = array containing version 2 Lab coordinates
 * 
 *****************************************************************************
 */
void CIccTagNamedColor2::Lab2ToLab4(icFloatNumber *Dst, const icFloatNumber *Src)
{
  Dst[0] = UnitClip((icFloatNumber)(Src[0] * 65535.0 / 65280.0));
  Dst[1] = UnitClip((icFloatNumber)(Src[1] * 65535.0 / 65280.0));
  Dst[2] = UnitClip((icFloatNumber)(Src[2] * 65535.0 / 65280.0));
}

/**
 ****************************************************************************
 * Name: CIccTagNamedColor2::Lab4ToLab2
 * 
 * Purpose: Convert version 4 Lab to version 2 Lab
 * 
 * Args: 
 *  Dst = array to store version 2 Lab coordinates,
 *  Src = array containing version 4 Lab coordinates
 * 
 *****************************************************************************
 */
void CIccTagNamedColor2::Lab4ToLab2(icFloatNumber *Dst, const icFloatNumber *Src)
{
  Dst[0] = (icFloatNumber)(Src[0] * 65280.0 / 65535.0);
  Dst[1] = (icFloatNumber)(Src[1] * 65280.0 / 65535.0);
  Dst[2] = (icFloatNumber)(Src[2] * 65280.0 / 65535.0);
}


/**
******************************************************************************
* Name: CIccTagNamedColor2::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagNamedColor2::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (!m_nSize) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " - Empty tag!\n";
    rv = icMaxStatus(rv, icValidateWarning);
  }

  if (m_nDeviceCoords) {
    if (pProfile) {
      icUInt32Number nCoords = icGetSpaceSamples(pProfile->m_Header.colorSpace);
      if (m_nDeviceCoords != nCoords) {
        sReport += icValidateNonCompliantMsg;
        sReport += sSigName;
        sReport += " - Incorrect number of device co-ordinates.\n";
        rv = icMaxStatus(rv, icValidateNonCompliant);
      }
    }
    else {
      sReport += icValidateWarningMsg;
      sReport += sSigName;
      sReport += " - Tag validation incomplete: Pointer to profile unavailable.\n";
      rv = icMaxStatus(rv, icValidateWarning);
    }
  }

  return rv;
}


/**
 ****************************************************************************
 * Name: CIccTagXYZ::CIccTagXYZ
 * 
 * Purpose: Constructor
 *
 * Args:
 *  nSize = number of XYZ entries
 * 
 *****************************************************************************
 */
CIccTagXYZ::CIccTagXYZ(int nSize/*=1*/)
{
  m_nSize = nSize;
  if (m_nSize <1)
    m_nSize = 1;
  m_XYZ = (icXYZNumber*)calloc(nSize, sizeof(icXYZNumber));
}


/**
 ****************************************************************************
 * Name: CIccTagXYZ::CIccTagXYZ
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITXYZ = The CIccTagXYZ object to be copied
 *****************************************************************************
 */
CIccTagXYZ::CIccTagXYZ(const CIccTagXYZ &ITXYZ)
{
  m_nSize = ITXYZ.m_nSize;

  m_XYZ = (icXYZNumber*)calloc(m_nSize, sizeof(icXYZNumber));
  memcpy(m_XYZ, ITXYZ.m_XYZ, sizeof(icXYZNumber)*m_nSize);
}



/**
 ****************************************************************************
 * Name: CIccTagXYZ::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  XYZTag = The CIccTagXYZ object to be copied
 *****************************************************************************
 */
CIccTagXYZ &CIccTagXYZ::operator=(const CIccTagXYZ &XYZTag)
{
  if (&XYZTag == this)
    return *this;

  m_nSize = XYZTag.m_nSize;

  if (m_XYZ)
    free(m_XYZ);
  m_XYZ = (icXYZNumber*)calloc(m_nSize, sizeof(icXYZNumber));
  memcpy(m_XYZ, XYZTag.m_XYZ, sizeof(icXYZNumber)*m_nSize);

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagXYZ::~CIccTagXYZ
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagXYZ::~CIccTagXYZ()
{
  if (m_XYZ)
    free(m_XYZ);
}


/**
 ****************************************************************************
 * Name: CIccTagXYZ::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagXYZ::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number) + 
      sizeof(icXYZNumber) > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  icUInt32Number nNum=((size-2*sizeof(icUInt32Number)) / sizeof(icXYZNumber));
  icUInt32Number nNum32 = nNum*sizeof(icXYZNumber)/sizeof(icUInt32Number);

  SetSize(nNum);

  if (pIO->Read32(m_XYZ, nNum32)!=nNum32 )
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagXYZ::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagXYZ::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  icUInt32Number nNum32 = m_nSize * sizeof(icXYZNumber)/sizeof(icUInt32Number);

  if (
    pIO->Write32(m_XYZ, nNum32)!=nNum32)
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagXYZ::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagXYZ::Describe(std::string &sDescription)
{
  icChar buf[128];

  if (m_nSize == 1 ) {
    sprintf(buf, "X=%.4lf, Y=%.4lf, Z=%.4lf\n", icFtoD(m_XYZ[0].X), icFtoD(m_XYZ[0].Y), icFtoD(m_XYZ[0].Z));
    sDescription += buf;
  }
  else {
    icUInt32Number i;
    sDescription.reserve(sDescription.size() + m_nSize*79);

    for (i=0; i<m_nSize; i++) {
      sprintf(buf, "value[%u]: X=%.4lf, Y=%.4lf, Z=%.4lf\n", i, icFtoD(m_XYZ[i].X), icFtoD(m_XYZ[i].Y), icFtoD(m_XYZ[i].Z));
      sDescription += buf;
    }
  }
}

/**
 ****************************************************************************
 * Name: CIccTagXYZ::SetSize
 * 
 * Purpose: Sets the size of the XYZ array.
 * 
 * Args: 
 *  nSize - number of XYZ entries,
 *  bZeroNew - flag to zero newly formed values
 *****************************************************************************
 */
void CIccTagXYZ::SetSize(icUInt32Number nSize, bool bZeroNew/*=true*/)
{
  if (nSize==m_nSize)
    return;

  m_XYZ = (icXYZNumber*)realloc(m_XYZ, nSize*sizeof(icXYZNumber));
  if (bZeroNew && m_nSize < nSize) {
    memset(&m_XYZ[m_nSize], 0, (nSize-m_nSize)*sizeof(icXYZNumber));
  }
  m_nSize = nSize;
}


/**
******************************************************************************
* Name: CIccTagXYZ::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagXYZ::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (!m_nSize) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " - Empty tag.\n";

    rv = icMaxStatus(rv, icValidateWarning);
    return rv;
  }

  for (int i=0; i<(int)m_nSize; i++) {
    rv = icMaxStatus(rv, Info.CheckData(sReport, m_XYZ[i]));
  }

  return rv;
}


/**
 ****************************************************************************
 * Name: CIccTagChromaticity::CIccTagChromaticity
 * 
 * Purpose: Constructor
 *
 * Args:
 *  nSize = number of xy entries
 * 
 *****************************************************************************
 */
CIccTagChromaticity::CIccTagChromaticity(int nSize/*=3*/)
{
  m_nChannels = nSize;
  if (m_nChannels <3)
    m_nChannels = 3;
  m_xy = (icChromaticityNumber*)calloc(nSize, sizeof(icChromaticityNumber));
}


/**
 ****************************************************************************
 * Name: CIccTagChromaticity::CIccTagChromaticity
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITCh = The CIccTagChromaticity object to be copied
 *****************************************************************************
 */
CIccTagChromaticity::CIccTagChromaticity(const CIccTagChromaticity &ITCh)
{
  m_nChannels = ITCh.m_nChannels;

  m_xy = (icChromaticityNumber*)calloc(m_nChannels, sizeof(icChromaticityNumber));
  memcpy(m_xy, ITCh.m_xy, sizeof(m_xy));
}


/**
 ****************************************************************************
 * Name: CIccTagChromaticity::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ChromTag = The CIccTagChromaticity object to be copied
 *****************************************************************************
 */
CIccTagChromaticity &CIccTagChromaticity::operator=(const CIccTagChromaticity &ChromTag)
{
  if (&ChromTag == this)
    return *this;

  m_nChannels = ChromTag.m_nChannels;

  if (m_xy)
    free(m_xy);
  m_xy = (icChromaticityNumber*)calloc(m_nChannels, sizeof(icChromaticityNumber));
  memcpy(m_xy, ChromTag.m_xy, sizeof(m_xy));

  return *this;  
}


/**
 ****************************************************************************
 * Name: CIccTagChromaticity::~CIccTagChromaticity
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagChromaticity::~CIccTagChromaticity()
{
  if (m_xy)
    free(m_xy);
}


/**
 ****************************************************************************
 * Name: CIccTagChromaticity::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagChromaticity::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt16Number nChannels;

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number) + 
      sizeof(icUInt32Number) +
      sizeof(icChromaticityNumber) > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&nChannels) ||
      !pIO->Read16(&m_nColorantType))
    return false;

  icUInt32Number nNum = (size-3*sizeof(icUInt32Number)) / sizeof(icChromaticityNumber);
  icUInt32Number nNum32 = nNum*sizeof(icChromaticityNumber)/sizeof(icU16Fixed16Number);

  if (nNum < nChannels)
    return false;

  SetSize((icUInt16Number)nNum);

  if (pIO->Read32(&m_xy[0], nNum32)!=nNum32 )
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagChromaticity::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagChromaticity::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write16(&m_nChannels))
    return false;

  if (!pIO->Write16(&m_nColorantType))
    return false;

  icUInt32Number nNum32 = m_nChannels*sizeof(icChromaticityNumber)/sizeof(icU16Fixed16Number);

  if (pIO->Write32(&m_xy[0], nNum32)!=nNum32)
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagChromaticity::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagChromaticity::Describe(std::string &sDescription)
{
  icChar buf[128];
  CIccInfo Fmt;

  icUInt32Number i;
  //sDescription.reserve(sDescription.size() + m_nChannels*79);
  sprintf(buf, "Number of Channels : %u\n", m_nChannels);
  sDescription += buf;

  sprintf(buf, "Colorant Encoding : %s\n", Fmt.GetColorantEncoding((icColorantEncoding)m_nColorantType));
  sDescription += buf;

  for (i=0; i<m_nChannels; i++) {
    sprintf(buf, "value[%u]: x=%.3lf, y=%.3lf\n", i, icUFtoD(m_xy[i].x), icUFtoD(m_xy[i].y));
    sDescription += buf;
  }

}

/**
 ****************************************************************************
 * Name: CIccTagChromaticity::SetSize
 * 
 * Purpose: Sets the size of the xy chromaticity array.
 * 
 * Args: 
 *  nSize - number of xy entries,
 *  bZeroNew - flag to zero newly formed values
 *****************************************************************************
 */
void CIccTagChromaticity::SetSize(icUInt16Number nSize, bool bZeroNew/*=true*/)
{
  if (m_nChannels == nSize)
    return;

  m_xy = (icChromaticityNumber*)realloc(m_xy, nSize*sizeof(icChromaticityNumber));
  if (bZeroNew && nSize > m_nChannels) {
    memset(&m_xy[m_nChannels], 0, (nSize - m_nChannels)*sizeof(icChromaticityNumber));
  }

  m_nChannels = nSize;
}


/**
******************************************************************************
* Name: CIccTagChromaticity::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagChromaticity::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (m_nColorantType) {

    if (m_nChannels!=3) {
      sReport += icValidateCriticalErrorMsg;
      sReport += sSigName;
      sReport += " - Number of device channels must be three.\n";
      rv = icMaxStatus(rv, icValidateCriticalError);
    }

    switch(m_nColorantType) {
      case icColorantITU:
        {
          if ( (m_xy[0].x != icDtoUF((icFloatNumber)0.640)) || (m_xy[0].y != icDtoUF((icFloatNumber)0.330)) ||
               (m_xy[1].x != icDtoUF((icFloatNumber)0.300)) || (m_xy[1].y != icDtoUF((icFloatNumber)0.600)) ||
               (m_xy[2].x != icDtoUF((icFloatNumber)0.150)) || (m_xy[2].y != icDtoUF((icFloatNumber)0.060)) ) {
                sReport += icValidateNonCompliantMsg;
                sReport += sSigName;
                sReport += " - Chromaticity data does not match specification.\n";
                rv = icMaxStatus(rv, icValidateNonCompliant);
              }
          break;
        }

      case icColorantSMPTE:
        {
          if ( (m_xy[0].x != icDtoUF((icFloatNumber)0.630)) || (m_xy[0].y != icDtoUF((icFloatNumber)0.340)) ||
               (m_xy[1].x != icDtoUF((icFloatNumber)0.310)) || (m_xy[1].y != icDtoUF((icFloatNumber)0.595)) ||
               (m_xy[2].x != icDtoUF((icFloatNumber)0.155)) || (m_xy[2].y != icDtoUF((icFloatNumber)0.070)) ) {
              sReport += icValidateNonCompliantMsg;
              sReport += sSigName;
              sReport += " - Chromaticity data does not match specification.\n";
              rv = icMaxStatus(rv, icValidateNonCompliant);
            }
            break;
        }

      case icColorantEBU:
        {
          if ( (m_xy[0].x != icDtoUF((icFloatNumber)0.64)) || (m_xy[0].y != icDtoUF((icFloatNumber)0.33)) ||
               (m_xy[1].x != icDtoUF((icFloatNumber)0.29)) || (m_xy[1].y != icDtoUF((icFloatNumber)0.60)) ||
               (m_xy[2].x != icDtoUF((icFloatNumber)0.15)) || (m_xy[2].y != icDtoUF((icFloatNumber)0.06)) ) {
              sReport += icValidateNonCompliantMsg;
              sReport += sSigName;
              sReport += " - Chromaticity data does not match specification.\n";
              rv = icMaxStatus(rv, icValidateNonCompliant);
            }
            break;
        }

      case icColorantP22:
        {
          if ( (m_xy[0].x != icDtoUF((icFloatNumber)0.625)) || (m_xy[0].y != icDtoUF((icFloatNumber)0.340)) ||
               (m_xy[1].x != icDtoUF((icFloatNumber)0.280)) || (m_xy[1].y != icDtoUF((icFloatNumber)0.605)) ||
               (m_xy[2].x != icDtoUF((icFloatNumber)0.155)) || (m_xy[2].y != icDtoUF((icFloatNumber)0.070)) ) {
              sReport += icValidateNonCompliantMsg;
              sReport += sSigName;
              sReport += " - Chromaticity data does not match specification.\n";
              rv = icMaxStatus(rv, icValidateNonCompliant);
            }
            break;
        }

      default:
        {
          sReport += icValidateNonCompliantMsg;
          sReport += sSigName;
          sReport += " - Invalid colorant type encoding.\n";
          rv = icMaxStatus(rv, icValidateNonCompliant);
        }
    }
  }

  return rv;
}


/**
 ****************************************************************************
 * Name: ::CIccTagFixedNum
 * 
 * Purpose: ConstrCIccTagFixedNumuctor
 * 
 * Args:
 *  nSize = number of data entries
 * 
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
CIccTagFixedNum<T, Tsig>::CIccTagFixedNum(int nSize/*=1*/)
{
  m_nSize = nSize;
  if (m_nSize <1)
    m_nSize = 1;
  m_Num = (T*)calloc(nSize, sizeof(T));
}


/**
 ****************************************************************************
 * Name: CIccTagFixedNum::CIccTagFixedNum
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITFN = The CIccTagFixedNum object to be copied
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
CIccTagFixedNum<T, Tsig>::CIccTagFixedNum(const CIccTagFixedNum<T, Tsig> &ITFN)
{
  m_nSize = ITFN.m_nSize;
  m_Num = (T*)calloc(m_nSize, sizeof(T));
  memcpy(m_Num, ITFN.m_Num, sizeof(m_Num));
}


/**
 ****************************************************************************
 * Name: CIccTagFixedNum::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ITFN = The CIccTagFixedNum object to be copied
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
CIccTagFixedNum<T, Tsig> &CIccTagFixedNum<T, Tsig>::operator=(const CIccTagFixedNum<T, Tsig> &ITFN)
{
  if (&ITFN == this)
    return *this;

  m_nSize = ITFN.m_nSize;

  if (m_Num)
    free(m_Num);
  m_Num = (T*)calloc(m_nSize, sizeof(T));
  memcpy(m_Num, ITFN.m_Num, sizeof(m_Num));

  return *this;
}



/**
 ****************************************************************************
 * Name: CIccTagFixedNum::~CIccTagFixedNum
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
CIccTagFixedNum<T, Tsig>::~CIccTagFixedNum()
{
  if (m_Num)
    free(m_Num);
}

/**
 ****************************************************************************
 * Name: CIccTagFixedNum::GetClassName
 * 
 * Purpose: Returns the tag type class name
 * 
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
const icChar* CIccTagFixedNum<T, Tsig>::GetClassName()
{
  if (Tsig==icSigS15Fixed16ArrayType)
    return "CIccTagS15Fixed16";
  else 
    return "CIccTagU16Fixed16";
}


/**
 ****************************************************************************
 * Name: CIccTagFixedNum::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
bool CIccTagFixedNum<T, Tsig>::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number) + 
      sizeof(T) > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  icUInt32Number nSize=((size-2*sizeof(icUInt32Number)) / sizeof(T));

  SetSize(nSize);

  if (pIO->Read32(m_Num, nSize)!=nSize )
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagFixedNum::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
bool CIccTagFixedNum<T, Tsig>::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (pIO->Write32(m_Num, m_nSize)!=m_nSize)
    return false;
 
  return true;
}

/**
 ****************************************************************************
 * Name: CIccTagFixedNum::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
void CIccTagFixedNum<T, Tsig>::Describe(std::string &sDescription)
{
  icChar buf[128];

  if (m_nSize == 1 ) {
    if (Tsig==icSigS15Fixed16ArrayType) 
      sprintf(buf, "Value = %.4lf\n", icFtoD(m_Num[0]));
    else
      sprintf(buf, "Value = %.4lf\n", icUFtoD(m_Num[0]));
    sDescription += buf;
  }
  else {
    icUInt32Number i;

    if (Tsig==icSigS15Fixed16ArrayType && m_nSize==9) {
      sDescription += "Matrix Form:\n";
      icMatrixDump(sDescription, (icS15Fixed16Number*)m_Num);

      sDescription += "\nArrayForm:\n";
    }
    sDescription.reserve(sDescription.size() + m_nSize*79);

    for (i=0; i<m_nSize; i++) {
      if (Tsig==icSigS15Fixed16ArrayType) 
        sprintf(buf, "Value[%u] = %.4lf\n", i, icFtoD(m_Num[i]));
      else
        sprintf(buf, "Value[%u] = %.4lf\n", i, icUFtoD(m_Num[i]));
      sDescription += buf;
    }
  }
}

/**
 ****************************************************************************
 * Name: CIccTagFixedNum::SetSize
 * 
 * Purpose: Sets the size of the data array.
 * 
 * Args: 
 *  nSize - number of data entries,
 *  bZeroNew - flag to zero newly formed values
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
void CIccTagFixedNum<T, Tsig>::SetSize(icUInt32Number nSize, bool bZeroNew/*=true*/)
{
  if (nSize==m_nSize)
    return;

  m_Num = (T*)realloc(m_Num, nSize*sizeof(T));
  if (bZeroNew && m_nSize < nSize) {
    memset(&m_Num[m_nSize], 0, (nSize-m_nSize)*sizeof(T));
  }
  m_nSize = nSize;
}


//Make sure typedef classes get built
template class CIccTagFixedNum<icS15Fixed16Number, icSigS15Fixed16ArrayType>;
template class CIccTagFixedNum<icU16Fixed16Number, icSigU16Fixed16ArrayType>;


/**
 ****************************************************************************
 * Name: CIccTagNum::CIccTagNum
 * 
 * Purpose: Constructor
 * 
 * Args:
 *  nSize = number of data entries
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
CIccTagNum<T, Tsig>::CIccTagNum(int nSize/*=1*/)
{
  m_nSize = nSize;
  if (m_nSize <1)
    m_nSize = 1;
  m_Num = (T*)calloc(nSize, sizeof(T));
}


/**
 ****************************************************************************
 * Name: CIccTagNum::CIccTagNum
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITNum = The CIccTagNum object to be copied
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
CIccTagNum<T, Tsig>::CIccTagNum(const CIccTagNum<T, Tsig> &ITNum)
{
  m_nSize = ITNum.m_nSize;

  m_Num = (T*)calloc(m_nSize, sizeof(T));
  memcpy(m_Num, ITNum.m_Num, sizeof(m_Num));
}


/**
 ****************************************************************************
 * Name: CIccTagNum::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ITNum = The CIccTagNum object to be copied
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
CIccTagNum<T, Tsig> &CIccTagNum<T, Tsig>::operator=(const CIccTagNum<T, Tsig> &ITNum)
{
  if (&ITNum == this)
    return *this;

  m_nSize = ITNum.m_nSize;

  m_Num = (T*)calloc(m_nSize, sizeof(T));
  memcpy(m_Num, ITNum.m_Num, sizeof(m_Num));

  return *this;
}



/**
 ****************************************************************************
 * Name: CIccTagNum::~CIccTagNum
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
CIccTagNum<T, Tsig>::~CIccTagNum()
{
  if (m_Num)
    free(m_Num);
}

/**
 ****************************************************************************
 * Name: CIccTagNum::GetClassName
 * 
 * Purpose: Returns the tag type class name
 * 
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
const icChar *CIccTagNum<T, Tsig>::GetClassName()
{
  if (sizeof(T)==sizeof(icUInt8Number))
    return "CIccTagUInt8";
  else if (sizeof(T)==sizeof(icUInt16Number))
    return "CIccTagUInt16";
  else if (sizeof(T)==sizeof(icUInt32Number))
    return "CIccTagUInt32";
  else if (sizeof(T)==sizeof(icUInt64Number))
    return "CIccTagUInt64";
  else
    return "CIccTagNum<>";
}


/**
 ****************************************************************************
 * Name: CIccTagNum::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
bool CIccTagNum<T, Tsig>::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number) + 
      sizeof(T) > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  icUInt32Number nSize=((size-2*sizeof(icUInt32Number)) / sizeof(T));

  SetSize(nSize);

  if (sizeof(T)==sizeof(icUInt8Number)) {
    if (pIO->Read8(m_Num, nSize)!=nSize )
      return false;
  }
  else if (sizeof(T)==sizeof(icUInt16Number)) {
    if (pIO->Read16(m_Num, nSize)!=nSize )
      return false;
  }
  else if (sizeof(T)==sizeof(icUInt32Number)) {
    if (pIO->Read32(m_Num, nSize)!=nSize )
      return false;
  }
  else if (sizeof(T)==sizeof(icUInt64Number)) {
    if (pIO->Read64(m_Num, nSize)!=nSize )
      return false;
  }
  else
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagNum::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
bool CIccTagNum<T, Tsig>::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (sizeof(T)==sizeof(icUInt8Number)) {
    if (pIO->Write32(m_Num, m_nSize)!=m_nSize)
      return false;
  }
  else if (sizeof(T)==sizeof(icUInt16Number)) {
    if (pIO->Write32(m_Num, m_nSize)!=m_nSize)
      return false;
  }
  else if (sizeof(T)==sizeof(icUInt32Number)) {
    if (pIO->Write32(m_Num, m_nSize)!=m_nSize)
      return false;
  }
  else if (sizeof(T)==sizeof(icUInt64Number)) {
    if (pIO->Write32(m_Num, m_nSize)!=m_nSize)
      return false;
  }
  else
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagNum::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
void CIccTagNum<T, Tsig>::Describe(std::string &sDescription)
{
  icChar buf[128];

  if (m_nSize == 1 ) {
    sprintf(buf, "Value = %u (0x%x)\n", m_Num[0], m_Num[0]);
    sDescription += buf;
  }
  else {
    icUInt32Number i;
    sDescription.reserve(sDescription.size() + m_nSize*79);

    for (i=0; i<m_nSize; i++) {
      sprintf(buf, "Value[%u] = %u (0x%x)\n", i, m_Num[i], m_Num[i]);
      sDescription += buf;
    }
  }
}

/**
 ****************************************************************************
 * Name: CIccTagNum::SetSize
 * 
 * Purpose: Sets the size of the data array.
 * 
 * Args: 
 *  nSize - number of data entries,
 *  bZeroNew - flag to zero newly formed values
 *****************************************************************************
 */
template <class T, icTagTypeSignature Tsig>
void CIccTagNum<T, Tsig>::SetSize(icUInt32Number nSize, bool bZeroNew/*=true*/)
{
  if (nSize==m_nSize)
    return;

  m_Num = (T*)realloc(m_Num, nSize*sizeof(T));
  if (bZeroNew && m_nSize < nSize) {
    memset(&m_Num[m_nSize], 0, (nSize-m_nSize)*sizeof(T));
  }
  m_nSize = nSize;
}

//Make sure typedef classes get built
template class CIccTagNum<icUInt8Number, icSigUInt8ArrayType>;
template class CIccTagNum<icUInt16Number, icSigUInt16ArrayType>;
template class CIccTagNum<icUInt32Number, icSigUInt32ArrayType>;
template class CIccTagNum<icUInt64Number, icSigUInt64ArrayType>;


/**
 ****************************************************************************
 * Name: CIccTagMeasurement::CIccTagMeasurement
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagMeasurement::CIccTagMeasurement()
{
  memset(&m_Data, 0, sizeof(m_Data));
}


/**
 ****************************************************************************
 * Name: CIccTagMeasurement::CIccTagMeasurement
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITM = The CIccTagMeasurement object to be copied
 *****************************************************************************
 */
CIccTagMeasurement::CIccTagMeasurement(const CIccTagMeasurement &ITM)
{
  memcpy(&m_Data, &ITM.m_Data, sizeof(m_Data));
}


/**
 ****************************************************************************
 * Name: CIccTagMeasurement::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  MeasTag = The CIccTagMeasurement object to be copied
 *****************************************************************************
 */
CIccTagMeasurement &CIccTagMeasurement::operator=(const CIccTagMeasurement &MeasTag)
{
  if (&MeasTag == this)
    return *this;

  memcpy(&m_Data, &MeasTag.m_Data, sizeof(m_Data));

  return *this;
}



/**
 ****************************************************************************
 * Name: CIccTagMeasurement::~CIccTagMeasurement
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagMeasurement::~CIccTagMeasurement()
{
}


/**
 ****************************************************************************
 * Name: CIccTagMeasurement::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagMeasurement::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number) + 
      sizeof(m_Data) > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  icUInt32Number nSize=sizeof(m_Data)/sizeof(icUInt32Number);

  if (pIO->Read32(&m_Data,nSize) != nSize)
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagMeasurement::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagMeasurement::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  icUInt32Number nSize=sizeof(m_Data)/sizeof(icUInt32Number);

  if (pIO->Write32(&m_Data,nSize) != nSize)
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagMeasurement::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagMeasurement::Describe(std::string &sDescription)
{
  CIccInfo Fmt;
  icChar buf[128];

   sDescription += Fmt.GetStandardObserverName(m_Data.stdObserver); sDescription += "\n";
   sprintf(buf, "Backing measurement: X=%.4lf, Y=%.4lf, Z=%.4lf\n",
           icFtoD(m_Data.backing.X),
           icFtoD(m_Data.backing.Y),
           icFtoD(m_Data.backing.Z)); 
   sDescription += buf;
   sDescription += Fmt.GetMeasurementGeometryName(m_Data.geometry); sDescription += "\n";
   sDescription += Fmt.GetMeasurementFlareName(m_Data.flare); sDescription += "\n";
   sDescription += Fmt.GetIlluminantName(m_Data.illuminant); sDescription += "\n";
}


/**
******************************************************************************
* Name: CIccTagMeasurement::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagMeasurement::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  switch(m_Data.stdObserver) {
  case icStdObsUnknown:
  case icStdObs1931TwoDegrees:
  case icStdObs1964TenDegrees:
    break;

  default:
    sReport += icValidateNonCompliantMsg;
    sReport += sSigName;
    sReport += " - Invalid standard observer encoding.\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  switch(m_Data.geometry) {
  case icGeometryUnknown:
  case icGeometry045or450:
  case icGeometry0dord0:
    break;

  default:
    sReport += icValidateNonCompliantMsg;
    sReport += sSigName;
    sReport += " - Invalid measurement geometry encoding.\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  switch(m_Data.illuminant) {
  case icIlluminantUnknown:
  case icIlluminantD50:
  case icIlluminantD65:
  case icIlluminantD93:
  case icIlluminantF2:
  case icIlluminantD55:
  case icIlluminantA:
  case icIlluminantEquiPowerE:
  case icIlluminantF8:
    break;

  default:
    sReport += icValidateNonCompliantMsg;
    sReport += sSigName;
    sReport += " - Invalid standard illuminant encoding.\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  return rv;
}


/**
 ****************************************************************************
 * Name: CIccLocalizedUnicode::CIccLocalizedUnicode
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccLocalizedUnicode::CIccLocalizedUnicode()
{
  m_pBuf = (icUInt16Number*)malloc(1*sizeof(icUInt16Number));
  *m_pBuf = 0;
  m_nLength = 0;
}


/**
 ****************************************************************************
 * Name: CIccLocalizedUnicode::CIccLocalizedUnicode
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ILU = The CIccLocalizedUnicode object to be copied
 *****************************************************************************
 */
CIccLocalizedUnicode::CIccLocalizedUnicode(const CIccLocalizedUnicode& ILU)
{
  m_nLength = ILU.GetLength();
  m_pBuf = (icUInt16Number*)malloc((m_nLength+1) * sizeof(icUInt16Number));
  if (m_nLength)
    memcpy(m_pBuf, ILU.GetBuf(), m_nLength*sizeof(icUInt16Number));
  m_pBuf[m_nLength] = 0;
  m_nLanguageCode = ILU.m_nLanguageCode;
  m_nCountryCode = ILU.m_nCountryCode;
}


/**
 ****************************************************************************
 * Name: CIccLocalizedUnicode::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  UnicodeText = The CIccLocalizedUnicode object to be copied
 *****************************************************************************
 */
CIccLocalizedUnicode &CIccLocalizedUnicode::operator=(const CIccLocalizedUnicode &UnicodeText)
{
  if (&UnicodeText == this)
    return *this;

  SetSize(UnicodeText.GetLength());
  memcpy(m_pBuf, UnicodeText.GetBuf(), m_nLength*sizeof(icUInt16Number));
  m_nLanguageCode = UnicodeText.m_nLanguageCode;
  m_nCountryCode = UnicodeText.m_nCountryCode;

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccLocalizedUnicode::~CIccLocalizedUnicode
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccLocalizedUnicode::~CIccLocalizedUnicode()
{
  if (m_pBuf)
    free(m_pBuf);
}

/**
 ****************************************************************************
 * Name: CIccLocalizedUnicode::GetAnsiSize
 * 
 * Purpose: Returns the size of the ANSI data buffer
 *
 *****************************************************************************
 */
icUInt32Number CIccLocalizedUnicode::GetAnsiSize()
{
  icUInt32Number len;
#ifdef USE_WINDOWS_MB_SUPPORT
  len = WideCharToMultiByte(CP_ACP, 0x00000400, (LPCWSTR)m_pBuf, m_nLength,  NULL, 0, NULL, NULL);
#else
  len = m_nLength;   
#endif

  return len;
}

/**
 ****************************************************************************
 * Name: CIccLocalizedUnicode::GetAnsi
 * 
 * Purpose: Extracts the ANSI data buffer
 *
 * Args:
 *  szBuf = pointer where the returned string buffer is to be stored
 *  nBufSize = size of the buffer to be extracted
 *
 * Return:
 *  Pointer to the ANSI data string
 *****************************************************************************
 */
const icChar *CIccLocalizedUnicode::GetAnsi(icChar *szBuf, icUInt32Number nBufSize)
{
  if (!szBuf)
    return NULL;

  if (!m_nLength) {
    *szBuf='\0';
  }
  else {
#ifdef USE_WINDOWS_MB_SUPPORT
    int len = WideCharToMultiByte(CP_ACP, 0x00000400, (LPCWSTR)m_pBuf, m_nLength,  szBuf, nBufSize, NULL, NULL);
    szBuf[len]='\0';
#else
  icUInt32Number i;
  
    for (i=0; i<m_nLength; i++) {
      if (m_pBuf[i]<256) {
        szBuf[i] = (icChar)m_pBuf[i];
      }
      else {
        szBuf[i] = '?';
      }
    }
#endif
  }

  return szBuf;
}

/**
 ****************************************************************************
 * Name: CIccLocalizedUnicode::SetSize
 * 
 * Purpose: Sets the size of the string buffer.
 * 
 * Args: 
 *  nSize - length of the string
 *
 *****************************************************************************
 */
void CIccLocalizedUnicode::SetSize(icUInt32Number nSize)
{
  if (nSize == m_nLength)
    return;

  m_pBuf = (icUInt16Number*)realloc(m_pBuf, (nSize+1)*sizeof(icUInt16Number));
  m_nLength = nSize;

  m_pBuf[nSize]=0;
}

/**
 ****************************************************************************
 * Name: CIccLocalizedUnicode::SetText
 * 
 * Purpose: Allows text data associated with the tag to be set.
 * 
 * Args: 
 *  szText = zero terminated string to put in tag,
 *  nLanguageCode = the language code type as defined by icLanguageCode,
 *  nRegionCode = the region code type as defined by icCountryCode
 *****************************************************************************
 */
void CIccLocalizedUnicode::SetText(const icChar *szText,
                                   icLanguageCode nLanguageCode/* = icLanguageCodeEnglish*/,
                                   icCountryCode nRegionCode/* = icCountryCodeUSA*/)
{
  int len=strlen(szText), i;
  icUInt16Number *pBuf;

  SetSize(len);
  pBuf = m_pBuf;
  for (i=0; i<len; i++) {
    *pBuf++ = *szText++;
  }
  *pBuf = 0;

  m_nLanguageCode = nLanguageCode;
  m_nCountryCode = nRegionCode;
}

/**
 ****************************************************************************
 * Name: CIccLocalizedUnicode::SetText
 * 
 * Purpose: Allows text data associated with the tag to be set.
 * 
 * Args: 
 *  sszUnicodeText = Unicode text to be set,
 *  nLanguageCode = the language code type as defined by icLanguageCode,
 *  nRegionCode = the region code type as defined by icCountryCode
 *****************************************************************************
 */
void CIccLocalizedUnicode::SetText(const icUInt16Number *sszUnicodeText,
                                   icLanguageCode nLanguageCode/* = icLanguageCodeEnglish*/,
                                   icCountryCode nRegionCode/* = icCountryCodeUSA*/)
{
  const icUInt16Number *pBuf=sszUnicodeText;
  int len;

  for (len=0; *pBuf; len++, pBuf++);

  SetSize(len);
  memcpy(m_pBuf, sszUnicodeText, (len+1)*sizeof(icUInt16Number));

  m_nLanguageCode = nLanguageCode;
  m_nCountryCode = nRegionCode;
}



/**
 ****************************************************************************
 * Name: CIccTagMultiLocalizedUnicode::CIccTagMultiLocalizedUnicode
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagMultiLocalizedUnicode::CIccTagMultiLocalizedUnicode()
{
  m_Strings = new(CIccMultiLocalizedUnicode);
}


/**
 ****************************************************************************
 * Name: CIccTagMultiLocalizedUnicode::CIccTagMultiLocalizedUnicode
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITMLU = The CIccTagMultiLocalizedUnicode object to be copied
 *****************************************************************************
 */
CIccTagMultiLocalizedUnicode::CIccTagMultiLocalizedUnicode(const CIccTagMultiLocalizedUnicode& ITMLU)
{
  m_Strings = new(CIccMultiLocalizedUnicode);
  *m_Strings = *ITMLU.m_Strings;
}


/**
 ****************************************************************************
 * Name: CIccTagMultiLocalizedUnicode::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  MultiLocalizedTag = The CIccTagMultiLocalizedUnicode object to be copied
 *****************************************************************************
 */
CIccTagMultiLocalizedUnicode &CIccTagMultiLocalizedUnicode::operator=(const CIccTagMultiLocalizedUnicode &MultiLocalizedTag)
{
  if (&MultiLocalizedTag == this)
    return *this;

  m_Strings->clear();
  *m_Strings = *MultiLocalizedTag.m_Strings;

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagMultiLocalizedUnicode::~CIccTagMultiLocalizedUnicode
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagMultiLocalizedUnicode::~CIccTagMultiLocalizedUnicode()
{
  delete m_Strings;
}


/**
 ****************************************************************************
 * Name: CIccTagMultiLocalizedUnicode::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagMultiLocalizedUnicode::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt32Number nNumRec, nRecSize;
  icLanguageCode nLanguageCode;
  icCountryCode nRegionCode;
  icUInt32Number nLength, nOffset, nNumChar;

  if (!m_Strings->empty())
    m_Strings->clear();

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number)*3 > size)
    return false;

  if (!pIO) {
    return false;
  }

  icUInt32Number nTagPos = pIO->Tell();
  
  if (!pIO->Read32(&sig) ||
      !pIO->Read32(&m_nReserved) ||
      !pIO->Read32(&nNumRec) ||
      !pIO->Read32(&nRecSize))
    return false;


  if (nRecSize!=12) { //Recognized version name records are 12 bytes each
    return false;
  }

  icUInt32Number i; 
  CIccLocalizedUnicode Unicode;

  for (i=0; i<nNumRec; i++) {
    if (4*sizeof(icUInt32Number) + (i+1)*12 > size)
      return false;

    pIO->Seek(nTagPos+4*sizeof(icUInt32Number) + i*12, icSeekSet);

    if (!pIO->Read16(&nLanguageCode) ||
        !pIO->Read16(&nRegionCode) ||
        !pIO->Read32(&nLength) ||
        !pIO->Read32(&nOffset))
      return false;
    
    if (nOffset+nLength > size)
      return false;

    nNumChar = nLength / sizeof(icUInt16Number);

    Unicode.SetSize(nNumChar);
    Unicode.m_nLanguageCode = nLanguageCode;
    Unicode.m_nCountryCode = nRegionCode;

    pIO->Seek(nTagPos+nOffset, icSeekSet);

    if (pIO->Read16(Unicode.GetBuf(), nNumChar) != nNumChar)
      return false;

    m_Strings->push_back(Unicode);
  }

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagMultiLocalizedUnicode::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagMultiLocalizedUnicode::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();
  icUInt32Number nNumRec=m_Strings->size(), nRecSize=12;
  icUInt32Number nLength;

  if (!pIO) {
    return false;
  }

  if (!pIO->Write32(&sig) ||
      !pIO->Write32(&m_nReserved) ||
      !pIO->Write32(&nNumRec) ||
      !pIO->Write32(&nRecSize))
    return false;


  icUInt32Number nPos = 4*sizeof(icUInt32Number) + nNumRec*12;

  CIccMultiLocalizedUnicode::iterator i;

  for (i=m_Strings->begin(); i!=m_Strings->end(); i++) {
    nLength = i->GetLength() * sizeof(icUInt16Number);

    if (!pIO->Write16(&i->m_nLanguageCode) ||
        !pIO->Write16(&i->m_nCountryCode) ||
        !pIO->Write32(&nLength) ||
        !pIO->Write32(&nPos))
      return false;
    nPos += nLength;
  }

  for (i=m_Strings->begin(); i!=m_Strings->end(); i++) {
    nLength = i->GetLength();

    if (nLength) {
      if (pIO->Write16(i->GetBuf(), nLength) != nLength)
        return false;
    }
  }

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagMultiLocalizedUnicode::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagMultiLocalizedUnicode::Describe(std::string &sDescription)
{
  icChar *szBuf = (icChar*)malloc(128);
  int nSize = 127, nAnsiSize;
  CIccMultiLocalizedUnicode::iterator i;

  for (i=m_Strings->begin(); i!=m_Strings->end(); i++) {
    if (i!=m_Strings->begin())
      sDescription += "\n";

    sprintf(szBuf, "Language = '%c%c', Region = '%c%c'\n",
      i->m_nLanguageCode>>8, i->m_nLanguageCode,
      i->m_nCountryCode>>8, i->m_nCountryCode);
    
    sDescription += szBuf;

    nAnsiSize = i->GetAnsiSize();

    if (nAnsiSize>nSize) {
      szBuf = (icChar*)realloc(szBuf, nAnsiSize+1);
      nSize = nAnsiSize;
    }
    i->GetAnsi(szBuf, nSize);
    sDescription += "\"";
    sDescription += szBuf;
    sDescription += "\"\n";
  }
}


/**
******************************************************************************
* Name: CIccTagMultiLocalizedUnicode::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagMultiLocalizedUnicode::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (!m_Strings->size()) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " - Empty tag!\n";
    rv = icMaxStatus(rv, icValidateWarning);
  }

  // TODO: Look at the ISO-639 and ISO-3166 documents for valid 
  // Language and Region codes
/*
  CIccMultiLocalizedUnicode::iterator i;
  for (i=m_Strings->begin(); i!=m_Strings->end(); i++) {
    switch(i->m_nLanguageCode) {
    case :
      break;
    default:
    }

    switch(i->m_nRegionCode) {
    case :
      break;
    default:
    }
  }
*/

  return rv;
}

/**
****************************************************************************
* Name: sampleICC::CIccTagMultiLocalizedUnicode::Find
* 
* Purpose: 
* 
* Args:
*  nLanguageCode
*  nRegionCode
*
* Return:
*  Pointer to CIccLocalizedUnicode object associated with the nLanguageCode
*  and nRegionCode or NULL if not found
*****************************************************************************
*/
CIccLocalizedUnicode *CIccTagMultiLocalizedUnicode::Find(icLanguageCode nLanguageCode /* = icLanguageCodeEnglish */,
                                                         icCountryCode nRegionCode /* = icCountryCodeUSA */)
{
  CIccMultiLocalizedUnicode::iterator i;

  for (i=m_Strings->begin(); i!=m_Strings->end(); i++) {
    if (i->m_nLanguageCode == nLanguageCode &&
      i->m_nCountryCode == nRegionCode) {
      return &(*i);
    }
  }

  return NULL;
}

/**
****************************************************************************
* Name: sampleICC::CIccTagMultiLocalizedUnicode::SetText
* 
* Purpose: 
* 
* Args:
*  sszUnicodeText
*  nLanguageCode
*  RegionCode
*****************************************************************************
*/
void CIccTagMultiLocalizedUnicode::SetText(const icChar *szText, 
                                           icLanguageCode nLanguageCode /* = icLanguageCodeEnglish */,
                                           icCountryCode nRegionCode /* = icCountryCodeUSA */)
{
   CIccLocalizedUnicode *pText = Find(nLanguageCode, nRegionCode);

   if (!pText) {
     CIccLocalizedUnicode newText;
     newText.SetText(szText, nLanguageCode, nRegionCode);
     m_Strings->push_back(newText);
   }
   else {
     pText->SetText(szText, nLanguageCode, nRegionCode);
   }
}


/**
****************************************************************************
* Name: sampleICC::CIccTagMultiLocalizedUnicode::SetText
* 
* Purpose: 
* 
* Args:
*  sszUnicodeText
*  nLanguageCode
*  RegionCode
*****************************************************************************
*/
void CIccTagMultiLocalizedUnicode::SetText(const icUInt16Number *sszUnicodeText, 
                                           icLanguageCode nLanguageCode /* = icLanguageCodeEnglish */,
                                           icCountryCode nRegionCode /* = icCountryCodeUSA */)
{
  CIccLocalizedUnicode *pText = Find(nLanguageCode, nRegionCode);

  if (!pText) {
    CIccLocalizedUnicode newText;
    newText.SetText(sszUnicodeText, nLanguageCode, nRegionCode);
    m_Strings->push_back(newText);
  }
  else {
    pText->SetText(sszUnicodeText, nLanguageCode, nRegionCode);
  }
}

//
// MD: Moved Curve and LUT tags to IccTagLut.cpp (4-30-05)
//


/**
 ****************************************************************************
 * Name: CIccTagData::CIccTagData
 * 
 * Purpose: Constructor
 *
 * Args:
 *  nSize = number of data entries
 * 
 *****************************************************************************
 */
CIccTagData::CIccTagData(int nSize/*=1*/)
{
  m_nSize = nSize;
  if (m_nSize <1)
    m_nSize = 1;
  m_pData = (icUInt8Number*)calloc(nSize, sizeof(icUInt8Number));
}


/**
 ****************************************************************************
 * Name: CIccTagData::CIccTagData
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITD = The CIccTagData object to be copied
 *****************************************************************************
 */
CIccTagData::CIccTagData(const CIccTagData &ITD)
{
  m_nDataFlag = ITD.m_nDataFlag;
  m_nSize = ITD.m_nSize;

  m_pData = (icUInt8Number*)calloc(m_nSize, sizeof(icUInt8Number));
  memcpy(m_pData, ITD.m_pData, sizeof(m_pData));
}


/**
 ****************************************************************************
 * Name: CIccTagData::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  DataTag = The CIccTagData object to be copied
 *****************************************************************************
 */
CIccTagData &CIccTagData::operator=(const CIccTagData &DataTag)
{
  if (&DataTag == this)
    return *this;

  m_nDataFlag = DataTag.m_nDataFlag;
  m_nSize = DataTag.m_nSize;

  if (m_pData)
    free(m_pData);
  m_pData = (icUInt8Number*)calloc(m_nSize, sizeof(icUInt8Number));
  memcpy(m_pData, DataTag.m_pData, sizeof(m_pData));

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagData::~CIccTagData
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagData::~CIccTagData()
{
  if (m_pData)
    free(m_pData);
}


/**
 ****************************************************************************
 * Name: CIccTagData::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagData::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number) + 
      sizeof(icUInt32Number) +
      sizeof(icUInt8Number) > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read32(&m_nDataFlag))
    return false;

  icUInt32Number nNum = size-3*sizeof(icUInt32Number);

  SetSize(nNum);

  if (pIO->Read8(m_pData, nNum)!=nNum)
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagData::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagData::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write32(&m_nDataFlag))
    return false;

  if (pIO->Write8(m_pData, m_nSize)!=m_nSize)
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagData::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagData::Describe(std::string &sDescription)
{
  icChar buf[128];

  sDescription = "\nData:\n";

  if (IsTypeAscii()) {
      sprintf(buf, "%s\n", (icChar*)m_pData);
      sDescription += buf;
  }
  else
    for (int i = 0; i<(int)m_nSize; i++) {
      sprintf(buf, "%d\n", m_pData[i]);
      sDescription += buf;
    }

}

/**
 ****************************************************************************
 * Name: CIccTagData::SetSize
 * 
 * Purpose: Sets the size of the data array.
 * 
 * Args: 
 *  nSize - number of data entries,
 *  bZeroNew - flag to zero newly formed values
 *****************************************************************************
 */
void CIccTagData::SetSize(icUInt32Number nSize, bool bZeroNew/*=true*/)
{
  if (m_nSize == nSize)
    return;

  m_pData = (icUInt8Number*)realloc(m_pData, nSize*sizeof(icUInt8Number));
  if (bZeroNew && nSize > m_nSize) {
    memset(&m_pData[m_nSize], 0, (nSize-m_nSize)*sizeof(icUInt8Number));
  }
  m_nSize = nSize;
}


/**
******************************************************************************
* Name: CIccTagData::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagData::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  switch(m_nDataFlag) {
  case 0x00000000:
  case 0x00000001:
    break;
  default:
    sReport += icValidateNonCompliantMsg;
    sReport += sSigName;
    sReport += " - Invalid data flag encoding.\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  return rv;
}

/**
 ****************************************************************************
 * Name: CIccTagDateTime::CIccTagDateTime
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagDateTime::CIccTagDateTime()
{
  memset(&m_DateTime, 0, sizeof(m_DateTime));
}


/**
 ****************************************************************************
 * Name: CIccTagDateTime::CIccTagDateTime
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITDT = The CIccTagDateTime object to be copied
 *****************************************************************************
 */
CIccTagDateTime::CIccTagDateTime(const CIccTagDateTime &ITDT)
{
  memcpy(&m_DateTime, &ITDT.m_DateTime, sizeof(m_DateTime));
}


/**
 ****************************************************************************
 * Name: CIccTagDateTime::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  DateTimeTag = The CIccTagDateTime object to be copied
 *****************************************************************************
 */
CIccTagDateTime &CIccTagDateTime::operator=(const CIccTagDateTime &DateTimeTag)
{
  if (&DateTimeTag == this)
    return *this;

  memcpy(&m_DateTime, &DateTimeTag.m_DateTime, sizeof(m_DateTime));

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagDateTime::~CIccTagDateTime
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagDateTime::~CIccTagDateTime()
{
}



/**
 ****************************************************************************
 * Name: CIccTagDateTime::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagDateTime::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number) + 
      sizeof(icDateTimeNumber) > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;


  icUInt32Number nsize = (size-2*sizeof(icUInt32Number))/sizeof(icUInt16Number);

  if (pIO->Read16(&m_DateTime,nsize) != nsize)
    return false;

  return true;
}



/**
 ****************************************************************************
 * Name: CIccTagDateTime::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagDateTime::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (pIO->Write16(&m_DateTime,6) != 6)
    return false;
  
  return true;
}



/**
 ****************************************************************************
 * Name: CIccTagDateTime::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagDateTime::Describe(std::string &sDescription)
{
  icChar buf[128];

  sDescription = "Date = ";
  sprintf(buf, "%u-%u-%u\n", m_DateTime.month, m_DateTime.day, m_DateTime.year);
  sDescription += buf;
  
  sDescription += "Time = ";
  sprintf(buf, "%u:%u:%u\n", m_DateTime.hours, m_DateTime.minutes, m_DateTime.seconds);
  sDescription += buf;
}


/**
******************************************************************************
* Name: CIccTagDateTime::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagDateTime::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);
  CIccInfo Info;

  rv = icMaxStatus(rv, Info.CheckData(sReport, m_DateTime));
  
  return rv;
}



/**
 ****************************************************************************
 * Name: CIccTagColorantOrder::CIccTagColorantOrder
 * 
 * Purpose: Constructor
 * 
 * Args:
 *  nSize = number of channels
 * 
 *****************************************************************************
 */
CIccTagColorantOrder::CIccTagColorantOrder(int nsize/*=1*/)
{
  m_nCount = nsize;
  if (m_nCount <1)
    m_nCount = 1;
  m_pData = (icUInt8Number*)calloc(nsize, sizeof(icUInt8Number));
}



/**
 ****************************************************************************
 * Name: CIccTagColorantOrder::CIccTagColorantOrder
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITCO = The CIccTagColorantOrder object to be copied
 *****************************************************************************
 */
CIccTagColorantOrder::CIccTagColorantOrder(const CIccTagColorantOrder &ITCO)
{
  m_nCount = ITCO.m_nCount;

  m_pData = (icUInt8Number*)calloc(m_nCount, sizeof(icUInt8Number));
  memcpy(m_pData, ITCO.m_pData, sizeof(m_pData));
}


/**
 ****************************************************************************
 * Name: CIccTagColorantOrder::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ColorantOrderTag = The CIccTagColorantOrder object to be copied
 *****************************************************************************
 */
CIccTagColorantOrder &CIccTagColorantOrder::operator=(const CIccTagColorantOrder &ColorantOrderTag)
{
  if (&ColorantOrderTag == this)
    return *this;

  m_nCount = ColorantOrderTag.m_nCount;

  if (m_pData)
    free(m_pData);
  m_pData = (icUInt8Number*)calloc(m_nCount, sizeof(icUInt8Number));
  memcpy(m_pData, ColorantOrderTag.m_pData, sizeof(m_pData));

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagColorantOrder::~CIccTagColorantOrder
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagColorantOrder::~CIccTagColorantOrder()
{
  if (m_pData)
    free(m_pData);
}



/**
 ****************************************************************************
 * Name: CIccTagColorantOrder::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagColorantOrder::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt32Number nCount;

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number) + 
      sizeof(icUInt32Number) > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read32(&nCount))
    return false;

  icUInt32Number nNum = (size - 3*sizeof(icUInt32Number))/sizeof(icUInt8Number);

  if (nNum < nCount)
    return false;

  SetSize((icUInt16Number)nCount);

  if (pIO->Read8(&m_pData[0],nNum) != nNum)
    return false;

  return true;
}



/**
 ****************************************************************************
 * Name: CIccTagColorantOrder::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagColorantOrder::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write32(&m_nCount))
    return false;

  if (pIO->Write8(&m_pData[0], m_nCount) != m_nCount)
    return false;
  
  return true;
}



/**
 ****************************************************************************
 * Name: CIccTagColorantOrder::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagColorantOrder::Describe(std::string &sDescription)
{
  icChar buf[128];

  sprintf(buf, "Colorant Count : %u\n", m_nCount);
  sDescription += buf;
  sDescription += "Order of Colorants:\n";
  
  for (int i=0; i<(int)m_nCount; i++) {
    sprintf(buf, "%u\n", m_pData[i]);
    sDescription += buf;
  }
}


/**
 ****************************************************************************
 * Name: CIccTagColorantOrder::SetSize
 * 
 * Purpose: Sets the size of the data array.
 * 
 * Args: 
 *  nSize - number of channels,
 *  bZeroNew - flag to zero newly formed values
 *****************************************************************************
 */
void CIccTagColorantOrder::SetSize(icUInt16Number nSize, bool bZeroNew/*=true*/)
{
  if (m_nCount == nSize)
    return;

  m_pData = (icUInt8Number*)realloc(m_pData, nSize*sizeof(icUInt8Number));
  if (bZeroNew && nSize > m_nCount) {
    memset(&m_pData[m_nCount], 0, (nSize - m_nCount)*sizeof(icUInt8Number));
  }

  m_nCount = nSize;
}


/**
******************************************************************************
* Name: CIccTagColorantOrder::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagColorantOrder::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (!pProfile) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " - Tag validation incomplete: Pointer to profile unavailable.\n";
    rv = icMaxStatus(rv, icValidateWarning);
    return rv;
  }

  if (m_nCount != icGetSpaceSamples(pProfile->m_Header.colorSpace)) {
    sReport += icValidateNonCompliantMsg;
    sReport += sSigName;
    sReport += " - Incorrect number of colorants.\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  return rv;
}



/**
 ****************************************************************************
 * Name: CIccTagColorantTable::CIccTagColorantTable
 * 
 * Purpose: Constructor
 * 
 * Args:
 *  nSize = number of entries
 * 
 *****************************************************************************
 */
CIccTagColorantTable::CIccTagColorantTable(int nSize/*=1*/)
{
  m_nCount = nSize;
  if (m_nCount<1)
    m_nCount = 1;

  m_pData = (icColorantTableEntry*)calloc(nSize, sizeof(icColorantTableEntry));
}


/**
 ****************************************************************************
 * Name: CIccTagColorantTable::CIccTagColorantTable
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITCT = The CIccTagUnknown object to be copied
 *****************************************************************************
 */
CIccTagColorantTable::CIccTagColorantTable(const CIccTagColorantTable &ITCT)
{
  m_PCS = ITCT.m_PCS;
  m_nCount = ITCT.m_nCount;

  m_pData = (icColorantTableEntry*)calloc(m_nCount, sizeof(icColorantTableEntry));
  memcpy(m_pData, ITCT.m_pData, sizeof(m_pData));
}


/**
 ****************************************************************************
 * Name: CIccTagColorantTable::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ColorantTableTag = The CIccTagColorantTable object to be copied
 *****************************************************************************
 */
CIccTagColorantTable &CIccTagColorantTable::operator=(const CIccTagColorantTable &ColorantTableTag)
{
  if (&ColorantTableTag == this)
    return *this;

  m_PCS = ColorantTableTag.m_PCS;
  m_nCount = ColorantTableTag.m_nCount;

  if (m_pData)
    free(m_pData);
  m_pData = (icColorantTableEntry*)calloc(m_nCount, sizeof(icColorantTableEntry));
  memcpy(m_pData, ColorantTableTag.m_pData, sizeof(m_pData));

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagColorantTable::~CIccTagColorantTable
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagColorantTable::~CIccTagColorantTable()
{
  if (m_pData)
    free(m_pData);
}


/**
 ****************************************************************************
 * Name: CIccTagColorantTable::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagColorantTable::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt32Number nCount;

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number) + 
      sizeof(icUInt32Number) +
      sizeof(icColorantTableEntry) > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read32(&nCount))
    return false;

  icUInt32Number nNum = (size - 3*sizeof(icUInt32Number))/sizeof(icColorantTableEntry);
  icUInt32Number nNum8 = sizeof(m_pData->name);
  icUInt32Number nNum16 = sizeof(m_pData->data)/sizeof(icUInt16Number);

  if (nNum < nCount)
    return false;
  
  SetSize((icUInt16Number)nCount);

  for (icUInt32Number i=0; i<nCount; i++) {
    if (pIO->Read8(&m_pData[i].name[0], nNum8) != nNum8)
      return false;

    if (pIO->Read16(&m_pData[i].data[0], nNum16) != nNum16)
      return false;
  }

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagColorantTable::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagColorantTable::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write32(&m_nCount))
    return false;

  icUInt32Number nNum8 = sizeof(m_pData->name);
  icUInt32Number nNum16 = sizeof(m_pData->data)/sizeof(icUInt16Number);

  for (icUInt32Number i=0; i<m_nCount; i++) {
    if (pIO->Write8(&m_pData[i].name[0],nNum8) != nNum8)
      return false;

    if (pIO->Write16(&m_pData[i].data[0],nNum16) != nNum16)
      return false;
  }

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagColorantTable::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagColorantTable::Describe(std::string &sDescription)
{
  icChar buf[128];

  icUInt32Number i, nLen, nMaxLen=0;
  icFloatNumber Lab[3];

  sprintf(buf, "BEGIN_COLORANTS %u\n", m_nCount);
  sDescription += buf;

  for (i=0; i<m_nCount; i++) {
    nLen = strlen(m_pData[i].name);
    if (nLen>nMaxLen)
      nMaxLen =nLen;
  }
  sDescription += "# NAME ";

  if (m_PCS == icSigXYZData) {
    sprintf(buf, "XYZ_X XYZ_Y XYZ_Z\n");
    sDescription += buf;
  }
  else {
    sprintf(buf, "Lab_L Lab_a Lab_b\n");
    sDescription += buf;
  }
  for (i=0; i<m_nCount; i++) {
    sprintf(buf, "%2u \"%s\"", i, m_pData[i].name);
    sDescription += buf;
    memset(buf, ' ', 128);
    buf[nMaxLen + 1 - strlen(m_pData[i].name)] ='\0';
    sDescription += buf;

    if (m_PCS == icSigXYZData) {
      sprintf(buf, "%7.4lf %7.4lf %7.4lf\n", icUSFtoD(m_pData[i].data[0]), icUSFtoD(m_pData[i].data[1]), icUSFtoD(m_pData[i].data[2]));
      sDescription += buf;
    }
    else {
      Lab[0] = icU16toF(m_pData[i].data[0]);
      Lab[1] = icU16toF(m_pData[i].data[1]);
      Lab[2] = icU16toF(m_pData[i].data[2]);
      icLabFromPcs(Lab);
      sprintf(buf, "%7.4lf %8.4lf %8.4lf\n", Lab[0], Lab[1], Lab[2]);
      sDescription += buf;
    }
  }

}

/**
 ****************************************************************************
 * Name: CIccTagColorantTable::SetSize
 * 
 * Purpose: Sets the size of the data array.
 * 
 * Args: 
 *  nSize - number of entries,
 *  bZeroNew - flag to zero newly formed values
 *****************************************************************************
 */
void CIccTagColorantTable::SetSize(icUInt16Number nSize, bool bZeroNew/*=true*/)
{
  if (m_nCount == nSize)
    return;

  m_pData = (icColorantTableEntry*)realloc(m_pData, nSize*sizeof(icColorantTableEntry));
  if (bZeroNew && nSize > m_nCount) {
    memset(&m_pData[m_nCount], 0, (nSize-m_nCount)*sizeof(icColorantTableEntry));
  }
  m_nCount = nSize;
}


/**
******************************************************************************
* Name: CIccTagColorantTable::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagColorantTable::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (!pProfile) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " - Tag validation incomplete: Pointer to profile unavailable.\n";
    rv = icMaxStatus(rv, icValidateWarning);
    return rv;
  }


  if (sig==icSigColorantTableOutTag) {
    if (pProfile->m_Header.deviceClass!=icSigLinkClass) {
      sReport += icValidateNonCompliantMsg;
      sReport += sSigName;
      sReport += " - Use of this tag is allowed only in DeviceLink Profiles.\n";
      rv = icMaxStatus(rv, icValidateNonCompliant);
    }
  }

  if (m_nCount != icGetSpaceSamples(pProfile->m_Header.colorSpace)) {
    sReport += icValidateNonCompliantMsg;
    sReport += sSigName;
    sReport += " - Incorrect number of colorants.\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  return rv;
}


/**
 ****************************************************************************
 * Name: CIccTagViewingConditions::CIccTagViewingConditions
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagViewingConditions::CIccTagViewingConditions()
{
}


/**
 ****************************************************************************
 * Name: CIccTagViewingConditions::CIccTagViewingConditions
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITVC = The CIccTagViewingConditions object to be copied
 *****************************************************************************
 */
CIccTagViewingConditions::CIccTagViewingConditions(const CIccTagViewingConditions &ITVC)
{
  m_illumType = ITVC.m_illumType;

  memcpy(&m_XYZIllum, &ITVC.m_XYZIllum, sizeof(icXYZNumber));
  memcpy(&m_XYZSurround, &ITVC.m_XYZSurround, sizeof(icXYZNumber));
}


/**
 ****************************************************************************
 * Name: CIccTagViewingConditions::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ViewCondTag = The CIccTagViewingConditions object to be copied
 *****************************************************************************
 */
CIccTagViewingConditions &CIccTagViewingConditions::operator=(const CIccTagViewingConditions &ViewCondTag)
{
  if (&ViewCondTag == this)
    return *this;

  m_illumType = ViewCondTag.m_illumType;

  memcpy(&m_XYZIllum, &ViewCondTag.m_XYZIllum, sizeof(icXYZNumber));
  memcpy(&m_XYZSurround, &ViewCondTag.m_XYZSurround, sizeof(icXYZNumber));

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagViewingConditions::~CIccTagViewingConditions
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagViewingConditions::~CIccTagViewingConditions()
{
}


/**
 ****************************************************************************
 * Name: CIccTagViewingConditions::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagViewingConditions::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  if (sizeof(icTagTypeSignature) + 
      2*sizeof(icUInt32Number) + 
      2*sizeof(icXYZNumber) > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (pIO->Read32(&m_XYZIllum.X, 3) != 3)
    return false;

  if (pIO->Read32(&m_XYZSurround.X, 3) != 3)
    return false;

  if (!pIO->Read32(&m_illumType))
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagViewingConditions::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagViewingConditions::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
   return false;

  if (!pIO->Write32(&sig))
   return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (pIO->Write32(&m_XYZIllum.X, 3) !=3)
    return false;

  if (pIO->Write32(&m_XYZSurround.X, 3) !=3)
    return false;

  if (!pIO->Write32(&m_illumType))
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagViewingConditions::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagViewingConditions::Describe(std::string &sDescription)
{
  icChar buf[128];
  CIccInfo Fmt;

  sprintf(buf, "Illuminant Tristimulus values: X = %.4lf, Y = %.4lf, Z = %.4lf\n", 
               icFtoD(m_XYZIllum.X), 
               icFtoD(m_XYZIllum.Y),
               icFtoD(m_XYZIllum.Z));
  sDescription += buf;

  sprintf(buf, "Surround Tristimulus values: X = %.4lf, Y = %.4lf, Z = %.4lf\n",
               icFtoD(m_XYZSurround.X),
               icFtoD(m_XYZSurround.Y),
               icFtoD(m_XYZSurround.Z));
  sDescription += buf;

  sDescription += "Illuminant Type: ";

  sDescription += Fmt.GetIlluminantName(m_illumType);
  sDescription += "\n";

}


/**
******************************************************************************
* Name: CIccTagViewingConditions::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagViewingConditions::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  rv = icMaxStatus(rv, Info.CheckData(sReport, m_XYZIllum));
  rv = icMaxStatus(rv, Info.CheckData(sReport, m_XYZSurround));

  return rv;
}


/**
 ****************************************************************************
 * Name: CIccProfileDescText::CIccProfileDescText
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccProfileDescText::CIccProfileDescText()
{
  m_pTag = NULL;
  m_bNeedsPading = false;
}


/**
 ****************************************************************************
 * Name: CIccProfileDescText::CIccProfileDescText
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  IPDC = The CIccTagUnknown object to be copied
 *****************************************************************************
 */
CIccProfileDescText::CIccProfileDescText(const CIccProfileDescText &IPDC)
{
  m_pTag = NULL;
  if (IPDC.m_pTag) {
    SetType(IPDC.GetType());
    if (m_pTag) {
      m_pTag = IPDC.m_pTag->NewCopy();
    }
    m_bNeedsPading = IPDC.m_bNeedsPading;
  }
}


/**
 ****************************************************************************
 * Name: CIccProfileDescText::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ProfDescText = The CIccProfileDescText object to be copied
 *****************************************************************************
 */
CIccProfileDescText &CIccProfileDescText::operator=(const CIccProfileDescText &ProfDescText)
{
  if (&ProfDescText == this)
    return *this;

  if (m_pTag)
    delete m_pTag;
  m_pTag = NULL;

  if (ProfDescText.m_pTag) {
    SetType(ProfDescText.GetType());
    if (m_pTag) {
      m_pTag = ProfDescText.m_pTag->NewCopy();
    }
    m_bNeedsPading = ProfDescText.m_bNeedsPading;
  }

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccProfileDescText::~CIccProfileDescText
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccProfileDescText::~CIccProfileDescText()
{
  if (m_pTag)
    delete m_pTag;
}


/**
 ****************************************************************************
 * Name: CIccProfileDescText::SetType
 * 
 * Purpose: Sets the type of the profile description text. Could be either 
 *  a MultiLocalizedUnicodeType or a TextDescriptionType.
 * 
 * Args:
 *  nType = the tag type signature
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccProfileDescText::SetType(icTagTypeSignature nType)
{
  if (m_pTag) {
    if (m_pTag->GetType() == nType)
      return true;

    delete m_pTag;
  }

  if (nType == icSigMultiLocalizedUnicodeType)
    m_pTag = new CIccTagMultiLocalizedUnicode;
  else if (nType == icSigTextDescriptionType)
    m_pTag = new CIccTagTextDescription;
  else
    m_pTag = NULL;

  return(m_pTag != NULL);
}


/**
 ****************************************************************************
 * Name: CIccProfileDescText::SetType
 * 
 * Purpose: Gets the type of the profile description text. Could be either 
 *  a MultiLocalizedUnicodeType or a TextDescriptionType.
 * 
 *****************************************************************************
 */
icTagTypeSignature CIccProfileDescText::GetType() const
{
  if (m_pTag)
    return m_pTag->GetType();

  return icSigUnknownType;
}


/**
 ****************************************************************************
 * Name: CIccProfileDescText::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccProfileDescText::Describe(std::string &sDescription)
{
  if (m_pTag)
    m_pTag->Describe(sDescription);
}


/**
 ****************************************************************************
 * Name: CIccProfileDescText::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccProfileDescText::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt32Number nPos;

  //Check for description tag type signature
  nPos = pIO->Tell();

  if ((nPos&0x03) != 0)
    m_bNeedsPading = true;

  if (!pIO->Read32(&sig))
    return false;
  pIO->Seek(nPos, icSeekSet);

  if (!SetType(sig)) {
    //We couldn't find it, but we may be looking in the wrong place
    //Re-Syncronize on a 4 byte boundary
    pIO->Sync32();

    nPos = pIO->Tell();
    if (!pIO->Read32(&sig))
      return false;
    pIO->Seek(nPos, icSeekSet);

    if (!SetType(sig)) {
      return false;
    }
  }

  if (m_pTag) {
    return m_pTag->Read(size, pIO);
  }
 
  return false;
}


/**
 ****************************************************************************
 * Name: CIccProfileDescText::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccProfileDescText::Write(CIccIO *pIO)
{
  if (!m_pTag)
    return false;

  if (m_pTag->Write(pIO)) {
    return pIO->Align32();
  }
  
  return false;
}



/**
 ****************************************************************************
 * Name: CIccProfileDescStruct::CIccProfileDescStruct
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccProfileDescStruct::CIccProfileDescStruct()
{
}


/**
 ****************************************************************************
 * Name: CIccProfileDescStruct::CIccProfileDescStruct
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  IPDS = The CIccProfileDescStruct object to be copied
 *****************************************************************************
 */
CIccProfileDescStruct::CIccProfileDescStruct(const CIccProfileDescStruct &IPDS)
{
  m_deviceMfg = IPDS.m_deviceMfg;
  m_deviceModel = IPDS.m_deviceModel;
  m_attributes = IPDS.m_attributes;
  m_technology = IPDS.m_technology;
  m_deviceMfgDesc = IPDS.m_deviceMfgDesc;
  m_deviceModelDesc = IPDS.m_deviceModelDesc;
}



/**
 ****************************************************************************
 * Name: CIccProfileDescStruct::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ProfDescStruct = The CIccProfileDescStruct object to be copied
 *****************************************************************************
 */
CIccProfileDescStruct &CIccProfileDescStruct::operator=(const CIccProfileDescStruct &ProfDescStruct)
{
  if (&ProfDescStruct == this)
    return *this;

  m_deviceMfg = ProfDescStruct.m_deviceMfg;
  m_deviceModel = ProfDescStruct.m_deviceModel;
  m_attributes = ProfDescStruct.m_attributes;
  m_technology = ProfDescStruct.m_technology;
  m_deviceMfgDesc = ProfDescStruct.m_deviceMfgDesc;
  m_deviceModelDesc = ProfDescStruct.m_deviceModelDesc;

  return *this;
}



/**
 ****************************************************************************
 * Name: CIccTagProfileSeqDesc::CIccTagProfileSeqDesc
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagProfileSeqDesc::CIccTagProfileSeqDesc()
{
  m_Descriptions = new(CIccProfileSeqDesc);
}


/**
 ****************************************************************************
 * Name: CIccTagProfileSeqDesc::CIccTagProfileSeqDesc
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITPSD = The CIccTagProfileSeqDesc object to be copied
 *****************************************************************************
 */
CIccTagProfileSeqDesc::CIccTagProfileSeqDesc(const CIccTagProfileSeqDesc &ITPSD)
{
  m_Descriptions = new(CIccProfileSeqDesc);
  *m_Descriptions = *ITPSD.m_Descriptions;
}


/**
 ****************************************************************************
 * Name: CIccTagProfileSeqDesc::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ProfSeqDescTag = The CIccTagProfileSeqDesc object to be copied
 *****************************************************************************
 */
CIccTagProfileSeqDesc &CIccTagProfileSeqDesc::operator=(const CIccTagProfileSeqDesc &ProfSeqDescTag)
{
  if (&ProfSeqDescTag == this)
    return *this;

  *m_Descriptions = *ProfSeqDescTag.m_Descriptions;

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagProfileSeqDesc::~CIccTagProfileSeqDesc
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagProfileSeqDesc::~CIccTagProfileSeqDesc()
{
  delete m_Descriptions;
}


/**
 ****************************************************************************
 * Name: CIccTagProfileSeqDesc::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagProfileSeqDesc::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt32Number nCount, nEnd;

  nEnd = pIO->Tell() + size;

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number)*2 > size)
    return false;

  if (!pIO) {
    return false;
  }
 
  if (!pIO->Read32(&sig) ||
      !pIO->Read32(&m_nReserved) ||
      !pIO->Read32(&nCount))
    return false;

  if (!nCount)
    return true;

  if (sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number)*2 +
    sizeof(CIccProfileDescStruct) > size)
    return false;

  icUInt32Number i, nPos; 
  CIccProfileDescStruct ProfileDescStruct;

  for (i=0; i<nCount; i++) {

    if (!pIO->Read32(&ProfileDescStruct.m_deviceMfg) ||
        !pIO->Read32(&ProfileDescStruct.m_deviceModel) ||
        !pIO->Read64(&ProfileDescStruct.m_attributes) ||
        !pIO->Read32(&ProfileDescStruct.m_technology))
      return false;

    nPos = pIO->Tell();

    if (!ProfileDescStruct.m_deviceMfgDesc.Read(nEnd - nPos, pIO))
      return false;
    
    nPos = pIO->Tell();
    if (!ProfileDescStruct.m_deviceModelDesc.Read(nEnd - nPos, pIO))
      return false;

    m_Descriptions->push_back(ProfileDescStruct);
  }

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagProfileSeqDesc::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagProfileSeqDesc::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();
  icUInt32Number nCount=m_Descriptions->size();

  if (!pIO) {
    return false;
  }

  if (!pIO->Write32(&sig) ||
      !pIO->Write32(&m_nReserved) ||
      !pIO->Write32(&nCount))
    return false;

  CIccProfileSeqDesc::iterator i;

  for (i=m_Descriptions->begin(); i!=m_Descriptions->end(); i++) {

    if (!pIO->Write32(&i->m_deviceMfg) ||
        !pIO->Write32(&i->m_deviceModel) ||
        !pIO->Write64(&i->m_attributes) ||
        !pIO->Write32(&i->m_technology))
      return false;

    if (!i->m_deviceMfgDesc.Write(pIO) ||
        !i->m_deviceModelDesc.Write(pIO))
      return false;
  }

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagProfileSeqDesc::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagProfileSeqDesc::Describe(std::string &sDescription)
{
  CIccProfileSeqDesc::iterator i;
  icChar buf[128], buf2[28];
  icUInt32Number count=0;

  sprintf(buf, "Number of Profile Description Structures: %u\n", m_Descriptions->size());
  sDescription += buf;

  for (i=m_Descriptions->begin(); i!=m_Descriptions->end(); i++, count++) {
    sDescription += "\n";

    sprintf(buf, "Profile Description Structure Number [%u] follows:\n", count+1);
    sDescription += buf;

    sprintf(buf, "Device Manufacturer Signature: %s\n", icGetSig(buf2, i->m_deviceMfg, false));
    sDescription += buf;

    sprintf(buf, "Device Model Signature: %s\n", icGetSig(buf2, i->m_deviceModel, false));
    sDescription += buf;

    sprintf(buf, "Device Attributes: %08x%08x\n", (icUInt32Number)(i->m_attributes >> 32), (icUInt32Number)(i->m_attributes));
    sDescription += buf;

    sprintf(buf, "Device Technology Signature: %s\n", icGetSig(buf2, i->m_technology, false));
    sDescription += buf;

    sprintf(buf, "Description of device manufacturer: \n");
    sDescription += buf;
    i->m_deviceMfgDesc.Describe(sDescription);

    sprintf(buf, "Description of device model: \n");
    sDescription += buf;
    i->m_deviceModelDesc.Describe(sDescription);
  }
}


/**
******************************************************************************
* Name: CIccTagProfileSeqDesc::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagProfileSeqDesc::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  char buf[128];
  std::string sSigName = Info.GetSigName(sig);

  CIccProfileSeqDesc::iterator i;
  for (i=m_Descriptions->begin(); i!=m_Descriptions->end(); i++) {
    switch(i->m_technology) {
    case 0x00000000:  //Technology not defined
    case icSigFilmScanner:
    case icSigDigitalCamera:
    case icSigReflectiveScanner:
    case icSigInkJetPrinter:
    case icSigThermalWaxPrinter:
    case icSigElectrophotographicPrinter:
    case icSigElectrostaticPrinter:
    case icSigDyeSublimationPrinter:
    case icSigPhotographicPaperPrinter:
    case icSigFilmWriter:
    case icSigVideoMonitor:
    case icSigVideoCamera:
    case icSigProjectionTelevision:
    case icSigCRTDisplay:
    case icSigPMDisplay:
    case icSigAMDisplay:
    case icSigPhotoCD:
    case icSigPhotoImageSetter:
    case icSigGravure:
    case icSigOffsetLithography:
    case icSigSilkscreen:
    case icSigFlexography:
      break;

    default:
      {
        sReport += icValidateNonCompliantMsg;
        sReport += sSigName;
        sprintf(buf, " - %s: Unknown Technology.\n", Info.GetSigName(i->m_technology));
        sReport += buf;
        rv = icMaxStatus(rv, icValidateNonCompliant);
      }
    }

    if (i->m_deviceMfgDesc.m_bNeedsPading) {
      sReport += icValidateNonCompliantMsg;
      sReport += sSigName;

      sReport += " Contains non-aligned deviceMfgDesc text tag information\n";

      rv = icMaxStatus(rv, icValidateNonCompliant);
    }

    if (i->m_deviceModelDesc.m_bNeedsPading) {
      sReport += icValidateNonCompliantMsg;
      sReport += sSigName;

      sReport += " Contains non-aligned deviceModelDesc text tag information\n";

      rv = icMaxStatus(rv, icValidateNonCompliant);
    }

    rv = icMaxStatus(rv, i->m_deviceMfgDesc.GetTag()->Validate(sig, sReport, pProfile));
    rv = icMaxStatus(rv, i->m_deviceModelDesc.GetTag()->Validate(sig, sReport, pProfile));
  }  

  return rv;
}


/**
 ****************************************************************************
 * Name: CIccResponseCurveStruct::CIccResponseCurveStruct
 * 
 * Purpose: Constructor
 * 
 * Args:
 *  nChannels = number of channels
 * 
 *****************************************************************************
 */
CIccResponseCurveStruct::CIccResponseCurveStruct(icUInt16Number nChannels/*=0*/)
{
  m_nChannels = nChannels;
  m_maxColorantXYZ = (icXYZNumber*)calloc(nChannels, sizeof(icXYZNumber));
  m_Response16ListArray = new CIccResponse16List[nChannels];
}


/**
 ****************************************************************************
 * Name: CIccResponseCurveStruct::CIccResponseCurveStruct
 * 
 * Purpose: Constructor
 *
 * Args:
 *  sig = measurement unit signature indicating the type of measurement data,
 *  nChannels = number of channels
 *****************************************************************************
 */
CIccResponseCurveStruct::CIccResponseCurveStruct(icMeasurementUnitSig sig,icUInt16Number nChannels/*=0*/)
{
  m_nChannels = nChannels;
  m_measurementUnitSig = sig;
  m_maxColorantXYZ = (icXYZNumber*)calloc(nChannels, sizeof(icXYZNumber));
  m_Response16ListArray = new CIccResponse16List[nChannels];
}

/**
 ****************************************************************************
 * Name: CIccResponseCurveStruct::CIccResponseCurveStruct
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  IRCS = The CIccTagUnknown object to be copied
 *****************************************************************************
 */
CIccResponseCurveStruct::CIccResponseCurveStruct(const CIccResponseCurveStruct &IRCS)
{
  m_nChannels = IRCS.m_nChannels;
  m_measurementUnitSig = IRCS.m_measurementUnitSig;

  m_maxColorantXYZ = (icXYZNumber*)calloc(m_nChannels, sizeof(icXYZNumber));
  memcpy(m_maxColorantXYZ, IRCS.m_maxColorantXYZ, m_nChannels*sizeof(icXYZNumber));

  m_Response16ListArray = new CIccResponse16List[m_nChannels];
  for (icUInt32Number i=0; i<m_nChannels; i++)
    m_Response16ListArray[i] = IRCS.m_Response16ListArray[i];
}


/**
 ****************************************************************************
 * Name: CIccResponseCurveStruct::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  RespCurveStruct = The CIccResponseCurveStruct object to be copied
 *****************************************************************************
 */
CIccResponseCurveStruct &CIccResponseCurveStruct::operator=(const CIccResponseCurveStruct &RespCurveStruct)
{
  if (&RespCurveStruct == this)
    return *this;

  m_nChannels = RespCurveStruct.m_nChannels;
  m_measurementUnitSig = RespCurveStruct.m_measurementUnitSig;

  if (m_maxColorantXYZ)
    free(m_maxColorantXYZ);

  m_maxColorantXYZ = (icXYZNumber*)calloc(m_nChannels, sizeof(icXYZNumber));
  memcpy(m_maxColorantXYZ, RespCurveStruct.m_maxColorantXYZ, m_nChannels*sizeof(icXYZNumber));

  if (m_Response16ListArray)
    delete [] m_Response16ListArray;
  m_Response16ListArray = new CIccResponse16List[m_nChannels];
  for (icUInt32Number i=0; i<m_nChannels; i++)
    m_Response16ListArray[i] = RespCurveStruct.m_Response16ListArray[i];

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccResponseCurveStruct::~CIccResponseCurveStruct
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccResponseCurveStruct::~CIccResponseCurveStruct()
{
  if (m_maxColorantXYZ)
    free(m_maxColorantXYZ);

  if (m_Response16ListArray)
    delete [] m_Response16ListArray;
}


/**
 ****************************************************************************
 * Name: CIccResponseCurveStruct::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccResponseCurveStruct::Read(icUInt32Number size, CIccIO *pIO)
{
  if (!m_nChannels)
    return false;

  if (sizeof(icTagTypeSignature) + 
      2*sizeof(icUInt32Number) +
      sizeof(icXYZNumber) + 
      sizeof(icResponse16Number) > size)
    return false;

  if (!pIO) {
    return false;
  }
 
  if (!pIO->Read32(&m_measurementUnitSig))
    return false;

  icUInt32Number* nMeasurements = new icUInt32Number[m_nChannels];

  if (pIO->Read32(&nMeasurements[0],m_nChannels) != m_nChannels)
    return false;

  icUInt32Number nNum32 = m_nChannels*sizeof(icXYZNumber)/sizeof(icS15Fixed16Number);
  if (pIO->Read32(&m_maxColorantXYZ[0], nNum32) != nNum32)
  return false;

  icResponse16Number nResponse16;
  CIccResponse16List nResponseList;

  for (int i = 0; i<m_nChannels; i++) {
    if (!nResponseList.empty())
      nResponseList.clear();
    for (int j=0; j<(int)nMeasurements[i]; j++) {
      if (!pIO->Read16(&nResponse16.deviceCode) ||
         !pIO->Read16(&nResponse16.reserved)   ||
         !pIO->Read32(&nResponse16.measurementValue))
        return false;
      nResponseList.push_back(nResponse16);
    }
    m_Response16ListArray[i] = nResponseList;
  }

  delete [] nMeasurements;
  return true;
}


/**
 ****************************************************************************
 * Name: CIccResponseCurveStruct::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccResponseCurveStruct::Write(CIccIO *pIO)
{
  if (!m_nChannels)
    return false;
  
  icMeasurementUnitSig sig = GetMeasurementType();

  if (!pIO) {
    return false;
  }

  if (!pIO->Write32(&sig))
    return false;

  if (m_nChannels) {

    icUInt32Number* nMeasurements = new icUInt32Number[m_nChannels];
    for (int i=0; i<m_nChannels; i++)
      nMeasurements[i] = m_Response16ListArray[i].size();

    if (pIO->Write32(&nMeasurements[0],m_nChannels) != m_nChannels)
      return false;
    delete [] nMeasurements;

    icUInt32Number nNum32 = m_nChannels*sizeof(icXYZNumber)/sizeof(icS15Fixed16Number);
    if (pIO->Write32(&m_maxColorantXYZ[0], nNum32) != nNum32)
      return false;
  }
  else
    return false;

  CIccResponse16List nResponseList;
  CIccResponse16List::iterator j;

  for (int i = 0; i<m_nChannels; i++) {
    nResponseList = m_Response16ListArray[i];
    for (j=nResponseList.begin(); j!=nResponseList.end(); j++) {
      if (!pIO->Write16(&j->deviceCode) ||
         !pIO->Write16(&j->reserved)   ||
         !pIO->Write32(&j->measurementValue))
        return false;
    }
    nResponseList.clear();
  }

  return true;
}


/**
 ****************************************************************************
 * Name: CIccResponseCurveStruct::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccResponseCurveStruct::Describe(std::string &sDescription)
{
  icChar buf[128];
  CIccInfo Fmt;
  CIccResponse16List nResponseList;
  CIccResponse16List::iterator j;

  sDescription += "Measurement Unit: ";
  sDescription += Fmt.GetMeasurementUnit((icSignature)GetMeasurementType()); sDescription += "\n";

  
  for (int i=0; i<m_nChannels; i++) {
    nResponseList = m_Response16ListArray[i];

    sDescription += "\n";
    sprintf(buf, "Maximum Colorant XYZ Measurement for Channel-%u : X=%.4lf, Y=%.4lf, Z=%.4lf\n", i+1, 
      icFtoD(m_maxColorantXYZ[i].X), icFtoD(m_maxColorantXYZ[i].Y), icFtoD(m_maxColorantXYZ[i].Z));
    sDescription += buf;

    sprintf(buf, "Number of Measurements for Channel-%u : %u\n", i+1, nResponseList.size());
    sDescription += buf;

    sprintf(buf, "Measurement Data for Channel-%u follows:\n", i+1);
    sDescription += buf;

    for (j=nResponseList.begin(); j!=nResponseList.end(); j++) {
      sprintf(buf, "Device Value= %u : Measurement Value= %.4lf\n", j->deviceCode, icFtoD(j->measurementValue));
      sDescription += buf;
    }
  }
}


/**
******************************************************************************
* Name: CIccResponseCurveStruct::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccResponseCurveStruct::Validate(std::string &sReport)
{
  icValidateStatus rv = icValidateOK;

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(m_measurementUnitSig);
  switch(m_measurementUnitSig) {
  case icSigStatusA:
  case icSigStatusE:
  case icSigStatusI:
  case icSigStatusT:
  case icSigStatusM:
  case icSigDN:
  case icSigDNP:
  case icSigDNN:
  case icSigDNNP:
    break;

  default:
    sReport += icValidateNonCompliantMsg;
    sReport += sSigName;
    sReport += " - Unknown measurement unit signature.\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  if (!m_nChannels) {
    sReport += icValidateNonCompliantMsg;
    sReport += sSigName;
    sReport += " - Incorrect number of channels.\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
    return rv;
  }
  for (int i=0; i<m_nChannels; i++) {
    rv = icMaxStatus(rv, Info.CheckData(sReport, m_maxColorantXYZ[i]));
  }

  return rv;
}



/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::CIccTagResponseCurveSet16
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagResponseCurveSet16::CIccTagResponseCurveSet16()
{
  m_nChannels = 0;

  m_ResponseCurves = new(CIccResponseCurveSet);
  m_Curve = new(CIccResponseCurveSetIter);
  m_Curve->inited = false;
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::CIccTagResponseCurveSet16
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITRCS = The CIccTagResponseCurveSet16 object to be copied
 *****************************************************************************
 */
CIccTagResponseCurveSet16::CIccTagResponseCurveSet16(const CIccTagResponseCurveSet16 &ITRCS)
{
  m_nChannels = ITRCS.m_nChannels;
  m_ResponseCurves = new(CIccResponseCurveSet);
  *m_ResponseCurves = *ITRCS.m_ResponseCurves;
  m_Curve = new(CIccResponseCurveSetIter);
  *m_Curve = *ITRCS.m_Curve;
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  RespCurveSet16Tag = The CIccTagResponseCurveSet16 object to be copied
 *****************************************************************************
 */
CIccTagResponseCurveSet16 &CIccTagResponseCurveSet16::operator=(const CIccTagResponseCurveSet16 &RespCurveSet16Tag)
{
  if (&RespCurveSet16Tag == this)
    return *this;

  if (!m_ResponseCurves->empty())
    m_ResponseCurves->clear();

  m_nChannels = RespCurveSet16Tag.m_nChannels;
  *m_ResponseCurves = *RespCurveSet16Tag.m_ResponseCurves;
  *m_Curve = *RespCurveSet16Tag.m_Curve;

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::~CIccTagResponseCurveSet16
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagResponseCurveSet16::~CIccTagResponseCurveSet16()
{
  if (!m_ResponseCurves->empty())
    m_ResponseCurves->clear();

  delete m_ResponseCurves;
  delete m_Curve;
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::Read
 * 
 * Purpose: Read in the tag contents into a data block
 * 
 * Args:
 *  size - # of bytes in tag,
 *  pIO - IO object to read tag from
 * 
 * Return: 
 *  true = successful, false = failure
 *****************************************************************************
 */
bool CIccTagResponseCurveSet16::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  if (sizeof(icTagTypeSignature) + 
      sizeof(icUInt32Number)*4 +
      sizeof(CIccResponseCurveStruct) > size)
    return false;

  if (!pIO) {
    return false;
  }
 
  if (!pIO->Read32(&sig) ||
      !pIO->Read32(&m_nReserved))
    return false;

  icUInt16Number nCountMeasmntTypes;
  
  if (!pIO->Read16(&m_nChannels) ||
      !pIO->Read16(&nCountMeasmntTypes))
    return false;


  icUInt32Number* nOffset = new icUInt32Number[nCountMeasmntTypes];

  if (pIO->Read32(&nOffset[0], nCountMeasmntTypes) != nCountMeasmntTypes)
    return false;

  delete [] nOffset;

  CIccResponseCurveStruct entry;

  for (icUInt16Number i=0; i<nCountMeasmntTypes; i++) {
    entry = CIccResponseCurveStruct(m_nChannels);
    if (!entry.Read(size, pIO))
      return false;

    m_ResponseCurves->push_back(entry);
  }
  m_Curve->inited = false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::Write
 * 
 * Purpose: Write the tag to a file
 * 
 * Args: 
 *  pIO - The IO object to write tag to.
 * 
 * Return: 
 *  true = succesful, false = failure
 *****************************************************************************
 */
bool CIccTagResponseCurveSet16::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();
  icUInt16Number nCountMeasmntTypes = m_ResponseCurves->size();

  if (!pIO) {
    return false;
  }

  icUInt32Number startPos = pIO->GetLength();

  if (!pIO->Write32(&sig) ||
      !pIO->Write32(&m_nReserved))
    return false;


  if (!pIO->Write16(&m_nChannels) ||
      !pIO->Write16(&nCountMeasmntTypes))
    return false;

  icUInt32Number offsetPos = pIO->GetLength();
  icUInt32Number* nOffset = new icUInt32Number[nCountMeasmntTypes];


  int j;
  for (j=0; j<nCountMeasmntTypes; j++) {
    nOffset[j] = 0;
    if (!pIO->Write32(&nOffset[j]))
      return false;
  }

  CIccResponseCurveSet::iterator i;

  for (i=m_ResponseCurves->begin(), j=0; i!=m_ResponseCurves->end(); i++, j++) {
    nOffset[j] = pIO->GetLength() - startPos;
    if (!i->Write(pIO))
      return false;
  }

  icUInt32Number curPOs = pIO->GetLength();

  pIO->Seek(offsetPos,icSeekSet);

  for (j=0; j<nCountMeasmntTypes; j++) {
    if (!pIO->Write32(&nOffset[j]))
      return false;
  }

  pIO->Seek(curPOs,icSeekSet);
  delete [] nOffset;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccTagResponseCurveSet16::Describe(std::string &sDescription)
{
  CIccResponseCurveSet::iterator i;
  icChar buf[128];

  sprintf(buf, "Number of Channels: %u\n", m_nChannels);
  sDescription += buf;

  sprintf(buf, "Number of Measurement Types used: %u\n", m_ResponseCurves->size());
  sDescription += buf;

  int count = 0;
  for (i=m_ResponseCurves->begin(); i!=m_ResponseCurves->end(); i++, count++) {
     sDescription += "\n";

    sprintf(buf, "Response Curve for measurement type [%u] follows:\n", count+1);
    sDescription += buf;

    i->Describe(sDescription);
  }
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::SetNumChannels
 * 
 * Purpose: Sets the number of channels. This will delete any prior Response
 *  curves from the set.
 * 
 * Args: 
 *  nChannels = number of channels
 *****************************************************************************
 */
void CIccTagResponseCurveSet16::SetNumChannels(icUInt16Number nChannels)
{
  m_nChannels = nChannels;

  if (!m_ResponseCurves->empty())
    m_ResponseCurves->clear();
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::NewResponseCurves
 * 
 * Purpose: Creates and adds a new set of response curves to the list. 
 *  SetNumChannels() must be called before calling this function.
 * 
 * Args: 
 *  sig = measurement unit signature
 *****************************************************************************
 */
CIccResponseCurveStruct *CIccTagResponseCurveSet16::NewResponseCurves(icMeasurementUnitSig sig)
{
  if (!m_nChannels)
    return NULL;

  CIccResponseCurveStruct *pResponseCurveStruct;
  pResponseCurveStruct = GetResponseCurves(sig);

  if (pResponseCurveStruct)
    return pResponseCurveStruct;

  CIccResponseCurveStruct entry;
  entry = CIccResponseCurveStruct(sig, m_nChannels);
  m_ResponseCurves->push_back(entry);
  m_Curve->inited = false;

  return GetResponseCurves(sig);
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::GetResponseCurves
 * 
 * Purpose: Returns pointer to the requested set of response curves
 * 
 * Args: 
 *  sig = measurement unit signature of the response curve set desired
 *****************************************************************************
 */
CIccResponseCurveStruct *CIccTagResponseCurveSet16::GetResponseCurves(icMeasurementUnitSig sig)
{
  if (!m_nChannels)
    return NULL;

  CIccResponseCurveSet::iterator i;

  for (i=m_ResponseCurves->begin(); i!=m_ResponseCurves->end(); i++) {
    if (i->GetMeasurementType() == sig)
      return (i->GetThis());
  }

  return NULL;
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::GetFirstCurves
 * 
 * Purpose: Returns pointer to the first set of response curves in the list.
 * 
 *****************************************************************************
 */
CIccResponseCurveStruct *CIccTagResponseCurveSet16::GetFirstCurves()
{
  if (!m_Curve)
    return NULL;

  m_Curve->item = m_ResponseCurves->begin();
  if (m_Curve->item == m_ResponseCurves->end()) {
    m_Curve->inited = false;
    return NULL;
  }
  m_Curve->inited = true;
  return m_Curve->item->GetThis();
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::GetNextCurves
 * 
 * Purpose: Serves as an iterator for the list containing response curves.
 *   GetFirstCurves() must be called before calling this function.
 * 
 *****************************************************************************
 */
CIccResponseCurveStruct *CIccTagResponseCurveSet16::GetNextCurves()
{
  if (!m_Curve || !m_Curve->inited)
    return NULL;

  m_Curve->item++;
  if (m_Curve->item==m_ResponseCurves->end()) {
    m_Curve->inited = false;
    return NULL;
  }
  return m_Curve->item->GetThis();
}


/**
 ****************************************************************************
 * Name: CIccTagResponseCurveSet16::GetNumResponseCurveTypes
 * 
 * Purpose: Get the number of response curve types.
 *   
 *****************************************************************************
 */
icUInt16Number CIccTagResponseCurveSet16::GetNumResponseCurveTypes() const
{
  return m_ResponseCurves->size();
}


/**
******************************************************************************
* Name: CIccTagResponseCurveSet16::Validate
* 
* Purpose: Check tag data validity.
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus CIccTagResponseCurveSet16::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (!pProfile) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " - Tag validation incomplete: Pointer to profile unavailable.\n";
    rv = icMaxStatus(rv, icValidateWarning);
    return rv;
  }

  if (m_nChannels!=icGetSpaceSamples(pProfile->m_Header.colorSpace)) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " - Incorrect number of channels.\n";
  }

  if (!GetNumResponseCurveTypes()) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " - Empty Tag!.\n";
    rv = icMaxStatus(rv, icValidateWarning);
  }
  else {
    CIccResponseCurveSet::iterator i;
    for (i=m_ResponseCurves->begin(); i!=m_ResponseCurves->end(); i++) {
      rv = icMaxStatus(rv, i->Validate(sReport));
    }
  }

  return rv;
}


#ifdef USESAMPLEICCNAMESPACE
} //namespace sampleICC
#endif
