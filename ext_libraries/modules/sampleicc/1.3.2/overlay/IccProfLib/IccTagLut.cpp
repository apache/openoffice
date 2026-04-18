/** @file
    File:       IccTagLut.cpp

    Contains:   Implementation of the Lut Tag classes

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
// -Moved LUT tags to separate file 4-30-2005
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

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

/**
****************************************************************************
* Name: CIccCurve::Find
* 
* Purpose: Read in the tag contents into a data block
* 
* Args: 
*  v = index to be searched,
*  v0 = index less than/equal to v,
*  p0 = the value at index v0,
*  v1 = index greater than/equal to v,
*  p1 = value at index v1
* 
* Return: The value at the requested index 
*  
*****************************************************************************
*/
icFloatNumber CIccCurve::Find(icFloatNumber v,
  icFloatNumber p0, icFloatNumber v0,
  icFloatNumber p1, icFloatNumber v1)
{
  if (v<=v0)
    return p0;
  if (v>=v1)
    return p1;

  if (p1-p0 <= 0.00001) {
    icFloatNumber d0 = fabs(v-v0);
    icFloatNumber d1 = fabs(v1-v);

    if (d0<d1)
      return p0;
    return p1;
  }

  icFloatNumber np = (icFloatNumber)((p0 + p1)/2.0);
  icFloatNumber nv = Apply(np);

  if (v<=nv) {
    return Find(v, p0, v0, np, nv);
  }
  return Find(v, np, nv, p1, v1);
}


/**
****************************************************************************
* Name: CIccTagCurve::CIccTagCurve
* 
* Purpose: Constructor
* 
*****************************************************************************
*/
CIccTagCurve::CIccTagCurve(int nSize/*=0*/)
{
  m_nSize = nSize;
  if (m_nSize <0)
    m_nSize = 0;
  if (m_nSize>0)
    m_Curve = (icFloatNumber*)calloc(nSize, sizeof(icFloatNumber));
  else
    m_Curve = NULL;
}


/**
****************************************************************************
* Name: CIccTagCurve::CIccTagCurve
* 
* Purpose: Copy Constructor
*
* Args:
*  ITCurve = The CIccTagCurve object to be copied
*****************************************************************************
*/
CIccTagCurve::CIccTagCurve(const CIccTagCurve &ITCurve)
{
  m_nSize = ITCurve.m_nSize;
  m_nMaxIndex = ITCurve.m_nMaxIndex;

  m_Curve = (icFloatNumber*)calloc(m_nSize, sizeof(icFloatNumber));
  memcpy(m_Curve, ITCurve.m_Curve, m_nSize*sizeof(icFloatNumber));
}


/**
****************************************************************************
* Name: CIccTagCurve::operator=
* 
* Purpose: Copy Operator
*
* Args:
*  CurveTag = The CIccTagCurve object to be copied
*****************************************************************************
*/
CIccTagCurve &CIccTagCurve::operator=(const CIccTagCurve &CurveTag)
{
  if (&CurveTag == this)
    return *this;

  m_nSize = CurveTag.m_nSize;
  m_nMaxIndex = CurveTag.m_nMaxIndex;

  if (m_Curve)
    free(m_Curve);
  m_Curve = (icFloatNumber*)calloc(m_nSize, sizeof(icFloatNumber));
  memcpy(m_Curve, CurveTag.m_Curve, m_nSize*sizeof(icFloatNumber));

  return *this;
}


/**
****************************************************************************
* Name: CIccTagCurve::~CIccTagCurve
* 
* Purpose: Destructor
* 
*****************************************************************************
*/
CIccTagCurve::~CIccTagCurve()
{
  if (m_Curve)
    free(m_Curve);
}


/**
****************************************************************************
* Name: CIccTagCurve::Read
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
bool CIccTagCurve::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

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

  icUInt32Number nSize;

  if (!pIO->Read32(&nSize))
    return false;

  SetSize(nSize, icInitNone);

  if (m_nSize) {
    if (pIO->Read16Float(m_Curve, m_nSize)!=m_nSize)
      return false;
  }

  return true;
}


/**
****************************************************************************
* Name: CIccTagCurve::Write
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
bool CIccTagCurve::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write32(&m_nSize))
    return false;

  if (m_nSize)
    if (pIO->Write16Float(m_Curve, m_nSize)!=m_nSize)
      return false;

  pIO->Align32();

  return true;
}


/**
****************************************************************************
* Name: CIccTagCurve::Describe
* 
* Purpose: Dump data associated with the tag to a string
* 
* Args: 
*  sDescription - string to concatenate tag dump to
*****************************************************************************
*/
void CIccTagCurve::Describe(std::string &sDescription)
{
  icChar buf[128], *ptr;

  if (!m_nSize) {
    sprintf(buf, "BEGIN_CURVE In_Out\n");
    sDescription += buf;
    sDescription += "Y = X\n";
  }
  else if (m_nSize==1) {
    icFloatNumber dGamma = (icFloatNumber)(m_Curve[0] * 256.0);
    sprintf(buf, "BEGIN_CURVE In_Out\n");
    sDescription += buf;
    sprintf(buf, "Y = X ^ %.4lf\n", dGamma);
    sDescription += buf;
  }
  else {
    int i;

    sprintf(buf, "BEGIN_LUT In_Out 1 1\n");
    sDescription += buf;
    sDescription += "IN OUT\n";

    for (i=0; i<(int)m_nSize; i++) {
      ptr = buf;

      icColorValue(buf, (icFloatNumber)i/(m_nSize-1), icSigMCH1Data, 1);
      ptr += strlen(buf);

      strcpy(ptr, " ");
      ptr ++;

      icColorValue(ptr, m_Curve[i], icSigMCH1Data, 1);

      ptr += strlen(ptr);

      strcpy(ptr, "\n");

      sDescription += buf;
    }
  }
  sDescription += "\n";
}


/**
****************************************************************************
* Name: CIccTagCurve::DumpLut
* 
* Purpose: Dump data associated with the tag to a string. Basically has 
*  the same function as Describe()
* 
* Args: 
*  sDescription = string to concatenate tag dump to,
*  szName = name of the curve to be printed,
*  csSig = color space signature of the LUT data,
*  nIndex = the channel number of color space
*****************************************************************************
*/
void CIccTagCurve::DumpLut(std::string &sDescription, const icChar *szName,
  icColorSpaceSignature csSig, int nIndex)
{
  icChar buf[128], *ptr;

  if (!m_nSize) {
    sprintf(buf, "BEGIN_CURVE %s\n", szName);
    sDescription += buf;
    sDescription += "Y = X\n";
  }
  else if (m_nSize==1) {
    icFloatNumber dGamma = (icFloatNumber)(m_Curve[0] * 256.0);
    sprintf(buf, "BEGIN_CURVE %s\n", szName);
    sDescription += buf;
    sprintf(buf, "Y = X ^ %.4lf\n", dGamma);
    sDescription += buf;
  }
  else {
    int i;

    sprintf(buf, "BEGIN_LUT %s 1 1\n", szName);
    sDescription += buf;
    sDescription += "IN OUT\n";

    sDescription.reserve(sDescription.size() + m_nSize * 20);

    for (i=0; i<(int)m_nSize; i++) {
      ptr = buf;

      icColorValue(buf, (icFloatNumber)i/(m_nSize-1), csSig, nIndex);
      ptr += strlen(buf);

      strcpy(ptr, " ");
      ptr ++;

      icColorValue(ptr, m_Curve[i], csSig, nIndex);

      ptr += strlen(ptr);

      strcpy(ptr, "\n");

      sDescription += buf;
    }
  }
  sDescription += "\n";
}


/**
****************************************************************************
* Name: CIccTagCurve::SetSize
* 
* Purpose: Sets the size of the curve array.
* 
* Args: 
*  nSize - number of entries in the curve,
*  nSizeOpt - flag to zero newly formed values
*****************************************************************************
*/
void CIccTagCurve::SetSize(icUInt32Number nSize, icTagCurveSizeInit nSizeOpt/*=icInitZero*/)
{
  if (nSize==m_nSize)
    return;

  if (!nSize && m_Curve) {
    free(m_Curve);
    m_Curve = NULL;
  }
  else {
    if (!m_Curve)
      m_Curve = (icFloatNumber*)malloc(nSize*sizeof(icFloatNumber));
    else
      m_Curve = (icFloatNumber*)realloc(m_Curve, nSize*sizeof(icFloatNumber));

    switch (nSizeOpt) {
    case icInitNone:
    default:
      break;

    case icInitZero:
      if (m_nSize < nSize) {
        memset(&m_Curve[m_nSize], 0, (nSize-m_nSize)*sizeof(icFloatNumber));
      }
      break;

    case icInitIdentity:
      if (nSize>1) {
        icUInt32Number i;
        icFloatNumber last = (icFloatNumber)(nSize-1);

        for (i=0; i<nSize; i++) {
          m_Curve[i] = (icFloatNumber)i/last;
        }
      }
      else if (nSize==1) {
        //Encode a gamma 1.0 u8Fixed8Number converted to 16 bit as a 0.0 to 1.0 float
        m_Curve[0] = (icFloatNumber)(0x0100) / (icFloatNumber)65535.0;
      }
      break;
    }
  }
  m_nSize = nSize;
  m_nMaxIndex = (icUInt16Number)(nSize - 1);
}

/**
****************************************************************************
* Name: sampleICC::CIccTagCurve::SetGamma
* 
* Purpose: Set the curve with a single gamma value.
* 
* Args:
*  gamma - gamma value to use
*****************************************************************************
*/
void CIccTagCurve::SetGamma(icFloatNumber gamma)
{
  SetSize(1, icInitNone);

  icInt16Number whole = (icInt16Number)gamma;
  icFloatNumber frac = gamma - (icFloatNumber)whole;

  m_Curve[0] = (icFloatNumber)((whole * 256) + (frac*256.0)) / (icFloatNumber)65535.0; 
}

/**
****************************************************************************
*  
*****************************************************************************
*/
const icFloatNumber VERYSMALLNUM = (icFloatNumber)0.0000001;
static bool IsUnity(const icFloatNumber& num)
{
  return (num>(1.0-VERYSMALLNUM) && num<(1.0+VERYSMALLNUM));
}

/**
****************************************************************************
* Name: CIccTagCurve::IsIdentity
* 
* Purpose: Checks if this is an identity curve.
* 
* Return: true if the curve is an identity
*  
*****************************************************************************
*/
bool CIccTagCurve::IsIdentity()
{
  if (!m_nSize) {
    return true;
  }

  if (m_nSize==1) {
    return  IsUnity(icFloatNumber(m_Curve[0]*65535.0/256.0));
  }

  icUInt32Number i;
  for (i=0; i<m_nSize; i++) {
    if (fabs(m_Curve[i]-((icFloatNumber)i/(icFloatNumber)m_nMaxIndex))>VERYSMALLNUM) {
      return false;
    }
  }

  return true;
}

/**
****************************************************************************
* Name: CIccTagCurve::Apply
* 
* Purpose: Applies the curve to the value passed.
* 
* Args: 
*  v = value to be passed through the curve.
*
* Return: The value modified by the curve. 
*  
*****************************************************************************
*/
icFloatNumber CIccTagCurve::Apply(icFloatNumber v)
{
  if(v<0.0) v = 0.0;
  else if(v>1.0) v = 1.0;

  icUInt32Number nIndex = (icUInt32Number)(v * m_nMaxIndex);

  if (!m_nSize) {
    return v;
  }
  if (m_nSize==1) {
    //Convert 0.0 to 1.0 float to 16bit and then convert from u8Fixed8Number
    icFloatNumber dGamma = (icFloatNumber)(m_Curve[0] * 65535.0 / 256.0);
    return pow(v, dGamma);
  }
  if (nIndex == m_nMaxIndex) {
    return m_Curve[nIndex];
  }
  else {
    icFloatNumber nDif = v*m_nMaxIndex - nIndex;
    icFloatNumber p0 = m_Curve[nIndex];

    icFloatNumber rv = p0 + (m_Curve[nIndex+1]-p0)*nDif;
    if (rv>1.0)
      rv=1.0;

    return rv;
  }
}


/**
******************************************************************************
* Name: CIccTagCurve::Validate
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
icValidateStatus CIccTagCurve::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (sig==icSigBlueTRCTag || sig==icSigRedTRCTag || sig==icSigGreenTRCTag || sig==icSigGrayTRCTag) {
    if (m_nSize>1) {
      if (m_Curve) {
        if (m_Curve[0]>0.0 || m_Curve[m_nSize-1]<1.0) {
          sReport += icValidateWarningMsg;
          sReport += sSigName;
          sReport += " - Curve cannot be accurately inverted.\n";
          rv = icMaxStatus(rv, icValidateWarning);
        }
      }
    }
  }

  return rv;
}


/**
****************************************************************************
* Name: CIccTagParametricCurve::CIccTagParametricCurve
* 
* Purpose: Constructor
* 
*****************************************************************************
*/
CIccTagParametricCurve::CIccTagParametricCurve()
{
  m_Param = NULL;
  m_nFunctionType = 0xffff;
  m_nNumParam = 0;
  m_dParam = NULL;
  m_nReserved2 = 0;
}


/**
****************************************************************************
* Name: CIccTagParametricCurve::CIccTagParametricCurve
* 
* Purpose: Copy Constructor
*
* Args:
*  ITPC = The CIccTagParametricCurve object to be copied
*****************************************************************************
*/
CIccTagParametricCurve::CIccTagParametricCurve(const CIccTagParametricCurve &ITPC)
{
  m_nFunctionType = ITPC.m_nFunctionType;
  m_nNumParam = ITPC.m_nNumParam;

  m_Param = new icS15Fixed16Number[m_nNumParam];
  memcpy(m_Param, ITPC.m_Param, sizeof(m_Param));  
  m_dParam = NULL;
}


/**
****************************************************************************
* Name: CIccTagParametricCurve::operator=
* 
* Purpose: Copy Operator
*
* Args:
*  ParamCurveTag = The CIccTagParametricCurve object to be copied
*****************************************************************************
*/
CIccTagParametricCurve &CIccTagParametricCurve::operator=(const CIccTagParametricCurve &ParamCurveTag)
{
  if (&ParamCurveTag == this)
    return *this;

  m_nFunctionType = ParamCurveTag.m_nFunctionType;
  m_nNumParam = ParamCurveTag.m_nNumParam;

  if (m_Param)
    delete [] m_Param;
  m_Param = new icS15Fixed16Number[m_nNumParam];
  memcpy(m_Param, ParamCurveTag.m_Param, sizeof(m_Param));  

  m_dParam = NULL;

  return *this;

}


/**
****************************************************************************
* Name: CIccTagParametricCurve::~CIccTagParametricCurve
* 
* Purpose: Destructor
* 
*****************************************************************************
*/
CIccTagParametricCurve::~CIccTagParametricCurve()
{
  if (m_Param)
    delete [] m_Param;

  if (m_dParam)
    delete [] m_dParam;
}


