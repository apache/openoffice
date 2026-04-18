/** @file
    File:       IccMpeBasic.cpp

    Contains:   Implementation of Basic Multi Processing Elements

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
#include "IccMpeBasic.h"
#include "IccIO.h"
#include <map>
#include "IccUtil.h"

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::CIccFormulaCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccFormulaCurveSegment::CIccFormulaCurveSegment(icFloatNumber start, icFloatNumber end)
{
  m_nReserved = 0;
  m_nReserved2 = 0;
  m_startPoint = start;
  m_endPoint = end;

  m_nFunctionType = 0;
  m_nParameters = 0;
  m_params = NULL;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::CIccFormulaCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccFormulaCurveSegment::CIccFormulaCurveSegment(const CIccFormulaCurveSegment &seg)
{
  m_nReserved = seg.m_nReserved;
  m_nReserved2 = seg.m_nReserved2;
  m_startPoint = seg.m_startPoint;
  m_endPoint = seg.m_endPoint;

  m_nFunctionType = seg.m_nFunctionType;
  m_nParameters = seg.m_nParameters;

  if (seg.m_params) {
    m_params = (icFloatNumber*)malloc(m_nParameters*sizeof(icFloatNumber));
    memcpy(m_params, seg.m_params, m_nParameters*sizeof(icFloatNumber));
  }
  else
    m_params = NULL;
}

/**
 ******************************************************************************
 * Name: &CIccFormulaCurveSegment::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccFormulaCurveSegment &CIccFormulaCurveSegment::operator=(const CIccFormulaCurveSegment &seg)
{
  if (m_params)
    free(m_params);

  m_nReserved = seg.m_nReserved;
  m_nReserved2 = seg.m_nReserved2;
  m_startPoint = seg.m_startPoint;
  m_endPoint = seg.m_endPoint;

  m_nFunctionType = seg.m_nFunctionType;
  m_nParameters = seg.m_nParameters;
  if (seg.m_params) {
    m_params = (icFloatNumber*)malloc(m_nParameters*sizeof(icFloatNumber));
    memcpy(m_params, seg.m_params, m_nParameters*sizeof(icFloatNumber));
  }
  else
    m_params = NULL;

  return (*this);
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::~CIccFormulaCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccFormulaCurveSegment::~CIccFormulaCurveSegment()
{
  if (m_params) {
    free(m_params);
  }
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccFormulaCurveSegment::Describe(std::string &sDescription)
{
  icChar buf[128];

  sDescription += "Segment [";
  if (m_startPoint==icMinFloat32Number)
    sDescription += "-Infinity, ";
  else {
  sprintf(buf, "%.8f, ", m_startPoint);
  sDescription += buf;
  }
  if (m_endPoint==icMaxFloat32Number)
    sDescription += "+Infinity";
  else {
  sprintf(buf, "%.8f", m_endPoint);
  sDescription += buf;
  }
  sprintf(buf, "]\nFunctionType: %04Xh\n", m_nFunctionType);
  sDescription += buf;

  switch(m_nFunctionType) {
  case 0x0000:
    if (m_params[1]==0.0 && m_params[2]==0.0)
      sprintf(buf, "Y = %.8f\n\n", m_params[3]);
    else if (m_params[0]==1.0 && m_params[1]==1.0 && m_params[2]==0.0 && m_params[3]==0.0)
      sprintf(buf, "Y = X\n\n");
    else if (m_params[0]==1.0 && m_params[2]==0.0)
      sprintf(buf, "Y = %.8f * X + %.8f\n\n", 
              m_params[1], m_params[3]);
    else
      sprintf(buf, "Y = (%.8f * X + %.8f)^%.4f + %.8f\n\n", 
              m_params[1], m_params[2], m_params[0], m_params[3]);
    sDescription += buf;
    return;

  case 0x0001:
    sprintf(buf, "Y = %.8f * log (%.8f * (X ^ %.8f)  + %.8f) + %.8f\n\n",
            m_params[1], m_params[2], m_params[0], m_params[3], m_params[4]);
    sDescription += buf;
    return;

  case 0x0002:
    sprintf(buf, "Y = %.8f * (%.8f ^ (%.8f * X + %.8f)) + %.8f\n\n",
            m_params[0], m_params[1], m_params[2], m_params[3], m_params[4]);
    sDescription += buf;
    return;

  default:
    int i;
    sprintf(buf, "Unknown Function with %d parameters:\n\n", m_nParameters);
    sDescription += buf;

    for (i=0; i<m_nParameters; i++) {
      sprintf(buf, "Param[%d] = %.8lf\n\n", i, m_params[i]);
      sDescription += buf;
    }
  }
}


/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::SetFunction
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccFormulaCurveSegment::SetFunction(icUInt16Number functionType, icUInt8Number num_parameters, icFloatNumber *parameters)
{
  if (m_params)
    free(m_params);

  if (num_parameters) {
    m_params = (icFloatNumber*)malloc(num_parameters * sizeof(icFloatNumber));
    memcpy(m_params, parameters, num_parameters * sizeof(icFloatNumber));
  }
  else
    m_params = NULL;

  m_nFunctionType = functionType;
  m_nParameters = num_parameters;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccFormulaCurveSegment::Read(icUInt32Number size, CIccIO *pIO)
{
  icCurveSegSignature sig;

  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&m_nFunctionType))
    return false;

  if (!pIO->Read16(&m_nReserved2))
    return false;

  if (m_params) {
    free(m_params);
  }

  switch(m_nFunctionType) {
    case 0x0000:
      m_nParameters = 4;
      break;

    case 0x0001:
    case 0x0002:
      m_nParameters = 5;
      break;

    default:
      return false;
  }

  if (m_nParameters) {

    m_params = (icFloatNumber*)malloc(m_nParameters * sizeof(icFloatNumber));
    if (!m_params)
      return false;

    if (pIO->ReadFloat32Float(m_params, m_nParameters)!= m_nParameters) {
      return false;
    }
  }
  else
    m_params = NULL;

  return true;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccFormulaCurveSegment::Write(CIccIO *pIO)
{
  icCurveSegSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write16(&m_nFunctionType))
    return false;

  if (!pIO->Write16(&m_nReserved2))
    return false;

  switch(m_nFunctionType) {
    case 0x0000:
      if (m_nParameters!=4)
        return false;
      break;

    case 0x0001:
    case 0x0002:
      if (m_nParameters!=5)
        return false;
      break;
  }

  if (m_nParameters) {
    if (pIO->WriteFloat32Float(m_params, m_nParameters)!=m_nParameters)
      return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccFormulaCurveSegment::Begin()
{
  switch (m_nFunctionType) {
  case 0x0000:
    if (!m_params || m_nParameters<4)
      return false;

    return true;

  case 0x0001:
    if (!m_params || m_nParameters<5)
      return false;

    return true;

  case 0x0002:
    if (!m_params || m_nParameters<5)
      return false;
    
    return true;

  default:
    return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icFloatNumber CIccFormulaCurveSegment::Apply(icFloatNumber v)
{
  switch (m_nFunctionType) {
  case 0x0000:
    //Y = (a * X + b) ^ g  + c        : g a b c
    return (pow(m_params[1] * v + m_params[2], m_params[0]) + m_params[3]);

  case 0x0001:
    // Y = a * log (b * X^g + c) + d  : g a b c d
    return (m_params[1] * log10(m_params[2] * pow(v, m_params[0]) + m_params[3]) + m_params[4]);

  case 0x0002:
    //Y = a * b^(c*X+d) + e           : a b c d e
    return (m_params[0] * pow(m_params[1], m_params[2] * v + m_params[3]) + m_params[4]);
  }

  //Shouldn't get here!
  return v;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccFormulaCurveSegment::Validate(icTagSignature sig, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  icValidateStatus rv = icValidateOK;
  if (m_nReserved || m_nReserved2) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " formula curve has non zero reserved data.\n";
    rv = icValidateWarning;
  }

  switch (m_nFunctionType) {
  case 0x0000:
    if (!m_params || m_nParameters<4) {
      sReport += icValidateCriticalErrorMsg;
      sReport += sSigName;
      sReport += " formula curve has Invalid formulaCurveSegment parameters.\n";
      rv = icValidateCriticalError;
    }
    else if (m_nParameters > 4) {
      sReport += icValidateWarningMsg;
      sReport += sSigName;
      sReport += " formula curve has too many formulaCurveSegment parameters.\n";
      rv = icValidateWarning;
    }
    break;

  case 0x0001:
    if (!m_params || m_nParameters<5) {
      sReport += icValidateCriticalErrorMsg;
      sReport += sSigName;
      sReport += " formula curve has Invalid formulaCurveSegment parameters.\n";
      rv = icValidateCriticalError;
    }
    else if (m_nParameters > 5) {
      sReport += icValidateWarningMsg;
      sReport += sSigName;
      sReport += " formula curve has too many formulaCurveSegment parameters.\n";
      rv = icValidateWarning;
    }
    break;

  case 0x0002:
    if (!m_params || m_nParameters<5) {
      sReport += icValidateCriticalErrorMsg;
      sReport += sSigName;
      sReport += " formula curve has Invalid formulaCurveSegment parameters.\n";
      rv = icValidateCriticalError;
    }
    else if (m_nParameters > 5) {
      sReport += icValidateWarningMsg;
      sReport += sSigName;
      sReport += " formula curve has too many formulaCurveSegment parameters.\n";
      rv = icValidateWarning;
    }
    break;

  default:
    {
      icChar buf[128];
      sReport += icValidateCriticalErrorMsg;
      sReport += sSigName;
      sprintf(buf, " formula curve uses unknown formulaCurveSegment function type %d\n", m_nFunctionType);
      sReport += buf;
      rv = icValidateCriticalError;
    }
  }

  return rv;
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::CIccSampledCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSampledCurveSegment::CIccSampledCurveSegment(icFloatNumber start, icFloatNumber end)
{
  m_nReserved = 0;
  m_startPoint = start;
  m_endPoint = end;
  m_nCount = 0;
  m_pSamples = 0;
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::CIccSampledCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSampledCurveSegment::CIccSampledCurveSegment(const CIccSampledCurveSegment &curve)
{
  m_nReserved = curve.m_nReserved;
  m_startPoint = curve.m_startPoint;
  m_endPoint = curve.m_endPoint;
  m_nCount = curve.m_nCount;

  if (m_nCount) {
    m_pSamples = (icFloatNumber*)malloc(m_nCount * sizeof(icFloatNumber));
    if (m_pSamples)
      memcpy(m_pSamples, curve.m_pSamples, m_nCount * sizeof(icFloatNumber));
    else
      m_nCount = 0;
  }
  else{
    m_pSamples = NULL;
  }
}

/**
 ******************************************************************************
 * Name: &CIccSampledCurveSegment::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSampledCurveSegment &CIccSampledCurveSegment::operator=(const CIccSampledCurveSegment &curve)
{
  if (m_pSamples)
    free(m_pSamples);

  m_nReserved = curve.m_nReserved;
  m_startPoint = curve.m_startPoint;
  m_endPoint = curve.m_endPoint;
  m_nCount = curve.m_nCount;

  if (m_nCount) {
    m_pSamples = (icFloatNumber*)malloc(m_nCount * sizeof(icFloatNumber));
    if (m_pSamples)
      memcpy(m_pSamples, curve.m_pSamples, m_nCount * sizeof(icFloatNumber));
    else
      m_nCount = 0;
  }
  else {
    m_pSamples = NULL;
  }
  return (*this);
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::~CIccSampledCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSampledCurveSegment::~CIccSampledCurveSegment()
{
  if (m_pSamples)
    free(m_pSamples);
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::SetSize
 * 
 * Purpose: 
 *  Sets size of sampled lookup table.  Previous data (if exists) is lost.
 * 
 * Args: 
 *  nCount = number of elements in lut (must be >= 2)
 *  bZeroAlloc = flag to decide if memory should be set to zero.
 * 
 * Return: 
 *  true if allocation successful.
 ******************************************************************************/
