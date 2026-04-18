/*
    File:       IccUtil.cpp

    Contains:   Implementation of utility classes/functions

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

#include "IccIO.h"
#include "IccUtil.h"
#include "IccTagFactory.h"
#include <stdlib.h>
#include <memory>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define PI 3.1415926535897932384626433832795


#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

const char *icValidateWarningMsg = "Warning! - ";
const char *icValidateNonCompliantMsg = "NonCompliant! - ";
const char *icValidateCriticalErrorMsg = "Error! - ";

/**
 ******************************************************************************
 * Name: icMaxStatus
 * 
 * Purpose: return worst status 
 * 
 * Args:
 *  s1, s2
 * 
 * Return: 
 ******************************************************************************
 */
icValidateStatus icMaxStatus(icValidateStatus s1, icValidateStatus s2)
{
  if (s1>s2)
    return s1;
  return s2;
}

bool icIsSpaceCLR(icColorSpaceSignature sig) 
{
  switch(sig) {
  case icSig2colorData:
  case icSig3colorData:
  case icSig4colorData: 
  case icSig5colorData:
  case icSig6colorData:
  case icSig7colorData:
  case icSig8colorData:
  case icSig9colorData:
  case icSig10colorData:
  case icSig11colorData:
  case icSig12colorData:
  case icSig13colorData:
  case icSig14colorData:
  case icSig15colorData:
    return true;

  default:
    return false;
  }

  return false;
}

void icColorIndexName(icChar *szName, icColorSpaceSignature csSig,
                      int nIndex, int nColors, const icChar *szUnknown)
{
  icChar szSig[5];
  int i;

  if (csSig!=icSigUnknownData) {
    szSig[0] = (icChar)(csSig>>24);
    szSig[1] = (icChar)(csSig>>16);
    szSig[2] = (icChar)(csSig>>8);
    szSig[3] = (icChar)(csSig);
    szSig[4] = '\0';

    for (i=3; i>0; i--) {
      if (szSig[i]==' ')
        szSig[i]='\0';
    }
    if (nColors==1) {
      strcpy(szName, szSig);
    }
    else if (nColors == strlen(szSig)) {
      sprintf(szName, "%s_%c", szSig, szSig[nIndex]);
    }
    else {
      sprintf(szName, "%s_%d", szSig, nIndex+1);
    }
  }
  else if (nColors==1) {
    strcpy(szName, szUnknown);
  }
  else {
    sprintf(szName, "%s_%d", szUnknown, nIndex+1);
  }
}

void icColorValue(icChar *szValue, icFloatNumber nValue,
                  icColorSpaceSignature csSig, int nIndex)
{
  if (csSig==icSigLabData) {
    if (!nIndex || nIndex>2)
      sprintf(szValue, "%7.3lf", nValue * 100.0);
    else
      sprintf(szValue, "%8.3lf", nValue * 255.0 - 128.0);
  }
  else if (csSig==icSigUnknownData) {
    sprintf(szValue, "%8.5lf", nValue);
  }
  else {
    sprintf(szValue, "%7.3lf", nValue * 100.0);
  }
}

/**
**************************************************************************
* Name: icMatrixInvert3x3
* 
* Purpose: 
*  Inversion of a 3x3 matrix using the Adjoint Cofactor and the determinant of
*  the 3x3 matrix.
*
*  Note: Matrix index positions:
*     0 1 2
*     3 4 5
*     6 7 8
* 
* Args: 
*  M = matrix to invert.
* 
* Return: 
*  true = matrix is invertible and stored back into M, false = matrix is not
*  invertible.
**************************************************************************
*/
bool icMatrixInvert3x3(icFloatNumber *M)
{
  icFloatNumber m48 = M[4]*M[8];
  icFloatNumber m75 = M[7]*M[5];
  icFloatNumber m38 = M[3]*M[8];
  icFloatNumber m65 = M[6]*M[5];
  icFloatNumber m37 = M[3]*M[7];
  icFloatNumber m64 = M[6]*M[4];

  icFloatNumber det = M[0]*(m48 - m75) - 
    M[1]*(m38 - m65) + 
    M[2]*(m37 - m64);

  if (!det)
    return false;

  icFloatNumber Co[9];

  Co[0] = +(m48 - m75);
  Co[1] = -(m38 - m65);
  Co[2] = +(m37 - m64);

  Co[3] = -(M[1]*M[8] - M[7]*M[2]);
  Co[4] = +(M[0]*M[8] - M[6]*M[2]);
  Co[5] = -(M[0]*M[7] - M[6]*M[1]);

  Co[6] = +(M[1]*M[5] - M[4]*M[2]);
  Co[7] = -(M[0]*M[5] - M[3]*M[2]);
  Co[8] = +(M[0]*M[4] - M[3]*M[1]);

  M[0] = Co[0] / det;
  M[1] = Co[3] / det;
  M[2] = Co[6] / det;

  M[3] = Co[1] / det;
  M[4] = Co[4] / det;
  M[5] = Co[7] / det;

  M[6] = Co[2] / det;
  M[7] = Co[5] / det;
  M[8] = Co[8] / det;

  return true;
}