/**
****************************************************************************
* Name: CIccTagParametricCurve::Read
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
bool CIccTagParametricCurve::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt16Number nFunctionType;

  icUInt32Number nHdrSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    2*sizeof(icUInt16Number);

  if ( nHdrSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig) ||
    !pIO->Read32(&m_nReserved) ||
    !pIO->Read16(&nFunctionType) ||
    !pIO->Read16(&m_nReserved2))
    return false;

  SetFunctionType(nFunctionType);

  if (!m_nNumParam) {
    m_nNumParam = (icUInt16Number)((size-nHdrSize) / sizeof(icS15Fixed16Number));
    m_Param = new icS15Fixed16Number[m_nNumParam];
  }

  if (m_nNumParam) {
    if (nHdrSize + m_nNumParam*sizeof(icS15Fixed16Number) > size)
      return false;

    if (pIO->Read32(GetParams(), m_nNumParam) != m_nNumParam)
      return false;
  }

  return true;
}


/**
****************************************************************************
* Name: CIccTagParametricCurve::Write
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
bool CIccTagParametricCurve::Write(CIccIO *pIO)
{
  icTagTypeSignature sig;

  if (!pIO) {
    return false;
  }

  sig = GetType();

  if (!pIO->Write32(&sig) ||
    !pIO->Write32(&m_nReserved) ||
    !pIO->Write16(&m_nFunctionType) ||
    !pIO->Write16(&m_nReserved2))
    return false;

  if (m_nNumParam) {
    if (pIO->Write32(GetParams(), m_nNumParam) != m_nNumParam)
      return false;
  }

  if (!pIO->Align32())
    return false;

  return true;
}


/**
****************************************************************************
* Name: CIccTagParametricCurve::Describe
* 
* Purpose: Dump data associated with the tag to a string
* 
* Args: 
*  sDescription - string to concatenate tag dump to
*****************************************************************************
*/
void CIccTagParametricCurve::Describe(std::string &sDescription)
{
  icChar buf[128];

  sprintf(buf, "FunctionType: %04Xh\n", m_nFunctionType);
  sDescription += buf;

  switch(m_nFunctionType) {
case 0x0000:
  sprintf(buf, "Y = X ^ %.4lf\n", icFtoD(m_Param[0]));
  sDescription += buf;
  return;

case 0x0001:
  sprintf(buf, "Y = (%.4lf * X + %.4lf) ^ %.4lf   when (X >= %.4lf / %.4lf)\n",
    icFtoD(m_Param[1]), icFtoD(m_Param[2]), icFtoD(m_Param[0]),
    -icFtoD(m_Param[2]), icFtoD(m_Param[1]));
  sDescription += buf;

  sprintf(buf, "Y = 0 when (X < %.4lf / %.4lf)\n",
    -icFtoD(m_Param[2]), icFtoD(m_Param[1]));
  sDescription += buf;
  return;

case 0x0002:
  sprintf(buf, "Y = (%.4lf * X + %.4lf) ^ %.4lf + %.4lf   when (X >= %.4lf / %.4lf)\n",
    icFtoD(m_Param[1]), icFtoD(m_Param[2]), icFtoD(m_Param[0]),
    icFtoD(m_Param[3]),
    -icFtoD(m_Param[2]), icFtoD(m_Param[1]));
  sDescription += buf;

  sprintf(buf, "Y = %.4lf   when (X < %.4lf / %.4lf)\n", icFtoD(m_Param[3]),
    -icFtoD(m_Param[2]), icFtoD(m_Param[1]));
  sDescription += buf;
  return;

case 0x0003:
  sprintf(buf, "Y = (%.4lf * X + %.4lf) ^ %.4lf   when (X >= %.4lf)\n",
    icFtoD(m_Param[1]), icFtoD(m_Param[2]), icFtoD(m_Param[0]),
    icFtoD(m_Param[4]));
  sDescription += buf;

  sprintf(buf, "Y = %lf * X   when (X < %.4lf)\n",
    icFtoD(m_Param[3]), icFtoD(m_Param[4]));
  sDescription += buf;
  return;

case 0x0004:
  sprintf(buf, "Y = (%.4lf * X + %.4lf) ^ %.4lf + %.4lf  when (X >= %.4lf)\n",
    icFtoD(m_Param[1]), icFtoD(m_Param[2]), icFtoD(m_Param[0]),
    icFtoD(m_Param[5]), icFtoD(m_Param[4]));
  sDescription += buf;

  sprintf(buf, "Y = %lf * X + %.4lf  when (X < %.4lf)\n",
    icFtoD(m_Param[3]), icFtoD(m_Param[6]), icFtoD(m_Param[4]));
  sDescription += buf;
  return;

default:
  int i;
  sprintf(buf, "Unknown Function with %d parameters:\n", m_nNumParam);
  sDescription += buf;

  for (i=0; i<m_nNumParam; i++) {
    sprintf(buf, "Param[%d] = %.4lf\n", i, icFtoD(m_Param[i]));
    sDescription += buf;
  }
  }
}

/**
****************************************************************************
* Name: CIccTagParametricCurve::DumpLut
* 
* Purpose: Dump data associated with the tag to a string. Basically has 
*  the same function as Describe()
* 
* Args: 
*  sDescription = string to concatenate tag dump to,
*  szName = name of the curve to be printed,
*  csSig = color space signature of the curve data,
*  nIndex = the channel number of color space
*****************************************************************************
*/
void CIccTagParametricCurve::DumpLut(std::string &sDescription, const icChar *szName,
  icColorSpaceSignature csSig, int nIndex)
{
  icChar buf[128];

  sprintf(buf, "BEGIN_CURVE %s\n", szName);
  sDescription += buf;
  Describe(sDescription);
  sDescription += "\n";
}


/**
****************************************************************************
* Name: CIccTagParametricCurve::SetFunctionType
* 
* Purpose: Sets the type of the function the Parametric curve represents
* 
* Args: 
*  nFunctionType = the type of the function encoded as 0-4
*
* Return:
*  always true!!
*****************************************************************************
*/
bool CIccTagParametricCurve::SetFunctionType(icUInt16Number nFunctionType)
{
  icUInt16Number nNumParam;

  switch(nFunctionType) {
    case 0x0000:
      nNumParam = 1;
      break;

    case 0x0001:
      nNumParam = 3;
      break;

    case 0x0002:
      nNumParam = 4;
      break;

    case 0x0003:
      nNumParam = 5;
      break;

    case 0x0004:
      nNumParam = 7;
      break;

    default:
      nNumParam = 0;
  }

  if (m_Param)
    delete m_Param;
  m_nNumParam = nNumParam;
  m_nFunctionType = nFunctionType;

  if (m_nNumParam)
    m_Param = new icS15Fixed16Number[m_nNumParam];
  else
    m_Param = NULL;

  return true;
}


/**
****************************************************************************
* Name: CIccTagParametricCurve::Begin
* 
* Purpose: Initializes the curve. Must be called before Apply().
*
*****************************************************************************
*/
void CIccTagParametricCurve::Begin()
{
  if (m_dParam)
    delete m_dParam;

  m_dParam = new icFloatNumber[m_nNumParam];

  int i;
  for (i=0; i<m_nNumParam; i++) 
    m_dParam[i] = icFtoD(m_Param[i]);
}

/**
****************************************************************************
* Name: CIccTagParametricCurve::IsIdentity
* 
* Purpose: Checks if this is an identity curve.
* 
* Return: true if the curve is an identity
*  
*****************************************************************************
*/
bool CIccTagParametricCurve::IsIdentity()
{
  switch(m_nFunctionType) {
    case 0x0000:
      return IsUnity(icFtoD(m_Param[0]));

    case 0x0001:
    case 0x0002:
    case 0x0003:
    case 0x0004:
      return false;

    default:
      return true;
  }
}

/**
****************************************************************************
* Name: CIccTagParametricCurve::Apply
* 
* Purpose: Applies the curve to the value passed.
* 
* Args: 
*  x = value to be passed through the curve.
*
* Return: The value modified by the curve. 
*  
*****************************************************************************
*/
icFloatNumber CIccTagParametricCurve::Apply(icFloatNumber X)
{
  double a, b;

  switch(m_nFunctionType) {
    case 0x0000:
      return pow(X, m_dParam[0]);

    case 0x0001:
      a=m_dParam[1];
      b=m_dParam[2];

      if (X >= -b/a) {
        return (icFloatNumber)pow((double)a*X + b, (double)m_dParam[0]);
      }
      else {
        return 0;
      }

    case 0x0002:
      a=m_dParam[1];
      b=m_dParam[2];

      if (X >= -b/a) {
        return (icFloatNumber)pow((double)a*X + b, (double)m_dParam[0]) + m_dParam[3];
      }
      else {
        return m_dParam[3];
      }

    case 0x0003:
      if (X >= m_dParam[4]) {
        return (icFloatNumber)pow((double)m_dParam[1]*X + m_dParam[2], (double)m_dParam[0]);
      }
      else {
        return m_dParam[3]*X;
      }

    case 0x0004:
      if (X >= m_dParam[4]) {
        return (icFloatNumber)pow((double)m_dParam[1]*X + m_dParam[2], (double)m_dParam[0]) + m_dParam[5];
      }
      else {
        return m_dParam[3]*X + m_dParam[6];
      }

    default:
      return X;
  }
}


/**
******************************************************************************
* Name: CIccTagParametricCurve::Validate
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
icValidateStatus CIccTagParametricCurve::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/)
{
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (m_nReserved2!=0) {
    sReport += icValidateNonCompliantMsg;
    sReport += sSigName;
    sReport += " - Reserved Value must be zero.\n";

    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  switch(m_nFunctionType) {
case 0x0000:
  if (m_nNumParam!=1) {
    sReport += icValidateCriticalErrorMsg;
    sReport += sSigName;
    sReport += " - Number of parameters inconsistent with function type.\n";
    rv = icMaxStatus(rv, icValidateCriticalError);
  }
  break;

case 0x0001:
  if (m_nNumParam!=3) {
    sReport += icValidateCriticalErrorMsg;
    sReport += sSigName;
    sReport += " - Number of parameters inconsistent with function type.\n";
    rv = icMaxStatus(rv, icValidateCriticalError);
  }
  break;

case 0x0002:
  if (m_nNumParam!=4) {
    sReport += icValidateCriticalErrorMsg;
    sReport += sSigName;
    sReport += " - Number of parameters inconsistent with function type.\n";
    rv = icMaxStatus(rv, icValidateCriticalError);
  }
  break;

case 0x0003:
  if (m_nNumParam!=5) {
    sReport += icValidateCriticalErrorMsg;
    sReport += sSigName;
    sReport += " - Number of parameters inconsistent with function type.\n";
    rv = icMaxStatus(rv, icValidateCriticalError);
  }
  break;

case 0x0004:
  if (m_nNumParam!=7) {
    sReport += icValidateCriticalErrorMsg;
    sReport += sSigName;
    sReport += " - Number of parameters inconsistent with function type.\n";
    rv = icMaxStatus(rv, icValidateCriticalError);
  }
  break;

default:
  sReport += icValidateCriticalErrorMsg;
  sReport += sSigName;
  sReport += " - Unknown function type.\n";
  rv = icMaxStatus(rv, icValidateCriticalError);
  }

  if (sig==icSigBlueTRCTag || sig==icSigRedTRCTag || sig==icSigGreenTRCTag || sig==icSigGrayTRCTag) {
    icFloatNumber lval = Apply(0.0);
    icFloatNumber uval = Apply(1.0);
    if (lval>0.0 || uval<1.0) {
      sReport += icValidateWarningMsg;
      sReport += sSigName;
      sReport += " - Curve cannot be accurately inverted.\n";
      rv = icMaxStatus(rv, icValidateWarning);
    }
  }

  return rv;
}

/**
****************************************************************************
* Name: CIccMatrix::CIccMatrix
* 
* Purpose: Constructor
* 
* Args:
*  bUseConstants = true if the matrix contains additional row for constants
*****************************************************************************
*/
CIccMatrix::CIccMatrix(bool bUseConstants/*=true*/)
{
  m_bUseConstants = bUseConstants;
  m_e[0] = m_e[4] = m_e[8] = 1.0;
  m_e[1] = m_e[2] = m_e[3] =
    m_e[5] = m_e[6] = m_e[7] = 0.0;

  if (!m_bUseConstants) {
    m_e[9] = m_e[10] = m_e[11] = 0.0;
  }
}


/**
****************************************************************************
* Name: CIccMatrix::CIccMatrix
* 
* Purpose: Copy Constructor
*
* Args:
*  MatrixClass = The CIccMatrix object to be copied
*****************************************************************************
*/
CIccMatrix::CIccMatrix(const CIccMatrix &MatrixClass)
{
  m_bUseConstants = MatrixClass.m_bUseConstants;
  memcpy(m_e, MatrixClass.m_e, sizeof(m_e));
}


/**
****************************************************************************
* Name: CIccMatrix::operator=
* 
* Purpose: Copy Operator
*
* Args:
*  MatrixClass = The CIccMatrix object to be copied
*****************************************************************************
*/
CIccMatrix &CIccMatrix::operator=(const CIccMatrix &MatrixClass)
{
  if (&MatrixClass == this)
    return *this;

  m_bUseConstants = MatrixClass.m_bUseConstants;
  memcpy(m_e, MatrixClass.m_e, sizeof(m_e));

  return *this;
}


/**
****************************************************************************
* Name: CIccTagParametricCurve::DumpLut
* 
* Purpose: Dump the matrix data to a string. 
* 
* Args: 
*  sDescription = string to concatenate tag dump to,
*  szName = name of the curve to be printed
*****************************************************************************
*/
void CIccMatrix::DumpLut(std::string &sDescription, const icChar *szName)
{
  icChar buf[128];

  sprintf(buf, "BEGIN_MATRIX %s\n", szName);
  sDescription += buf;

  if (!m_bUseConstants) {
    sprintf(buf, "%8.4lf %8.4lf %8.4lf\n",
      m_e[0], m_e[1], m_e[2]);
    sDescription += buf;
    sprintf(buf, "%8.4lf %8.4lf %8.4lf\n",
      m_e[3], m_e[4], m_e[5]);
    sDescription += buf;
    sprintf(buf, "%8.4lf %8.4lf %8.4lf\n",
      m_e[6], m_e[7], m_e[8]);
    sDescription += buf;
  }
  else {
    sprintf(buf, "%8.4lf %8.4lf %8.4lf  +  %8.4lf\n",
      m_e[0], m_e[1], m_e[2], m_e[9]);
    sDescription += buf;
    sprintf(buf, "%8.4lf %8.4lf %8.4lf  +  %8.4lf\n",
      m_e[3], m_e[4], m_e[5], m_e[10]);
    sDescription += buf;
    sprintf(buf, "%8.4lf %8.4lf %8.4lf  +  %8.4lf\n",
      m_e[6], m_e[7], m_e[8], m_e[11]);
    sDescription += buf;
  }
  sDescription += "\n";
}

/**
****************************************************************************
* Name: CIccMatrix::IsIdentity
* 
* Purpose: Checks if the matrix is identity
* 
* Return: 
*  true if matrix is identity and uses no constants, else false
*
*****************************************************************************
*/
bool CIccMatrix::IsIdentity()
{
  if (m_bUseConstants) {
    if (fabs(m_e[9])>0.0 || fabs(m_e[10])>0.0 || fabs(m_e[11])>0.0) {
      return false;
    }
  }

  if (!IsUnity(m_e[0]) || !IsUnity(m_e[4]) || !IsUnity(m_e[8])) {
    return false;
  }

  if (fabs(m_e[1])>0.0 || fabs(m_e[2])>0.0 || fabs(m_e[3])>0.0 ||
      fabs(m_e[5])>0.0 || fabs(m_e[6])>0.0 || fabs(m_e[7])>0.0) 
  {
    return false;
  }

  return true;
}

/**
****************************************************************************
* Name: CIccMatrix::Apply
* 
* Purpose: Multiplies the pixel by the matrix.
* 
* Args: 
*  Pixel = Pixel to be multiplied by the matrix
*
*****************************************************************************
*/
void CIccMatrix::Apply(icFloatNumber *Pixel)
{
  icFloatNumber a=Pixel[0];
  icFloatNumber b=Pixel[1];
  icFloatNumber c=Pixel[2];

  icFloatNumber x = m_e[0]*a + m_e[1]*b + m_e[2]*c;
  icFloatNumber y = m_e[3]*a + m_e[4]*b + m_e[5]*c;
  icFloatNumber z = m_e[6]*a + m_e[7]*b + m_e[8]*c;

  if (m_bUseConstants) {
    x += m_e[9];
    y += m_e[10];
    z += m_e[11];
  }

  Pixel[0] = x;
  Pixel[1] = y;
  Pixel[2] = z;
}


/**
******************************************************************************
* Name: CIccMatrix::Validate
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
icValidateStatus CIccMatrix::Validate(icTagTypeSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = icValidateOK;

  if (sig==icSigLut8Type || sig==icSigLut16Type) {
    if (pProfile->m_Header.pcs!=icSigXYZData) {
      CIccInfo Info;
      std::string sSigName = Info.GetSigName(sig);
      icFloatNumber sum=0.0;
      for (int i=0; i<9; i++) {
        sum += m_e[i];
      }
      if (m_e[0]!=1.0 || m_e[4]!=1.0 || m_e[9]!=1.0 || sum!=3.0) {
        sReport += icValidateNonCompliantMsg;
        sReport += sSigName;
        sReport += " - Matrix must be identity.\n";
        rv = icValidateNonCompliant;
      }
    }
  }

  return rv;
}

  
static icFloatNumber ClutUnitClip(icFloatNumber v)
{
  if (v<0)
    return 0;
  else if (v>1.0)
    return 1.0;
 
  return v;
}

/**
 ****************************************************************************
 * Name: CIccCLUT::CIccCLUT
 * 
 * Purpose: Constructor
 * 
 * Args:
 *  nInputChannels = number of input channels,
 *  nOutputChannels = number of output channels 
 * 
 *****************************************************************************
 */
CIccCLUT::CIccCLUT(icUInt8Number nInputChannels, icUInt8Number nOutputChannels, icUInt8Number nPrecision/*=2*/)
{
  m_nInput = nInputChannels;
  m_nOutput = nOutputChannels;
  m_nPrecision = nPrecision;
  m_pData = NULL;
  m_nOffset = NULL;
  m_g = NULL;
  m_ig = NULL;
  m_s = NULL;
  m_df = NULL;

  UnitClip = ClutUnitClip;
}


/**
 ****************************************************************************
 * Name: CIccCLUT::CIccCLUT
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ICLUT = The CIccCLUT object to be copied
 *****************************************************************************
 */
CIccCLUT::CIccCLUT(const CIccCLUT &ICLUT)
{
  m_pData = NULL;
  m_nOffset = NULL;
  m_g = NULL;
  m_ig = NULL;
  m_s = NULL;
  m_df = NULL;
  m_nInput = ICLUT.m_nInput;
  m_nOutput = ICLUT.m_nOutput;
  m_nPrecision = ICLUT.m_nPrecision;
  m_nNumPoints = ICLUT.m_nNumPoints;

  m_csInput = ICLUT.m_csInput;
  m_csOutput = ICLUT.m_csOutput;

  memcpy(m_GridPoints, ICLUT.m_GridPoints, sizeof(m_GridPoints));
  memcpy(m_DimSize, ICLUT.m_DimSize, sizeof(m_DimSize));
  memcpy(m_GridAdr, ICLUT.m_GridAdr, sizeof(m_GridAdr));

  int num = NumPoints()*m_nOutput;
  m_pData = new icFloatNumber[num];
  memcpy(m_pData, ICLUT.m_pData, num*sizeof(icFloatNumber));

  UnitClip = ICLUT.UnitClip;
}