bool CIccSampledCurveSegment::SetSize(icUInt32Number nCount, bool bZeroAlloc/*=true*/)
{
  if (!nCount) {
    if (m_pSamples)
      free(m_pSamples);
    m_pSamples = NULL;
    m_nCount = nCount;
    return true;
  }

  if (m_pSamples) {
    free(m_pSamples);
  }

  if (bZeroAlloc)
    m_pSamples = (icFloatNumber*)calloc(nCount, sizeof(icFloatNumber));
  else
    m_pSamples = (icFloatNumber*)malloc(nCount * sizeof(icFloatNumber));

  if (m_pSamples)
    m_nCount = nCount;
  else
    m_nCount = 0;

  return (m_pSamples != NULL);
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccSampledCurveSegment::Describe(std::string &sDescription)
{
  icChar buf[128];

  if (m_nCount<2) {
    sDescription += "Empty Segment [";
    if (m_startPoint==icMinFloat32Number)
      sDescription += "-Infinity, ";
    else {
    sprintf(buf, "%.8f, ", m_startPoint);
    sDescription += buf;
    }
    if (m_endPoint==icMaxFloat32Number)
      sDescription += "+Infinity";
    else {
    sprintf(buf, "%.8f", m_endPoint);
    sDescription += buf;
    }

    sprintf(buf, "]\n");
    sDescription += buf;
  }
  else {
    sDescription += "Sampled Segment [";
    if (m_startPoint==icMinFloat32Number)
      sDescription += "-Infinity, ";
    else {
    sprintf(buf, "%.8f, ", m_startPoint);
    sDescription += buf;
    }
    if (m_endPoint==icMaxFloat32Number)
      sDescription += "+Infinity";
    else {
    sprintf(buf, "%.8f", m_endPoint);
    sDescription += buf;
    }
    sprintf(buf, "]\n");
    sDescription += buf;
    sDescription += "IN  OUT\n";

    icUInt32Number i;

    icFloatNumber range = m_endPoint - m_startPoint;
    icFloatNumber last = (icFloatNumber)(m_nCount-1);

    for (i=0; i<m_nCount; i++) {
      sprintf(buf, "%.8f %.8f\n", m_startPoint + (icFloatNumber)i*range/last, m_pSamples[i]);
      sDescription += buf;
    }
  }
  sDescription += "\n";
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSampledCurveSegment::Read(icUInt32Number size, CIccIO *pIO)
{
  icCurveSegSignature sig;

  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
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

  if (!pIO->Read32(&m_nCount))
    return false;

  if (!SetSize(m_nCount, false))
    return false;

  if (m_nCount) {
    if (pIO->ReadFloat32Float(m_pSamples, m_nCount)!=m_nCount)
      return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSampledCurveSegment::Write(CIccIO *pIO)
{
  icCurveSegSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write32(&m_nCount))
    return false;

  if (m_nCount) {
    if (pIO->WriteFloat32Float(m_pSamples, m_nCount)!=m_nCount)
      return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSampledCurveSegment::Begin()
{
  if (m_nCount<2)
    return false;

  m_range = m_endPoint - m_startPoint;
  m_last = (icFloatNumber)(m_nCount - 1);

  if (m_endPoint-m_startPoint == 0.0)
    return false;

  return true;
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icFloatNumber CIccSampledCurveSegment::Apply(icFloatNumber v)
{
  if (v<m_startPoint)
    v=m_startPoint;
  else if (v>m_endPoint)
    v=m_endPoint;

  icFloatNumber pos = (v-m_startPoint)/m_range * m_last;
  icUInt32Number index = (icUInt32Number) pos;
  icFloatNumber remainder = pos - (icFloatNumber)index;

  if (remainder==0.0)
    return m_pSamples[index];

  return (icFloatNumber)((1.0-remainder)*m_pSamples[index] + remainder*m_pSamples[index+1]);
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccSampledCurveSegment::Validate(icTagSignature sig, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  icValidateStatus rv = icValidateOK;
  if (m_nReserved) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " sampled curve has non zero reserved data.\n";
    rv = icValidateWarning;
  }

  if (m_nCount<2) {
    sReport += icValidateCriticalErrorMsg;
    sReport += sSigName;
    sReport += " sampled curve has too few sample points.\n";
    rv = icValidateCriticalError;
  }
  else if (m_endPoint-m_startPoint == 0.0) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " sampled curve has a range of zero.\n";
    rv = icMaxStatus(rv, icValidateWarning);
  }

  return rv;
}


/**
 ******************************************************************************
 * Name: CIccCurveSegment::Create
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccCurveSegment* CIccCurveSegment::Create(icCurveSegSignature sig, icFloatNumber start, icFloatNumber end)
{
  switch(sig) {
  case icSigFormulaCurveSeg:
    return new CIccFormulaCurveSegment(start, end);
  case icSigSampledCurveSeg:
    return new CIccSampledCurveSegment(start, end);
  default:
    return NULL;
  }

}

/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::CIccSegmentedCurve
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSegmentedCurve::CIccSegmentedCurve()
{
  m_list = new CIccCurveSegmentList();
  m_nReserved1 = 0;
  m_nReserved2 = 0;

}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::CIccSegmentedCurve
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSegmentedCurve::CIccSegmentedCurve(const CIccSegmentedCurve &curve)
{
  CIccCurveSegmentList::iterator i;

  m_list = new CIccCurveSegmentList();

  for (i=curve.m_list->begin(); i!=curve.m_list->end(); i++) {
    m_list->push_back((*i)->NewCopy());
  }
  m_nReserved1 = curve.m_nReserved1;
  m_nReserved2 = curve.m_nReserved2;
}


/**
 ******************************************************************************
 * Name: &CIccSegmentedCurve::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSegmentedCurve &CIccSegmentedCurve::operator=(const CIccSegmentedCurve &curve)
{
  Reset();

  CIccCurveSegmentList::iterator i;

  for (i=curve.m_list->begin(); i!=curve.m_list->end(); i++) {
    m_list->push_back((*i)->NewCopy());
  }
  m_nReserved1 = curve.m_nReserved1;
  m_nReserved2 = curve.m_nReserved2;

  return (*this);
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::~CIccSegmentedCurve
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSegmentedCurve::~CIccSegmentedCurve()
{
  Reset();
  delete m_list;
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccSegmentedCurve::Describe(std::string &sDescription)
{
  CIccCurveSegmentList::iterator i;

  sDescription += "BEGIN_CURVE\n";
  for (i=m_list->begin(); i!=m_list->end(); i++) {
    (*i)->Describe(sDescription);
  }
}



/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSegmentedCurve::Read(icUInt32Number size, CIccIO *pIO)
{
  icElemTypeSignature sig;

  icUInt32Number startPos = pIO->Tell();
  
  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved1))
    return false;

  icUInt16Number nSegments;

  if (!pIO->Read16(&nSegments))
    return false;

  if (!pIO->Read16(&m_nReserved2))
    return false;

  Reset();

  icUInt32Number pos = pIO->Tell();
  icCurveSegSignature segSig;
  CIccCurveSegment *pSeg;

  if (nSegments==1) {
    if (!pIO->Read32(&segSig))
      return false;
    pSeg = CIccCurveSegment::Create(segSig, icMinFloat32Number, icMaxFloat32Number);
    if (!pSeg)
      return false;

    pIO->Seek(pos, icSeekSet);

    if (!pSeg->Read(size-(pos-startPos), pIO)) {
      delete pSeg;
      return false;
    }

    m_list->push_back(pSeg);
  }
  else if (nSegments) {
    icFloatNumber *breakpoints=(icFloatNumber*)calloc(nSegments-1, sizeof(icFloatNumber));

    if (!breakpoints)
      return false;

    if (pIO->ReadFloat32Float(breakpoints, nSegments-1)!=nSegments-1) {
      free(breakpoints);
      return false;
    }

    int i;
    for (i=0; i<nSegments; i++) {
      pos = pIO->Tell();
      if (!pIO->Read32(&segSig)) {
        free(breakpoints);
        return false;
      }
      if (pIO->Seek(pos, icSeekSet)!=pos)
        return false;;

      if (!i)
        pSeg = CIccCurveSegment::Create(segSig, icMinFloat32Number, breakpoints[i]);
      else if (i==nSegments-1) 
        pSeg = CIccCurveSegment::Create(segSig, breakpoints[i-1], icMaxFloat32Number);
      else
        pSeg = CIccCurveSegment::Create(segSig, breakpoints[i-1], breakpoints[i]);
      
      if (!pSeg) {
        free(breakpoints);
        return false;
      }

      if (!pSeg->Read(size-(pos-startPos), pIO)) {
        delete pSeg;
        free(breakpoints);
        return false;
      }

      m_list->push_back(pSeg);
    }

    free(breakpoints);
  }

  return true;
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSegmentedCurve::Write(CIccIO *pIO)
{
  icCurveElemSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved1))
    return false;

  icUInt16Number nSegments = m_list->size();

  if (!pIO->Write16(&nSegments))
    return false;

  if (!pIO->Write16(&m_nReserved2))
    return false;

  CIccCurveSegmentList::iterator i;
  if (nSegments>1) {
    icFloatNumber breakpoint;
    
    i=m_list->begin();
    for (i++; i!=m_list->end(); i++) {
      breakpoint = (*i)->StartPoint();
      if (!pIO->WriteFloat32Float(&breakpoint))
        return false;
    }
  }
  for (i=m_list->begin(); i!=m_list->end(); i++) {
    if (!(*i)->Write(pIO))
      return false;
  }
  
  return true;
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Reset
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccSegmentedCurve::Reset()
{
  CIccCurveSegmentList::iterator i;

  for (i=m_list->begin(); i!=m_list->end(); i++) {
    delete (*i);
  }
  m_list->clear();
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Insert
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSegmentedCurve::Insert(CIccCurveSegment *pCurveSegment)
{
  CIccCurveSegmentList::reverse_iterator last = m_list->rbegin();

  if (last!=m_list->rend()) {
    if (pCurveSegment->StartPoint() == (*last)->EndPoint()) {
      m_list->push_back(pCurveSegment);
      return true;
    }
  }
  else {
    m_list->push_back(pCurveSegment);
    return true;
  }

  return false;
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSegmentedCurve::Begin()
{
  if (m_list->size()==0)
    return false;

 CIccCurveSegmentList::iterator i;

  for (i=m_list->begin(); i!=m_list->end(); i++) {
    if (!(*i)->Begin())
      return false;
  }

  return true;
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icFloatNumber CIccSegmentedCurve::Apply(icFloatNumber v)
{
 CIccCurveSegmentList::iterator i;

  for (i=m_list->begin(); i!=m_list->end(); i++) {
    if (v <= (*i)->EndPoint())
      return (*i)->Apply(v);
  }
  return v;
}

/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccSegmentedCurve::Validate(icTagSignature sig, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  icValidateStatus rv = icValidateOK;
  if (m_nReserved1 || m_nReserved2) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " Segmented curve has non zero reserved data.\n";
    rv = icValidateWarning;
  }

  if (m_list->size()==0) {
    sReport += icValidateCriticalErrorMsg;
    sReport += sSigName;
    sReport += " Has Empty CurveSegment!\n";
    return icValidateCriticalError;
  }

  CIccCurveSegmentList::iterator i;

  for (i=m_list->begin(); i!=m_list->end(); i++) {
    rv = icMaxStatus(rv, (*i)->Validate(sig, sReport, pMPE));
  }

  return rv;
}


/**
 ******************************************************************************
 * Name: CIccCurveSetCurve::Create
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccCurveSetCurve* CIccCurveSetCurve::Create(icCurveElemSignature sig)
{
  switch (sig) {
  case icSigSementedCurve:
    return new CIccSegmentedCurve();
  default:
    return NULL;
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::CIccMpeCurveSet
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCurveSet::CIccMpeCurveSet(int nSize/*=0*/)
{
  m_nReserved = 0;
  if (nSize) {
    m_nInputChannels = m_nOutputChannels = nSize;
    m_curve = (icCurveSetCurvePtr*)calloc(nSize, sizeof(icCurveSetCurvePtr));
    m_position = (icPositionNumber*)calloc(nSize, sizeof(icPositionNumber));
  }
  else {
    m_nInputChannels = m_nOutputChannels = 0;
    m_curve = NULL;
    m_position = NULL;
  }
}

typedef std::map<icCurveSetCurvePtr, icCurveSetCurvePtr> icCurveMap;

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::CIccMpeCurveSet
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCurveSet::CIccMpeCurveSet(const CIccMpeCurveSet &curveSet)
{
  m_nReserved = curveSet.m_nReserved;

  if (curveSet.m_nInputChannels) {
    int i;

    m_nInputChannels = m_nOutputChannels = curveSet.m_nInputChannels;
    m_curve = (icCurveSetCurvePtr*)calloc(m_nInputChannels, sizeof(icCurveSetCurvePtr));
    m_position = (icPositionNumber*)calloc(m_nInputChannels, sizeof(icPositionNumber));

    icCurveMap map;
    for (i=0; i<m_nInputChannels; i++) {
      CIccCurveSetCurve *ptr = curveSet.m_curve[i];
      if (ptr) {
        if (!map[ptr]) {
          m_curve[i] = ptr->NewCopy();
          map[ptr] = m_curve[i];
        }
        else
          m_curve[i] = map[ptr];
      }
    }
  }
  else {
    m_nInputChannels = m_nOutputChannels = 0;
    m_curve = NULL;
  }
}

/**
 ******************************************************************************
 * Name: &CIccMpeCurveSet::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCurveSet &CIccMpeCurveSet::operator=(const CIccMpeCurveSet &curveSet)
{
  m_nReserved = m_nReserved;

  if (m_curve) {
    delete [] m_curve;
  }

  if (curveSet.m_nInputChannels) {
    int i;

    m_nInputChannels = m_nOutputChannels = curveSet.m_nInputChannels;
    m_curve = (icCurveSetCurvePtr*)calloc(m_nInputChannels, sizeof(icCurveSetCurvePtr));
    m_position = (icPositionNumber*)calloc(m_nInputChannels, sizeof(icPositionNumber));

    icCurveMap map;
    for (i=0; i<m_nInputChannels; i++) {
      CIccCurveSetCurve *ptr = curveSet.m_curve[i];
      if (ptr) {
        if (!map[ptr]) {
          m_curve[i] = ptr->NewCopy();
          map[ptr] = m_curve[i];
        }
        else
          m_curve[i] = map[ptr];
      }
    }
  }
  else {
    m_nInputChannels = m_nOutputChannels = 0;
    m_curve = NULL;
  }

  return *this;
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::~CIccMpeCurveSet
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCurveSet::~CIccMpeCurveSet()
{
  SetSize(0);
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::SetSize
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeCurveSet::SetSize(int nNewSize)
{
  if (m_curve) {
    icCurveMap map;
    int i;

    for (i=0; i<m_nInputChannels; i++) {
      if (!map[m_curve[i]]) {
        map[m_curve[i]] = (CIccCurveSetCurve*)1;
        delete m_curve[i];
      }
    }
    free(m_curve);
  }
  if (m_position) {
    free(m_position);
  }

  if (nNewSize) {
    m_curve = (icCurveSetCurvePtr*)calloc(nNewSize, sizeof(icCurveSetCurvePtr));
    m_position = (icPositionNumber*)calloc(nNewSize, sizeof(icPositionNumber));

    m_nInputChannels = m_nOutputChannels = nNewSize;
  }
  else {
    m_curve = NULL;
    m_nInputChannels = m_nOutputChannels = 0;
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::SetCurve
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCurveSet::SetCurve(int nIndex, icCurveSetCurvePtr newCurve)
{
  if (nIndex<0 || nIndex>m_nInputChannels)
    return false;

  if (m_curve) {
    int i;

    for (i=0; i<m_nInputChannels; i++)
      if (i!=nIndex && m_curve[i]==m_curve[nIndex])
        break;

    if (i==m_nInputChannels && m_curve[nIndex]) {
      delete m_curve[nIndex];
    }
  }
  m_curve[nIndex] = newCurve;
  
  return true;
}


/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeCurveSet::Describe(std::string &sDescription)
{
  if (m_curve) {
    icChar buf[81];
    int i;

    sprintf(buf, "BEGIN_CURVE_SET %d\n", m_nInputChannels);
    sDescription += buf;

    for (i=0; i<m_nInputChannels; i++) {
      sprintf(buf, "Curve %d of %d\n", i+1, m_nInputChannels);
      sDescription += buf;
      if (m_curve[i]) {
        m_curve[i]->Describe(sDescription);
      }
    }
  }
}

typedef std::map<icUInt32Number, CIccCurveSetCurve*> icCurveOffsetMap;
typedef std::map<CIccCurveSetCurve*, icPositionNumber> icCurvePtrMap;

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCurveSet::Read(icUInt32Number size, CIccIO *pIO)
{
  icElemTypeSignature sig;

  icUInt32Number startPos = pIO->Tell();
  
  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  icUInt16Number nInputChannels, nOutputChannels;

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&nInputChannels))
    return false;

  if (!pIO->Read16(&nOutputChannels))
    return false;

  if (nInputChannels != nOutputChannels)
    return false;

  SetSize(nInputChannels);

  if (m_curve) {
    int i;

    if (headerSize + m_nInputChannels*2*sizeof(icUInt32Number) > size)
      return false;

    for (i=0; i<m_nInputChannels; i++) {
      if (!pIO->Read32(&m_position[i].offset)) {
        return false;
      }
      if (!pIO->Read32(&m_position[i].size)) {
        return false;
      }
    }

    icCurveOffsetMap map;
    icCurveElemSignature curveSig;
    for (i=0; i<m_nInputChannels; i++) {
      if (!map[m_position[i].offset]) {
        icUInt32Number pos;
        if (!m_position[i].offset || !m_position[i].size) {
          return false;
        }

        pos = startPos + m_position[i].offset;
        if (pIO->Seek(pos, icSeekSet)!=pos) {
          return false;
        }
        
        if (!pIO->Read32(&curveSig)) {
          return false;
        }
        m_curve[i] = CIccCurveSetCurve::Create(curveSig);

        if (!m_curve[i]) {
          return false;
        }

        if (pIO->Seek(pos, icSeekSet)!=pos) {
          return false;
        }
      
        if (!m_curve[i]->Read(m_position[i].size, pIO)) {
          return false;
        }

        map[m_position[i].offset] = m_curve[i];
      }
      else {
        m_curve[i] = map[m_position[i].offset];
      }
    }
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCurveSet::Write(CIccIO *pIO)
{
  icElemTypeSignature sig = GetType();

  if (!pIO)
    return false;

  icUInt32Number elemStart = pIO->Tell();

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write16(&m_nInputChannels))
    return false;

  if (!pIO->Write16(&m_nInputChannels))
    return false;

  if (m_curve && m_nInputChannels) {
    int i;
    icCurvePtrMap map;
    icUInt32Number start, end;
    icUInt32Number zeros[2] = { 0, 0};
    icPositionNumber position;

    icUInt32Number startTable = pIO->Tell();

    //First write empty position table
    for (i=0; i<m_nInputChannels; i++) {
      if (pIO->Write32(&zeros[0], 2)!=2)
        return false;
    }

    //Now Write curves
    for (i=0; i<m_nInputChannels; i++) {
      if (m_curve[i]) {
        if (map.find(m_curve[i])==map.end()) {
          start = pIO->Tell();
          m_curve[i]->Write(pIO);
          end = pIO->Tell();
          pIO->Sync32();
          position.offset = start - elemStart;
          position.size = end - start;
          map[m_curve[i]] = position;
        }
        m_position[i] = map[m_curve[i]];
      }
    }
    end = pIO->Tell();
    
    //Back fill position table
    pIO->Seek(startTable, icSeekSet);
    for (i=0; i<m_nInputChannels; i++) {
      if (!pIO->Write32(&m_position[i].offset))
        return false;
      if (!pIO->Write32(&m_position[i].size))
        return false;
    }

    pIO->Seek(end, icSeekSet);
  }
  
  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCurveSet::Begin(icElemInterp nInterp, CIccTagMultiProcessElement *pMPE)
{
  if (!m_curve)
    return false;

  int i;
  for (i=0; i<m_nInputChannels; i++) {
    if (!m_curve[i] || !m_curve[i]->Begin())
      return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeCurveSet::Apply(icFloatNumber *pDestPixel, const icFloatNumber *pSrcPixel)
{
  int i;
  for (i=0; i<m_nInputChannels; i++) {
    *pDestPixel++ = m_curve[i]->Apply(*pSrcPixel++);
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccMpeCurveSet::Validate(icTagSignature sig, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  icValidateStatus rv = CIccMultiProcessElement::Validate(sig, sReport, pMPE);

  bool empty=false;
  if (m_curve) {
    int i;
    for (i=0; !empty && i<m_nInputChannels; i++) {
      if (!m_curve[i]) {
        empty = true;
      }
      else {
        rv = icMaxStatus(rv, m_curve[i]->Validate(sig, sReport, pMPE));
      }
    }
  }
  else
    empty = true;
  
  if (empty) {
    CIccInfo Info;
    std::string sSigName = Info.GetSigName(sig);

    sReport += icValidateCriticalErrorMsg;
    sReport += sSigName;
    sReport += " - Element ";
    sSigName = Info.GetSigName(GetType());
    sReport += sSigName;
    sReport += " Has Empty Curve Element(s)!\n";
    return icValidateCriticalError;
  }

  return rv;
}


/**
 ******************************************************************************
 * Name: CIccMpeMatrix::CIccMpeMatrix
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeMatrix::CIccMpeMatrix()
{
  m_nReserved = 0;
  m_nInputChannels = m_nOutputChannels = 0;
  m_size = 0;
  m_pMatrix = NULL;
  m_pConstants = NULL;
}


/**
 ******************************************************************************
 * Name: CIccMpeMatrix::CIccMpeMatrix
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeMatrix::CIccMpeMatrix(const CIccMpeMatrix &matrix)
{
  m_nReserved = matrix.m_nReserved;

  m_nInputChannels = matrix.m_nInputChannels;
  m_nOutputChannels = matrix.m_nOutputChannels;

  m_size = matrix.m_size;
  if(matrix.m_pMatrix) {
    int num = m_size * sizeof(icFloatNumber);
    m_pMatrix = (icFloatNumber*)malloc(num);
    memcpy(m_pMatrix, matrix.m_pMatrix, num);
  }
  else
    m_pMatrix = NULL;

  if (matrix.m_pConstants) {
    int num = m_nOutputChannels*sizeof(icFloatNumber);
    m_pConstants = (icFloatNumber*)malloc(num);
    memcpy(m_pConstants, matrix.m_pConstants, num);
  }
  else
    m_pConstants = NULL;
}

/**
 ******************************************************************************
 * Name: &CIccMpeMatrix::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeMatrix &CIccMpeMatrix::operator=(const CIccMpeMatrix &matrix)
{
  m_nReserved = matrix.m_nReserved;

  m_nInputChannels = matrix.m_nInputChannels;
  m_nOutputChannels = matrix.m_nOutputChannels;

  if (m_pMatrix)
    free(m_pMatrix);

  m_size = matrix.m_size;
  if (matrix.m_pMatrix) {
    int num = m_size * sizeof(icFloatNumber);
    m_pMatrix = (icFloatNumber*)malloc(num);
    memcpy(m_pMatrix, matrix.m_pMatrix, num);
  }
  else
    m_pMatrix = NULL;

  if (m_pConstants)
    free(m_pConstants);

  if (matrix.m_pConstants) {
    int num = m_nOutputChannels*sizeof(icFloatNumber);
    m_pConstants = (icFloatNumber*)malloc(num);
    memcpy(m_pConstants, matrix.m_pConstants, num);
  }
  else
    m_pConstants = NULL;

  return *this;
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::~CIccMpeMatrix
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeMatrix::~CIccMpeMatrix()
{
  if (m_pMatrix)
    free(m_pMatrix);

  if (m_pConstants)
    free(m_pConstants);
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::SetSize
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeMatrix::SetSize(icUInt16Number nInputChannels, icUInt16Number nOutputChannels)
{
  if (m_pMatrix)
    free(m_pMatrix);

  m_size = (icUInt32Number)nInputChannels * nOutputChannels;

  m_pMatrix = (icFloatNumber*)calloc(m_size, sizeof(icFloatNumber));
  m_pConstants = (icFloatNumber*)calloc(nOutputChannels, sizeof(icFloatNumber));

  m_nInputChannels = nInputChannels;
  m_nOutputChannels = nOutputChannels;
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeMatrix::Describe(std::string &sDescription)
{
  icChar buf[81];
  int i, j;
  icFloatNumber *data = m_pMatrix;

  sprintf(buf, "BEGIN_ELEM_MATRIX %d %d\n", m_nInputChannels, m_nOutputChannels);
  sDescription += buf;

  for (j=0; j<m_nOutputChannels; j++) {
    for (i=0; i<m_nInputChannels; i++) {
      if (i)
        sDescription += " ";
      sprintf(buf, "%12.8lf", data[i]);
      sDescription += buf;
    }
    sprintf(buf, "  +  %12.8lf\n", m_pConstants[j]);
    sDescription += buf;
    data += i;
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeMatrix::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  
  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt8Number) + 
    sizeof(icUInt8Number) + 
    sizeof(icUInt16Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  icUInt16Number nInputChannels, nOutputChannels;

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&nInputChannels))
    return false;

  if (!pIO->Read16(&nOutputChannels))
    return false;

  SetSize(nInputChannels, nOutputChannels);

  if (!m_pMatrix) 
    return false;

  if (headerSize + m_size*sizeof(icFloat32Number) > size)
    return false;
  
  //Read Matrix data
  if (pIO->ReadFloat32Float(m_pMatrix, m_size)!=m_size)
    return false;

  //Read Constant data
  if (pIO->ReadFloat32Float(m_pConstants, m_nOutputChannels)!=m_nOutputChannels)
    return false;
  
  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeMatrix::Write(CIccIO *pIO)
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

  if (m_pMatrix) {
    if (pIO->WriteFloat32Float(m_pMatrix, m_size)!=m_size)
      return false;
  }

  //Write Constant data
  if (m_pConstants) {
    if (pIO->WriteFloat32Float(m_pConstants, m_nOutputChannels)!=m_nOutputChannels)
      return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeMatrix::Begin(icElemInterp nInterp, CIccTagMultiProcessElement *pMPE)
{
  if (!m_pMatrix || !m_pConstants)
    return false;

  if (m_nInputChannels==3 && m_nOutputChannels==3)
    m_type = ic3x3Matrix;
  else if (m_nInputChannels==3 && m_nOutputChannels==4)
    m_type = ic3x4Matrix;
  else if (m_nInputChannels==4 && m_nOutputChannels==3)
    m_type = ic4x3Matrix;
  else if (m_nInputChannels==4 && m_nOutputChannels==4)
    m_type = ic4x4Matrix;
  else
    m_type = icOtherMatrix;

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeMatrix::Apply(icFloatNumber *dstPixel, const icFloatNumber *srcPixel)
{
  icFloatNumber *data = m_pMatrix;
  switch (m_type) {
    case ic3x3Matrix:
      *dstPixel++ = data[ 0]*srcPixel[0] + data[ 1]*srcPixel[1] + data[ 2]*srcPixel[2] + m_pConstants[0];
      *dstPixel++ = data[ 3]*srcPixel[0] + data[ 4]*srcPixel[1] + data[ 5]*srcPixel[2] + m_pConstants[1];
      *dstPixel   = data[ 6]*srcPixel[0] + data[ 7]*srcPixel[1] + data[ 8]*srcPixel[2] + m_pConstants[2];
      break;

    case ic3x4Matrix:
      *dstPixel++ = data[ 0]*srcPixel[0] + data[ 1]*srcPixel[1] + data[ 2]*srcPixel[2] + m_pConstants[0];
      *dstPixel++ = data[ 3]*srcPixel[0] + data[ 4]*srcPixel[1] + data[ 5]*srcPixel[2] + m_pConstants[1];
      *dstPixel++ = data[ 6]*srcPixel[0] + data[ 7]*srcPixel[1] + data[ 8]*srcPixel[2] + m_pConstants[2];
      *dstPixel   = data[ 9]*srcPixel[0] + data[10]*srcPixel[1] + data[11]*srcPixel[2] + m_pConstants[3];
      break;

    case ic4x3Matrix:
      *dstPixel++ = data[ 0]*srcPixel[0] + data[ 1]*srcPixel[1] + data[ 2]*srcPixel[2] + data[ 3]*srcPixel[3] + m_pConstants[0];
      *dstPixel++ = data[ 4]*srcPixel[0] + data[ 5]*srcPixel[1] + data[ 6]*srcPixel[2] + data[ 7]*srcPixel[3] + m_pConstants[1];
      *dstPixel   = data[ 8]*srcPixel[0] + data[ 9]*srcPixel[1] + data[10]*srcPixel[2] + data[11]*srcPixel[3] + m_pConstants[2];
      break;

    case ic4x4Matrix:
      *dstPixel++ = data[ 0]*srcPixel[0] + data[ 1]*srcPixel[1] + data[ 2]*srcPixel[2] + data[ 3]*srcPixel[3] + m_pConstants[0];
      *dstPixel++ = data[ 4]*srcPixel[0] + data[ 5]*srcPixel[1] + data[ 6]*srcPixel[2] + data[ 7]*srcPixel[3] + m_pConstants[1];
      *dstPixel++ = data[ 8]*srcPixel[0] + data[ 9]*srcPixel[1] + data[10]*srcPixel[2] + data[11]*srcPixel[3] + m_pConstants[2];
      *dstPixel   = data[12]*srcPixel[0] + data[13]*srcPixel[1] + data[14]*srcPixel[2] + data[15]*srcPixel[3] + m_pConstants[3];
      break;
    
    case icOtherMatrix:
    default:
      {
        int i, j;

        for (j=0; j<m_nOutputChannels; j++) {
          *dstPixel = m_pConstants[j];

          for (i=0; i<m_nInputChannels; i++) {
            *dstPixel += data[i]*srcPixel[i];
          }

          data += i;
          dstPixel++;
        }
      }
      break;
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccMpeMatrix::Validate(icTagSignature sig, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  icValidateStatus rv = CIccMultiProcessElement::Validate(sig, sReport, pMPE);

  if (!m_pMatrix || !m_pConstants) {
    CIccInfo Info;
    std::string sSigName = Info.GetSigName(sig);

    sReport += icValidateCriticalErrorMsg;
    sReport += sSigName;
    sReport += " - Element ";
    sSigName = Info.GetSigName(GetType());
    sReport += sSigName;
    sReport += " Has Empty Matrix data!\n";
    return icValidateCriticalError;
  }

  return rv;
}


static icFloatNumber NoClip(icFloatNumber v)
{
  return v;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::CIccMpeCLUT
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCLUT::CIccMpeCLUT()
{
  m_pCLUT = NULL;
  m_nInputChannels = 0;
  m_nOutputChannels = 0;

  m_nReserved = 0;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::CIccMpeCLUT
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCLUT::CIccMpeCLUT(const CIccMpeCLUT &clut)
{
  if (clut.m_pCLUT)
    m_pCLUT = new CIccCLUT(*clut.m_pCLUT);
  else
    m_pCLUT = NULL;

  m_nReserved = clut.m_nReserved;
  m_nInputChannels = clut.m_nInputChannels;
  m_nOutputChannels = clut.m_nOutputChannels;
}

/**
 ******************************************************************************
 * Name: &CIccMpeCLUT::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCLUT &CIccMpeCLUT::operator=(const CIccMpeCLUT &clut)
{
  if (m_pCLUT)
    delete m_pCLUT;

  if (clut.m_pCLUT)
    m_pCLUT = new CIccCLUT(*clut.m_pCLUT);
  else
    m_pCLUT = NULL;

  m_nReserved = clut.m_nReserved;
  m_nInputChannels = clut.m_nInputChannels;
  m_nOutputChannels = clut.m_nOutputChannels;

  return *this;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::~CIccMpeCLUT
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCLUT::~CIccMpeCLUT()
{
  if (m_pCLUT)
    delete m_pCLUT;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::SetCLUT
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeCLUT::SetCLUT(CIccCLUT *pCLUT)
{
  if (m_pCLUT)
    delete m_pCLUT;

  m_pCLUT = pCLUT;
  if (pCLUT) {
    pCLUT->SetClipFunc(NoClip);
    m_nInputChannels = pCLUT->GetInputDim();
    m_nOutputChannels = pCLUT->GetOutputChannels();
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeCLUT::Describe(std::string &sDescription)
{
  if (m_pCLUT) {
    m_pCLUT->DumpLut(sDescription, "ELEM_CLUT", icSigUnknownData, icSigUnknownData);
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCLUT::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  
  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number) + 
    16 * sizeof(icUInt8Number);

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

  icUInt8Number gridPoints[16];

  if (pIO->Read8(gridPoints, 16)!=16) {
    return false;
  }

  m_pCLUT = new CIccCLUT((icUInt8Number)m_nInputChannels, (icUInt8Number)m_nOutputChannels, 4);

  if (!m_pCLUT)
    return false;

  m_pCLUT->SetClipFunc(NoClip);

  m_pCLUT->Init(gridPoints);

  icFloatNumber *pData = m_pCLUT->GetData(0);

  if (!pData)
    return false;

  icUInt32Number nPoints = m_pCLUT->NumPoints()*m_nOutputChannels;

  if (pIO->ReadFloat32Float(pData,nPoints)!= nPoints)
    return false;
  
  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCLUT::Write(CIccIO *pIO)
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

  if (m_pCLUT) {
    icUInt8Number gridPoints[16];
    int i;

    for (i=0; i<16; i++)
      gridPoints[i] = m_pCLUT->GridPoint(i);

    if (pIO->Write8(gridPoints, 16)!=16)
      return false;

    icFloatNumber *pData = m_pCLUT->GetData(0);
    icUInt32Number nPoints = m_pCLUT->NumPoints()*m_nOutputChannels;

    if (pIO->WriteFloat32Float(pData, nPoints) != nPoints) 
      return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCLUT::Begin(icElemInterp nInterp, CIccTagMultiProcessElement *pMPE)
{
  if (!m_pCLUT)
    return false;

  m_pCLUT->Begin();

  switch (m_nInputChannels) {
  case 3:
    if (nInterp==icElemInterpTetra)
      m_interpType = ic3dInterpTetra;
    else
      m_interpType = ic3dInterp;
    break;
  case 4:
    m_interpType = ic4dInterp;
    break;
  case 5:
    m_interpType = ic5dInterp;
    break;
  case 6:
    m_interpType = ic6dInterp;
    break;
  default:
    m_interpType = icNdInterp;
    break;
  }
  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeCLUT::Apply(icFloatNumber *dstPixel, const icFloatNumber *srcPixel)
{
  switch(m_interpType) {
  case ic3dInterp:
    m_pCLUT->Interp3d(dstPixel, srcPixel);
    break;
  case ic4dInterp:
    m_pCLUT->Interp4d(dstPixel, srcPixel);
    break;
  case ic5dInterp:
    m_pCLUT->Interp5d(dstPixel, srcPixel);
    break;
  case ic6dInterp:
    m_pCLUT->Interp6d(dstPixel, srcPixel);
    break;
  case icNdInterp:
    m_pCLUT->InterpND(dstPixel, srcPixel);
    break;
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccMpeCLUT::Validate(icTagSignature sig, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  icValidateStatus rv = CIccMultiProcessElement::Validate(sig, sReport, pMPE);

  if (!m_pCLUT) {
    CIccInfo Info;
    std::string sSigName = Info.GetSigName(sig);

    sReport += icValidateCriticalErrorMsg;
    sReport += sSigName;
    sReport += " - Element ";
    sSigName = Info.GetSigName(GetType());
    sReport += sSigName;
    sReport += " Has No CLUT!\n";
    return icValidateCriticalError;
  }

  return rv;

}

#ifdef USESAMPLEICCNAMESPACE
} //namespace sampleICC
#endif