/**
**************************************************************************
* Name: icMatrixMultiply3x3
* 
* Purpose: 
*  Multiply two 3x3 matricies resulting in a 3x3 matrix.
*
*  Note: Matrix index positions:
*     0 1 2
*     3 4 5
*     6 7 8
* 
* Args: 
*  result = matrix to recieve result.
*  l = left matrix to multiply (matrix multiplication is order dependent)
*  r = right matrix to multiply (matrix multiplicaiton is order dependent)
*
**************************************************************************
*/
void icMatrixMultiply3x3(icFloatNumber* result,
                         const icFloatNumber* l,
                         const icFloatNumber* r)
{
  const unsigned int e11 = 0;
  const unsigned int e12 = 1;
  const unsigned int e13 = 2;
  const unsigned int e21 = 3;
  const unsigned int e22 = 4;
  const unsigned int e23 = 5;
  const unsigned int e31 = 6;
  const unsigned int e32 = 7;
  const unsigned int e33 = 8;
  result[e11] = l[e11] * r[e11] + l[e12] * r[e21] + l[e13] * r[e31];
  result[e12] = l[e11] * r[e12] + l[e12] * r[e22] + l[e13] * r[e32];
  result[e13] = l[e11] * r[e13] + l[e12] * r[e23] + l[e13] * r[e33];
  result[e21] = l[e21] * r[e11] + l[e22] * r[e21] + l[e23] * r[e31];
  result[e22] = l[e21] * r[e12] + l[e22] * r[e22] + l[e23] * r[e32];
  result[e23] = l[e21] * r[e13] + l[e22] * r[e23] + l[e23] * r[e33];
  result[e31] = l[e31] * r[e11] + l[e32] * r[e21] + l[e33] * r[e31];
  result[e32] = l[e31] * r[e12] + l[e32] * r[e22] + l[e33] * r[e32];
  result[e33] = l[e31] * r[e13] + l[e32] * r[e23] + l[e33] * r[e33];
}

/**
**************************************************************************
* Name: icVectorApplyMatrix3x3
* 
* Purpose: 
*  Applies a 3x3 matrix to a 3 element column vector. 
*
*  Note: Matrix index positions:
*     0 1 2
*     3 4 5
*     6 7 8
* 
*  Note: result = m x v
*
* Args: 
*  result = vector to receive result.
*  m = matrix to multiply
*  v = vector to apply matrix to
*
**************************************************************************
*/
void icVectorApplyMatrix3x3(icFloatNumber* result,
                            const icFloatNumber* m,
                            const icFloatNumber* v)
{
  const unsigned int e11 = 0;
  const unsigned int e12 = 1;
  const unsigned int e13 = 2;
  const unsigned int e21 = 3;
  const unsigned int e22 = 4;
  const unsigned int e23 = 5;
  const unsigned int e31 = 6;
  const unsigned int e32 = 7;
  const unsigned int e33 = 8;
  result[0] = m[e11] * v[0] + m[e12] * v[1] + m[e13] * v[2];
  result[1] = m[e21] * v[0] + m[e22] * v[1] + m[e23] * v[2];
  result[2] = m[e31] * v[0] + m[e32] * v[1] + m[e33] * v[2];
}


icS15Fixed16Number icDtoF(icFloatNumber num)
{
  icS15Fixed16Number rv;

  if (num<-32768.0)
    num = -32768.0;
  else if (num>32767.0)
    num = 32767.0;

  rv = (icS15Fixed16Number)(num*65536 + 0.5);

  return rv;
}

icFloatNumber icFtoD(icS15Fixed16Number num)
{
  icFloatNumber rv = (icFloatNumber)((icFloatNumber)num / 65536.0);

  return rv;
}

icU16Fixed16Number icDtoUF(icFloatNumber num)
{
  icU16Fixed16Number rv;

  if (num<0)
    num = 0;
  else if (num>65535.0)
    num = 65535.0;

  rv = (icU16Fixed16Number)(num*65536 + 0.5);

  return rv;
}

icFloatNumber icUFtoD(icU16Fixed16Number num)
{
  icFloatNumber rv = (icFloatNumber)((icFloatNumber)num / 65536.0);

  return rv;
}

icU1Fixed15Number icDtoUSF(icFloatNumber num)
{
  icU1Fixed15Number rv;

  if (num<0)
    num = 0;
  else if (num>65535.0/32768.0)
    num = 65535.0/32768.0;

  rv = (icU1Fixed15Number)(num*32768.0 + 0.5);

  return rv;
}

icFloatNumber icUSFtoD(icU1Fixed15Number num)
{
  icFloatNumber rv = (icFloatNumber)((icFloatNumber)num / 32768.0);

  return rv;
}

icU8Fixed8Number icDtoUCF(icFloatNumber num)
{
  icU8Fixed8Number rv;

  if (num<0)
    num = 0;
  else if (num>255.0)
    num = 255.0;

  rv = (icU8Fixed8Number)(num*256.0 + 0.5);

  return rv;
}

icFloatNumber icUCFtoD(icU8Fixed8Number num)
{
  icFloatNumber rv = (icFloatNumber)((icFloatNumber)num / 256.0);

  return rv;
}

icUInt8Number icFtoU8(icFloatNumber num)
{
  icUInt8Number rv;

  if (num<0)
    num = 0;
  else if (num>1.0)
    num = 1.0;

  rv = (icUInt8Number)(num*255.0 + 0.5);

  return rv;
}

icFloatNumber icU8toF(icUInt8Number num)
{
  icFloatNumber rv = (icFloatNumber)((icFloatNumber)num / 255.0);

  return rv;
}