/**
 ****************************************************************************
 * Name: CIccCLUT::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  CLUTTag = The CIccCLUT object to be copied
 *****************************************************************************
 */
CIccCLUT &CIccCLUT::operator=(const CIccCLUT &CLUTTag)
{
  if (&CLUTTag == this)
    return *this;
  
  m_nInput = CLUTTag.m_nInput;
  m_nOutput = CLUTTag.m_nOutput;
  m_nPrecision = CLUTTag.m_nPrecision;
  m_nNumPoints = CLUTTag.m_nNumPoints;

  m_csInput = CLUTTag.m_csInput;
  m_csOutput = CLUTTag.m_csOutput;

  memcpy(m_GridPoints, CLUTTag.m_GridPoints, sizeof(m_GridPoints));
  memcpy(m_DimSize, CLUTTag.m_DimSize, sizeof(m_DimSize));
  memcpy(m_GridAdr, CLUTTag.m_GridAdr, sizeof(m_GridAdr));

  int num;
  if (m_pData)
    delete [] m_pData;
  num = NumPoints()*m_nOutput;
  m_pData = new icFloatNumber[num];
  memcpy(m_pData, CLUTTag.m_pData, num*sizeof(icFloatNumber));

  UnitClip = CLUTTag.UnitClip;

  return *this;
}



/**
 ****************************************************************************
 * Name: CIccCLUT::~CIccCLUT
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccCLUT::~CIccCLUT()
{
  if (m_pData)
    delete [] m_pData;

  if (m_nOffset)
    delete [] m_nOffset;

  if (m_g)
    delete [] m_g;

  if (m_ig)
    delete [] m_ig;

  if (m_s)
    delete [] m_s;

  if (m_df)
    delete [] m_df;
}

/**
 ****************************************************************************
 * Name: CIccCLUT::Init
 * 
 * Purpose: Initializes and sets the size of the CLUT
 * 
 * Args:
 *  nGridPoints = number of grid points in the CLUT
 *****************************************************************************
 */
void CIccCLUT::Init(icUInt8Number nGridPoints)
{
  memset(&m_GridPoints, 0, sizeof(m_GridPoints));
  memset(m_GridPoints, nGridPoints, m_nInput);
  memset(m_nReserved2, 0, sizeof(m_nReserved2));
  Init(&m_GridPoints[0]);
}

/**
 ****************************************************************************
 * Name: CIccCLUT::Init
 * 
 * Purpose: Initializes and sets the size of the CLUT
 * 
 * Args:
 *  pGridPoints = number of grid points in the CLUT
 *****************************************************************************
 */
void CIccCLUT::Init(icUInt8Number *pGridPoints)
{
  if (pGridPoints!=&m_GridPoints[0]) {
    memcpy(m_GridPoints, pGridPoints, m_nInput);
    if (m_nInput<16)
      memset(m_GridPoints+m_nInput, 0, 16-m_nInput);
  }

  if (m_pData) {
    delete [] m_pData;
  }

  int i=m_nInput-1;

  m_DimSize[i] = m_nOutput;
  m_nNumPoints = m_GridPoints[i];
  for (i--; i>=0; i--) {
    m_DimSize[i] = m_DimSize[i+1] * m_GridPoints[i+1];
    m_nNumPoints *= m_GridPoints[i];
  }

  icUInt32Number nSize = NumPoints() * m_nOutput;

  m_pData = new icFloatNumber[nSize];
}


/**
 ****************************************************************************
 * Name: CIccCLUT::ReadData
 * 
 * Purpose: Reads the CLUT data points into the data buffer
 * 
 * Args:
 *  size = # of bytes in the tag,
 *  pIO = IO object to read data from,
 *  nPrecision = data precision (8bit encoded as 1 or 16bit encoded as 2)
 *
 * Return:
 *  true = data read succesfully,
 *  false = read data failed
 *****************************************************************************
 */
