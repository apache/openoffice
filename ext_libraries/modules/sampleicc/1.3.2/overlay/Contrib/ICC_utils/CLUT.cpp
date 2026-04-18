/*
  File:       CLUT.cpp

  Contains:   originally part of iccCreateCLUTProfile command-line tool:
  create and write CLUT tag data

  Version:    V1

  Copyright:  © see below
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
 * 4. The names "ICC" and "The International Color Consortium" must
 *    not be used to imply that the ICC organization endorses or
 *    promotes products derived from this software without prior
 *    written permission. For written permission, please see
 *    <http://www.color.org/>.
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
// -Initial implementation by Joseph Goldstone spring 2006
//
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

#include "CLUT.h"
#include "CLUT_stuffer.h"
#include "ICC_tool_exception.h"

void
CLUT::loadInputShaperLUTs(CIccTagCurve** inputShaperLUTs,
                          const std::string& inputShaperFilename) const
{
  ifstream s(inputShaperFilename.c_str());
  if (! s)
  {
    ostringstream os;
    os << "Could not load input shaper LUTs from `" << inputShaperFilename
       << "'";
    throw ICC_tool_exception(os.str());
  }

  string maxChannelValueAsString;
  s >> maxChannelValueAsString;
  int maxChannelValue = atoi(maxChannelValueAsString.c_str());
  vector<double> redVals;
  vector<double> greenVals;
  vector<double> blueVals;
  string line("");
  while (getline(s, line))
  {
    if (line == "")
      continue;
    double redVal;
    double greenVal;
    double blueVal;
    istringstream is(line);
    is >> redVal;
    is >> greenVal;
    is >> blueVal;
    redVals.push_back(redVal);
    greenVals.push_back(greenVal);
    blueVals.push_back(blueVal);
  }
  unsigned int numEntries = (unsigned int)redVals.size();
  // now make the LUT objects of the appropriate length and stuff them.  
  CIccTagCurve*   redCurve = inputShaperLUTs[0];
  CIccTagCurve* greenCurve = inputShaperLUTs[1];
  CIccTagCurve*  blueCurve = inputShaperLUTs[2];
  redCurve->SetSize(numEntries, icInitIdentity);
  greenCurve->SetSize(numEntries, icInitIdentity);
  blueCurve->SetSize(numEntries, icInitIdentity);
  for (unsigned int j = 0, N = numEntries; j < N; ++j)
  {
    (  *redCurve)[j] = (icFloatNumber)(  redVals[j] / maxChannelValue);
    (*greenCurve)[j] = (icFloatNumber)(greenVals[j] / maxChannelValue);
    ( *blueCurve)[j] = (icFloatNumber)( blueVals[j] / maxChannelValue);
  }
}

CIccTagLut16*
CLUT::makeAToBxTag(const unsigned int edgeN,
                   const icFloatNumber* const rawXYZ,
                   const icFloatNumber* const flare,
                   const icFloatNumber* const illuminant,
                   const CAT* const CATToPCS,
                   const icFloatNumber inputShaperGamma,
                   const std::string& inputShaperFilename,
                   const icFloatNumber* const mediaWhite,
                   const bool LABPCS)
{
  unsigned int i;
  CIccTagLut16* lut16 = new CIccTagLut16();
  lut16->Init(3, 3);
  lut16->SetColorSpaces(icSigRgbData, icSigLabData);

  lut16->NewMatrix();

  LPIccCurve* iLUT = lut16->NewCurvesA();
  for (i = 0; i < 3; ++i)
  {
    CIccTagCurve* pCurve = new CIccTagCurve(0);
    pCurve->SetSize(2, icInitIdentity);
    iLUT[i] = pCurve;
  }
  
  m_innerCLUT = lut16->NewCLUT(edgeN);

  CLUT_stuffer stuffer(edgeN, rawXYZ, flare, illuminant[1], CATToPCS,
                       mediaWhite, LABPCS);
  m_innerCLUT->Iterate(&stuffer);

  LPIccCurve* oLUT = lut16->NewCurvesB();
  if (inputShaperGamma == 1.0)
  {
    if (inputShaperFilename == "")
      for (i = 0; i < 3; ++i)
      {
        CIccTagCurve* pCurve = new CIccTagCurve(0);
        pCurve->SetSize(2, icInitIdentity);
        oLUT[i] = pCurve;
      }
    else
    {
      CIccTagCurve* inputShaperLUTs[3];
      for (i = 0; i < 3; ++i)
      {
        inputShaperLUTs[i] = new CIccTagCurve(0);
        inputShaperLUTs[i]->SetSize(2, icInitIdentity);
      }
      loadInputShaperLUTs(inputShaperLUTs, inputShaperFilename);
      for (i = 0; i < 3; ++i)
        oLUT[i] = inputShaperLUTs[i];
    }
  }
  else
  {
    if (inputShaperFilename != "")
      throw ICC_tool_exception("input shaper LUT gamma and input shaper LUT"
                               " filename both specified, but they are mutually exclusive");
    cout << "------- where we should be -------, gamma is " 
         << inputShaperGamma << endl;
    for (i = 0; i < 3; ++i)
    {
      CIccTagCurve* pCurve = new CIccTagCurve(0);
      pCurve->SetSize(4096, icInitIdentity);
      for (unsigned int j = 0; j < 4096; ++j)
        (*pCurve)[j] = pow((*pCurve)[j], inputShaperGamma);
      oLUT[i] = pCurve;
    }
  }
  return lut16;
}

void
CLUT::measuredXYZToAdaptedXYZ(icFloatNumber* const adaptedXYZ,
                              const icFloatNumber* const measuredXYZ,
                              const icFloatNumber* const flare,
                              const icFloatNumber illuminantY,
                              const CAT* CATToPCS)
{
  icFloatNumber flarelessMeasuredXYZ[3];
  icFloatNumber illuminantRelativeXYZ[3];
  for (unsigned int i = 0; i < 3; ++i)
  {
    flarelessMeasuredXYZ[i] = measuredXYZ[i] - flare[i];
    illuminantRelativeXYZ[i] = flarelessMeasuredXYZ[i] / illuminantY;
  }
  CATToPCS->Apply(adaptedXYZ, illuminantRelativeXYZ);
}

void
CLUT::adaptedXYZToMeasuredXYZ(icFloatNumber* const measuredXYZ,
                              const icFloatNumber* const adaptedXYZ,
                              const icFloatNumber* const flare,
                              const icFloatNumber illuminantY,
                              const CAT* invCATToPCS)
{
  icFloatNumber illuminantRelativeXYZ[3];
  invCATToPCS->Apply(illuminantRelativeXYZ, adaptedXYZ);
  icFloatNumber flarelessMeasuredXYZ[3];
  for (unsigned int i = 0; i < 3; ++i)
  {
    flarelessMeasuredXYZ[i] = illuminantRelativeXYZ[i] * illuminantY;
    measuredXYZ[i] = flarelessMeasuredXYZ[i] + flare[i];
  }
}

void
CLUT::Iterate(IIccCLUTExec* pExec)
{
  m_innerCLUT->Iterate(pExec);
}