icUInt16Number icFtoU16(icFloatNumber num)
{
  icUInt16Number rv;

  if (num<0)
    num = 0;
  else if (num>1.0)
    num = 1.0;

  rv = (icUInt16Number)(num*65535.0 + 0.5);

  return rv;
}

icFloatNumber icU16toF(icUInt16Number num)
{
  icFloatNumber rv = (icFloatNumber)((icFloatNumber)num / 65535.0);

  return rv;
}


icFloatNumber icD50XYZ[3] = { 0.9642f, 1.0000f, 0.8249f };
icFloatNumber icD50XYZxx[3] = { 96.42f, 100.00f, 82.49f };

void icNormXyz(icFloatNumber *XYZ, icFloatNumber *WhiteXYZ)
{
  if (!WhiteXYZ)
    WhiteXYZ = icD50XYZ;

  XYZ[0] = XYZ[0] / WhiteXYZ[0];
  XYZ[1] = XYZ[1] / WhiteXYZ[1];
  XYZ[2] = XYZ[2] / WhiteXYZ[2];
}

void icDeNormXyz(icFloatNumber *XYZ, icFloatNumber *WhiteXYZ)
{
  if (!WhiteXYZ)
    WhiteXYZ = icD50XYZ;

  XYZ[0] = XYZ[0] * WhiteXYZ[0];
  XYZ[1] = XYZ[1] * WhiteXYZ[1];
  XYZ[2] = XYZ[2] * WhiteXYZ[2];
}

static icFloatNumber cubeth(icFloatNumber v)
{
  if (v> 0.008856) {
    return (icFloatNumber)pow((double)v, 1.0/3.0);
  }
  else {
    return (icFloatNumber)(7.787037037037037037037037037037*v + 16.0/116.0);
  }
}

static icFloatNumber icubeth(icFloatNumber v)
{
  if (v > 0.20689303448275862068965517241379)
    return v*v*v;
  else 
#ifndef SAMPLEICC_NOCLIPLABTOXYZ
  if (v>16.0/116.0)
#endif
    return (icFloatNumber)((v - 16.0 / 116.0) / 7.787037037037037037037037037037);
#ifndef SAMPLEICC_NOCLIPLABTOXYZ
  else
    return 0.0;
#endif
}

void icLabtoXYZ(icFloatNumber *XYZ, icFloatNumber *Lab /*=NULL*/, icFloatNumber *WhiteXYZ /*=NULL*/)
{
  if (!Lab)
    Lab = XYZ;

  if (!WhiteXYZ)
    WhiteXYZ = icD50XYZ;

  icFloatNumber fy = (icFloatNumber)((Lab[0] + 16.0) / 116.0);

  XYZ[0] = icubeth((icFloatNumber)(Lab[1]/500.0 + fy)) * WhiteXYZ[0];
  XYZ[1] = icubeth(fy) * WhiteXYZ[1];
  XYZ[2] = icubeth((icFloatNumber)(fy - Lab[2]/200.0)) * WhiteXYZ[2];

}

void icXYZtoLab(icFloatNumber *Lab, icFloatNumber *XYZ /*=NULL*/, icFloatNumber *WhiteXYZ /*=NULL*/)
{
  icFloatNumber Xn, Yn, Zn;
  
  if (!XYZ)
    XYZ = Lab;

  if (!WhiteXYZ)
    WhiteXYZ = icD50XYZ;

  Xn = cubeth(XYZ[0] / WhiteXYZ[0]);
  Yn = cubeth(XYZ[1] / WhiteXYZ[1]);
  Zn = cubeth(XYZ[2] / WhiteXYZ[2]);

  Lab[0] = (icFloatNumber)(116.0 * Yn - 16.0);
  Lab[1] = (icFloatNumber)(500.0 * (Xn - Yn));
  Lab[2] = (icFloatNumber)(200.0 * (Yn - Zn));

}

void icLch2Lab(icFloatNumber *Lab, icFloatNumber *Lch /*=NULL*/)
{
  if (!Lch) {
    Lch = Lab;
  }
  else
    Lab[0] = Lch[0];

  icFloatNumber a = (icFloatNumber)(Lch[1] * cos(Lch[2] * PI / 180.0));
  icFloatNumber b = (icFloatNumber)(Lch[1] * sin(Lch[2] * PI / 180.0));

  Lab[1] = a;
  Lab[2] = b;
}

void icLab2Lch(icFloatNumber *Lch, icFloatNumber *Lab /*=NULL*/)
{
  if (!Lab) {
    Lab = Lch;
  }
  else
    Lch[0] = Lab[0];

  icFloatNumber c = sqrt(Lab[1]*Lab[1] + Lab[2]*Lab[2]);
  icFloatNumber h = (icFloatNumber)(atan2(Lab[2], Lab[1]) * 180.0 / PI);
  while (h<0.0)
    h+=360.0;

  Lch[1] = c;
  Lch[2] = h;
}

icFloatNumber icMin(icFloatNumber v1, icFloatNumber v2)
{
  return( v1 < v2 ? v1 : v2 );
}

icFloatNumber icMax(icFloatNumber v1, icFloatNumber v2)
{
  return( v1 > v2 ? v1 : v2 );
}

icUInt32Number icIntMin(icUInt32Number v1, icUInt32Number v2)
{
  return( v1 < v2 ? v1 : v2 );
}

icUInt32Number icIntMax(icUInt32Number v1, icUInt32Number v2)
{
  return( v1 > v2 ? v1 : v2 );
}