bool CIccCLUT::ReadData(icUInt32Number size, CIccIO *pIO, icUInt8Number nPrecision)
{
  icUInt32Number nNum=NumPoints() * m_nOutput;

  if (nNum * nPrecision > size)
    return false;

  if (nPrecision==1) {
    if (pIO->Read8Float(m_pData, nNum)!=nNum)
      return false;
  }
  else if (nPrecision==2) {
    if (pIO->Read16Float(m_pData, nNum)!=nNum)
      return false;
  }
  else
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccCLUT::WriteData
 * 
 * Purpose: Writes the CLUT data points from the data buffer
 * 
 * Args:
 *  pIO = IO object to write data to,
 *  nPrecision = data precision (8bit encoded as 1 or 16bit encoded as 2)
 *
 * Return:
 *  true = data written succesfully,
 *  false = write operation failed
 *****************************************************************************
 */
bool CIccCLUT::WriteData(CIccIO *pIO, icUInt8Number nPrecision)
{
  icUInt32Number nNum=NumPoints() * m_nOutput;

  if (nPrecision==1) {
    if (pIO->Write8Float(m_pData, nNum)!=nNum)
      return false;
  }
  else if (nPrecision==2) {
    if (pIO->Write16Float(m_pData, nNum)!=nNum)
      return false;
  }
  else
    return false;

  return true;
}


/**
 ****************************************************************************
 * Name: CIccCLUT::Read
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
bool CIccCLUT::Read(icUInt32Number size, CIccIO *pIO)
{
  if (size < 20)
    return false;

  if (pIO->Read8(m_GridPoints, 16)!=16 ||
      !pIO->Read8(&m_nPrecision) ||
      pIO->Read8(&m_nReserved2[0], 3)!=3)
    return false;

  Init(m_GridPoints);

  return ReadData(size-20, pIO, m_nPrecision);
}


/**
 ****************************************************************************
 * Name: CIccCLUT::Write
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
bool CIccCLUT::Write(CIccIO *pIO)
{
  if (pIO->Write8(m_GridPoints, 16)!=16 ||
      !pIO->Write8(&m_nPrecision) ||
      pIO->Write8(&m_nReserved2[0], 3)!=3)
    return false;

  return WriteData(pIO, m_nPrecision);
}

/**
 ****************************************************************************
 * Name: CIccCLUT::Iterate
 * 
 * Purpose: Iterate through the CLUT to dump the data
 * 
 * Args: 
 *  sDescription = string to concatenate data dump to,
 *  nIndex = the channel number,
 *  nPos = the current position in the CLUT
 * 
 *****************************************************************************
 */
void CIccCLUT::Iterate(std::string &sDescription, icUInt8Number nIndex, icUInt32Number nPos)
{
  if (nIndex < m_nInput) {
    int i;
    for (i=0; i<m_GridPoints[nIndex]; i++) {
      m_GridAdr[nIndex] = i;
      Iterate(sDescription, nIndex+1, nPos);
      nPos += m_DimSize[nIndex];
    }
  }
  else {
    icChar *ptr = m_pOutText;
    icFloatNumber *pData = &m_pData[nPos];
    int i;

    for (i=0; i<m_nInput; i++) {
      icColorValue(m_pVal, (icFloatNumber)m_GridAdr[i] / (m_GridPoints[i]-1) , m_csInput, i);

      ptr += sprintf(ptr, " %s", m_pVal);
    }
    strcpy(ptr, "  ");
    ptr += 2;

    for (i=0; i<m_nOutput; i++) {
      icColorValue(m_pVal, pData[i], m_csOutput, i);

      ptr += sprintf(ptr, " %s", m_pVal);
    }
    strcpy(ptr, "\n");
    sDescription += (const icChar*)m_pOutText;

  }
}


/**
 ****************************************************************************
 * Name: CIccCLUT::Iterate
 * 
 * Purpose: Iterate through the CLUT to get the data and execute PixelOp 
 * 
 * Args: 
 *  pExec = pointer to the IIccCLUTExec object that implements the 
 *          IIccCLUTExec::Apply() function
 * 
 *****************************************************************************
 */
void CIccCLUT::Iterate(IIccCLUTExec* pExec)
{
  memset(&m_fGridAdr[0], 0, sizeof(m_fGridAdr));
  if (m_nInput==3) {
    int i,j,k;
    icUInt32Number index=0;
    for (i=0; i<m_GridPoints[0]; i++) {
      for (j=0; j<m_GridPoints[1]; j++) {
        for (k=0; k<m_GridPoints[2]; k++) {
          m_fGridAdr[2] = (icFloatNumber)k/(icFloatNumber)(m_GridPoints[2]-1);
          m_fGridAdr[1] = (icFloatNumber)j/(icFloatNumber)(m_GridPoints[1]-1);
          m_fGridAdr[0] = (icFloatNumber)i/(icFloatNumber)(m_GridPoints[0]-1);

          index = (m_DimSize[0]*i + m_DimSize[1]*j + m_DimSize[2]*k); 
          pExec->PixelOp(m_fGridAdr, &m_pData[index]);

        }
      }
    }
  }
  else if (m_nInput==4) {
    int i,j,k,l;
    icUInt32Number index=0;
    for (i=0; i<m_GridPoints[0]; i++) {
      for (j=0; j<m_GridPoints[1]; j++) {
        for (k=0; k<m_GridPoints[2]; k++) {
          for (l=0; l<m_GridPoints[3]; l++) {
            m_fGridAdr[3] = (icFloatNumber)l/(icFloatNumber)(m_GridPoints[3]-1);
            m_fGridAdr[2] = (icFloatNumber)k/(icFloatNumber)(m_GridPoints[2]-1);
            m_fGridAdr[1] = (icFloatNumber)j/(icFloatNumber)(m_GridPoints[1]-1);
            m_fGridAdr[0] = (icFloatNumber)i/(icFloatNumber)(m_GridPoints[0]-1);

            index = (m_DimSize[0]*i + m_DimSize[1]*j + 
                     m_DimSize[2]*k + m_DimSize[3]*l); 
            pExec->PixelOp(m_fGridAdr, &m_pData[index]);

          }
        }
      }
    }
  }
  else
    SubIterate(pExec, 0, 0);
}


/**
 ****************************************************************************
 * Name: CIccCLUT::SubIterate
 * 
 * Purpose: Iterate through the CLUT to get the data
 * 
 * Args: 
 *  pExec = pointer to the IIccCLUTExec object that implements the 
 *          IIccCLUTExec::Apply() function,
 *  nIndex = the channel number,
 *  nPos = the current position in the CLUT
 * 
 *****************************************************************************
 */
void CIccCLUT::SubIterate(IIccCLUTExec* pExec, icUInt8Number nIndex, icUInt32Number nPos)
{
  if (nIndex < m_nInput) {
    int i;
    for (i=0; i<m_GridPoints[nIndex]; i++) {
      m_fGridAdr[nIndex] = (icFloatNumber)i/(icFloatNumber)(m_GridPoints[nIndex]-1);
      SubIterate(pExec, nIndex+1, nPos);
      nPos += m_DimSize[nIndex];
    }
  }
  else
    pExec->PixelOp(m_fGridAdr, &m_pData[nPos]);
}

/**
 ****************************************************************************
 * Name: CIccCLUT::DumpLut
 * 
 * Purpose: Dump data associated with the tag to a string. 
 * 
 * Args: 
 *  sDescription = string to concatenate tag dump to,
 *  szName = name of the LUT to be printed,
 *  csInput = color space signature of the input data,
 *  csOutput = color space signature of the output data
 *****************************************************************************
 */
void CIccCLUT::DumpLut(std::string  &sDescription, const icChar *szName,
                       icColorSpaceSignature csInput, icColorSpaceSignature csOutput)
{
  icChar szOutText[2048], szColor[40];
  int i, len;

  sprintf(szOutText, "BEGIN_LUT %s %d %d\n", szName, m_nInput, m_nOutput);
  sDescription += szOutText;

  for (i=0; i<m_nInput; i++) {
    icColorIndexName(szColor, csInput, i, m_nInput, "In");
    sprintf(szOutText, " %s=%d", szColor, m_GridPoints[i]);
    sDescription += szOutText;
  }

  sDescription += "  ";

  for (i=0; i<m_nOutput; i++) {
    icColorIndexName(szColor, csOutput, i, m_nOutput, "Out");
    sprintf(szOutText, " %s", szColor);
    sDescription += szOutText;
  }

  sDescription += "\n";

  len = 0;
  for (i=0; i<m_nInput; i++) {
    icColorValue(szColor, 1.0, csInput, i);
    len+= strlen(szColor);
  }
  for (i=0; i<m_nOutput; i++) {
    icColorValue(szColor, 1.0, csOutput, i);
    len+= strlen(szColor);
  }
  len += m_nInput + m_nOutput + 6;

  sDescription.reserve(sDescription.size() + NumPoints()*len);

  //Initialize iteration member variables
  m_csInput = csInput;
  m_csOutput = csOutput;
  m_pOutText = szOutText;
  m_pVal = szColor;
  memset(m_GridAdr, 0, 16);

  Iterate(sDescription, 0, 0);
  
  sDescription += "\n";
}



/**
 ****************************************************************************
 * Name: CIccCLUT::Begin
 * 
 * Purpose: Initializes the CLUT. Must be called before Apply().
 *
 *****************************************************************************
 */
void CIccCLUT::Begin()
{
  int i;
  for (i=0; i<m_nInput; i++) {
    m_MaxGridPoint[i] = m_GridPoints[i] - 1;
  }
  m_nNodes = (1<<m_nInput);

  if (m_nOffset)
    delete [] m_nOffset;

  m_nOffset = new icUInt32Number[m_nNodes];

  if (m_nInput==3) {
  n000 = 0;
    n001 = m_DimSize[0];
    n010 = m_DimSize[1];
    n011 = n001 + n010;
    n100 = m_DimSize[2];
    n101 = n100 + n001;
    n110 = n100 + n010;
    n111 = n110 + n001;
  }
  else if (m_nInput == 4) {
    m_nOffset[ 0] = 0;
    m_nOffset[ 1] = n001 = m_DimSize[ 0];
    m_nOffset[ 2] = n010 = m_DimSize[ 1];
    m_nOffset[ 3] = m_nOffset[ 2] + m_nOffset[ 1];
    m_nOffset[ 4] = n100 = m_DimSize[ 2];
    m_nOffset[ 5] = m_nOffset[ 4] + m_nOffset[ 1];
    m_nOffset[ 6] = m_nOffset[ 4] + m_nOffset[ 2];
    m_nOffset[ 7] = m_nOffset[ 4] + m_nOffset[ 3];
    m_nOffset[ 8] = n1000 = m_DimSize[ 3];
    m_nOffset[ 9] = m_nOffset[ 8] + m_nOffset[ 1];
    m_nOffset[10] = m_nOffset[ 8] + m_nOffset[ 2];
    m_nOffset[11] = m_nOffset[ 8] + m_nOffset[ 3];
    m_nOffset[12] = m_nOffset[ 8] + m_nOffset[ 4];
    m_nOffset[13] = m_nOffset[ 8] + m_nOffset[ 5];
    m_nOffset[14] = m_nOffset[ 8] + m_nOffset[ 6];
    m_nOffset[15] = m_nOffset[ 8] + m_nOffset[ 7];
  }
  else if (m_nInput == 5) {
    m_nOffset[ 0] = 0;
    m_nOffset[ 1] = n001 = m_DimSize[ 0];
    m_nOffset[ 2] = n010 = m_DimSize[ 1];
    m_nOffset[ 3] = m_nOffset[ 2] + m_nOffset[ 1];
    m_nOffset[ 4] = n100 = m_DimSize[ 2];
    m_nOffset[ 5] = m_nOffset[ 4] + m_nOffset[ 1];
    m_nOffset[ 6] = m_nOffset[ 4] + m_nOffset[ 2];
    m_nOffset[ 7] = m_nOffset[ 4] + m_nOffset[ 3];
    m_nOffset[ 8] = n1000 = m_DimSize[ 3];
    m_nOffset[ 9] = m_nOffset[ 8] + m_nOffset[ 1];
    m_nOffset[10] = m_nOffset[ 8] + m_nOffset[ 2];
    m_nOffset[11] = m_nOffset[ 8] + m_nOffset[ 3];
    m_nOffset[12] = m_nOffset[ 8] + m_nOffset[ 4];
    m_nOffset[13] = m_nOffset[ 8] + m_nOffset[ 5];
    m_nOffset[14] = m_nOffset[ 8] + m_nOffset[ 6];
    m_nOffset[15] = m_nOffset[ 8] + m_nOffset[ 7];
    m_nOffset[16] = n10000 = m_DimSize[ 4];
    m_nOffset[17] = m_nOffset[16] + m_nOffset[ 1];
    m_nOffset[18] = m_nOffset[16] + m_nOffset[ 2];
    m_nOffset[19] = m_nOffset[16] + m_nOffset[ 3];
    m_nOffset[20] = m_nOffset[16] + m_nOffset[ 4];
    m_nOffset[21] = m_nOffset[16] + m_nOffset[ 5];
    m_nOffset[22] = m_nOffset[16] + m_nOffset[ 6];
    m_nOffset[23] = m_nOffset[16] + m_nOffset[ 7];
    m_nOffset[24] = m_nOffset[16] + m_nOffset[ 8];
    m_nOffset[25] = m_nOffset[16] + m_nOffset[ 9];
    m_nOffset[26] = m_nOffset[16] + m_nOffset[10];
    m_nOffset[27] = m_nOffset[16] + m_nOffset[11];
    m_nOffset[28] = m_nOffset[16] + m_nOffset[12];
    m_nOffset[29] = m_nOffset[16] + m_nOffset[13];
    m_nOffset[30] = m_nOffset[16] + m_nOffset[14];
    m_nOffset[31] = m_nOffset[16] + m_nOffset[15];
  }
  else if (m_nInput == 6) {
    m_nOffset[ 0] = 0;
    m_nOffset[ 1] = n001 = m_DimSize[ 0];
    m_nOffset[ 2] = n010 = m_DimSize[ 1];
    m_nOffset[ 3] = m_nOffset[ 2] + m_nOffset[ 1];
    m_nOffset[ 4] = n100 = m_DimSize[ 2];
    m_nOffset[ 5] = m_nOffset[ 4] + m_nOffset[ 1];
    m_nOffset[ 6] = m_nOffset[ 4] + m_nOffset[ 2];
    m_nOffset[ 7] = m_nOffset[ 4] + m_nOffset[ 3];
    m_nOffset[ 8] = n1000 = m_DimSize[ 3];
    m_nOffset[ 9] = m_nOffset[ 8] + m_nOffset[ 1];
    m_nOffset[10] = m_nOffset[ 8] + m_nOffset[ 2];
    m_nOffset[11] = m_nOffset[ 8] + m_nOffset[ 3];
    m_nOffset[12] = m_nOffset[ 8] + m_nOffset[ 4];
    m_nOffset[13] = m_nOffset[ 8] + m_nOffset[ 5];
    m_nOffset[14] = m_nOffset[ 8] + m_nOffset[ 6];
    m_nOffset[15] = m_nOffset[ 8] + m_nOffset[ 7];
    m_nOffset[16] = n10000 = m_DimSize[ 4];
    m_nOffset[17] = m_nOffset[16] + m_nOffset[ 1];
    m_nOffset[18] = m_nOffset[16] + m_nOffset[ 2];
    m_nOffset[19] = m_nOffset[16] + m_nOffset[ 3];
    m_nOffset[20] = m_nOffset[16] + m_nOffset[ 4];
    m_nOffset[21] = m_nOffset[16] + m_nOffset[ 5];
    m_nOffset[22] = m_nOffset[16] + m_nOffset[ 6];
    m_nOffset[23] = m_nOffset[16] + m_nOffset[ 7];
    m_nOffset[24] = m_nOffset[16] + m_nOffset[ 8];
    m_nOffset[25] = m_nOffset[16] + m_nOffset[ 9];
    m_nOffset[26] = m_nOffset[16] + m_nOffset[10];
    m_nOffset[27] = m_nOffset[16] + m_nOffset[11];
    m_nOffset[28] = m_nOffset[16] + m_nOffset[12];
    m_nOffset[29] = m_nOffset[16] + m_nOffset[13];
    m_nOffset[30] = m_nOffset[16] + m_nOffset[14];
    m_nOffset[31] = m_nOffset[16] + m_nOffset[15];
    m_nOffset[32] = n100000 = m_DimSize[5];
    m_nOffset[33] = m_nOffset[32] + m_nOffset[ 1];
    m_nOffset[34] = m_nOffset[32] + m_nOffset[ 2];
    m_nOffset[35] = m_nOffset[32] + m_nOffset[ 3];
    m_nOffset[36] = m_nOffset[32] + m_nOffset[ 4];
    m_nOffset[37] = m_nOffset[32] + m_nOffset[ 5];
    m_nOffset[38] = m_nOffset[32] + m_nOffset[ 6];
    m_nOffset[39] = m_nOffset[32] + m_nOffset[ 7];
    m_nOffset[40] = m_nOffset[32] + m_nOffset[ 8];
    m_nOffset[41] = m_nOffset[32] + m_nOffset[ 9];
    m_nOffset[42] = m_nOffset[32] + m_nOffset[10];
    m_nOffset[43] = m_nOffset[32] + m_nOffset[11];
    m_nOffset[44] = m_nOffset[32] + m_nOffset[12];
    m_nOffset[45] = m_nOffset[32] + m_nOffset[13];
    m_nOffset[46] = m_nOffset[32] + m_nOffset[14];
    m_nOffset[47] = m_nOffset[32] + m_nOffset[15];
    m_nOffset[48] = m_nOffset[32] + m_nOffset[16];
    m_nOffset[49] = m_nOffset[32] + m_nOffset[17];
    m_nOffset[50] = m_nOffset[32] + m_nOffset[18];
    m_nOffset[51] = m_nOffset[32] + m_nOffset[19];
    m_nOffset[52] = m_nOffset[32] + m_nOffset[20];
    m_nOffset[53] = m_nOffset[32] + m_nOffset[21];
    m_nOffset[54] = m_nOffset[32] + m_nOffset[22];
    m_nOffset[55] = m_nOffset[32] + m_nOffset[23];
    m_nOffset[56] = m_nOffset[32] + m_nOffset[24];
    m_nOffset[57] = m_nOffset[32] + m_nOffset[25];
    m_nOffset[58] = m_nOffset[32] + m_nOffset[26];
    m_nOffset[59] = m_nOffset[32] + m_nOffset[27];
    m_nOffset[60] = m_nOffset[32] + m_nOffset[28];
    m_nOffset[61] = m_nOffset[32] + m_nOffset[29];
    m_nOffset[62] = m_nOffset[32] + m_nOffset[30];
    m_nOffset[63] = m_nOffset[32] + m_nOffset[31];
  }
  else {
    //initialize ND interpolation variables
    m_g = new icFloatNumber[m_nInput];
    m_ig = new icUInt32Number[m_nInput];
    m_s = new icFloatNumber[m_nInput];
    m_df = new icFloatNumber[m_nNodes];
    
    m_nOffset[0] = 0;
    int count, nFlag;
    icUInt32Number nPower[2];
    nPower[1] = 1;

    for (count=0; count<m_nInput; count++) {
      m_nPower[count] = (1<<(m_nInput-1-count));
    }

    count = 0;
    nFlag = 1;
    for (icUInt32Number j=1; j<m_nNodes; j++) {
      if (j == nPower[1]) {
        m_nOffset[j] = m_DimSize[count];
        nPower[0] = (1<<count);
        count++;
        nPower[1] = (1<<count);
        nFlag = 1;
      }
      else {
        m_nOffset[j] = m_nOffset[nPower[0]] + m_nOffset[nFlag];
        nFlag++;
      }
    }
  }
}



static icInt32Number Scale3d(icInt32Number t,
                                     icInt32Number u,
                                     icInt32Number v)
{
  return (icInt32Number)(((icInt64Number)t * u * v)>>16);
}



/**
 ******************************************************************************
 * Name: CIccCLUT::Interp3dTetra
 * 
 * Purpose: Tetrahedral interpolation function
 *
 * Args:
 *  Pixel = Pixel value to be found in the CLUT. Also used to store the result.
 *******************************************************************************
 */
void CIccCLUT::Interp3dTetra(icFloatNumber *destPixel, const icFloatNumber *srcPixel)
{
  icUInt8Number mx = m_MaxGridPoint[0];
  icUInt8Number my = m_MaxGridPoint[1];
  icUInt8Number mz = m_MaxGridPoint[2];

  icFloatNumber x = srcPixel[0] * mx;
  icFloatNumber y = srcPixel[1] * my;
  icFloatNumber z = srcPixel[2] * mz;

  icUInt32Number ix = (icUInt32Number)x;
  icUInt32Number iy = (icUInt32Number)y;
  icUInt32Number iz = (icUInt32Number)z;

  icFloatNumber v = x - ix;
  icFloatNumber u = y - iy;
  icFloatNumber t = z - iz;

  if (ix==mx) {
    ix--;
    v = 1.0;
  }
  if (iy==my) {
    iy--;
    u = 1.0;
  }
  if (iz==mz) {
    iz--;
    t = 1.0;
  }

  int i;
  icFloatNumber *p = &m_pData[ix*n001 + iy*n010 + iz*n100];

  //Normalize grid units

  for (i=0; i<m_nOutput; i++, p++) {
    if (t<u) {
      if (t>v) {
        destPixel[i] = UnitClip(p[n000] + t*(p[n110]-p[n010]) +
                                      u*(p[n010]-p[n000]) +
                                      v*(p[n111]-p[n110]));
      }
      else if (u<v) {
        destPixel[i] = UnitClip(p[n000] + t*(p[n111]-p[n011]) + 
                                      u*(p[n011]-p[n001]) +
                                      v*(p[n001]-p[n000]));
      }
      else {
        destPixel[i] = UnitClip(p[n000] + t*(p[n111]-p[n011]) +
                                      u*(p[n010]-p[n000]) +
                                      v*(p[n011]-p[n010]));
      }
    }
    else { 
      if (t<v) {
        destPixel[i] = UnitClip(p[n000] + t*(p[n101]-p[n001]) + 
                                      u*(p[n111]-p[n101]) + 
                                      v*(p[n001]-p[n000]));
      }
      else if (u<v) {
        destPixel[i] = UnitClip(p[n000] + t*(p[n100]-p[n000]) + 
                                      u*(p[n110]-p[n100]) + 
                                      v*(p[n111]-p[n110]));
      }
      else {
        destPixel[i] = UnitClip(p[n000] + t*(p[n100]-p[n000]) + 
                                      u*(p[n111]-p[n101]) + 
                                      v*(p[n101]-p[n100]));
      }
    }
  }
}



/**
 ******************************************************************************
 * Name: CIccCLUT::Interp3d
 * 
 * Purpose: Three dimensional interpolation function
 *
 * Args:
 *  Pixel = Pixel value to be found in the CLUT. Also used to store the result.
 *******************************************************************************
 */
void CIccCLUT::Interp3d(icFloatNumber *destPixel, const icFloatNumber *srcPixel)
{
  icUInt8Number mx = m_MaxGridPoint[0];
  icUInt8Number my = m_MaxGridPoint[1];
  icUInt8Number mz = m_MaxGridPoint[2];

  icFloatNumber x = srcPixel[0] * mx;
  icFloatNumber y = srcPixel[1] * my;
  icFloatNumber z = srcPixel[2] * mz;

  icUInt32Number ix = (icUInt32Number)x;
  icUInt32Number iy = (icUInt32Number)y;
  icUInt32Number iz = (icUInt32Number)z;

  icFloatNumber u = x - ix;
  icFloatNumber t = y - iy;
  icFloatNumber s = z - iz;

  if (ix==mx) {
    ix--;
    u = 1.0;
  }
  if (iy==my) {
    iy--;
    t = 1.0;
  }
  if (iz==mz) {
    iz--;
    s = 1.0;
  }

  icFloatNumber ns = (icFloatNumber)(1.0 - s);
  icFloatNumber nt = (icFloatNumber)(1.0 - t);
  icFloatNumber nu = (icFloatNumber)(1.0 - u);

  int i;
  icFloatNumber *p = &m_pData[ix*n001 + iy*n010 + iz*n100];

  //Normalize grid units
  icFloatNumber dF0, dF1, dF2, dF3, dF4, dF5, dF6, dF7, pv;

  dF0 = ns* nt* nu;
  dF1 = ns* nt*  u;
  dF2 = ns*  t* nu;
  dF3 = ns*  t*  u;
  dF4 =  s* nt* nu;
  dF5 =  s* nt*  u;
  dF6 =  s*  t* nu;
  dF7 =  s*  t*  u;

  for (i=0; i<m_nOutput; i++, p++) {
    pv = p[n000]*dF0 + p[n001]*dF1 + p[n010]*dF2 + p[n011]*dF3 +
         p[n100]*dF4 + p[n101]*dF5 + p[n110]*dF6 + p[n111]*dF7;

    destPixel[i] = UnitClip(pv);
  }
}



/**
 ******************************************************************************
 * Name: CIccCLUT::Interp4d
 * 
 * Purpose: Four dimensional interpolation function
 *
 * Args:
 *  Pixel = Pixel value to be found in the CLUT. Also used to store the result.
 *******************************************************************************
 */
void CIccCLUT::Interp4d(icFloatNumber *destPixel, const icFloatNumber *srcPixel)
{
  icUInt8Number mw = m_MaxGridPoint[0];
  icUInt8Number mx = m_MaxGridPoint[1];
  icUInt8Number my = m_MaxGridPoint[2];
  icUInt8Number mz = m_MaxGridPoint[3];

  icFloatNumber w = srcPixel[0] * mw;
  icFloatNumber x = srcPixel[1] * mx;
  icFloatNumber y = srcPixel[2] * my;
  icFloatNumber z = srcPixel[3] * mz;

  icUInt32Number iw = (icUInt32Number)w;
  icUInt32Number ix = (icUInt32Number)x;
  icUInt32Number iy = (icUInt32Number)y;
  icUInt32Number iz = (icUInt32Number)z;

  icFloatNumber v = w - iw;
  icFloatNumber u = x - ix;
  icFloatNumber t = y - iy;
  icFloatNumber s = z - iz;

  if (iw==mw) {
    iw--;
    v = 1.0;
  }
  if (ix==mx) {
    ix--;
    u = 1.0;
  }
  if (iy==my) {
    iy--;
    t = 1.0;
  }
  if (iz==mz) {
    iz--;
    s = 1.0;
  }

  icFloatNumber ns = (icFloatNumber)(1.0 - s);
  icFloatNumber nt = (icFloatNumber)(1.0 - t);
  icFloatNumber nu = (icFloatNumber)(1.0 - u);
  icFloatNumber nv = (icFloatNumber)(1.0 - v);

  int i, j;
  icFloatNumber *p = &m_pData[iw*n001 + ix*n010 + iy*n100 + iz*n1000];

  //Normalize grid units
  icFloatNumber dF[16], pv;

  dF[ 0] = ns* nt* nu* nv;
  dF[ 1] = ns* nt* nu*  v;
  dF[ 2] = ns* nt*  u* nv;
  dF[ 3] = ns* nt*  u*  v;
  dF[ 4] = ns*  t* nu* nv;
  dF[ 5] = ns*  t* nu*  v;
  dF[ 6] = ns*  t*  u* nv;
  dF[ 7] = ns*  t*  u*  v;
  dF[ 8] =  s* nt* nu* nv;
  dF[ 9] =  s* nt* nu*  v;
  dF[10] =  s* nt*  u* nv;
  dF[11] =  s* nt*  u*  v;
  dF[12] =  s*  t* nu* nv;
  dF[13] =  s*  t* nu*  v;
  dF[14] =  s*  t*  u* nv;
  dF[15] =  s*  t*  u*  v;

  for (i=0; i<m_nOutput; i++, p++) {
    for (pv=0, j=0; j<16; j++)
      pv += p[m_nOffset[j]] * dF[j];

    destPixel[i] = UnitClip(pv);
  }
}



/**
 ******************************************************************************
 * Name: CIccCLUT::Interp5d
 * 
 * Purpose: Five dimensional interpolation function
 *
 * Args:
 *  Pixel = Pixel value to be found in the CLUT. Also used to store the result.
 *******************************************************************************
 */
void CIccCLUT::Interp5d(icFloatNumber *destPixel, const icFloatNumber *srcPixel)
{
  icUInt8Number m0 = m_MaxGridPoint[0];
  icUInt8Number m1 = m_MaxGridPoint[1];
  icUInt8Number m2 = m_MaxGridPoint[2];
  icUInt8Number m3 = m_MaxGridPoint[3];
  icUInt8Number m4 = m_MaxGridPoint[4];

  icFloatNumber g0 = srcPixel[0] * m0;
  icFloatNumber g1 = srcPixel[1] * m1;
  icFloatNumber g2 = srcPixel[2] * m2;
  icFloatNumber g3 = srcPixel[3] * m3;
  icFloatNumber g4 = srcPixel[4] * m4;

  icUInt32Number ig0 = (icUInt32Number)g0;
  icUInt32Number ig1 = (icUInt32Number)g1;
  icUInt32Number ig2 = (icUInt32Number)g2;
  icUInt32Number ig3 = (icUInt32Number)g3;
  icUInt32Number ig4 = (icUInt32Number)g4;

  icFloatNumber s4 = g0 - ig0;
  icFloatNumber s3 = g1 - ig1;
  icFloatNumber s2 = g2 - ig2;
  icFloatNumber s1 = g3 - ig3;
  icFloatNumber s0 = g4 - ig4;

  if (ig0==m0) {
    ig0--;
    s4 = 1.0;
  }
  if (ig1==m1) {
    ig1--;
    s3 = 1.0;
  }
  if (ig2==m2) {
    ig2--;
    s2 = 1.0;
  }
  if (ig3==m3) {
    ig3--;
    s1 = 1.0;
  }
  if (ig4==m4) {
    ig4--;
    s0 = 1.0;
  }

  icFloatNumber ns0 = (icFloatNumber)(1.0 - s0);
  icFloatNumber ns1 = (icFloatNumber)(1.0 - s1);
  icFloatNumber ns2 = (icFloatNumber)(1.0 - s2);
  icFloatNumber ns3 = (icFloatNumber)(1.0 - s3);
  icFloatNumber ns4 = (icFloatNumber)(1.0 - s4);

  int i, j;
  icFloatNumber *p = &m_pData[ig0*n001 + ig1*n010 + ig2*n100 + ig3*n1000 + ig4*n10000];

  //Normalize grid units
  icFloatNumber dF[32], pv;

  dF[ 0] = ns0 * ns1 * ns2 * ns3 * ns4;
  dF[ 1] = ns0 * ns1 * ns2 * ns3 *  s4;
  dF[ 2] = ns0 * ns1 * ns2 *  s3 * ns4;
  dF[ 3] = ns0 * ns1 * ns2 *  s3 *  s4;
  dF[ 4] = ns0 * ns1 *  s2 * ns3 * ns4;
  dF[ 5] = ns0 * ns1 *  s2 * ns3 *  s4;
  dF[ 6] = ns0 * ns1 *  s2 *  s3 * ns4;
  dF[ 7] = ns0 * ns1 *  s2 *  s3 *  s4;
  dF[ 8] = ns0 *  s1 * ns2 * ns3 * ns4;
  dF[ 9] = ns0 *  s1 * ns2 * ns3 *  s4;
  dF[10] = ns0 *  s1 * ns2 *  s3 * ns4;
  dF[11] = ns0 *  s1 * ns2 *  s3 *  s4;
  dF[12] = ns0 *  s1 *  s2 * ns3 * ns4;
  dF[13] = ns0 *  s1 *  s2 * ns3 *  s4;
  dF[14] = ns0 *  s1 *  s2 *  s3 * ns4;
  dF[15] = ns0 *  s1 *  s2 *  s3 *  s4;
  dF[16] =  s0 * ns1 * ns2 * ns3 * ns4;
  dF[17] =  s0 * ns1 * ns2 * ns3 *  s4;
  dF[18] =  s0 * ns1 * ns2 *  s3 * ns4;
  dF[19] =  s0 * ns1 * ns2 *  s3 *  s4;
  dF[20] =  s0 * ns1 *  s2 * ns3 * ns4;
  dF[21] =  s0 * ns1 *  s2 * ns3 *  s4;
  dF[22] =  s0 * ns1 *  s2 *  s3 * ns4;
  dF[23] =  s0 * ns1 *  s2 *  s3 *  s4;
  dF[24] =  s0 *  s1 * ns2 * ns3 * ns4;
  dF[25] =  s0 *  s1 * ns2 * ns3 *  s4;
  dF[26] =  s0 *  s1 * ns2 *  s3 * ns4;
  dF[27] =  s0 *  s1 * ns2 *  s3 *  s4;
  dF[28] =  s0 *  s1 *  s2 * ns3 * ns4;
  dF[29] =  s0 *  s1 *  s2 * ns3 *  s4;
  dF[30] =  s0 *  s1 *  s2 *  s3 * ns4;
  dF[31] =  s0 *  s1 *  s2 *  s3 *  s4;

  for (i=0; i<m_nOutput; i++, p++) {
    for (pv=0.0, j=0; j<32; j++)
      pv += p[m_nOffset[j]] * dF[j];

    destPixel[i] = UnitClip(pv);
  }
}



/**
 ******************************************************************************
 * Name: CIccCLUT::Interp6d
 * 
 * Purpose: Six dimensional interpolation function
 *
 * Args:
 *  Pixel = Pixel value to be found in the CLUT. Also used to store the result.
 *******************************************************************************
 */
void CIccCLUT::Interp6d(icFloatNumber *destPixel, const icFloatNumber *srcPixel)
{
  icUInt8Number m0 = m_MaxGridPoint[0];
  icUInt8Number m1 = m_MaxGridPoint[1];
  icUInt8Number m2 = m_MaxGridPoint[2];
  icUInt8Number m3 = m_MaxGridPoint[3];
  icUInt8Number m4 = m_MaxGridPoint[4];
  icUInt8Number m5 = m_MaxGridPoint[5];

  icFloatNumber g0 = srcPixel[0] * m0;
  icFloatNumber g1 = srcPixel[1] * m1;
  icFloatNumber g2 = srcPixel[2] * m2;
  icFloatNumber g3 = srcPixel[3] * m3;
  icFloatNumber g4 = srcPixel[4] * m4;
  icFloatNumber g5 = srcPixel[5] * m5;

  icUInt32Number ig0 = (icUInt32Number)g0;
  icUInt32Number ig1 = (icUInt32Number)g1;
  icUInt32Number ig2 = (icUInt32Number)g2;
  icUInt32Number ig3 = (icUInt32Number)g3;
  icUInt32Number ig4 = (icUInt32Number)g4;
  icUInt32Number ig5 = (icUInt32Number)g5;

  icFloatNumber s5 = g0 - ig0;
  icFloatNumber s4 = g1 - ig1;
  icFloatNumber s3 = g2 - ig2;
  icFloatNumber s2 = g3 - ig3;
  icFloatNumber s1 = g4 - ig4;
  icFloatNumber s0 = g5 - ig5;

  if (ig0==m0) {
    ig0--;
    s5 = 1.0;
  }
  if (ig1==m1) {
    ig1--;
    s4 = 1.0;
  }
  if (ig2==m2) {
    ig2--;
    s3 = 1.0;
  }
  if (ig3==m3) {
    ig3--;
    s2 = 1.0;
  }
  if (ig4==m4) {
    ig4--;
    s1 = 1.0;
  }
  if (ig5==m5) {
    ig5--;
    s0 = 1.0;
  }

  icFloatNumber ns0 = (icFloatNumber)(1.0 - s0);
  icFloatNumber ns1 = (icFloatNumber)(1.0 - s1);
  icFloatNumber ns2 = (icFloatNumber)(1.0 - s2);
  icFloatNumber ns3 = (icFloatNumber)(1.0 - s3);
  icFloatNumber ns4 = (icFloatNumber)(1.0 - s4);
  icFloatNumber ns5 = (icFloatNumber)(1.0 - s5);

  int i, j;
  icFloatNumber *p = &m_pData[ig0*n001 + ig1*n010 + ig2*n100 + ig3*n1000 + ig4*n10000 + ig5*n100000];

  //Normalize grid units
  icFloatNumber dF[64], pv;

  dF[ 0] = ns0 * ns1 * ns2 * ns3 * ns4 * ns5;
  dF[ 1] = ns0 * ns1 * ns2 * ns3 * ns4 *  s5;
  dF[ 2] = ns0 * ns1 * ns2 * ns3 *  s4 * ns5;
  dF[ 3] = ns0 * ns1 * ns2 * ns3 *  s4 *  s5;
  dF[ 4] = ns0 * ns1 * ns2 *  s3 * ns4 * ns5;
  dF[ 5] = ns0 * ns1 * ns2 *  s3 * ns4 *  s5;
  dF[ 6] = ns0 * ns1 * ns2 *  s3 *  s4 * ns5;
  dF[ 7] = ns0 * ns1 * ns2 *  s3 *  s4 *  s5;
  dF[ 8] = ns0 * ns1 *  s2 * ns3 * ns4 * ns5;
  dF[ 9] = ns0 * ns1 *  s2 * ns3 * ns4 *  s5;
  dF[10] = ns0 * ns1 *  s2 * ns3 *  s4 * ns5;
  dF[11] = ns0 * ns1 *  s2 * ns3 *  s4 *  s5;
  dF[12] = ns0 * ns1 *  s2 *  s3 * ns4 * ns5;
  dF[13] = ns0 * ns1 *  s2 *  s3 * ns4 *  s5;
  dF[14] = ns0 * ns1 *  s2 *  s3 *  s4 * ns5;
  dF[15] = ns0 * ns1 *  s2 *  s3 *  s4 *  s5;
  dF[16] = ns0 *  s1 * ns2 * ns3 * ns4 * ns5;
  dF[17] = ns0 *  s1 * ns2 * ns3 * ns4 *  s5;
  dF[18] = ns0 *  s1 * ns2 * ns3 *  s4 * ns5;
  dF[19] = ns0 *  s1 * ns2 * ns3 *  s4 *  s5;
  dF[20] = ns0 *  s1 * ns2 *  s3 * ns4 * ns5;
  dF[21] = ns0 *  s1 * ns2 *  s3 * ns4 *  s5;
  dF[22] = ns0 *  s1 * ns2 *  s3 *  s4 * ns5;
  dF[23] = ns0 *  s1 * ns2 *  s3 *  s4 *  s5;
  dF[24] = ns0 *  s1 *  s2 * ns3 * ns4 * ns5;
  dF[25] = ns0 *  s1 *  s2 * ns3 * ns4 *  s5;
  dF[26] = ns0 *  s1 *  s2 * ns3 *  s4 * ns5;
  dF[27] = ns0 *  s1 *  s2 * ns3 *  s4 *  s5;
  dF[28] = ns0 *  s1 *  s2 *  s3 * ns4 * ns5;
  dF[29] = ns0 *  s1 *  s2 *  s3 * ns4 *  s5;
  dF[30] = ns0 *  s1 *  s2 *  s3 *  s4 * ns5;
  dF[31] = ns0 *  s1 *  s2 *  s3 *  s4 *  s5;
  dF[32] =  s0 * ns1 * ns2 * ns3 * ns4 * ns5;
  dF[33] =  s0 * ns1 * ns2 * ns3 * ns4 *  s5;
  dF[34] =  s0 * ns1 * ns2 * ns3 *  s4 * ns5;
  dF[35] =  s0 * ns1 * ns2 * ns3 *  s4 *  s5;
  dF[36] =  s0 * ns1 * ns2 *  s3 * ns4 * ns5;
  dF[37] =  s0 * ns1 * ns2 *  s3 * ns4 *  s5;
  dF[38] =  s0 * ns1 * ns2 *  s3 *  s4 * ns5;
  dF[39] =  s0 * ns1 * ns2 *  s3 *  s4 *  s5;
  dF[40] =  s0 * ns1 *  s2 * ns3 * ns4 * ns5;
  dF[41] =  s0 * ns1 *  s2 * ns3 * ns4 *  s5;
  dF[42] =  s0 * ns1 *  s2 * ns3 *  s4 * ns5;
  dF[43] =  s0 * ns1 *  s2 * ns3 *  s4 *  s5;
  dF[44] =  s0 * ns1 *  s2 *  s3 * ns4 * ns5;
  dF[45] =  s0 * ns1 *  s2 *  s3 * ns4 *  s5;
  dF[46] =  s0 * ns1 *  s2 *  s3 *  s4 * ns5;
  dF[47] =  s0 * ns1 *  s2 *  s3 *  s4 *  s5;
  dF[48] =  s0 *  s1 * ns2 * ns3 * ns4 * ns5;
  dF[49] =  s0 *  s1 * ns2 * ns3 * ns4 *  s5;
  dF[50] =  s0 *  s1 * ns2 * ns3 *  s4 * ns5;
  dF[51] =  s0 *  s1 * ns2 * ns3 *  s4 *  s5;
  dF[52] =  s0 *  s1 * ns2 *  s3 * ns4 * ns5;
  dF[53] =  s0 *  s1 * ns2 *  s3 * ns4 *  s5;
  dF[54] =  s0 *  s1 * ns2 *  s3 *  s4 * ns5;
  dF[55] =  s0 *  s1 * ns2 *  s3 *  s4 *  s5;
  dF[56] =  s0 *  s1 *  s2 * ns3 * ns4 * ns5;
  dF[57] =  s0 *  s1 *  s2 * ns3 * ns4 *  s5;
  dF[58] =  s0 *  s1 *  s2 * ns3 *  s4 * ns5;
  dF[59] =  s0 *  s1 *  s2 * ns3 *  s4 *  s5;
  dF[60] =  s0 *  s1 *  s2 *  s3 * ns4 * ns5;
  dF[61] =  s0 *  s1 *  s2 *  s3 * ns4 *  s5;
  dF[62] =  s0 *  s1 *  s2 *  s3 *  s4 * ns5;
  dF[63] =  s0 *  s1 *  s2 *  s3 *  s4 *  s5;

  for (i=0; i<m_nOutput; i++, p++) {
    for (pv=0, j=0; j<64; j++)
      pv += p[m_nOffset[j]] * dF[j];

    destPixel[i] = UnitClip(pv);
  }
}


/**
 ******************************************************************************
 * Name: CIccCLUT::InterpND
 * 
 * Purpose: Generic N-dimensional interpolation function
 *
 * Args:
 *  Pixel = Pixel value to be found in the CLUT. Also used to store the result.
 *******************************************************************************
 */
void CIccCLUT::InterpND(icFloatNumber *destPixel, const icFloatNumber *srcPixel)
{
  icUInt32Number i,j, index = 0;

  for (i=0; i<m_nInput; i++) {
    m_g[i] = srcPixel[i] * m_MaxGridPoint[i];
    m_ig[i] = (icUInt32Number)m_g[i];
    m_s[m_nInput-1-i] = m_g[i] - m_ig[i];
    if (m_ig[i]==m_MaxGridPoint[i]) {
      m_ig[i]--;
      m_s[m_nInput-1-i] = 1.0;      
    }
    index += m_ig[i]*m_DimSize[i];
  }

  icFloatNumber *p = &m_pData[index];
  icFloatNumber temp[2];
  icFloatNumber pv;
  int nFlag = 0;

  for (i=0; i<m_nNodes; i++) {
    m_df[i] = 1.0;
  }


  for (i=0; i<m_nInput; i++) {
    temp[0] = (icFloatNumber)(1.0 - m_s[i]);
    temp[1] = (icFloatNumber)(m_s[i]);
    index = m_nPower[i];
    for (j=0; j<m_nNodes; j++) {
      m_df[j] *= temp[nFlag];
      if ((j+1)%index == 0)
        nFlag = !nFlag;
    }
    nFlag = 0;
  }

  for (i=0; i<m_nOutput; i++, p++) {
    for (pv=0, j=0; j<m_nNodes; j++)
      pv += p[m_nOffset[j]] * m_df[j];

    destPixel[i] = UnitClip(pv);
  }

}


/**
******************************************************************************
* Name: CIccCLUT::Validate
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
icValidateStatus CIccCLUT::Validate(icTagTypeSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/) const
{
  icValidateStatus rv = icValidateOK;

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);
  if (m_nReserved2[0]!=0 || m_nReserved2[1]!=0 || m_nReserved2[2]!=0) {
    sReport += icValidateNonCompliantMsg;
    sReport += sSigName;
    sReport += " - Reserved Value must be zero.\n";

    rv = icValidateNonCompliant;
  }

  if (sig==icSigLutAtoBType || sig==icSigLutBtoAType) {
    char temp[256];
    for (int i=0; i<m_nInput; i++) {
      if (m_GridPoints[i]<2) {
        sReport += icValidateCriticalErrorMsg;
        sReport += sSigName;
        sprintf(temp, " - CLUT: At least 2 grid points should be present in dimension %u.\n",i );
        sReport += temp;
        rv = icMaxStatus(rv, icValidateCriticalError);
      }
    }
  }

  return rv;
}


/**
 ****************************************************************************
 * Name: CIccMBB::CIccMBB
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccMBB::CIccMBB()
{
  m_nInput = 0;
  m_nOutput = 0;

  m_CurvesA = NULL;
  m_CLUT = NULL;
  m_Matrix = NULL;
  m_CurvesM = NULL;
  m_CurvesB = NULL;

  m_csInput  = icSigUnknownData;
  m_csOutput = icSigUnknownData;

  m_bInputMatrix = true;
}


/**
 ****************************************************************************
 * Name: CIccMBB::CIccMBB
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  IMBB = The CIccMBB object to be copied
 *****************************************************************************
 */
CIccMBB::CIccMBB(const CIccMBB &IMBB)
{
 icUInt8Number nCurves;
 int i;
 
  m_bInputMatrix = IMBB.m_bInputMatrix;
  m_nInput = IMBB.m_nInput;
  m_nOutput = IMBB.m_nOutput;
  m_csInput = IMBB.m_csInput;
  m_csOutput = IMBB.m_csOutput;

  if (IMBB.m_CLUT) {
    m_CLUT = new CIccCLUT(*IMBB.m_CLUT);
  }
  else
    m_CLUT = NULL;

  if (IMBB.m_CurvesA) {
    nCurves = !IsInputB() ? m_nInput : m_nOutput;

    m_CurvesA = new LPIccCurve[nCurves];
    for (i=0; i<nCurves; i++)
      m_CurvesA[i] = (CIccTagCurve*)IMBB.m_CurvesA[i]->NewCopy();
  }
  else {
    m_CurvesA = NULL;
  }

  if (IMBB.m_CurvesM) {
    nCurves = IsInputMatrix() ? m_nInput : m_nOutput;

    m_CurvesM = new LPIccCurve[nCurves];
    for (i=0; i<nCurves; i++)
      m_CurvesM[i] = (CIccTagCurve*)IMBB.m_CurvesM[i]->NewCopy();
  }
  else {
    m_CurvesM = NULL;
  }

  if (IMBB.m_CurvesB) {
    nCurves = IsInputB() ? m_nInput : m_nOutput;

    m_CurvesB = new LPIccCurve[nCurves];
    for (i=0; i<nCurves; i++)
      m_CurvesB[i] = (CIccTagCurve*)IMBB.m_CurvesB[i]->NewCopy();
  }
  else {
    m_CurvesB = NULL;
  }

  if (IMBB.m_Matrix) {
    m_Matrix = new CIccMatrix(*IMBB.m_Matrix);
  }
  else {
    m_Matrix = NULL;
  }
}


/**
 ****************************************************************************
 * Name: CIccMBB::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  IMBB = The CIccMBB object to be copied
 *****************************************************************************
 */
CIccMBB &CIccMBB::operator=(const CIccMBB &IMBB)
{
  if (&IMBB == this)
    return *this;

  Cleanup();

  icUInt8Number nCurves;
  int i;

  m_bInputMatrix = IMBB.m_bInputMatrix;
  m_nInput = IMBB.m_nInput;
  m_nOutput = IMBB.m_nOutput;
  m_csInput = IMBB.m_csInput;
  m_csOutput = IMBB.m_csOutput;

  if (IMBB.m_CLUT) {
    m_CLUT = new CIccCLUT(*IMBB.m_CLUT);
  }
  else
    m_CLUT = NULL;

  if (IMBB.m_CurvesA) {
    nCurves = !IsInputB() ? m_nInput : m_nOutput;

    m_CurvesA = new LPIccCurve[nCurves];
    for (i=0; i<nCurves; i++)
      m_CurvesA[i] = (CIccTagCurve*)IMBB.m_CurvesA[i]->NewCopy();
  }
  else {
    m_CurvesA = NULL;
  }

  if (IMBB.m_CurvesM) {
    nCurves = IsInputMatrix() ? m_nInput : m_nOutput;

    m_CurvesM = new LPIccCurve[nCurves];
    for (i=0; i<nCurves; i++)
      m_CurvesM[i] = (CIccTagCurve*)IMBB.m_CurvesM[i]->NewCopy();
  }
  else {
    m_CurvesM = NULL;
  }

  if (IMBB.m_CurvesB) {
    nCurves = IsInputB() ? m_nInput : m_nOutput;

    m_CurvesB = new LPIccCurve[nCurves];
    for (i=0; i<nCurves; i++)
      m_CurvesB[i] = (CIccTagCurve*)IMBB.m_CurvesB[i]->NewCopy();
  }
  else {
    m_CurvesB = NULL;
  }

  if (IMBB.m_Matrix) {
    m_Matrix = new CIccMatrix(*IMBB.m_Matrix);
  }
  else {
    m_Matrix = NULL;
  }

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccMBB::~CIccMBB
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccMBB::~CIccMBB()
{
  Cleanup();
}

/**
 ****************************************************************************
 * Name: CIccMBB::Cleanup
 * 
 * Purpose: Frees the memory allocated to the object
 * 
 *****************************************************************************
 */
void CIccMBB::Cleanup()
{
  int i;

  if (IsInputMatrix()) {
    if (m_CurvesB) {
      for (i=0; i<m_nInput; i++) 
        if (m_CurvesB[i])
          delete m_CurvesB[i];

      delete [] m_CurvesB;
      m_CurvesB = NULL;
    }

    if (m_CurvesM) {
      for (i=0; i<m_nInput; i++) 
        if (m_CurvesM[i])
          delete m_CurvesM[i];

      delete [] m_CurvesM;
      m_CurvesM = NULL;
    }


    if (m_CurvesA) {
      for (i=0; i<m_nOutput; i++) 
        if (m_CurvesA[i])
          delete m_CurvesA[i];

      delete [] m_CurvesA;
      m_CurvesA = NULL;
    }

  }
  else {
    if (m_CurvesA) {
      for (i=0; i<m_nInput; i++) 
        if (m_CurvesA[i])
          delete m_CurvesA[i];

      delete [] m_CurvesA;
      m_CurvesA = NULL;
    }

    if (m_CurvesM) {
      for (i=0; i<m_nOutput; i++) 
        if (m_CurvesM[i])
          delete m_CurvesM[i];

      delete [] m_CurvesM;
      m_CurvesM = NULL;
    }

    if (m_CurvesB) {
      for (i=0; i<m_nOutput; i++) 
        if (m_CurvesB[i])
          delete m_CurvesB[i];

      delete [] m_CurvesB;
      m_CurvesB = NULL;
    }
  }

  if (m_Matrix) {
    delete m_Matrix;
    m_Matrix = NULL;
  }

  if (m_CLUT) {
    delete m_CLUT;
    m_CLUT = NULL;
  }

}

/**
 ****************************************************************************
 * Name: CIccMBB::Init
 * 
 * Purpose: Cleans up any prior memory and Initializes the object.
 * 
 * Args:
 *  nInputChannels = number of input channels,
 *  nOutputChannels = number of output channels
 *****************************************************************************
 */
void CIccMBB::Init(icUInt8Number nInputChannels, icUInt8Number nOutputChannels)
{
  Cleanup();
  m_nInput = nInputChannels;
  m_nOutput = nOutputChannels;
}

/**
 ****************************************************************************
 * Name: CIccMBB::SetColorSpaces
 * 
 * Purpose: Sets the input and output color spaces
 *
 * Args:
 *  csInput = input color space signature,
 *  csOutput = output color space signature
 *****************************************************************************
 */
void CIccMBB::SetColorSpaces(icColorSpaceSignature csInput, icColorSpaceSignature csOutput)
{
   m_csInput = csInput;
   m_csOutput = csOutput;
}

/**
 ****************************************************************************
 * Name: CIccMBB::Describe
 * 
 * Purpose: Dump data associated with the tag to a string
 * 
 * Args: 
 *  sDescription - string to concatenate tag dump to
 *****************************************************************************
 */
void CIccMBB::Describe(std::string &sDescription)
{
  int i;
  icChar buf[128], color[40];


  if (IsInputMatrix()) {
    if (m_CurvesB) {
      for (i=0; i<m_nInput; i++) {
        icColorIndexName(color, m_csInput, i, m_nInput, "");
        sprintf(buf, "B_Curve_%s", color);
        m_CurvesB[i]->DumpLut(sDescription, buf, m_csInput, i);
      }
    }

    if (m_Matrix)
      m_Matrix->DumpLut(sDescription, "Matrix");

    if (m_CurvesM) {
      for (i=0; i<m_nInput; i++) {
        icColorIndexName(color, m_csInput, i, m_nInput, "");
        sprintf(buf, "M_Curve_%s", color);
        m_CurvesM[i]->DumpLut(sDescription, buf, m_csInput, i);
      }
    }

    if (m_CLUT)
      m_CLUT->DumpLut(sDescription, "CLUT", m_csInput, m_csOutput);

    if (m_CurvesA) {
      for (i=0; i<m_nOutput; i++) {
        icColorIndexName(color, m_csOutput, i, m_nOutput, "");
        sprintf(buf, "A_Curve_%s", color);
        m_CurvesA[i]->DumpLut(sDescription, buf, m_csOutput, i);
      }
    }
  }
  else {
    if (m_CurvesA) {
      for (i=0; i<m_nInput; i++) {
        icColorIndexName(color, m_csInput, i, m_nInput, "");
        sprintf(buf, "A_Curve_%s", color);
        m_CurvesA[i]->DumpLut(sDescription, buf, m_csInput, i);
      }
    }

    if (m_CLUT)
      m_CLUT->DumpLut(sDescription, "CLUT", m_csInput, m_csOutput);

    if (m_CurvesM) {
      for (i=0; i<m_nOutput; i++) {
        icColorIndexName(color, m_csOutput, i, m_nOutput, "");
        sprintf(buf, "M_Curve_%s", color);
        m_CurvesM[i]->DumpLut(sDescription, buf, m_csOutput, i);
      }
    }

    if (m_Matrix)
      m_Matrix->DumpLut(sDescription, "Matrix");

    if (m_CurvesB) {
      for (i=0; i<m_nOutput; i++) {
        icColorIndexName(color, m_csOutput, i, m_nOutput, "");
        sprintf(buf, "B_Curve_%s", color);
        m_CurvesB[i]->DumpLut(sDescription, buf, m_csOutput, i);
      }
    }
  }
}


/**
******************************************************************************
* Name: CIccMBB::Validate
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
icValidateStatus CIccMBB::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/)
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
  icUInt32Number nInput, nOutput;

  //Check # of channels 
  switch(sig) {
  case icSigAToB0Tag:
  case icSigAToB1Tag:
  case icSigAToB2Tag:
    {
      nInput = icGetSpaceSamples(pProfile->m_Header.colorSpace);
      if (m_nInput!=nInput) {
        sReport += icValidateCriticalErrorMsg;
        sReport += sSigName;
        sReport += " - Incorrect number of input channels.\n";
        rv = icMaxStatus(rv, icValidateCriticalError);
      }

      nOutput = icGetSpaceSamples(pProfile->m_Header.pcs);
      if (m_nOutput!=nOutput) {
        sReport += icValidateCriticalErrorMsg;
        sReport += sSigName;
        sReport += " - Incorrect number of output channels.\n";
        rv = icMaxStatus(rv, icValidateCriticalError);
      }

      break;
    }
  case icSigBToA0Tag:
  case icSigBToA1Tag:
  case icSigBToA2Tag:
    {
      nInput = icGetSpaceSamples(pProfile->m_Header.pcs);
      if (m_nInput!=nInput) {
        sReport += icValidateCriticalErrorMsg;
        sReport += sSigName;
        sReport += " - Incorrect number of input channels.\n";
        rv = icMaxStatus(rv, icValidateCriticalError);
      }

      nOutput = icGetSpaceSamples(pProfile->m_Header.colorSpace);
      if (m_nOutput!=nOutput) {
        sReport += icValidateCriticalErrorMsg;
        sReport += sSigName;
        sReport += " - Incorrect number of output channels.\n";
        rv = icMaxStatus(rv, icValidateCriticalError);
      }

      break;
    }
  case icSigGamutTag:
    {
      nInput = 1;
      if (m_nInput!=nInput) {
        sReport += icValidateCriticalErrorMsg;
        sReport += sSigName;
        sReport += " - Incorrect number of input channels.\n";
        rv = icMaxStatus(rv, icValidateCriticalError);
      }

      nOutput = icGetSpaceSamples(pProfile->m_Header.colorSpace);
      if (m_nOutput!=nOutput) {
        sReport += icValidateCriticalErrorMsg;
        sReport += sSigName;
        sReport += " - Incorrect number of output channels.\n";
        rv = icMaxStatus(rv, icValidateCriticalError);
      }

      break;
    }
  default:
    {
      nInput = m_nInput;
      nOutput = m_nOutput;
    }
  }

  //CLUT check
  if (nInput!=nOutput) {
    if (!m_CLUT) {
      sReport += icValidateCriticalErrorMsg;
      sReport += sSigName;
      sReport += " - CLUT must be present.\n";
      rv = icMaxStatus(rv, icValidateCriticalError);          
    }
  }

  if (m_CLUT) {
    rv = icMaxStatus(rv, m_CLUT->Validate(GetType(), sReport, pProfile));
  }

  return rv;
}

/**
 ****************************************************************************
 * Name: CIccMBB::NewCurvesA
 * 
 * Purpose: Allocates memory for a new set of A-curves
 *
 * Return: Pointer to the LPIccCurve object
 *****************************************************************************
 */
LPIccCurve* CIccMBB::NewCurvesA()
{
  if (m_CurvesA)
    return m_CurvesA;

  icUInt8Number nCurves = !IsInputB() ? m_nInput : m_nOutput;

  m_CurvesA = new LPIccCurve[nCurves];
  memset(m_CurvesA, 0, nCurves * sizeof(LPIccCurve));

  return m_CurvesA;
}


/**
 ****************************************************************************
 * Name: CIccMBB::NewCurvesM
 * 
 * Purpose: Allocates memory for a new set of M-curves
 *
 * Return: Pointer to the LPIccCurve object
 *****************************************************************************
 */
LPIccCurve* CIccMBB::NewCurvesM()
{
  if (m_CurvesM)
    return m_CurvesM;

  icUInt8Number nCurves = IsInputMatrix() ? m_nInput : m_nOutput;

  m_CurvesM = new LPIccCurve[nCurves];
  memset(m_CurvesM, 0, nCurves * sizeof(LPIccCurve));

  return m_CurvesM;
}

/**
 ****************************************************************************
 * Name: CIccMBB::NewCurvesB
 * 
 * Purpose: Allocates memory for a new set of B-curves
 *
 * Return: Pointer to the LPIccCurve object
 *****************************************************************************
 */
LPIccCurve* CIccMBB::NewCurvesB()
{
  if (m_CurvesB)
    return m_CurvesB;

  icUInt8Number nCurves = IsInputB() ? m_nInput : m_nOutput;

  m_CurvesB = new LPIccCurve[nCurves];
  memset(m_CurvesB, 0, nCurves * sizeof(LPIccCurve));

  return m_CurvesB;
}

/**
 ****************************************************************************
 * Name: CIccMBB::NewMatrix
 * 
 * Purpose: Allocates memory for a new matrix
 *
 * Return: Pointer to the CIccMatrix object
 *****************************************************************************
 */
CIccMatrix* CIccMBB::NewMatrix()
{
  if (m_Matrix)
    return m_Matrix;

  m_Matrix = new CIccMatrix;

  return m_Matrix;
}

/**
 ****************************************************************************
 * Name: CIccMBB::NewCLUT
 * 
 * Purpose: Allocates memory for a new CLUT and initializes it
 *
 * Args:
 *  pGridPoints = number of grid points in the CLUT
 *
 * Return: Pointer to the CIccCLUT object
 *****************************************************************************
 */
CIccCLUT* CIccMBB::NewCLUT(icUInt8Number *pGridPoints, icUInt8Number nPrecision/*=2*/)
{
  if (m_CLUT)
    return m_CLUT;

  m_CLUT = new CIccCLUT(m_nInput, m_nOutput, nPrecision);

  m_CLUT->Init(pGridPoints);

  return m_CLUT;
}

/**
 ****************************************************************************
 * Name: CIccMBB::NewCLUT
 * 
 * Purpose: Allocates memory for a new CLUT and initializes it
 *
 * Args:
 *  nGridPoints = number of grid points in the CLUT
 *
 * Return: Pointer to the CIccCLUT object
 *****************************************************************************
 */
CIccCLUT* CIccMBB::NewCLUT(icUInt8Number nGridPoints, icUInt8Number nPrecision/*=2*/)
{
  if (m_CLUT)
    return m_CLUT;

  m_CLUT = new CIccCLUT(m_nInput, m_nOutput, nPrecision);

  m_CLUT->Init(nGridPoints);

  return m_CLUT;
}


/**
 ****************************************************************************
 * Name: CIccTagLutAtoB::CIccTagLutAtoB
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagLutAtoB::CIccTagLutAtoB()
{
  m_bInputMatrix = false;
  m_nReservedWord = 0;
}


/**
 ****************************************************************************
 * Name: CIccTagLutAtoB::CIccTagLutAtoB
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITLA2B = The CIccTagLutAtoB object to be copied
 *****************************************************************************
 */
CIccTagLutAtoB::CIccTagLutAtoB(const CIccTagLutAtoB &ITLA2B) : CIccMBB(ITLA2B)
{
  m_nReservedWord = 0;
}


/**
 ****************************************************************************
 * Name: CIccTagLutAtoB::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ITLA2B = The CIccTagLutAtoB object to be copied
 *****************************************************************************
 */
CIccTagLutAtoB &CIccTagLutAtoB::operator=(const CIccTagLutAtoB &ITLA2B)
{
  if (&ITLA2B == this)
    return *this;

  CIccMBB::operator=(ITLA2B);

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagLutAtoB::~CIccTagLutAtoB
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagLutAtoB::~CIccTagLutAtoB()
{
}


/**
 ****************************************************************************
 * Name: CIccTagLutAtoB::Read
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
bool CIccTagLutAtoB::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt32Number Offset[5], nStart, nEnd, nPos;
  icUInt8Number nCurves, i;

  if (size<8*sizeof(icUInt32Number) || !pIO) {
    return false;
  }

  nStart = pIO->Tell();
  nEnd = nStart + size;

  if (!pIO->Read32(&sig) ||
      !pIO->Read32(&m_nReserved) ||
      !pIO->Read8(&m_nInput) ||
      !pIO->Read8(&m_nOutput) ||
      !pIO->Read16(&m_nReservedWord) ||
      pIO->Read32(Offset, 5)!=5)
    return false;

  if (sig!=GetType())
    return false;

  //B Curves
  if (Offset[0]) {
    nCurves = IsInputB() ? m_nInput : m_nOutput;
    LPIccCurve *pCurves = NewCurvesB();

    if (pIO->Seek(nStart + Offset[0], icSeekSet)<0)
      return false;

    for (i=0; i<nCurves; i++) {
      nPos = pIO->Tell();

      if (!pIO->Read32(&sig))
        return false;

      if (pIO->Seek(nPos, icSeekSet)<0)
        return false;

      if (sig!=icSigCurveType &&
          sig!=icSigParametricCurveType)
        return false;

      pCurves[i] = (CIccCurve*)CIccTag::Create(sig);

      if (!pCurves[i]->Read(nEnd - pIO->Tell(), pIO))
        return false;

      if (!pIO->Sync32(Offset[1]))
        return false;
    }
  }

  //Matrix
  if (Offset[1]) {
    icS15Fixed16Number tmp;

    if (Offset[1] + 12*sizeof(icS15Fixed16Number) >size)
      return false;

    m_Matrix = new CIccMatrix();

    if (pIO->Seek(nStart + Offset[1], icSeekSet)<0)
      return false;

    for (i=0; i<12; i++) {
       if (pIO->Read32(&tmp, 1)!=1)
        return false;
      m_Matrix->m_e[i] = icFtoD(tmp);
    }
  }


  //M Curves
  if (Offset[2]) {
    nCurves = IsInputMatrix() ? m_nInput : m_nOutput;
    LPIccCurve *pCurves = NewCurvesM();

    if (pIO->Seek(nStart + Offset[2], icSeekSet)<0)
      return false;

    for (i=0; i<nCurves; i++) {
      nPos = pIO->Tell();

      if (!pIO->Read32(&sig))
        return false;

      if (pIO->Seek(nPos, icSeekSet)<0)
        return false;

      if (sig!=icSigCurveType &&
          sig!=icSigParametricCurveType)
        return false;

      pCurves[i] = (CIccCurve*)CIccTag::Create(sig);

      if (!pCurves[i]->Read(nEnd - pIO->Tell(), pIO))
        return false;

      if (!pIO->Sync32(Offset[2]))
        return false;
    }
  }

  //CLUT
  if (Offset[3]) {
    if (pIO->Seek(nStart + Offset[3], icSeekSet)<0)
      return false;

    m_CLUT = new CIccCLUT(m_nInput, m_nOutput);

    if (!m_CLUT->Read(nEnd - pIO->Tell(), pIO))
      return false;
  }

  //A Curves
  if (Offset[4]) {
    nCurves = !IsInputB() ? m_nInput : m_nOutput;
    LPIccCurve *pCurves = NewCurvesA();

    if (pIO->Seek(nStart + Offset[4], icSeekSet)<0)
      return false;

    for (i=0; i<nCurves; i++) {
      nPos = pIO->Tell();

      if (!pIO->Read32(&sig))
        return false;

      if (pIO->Seek(nPos, icSeekSet)<0)
        return false;

      if (sig!=icSigCurveType &&
          sig!=icSigParametricCurveType)
        return false;

      pCurves[i] = (CIccCurve*)CIccTag::Create(sig);

      if (!pCurves[i]->Read(nEnd - pIO->Tell(), pIO))
        return false;

      if (!pIO->Sync32(Offset[4]))
        return false;
    }
  }
  return true;
}



/**
 ****************************************************************************
 * Name: CIccTagLutAtoB::Write
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
bool CIccTagLutAtoB::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();
  icUInt32Number Offset[5], nStart, nEnd, nOffsetPos;
  icUInt8Number nCurves, i;

  nStart = pIO->Tell();
  memset(&Offset[0], 0, sizeof(Offset));
  
  if (!pIO->Write32(&sig) ||
      !pIO->Write32(&m_nReserved) ||
      !pIO->Write8(&m_nInput) ||
      !pIO->Write8(&m_nOutput) ||
      !pIO->Write16(&m_nReservedWord))
    return false;

  nOffsetPos = pIO->Tell();
  if (pIO->Write32(Offset, 5)!=5)
    return false;

  //B Curves
  if (m_CurvesB) {
    Offset[0] = pIO->Tell() - nStart;
    nCurves = IsInputB() ? m_nInput : m_nOutput;

    for (i=0; i<nCurves; i++) {
      if (!m_CurvesB[i])
        return false;

      if (!m_CurvesB[i]->Write(pIO))
        return false;

      if (!pIO->Align32())
        return false;
    }
  }

  //Matrix
  if (m_Matrix) {
    icS15Fixed16Number tmp;

    Offset[1] = pIO->Tell() - nStart;

    for (i=0; i<12; i++) {
      tmp = icDtoF(m_Matrix->m_e[i]);
      if (pIO->Write32(&tmp, 1)!=1)
        return false;
    }
  }


  //M Curves
  if (m_CurvesM) {
    Offset[2] = pIO->Tell() - nStart;
    nCurves = IsInputMatrix() ? m_nInput : m_nOutput;

    for (i=0; i<nCurves; i++) {
      if (!m_CurvesM[i])
        return false;

      if (!m_CurvesM[i]->Write(pIO))
        return false;

      if (!pIO->Align32())
        return false;
    }
  }

  //CLUT
  if (m_CLUT) {
    Offset[3] = pIO->Tell() - nStart;

    if (!m_CLUT->Write(pIO))
      return false;

    if (!pIO->Align32())
      return false;
  }

  //A Curves
  if (m_CurvesA) {
    Offset[4] = pIO->Tell() - nStart;
    nCurves = !IsInputB() ? m_nInput : m_nOutput;

    for (i=0; i<nCurves; i++) {
      if (!m_CurvesA[i])
        return false;

      if (!m_CurvesA[i]->Write(pIO))
        return false;

      if (!pIO->Align32())
        return false;
    }
  }

  nEnd = pIO->Tell();

  if (!pIO->Seek(nOffsetPos, icSeekSet))
    return false;

  if (pIO->Write32(&Offset[0], 5)!=5)
    return false;

  return pIO->Seek(nEnd, icSeekSet)>=0;
}


/**
******************************************************************************
* Name: CIccTagLutAtoB::Validate
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
icValidateStatus CIccTagLutAtoB::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/)
{
  icValidateStatus rv = CIccMBB::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (!pProfile) {
    return rv;
  }

  switch(sig) {
  case icSigAToB0Tag:
  case icSigAToB1Tag:
  case icSigAToB2Tag:
    {
      icUInt32Number nInput = icGetSpaceSamples(pProfile->m_Header.colorSpace);

      icUInt32Number nOutput = icGetSpaceSamples(pProfile->m_Header.pcs);

      icUInt8Number i;
      if (m_CurvesB) {
        for (i=0; i<nOutput; i++) {
          if (m_CurvesB[i]) {
            rv = icMaxStatus(rv, m_CurvesB[i]->Validate(sig, sReport, pProfile));
          }
          else {
            sReport += icValidateCriticalErrorMsg;
            sReport += sSigName;
            sReport += " - Incorrect number of B-curves.\n";
            rv = icMaxStatus(rv, icValidateCriticalError);
          }
        }
      }

      if (m_CurvesM) {
        for (i=0; i<nOutput; i++) {
          if (m_CurvesM[i]) {
            rv = icMaxStatus(rv, m_CurvesM[i]->Validate(sig, sReport, pProfile));
          }
          else {
            sReport += icValidateCriticalErrorMsg;
            sReport += sSigName;
            sReport += " - Incorrect number of M-curves.\n";
            rv = icMaxStatus(rv, icValidateCriticalError);
          }
        }
      }

      if (m_CurvesA) {
        if (!m_CLUT) {
          sReport += icValidateNonCompliantMsg;
          sReport += sSigName;
          sReport += " - CLUT must be present if using A-curves.\n";

          rv = icMaxStatus(rv, icValidateNonCompliant);
        }

        for (i=0; i<nInput; i++) {
          if (m_CurvesA[i]) {
            rv = icMaxStatus(rv, m_CurvesA[i]->Validate(sig, sReport, pProfile));
          }
          else {
            sReport += icValidateCriticalErrorMsg;
            sReport += sSigName;
            sReport += " - Incorrect number of A-curves.\n";
            rv = icMaxStatus(rv, icValidateCriticalError);
          }
        }

      }

      break;
    }
  default:
    {
    }
  }


  return rv;
}

/**
 ****************************************************************************
 * Name: CIccTagLutBtoA::CIccTagLutBtoA
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagLutBtoA::CIccTagLutBtoA()
{
  m_bInputMatrix = true;
}


/**
 ****************************************************************************
 * Name: CIccTagLutBtoA::CIccTagLutBtoA
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITLB2A = The CIccTagLutBtoA object to be copied
 *****************************************************************************
 */
CIccTagLutBtoA::CIccTagLutBtoA(const CIccTagLutBtoA &ITLB2A) : CIccTagLutAtoB(ITLB2A)
{
}


/**
 ****************************************************************************
 * Name: CIccTagLutBtoA::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ITLB2A = The CIccTagLutBtoA object to be copied
 *****************************************************************************
 */
CIccTagLutBtoA &CIccTagLutBtoA::operator=(const CIccTagLutBtoA &ITLB2A)
{
  if (&ITLB2A == this)
    return *this;

  CIccMBB::operator=(ITLB2A);

  return *this;
}


/**
******************************************************************************
* Name: CIccTagLutBtoA::Validate
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
icValidateStatus CIccTagLutBtoA::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/)
{
  icValidateStatus rv = CIccMBB::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (!pProfile) {
    sReport += icValidateWarningMsg;
    sReport += sSigName;
    sReport += " - Tag validation incomplete: Pointer to profile unavailable.\n";
    rv = icMaxStatus(rv, icValidateCriticalError);
    return rv;
  }

  switch(sig) {
  case icSigBToA0Tag:
  case icSigBToA1Tag:
  case icSigBToA2Tag:
  case icSigGamutTag:
    {
      icUInt32Number nInput = icGetSpaceSamples(pProfile->m_Header.pcs);

      icUInt32Number nOutput;
      if (sig==icSigGamutTag) {
        nOutput = 1;
      }
      else {
        nOutput = icGetSpaceSamples(pProfile->m_Header.colorSpace);
      }

      if (m_nOutput!=nOutput) {
        sReport += icValidateCriticalErrorMsg;
        sReport += sSigName;
        sReport += " - Incorrect number of output channels.\n";
        rv = icMaxStatus(rv, icValidateCriticalError);
      }

      icUInt8Number i;
      if (m_CurvesB) {
        for (i=0; i<nInput; i++) {
          if (m_CurvesB[i]) {
            rv = icMaxStatus(rv, m_CurvesB[i]->Validate(sig, sReport, pProfile));
          }
          else {
            sReport += icValidateCriticalErrorMsg;
            sReport += sSigName;
            sReport += " - Incorrect number of B-curves.\n";
            rv = icMaxStatus(rv, icValidateCriticalError);
          }
        }
      }

      if (m_CurvesM) {
        for (i=0; i<nInput; i++) {
          if (m_CurvesM[i]) {
            rv = icMaxStatus(rv, m_CurvesM[i]->Validate(sig, sReport, pProfile));
          }
          else {
            sReport += icValidateCriticalErrorMsg;
            sReport += sSigName;
            sReport += " - Incorrect number of M-curves.\n";
            rv = icMaxStatus(rv, icValidateCriticalError);
          }
        }
      }

      if (m_CurvesA) {
        if (!m_CLUT) {
          sReport += icValidateNonCompliantMsg;
          sReport += sSigName;
          sReport += " - CLUT must be present if using A-curves.\n";

          rv = icMaxStatus(rv, icValidateNonCompliant);
        }

        for (i=0; i<nOutput; i++) {
          if (m_CurvesA[i]) {
            rv = icMaxStatus(rv, m_CurvesA[i]->Validate(sig, sReport, pProfile));
          }
          else {
            sReport += icValidateCriticalErrorMsg;
            sReport += sSigName;
            sReport += " - Incorrect number of A-curves.\n";
            rv = icMaxStatus(rv, icValidateCriticalError);
          }
        }

      }

      break;
    }
  default:
    {
    }
  }


  return rv;
}


/**
 ****************************************************************************
 * Name: CIccTagLut8::CIccTagLut8
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagLut8::CIccTagLut8()
{
  memset(m_XYZMatrix, 0, sizeof(m_XYZMatrix));
  m_nReservedByte = 0;
}


/**
 ****************************************************************************
 * Name: CIccTagLut8::CIccTagLut8
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITL = The CIccTagLut8 object to be copied
 *****************************************************************************
 */
CIccTagLut8::CIccTagLut8(const CIccTagLut8& ITL) : CIccMBB(ITL)
{
  memcpy(&m_XYZMatrix, &ITL.m_XYZMatrix, sizeof(m_XYZMatrix));
  m_nReservedByte = 0;
}


/**
 ****************************************************************************
 * Name: CIccTagLut8::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ITL = The CIccTagLut8 object to be copied
 *****************************************************************************
 */
CIccTagLut8 &CIccTagLut8::operator=(const CIccTagLut8 &ITL) 
{
  if (&ITL==this)
    return *this;

  CIccMBB::operator=(ITL);
  memcpy(&m_XYZMatrix, &ITL.m_XYZMatrix, sizeof(m_XYZMatrix));

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagLut8::~CIccTagLut8
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagLut8::~CIccTagLut8()
{
}


/**
 ****************************************************************************
 * Name: CIccTagLut8::Read
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
bool CIccTagLut8::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt32Number nStart, nEnd;
  icUInt8Number i, nGrid;
  LPIccCurve *pCurves;
  CIccTagCurve *pCurve;

  if (size<13*sizeof(icUInt32Number) || !pIO) {
    return false;
  }

  nStart = pIO->Tell();
  nEnd = nStart + size;
 
  if (!pIO->Read32(&sig) ||
      !pIO->Read32(&m_nReserved) ||
      !pIO->Read8(&m_nInput) ||
      !pIO->Read8(&m_nOutput) ||
      !pIO->Read8(&nGrid) ||
      !pIO->Read8(&m_nReservedByte) ||
      pIO->Read32(m_XYZMatrix, 9) != 9)
    return false;

  if (sig!=GetType())
    return false;

  //B Curves
  pCurves = NewCurvesB();

  for (i=0; i<m_nInput; i++) {
    if (256 > nEnd - pIO->Tell())
      return false;

    pCurves[i] = pCurve = (CIccTagCurve*)CIccTag::Create(icSigCurveType);

    pCurve->SetSize(256);

    if (pIO->Read8Float(&(*pCurve)[0], 256) != 256)
      return false;                                                                       
  }

  //CLUT
  m_CLUT = new CIccCLUT(m_nInput, m_nOutput);

  m_CLUT->Init(nGrid);

  if (!m_CLUT->ReadData(nEnd - pIO->Tell(), pIO, 1))
    return false;

  //A Curves
  pCurves = NewCurvesA();

  for (i=0; i<m_nOutput; i++) {
    if (256 > nEnd - pIO->Tell())
      return false;

    pCurves[i] = pCurve = (CIccTagCurve*)CIccTag::Create(icSigCurveType);

    pCurve->SetSize(256);

    if (pIO->Read8Float(&(*pCurve)[0], 256) != 256)
      return false;                                                                       
  }
  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagLut8::SetColorSpaces
 * 
 * Purpose: Sets the input and output color spaces
 * 
 * Args: 
 *  csInput = input color space signature,
 *  csOutput = output color space signature
 * 
 *****************************************************************************
 */
void CIccTagLut8::SetColorSpaces(icColorSpaceSignature csInput, icColorSpaceSignature csOutput)
{
  if (csInput==icSigXYZData) {
    if (!m_CurvesM && !m_Matrix) {
      int i;

      m_CurvesM = m_CurvesB;

      CIccMatrix *pMatrix = NewMatrix();
      for (i=0; i<9; i++) {
        pMatrix->m_e[i] = icFtoD(m_XYZMatrix[i]);
      }
 
      pMatrix->m_bUseConstants=false;

      LPIccCurve *pCurves = NewCurvesB();
      CIccTagCurve *pCurve;
      for (i=0; i<m_nInput; i++) {
        pCurves[i] = pCurve = new CIccTagCurve();
        pCurve->SetSize(0);
      }
    }
  }
  else {
    m_XYZMatrix[0] = m_XYZMatrix[4] = m_XYZMatrix[8] = icDtoF(1.0);
    m_XYZMatrix[1] = m_XYZMatrix[2] = m_XYZMatrix[3] =
    m_XYZMatrix[5] = m_XYZMatrix[6] = m_XYZMatrix[7] = 0;
  }

  CIccMBB::SetColorSpaces(csInput, csOutput);
}


/**
 ****************************************************************************
 * Name: CIccTagLut8::Write
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
bool CIccTagLut8::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();
  icUInt8Number i, nGrid;
  icS15Fixed16Number XYZMatrix[9];
  icUInt16Number nInputEntries, nOutputEntries;
  LPIccCurve *pCurves;
  CIccTagCurve *pCurve;

  if (m_Matrix) {
    for (i=0; i<9; i++)
      XYZMatrix[i] = icDtoF(m_Matrix->m_e[i]);
  }
  else {
    memset(XYZMatrix, 0, 9*sizeof(icS15Fixed16Number));
    XYZMatrix[0] = XYZMatrix[4] = XYZMatrix[8] = icDtoF(1.0);
  }
  pCurves = m_CurvesB;

  if (!pCurves || !m_CurvesA || !m_CLUT)
    return false;

  nGrid = m_CLUT->GridPoints();

  nInputEntries = (icUInt16Number)(((CIccTagCurve*)pCurves[0])->GetSize());
  nOutputEntries = (icUInt16Number)(((CIccTagCurve*)m_CurvesA[0])->GetSize());

  if (!pIO->Write32(&sig) ||
      !pIO->Write32(&m_nReserved) ||
      !pIO->Write8(&m_nInput) ||
      !pIO->Write8(&m_nOutput) ||
      !pIO->Write8(&nGrid) ||
      !pIO->Write8(&m_nReservedByte) ||
      pIO->Write32(XYZMatrix, 9) != 9)
    return false;

  //B Curves
  for (i=0; i<m_nInput; i++) {
    if (pCurves[i]->GetType()!=icSigCurveType)
      return false;

    pCurve = (CIccTagCurve*)pCurves[i];
    if (!pCurve)
      return false;

    if (pCurve->GetSize()!=256)
      return false;

    if (pIO->Write8Float(&(*pCurve)[0], 256) != 256)
      return false;                                                                       
  }

  //CLUT
  if (!m_CLUT->WriteData(pIO, 1))
    return false;

  //A Curves
  pCurves = m_CurvesA;

  for (i=0; i<m_nOutput; i++) {
    if (pCurves[i]->GetType()!=icSigCurveType)
      return false;

    pCurve = (CIccTagCurve*)pCurves[i];

    if (!pCurve)
      return false;

    if (pCurve->GetSize()!=256)
      return false;

    if (pIO->Write8Float(&(*pCurve)[0], 256) != 256)
      return false;                                                                       
  }
  return true;
}


/**
******************************************************************************
* Name: CIccTagLut8::Validate
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
icValidateStatus CIccTagLut8::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/)
{
  icValidateStatus rv = CIccMBB::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (!pProfile) {
    return rv;
  }

  switch(sig) {
  case icSigAToB0Tag:
  case icSigAToB1Tag:
  case icSigAToB2Tag:
  case icSigBToA0Tag:
  case icSigBToA1Tag:
  case icSigBToA2Tag:
  case icSigGamutTag:
    {
      icUInt32Number nInput, nOutput;
      if (sig==icSigAToB0Tag || sig==icSigAToB1Tag || sig==icSigAToB2Tag || sig==icSigGamutTag) {
        nInput = icGetSpaceSamples(pProfile->m_Header.pcs);
        nOutput = icGetSpaceSamples(pProfile->m_Header.colorSpace);
      }
      else {
        nInput = icGetSpaceSamples(pProfile->m_Header.colorSpace);
        nOutput = icGetSpaceSamples(pProfile->m_Header.pcs);
      }

      if (sig==icSigGamutTag) {
        nOutput = 1;
      }

      icUInt8Number i;
      if (m_CurvesB) {
        for (i=0; i<nInput; i++) {
          if (m_CurvesB[i]) {
            rv = icMaxStatus(rv, m_CurvesB[i]->Validate(sig, sReport, pProfile));
            if (m_CurvesB[i]->GetType()==icSigCurveType) {
              CIccTagCurve *pTagCurve = (CIccTagCurve*)m_CurvesB[i];
              if (pTagCurve->GetSize()==1) {
                sReport += icValidateCriticalErrorMsg;
                sReport += sSigName;
                sReport += " - lut8Tags do not support single entry gamma curves.\n";
                rv = icMaxStatus(rv, icValidateCriticalError);
              }
            }
          }
          else {
            sReport += icValidateCriticalErrorMsg;
            sReport += sSigName;
            sReport += " - Incorrect number of B-curves.\n";
            rv = icMaxStatus(rv, icValidateCriticalError);
          }
        }
      }

      if (m_Matrix) {
        rv = icMaxStatus(rv, m_Matrix->Validate(GetType(), sReport, pProfile));
      }
      else {
        int sum=0;
        for (int i=0; i<9; i++) {
          sum += m_XYZMatrix[i];
        }
        if (m_XYZMatrix[0]!=1.0 || m_XYZMatrix[4]!=1.0 || m_XYZMatrix[9]!=1.0 || sum!=3.0) {
          sReport += icValidateWarningMsg;
          sReport += sSigName;
          sReport += " - Matrix must be identity.\n";
          rv = icMaxStatus(rv, icValidateWarning);
        }
      }

      if (m_CurvesA) {

        for (i=0; i<nOutput; i++) {
          if (m_CurvesA[i]) {
            rv = icMaxStatus(rv, m_CurvesA[i]->Validate(sig, sReport, pProfile));
            if (m_CurvesA[i]->GetType()==icSigCurveType) {
              CIccTagCurve *pTagCurve = (CIccTagCurve*)m_CurvesA[i];
              if (pTagCurve->GetSize()==1) {
                sReport += icValidateCriticalErrorMsg;
                sReport += sSigName;
                sReport += " - lut8Tags do not support single entry gamma curves.\n";
                rv = icMaxStatus(rv, icValidateCriticalError);
              }
            }
          }
          else {
            sReport += icValidateCriticalErrorMsg;
            sReport += sSigName;
            sReport += " - Incorrect number of A-curves.\n";
            rv = icMaxStatus(rv, icValidateCriticalError);
          }
        }

      }

      break;
    }
  default:
    {
    }
  }


  return rv;
}


/**
 ****************************************************************************
 * Name: CIccTagLut16::CIccTagLut16
 * 
 * Purpose: Constructor
 * 
 *****************************************************************************
 */
CIccTagLut16::CIccTagLut16()
{
  memset(m_XYZMatrix, 0, sizeof(m_XYZMatrix));
  m_nReservedByte = 0;
}


/**
 ****************************************************************************
 * Name: CIccTagLut16::CIccTagLut16
 * 
 * Purpose: Copy Constructor
 *
 * Args:
 *  ITL = The CIccTagUnknown object to be copied
 *****************************************************************************
 */
CIccTagLut16::CIccTagLut16(const CIccTagLut16& ITL) : CIccMBB(ITL)
{
  memcpy(&m_XYZMatrix, &ITL.m_XYZMatrix, sizeof(m_XYZMatrix));
  m_nReservedByte = 0;
}


/**
 ****************************************************************************
 * Name: CIccTagLut16::operator=
 * 
 * Purpose: Copy Operator
 *
 * Args:
 *  ITL = The CIccTagLut16 object to be copied
 *****************************************************************************
 */
CIccTagLut16 &CIccTagLut16::operator=(const CIccTagLut16 &ITL) 
{
  if (&ITL==this)
    return *this;

  CIccMBB::operator=(ITL);
  memcpy(&m_XYZMatrix, &ITL.m_XYZMatrix, sizeof(m_XYZMatrix));

  return *this;
}


/**
 ****************************************************************************
 * Name: CIccTagLut16::~CIccTagLut16
 * 
 * Purpose: Destructor
 * 
 *****************************************************************************
 */
CIccTagLut16::~CIccTagLut16()
{
}


/**
 ****************************************************************************
 * Name: CIccTagLut16::Read
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
bool CIccTagLut16::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  icUInt32Number nStart, nEnd;
  icUInt8Number i, nGrid;
  icUInt16Number nInputEntries, nOutputEntries;
  LPIccCurve *pCurves;
  CIccTagCurve *pCurve;

  if (size<13*sizeof(icUInt32Number) || !pIO) {
    return false;
  }

  nStart = pIO->Tell();
  nEnd = nStart + size;
 
  if (!pIO->Read32(&sig) ||
      !pIO->Read32(&m_nReserved) ||
      !pIO->Read8(&m_nInput) ||
      !pIO->Read8(&m_nOutput) ||
      !pIO->Read8(&nGrid) ||
      !pIO->Read8(&m_nReservedByte) ||
      pIO->Read32(m_XYZMatrix, 9) != 9 ||
      !pIO->Read16(&nInputEntries) ||
      !pIO->Read16(&nOutputEntries))
    return false;

  if (sig!=GetType())
    return false;


  //B Curves
  pCurves = NewCurvesB();

  for (i=0; i<m_nInput; i++) {
    if (nInputEntries*sizeof(icUInt16Number) > nEnd - pIO->Tell())
      return false;

    pCurves[i] = pCurve = (CIccTagCurve*)CIccTag::Create(icSigCurveType);

    pCurve->SetSize(nInputEntries);

    if (pIO->Read16Float(&(*pCurve)[0], nInputEntries) != nInputEntries)
      return false;
  }

  //CLUT
  m_CLUT = new CIccCLUT(m_nInput, m_nOutput);

  m_CLUT->Init(nGrid);

  if (!m_CLUT->ReadData(nEnd - pIO->Tell(), pIO, 2))
    return false;

  //A Curves
  pCurves = NewCurvesA();

  for (i=0; i<m_nOutput; i++) {
    if (nOutputEntries*sizeof(icUInt16Number) > nEnd - pIO->Tell())
      return false;

    pCurves[i] = pCurve = (CIccTagCurve*)CIccTag::Create(icSigCurveType);

    pCurve->SetSize(nOutputEntries);

    if (pIO->Read16Float(&(*pCurve)[0], nOutputEntries) != nOutputEntries)
      return false;
  }
  return true;
}


/**
 ****************************************************************************
 * Name: CIccTagLut16::SetColorSpaces
 * 
 * Purpose: Sets the input and output color spaces
 * 
 * Args: 
 *  csInput = input color space signature,
 *  csOutput = output color space signature
 * 
 *****************************************************************************
 */
void CIccTagLut16::SetColorSpaces(icColorSpaceSignature csInput, icColorSpaceSignature csOutput)
{
  if (csInput==icSigXYZData) {
    if (!m_CurvesM && !m_Matrix) {
      int i;

      m_CurvesM = m_CurvesB;
      m_CurvesB = NULL;

      CIccMatrix *pMatrix = NewMatrix();
    for (i=0; i<9; i++) {
      pMatrix->m_e[i] = icFtoD(m_XYZMatrix[i]);
    }

      pMatrix->m_bUseConstants=false;

      LPIccCurve *pCurves = NewCurvesB();
      CIccTagCurve *pCurve;
      for (i=0; i<m_nInput; i++) {
        pCurves[i] = pCurve = new CIccTagCurve();
        pCurve->SetSize(0);
      }
    }
  }
  else {
    m_XYZMatrix[0] = m_XYZMatrix[4] = m_XYZMatrix[8] = icDtoF(1.0);
    m_XYZMatrix[1] = m_XYZMatrix[2] = m_XYZMatrix[3] =
    m_XYZMatrix[5] = m_XYZMatrix[6] = m_XYZMatrix[7] = 0;
  }

  CIccMBB::SetColorSpaces(csInput, csOutput);
}


/**
 ****************************************************************************
 * Name: CIccTagLut16::Write
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
bool CIccTagLut16::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();
  icUInt8Number i, nGrid;
  icS15Fixed16Number XYZMatrix[9];
  icUInt16Number nInputEntries, nOutputEntries;
  LPIccCurve *pCurves;
  CIccTagCurve *pCurve;

  if (m_Matrix) {
    for (i=0; i<9; i++) {
      XYZMatrix[i] = icDtoF(m_Matrix->m_e[i]);
    }
  }
  else {
    memset(XYZMatrix, 0, 9*sizeof(icS15Fixed16Number));
    XYZMatrix[0] = XYZMatrix[4] = XYZMatrix[8] = icDtoF(1.0);
  }
  pCurves = m_CurvesB;

  if (!pCurves || !m_CurvesA || !m_CLUT)
    return false;

  nGrid = m_CLUT->GridPoints();

  nInputEntries = (icUInt16Number)(((CIccTagCurve*)pCurves[0])->GetSize());
  nOutputEntries = (icUInt16Number)(((CIccTagCurve*)m_CurvesA[0])->GetSize());

  if (!pIO->Write32(&sig) ||
      !pIO->Write32(&m_nReserved) ||
      !pIO->Write8(&m_nInput) ||
      !pIO->Write8(&m_nOutput) ||
      !pIO->Write8(&nGrid) ||
      !pIO->Write8(&m_nReservedByte) ||
      pIO->Write32(XYZMatrix, 9) != 9 ||
      !pIO->Write16(&nInputEntries) ||
      !pIO->Write16(&nOutputEntries))
    return false;

  //B Curves
  for (i=0; i<m_nInput; i++) {
    if (pCurves[i]->GetType()!=icSigCurveType)
      return false;

    pCurve = (CIccTagCurve*)pCurves[i];
    if (!pCurve)
      return false;

    if (pIO->Write16Float(&(*pCurve)[0], nInputEntries) != nInputEntries)
      return false;
  }

  //CLUT
  if (!m_CLUT->WriteData(pIO, 2))
    return false;

  //A Curves
  pCurves = m_CurvesA;

  for (i=0; i<m_nOutput; i++) {
    if (pCurves[i]->GetType()!=icSigCurveType)
      return false;

    pCurve = (CIccTagCurve*)pCurves[i];

    if (pIO->Write16Float(&(*pCurve)[0], nOutputEntries) != nOutputEntries)
      return false;
  }
  return true;
}


/**
******************************************************************************
* Name: CIccTagLut16::Validate
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
icValidateStatus CIccTagLut16::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile/*=NULL*/)
{
  icValidateStatus rv = CIccMBB::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  if (!pProfile) {
    rv = icMaxStatus(rv, icValidateWarning);
    return rv;
  }

  switch(sig) {
  case icSigAToB0Tag:
  case icSigAToB1Tag:
  case icSigAToB2Tag:
  case icSigBToA0Tag:
  case icSigBToA1Tag:
  case icSigBToA2Tag:
  case icSigGamutTag:
    {
      icUInt32Number nInput, nOutput;
      if (sig==icSigAToB0Tag || sig==icSigAToB1Tag || sig==icSigAToB2Tag || sig==icSigGamutTag) {
        nInput = icGetSpaceSamples(pProfile->m_Header.pcs);
        nOutput = icGetSpaceSamples(pProfile->m_Header.colorSpace);
      }
      else {
        nInput = icGetSpaceSamples(pProfile->m_Header.colorSpace);
        nOutput = icGetSpaceSamples(pProfile->m_Header.pcs);
      }

      if (sig==icSigGamutTag) {
        nOutput = 1;
      }

      icUInt8Number i;
      if (m_CurvesB) {
        for (i=0; i<nInput; i++) {
          if (m_CurvesB[i]) {
            rv = icMaxStatus(rv, m_CurvesB[i]->Validate(sig, sReport, pProfile));
            if (m_CurvesB[i]->GetType()==icSigCurveType) {
              CIccTagCurve *pTagCurve = (CIccTagCurve*)m_CurvesB[i];
              if (pTagCurve->GetSize()==1) {
                sReport += icValidateCriticalErrorMsg;
                sReport += sSigName;
                sReport += " - lut16Tags do not support single entry gamma curves.\n";
                rv = icMaxStatus(rv, icValidateCriticalError);
              }
            }
          }
          else {
            sReport += icValidateCriticalErrorMsg;
            sReport += sSigName;
            sReport += " - Incorrect number of B-curves.\n";
            rv = icMaxStatus(rv, icValidateCriticalError);
          }
        }
      }

      if (m_Matrix) {
        rv = icMaxStatus(rv, m_Matrix->Validate(GetType(), sReport, pProfile));
      }
      else {
        int sum=0;
        for (int i=0; i<9; i++) {
          sum += m_XYZMatrix[i];
        }
        if (m_XYZMatrix[0]!=1.0 || m_XYZMatrix[4]!=1.0 || m_XYZMatrix[9]!=1.0 || sum!=3.0) {
          sReport += icValidateWarningMsg;
          sReport += sSigName;
          sReport += " - Matrix must be identity.\n";
          rv = icMaxStatus(rv, icValidateWarning);
        }
      }

      if (m_CurvesA) {

        for (i=0; i<nOutput; i++) {
          if (m_CurvesA[i]) {
            rv = icMaxStatus(rv, m_CurvesA[i]->Validate(sig, sReport, pProfile));
            if (m_CurvesA[i]->GetType()==icSigCurveType) {
              CIccTagCurve *pTagCurve = (CIccTagCurve*)m_CurvesA[i];
              if (pTagCurve->GetSize()==1) {
                sReport += icValidateCriticalErrorMsg;
                sReport += sSigName;
                sReport += " - lut16Tags do not support single entry gamma curves.\n";
                rv = icMaxStatus(rv, icValidateCriticalError);
              }
            }
          }
          else {
            sReport += icValidateCriticalErrorMsg;
            sReport += sSigName;
            sReport += " - Incorrect number of A-curves.\n";
            rv = icMaxStatus(rv, icValidateCriticalError);
          }
        }

      }

      break;
    }
  default:
    {
    }
  }


  return rv;
}


#ifdef USESAMPLEICCNAMESPACE
} //namespace sampleICC
#endif
