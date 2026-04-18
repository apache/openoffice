/** @file
    File:       IccMpeACS.cpp

    Contains:   Implementation of ACS Elements

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
// -Initial implementation by Max Derhak 1-30-2006
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning( disable: 4786) //disable warning in <list.h>
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "IccMpeACS.h"
#include "IccIO.h"
#include <map>
#include "IccUtil.h"

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif


/**
******************************************************************************
* Name: CIccMpeAcs::CIccMpeACS
* 
* Purpose:
*  Base constructor (protected)
******************************************************************************/
CIccMpeAcs::CIccMpeAcs()
{
  m_pData = NULL;
  m_nDataSize = 0;

  m_nReserved = 0;
}

/**
******************************************************************************
* Name: CIccMpeAcs::~CIccMpeAcs
* 
* Purpose: 
*  Base destructor
******************************************************************************/
CIccMpeAcs::~CIccMpeAcs()
{
  if (m_pData)
    free(m_pData);
}

/**
******************************************************************************
* Name: CIccMpeAcs::Describe
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
void CIccMpeAcs::Describe(std::string &sDescription)
{
  icChar sigBuf[30];

  if (GetBAcsSig())
    sDescription += "ELEM_bACS\n";
  else
    sDescription += "ELEM_eACS\n";

  icGetSig(sigBuf, m_signature);
  sDescription += "  Signature = ";
  sDescription += sigBuf;
  sDescription += "\n";

  if (m_pData) {
    sDescription += "\nData Follows:\n";

    icMemDump(sDescription, m_pData, m_nDataSize);
  }
}

/**
******************************************************************************
* Name: CIccMpeAcs::Read
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
bool CIccMpeAcs::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt32Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&m_nInputChannels))
    return false;

  if (!pIO->Read16(&m_nOutputChannels))
    return false;

  if (!pIO->Read32(&m_signature))
    return false;

  icUInt32Number dataSize = size - headerSize;

  if (!AllocData(dataSize))
    return false;

  if (dataSize) {
    if (pIO->Read8(m_pData, dataSize)!=dataSize)
      return false;
  }

  return true;
}

/**
******************************************************************************
* Name: CIccMpeAcs::Write
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
bool CIccMpeAcs::Write(CIccIO *pIO)
{
  icElemTypeSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write16(&m_nInputChannels))
    return false;

  if (!pIO->Write16(&m_nOutputChannels))
    return false;

  if (!pIO->Write32(&m_signature))
    return false;

  if (m_pData && m_nDataSize) {
    if (!pIO->Write8(m_pData, m_nDataSize)!=m_nDataSize)
      return false;
  }

  return true;
}

/**
******************************************************************************
* Name: CIccMpeAcs::Begin
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
bool CIccMpeAcs::Begin(icElemInterp nInterp, CIccTagMultiProcessElement *pMPE)
{
  if (m_nInputChannels!=m_nOutputChannels)
    return false;

  return true;
}

/**
******************************************************************************
* Name: CIccMpeAcs::Apply
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
void CIccMpeAcs::Apply(icFloatNumber *dstPixel, const icFloatNumber *srcPixel)
{
  memcpy(dstPixel, srcPixel, m_nInputChannels*sizeof(icFloatNumber));
}

/**
******************************************************************************
* Name: CIccMpeAcs::Validate
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
icValidateStatus CIccMpeAcs::Validate(icTagSignature sig, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  icValidateStatus rv = CIccMultiProcessElement::Validate(sig, sReport, pMPE);

  return rv;
}

/**
******************************************************************************
* Name: CIccMpeAcs::AllocData
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
bool CIccMpeAcs::AllocData(icUInt32Number size)
{
  if (m_pData)
    free(m_pData);

  if (size) {
    m_pData = (icUInt8Number*)malloc(size);
    if (m_pData)
      m_nDataSize = size;
  }
  else {
    m_pData = NULL;
    m_nDataSize = 0;
  }

  return (size==0 || m_pData!=NULL);
}


/**
******************************************************************************
* Name: CIccMpeBeginAcs::CIccMpeBeginAcs
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccMpeBAcs::CIccMpeBAcs(icUInt16Number nChannels/* =0 */, icAcsSignature sig /* = icSigUnknownAcs */)
{
  m_signature = sig;

  m_nInputChannels = nChannels;
  m_nOutputChannels = nChannels;
}