void icLabFromPcs(icFloatNumber *Lab)
{
  Lab[0] *= 100.0;
  Lab[1] = (icFloatNumber)(Lab[1]*255.0 - 128.0);
  Lab[2] = (icFloatNumber)(Lab[2]*255.0 - 128.0);
}


void icLabToPcs(icFloatNumber *Lab)
{
  Lab[0] /= 100.0;
  Lab[1] = (icFloatNumber)((Lab[1] + 128.0) / 255.0);
  Lab[2] = (icFloatNumber)((Lab[2] + 128.0) / 255.0);
}

void icXyzFromPcs(icFloatNumber *XYZ)
{
  XYZ[0] = (icFloatNumber)(XYZ[0] * 65535.0 / 32768.0);
  XYZ[1] = (icFloatNumber)(XYZ[1] * 65535.0 / 32768.0);
  XYZ[2] = (icFloatNumber)(XYZ[2] * 65535.0 / 32768.0);
}

void icXyzToPcs(icFloatNumber *XYZ)
{
  XYZ[0] = (icFloatNumber)(XYZ[0] * 32768.0 / 65535.0);
  XYZ[1] = (icFloatNumber)(XYZ[1] * 32768.0 / 65535.0);
  XYZ[2] = (icFloatNumber)(XYZ[2] * 32768.0 / 65535.0);
}


#define DUMPBYTESPERLINE 16

void icMemDump(std::string &sDump, void *pBuf, icUInt32Number nNum)
{
  icUInt8Number *pData = (icUInt8Number *)pBuf;
  icChar buf[80], num[10];

  icInt32Number i, j;
  icUInt8Number c;

  icInt32Number lines = (nNum + DUMPBYTESPERLINE - 1)/DUMPBYTESPERLINE;
  sDump.reserve(sDump.size() + lines*79);

  for (i=0; i<(icInt32Number)nNum; i++, pData++) {
    j=i%DUMPBYTESPERLINE;
    if (!j) {
      if (i) {
        sDump += (const icChar*)buf;
      }
      memset(buf, ' ', 76);
      buf[76] = '\r';
      buf[77] = '\n';
      buf[78] = '\0';
      sprintf(num, "%08X:", i);
      strncpy(buf, num, 9);
    }

    sprintf(num, "%02X", *pData);
    strncpy(buf+10+j*3, num, 2);

    c=*pData;
    if (!isprint(c))
      c='.';
    buf[10+16*3 + 1 + j] = c;
  }
  sDump += buf;
}

void icMatrixDump(std::string &sDump, icS15Fixed16Number *pMatrix)
{
  icChar buf[128];

  sprintf(buf, "%8.4lf %8.4lf %8.4lf\n", icFtoD(pMatrix[0]), icFtoD(pMatrix[1]), icFtoD(pMatrix[2]));
  sDump += buf;
  sprintf(buf, "%8.4lf %8.4lf %8.4lf\n", icFtoD(pMatrix[3]), icFtoD(pMatrix[4]), icFtoD(pMatrix[5]));
  sDump += buf;
  sprintf(buf, "%8.4lf %8.4lf %8.4lf\n", icFtoD(pMatrix[6]), icFtoD(pMatrix[7]), icFtoD(pMatrix[8]));
  sDump += buf;
}

const icChar *icGetSig(icChar *pBuf, icUInt32Number nSig, bool bGetHexVal)
{
  int i;
  icUInt32Number sig=nSig;
  icUInt8Number c;

  if (!nSig) {
    strcpy(pBuf, "NULL");
    return pBuf;
  }

  pBuf[0] = '\'';
  for (i=1; i<5; i++) {
    c=(icUInt8Number)(sig>>24);
    if (!isprint(c))
      c='?';
    pBuf[i]=c;
    sig <<=8;
  }

  if (bGetHexVal)
  sprintf(pBuf+5, "' = %08X", nSig);
  else
    sprintf(pBuf+5, "'");

  return pBuf;
}

const icChar *icGetSigStr(icChar *pBuf, icUInt32Number nSig)
{
  int i, j=-1;
  icUInt32Number sig=nSig;
  icUInt8Number c;
  bool bGetHexVal = false;

  for (i=0; i<4; i++) {
    c=(icUInt8Number)(sig>>24);
    if (!c) {
      j=i;
    }
    else if (j!=-1) {
      bGetHexVal = true;
    }
    else if (!isprint(c)) {
      c='?';
      bGetHexVal = true;
    }
    pBuf[i]=c;
    sig <<=8;
  }

  if (bGetHexVal)
    sprintf(pBuf, "%08Xh", nSig);
  else
    pBuf[4] = '\0';

  return pBuf;
}

icUInt32Number icGetSigVal(const icChar *pBuf)
{
  switch(strlen(pBuf)) {
    case 1:
      return (((unsigned long)pBuf[0])<<24) +
             0x202020;

    case 2:
      return (((unsigned long)pBuf[0])<<24) +
             (((unsigned long)pBuf[1])<<16) +
             0x2020;

    case 3:
      return (((unsigned long)pBuf[0])<<24) +
             (((unsigned long)pBuf[1])<<16) +
             (((unsigned long)pBuf[2])<<8) +
             0x20;

    case 4:
      return (((unsigned long)pBuf[0])<<24) +
             (((unsigned long)pBuf[1])<<16) +
             (((unsigned long)pBuf[2])<<8) +
             (((unsigned long)pBuf[3]));

    default:
      return icSigUnknownData;
  }
}


icUInt32Number icGetSpaceSamples(icColorSpaceSignature sig)
{
  switch(sig) {
  case icSigGrayData:
  case icSigGamutData:
    return 1;

  case icSig2colorData:
    return 2;

  case icSigXYZData:
  case icSigLabData:
  case icSigLuvData:
  case icSigYCbCrData:
  case icSigYxyData:
  case icSigRgbData:
  case icSigHsvData:
  case icSigHlsData:
  case icSigCmyData:
  case icSig3colorData:
  case icSigDevLabData:
  case icSigDevXYZData:
    return 3;

  case icSigCmykData:
  case icSig4colorData:
    return 4;

  case icSig5colorData:
    return 5;

  case icSig6colorData:
    return 6;

  case icSig7colorData:
    return 7;

  case icSig8colorData:
    return 8;

  case icSig9colorData:
    return 9;

  case icSig10colorData:
    return 10;

  case icSig11colorData:
    return 11;

  case icSig12colorData:
    return 12;

  case icSig13colorData:
    return 13;

  case icSig14colorData:
    return 14;

  case icSig15colorData:
    return 15;

  case icSigNamedData:
  default:
    return 0;
  }
}

const icChar *CIccInfo::GetUnknownName(icUInt32Number val)
{
  icChar buf[24];
  if (!val)
    return "Unknown";

  sprintf(m_szStr, "Unknown %s", icGetSig(buf, val)); 

  return m_szStr;
}

const icChar *CIccInfo::GetVersionName(icUInt32Number val)
{
  icFloatNumber ver = (icFloatNumber)(((val>>28)&0xf)*10.0 + ((val>>24)&0xf) +
                                      ((val>>20)&0xf)/10.0 + ((val>>16)&0xf)/100.0);

  sprintf(m_szStr, "%.2lf", ver);

  return m_szStr;
}

const icChar *CIccInfo::GetDeviceAttrName(icUInt64Number val)
{
  if (val & icTransparency)
    strcpy(m_szStr, "Transparency");
  else
    strcpy(m_szStr, "Reflective");

  int l=strlen(m_szStr);

  if (val & icMatte)
    strcpy(m_szStr+l, " | Matte");
  else
    strcpy(m_szStr+l, " | Glossy");

  return m_szStr;
}

const icChar *CIccInfo::GetProfileFlagsName(icUInt32Number val)
{
  if (val & icEmbeddedProfileTrue) 
    strcpy(m_szStr, "EmbeddedProfileTrue");
  else
    strcpy(m_szStr, "EmbeddedProfileFalse");

  int l=strlen(m_szStr);

  if (val & icUseWithEmbeddedDataOnly)
    strcpy(m_szStr+l, " | UseWithEmbeddedDataOnly");
  else
    strcpy(m_szStr+l, " | UseAnywhere");

  return m_szStr;
}

const icChar *CIccInfo::GetTagSigName(icTagSignature sig)
{
  const icChar *rv = CIccTagCreator::GetTagSigName(sig);
  if (rv) {
    return rv;
  }
  return GetUnknownName(sig);
}

const icChar *CIccInfo::GetTechnologySigName(icTechnologySignature sig)
{
  switch(sig) {
  case icSigDigitalCamera:
    return "DigitalCamera";

  case icSigFilmScanner:
    return "FilmScanner";

  case icSigReflectiveScanner:
    return "ReflectiveScanner";

  case icSigInkJetPrinter:
    return "InkJetPrinter";

  case icSigThermalWaxPrinter:
    return "ThermalWaxPrinter";

  case icSigElectrophotographicPrinter:
    return "ElectrophotographicPrinter";

  case icSigElectrostaticPrinter:
    return "ElectrostaticPrinter";

  case icSigDyeSublimationPrinter:
    return "DyeSublimationPrinter";

  case icSigPhotographicPaperPrinter:
    return "PhotographicPaperPrinter";

  case icSigFilmWriter:
    return "FilmWriter";

  case icSigVideoMonitor:
    return "VideoMonitor";

  case icSigVideoCamera:
    return "VideoCamera";

  case icSigProjectionTelevision:
    return "ProjectionTelevision";

  case icSigCRTDisplay:
    return "CRTDisplay";

  case icSigPMDisplay:
    return "PMDisplay";

  case icSigAMDisplay:
    return "AMDisplay";

  case icSigPhotoCD:
    return "PhotoCD";

  case icSigPhotoImageSetter:
    return "PhotoImageSetter";

  case icSigGravure:
    return "Gravure";

  case icSigOffsetLithography:
    return "OffsetLithography";

  case icSigSilkscreen:
    return "Silkscreen";

  case icSigFlexography:
    return "Flexography";

  default:
    return GetUnknownName(sig);
  }
}

const icChar *CIccInfo::GetTagTypeSigName(icTagTypeSignature sig)
{
  const icChar *rv = CIccTagCreator::GetTagTypeSigName(sig);
  if (rv) {
    return rv;
  }

  return GetUnknownName(sig);
}