/**
******************************************************************************
* Name: CIccMpeBeginAcs::CIccMpeBeginAcs
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccMpeBAcs::CIccMpeBAcs(const CIccMpeBAcs &elemAcs)
{

  m_signature = elemAcs.m_signature;
  m_nReserved = elemAcs.m_nReserved;
  m_nInputChannels = elemAcs.m_nInputChannels;
  m_nOutputChannels = elemAcs.m_nOutputChannels;

  m_pData = NULL;
  m_nDataSize = 0;

  AllocData(elemAcs.m_nDataSize);
  if (m_pData && elemAcs.m_nDataSize) {
    memcpy(m_pData, elemAcs.m_pData, m_nDataSize);
  }

  m_nReserved = 0;
}

/**
******************************************************************************
* Name: &CIccMpeBeginAcs::operator=
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccMpeBAcs &CIccMpeBAcs::operator=(const CIccMpeBAcs &elemAcs)
{
  m_signature = elemAcs.m_signature;
  m_nReserved = elemAcs.m_nReserved;
  m_nInputChannels = elemAcs.m_nInputChannels;
  m_nOutputChannels = elemAcs.m_nOutputChannels;

  AllocData(elemAcs.m_nDataSize);
  if (m_pData && elemAcs.m_nDataSize) {
    memcpy(m_pData, elemAcs.m_pData, m_nDataSize);
  }

  return *this;
}

/**
******************************************************************************
* Name: CIccMpeBeginAcs::~CIccMpeBeginAcs
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccMpeBAcs::~CIccMpeBAcs()
{
}

/**
******************************************************************************
* Name: CIccMpeEndAcs::CIccMpeEndAcs
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccMpeEAcs::CIccMpeEAcs(icUInt16Number nChannels/* =0 */, icAcsSignature sig /* = icSigUnknownAcs */)
{
  m_signature = sig;

  m_nInputChannels = nChannels;
  m_nOutputChannels = nChannels;
}

/**
******************************************************************************
* Name: CIccMpeEndAcs::CIccMpeEndAcs
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccMpeEAcs::CIccMpeEAcs(const CIccMpeEAcs &elemAcs)
{
  m_signature = elemAcs.m_signature;
  m_nReserved = elemAcs.m_nReserved;

  m_nInputChannels = elemAcs.m_nInputChannels;
  m_nOutputChannels = elemAcs.m_nOutputChannels;

  AllocData(elemAcs.m_nDataSize);
  if (m_pData && elemAcs.m_nDataSize) {
    memcpy(m_pData, elemAcs.m_pData, m_nDataSize);
  }
}

/**
******************************************************************************
* Name: &CIccMpeEndAcs::operator=
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccMpeEAcs &CIccMpeEAcs::operator=(const CIccMpeEAcs &elemAcs)
{
  m_signature = elemAcs.m_signature;
  m_nReserved = elemAcs.m_nReserved;
  m_nInputChannels = elemAcs.m_nInputChannels;
  m_nOutputChannels = elemAcs.m_nOutputChannels;

  AllocData(elemAcs.m_nDataSize);
  if (m_pData && elemAcs.m_nDataSize) {
    memcpy(m_pData, elemAcs.m_pData, m_nDataSize);
  }

  return *this;
}

/**
******************************************************************************
* Name: CIccMpeEndAcs::~CIccMpeEndAcs
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccMpeEAcs::~CIccMpeEAcs()
{
}

#ifdef USESAMPLEICCNAMESPACE
} //namespace sampleICC
#endif