const icChar *CIccInfo::GetColorSpaceSigName(icColorSpaceSignature sig)
{
  switch (sig) {
  case icSigXYZData:
  case icSigDevXYZData:
    return "XYZData";

  case icSigLabData:
  case icSigDevLabData:
    return "LabData";

  case icSigLuvData:
    return "LuvData";

  case icSigYCbCrData:
    return "YCbCrData";

  case icSigYxyData:
    return "YxyData";

  case icSigRgbData:
    return "RgbData";

  case icSigGrayData:
    return "GrayData";

  case icSigHsvData:
    return "HsvData";

  case icSigHlsData:
    return "HlsData";

  case icSigCmykData:
    return "CmykData";

  case icSigCmyData:
    return "CmyData";


  case icSigMCH1Data:
    return "MCH1Data/1colorData";

  case icSigMCH2Data:
    return "MCH2Data/2colorData";

  case icSigMCH3Data:
    return "MCH3Data/3colorData";

  case icSigMCH4Data:
    return "MCH4Data/4colorData";

  case icSigMCH5Data:
    return "MCH5Data/5colorData";

  case icSigMCH6Data:
    return "MCH6Data/6colorData";

  case icSigMCH7Data:
    return "MCH7Data/7colorData";

  case icSigMCH8Data:
    return "MCH8Data/8colorData";

  case icSigMCH9Data:
    return "MCH9Data/9colorData";

  case icSigMCHAData:
    return "MCHAData/10colorData";

  case icSigMCHBData:
    return "MCHBData/11colorData";

  case icSigMCHCData:
    return "MCHCData/12colorData";

  case icSigMCHDData:
    return "MCHDData/13colorData";

  case icSigMCHEData:
    return "MCHEData/14colorData";

  case icSigMCHFData:
    return "MCHFData/15colorData";

  case icSigGamutData:
    return "GamutData";
    
  case icSigNamedData:
    return "NamedData";

  default:
    return GetUnknownName(sig);
  }
}

const icChar *CIccInfo::GetProfileClassSigName(icProfileClassSignature sig)
{
  switch (sig) {
  case icSigInputClass:
    return "InputClass";

  case icSigDisplayClass:
    return "DisplayClass";

  case icSigOutputClass:
    return "OutputClass";

  case icSigLinkClass:
    return "LinkClass";

  case icSigAbstractClass:
    return "AbstractClass";

  case icSigColorSpaceClass:
    return "ColorSpaceClass";

  case icSigNamedColorClass:
    return "NamedColorClass";

  default:
    return GetUnknownName(sig);
  }
}

const icChar *CIccInfo::GetPlatformSigName(icPlatformSignature sig)
{
  switch (sig) {
  case icSigMacintosh:
    return "Macintosh";

  case icSigMicrosoft:
    return "Microsoft";

  case icSigSolaris:
    return "Solaris";

  case icSigSGI:
    return "SGI";

  case icSigTaligent:
    return "Taligent";

  case icSigUnkownPlatform:
    return "Unknown";

  default:
    return GetUnknownName(sig);
  }
}


//The following signatures come from the signature registry
//Return the Description (minus CMM).
const icChar *CIccInfo::GetCmmSigName(icCmmSignature sig)
{
  switch (sig) {
  case icSigAdobe:
    return "Adobe";

  case icSigApple:
    return "Apple";

  case icSigColorGear:
    return "ColorGear";

  case icSigColorGearLite:
    return "ColorGear Lite";

  case icSigFujiFilm:
    return "Fuji Film";

  case icSigHarlequinRIP:
    return "Harlequin RIP";

  case icSigArgyllCMS:
    return "Argyll CMS";

  case icSigLogoSync:
    return "LogoSync";

  case icSigHeidelberg:
    return "Heidelberg";

  case icSigLittleCMS:
    return "Little CMS";

  case icSigKodak:
    return "Kodak";

  case icSigKonicaMinolta:
    return "Konica Minolta";

  case icSigMutoh:
    return "Mutoh";

  case icSigSampleICC:
    return "SampleIcc";

  case icSigTheImagingFactory:
    return "the imaging factory";

  default:
    return GetUnknownName(sig);
  }
}


const icChar *CIccInfo::GetReferenceMediumGamutSigNameName(icReferenceMediumGamutSignature sig)
{
  switch (sig) {
  case icSigPerceptualReferenceMediumGamut:
    return "perceptualReferenceMediumGamut";

  default:
    return GetUnknownName(sig);
  }
}


const icChar *CIccInfo::GetColorimetricIntentImageStateName(icColorimetricIntentImageStateSignature sig)
{
  switch (sig) {
  case icSigSceneColorimetryEstimates:
    return "Scene Colorimetry Estimates";

  case icSigSceneAppearanceEstimates:
    return "Scene Appearance Estimates";

  case icSigFocalPlaneColorimetryEstimates:
    return "Focal Plane Colorimetry Estimates";

  case icSigReflectionHardcopyOriginalColorimetry:
    return "Reflection Hardcopy Original Colorimetry";

  case icSigReflectionPrintOutputColorimetry:
    return "Reflection Print Output Colorimetry";

  default:
    return GetUnknownName(sig);
  }
}


const icChar *CIccInfo::GetSigName(icUInt32Number sig)
{
  const icChar *rv;

  rv = GetTagSigName((icTagSignature)sig);
  if (rv != m_szStr)
    return rv;

  rv = GetTechnologySigName((icTechnologySignature)sig);
  if (rv != m_szStr)
    return rv;

  rv = GetTagTypeSigName((icTagTypeSignature)sig);
  if (rv != m_szStr)
    return rv;

  rv = GetColorSpaceSigName((icColorSpaceSignature)sig);
  if (rv != m_szStr)
    return rv;

  rv = GetProfileClassSigName((icProfileClassSignature)sig);
  if (rv != m_szStr)
    return rv;

  rv = GetPlatformSigName((icPlatformSignature)sig);
  if (rv != m_szStr)
    return rv;

  rv = GetReferenceMediumGamutSigNameName((icReferenceMediumGamutSignature)sig);
  if (rv != m_szStr)
    return rv;

  return GetColorimetricIntentImageStateName((icColorimetricIntentImageStateSignature)sig);
}


const icChar *CIccInfo::GetMeasurementFlareName(icMeasurementFlare val)
{
  switch (val) {
  case icFlare0:
    return "Flare 0";

  case icFlare100:
    return "Flare 100";

  case icMaxEnumFlare:
    return "Max Flare";

  default:
    sprintf(m_szStr, "Unknown Flare '%d'", (int)val);
    return m_szStr;
  }
}

const icChar *CIccInfo::GetMeasurementGeometryName(icMeasurementGeometry val)
{
  switch (val) {
  case icGeometryUnknown:
    return "Geometry Unknown";

  case icGeometry045or450:
    return "Geometry 0-45 or 45-0";

  case icGeometry0dord0:
    return "Geometry 0-d or d-0";

  case icMaxEnumGeometry:
    return "Max Geometry";

  default:
    sprintf(m_szStr, "Unknown Geometry '%d'", (int)val);
    return m_szStr;
  }
}

const icChar *CIccInfo::GetRenderingIntentName(icRenderingIntent val)
{
  switch (val) {
  case icPerceptual:
    return "Perceptual";

  case icRelativeColorimetric:
    return "Relative Colorimetric";

  case icSaturation:
    return "Saturation";

  case icAbsoluteColorimetric:
    return "Absolute Colorimetric";

  default:
    sprintf(m_szStr, "Unknown Intent '%d", val);
    return m_szStr;
  }
}

const icChar *CIccInfo::GetSpotShapeName(icSpotShape val)
{
  switch (val) {
  case icSpotShapeUnknown:
    return "Spot Shape Unknown";

  case icSpotShapePrinterDefault:
    return "Spot Shape Printer Default";

  case icSpotShapeRound:
    return "Spot Shape Round";

  case icSpotShapeDiamond:
    return "Spot Shape Diamond";

  case icSpotShapeEllipse:
    return "Spot Shape Ellipse";

  case icSpotShapeLine:
    return "Spot Shape Line";

  case icSpotShapeSquare:
    return "Spot Shape Square";

  case icSpotShapeCross:
    return "Spot Shape Cross";

  default:
    sprintf(m_szStr, "Unknown Spot Shape '%d", val);
    return m_szStr;
  }
}

const icChar *CIccInfo::GetStandardObserverName(icStandardObserver val)
{
  switch (val) {
  case icStdObsUnknown:
    return "Unknown observer";

  case icStdObs1931TwoDegrees:
    return "CIE 1931 (two degree) standard observer";

  case icStdObs1964TenDegrees:
    return "CIE 964 (ten degree) standard observer";

  default:
    sprintf(m_szStr, "Unknown Observer '%d", val);
    return m_szStr;
  }
}

const icChar *CIccInfo::GetIlluminantName(icIlluminant val)
{
  switch (val) {
  case icIlluminantUnknown:
    return "Illuminant Unknown";

  case icIlluminantD50:
    return "Illuminant D50";

  case icIlluminantD65:
    return "Illuminant D65";

  case icIlluminantD93:
    return "Illuminant D93";

  case icIlluminantF2:
    return "Illuminant F2";

  case icIlluminantD55:
    return "Illuminant D55";

  case icIlluminantA:
    return "Illuminant A";

  case icIlluminantEquiPowerE:
    return "Illuminant EquiPowerE";

  case icIlluminantF8:
    return "Illuminant F8";

  default:
    sprintf(m_szStr, "Unknown Illuminant '%d", val);
    return m_szStr;
  }
}

const icChar *CIccInfo::GetMeasurementUnit(icSignature sig)
{
  switch (sig) {
    case icSigStatusA:
      return "Status A";

    case icSigStatusE:
      return "Status E";

    case icSigStatusI:
      return "Status I";

    case icSigStatusT:
      return "Status T";

    case icSigStatusM:
      return "Status M";

    case icSigDN:
      return "DIN with no polarizing filter";

    case icSigDNP:
      return "DIN with polarizing filter";

    case icSigDNN:
      return "Narrow band DIN with no polarizing filter";

    case icSigDNNP:
      return "Narrow band DIN with polarizing filter";

    default:
    {
      char buf[10];
      buf[0] = (char)(sig>>24);
      buf[1] = (char)(sig>>16);
      buf[2] = (char)(sig>>8);
      buf[3] = (char)(sig);
      buf[4] = '\0';

      sprintf(m_szStr, "Unknown Measurement Type '%s'", buf);
      return m_szStr;
    }
  }
}


const icChar *CIccInfo::GetProfileID(icProfileID *profileID)
{
  icChar buf[16];
  icChar temp[16];

  for (int i=0; i<16; i++) {
    sprintf(temp, "%02x", profileID->ID8[i]);
    buf[i] = temp[0];
  }

  sprintf(m_szStr,"%.16s",buf);
  return m_szStr;
}

bool CIccInfo::IsProfileIDCalculated(icProfileID *profileID)
{
  icChar temp[16];
  bool bFlag = false;

  for (int i=0; i<16; i++) {
    sprintf(temp, "%02x", profileID->ID8[i]);
    if (temp[0] != '0') bFlag = true;
  }

  return bFlag;
}

const icChar *CIccInfo::GetColorantEncoding(icColorantEncoding colorant)
{
  switch(colorant) {
    case icColorantITU:
      return "ITU-R BT.709";

    case icColorantSMPTE:
      return "SMPTE RP145-1994";

    case icColorantEBU:
      return "EBU Tech.3213-E";

    case icColorantP22:
      return "P22";

    default:
      return "Customized Encoding";
  }
}

icValidateStatus CIccInfo::CheckData(std::string &sReport, const icXYZNumber &XYZ)
{
  icValidateStatus rv = icValidateOK;

  if (XYZ.X < 0) {
    sReport += icValidateNonCompliantMsg;
    sReport += " - XYZNumber: Negative X value!\n";
    rv = icValidateNonCompliant;
  }

  if (XYZ.Y < 0) {
    sReport += icValidateNonCompliantMsg;
    sReport += " - XYZNumber: Negative Y value!\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  if (XYZ.Z < 0) {
    sReport += icValidateNonCompliantMsg;
    sReport += " - XYZNumber: Negative Z value!\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  return rv;
}

icValidateStatus CIccInfo::CheckData(std::string &sReport, const icDateTimeNumber &dateTime)
{
  icValidateStatus rv = icValidateOK;

  struct tm *newtime;
  time_t long_time;

  time( &long_time );                /* Get time as long integer. */
  newtime = localtime( &long_time );

  icChar buf[128];
  if (dateTime.year<1992) {
    sReport += icValidateWarningMsg;
    sprintf(buf," - %u: Invalid year!\n",dateTime.year);
    sReport += buf;
    rv = icValidateWarning;
  }

  int year = newtime->tm_year+1900;
  if (newtime->tm_mon==11 && newtime->tm_mday==31) {
    if (dateTime.year>(year+1)) {
      sReport += icValidateWarningMsg;
      sprintf(buf," - %u: Invalid year!\n",dateTime.year);
      sReport += buf;
      rv = icMaxStatus(rv, icValidateWarning);
    }
  }
  else {
    if (dateTime.year>year) {
      sReport += icValidateWarningMsg;
      sprintf(buf," - %u: Invalid year!\n",dateTime.year);
      sReport += buf;
      rv = icMaxStatus(rv, icValidateWarning);
    }
  }

  if (dateTime.month<1 || dateTime.month>12) {
    sReport += icValidateWarningMsg;
    sprintf(buf," - %u: Invalid month!\n",dateTime.month);
    sReport += buf;
    rv = icMaxStatus(rv, icValidateWarning);
  }

  if (dateTime.day<1 || dateTime.day>31) {
    sReport += icValidateWarningMsg;
    sprintf(buf," - %u: Invalid day!\n",dateTime.day);
    sReport += buf;
    rv = icMaxStatus(rv, icValidateWarning);
  }

  if (dateTime.month==2) {
    if (dateTime.day>29) {
      sReport += icValidateWarningMsg;
      sprintf(buf," - %u: Invalid day for February!\n",dateTime.day);
      sReport += buf;
      rv = icMaxStatus(rv, icValidateWarning);
    }

    if (dateTime.day==29) {
      if ((dateTime.year%4)!=0) {
        sReport += icValidateWarningMsg;
        sprintf(buf," - %u: Invalid day for February, year is not a leap year(%u)!\n",dateTime.day, dateTime.year);
        sReport += buf;
        rv = icMaxStatus(rv, icValidateWarning);
      }
    }
  }

  if (dateTime.hours>23) {
    sReport += icValidateWarningMsg;
    sprintf(buf," - %u: Invalid hour!\n",dateTime.hours);
    sReport += buf;
    rv = icMaxStatus(rv, icValidateWarning);
  }

  if (dateTime.minutes>59) {
    sReport += icValidateWarningMsg;
    sprintf(buf," - %u: Invalid minutes!\n",dateTime.minutes);
    sReport += buf;
    rv = icMaxStatus(rv, icValidateWarning);
  }

  if (dateTime.seconds>59) {
    sReport += icValidateWarningMsg;
    sprintf(buf," - %u: Invalid seconds!\n",dateTime.hours);
    sReport += buf;
    rv = icMaxStatus(rv, icValidateWarning);
  }

  return rv;
}

bool CIccInfo::IsValidSpace(icColorSpaceSignature sig)
{
  bool rv = true;

  switch(sig) {
  case icSigXYZData:
  case icSigLabData:
  case icSigLuvData:
  case icSigYCbCrData:
  case icSigYxyData:
  case icSigRgbData:
  case icSigGrayData:
  case icSigHsvData:
  case icSigHlsData:
  case icSigCmykData:
  case icSigCmyData:
  case icSigMCH1Data:
  case icSigNamedData:
  case icSigGamutData:
  case icSig2colorData:
  case icSig3colorData:
  case icSig4colorData: 
  case icSig5colorData:
  case icSig6colorData:
  case icSig7colorData:
  case icSig8colorData:
  case icSig9colorData:
  case icSig10colorData:
  case icSig11colorData:
  case icSig12colorData:
  case icSig13colorData:
  case icSig14colorData:
  case icSig15colorData:
    break;

  default:
    rv = false;
  }

  return rv;
}

#ifdef USESAMPLEICCNAMESPACE
} //namespace sampleICC
#endif
