/** @file
    File:       IccCmm.cpp

    Contains:   Implementation of the CIccCmm class.

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
#endif

#include <stdlib.h>
#include <string.h>
#include "IccCmm.h"
#include "IccTag.h"
#include "IccIO.h"

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

////
// Useful Macros
////

#define IsSpacePCS(x) ((x)==icSigXYZData || (x)==icSigLabData)
#define IsSpaceCMYK(x) ((x)==icSigCmykData || (x)==icSig4colorData)

#define IsCompatSpace(x, y) ((x)==(y) || (IsSpacePCS(x) && IsSpacePCS(y)) || (IsSpaceCMYK(x) && IsSpaceCMYK(y)))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 **************************************************************************
 * Class CIccPCS Constructor
 * 
 * Purpose:
 *  This is a class constructor.
 * 
 **************************************************************************
 */
CIccPCS::CIccPCS()
{
  m_bIsV2Lab = false;
  m_Space = icSigUnknownData;
}

/**
**************************************************************************
* Name: CIccPCS::Reset
* 
* Purpose:
*  This is called with the initial color space and a bool 
*  argument which is true if the PCS is version 2.
* 
* Args: 
*  Startpsace = Starting Colorspace
*  bUseLegacyPCS = legacy PCS flag
**************************************************************************
*/
void CIccPCS::Reset(icColorSpaceSignature StartSpace, bool bUseLegacyPCS)
{
  m_bIsV2Lab = IsSpacePCS(StartSpace) && bUseLegacyPCS;
  m_Space = StartSpace;
}

/**
 **************************************************************************
 * Name: CIccPCS::Check
 * 
 * Purpose:
 *  This is called before the apply of each profile's xform to adjust the PCS
 *  to the xform's needed PCS.
 * 
 * Args: 
 *   SrcPixel = source pixel data (this may need adjusting),
 *   pXform = the xform that who's Apply function will shortly be called
 * 
 * Return: 
 *  SrcPixel or ptr to adjusted pixel data (we dont want to modify the source data).
 **************************************************************************
 */
const icFloatNumber *CIccPCS::Check(const icFloatNumber *SrcPixel, CIccXform *pXform)
{
  icColorSpaceSignature NextSpace = pXform->GetSrcSpace();
  bool bIsV2 = pXform->UseLegacyPCS();
  bool bIsNextV2Lab = bIsV2 && (NextSpace == icSigLabData);
  const icFloatNumber *rv;

  if (m_bIsV2Lab && !bIsNextV2Lab) {
    Lab2ToLab4(m_Convert, SrcPixel);
    if (NextSpace==icSigXYZData) {
      LabToXyz(m_Convert, m_Convert);
    }
    rv = m_Convert;
  }
  else if (!m_bIsV2Lab && bIsNextV2Lab) {
    if (m_Space==icSigXYZData) {
      XyzToLab(m_Convert, SrcPixel);
      SrcPixel = m_Convert;
    }
    Lab4ToLab2(m_Convert, SrcPixel);
    rv = m_Convert;
  }
  else if (m_Space==NextSpace) {
    rv = SrcPixel;
  }
  else if (m_Space==icSigXYZData && NextSpace==icSigLabData) {
    XyzToLab(m_Convert, SrcPixel);
    rv = m_Convert;
  }
  else if (m_Space==icSigLabData && NextSpace==icSigXYZData) {
    LabToXyz(m_Convert, SrcPixel);
    rv = m_Convert;
  }
  else {
    rv = SrcPixel;
  }

  m_Space = pXform->GetDstSpace();
  m_bIsV2Lab = bIsV2 && (m_Space == icSigLabData);

  return rv;
}

/**
 **************************************************************************
 * Name: CIccPCS::CheckLast
 * 
 * Purpose: 
 *   Called after all xforms are applied to adjust PCS to final space if needed
 *   Note: space will always be V4.
 * 
 * Args: 
 *  Pixel = Pixel data,
 *  DestSpace = destination color space
 **************************************************************************
 */
void CIccPCS::CheckLast(icFloatNumber *Pixel, icColorSpaceSignature DestSpace)
{
  if (m_bIsV2Lab) {
    Lab2ToLab4(Pixel, Pixel);
    if (DestSpace==icSigXYZData) {
      LabToXyz(Pixel, Pixel);
    }
  }
  else if (m_Space==DestSpace) {
    return;
  }
  else if (m_Space==icSigXYZData) {
    XyzToLab(Pixel, Pixel);
  }
  else if (m_Space==icSigLabData) {
    LabToXyz(Pixel, Pixel);
  }
}

/**
 **************************************************************************
 * Name: CIccPCS::UnitClip
 * 
 * Purpose: 
 *  Convert a double to an icUInt16Number with clipping
 **************************************************************************
 */
icFloatNumber CIccPCS::UnitClip(icFloatNumber v)
{
  if (v<0)
    v = 0;
  if (v>1.0)
    v = 1.0;

  return v;
}

/**
 **************************************************************************
 * Name: CIccPCS::NegClip
 * 
 * Purpose: 
 *  Convert a double to an icUInt16Number with clipping of negative numbers
 **************************************************************************
 */
icFloatNumber CIccPCS::NegClip(icFloatNumber v)
{
  if (v<0)
    v=0;
  
  return v;
}

/**
 **************************************************************************
 * Name: CIccPCS::LabToXyz
 * 
 * Purpose: 
 *  Convert Lab to XYZ
 **************************************************************************
 */
void CIccPCS::LabToXyz(icFloatNumber *Dst, const icFloatNumber *Src, bool bNoClip)
{
  icFloatNumber Lab[3];

  memcpy(&Lab,Src,sizeof(Lab));

  icLabFromPcs(Lab);

  icLabtoXYZ(Lab);

  icXyzToPcs(Lab);

  if (!bNoClip) {
    Dst[0] = UnitClip(Lab[0]);
    Dst[1] = UnitClip(Lab[1]);
    Dst[2] = UnitClip(Lab[2]);
  }
  else {
    Dst[0] = Lab[0];
    Dst[1] = Lab[1];
    Dst[2] = Lab[2];
  }
}


/**
 **************************************************************************
 * Name: CIccPCS::XyzToLab
 * 
 * Purpose: 
 *  Convert XYZ to Lab
 **************************************************************************
 */
void CIccPCS::XyzToLab(icFloatNumber *Dst, const icFloatNumber *Src, bool bNoClip)
{
  icFloatNumber XYZ[3];


  if (!bNoClip) {
    XYZ[0] = UnitClip(Src[0]);
    XYZ[1] = UnitClip(Src[1]);
    XYZ[2] = UnitClip(Src[2]);
  }
  else {
    XYZ[0] = Src[0];
    XYZ[1] = Src[1];
    XYZ[2] = Src[2];
  }
 
  icXyzFromPcs(XYZ);

  icXYZtoLab(XYZ);

  icLabToPcs(XYZ);

  Dst[0] = UnitClip(XYZ[0]);
  Dst[1] = UnitClip(XYZ[1]);
  Dst[2] = UnitClip(XYZ[2]);
}


/**
 **************************************************************************
 * Name: CIccPCS::Lab2ToXyz
 * 
 * Purpose:
 *  Convert version 2 Lab to XYZ
 **************************************************************************
 */
void CIccPCS::Lab2ToXyz(icFloatNumber *Dst, const icFloatNumber *Src, bool bNoClip)
{
  Lab2ToLab4(Dst, Src, bNoClip);
  LabToXyz(Dst, Dst, bNoClip);
}


/**
 **************************************************************************
 * Name: CIccPCS::XyzToLab2
 * 
 * Purpose: 
 *  Convert XYZ to version 2 Lab
 **************************************************************************
 */
void CIccPCS::XyzToLab2(icFloatNumber *Dst, const icFloatNumber *Src, bool bNoClip)
{
  XyzToLab(Dst, Src, bNoClip);
  Lab4ToLab2(Dst, Dst);
}


/**
 **************************************************************************
 * Name: CIccPCS::Lab2ToLab4
 * 
 * Purpose: 
 *  Convert version 2 Lab to version 4 Lab
 **************************************************************************
 */
void CIccPCS::Lab2ToLab4(icFloatNumber *Dst, const icFloatNumber *Src, bool bNoClip)
{
  if (bNoClip) {
    Dst[0] = (icFloatNumber)(Src[0] * 65535.0 / 65280.0);
    Dst[1] = (icFloatNumber)(Src[1] * 65535.0 / 65280.0);
    Dst[2] = (icFloatNumber)(Src[2] * 65535.0 / 65280.0);
  }
  else {
    Dst[0] = UnitClip((icFloatNumber)(Src[0] * 65535.0 / 65280.0));
    Dst[1] = UnitClip((icFloatNumber)(Src[1] * 65535.0 / 65280.0));
    Dst[2] = UnitClip((icFloatNumber)(Src[2] * 65535.0 / 65280.0));
  }
}

/**
 **************************************************************************
 * Name: CIccPCS::Lab4ToLab2
 * 
 * Purpose: 
 *  Convert version 4 Lab to version 2 Lab
 **************************************************************************
 */
void CIccPCS::Lab4ToLab2(icFloatNumber *Dst, const icFloatNumber *Src)
{
  Dst[0] = (icFloatNumber)(Src[0] * 65280.0 / 65535.0);
  Dst[1] = (icFloatNumber)(Src[1] * 65280.0 / 65535.0);
  Dst[2] = (icFloatNumber)(Src[2] * 65280.0 / 65535.0);
}



/**
 **************************************************************************
 * Name: CIccXform::CIccXform
 * 
 * Purpose: 
 *  Constructor
 **************************************************************************
 */
CIccXform::CIccXform()
{
  m_pProfile = NULL;
  m_bInput = true;
  m_nIntent = icUnknownIntent;
}

/**
 **************************************************************************
 * Name: CIccXform::~CIccXform
 * 
 * Purpose: 
 *  Destructor
 **************************************************************************
 */
CIccXform::~CIccXform()
{
  if (m_pProfile)
    delete m_pProfile;
}


/**
 **************************************************************************
 * Name: CIccXform::Create
 * 
 * Purpose:
 *  This is a static Creation function that creates derived CIccXform objects and
 *  initializes them.
 * 
 * Args: 
 *  pProfile = pointer to a CIccProfile object that will be owned by the transform.  This object will
 *   be destroyed when the returned CIccXform object is destroyed.  The means that the CIccProfile
 *   object needs to be allocated on the heap.
 *  bInput = flag to indicate whether to use the input or output side of the profile,
 *  nIntent = the rendering intent to apply to the profile,   
 *  nInterp = the interpolation algorithm to use for N-D luts.
 *  nLutType = selection of which transform lut to use
 * 
 * Return: 
 *  A suitable pXform object
 **************************************************************************
 */
CIccXform *CIccXform::Create(CIccProfile *pProfile, bool bInput, icRenderingIntent nIntent, 
                             icXformInterp nInterp, icXformLutType nLutType, bool bUseMpeTags)
{
  CIccXform *rv = NULL;
  icRenderingIntent nTagIntent = nIntent;

  if (nTagIntent == icUnknownIntent)
    nTagIntent = icPerceptual;

  switch (nLutType) {
    case icXformLutColor:
      if (bInput) {
        CIccTag *pTag = NULL;
        if (bUseMpeTags) {
          pTag = pProfile->FindTag(icSigDToB0Tag + nTagIntent);

          if (!pTag && nTagIntent ==icAbsoluteColorimetric) {
            pTag = pProfile->FindTag(icSigDToB1Tag);
            if (pTag)
              nTagIntent = icRelativeColorimetric;
          }

          if (!pTag) {
            pTag = pProfile->FindTag(icSigDToB0Tag);
          }

          //Unsupported elements cause fall back behavior
          if (pTag && !pTag->IsSupported())
            pTag = NULL;
        }

        if (!pTag) {
          if (nTagIntent == icAbsoluteColorimetric)
            nTagIntent = icRelativeColorimetric;
          pTag = pProfile->FindTag(icSigAToB0Tag + nTagIntent);
        }

        if (!pTag) {
          pTag = pProfile->FindTag(icSigAToB0Tag);
        }

        if (!pTag) {
          if (pProfile->m_Header.colorSpace == icSigRgbData) {
            rv = new CIccXformMatrixTRC();
          }
          else
            return NULL;
        }
        else if (pTag->GetType()==icSigMultiProcessElementType) {
          rv = new CIccXformMpe(pTag);
        }
        else {
          switch(pProfile->m_Header.colorSpace) {
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
              rv = new CIccXform3DLut(pTag);
              break;

            case icSigCmykData:
            case icSig4colorData:
              rv = new CIccXform4DLut(pTag);
              break;

            default:
              rv = new CIccXformNDLut(pTag);
              break;
          }
        }
      }
      else {
        CIccTag *pTag = NULL;
        
        if (bUseMpeTags) {
          pTag = pProfile->FindTag(icSigBToD0Tag + nTagIntent);

          if (!pTag && nTagIntent ==icAbsoluteColorimetric) {
            pTag = pProfile->FindTag(icSigBToD1Tag);
            if (pTag)
              nTagIntent = icRelativeColorimetric;
          }

          if (!pTag) {
            pTag = pProfile->FindTag(icSigBToD0Tag);
          }

          //Unsupported elements cause fall back behavior
          if (pTag && !pTag->IsSupported())
            pTag = NULL;
        }

        if (!pTag) {
          if (nTagIntent == icAbsoluteColorimetric)
            nTagIntent = icRelativeColorimetric;
          pTag = pProfile->FindTag(icSigBToA0Tag + nTagIntent);
        }

        if (!pTag) {
          pTag = pProfile->FindTag(icSigBToA0Tag);
        }

        if (!pTag) {
          if (pProfile->m_Header.colorSpace == icSigRgbData) {
            rv = new CIccXformMatrixTRC();
          }
          else
            return NULL;
        }
        else if (pTag->GetType()==icSigMultiProcessElementType) {
          rv = new CIccXformMpe(pTag);
        }
        else {
          switch(pProfile->m_Header.pcs) {
            case icSigXYZData:
            case icSigLabData:
              rv = new CIccXform3DLut(pTag);
              break;

          default:
            break;
          }
        }
      }
      break;

    case icXformLutNamedColor:
      {
        CIccTag *pTag = pProfile->FindTag(icSigNamedColor2Tag);
        if (!pTag)
          return NULL;

        rv = new CIccXformNamedColor(pTag, pProfile->m_Header.pcs, pProfile->m_Header.colorSpace);
      }
      break;

    case icXformLutPreview:
      {
        bInput = false;
        CIccTag *pTag = pProfile->FindTag(icSigPreview0Tag + nTagIntent);
        if (!pTag) {
          pTag = pProfile->FindTag(icSigPreview0Tag);
        }
        if (!pTag) {
          return NULL;
        }
        else {
          switch(pProfile->m_Header.pcs) {
    case icSigXYZData:
    case icSigLabData:
      rv = new CIccXform3DLut(pTag);

    default:
      break;
          }
        }
      }
      break;

    case icXformLutGamut:
      {
        bInput = false;
        CIccTag *pTag = pProfile->FindTag(icSigGamutTag);
        if (!pTag) {
          return NULL;
        }
        else {
          switch(pProfile->m_Header.pcs) {
    case icSigXYZData:
    case icSigLabData:
      rv = new CIccXform3DLut(pTag);

    default:
      break;
          }
        }
      }
      break;
  }

  if (rv) {
    rv->SetParams(pProfile, bInput, nIntent, nInterp);
  }

  return rv;
}

/**
 ******************************************************************************
 * Name: CIccXform::SetParams
 * 
 * Purpose: 
 *   This is an accessor function to set private values.  
 * 
 * Args: 
 *  pProfile = pointer to profile associated with transform
 *  bInput = indicates whether profile is input profile
 *  nIntent = rendering intent to apply to the profile
 *  nInterp = the interpolation algorithm to use for N-D luts
 ******************************************************************************/
void CIccXform::SetParams(CIccProfile *pProfile, bool bInput, icRenderingIntent nIntent, icXformInterp nInterp)
{
  m_pProfile = pProfile;
  m_bInput = bInput;
  m_nIntent = nIntent;
  m_nInterp = nInterp;
}

/**
 **************************************************************************
 * Name: CIccXform::Create
 * 
 * Purpose:
 *  This is a static Creation function that creates derived CIccXform objects and
 *  initializes them.
 * 
 * Args: 
 *  Profile = reference to a CIccProfile object that will be used to create the transform.
 *   A copy of the CIccProfile object will be created and passed to the pointer based Create().
 *   The copied object will be destroyed when the returned CIccXform object is destroyed.
 *  bInput = flag to indicate whether to use the input or output side of the profile,
 *  nIntent = the rendering intent to apply to the profile,   
 *  nInterp = the interpolation algorithm to use for N-D luts.
 *  nLutType = selection of which transform lut to use
 * 
 * Return: 
 *  A suitable pXform object
 **************************************************************************
 */
CIccXform *CIccXform::Create(CIccProfile &Profile, bool bInput, icRenderingIntent nIntent, icXformInterp nInterp, icXformLutType nLutType,
                             bool bUseMpeTags)
{
  CIccProfile *pProfile = new CIccProfile(Profile);
  CIccXform *pXform = Create(pProfile, bInput, nIntent, nInterp, nLutType, bUseMpeTags);

  if (!pXform)
    delete pProfile;

  return pXform;
}


/**
 **************************************************************************
 * Name: CIccXform::Begin
 * 
 * Purpose: 
 *  This function will be called before the xform is applied.  Derived objects
 *  should also call this base class function to initialize for Absolute Colorimetric
 *  Intent handling which is performed through the use of the CheckSrcAbs and
 *  CheckDstAbs functions.
 **************************************************************************
 */
icStatusCMM CIccXform::Begin()
{
  if (m_nIntent==icAbsoluteColorimetric) {
    CIccTag *pTag = m_pProfile->FindTag(icSigMediaWhitePointTag);

    if (!pTag || pTag->GetType()!=icSigXYZType)
      return icCmmStatInvalidProfile;

    CIccTagXYZ *pXyzTag = (CIccTagXYZ*)pTag;

    m_MediaXYZ = (*pXyzTag)[0];

  }
  return icCmmStatOk;
}

/**
 **************************************************************************
 * Name: CIccXform::CheckSrcAbs
 * 
 * Purpose: 
 *  This function will be called by a derived CIccXform object's Apply() function
 *  BEFORE the actual xform is performed to take care of Absolute to Relative
 *  adjustments needed by the xform (IE the PCS is always version 4 relative).
 * 
 * Args: 
 *  Pixel = src pixel data (will not be modified)
 * 
 * Return: 
 *  returns Pixel or adjusted pixel data.
 **************************************************************************
 */
const icFloatNumber *CIccXform::CheckSrcAbs(const icFloatNumber *Pixel)
{
  if (!m_bInput) {
    if (m_nIntent == icAbsoluteColorimetric && 
        (m_MediaXYZ.X != m_pProfile->m_Header.illuminant.X ||
         m_MediaXYZ.Y != m_pProfile->m_Header.illuminant.Y ||
         m_MediaXYZ.Z != m_pProfile->m_Header.illuminant.Z)) {

      icColorSpaceSignature Space = m_pProfile->m_Header.pcs;

      if (IsSpacePCS(Space)) {
        if (Space==icSigLabData) {
          if (UseLegacyPCS()) {
            CIccPCS::Lab2ToXyz(m_AbsLab, Pixel, true);
          }
          else
            CIccPCS::LabToXyz(m_AbsLab, Pixel, true);
          Pixel = m_AbsLab;
        }

        m_AbsLab[0] = Pixel[0] * m_pProfile->m_Header.illuminant.X / m_MediaXYZ.X;
        m_AbsLab[1] = Pixel[1] * m_pProfile->m_Header.illuminant.Y / m_MediaXYZ.Y;
        m_AbsLab[2] = Pixel[2] * m_pProfile->m_Header.illuminant.Z / m_MediaXYZ.Z;

        if (Space==icSigLabData) {
          if (UseLegacyPCS()) {
            CIccPCS::XyzToLab2(m_AbsLab, m_AbsLab, true);
          }
          else {
            CIccPCS::XyzToLab(m_AbsLab, m_AbsLab, true);
          }
        }

        return m_AbsLab;
      }
    }
    else if (m_nIntent == icPerceptual && IsVersion2()) {
      icColorSpaceSignature Space = m_pProfile->m_Header.pcs;

      if (IsSpacePCS(Space)) {
        if (Space==icSigLabData) {
          if (UseLegacyPCS()) {
            CIccPCS::Lab2ToXyz(m_AbsLab, Pixel, true);
          }
          else
            CIccPCS::LabToXyz(m_AbsLab, Pixel, true);
          Pixel = m_AbsLab;
        }

        //Convert version 4 black point to version 2
        m_AbsLab[0] = CIccPCS::NegClip((icFloatNumber)((Pixel[0] - icPerceptualRefBlackX * 32768.0 / 65535.0) / (1.0 - icPerceptualRefBlackX / icPerceptualRefWhiteX)));
        m_AbsLab[1] = CIccPCS::NegClip((icFloatNumber)((Pixel[1] - icPerceptualRefBlackY * 32768.0 / 65535.0) / (1.0 - icPerceptualRefBlackY / icPerceptualRefWhiteY)));
        m_AbsLab[2] = CIccPCS::NegClip((icFloatNumber)((Pixel[2] - icPerceptualRefBlackZ * 32768.0 / 65535.0) / (1.0 - icPerceptualRefBlackZ / icPerceptualRefWhiteZ)));

        if (Space==icSigLabData) {
          if (UseLegacyPCS()) {
            CIccPCS::XyzToLab2(m_AbsLab, m_AbsLab, true);
          }
          else {
            CIccPCS::XyzToLab(m_AbsLab, m_AbsLab, true);
          }
        }

        return m_AbsLab;
      }
    }
  }
  return Pixel;
}

/**
 **************************************************************************
 * Name: CIccXform::CheckDstAbs
 * 
 * Purpose: 
 *  This function will be called by a derived CIccXform object's Apply() function
 *  AFTER the actual xform is performed to take care of Absolute to Relative
 *  adjustments needed by the xform (IE the PCS is always version 4 relative).
 * 
 * Args: 
 *  Pixel = source pixel data which will be modified
 *
 **************************************************************************
 */
void CIccXform::CheckDstAbs(icFloatNumber *Pixel)
{
  if (m_bInput) {
    if (m_nIntent == icAbsoluteColorimetric && 
        (m_MediaXYZ.X != m_pProfile->m_Header.illuminant.X ||
         m_MediaXYZ.Y != m_pProfile->m_Header.illuminant.Y ||
         m_MediaXYZ.Z != m_pProfile->m_Header.illuminant.Z)) {
      icColorSpaceSignature Space = m_pProfile->m_Header.pcs;
      
      if (IsSpacePCS(Space)) {
        if (Space==icSigLabData) {
          if (UseLegacyPCS()) {
            CIccPCS::Lab2ToXyz(Pixel, Pixel, true);
          }
          else
            CIccPCS::LabToXyz(Pixel, Pixel, true);
        }
        
        Pixel[0] = Pixel[0] * m_MediaXYZ.X / m_pProfile->m_Header.illuminant.X;
        Pixel[1] = Pixel[1] * m_MediaXYZ.Y / m_pProfile->m_Header.illuminant.Y;
        Pixel[2] = Pixel[2] * m_MediaXYZ.Z / m_pProfile->m_Header.illuminant.Z;
        
        if (Space==icSigLabData) {
          if (UseLegacyPCS()) {
            CIccPCS::XyzToLab2(Pixel, Pixel, true);
          }
          else {
            CIccPCS::XyzToLab(Pixel, Pixel, true);
          }
        }
      }
    }    
    else if (m_nIntent == icPerceptual && IsVersion2()) {
      icColorSpaceSignature Space = m_pProfile->m_Header.pcs;
      
      if (IsSpacePCS(Space)) {
        if (Space==icSigLabData) {
          if (UseLegacyPCS()) {
            CIccPCS::Lab2ToXyz(Pixel, Pixel, true);
          }
          else
            CIccPCS::LabToXyz(Pixel, Pixel, true);
        }
        
        //Convert version 2 black point to version 4
        Pixel[0] = (icFloatNumber)(Pixel[0] * (1.0 - icPerceptualRefBlackX / icPerceptualRefWhiteX) + icPerceptualRefBlackX * 32768.0 / 65535.0);
        Pixel[1] = (icFloatNumber)(Pixel[1] * (1.0 - icPerceptualRefBlackY / icPerceptualRefWhiteY) + icPerceptualRefBlackY * 32768.0 / 65535.0);
        Pixel[2] = (icFloatNumber)(Pixel[2] * (1.0 - icPerceptualRefBlackZ / icPerceptualRefWhiteZ) + icPerceptualRefBlackZ * 32768.0 / 65535.0);
        
        if (Space==icSigLabData) {
          if (UseLegacyPCS()) {
            CIccPCS::XyzToLab2(Pixel, Pixel, true);
          }
          else {
            CIccPCS::XyzToLab(Pixel, Pixel, true);
          }
        }
      }
    }    
  }
}


/**
**************************************************************************
* Name: CIccXformMatrixTRC::GetSrcSpace
* 
* Purpose: 
*  Return the color space that is input to the transform.  
*  If a device space is either XYZ/Lab it is changed to dXYZ/dLab to avoid
*  confusion with PCS encoding of these spaces.  Device encoding of XYZ
*  and Lab spaces left to the device.
**************************************************************************
*/
icColorSpaceSignature CIccXform::GetSrcSpace() const
{
  icColorSpaceSignature rv;
  icProfileClassSignature deviceClass = m_pProfile->m_Header.deviceClass;

  if (m_bInput) {
    rv = m_pProfile->m_Header.colorSpace;

    if (deviceClass != icSigAbstractClass) {
      //convert signature to device colorspace signature (avoid confusion about encoding).
      if (rv==icSigXYZData) {
        rv = icSigDevXYZData;
      }
      else if (rv==icSigLabData) {
        rv = icSigDevLabData;
      }
    }
  }
  else {
    rv = m_pProfile->m_Header.pcs;
  }

  return rv;
}

/**
**************************************************************************
* Name: CIccXformMatrixTRC::GetDstSpace
* 
* Purpose: 
*  Return the color space that is output by the transform.  
*  If a device space is either XYZ/Lab it is changed to dXYZ/dLab to avoid
*  confusion with PCS encoding of these spaces.  Device encoding of XYZ
*  and Lab spaces left to the device.
**************************************************************************
*/
icColorSpaceSignature CIccXform::GetDstSpace() const
{
  icColorSpaceSignature rv;
  icProfileClassSignature deviceClass = m_pProfile->m_Header.deviceClass;

  if (m_bInput) {
    rv = m_pProfile->m_Header.pcs;
  }
  else {
    rv = m_pProfile->m_Header.colorSpace;

    //convert signature to device colorspace signature (avoid confusion about encoding).
    if (deviceClass != icSigAbstractClass) {
      if (rv==icSigXYZData) {
        rv = icSigDevXYZData;
      }
      else if (rv==icSigLabData) {
        rv = icSigDevLabData;
      }
    }
  }

  return rv;
}


/**
 **************************************************************************
 * Name: CIccXformMatrixTRC::CIccXformMatrixTRC
 * 
 * Purpose: 
 *  Constructor
 **************************************************************************
 */
CIccXformMatrixTRC::CIccXformMatrixTRC()
{
  m_Curve[0] = m_Curve[1] = m_Curve[2] = NULL;
  m_ApplyCurvePtr = NULL;
  m_bFreeCurve = false;
}

/**
 **************************************************************************
 * Name: CIccXformMatrixTRC::~CIccXformMatrixTRC
 * 
 * Purpose: 
 *  Destructor
 **************************************************************************
 */
CIccXformMatrixTRC::~CIccXformMatrixTRC()
{
  if (m_bFreeCurve) {
    if (m_Curve[0])
      delete m_Curve[0];
    if (m_Curve[1])
      delete m_Curve[1];
    if (m_Curve[2])
      delete m_Curve[2];
  }
}

/**
 **************************************************************************
 * Name: CIccXformMatrixTRC::Begin
 * 
 * Purpose: 
 *  Does the initialization of the Xform before Apply() is called.
 *  Must be called before Apply().
 *
 **************************************************************************
 */
icStatusCMM CIccXformMatrixTRC::Begin()
{
  icStatusCMM rv = CIccXform::Begin();
  CIccTagXYZ *pXYZ;

  if (rv != icCmmStatOk)
    return rv;

  pXYZ = GetColumn(icSigRedMatrixColumnTag);
  if (!pXYZ)
    return icCmmStatProfileMissingTag;

  m_e[0] = icFtoD((*pXYZ)[0].X);
  m_e[3] = icFtoD((*pXYZ)[0].Y);
  m_e[6] = icFtoD((*pXYZ)[0].Z);

  pXYZ = GetColumn(icSigGreenMatrixColumnTag);
  if (!pXYZ)
    return icCmmStatProfileMissingTag;

  m_e[1] = icFtoD((*pXYZ)[0].X);
  m_e[4] = icFtoD((*pXYZ)[0].Y);
  m_e[7] = icFtoD((*pXYZ)[0].Z);

  pXYZ = GetColumn(icSigBlueMatrixColumnTag);
  if (!pXYZ)
    return icCmmStatProfileMissingTag;

  m_e[2] = icFtoD((*pXYZ)[0].X);
  m_e[5] = icFtoD((*pXYZ)[0].Y);
  m_e[8] = icFtoD((*pXYZ)[0].Z);

  m_ApplyCurvePtr = NULL;

  if (m_bInput) {
    m_Curve[0] = GetCurve(icSigRedTRCTag);
    m_Curve[1] = GetCurve(icSigGreenTRCTag);
    m_Curve[2] = GetCurve(icSigBlueTRCTag);

    if (!m_Curve[0] || !m_Curve[1] || !m_Curve[2]) {
      return icCmmStatProfileMissingTag;
    }

  }
  else {
    if (m_pProfile->m_Header.pcs!=icSigXYZData)
      return icCmmStatBadSpaceLink;

    m_Curve[0] = GetInvCurve(icSigRedTRCTag);
    m_Curve[1] = GetInvCurve(icSigGreenTRCTag);
    m_Curve[2] = GetInvCurve(icSigBlueTRCTag);

    m_bFreeCurve = true;

    if (!m_Curve[0] || !m_Curve[1] || !m_Curve[2]) {
      return icCmmStatProfileMissingTag;
    }

    if (!icMatrixInvert3x3(m_e)) {
      return icCmmStatInvalidProfile;
    }
  }

  m_Curve[0]->Begin();
  m_Curve[1]->Begin();
  m_Curve[2]->Begin();

  if (!m_Curve[0]->IsIdentity() || !m_Curve[1]->IsIdentity() || !m_Curve[2]->IsIdentity()) {
    m_ApplyCurvePtr = m_Curve;
  }

  return icCmmStatOk;
}


static icFloatNumber XYZScale(icFloatNumber v)
{
  v = (icFloatNumber)(v * 32768.0 / 65535.0);
  return v;
}

static icFloatNumber XYZDescale(icFloatNumber v)
{
  return (icFloatNumber)(v * 65535.0 / 32768.0);
}

static icFloatNumber RGBClip(icFloatNumber v, CIccCurve *pCurve)
{
  if (v<=0)
    return(pCurve->Apply(0));
  else if (v>=1.0)
    return (pCurve->Apply(1.0));

  return pCurve->Apply(v);
}

/**
 **************************************************************************
 * Name: CIccXformMatrixTRC::Apply
 * 
 * Purpose: 
 *  Does the actual application of the Xform.
 *  
 * Args:
 *  DstPixel = Destination pixel where the result is stored,
 *  SrcPixel = Source pixel which is to be applied.
 **************************************************************************
 */
void CIccXformMatrixTRC::Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel)
{
  icFloatNumber Pixel[3];

  SrcPixel = CheckSrcAbs(SrcPixel);
  Pixel[0] = SrcPixel[0];
  Pixel[1] = SrcPixel[1];
  Pixel[2] = SrcPixel[2];

  if (m_bInput) {

    double LinR, LinG, LinB;
    if (m_ApplyCurvePtr) {
      LinR = m_ApplyCurvePtr[0]->Apply(Pixel[0]);
      LinG = m_ApplyCurvePtr[1]->Apply(Pixel[1]);
      LinB = m_ApplyCurvePtr[2]->Apply(Pixel[2]);
    }
    else {
      LinR = Pixel[0];
      LinG = Pixel[1];
      LinB = Pixel[2];
    }

    DstPixel[0] = XYZScale((icFloatNumber)(m_e[0] * LinR + m_e[1] * LinG + m_e[2] * LinB));
    DstPixel[1] = XYZScale((icFloatNumber)(m_e[3] * LinR + m_e[4] * LinG + m_e[5] * LinB));
    DstPixel[2] = XYZScale((icFloatNumber)(m_e[6] * LinR + m_e[7] * LinG + m_e[8] * LinB));
  }
  else {
    double X = XYZDescale(Pixel[0]);
    double Y = XYZDescale(Pixel[1]);
    double Z = XYZDescale(Pixel[2]);

    if (m_ApplyCurvePtr) {
      DstPixel[0] = RGBClip((icFloatNumber)(m_e[0] * X + m_e[1] * Y + m_e[2] * Z), m_ApplyCurvePtr[0]);
      DstPixel[1] = RGBClip((icFloatNumber)(m_e[3] * X + m_e[4] * Y + m_e[5] * Z), m_ApplyCurvePtr[1]);
      DstPixel[2] = RGBClip((icFloatNumber)(m_e[6] * X + m_e[7] * Y + m_e[8] * Z), m_ApplyCurvePtr[2]);
    }
    else {
      DstPixel[0] = (icFloatNumber)(m_e[0] * X + m_e[1] * Y + m_e[2] * Z);
      DstPixel[1] = (icFloatNumber)(m_e[3] * X + m_e[4] * Y + m_e[5] * Z);
      DstPixel[2] = (icFloatNumber)(m_e[6] * X + m_e[7] * Y + m_e[8] * Z);
    }
  }

  CheckDstAbs(DstPixel);
}

/**
 **************************************************************************
 * Name: CIccXformMatrixTRC::GetCurve
 * 
 * Purpose: 
 *  Gets the curve having the passed signature, from the profile.
 *  
 * Args:
 *  sig = signature of the curve to be found
 *
 * Return:
 *  Pointer to the curve.
 **************************************************************************
 */
CIccCurve *CIccXformMatrixTRC::GetCurve(icSignature sig) const
{
  CIccTag *pTag = m_pProfile->FindTag(sig);

  if (pTag->GetType()==icSigCurveType || pTag->GetType()==icSigParametricCurveType) {
    return (CIccCurve*)pTag;
  }

  return NULL;
}

/**
 **************************************************************************
 * Name: CIccXformMatrixTRC::GetColumn
 * 
 * Purpose: 
 *  Gets the XYZ tag from the profile.
 *  
 * Args:
 *  sig = signature of the XYZ tag to be found.
 * 
 * Return:
 *  Pointer to the XYZ tag.
 **************************************************************************
 */
CIccTagXYZ *CIccXformMatrixTRC::GetColumn(icSignature sig) const
{
  CIccTag *pTag = m_pProfile->FindTag(sig);

  if (!pTag || pTag->GetType()!=icSigXYZType) {
    return NULL;
  }

  return (CIccTagXYZ*)pTag;
}

/**
 **************************************************************************
 * Name: CIccXformMatrixTRC::GetInvCurve
 * 
 * Purpose: 
 *  Gets the inverted curve having the passed signature, from the profile.
 *  
 * Args:
 *  sig = signature of the curve to be inverted
 *
 * Return:
 *  Pointer to the inverted curve.
 **************************************************************************
 */
CIccCurve *CIccXformMatrixTRC::GetInvCurve(icSignature sig) const
{
  CIccCurve *pCurve;
  CIccTagCurve *pInvCurve;

  if (!(pCurve = GetCurve(sig)))
    return NULL;

  pCurve->Begin();

  pInvCurve = new CIccTagCurve(2048);

  int i;
  icFloatNumber x;
  icFloatNumber *Lut = &(*pInvCurve)[0];

  for (i=0; i<2048; i++) {
    x=(icFloatNumber)i / 2047;

    Lut[i] = pCurve->Find(x);
  }

  return pInvCurve;
}

/**
**************************************************************************
* Name: CIccXformMatrixTRC::ExtractInputCurves
* 
* Purpose: 
*  Gets the input curves. Should be called only after Begin() 
*  has been called. Once the curves are extracted, they will 
*  not be used by the Apply() function.
*  WARNING:  caller owns the curves and must be deleted by the caller.
*  
* Return:
*  Pointer to the input curves.
**************************************************************************
*/
LPIccCurve* CIccXformMatrixTRC::ExtractInputCurves()
{
  if (m_bInput) {
    if (m_Curve[0]) {
			LPIccCurve* Curve = new LPIccCurve[3];
			Curve[0] = (LPIccCurve)(m_Curve[0]->NewCopy());
			Curve[1] = (LPIccCurve)(m_Curve[1]->NewCopy());
			Curve[2] = (LPIccCurve)(m_Curve[2]->NewCopy());
      m_ApplyCurvePtr = NULL;
      return Curve;
    }
  }

  return NULL;
}

/**
**************************************************************************
* Name: CIccXformMatrixTRC::ExtractOutputCurves
* 
* Purpose: 
*  Gets the output curves. Should be called only after Begin() 
*  has been called. Once the curves are extracted, they will 
*  not be used by the Apply() function.
*  WARNING:  caller owns the curves and must be deleted by the caller.
*  
* Return:
*  Pointer to the output curves.
**************************************************************************
*/
LPIccCurve* CIccXformMatrixTRC::ExtractOutputCurves()
{
  if (!m_bInput) {
    if (m_Curve[0]) {
			LPIccCurve* Curve = new LPIccCurve[3];
			Curve[0] = (LPIccCurve)(m_Curve[0]->NewCopy());
			Curve[1] = (LPIccCurve)(m_Curve[1]->NewCopy());
			Curve[2] = (LPIccCurve)(m_Curve[2]->NewCopy());
      m_ApplyCurvePtr = NULL;
      return Curve;
    }
  }

  return NULL;
}

/**
 **************************************************************************
 * Name: CIccXform3DLut::CIccXform3DLut
 * 
 * Purpose: 
 *  Constructor
 *
 * Args:
 *   pTag = Pointer to the tag of type CIccMBB 
 **************************************************************************
 */
CIccXform3DLut::CIccXform3DLut(CIccTag *pTag)
{
  if (pTag && pTag->IsMBBType()) {
    m_pTag = (CIccMBB*)pTag;
  }
  else
    m_pTag = NULL;

  m_ApplyCurvePtrA = m_ApplyCurvePtrB = m_ApplyCurvePtrM = NULL;
  m_ApplyMatrixPtr = NULL;
}

/**
 **************************************************************************
 * Name: CIccXform3DLut::~CIccXform3DLut
 * 
 * Purpose: 
 *  Destructor
 **************************************************************************
 */
CIccXform3DLut::~CIccXform3DLut()
{
}

/**
 **************************************************************************
 * Name: CIccXform3DLut::Begin
 * 
 * Purpose: 
 *  Does the initialization of the Xform before Apply() is called.
 *  Must be called before Apply().
 *
 **************************************************************************
 */
icStatusCMM CIccXform3DLut::Begin()
{
  icStatusCMM rv = CIccXform::Begin();
  CIccCurve **Curve;
  int i;

  if (rv != icCmmStatOk)
    return rv;

  if (!m_pTag ||
      m_pTag->InputChannels()!=3)
    return icCmmStatInvalidLut;

  m_ApplyCurvePtrA = NULL;
  m_ApplyCurvePtrB = NULL;
  m_ApplyCurvePtrM = NULL;

  if (m_pTag->m_bInputMatrix) {
    if (m_pTag->m_CurvesB) {
      Curve = m_pTag->m_CurvesB;

      Curve[0]->Begin();
      Curve[1]->Begin();
      Curve[2]->Begin();

      if (!Curve[0]->IsIdentity() || !Curve[1]->IsIdentity() || !Curve[2]->IsIdentity()) {
        m_ApplyCurvePtrB = Curve;
      }
    }

    if (m_pTag->m_CurvesM) {
      Curve = m_pTag->m_CurvesM;

      Curve[0]->Begin();
      Curve[1]->Begin();
      Curve[2]->Begin();
      
      if (!Curve[0]->IsIdentity() || !Curve[1]->IsIdentity() || !Curve[2]->IsIdentity()) {
        m_ApplyCurvePtrM = Curve;
      }
    }

    if (m_pTag->m_CLUT) {
      m_pTag->m_CLUT->Begin();
    }

    if (m_pTag->m_CurvesA) {
      Curve = m_pTag->m_CurvesA;

      for (i=0; i<m_pTag->m_nOutput; i++) {
        Curve[i]->Begin();
      }

      for (i=0; i<m_pTag->m_nOutput; i++) {
        if (!Curve[i]->IsIdentity()) {
          m_ApplyCurvePtrA = Curve;
          break;
        }
      }
    }

  }
  else {
    if (m_pTag->m_CurvesA) {
      Curve = m_pTag->m_CurvesA;

      Curve[0]->Begin();
      Curve[1]->Begin();
      Curve[2]->Begin();

      if (!Curve[0]->IsIdentity() || !Curve[1]->IsIdentity() || !Curve[2]->IsIdentity()) {
        m_ApplyCurvePtrA = Curve;
      }
    }

    if (m_pTag->m_CLUT) {
      m_pTag->m_CLUT->Begin();
    }

    if (m_pTag->m_CurvesM) {
      Curve = m_pTag->m_CurvesM;

      for (i=0; i<m_pTag->m_nOutput; i++) {
        Curve[i]->Begin();
      }

      for (i=0; i<m_pTag->m_nOutput; i++) {
        if (!Curve[i]->IsIdentity()) {
          m_ApplyCurvePtrM = Curve;
          break;
        }
      }
    }

    if (m_pTag->m_CurvesB) {
      Curve = m_pTag->m_CurvesB;

      for (i=0; i<m_pTag->m_nOutput; i++) {
        Curve[i]->Begin();
      }

      for (i=0; i<m_pTag->m_nOutput; i++) {
        if (!Curve[i]->IsIdentity()) {
          m_ApplyCurvePtrB = Curve;
          break;
        }
      }
    }
  }

  m_ApplyMatrixPtr = NULL;
  if (m_pTag->m_Matrix) {
    if (m_pTag->m_bInputMatrix) {
      if (m_pTag->m_nInput!=3) {
        return icCmmStatInvalidProfile;
      }
    }
    else {
      if (m_pTag->m_nOutput!=3) {
        return icCmmStatInvalidProfile;
      }
    }

    if (!m_pTag->m_Matrix->IsIdentity()) {
      m_ApplyMatrixPtr = m_pTag->m_Matrix;
    }
  }

  return icCmmStatOk;
}

/**
 **************************************************************************
 * Name: CIccXform3DLut::Apply
 * 
 * Purpose: 
 *  Does the actual application of the Xform.
 *  
 * Args:
 *  DstPixel = Destination pixel where the result is stored,
 *  SrcPixel = Source pixel which is to be applied.
 **************************************************************************
 */
void CIccXform3DLut::Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel)
{
  icFloatNumber Pixel[16];
  int i;

  SrcPixel = CheckSrcAbs(SrcPixel);
  Pixel[0] = SrcPixel[0];
  Pixel[1] = SrcPixel[1];
  Pixel[2] = SrcPixel[2];

  if (m_pTag->m_bInputMatrix) {
    if (m_ApplyCurvePtrB) {
      Pixel[0] = m_ApplyCurvePtrB[0]->Apply(Pixel[0]);
      Pixel[1] = m_ApplyCurvePtrB[1]->Apply(Pixel[1]);
      Pixel[2] = m_ApplyCurvePtrB[2]->Apply(Pixel[2]);
    }

    if (m_ApplyMatrixPtr) {
      m_ApplyMatrixPtr->Apply(Pixel);
    }

    if (m_ApplyCurvePtrM) {
      Pixel[0] = m_ApplyCurvePtrM[0]->Apply(Pixel[0]);
      Pixel[1] = m_ApplyCurvePtrM[1]->Apply(Pixel[1]);
      Pixel[2] = m_ApplyCurvePtrM[2]->Apply(Pixel[2]);
    }

    if (m_pTag->m_CLUT) {
      if (m_nInterp==icInterpLinear)
        m_pTag->m_CLUT->Interp3d(Pixel, Pixel);
      else
        m_pTag->m_CLUT->Interp3dTetra(Pixel, Pixel);
    }

    if (m_ApplyCurvePtrA) {
      for (i=0; i<m_pTag->m_nOutput; i++) {
        Pixel[i] = m_ApplyCurvePtrA[i]->Apply(Pixel[i]);
      }
    }

  }
  else {
    if (m_ApplyCurvePtrA) {
      Pixel[0] = m_ApplyCurvePtrA[0]->Apply(Pixel[0]);
      Pixel[1] = m_ApplyCurvePtrA[1]->Apply(Pixel[1]);
      Pixel[2] = m_ApplyCurvePtrA[2]->Apply(Pixel[2]);
    }

    if (m_pTag->m_CLUT) {
      if (m_nInterp==icInterpLinear)
        m_pTag->m_CLUT->Interp3d(Pixel, Pixel);
      else
        m_pTag->m_CLUT->Interp3dTetra(Pixel, Pixel);
    }

    if (m_ApplyCurvePtrM) {
      for (i=0; i<m_pTag->m_nOutput; i++) {
        Pixel[i] = m_ApplyCurvePtrM[i]->Apply(Pixel[i]);
      }
    }

    if (m_ApplyMatrixPtr) {
      m_ApplyMatrixPtr->Apply(Pixel);
    }

    if (m_ApplyCurvePtrB) {
      for (i=0; i<m_pTag->m_nOutput; i++) {
        Pixel[i] = m_ApplyCurvePtrB[i]->Apply(Pixel[i]);
      }
    }
  }

  for (i=0; i<m_pTag->m_nOutput; i++) {
    DstPixel[i] = Pixel[i];
  }

  CheckDstAbs(DstPixel);
}

/**
**************************************************************************
* Name: CIccXform3DLut::ExtractInputCurves
* 
* Purpose: 
*  Gets the input curves. Should be called only after Begin() 
*  has been called. Once the curves are extracted, they will 
*  not be used by the Apply() function.
*  WARNING:  caller owns the curves and must be deleted by the caller.
*  
* Return:
*  Pointer to the input curves.
**************************************************************************
*/
LPIccCurve* CIccXform3DLut::ExtractInputCurves()
{
  if (m_bInput) {
    if (m_pTag->m_bInputMatrix) {
      if (m_pTag->m_CurvesB) {
        LPIccCurve* Curve = new LPIccCurve[3];
				Curve[0] = (LPIccCurve)(m_pTag->m_CurvesB[0]->NewCopy());
				Curve[1] = (LPIccCurve)(m_pTag->m_CurvesB[1]->NewCopy());
				Curve[2] = (LPIccCurve)(m_pTag->m_CurvesB[2]->NewCopy());
        m_ApplyCurvePtrB = NULL;
        return Curve;
      }
    }
    else {
      if (m_pTag->m_CurvesA) {
        LPIccCurve* Curve = new LPIccCurve[3];
				Curve[0] = (LPIccCurve)(m_pTag->m_CurvesA[0]->NewCopy());
				Curve[1] = (LPIccCurve)(m_pTag->m_CurvesA[1]->NewCopy());
				Curve[2] = (LPIccCurve)(m_pTag->m_CurvesA[2]->NewCopy());
        m_ApplyCurvePtrA = NULL;
        return Curve;
      }
    }
  }

  return NULL;
}

/**
**************************************************************************
* Name: CIccXform3DLut::ExtractOutputCurves
* 
* Purpose: 
*  Gets the output curves. Should be called only after Begin() 
*  has been called. Once the curves are extracted, they will 
*  not be used by the Apply() function.
*  WARNING:  caller owns the curves and must be deleted by the caller.
*  
* Return:
*  Pointer to the output curves.
**************************************************************************
*/
LPIccCurve* CIccXform3DLut::ExtractOutputCurves()
{
  if (!m_bInput) {
    if (m_pTag->m_bInputMatrix) {
      if (m_pTag->m_CurvesA) {
        LPIccCurve* Curve = new LPIccCurve[m_pTag->m_nOutput];
				for (int i=0; i<m_pTag->m_nOutput; i++) {
					Curve[i] = (LPIccCurve)(m_pTag->m_CurvesA[i]->NewCopy());
				}
        m_ApplyCurvePtrA = NULL;
        return Curve;
      }
    }
    else {
      if (m_pTag->m_CurvesB) {
        LPIccCurve* Curve = new LPIccCurve[m_pTag->m_nOutput];
				for (int i=0; i<m_pTag->m_nOutput; i++) {
					Curve[i] = (LPIccCurve)(m_pTag->m_CurvesB[i]->NewCopy());
				}
        m_ApplyCurvePtrB = NULL;
        return Curve;
      }
    }
  }

  return NULL;
}

/**
 **************************************************************************
 * Name: CIccXform4DLut::CIccXform4DLut
 * 
 * Purpose: 
 *  Constructor
 *
 * Args:
 *   pTag = Pointer to the tag of type CIccMBB 
 **************************************************************************
 */
CIccXform4DLut::CIccXform4DLut(CIccTag *pTag)
{
  if (pTag && pTag->IsMBBType()) {
    m_pTag = (CIccMBB*)pTag;
  }
  else
    m_pTag = NULL;

  m_ApplyCurvePtrA = m_ApplyCurvePtrB = m_ApplyCurvePtrM = NULL;
  m_ApplyMatrixPtr = NULL;
}


/**
 **************************************************************************
 * Name: CIccXform4DLut::~CIccXform4DLut
 * 
 * Purpose: 
 *  Destructor
 **************************************************************************
 */
CIccXform4DLut::~CIccXform4DLut()
{
}


/**
 **************************************************************************
 * Name: CIccXform4DLut::Begin
 * 
 * Purpose: 
 *  Does the initialization of the Xform before Apply() is called.
 *  Must be called before Apply().
 *
 **************************************************************************
 */
icStatusCMM CIccXform4DLut::Begin()
{
  icStatusCMM rv = CIccXform::Begin();
  CIccCurve **Curve;
  int i;

  if (rv != icCmmStatOk)
    return rv;

  if (!m_pTag ||
      m_pTag->InputChannels()!=4)
    return icCmmStatInvalidLut;

  m_ApplyCurvePtrA = m_ApplyCurvePtrB = m_ApplyCurvePtrM = NULL;

  if (m_pTag->m_bInputMatrix) {
    if (m_pTag->m_CurvesB) {
      Curve = m_pTag->m_CurvesB;

      Curve[0]->Begin();
      Curve[1]->Begin();
      Curve[2]->Begin();
      Curve[3]->Begin();

      if (!Curve[0]->IsIdentity() || !Curve[1]->IsIdentity() ||
          !Curve[2]->IsIdentity() || !Curve[3]->IsIdentity()) 
      {
        m_ApplyCurvePtrB = Curve;
      }
    }

    if (m_pTag->m_CLUT) {
      m_pTag->m_CLUT->Begin();
    }

    if (m_pTag->m_CurvesA) {
      Curve = m_pTag->m_CurvesA;

      for (i=0; i<m_pTag->m_nOutput; i++) {
        Curve[i]->Begin();
      }

      for (i=0; i<m_pTag->m_nOutput; i++) {
        if (!Curve[i]->IsIdentity()) {
          m_ApplyCurvePtrA = Curve;
          break;
        }
      }
    }

  }
  else {
    if (m_pTag->m_CurvesA) {
      Curve = m_pTag->m_CurvesA;

      Curve[0]->Begin();
      Curve[1]->Begin();
      Curve[2]->Begin();
      Curve[3]->Begin();

      if (!Curve[0]->IsIdentity() || !Curve[1]->IsIdentity() ||
          !Curve[2]->IsIdentity() || !Curve[3]->IsIdentity()) 
      {
        m_ApplyCurvePtrA = Curve;
      }
    }

    if (m_pTag->m_CLUT) {
      m_pTag->m_CLUT->Begin();
    }

    if (m_pTag->m_CurvesM) {
      Curve = m_pTag->m_CurvesM;

      for (i=0; i<m_pTag->m_nOutput; i++) {
        Curve[i]->Begin();
      }

      for (i=0; i<m_pTag->m_nOutput; i++) {
        if (!Curve[i]->IsIdentity()) {
          m_ApplyCurvePtrM = Curve;
          break;
        }
      }
    }

    if (m_pTag->m_CurvesB) {
      Curve = m_pTag->m_CurvesB;

      for (i=0; i<m_pTag->m_nOutput; i++) {
        Curve[i]->Begin();
      }

      for (i=0; i<m_pTag->m_nOutput; i++) {
        if (!Curve[i]->IsIdentity()) {
          m_ApplyCurvePtrB = Curve;
          break;
        }
      }
    }
  }

  m_ApplyMatrixPtr = NULL;
  if (m_pTag->m_Matrix) {
    if (m_pTag->m_bInputMatrix) {
      return icCmmStatInvalidProfile;
    }
    else {
      if (m_pTag->m_nOutput!=3) {
        return icCmmStatInvalidProfile;
      }
    }

    if (!m_pTag->m_Matrix->IsIdentity()) {
      m_ApplyMatrixPtr = m_pTag->m_Matrix;
    }
  }

  return icCmmStatOk;
}


/**
 **************************************************************************
 * Name: CIccXform4DLut::Apply
 * 
 * Purpose: 
 *  Does the actual application of the Xform.
 *  
 * Args:
 *  DstPixel = Destination pixel where the result is stored,
 *  SrcPixel = Source pixel which is to be applied.
 **************************************************************************
 */
void CIccXform4DLut::Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel)
{
  icFloatNumber Pixel[16];
  int i;

  SrcPixel = CheckSrcAbs(SrcPixel);
  Pixel[0] = SrcPixel[0];
  Pixel[1] = SrcPixel[1];
  Pixel[2] = SrcPixel[2];
  Pixel[3] = SrcPixel[3];

  if (m_pTag->m_bInputMatrix) {
    if (m_ApplyCurvePtrB) {
      Pixel[0] = m_ApplyCurvePtrB[0]->Apply(Pixel[0]);
      Pixel[1] = m_ApplyCurvePtrB[1]->Apply(Pixel[1]);
      Pixel[2] = m_ApplyCurvePtrB[2]->Apply(Pixel[2]);
      Pixel[3] = m_ApplyCurvePtrB[3]->Apply(Pixel[3]);
    }

    if (m_pTag->m_CLUT) {
      m_pTag->m_CLUT->Interp4d(Pixel, Pixel);
    }

    if (m_ApplyCurvePtrA) {
      for (i=0; i<m_pTag->m_nOutput; i++) {
        Pixel[i] = m_ApplyCurvePtrA[i]->Apply(Pixel[i]);
      }
    }

  }
  else {
    if (m_ApplyCurvePtrA) {
      Pixel[0] = m_ApplyCurvePtrA[0]->Apply(Pixel[0]);
      Pixel[1] = m_ApplyCurvePtrA[1]->Apply(Pixel[1]);
      Pixel[2] = m_ApplyCurvePtrA[2]->Apply(Pixel[2]);
      Pixel[3] = m_ApplyCurvePtrA[3]->Apply(Pixel[3]);
    }

    if (m_pTag->m_CLUT) {
      m_pTag->m_CLUT->Interp4d(Pixel, Pixel);
    }

    if (m_ApplyCurvePtrM) {
      for (i=0; i<m_pTag->m_nOutput; i++) {
        Pixel[i] = m_ApplyCurvePtrM[i]->Apply(Pixel[i]);
      }
    }

    if (m_ApplyMatrixPtr) {
      m_ApplyMatrixPtr->Apply(Pixel);
    }

    if (m_ApplyCurvePtrB) {
      for (i=0; i<m_pTag->m_nOutput; i++) {
        Pixel[i] = m_ApplyCurvePtrB[i]->Apply(Pixel[i]);
      }
    }
  }

  for (i=0; i<m_pTag->m_nOutput; i++) {
    DstPixel[i] = Pixel[i];
  }

  CheckDstAbs(DstPixel);
}

/**
**************************************************************************
* Name: CIccXform4DLut::ExtractInputCurves
* 
* Purpose: 
*  Gets the input curves. Should be called only after Begin() 
*  has been called. Once the curves are extracted, they will 
*  not be used by the Apply() function.
*  WARNING:  caller owns the curves and must be deleted by the caller.
*  
* Return:
*  Pointer to the input curves.
**************************************************************************
*/
LPIccCurve* CIccXform4DLut::ExtractInputCurves()
{
	if (m_bInput) {
		if (m_pTag->m_bInputMatrix) {
			if (m_pTag->m_CurvesB) {
				LPIccCurve* Curve = new LPIccCurve[4];
				Curve[0] = (LPIccCurve)(m_pTag->m_CurvesB[0]->NewCopy());
				Curve[1] = (LPIccCurve)(m_pTag->m_CurvesB[1]->NewCopy());
				Curve[2] = (LPIccCurve)(m_pTag->m_CurvesB[2]->NewCopy());
				Curve[3] = (LPIccCurve)(m_pTag->m_CurvesB[3]->NewCopy());
				m_ApplyCurvePtrB = NULL;
				return Curve;
			}
		}
		else {
			if (m_pTag->m_CurvesA) {
				LPIccCurve* Curve = new LPIccCurve[4];
				Curve[0] = (LPIccCurve)(m_pTag->m_CurvesA[0]->NewCopy());
				Curve[1] = (LPIccCurve)(m_pTag->m_CurvesA[1]->NewCopy());
				Curve[2] = (LPIccCurve)(m_pTag->m_CurvesA[2]->NewCopy());
				Curve[3] = (LPIccCurve)(m_pTag->m_CurvesA[3]->NewCopy());
				m_ApplyCurvePtrA = NULL;
				return Curve;
			}
		}
	}

  return NULL;
}

/**
**************************************************************************
* Name: CIccXform4DLut::ExtractOutputCurves
* 
* Purpose: 
*  Gets the output curves. Should be called only after Begin() 
*  has been called. Once the curves are extracted, they will 
*  not be used by the Apply() function.
*  WARNING:  caller owns the curves and must be deleted by the caller.
*  
* Return:
*  Pointer to the output curves.
**************************************************************************
*/
LPIccCurve* CIccXform4DLut::ExtractOutputCurves()
{
	if (!m_bInput) {
		if (m_pTag->m_bInputMatrix) {
			if (m_pTag->m_CurvesA) {
				LPIccCurve* Curve = new LPIccCurve[m_pTag->m_nOutput];
				for (int i=0; i<m_pTag->m_nOutput; i++) {
					Curve[i] = (LPIccCurve)(m_pTag->m_CurvesA[i]->NewCopy());
				}
				m_ApplyCurvePtrA = NULL;
				return Curve;
			}
		}
		else {
			if (m_pTag->m_CurvesB) {
				LPIccCurve* Curve = new LPIccCurve[m_pTag->m_nOutput];
				for (int i=0; i<m_pTag->m_nOutput; i++) {
					Curve[i] = (LPIccCurve)(m_pTag->m_CurvesB[i]->NewCopy());
				}
				m_ApplyCurvePtrB = NULL;
				return Curve;
			}
		}
	}

  return NULL;
}

/**
 **************************************************************************
 * Name: CIccXformNDLut::CIccXformNDLut
 * 
 * Purpose: 
 *  Constructor
 *
 * Args:
 *   pTag = Pointer to the tag of type CIccMBB 
 **************************************************************************
 */
CIccXformNDLut::CIccXformNDLut(CIccTag *pTag)
{
  if (pTag && pTag->IsMBBType()) {
    m_pTag = (CIccMBB*)pTag;
  }
  else
    m_pTag = NULL;

  m_ApplyCurvePtrA = m_ApplyCurvePtrB = m_ApplyCurvePtrM = NULL;
  m_ApplyMatrixPtr = NULL;
}


/**
 **************************************************************************
 * Name: CIccXformNDLut::~CIccXformNDLut
 * 
 * Purpose: 
 *  Destructor
 **************************************************************************
 */
CIccXformNDLut::~CIccXformNDLut()
{
}


/**
 **************************************************************************
 * Name: CIccXformNDLut::Begin
 * 
 * Purpose: 
 *  Does the initialization of the Xform before Apply() is called.
 *  Must be called before Apply().
 *
 **************************************************************************
 */
icStatusCMM CIccXformNDLut::Begin()
{
  icStatusCMM rv = CIccXform::Begin();
  CIccCurve **Curve;
  int i;

  if (rv != icCmmStatOk)
    return rv;

  if (!m_pTag || (m_pTag->InputChannels()>2 && m_pTag->InputChannels()<5))
    return icCmmStatInvalidLut;

  m_nNumInput = m_pTag->m_nInput;

  m_ApplyCurvePtrA = m_ApplyCurvePtrB = m_ApplyCurvePtrM = NULL;

  if (m_pTag->m_bInputMatrix) {
    if (m_pTag->m_CurvesB) {
      Curve = m_pTag->m_CurvesB;

      for (i=0; i<m_nNumInput; i++)
        Curve[i]->Begin();

      for (i=0; i<m_nNumInput; i++) {
        if (!Curve[i]->IsIdentity()) {
          m_ApplyCurvePtrB = Curve;
          break;
        }
      }
    }

    if (m_pTag->m_CLUT) {
      m_pTag->m_CLUT->Begin();
    }

    if (m_pTag->m_CurvesA) {
      Curve = m_pTag->m_CurvesA;

      for (i=0; i<m_pTag->m_nOutput; i++) {
        Curve[i]->Begin();
      }

      for (i=0; i<m_pTag->m_nOutput; i++) {
        if (!Curve[i]->IsIdentity()) {
          m_ApplyCurvePtrA = Curve;
          break;
        }
      }
    }

  }
  else {
    if (m_pTag->m_CurvesA) {
      Curve = m_pTag->m_CurvesA;

      for (i=0; i<m_nNumInput; i++)
        Curve[i]->Begin();

      for (i=0; i<m_nNumInput; i++) {
        if (!Curve[i]->IsIdentity()) {
          m_ApplyCurvePtrA = Curve;
          break;
        }
      }
    }

    if (m_pTag->m_CLUT) {
      m_pTag->m_CLUT->Begin();
    }

    if (m_pTag->m_CurvesM) {
      Curve = m_pTag->m_CurvesM;

      for (i=0; i<m_pTag->m_nOutput; i++) {
        Curve[i]->Begin();
      }

      for (i=0; i<m_pTag->m_nOutput; i++) {
        if (!Curve[i]->IsIdentity()) {
          m_ApplyCurvePtrM = Curve;
          break;
        }
      }
    }

    if (m_pTag->m_CurvesB) {
      Curve = m_pTag->m_CurvesB;

      for (i=0; i<m_pTag->m_nOutput; i++) {
        Curve[i]->Begin();
      }

      for (i=0; i<m_pTag->m_nOutput; i++) {
        if (!Curve[i]->IsIdentity()) {
          m_ApplyCurvePtrB = Curve;
          break;
        }
      }
    }
  }

  m_ApplyMatrixPtr = NULL;
  if (m_pTag->m_Matrix) {
    if (m_pTag->m_bInputMatrix) {
      return icCmmStatInvalidProfile;
    }
    else {
      if (m_pTag->m_nOutput!=3) {
        return icCmmStatInvalidProfile;
      }
    }

    if (!m_pTag->m_Matrix->IsIdentity()) {
      m_ApplyMatrixPtr = m_pTag->m_Matrix;
    }
  }

  return icCmmStatOk;
}


/**
 **************************************************************************
 * Name: CIccXformNDLut::Apply
 * 
 * Purpose: 
 *  Does the actual application of the Xform.
 *  
 * Args:
 *  DstPixel = Destination pixel where the result is stored,
 *  SrcPixel = Source pixel which is to be applied.
 **************************************************************************
 */
void CIccXformNDLut::Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel)
{
  icFloatNumber Pixel[16];
  int i;

  SrcPixel = CheckSrcAbs(SrcPixel);
  for (i=0; i<m_nNumInput; i++)
    Pixel[i] = SrcPixel[i];

  if (m_pTag->m_bInputMatrix) {
    if (m_ApplyCurvePtrB) {
      for (i=0; i<m_nNumInput; i++)
        Pixel[i] = m_ApplyCurvePtrB[i]->Apply(Pixel[i]);
    }

    if (m_pTag->m_CLUT) {
      switch(m_nNumInput) {
      case 5:
        m_pTag->m_CLUT->Interp5d(Pixel, Pixel);
        break;
      case 6:
        m_pTag->m_CLUT->Interp6d(Pixel, Pixel);
        break;
      default:
        m_pTag->m_CLUT->InterpND(Pixel, Pixel);
        break;
      }
    }

    if (m_ApplyCurvePtrA) {
      for (i=0; i<m_pTag->m_nOutput; i++) {
        Pixel[i] = m_ApplyCurvePtrA[i]->Apply(Pixel[i]);
      }
    }

  }
  else {
    if (m_ApplyCurvePtrA) {
      for (i=0; i<m_nNumInput; i++)
        Pixel[i] = m_ApplyCurvePtrA[i]->Apply(Pixel[i]);
    }

    if (m_pTag->m_CLUT) {
      switch(m_nNumInput) {
      case 5:
        m_pTag->m_CLUT->Interp5d(Pixel, Pixel);
        break;
      case 6:
        m_pTag->m_CLUT->Interp6d(Pixel, Pixel);
        break;
      default:
        m_pTag->m_CLUT->InterpND(Pixel, Pixel);
        break;
      }
    }

    if (m_ApplyCurvePtrM) {
      for (i=0; i<m_pTag->m_nOutput; i++) {
        Pixel[i] = m_ApplyCurvePtrM[i]->Apply(Pixel[i]);
      }
    }

    if (m_ApplyMatrixPtr) {
      m_ApplyMatrixPtr->Apply(Pixel);
    }

    if (m_ApplyCurvePtrB) {
      for (i=0; i<m_pTag->m_nOutput; i++) {
        Pixel[i] = m_ApplyCurvePtrB[i]->Apply(Pixel[i]);
      }
    }
  }

  for (i=0; i<m_pTag->m_nOutput; i++) {
    DstPixel[i] = Pixel[i];
  }

  CheckDstAbs(DstPixel);
}

/**
**************************************************************************
* Name: CIccXformNDLut::ExtractInputCurves
* 
* Purpose: 
*  Gets the input curves. Should be called only after Begin() 
*  has been called. Once the curves are extracted, they will 
*  not be used by the Apply() function.
*  WARNING:  caller owns the curves and must be deleted by the caller.
*  
* Return:
*  Pointer to the input curves.
**************************************************************************
*/
LPIccCurve* CIccXformNDLut::ExtractInputCurves()
{
	if (m_bInput) {
		if (m_pTag->m_bInputMatrix) {
			if (m_pTag->m_CurvesB) {
				LPIccCurve* Curve = new LPIccCurve[m_pTag->m_nInput];
				for (int i=0; i<m_pTag->m_nInput; i++) {
					Curve[i] = (LPIccCurve)(m_pTag->m_CurvesB[i]->NewCopy());
				}
				m_ApplyCurvePtrB = NULL;
				return Curve;
			}
		}
		else {
			if (m_pTag->m_CurvesA) {
				LPIccCurve* Curve = new LPIccCurve[m_pTag->m_nInput];
				for (int i=0; i<m_pTag->m_nInput; i++) {
					Curve[i] = (LPIccCurve)(m_pTag->m_CurvesA[i]->NewCopy());
				}
				m_ApplyCurvePtrA = NULL;
				return Curve;
			}
		}
	}

  return NULL;
}

/**
**************************************************************************
* Name: CIccXformNDLut::ExtractOutputCurves
* 
* Purpose: 
*  Gets the output curves. Should be called only after Begin() 
*  has been called. Once the curves are extracted, they will 
*  not be used by the Apply() function.
*  WARNING:  caller owns the curves and must be deleted by the caller.
*  
* Return:
*  Pointer to the output curves.
**************************************************************************
*/
LPIccCurve* CIccXformNDLut::ExtractOutputCurves()
{
	if (!m_bInput) {
		if (m_pTag->m_bInputMatrix) {
			if (m_pTag->m_CurvesA) {
				LPIccCurve* Curve = new LPIccCurve[m_pTag->m_nOutput];
				for (int i=0; i<m_pTag->m_nOutput; i++) {
					Curve[i] = (LPIccCurve)(m_pTag->m_CurvesA[i]->NewCopy());
				}
				m_ApplyCurvePtrA = NULL;
				return Curve;
			}
		}
		else {
			if (m_pTag->m_CurvesB) {
				LPIccCurve* Curve = new LPIccCurve[m_pTag->m_nOutput];
				for (int i=0; i<m_pTag->m_nOutput; i++) {
					Curve[i] = (LPIccCurve)(m_pTag->m_CurvesB[i]->NewCopy());
				}
				m_ApplyCurvePtrB = NULL;
				return Curve;
			}
		}
	}

  return NULL;
}

/**
 **************************************************************************
 * Name: CIccXformNamedColor::CIccXformNamedColor
 * 
 * Purpose: 
 *  Constructor
 *
 * Args:
 *  pTag = Pointer to the tag of type CIccTagNamedColor2,
 *  csPCS = PCS color space,
 *  csDevice = Device color space 
 **************************************************************************
 */
CIccXformNamedColor::CIccXformNamedColor(CIccTag *pTag, icColorSpaceSignature csPCS, icColorSpaceSignature csDevice)
{
  if (pTag && pTag->GetType()==icSigNamedColor2Type) {
    m_pTag = (CIccTagNamedColor2*)pTag;
    m_pTag->SetColorSpaces(csPCS, csDevice);
  }
  else
    m_pTag = NULL;

  m_nSrcSpace = icSigUnknownData;
  m_nDestSpace = icSigUnknownData;
}


/**
 **************************************************************************
 * Name: CIccXformNamedColor::CIccXformNamedColor
 * 
 * Purpose: 
 *  Destructor
 **************************************************************************
 */
CIccXformNamedColor::~CIccXformNamedColor()
{
}

/**
 **************************************************************************
 * Name: CIccXformNamedColor::Begin
 * 
 * Purpose: 
 *  Does the initialization of the Xform before Apply() is called.
 *  Must be called before Apply().
 *
 **************************************************************************
 */
icStatusCMM CIccXformNamedColor::Begin()
{
  icStatusCMM rv = CIccXform::Begin();
  if (rv != icCmmStatOk)
    return rv;

  if (m_pTag == NULL)
    return icCmmStatProfileMissingTag;

  if (m_nSrcSpace==icSigUnknownData ||
     m_nDestSpace==icSigUnknownData)
    return icCmmStatIncorrectApply;

  if (m_nSrcSpace != icSigNamedData) {
    if (m_nDestSpace != icSigNamedData) {
      m_nApplyInterface = icApplyPixel2Pixel;
    }
    else {
      m_nApplyInterface = icApplyPixel2Named;
    }
  }
  else {
    if (m_nDestSpace != icSigNamedData) {
      m_nApplyInterface = icApplyNamed2Pixel;
    }
    else {
      return icCmmStatIncorrectApply;
    }
  }

  return icCmmStatOk;
}



/**
 **************************************************************************
 * Name: CIccXformNamedColor::Apply
 * 
 * Purpose: 
 *  Does the actual application of the Xform.
 *  
 * Args:
 *  DstPixel = Destination pixel where the result is stored,
 *  SrcPixel = Source pixel which is to be applied.
 **************************************************************************
 */
void CIccXformNamedColor::Apply(icChar *DstColorName, const icFloatNumber *SrcPixel)
{
  if (m_pTag == NULL)
    return;

  icFloatNumber DevicePix[16], PCSPix[3];
  std::string NamedColor;
  icUInt32Number i, j;

  if (IsSrcPCS()) {
    SrcPixel = CheckSrcAbs(SrcPixel);
    for(i=0; i<3; i++)
      PCSPix[i] = SrcPixel[i];

    j = m_pTag->FindPCSColor(PCSPix);
    m_pTag->GetColorName(NamedColor, j);
  }
  else {
    for(i=0; i<m_pTag->GetDeviceCoords(); i++)
      DevicePix[i] = SrcPixel[i];

    j = m_pTag->FindDeviceColor(DevicePix);
    m_pTag->GetColorName(NamedColor, j);
  }

  sprintf(DstColorName, "%s", NamedColor.c_str());
}

icStatusCMM CIccXformNamedColor::Apply(icFloatNumber *DstPixel, const icChar *SrcColorName)
{
  if (m_pTag == NULL)
    return icCmmStatProfileMissingTag;

  icUInt32Number j;

  if (m_nSrcSpace != icSigNamedData)
    return icCmmStatBadSpaceLink;

  if (IsDestPCS()) {

    j = m_pTag->FindColor(SrcColorName);
    if (j<0)
      return icCmmStatColorNotFound;

    if (m_nDestSpace == icSigLabData) {
      memcpy(DstPixel, m_pTag->GetEntry(j)->pcsCoords, 3*sizeof(icFloatNumber));
    }
    else {
      memcpy(DstPixel, m_pTag->GetEntry(j)->pcsCoords, 3*sizeof(icFloatNumber));
    }
    CheckDstAbs(DstPixel);
  }
  else {
    j = m_pTag->FindColor(SrcColorName);
    if (j<0)
      return icCmmStatColorNotFound;
    memcpy(DstPixel, m_pTag->GetEntry(j)->deviceCoords, m_pTag->GetDeviceCoords()*sizeof(icFloatNumber));
  }

  return icCmmStatOk;
}

/**
 **************************************************************************
 * Name: CIccXformNamedColor::SetSrcSpace
 * 
 * Purpose: 
 *  Sets the source space of the Xform
 *  
 * Args:
 *  nSrcSpace = signature of the color space to be set
 **************************************************************************
 */
icStatusCMM CIccXformNamedColor::SetSrcSpace(icColorSpaceSignature nSrcSpace)
{
  if (nSrcSpace!=m_pTag->GetPCS())
    if (nSrcSpace!=m_pTag->GetDeviceSpace())
      if (nSrcSpace!=icSigNamedData)
        return icCmmStatBadSpaceLink;

  m_nSrcSpace = nSrcSpace;

  return icCmmStatOk;
}

/**
 **************************************************************************
 * Name: CIccXformNamedColor::SetSrcSpace
 * 
 * Purpose: 
 *  Sets the destination space of the Xform
 *  
 * Args:
 *  nDestSpace = signature of the color space to be set
 **************************************************************************
 */
icStatusCMM CIccXformNamedColor::SetDestSpace(icColorSpaceSignature nDestSpace)
{
  if (m_nSrcSpace == nDestSpace)
    return icCmmStatBadSpaceLink;

  if (nDestSpace!=m_pTag->GetPCS())
    if (nDestSpace!=m_pTag->GetDeviceSpace())
      if (nDestSpace!=icSigNamedData)
        return icCmmStatBadSpaceLink;

  m_nDestSpace = nDestSpace;

  return icCmmStatOk;
}

/**
**************************************************************************
* Name: CIccXformMPE::CIccXformMPE
* 
* Purpose: 
*  Constructor
**************************************************************************
*/
CIccXformMpe::CIccXformMpe(CIccTag *pTag)
{
  if (pTag && pTag->GetType()==icSigMultiProcessElementType)
    m_pTag = (CIccTagMultiProcessElement*)pTag;
  else
    m_pTag = NULL;

  m_bUsingAcs = false;
}

/**
**************************************************************************
* Name: CIccXformMPE::~CIccXformMPE
* 
* Purpose: 
*  Destructor
**************************************************************************
*/
CIccXformMpe::~CIccXformMpe()
{
}

/**
**************************************************************************
* Name: CIccXformMPE::Create
* 
* Purpose:
*  This is a static Creation function that creates derived CIccXform objects and
*  initializes them.
* 
* Args: 
*  pProfile = pointer to a CIccProfile object that will be owned by the transform.  This object will
*   be destroyed when the returned CIccXform object is destroyed.  The means that the CIccProfile
*   object needs to be allocated on the heap.
*  bInput = flag to indicate whether to use the input or output side of the profile,
*  nIntent = the rendering intent to apply to the profile,   
*  nInterp = the interpolation algorithm to use for N-D luts.
*  nLutType = selection of which transform lut to use
* 
* Return: 
*  A suitable pXform object
**************************************************************************
*/
CIccXform *CIccXformMpe::Create(CIccProfile *pProfile, bool bInput, icRenderingIntent nIntent, 
                                icXformInterp nInterp, icXformLutType nLutType)
{
  CIccXform *rv = NULL;
  icRenderingIntent nTagIntent = nIntent;

  if (nTagIntent == icUnknownIntent)
    nTagIntent = icPerceptual;

  switch (nLutType) {
    case icXformLutColor:
      if (bInput) {
        CIccTag *pTag = pProfile->FindTag(icSigDToB0Tag + nTagIntent);

        if (!pTag && nTagIntent ==icAbsoluteColorimetric) {
          pTag = pProfile->FindTag(icSigDToB1Tag);
          if (pTag)
            nTagIntent = icRelativeColorimetric;
        }

        if (!pTag) {
          pTag = pProfile->FindTag(icSigDToB0Tag);
        }

        //Unsupported elements cause fall back behavior
        if (pTag && !pTag->IsSupported())
          pTag = NULL;

        if (!pTag) {
          if (nTagIntent == icAbsoluteColorimetric)
            nTagIntent = icRelativeColorimetric;
          pTag = pProfile->FindTag(icSigAToB0Tag + nTagIntent);
        }

        if (!pTag) {
          pTag = pProfile->FindTag(icSigAToB0Tag);
        }

        if (!pTag) {
          if (pProfile->m_Header.colorSpace == icSigRgbData) {
            rv = new CIccXformMatrixTRC();
          }
          else
            return NULL;
        }
        else if (pTag->GetType()==icSigMultiProcessElementType) {
          rv = new CIccXformMpe(pTag);
        }
        else {
          switch(pProfile->m_Header.colorSpace) {
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
      rv = new CIccXform3DLut(pTag);
      break;

    case icSigCmykData:
    case icSig4colorData:
      rv = new CIccXform4DLut(pTag);
      break;

    default:
      rv = new CIccXformNDLut(pTag);
      break;
          }
        }
      }
      else {
        CIccTag *pTag = pProfile->FindTag(icSigBToD0Tag + nTagIntent);

        if (!pTag && nTagIntent ==icAbsoluteColorimetric) {
          pTag = pProfile->FindTag(icSigBToD1Tag);
          if (pTag)
            nTagIntent = icRelativeColorimetric;
        }

        if (!pTag) {
          pTag = pProfile->FindTag(icSigBToD0Tag);
        }

        //Unsupported elements cause fall back behavior
        if (pTag && !pTag->IsSupported())
          pTag = NULL;

        if (!pTag) {
          if (nTagIntent == icAbsoluteColorimetric)
            nTagIntent = icRelativeColorimetric;
          pTag = pProfile->FindTag(icSigBToA0Tag + nTagIntent);
        }

        if (!pTag) {
          pTag = pProfile->FindTag(icSigBToA0Tag);
        }

        if (!pTag) {
          if (pProfile->m_Header.colorSpace == icSigRgbData) {
            rv = new CIccXformMatrixTRC();
          }
          else
            return NULL;
        }
        if (pTag->GetType()==icSigMultiProcessElementType) {
          rv = new CIccXformMpe(pTag);
        }
        else {
          switch(pProfile->m_Header.pcs) {
    case icSigXYZData:
    case icSigLabData:
      rv = new CIccXform3DLut(pTag);

    default:
      break;
          }
        }
      }
      break;

    case icXformLutNamedColor:
      {
        CIccTag *pTag = pProfile->FindTag(icSigNamedColor2Tag);
        if (!pTag)
          return NULL;

        rv = new CIccXformNamedColor(pTag, pProfile->m_Header.pcs, pProfile->m_Header.colorSpace);
      }
      break;

    case icXformLutPreview:
      {
        bInput = false;
        CIccTag *pTag = pProfile->FindTag(icSigPreview0Tag + nTagIntent);
        if (!pTag) {
          pTag = pProfile->FindTag(icSigPreview0Tag);
        }
        if (!pTag) {
          return NULL;
        }
        else {
          switch(pProfile->m_Header.pcs) {
    case icSigXYZData:
    case icSigLabData:
      rv = new CIccXform3DLut(pTag);

    default:
      break;
          }
        }
      }
      break;

    case icXformLutGamut:
      {
        bInput = false;
        CIccTag *pTag = pProfile->FindTag(icSigGamutTag);
        if (!pTag) {
          return NULL;
        }
        else {
          switch(pProfile->m_Header.pcs) {
    case icSigXYZData:
    case icSigLabData:
      rv = new CIccXform3DLut(pTag);

    default:
      break;
          }
        }
      }
      break;
  }

  if (rv) {
    rv->SetParams(pProfile, bInput, nIntent, nInterp);
  }

  return rv;
}


/**
**************************************************************************
* Name: CIccXformMPE::Begin
* 
* Purpose: 
*  This function will be called before the xform is applied.  Derived objects
*  should also call this base class function to initialize for Absolute Colorimetric
*  Intent handling which is performed through the use of the CheckSrcAbs and
*  CheckDstAbs functions.
**************************************************************************
*/
icStatusCMM CIccXformMpe::Begin()
{
  icStatusCMM rv = CIccXform::Begin();

  if (rv != icCmmStatOk)
    return rv;

  if (!m_pTag)
    return icCmmStatInvalidLut;

  if (!m_pTag->Begin()) {
    return icCmmStatInvalidProfile;
  }

  return icCmmStatOk;
}

/**
**************************************************************************
* Name: CIccXformMPE::Apply
* 
* Purpose: 
*  Does the actual application of the Xform.
*  
* Args:
*  DstPixel = Destination pixel where the result is stored,
*  SrcPixel = Source pixel which is to be applied.
**************************************************************************
*/
void CIccXformMpe::Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel)
{
  SrcPixel = CheckSrcAbs(SrcPixel);

  //Since MPE tags use "real" values for PCS we need to convert from 
  //internal encoding used by IccProfLib
  icFloatNumber temp[3];
  switch (GetSrcSpace()) {
    case icSigXYZData:
      memcpy(&temp[0], SrcPixel, 3*sizeof(icFloatNumber));
      icXyzFromPcs(temp);
      SrcPixel = &temp[0];
      break;

    case icSigLabData:
      memcpy(&temp[0], SrcPixel, 3*sizeof(icFloatNumber));
      icLabFromPcs(temp);
      SrcPixel = &temp[0];
      break;
  }

  m_pTag->Apply(DstPixel, SrcPixel);

  //Since MPE tags use "real" values for PCS we need to convert to
  //internal encoding used by IccProfLib
  switch(GetDstSpace()) {
    case icSigXYZData:
      icXyzToPcs(DstPixel);
      break;

    case icSigLabData:
      icLabToPcs(DstPixel);
      break;
  }

  CheckDstAbs(DstPixel);
}


/**
 **************************************************************************
 * Name: CIccCmm::CIccCmm
 * 
 * Purpose: 
 *  Constructor
 *
 * Args:
 *  nSrcSpace = signature of the source color space,
 *  nDestSpace = signature of the destination color space,
 *  bFirstInput = true if the first profile added is an input profile
 **************************************************************************
 */
CIccCmm::CIccCmm(icColorSpaceSignature nSrcSpace /*=icSigUnknownData*/,
                 icColorSpaceSignature nDestSpace /*=icSigUnknownData*/,
                 bool bFirstInput /*=true*/)
{
  m_bValid = false;

  m_bLastInput = !bFirstInput;
  m_nSrcSpace = nSrcSpace;
  m_nDestSpace = nDestSpace;

  m_nLastSpace = nSrcSpace;
  m_nLastIntent = icUnknownIntent;

  m_Xforms = new CIccXformList;
  m_Xforms->clear();

  m_pPCS = NULL;
}

/**
 **************************************************************************
 * Name: CIccCmm::~CIccCmm
 * 
 * Purpose: 
 *  Destructor
 **************************************************************************
 */
CIccCmm::~CIccCmm()
{
  if (m_Xforms) {
    CIccXformList::iterator i;

    for (i=m_Xforms->begin(); i!=m_Xforms->end(); i++) {
      if (i->ptr)
        delete i->ptr;
    }

    delete m_Xforms;
  }

  if (m_pPCS)
    delete m_pPCS;
}

/**
 **************************************************************************
 * Name: CIccCmm::AddXform
 * 
 * Purpose: 
 *  Adds a profile at the end of the Xform list 
 * 
 * Args: 
 *  szProfilePath = file name of the profile to be added,
 *  nIntent = rendering intent to be used with the profile,
 *  nInterp = type of interpolation to be used with the profile,
 *  nLutType = selection of which transform lut to use
 * 
 * Return: 
 *  icCmmStatOk, if the profile was added to the list succesfully
 **************************************************************************
 */
icStatusCMM CIccCmm::AddXform(const icChar *szProfilePath,
                              icRenderingIntent nIntent /*=icUnknownIntent*/,
                              icXformInterp nInterp /*icXformInterp*/,
                              icXformLutType nLutType /*=icXformLutColor*/,
                              bool bUseMpeTags /*=true*/)
{
  CIccProfile *pProfile = OpenIccProfile(szProfilePath);

  if (!pProfile) 
    return icCmmStatCantOpenProfile;

  icStatusCMM rv = AddXform(pProfile, nIntent, nInterp, nLutType, bUseMpeTags);

  if (rv != icCmmStatOk)
    delete pProfile;

  return rv;
}


/**
**************************************************************************
* Name: CIccCmm::AddXform
* 
* Purpose: 
*  Adds a profile at the end of the Xform list 
* 
* Args: 
*  pProfileMem = ptr to profile loaded into memory. Note: this memory
*   needs to be available until after the Begin() function is called.
*  nProfileLen = size in bytes of profile loaded into memory
*  nIntent = rendering intent to be used with the profile,
*  nInterp = type of interpolation to be used with the profile,
*  nLutType = selection of which transform lut to use
* 
* Return: 
*  icCmmStatOk, if the profile was added to the list succesfully
**************************************************************************
*/
icStatusCMM CIccCmm::AddXform(icUInt8Number *pProfileMem,
                              icUInt32Number nProfileLen,
                              icRenderingIntent nIntent /*=icUnknownIntent*/,
                              icXformInterp nInterp /*icXformInterp*/,
                              icXformLutType nLutType /*=icXformLutColor*/,
                              bool bUseMpeTags /*=true*/)
{
  CIccMemIO *pFile = new CIccMemIO;

  if (!pFile || !pFile->Attach(pProfileMem, nProfileLen))
    return icCmmStatCantOpenProfile;

  CIccProfile *pProfile = new CIccProfile;

  if (!pProfile)
    return icCmmStatCantOpenProfile;

  if (!pProfile->Attach(pFile)) {
    delete pFile;
    delete pProfile;
    return icCmmStatCantOpenProfile;
  }

  icStatusCMM rv = AddXform(pProfile, nIntent, nInterp, nLutType, bUseMpeTags);

  if (rv != icCmmStatOk)
    delete pProfile;

  return rv;
}


/**
 **************************************************************************
 * Name: CIccCmm::AddXform
 * 
 * Purpose: 
 *  Adds a profile at the end of the Xform list 
 * 
 * Args: 
 *  pProfile = pointer to the CIccProfile object to be added,
 *  nIntent = rendering intent to be used with the profile,
 *  nInterp = type of interpolation to be used with the profile,
 *  nLutType = selection of which transform lut to use
 * 
 * Return: 
 *  icCmmStatOk, if the profile was added to the list succesfully
 **************************************************************************
 */
icStatusCMM CIccCmm::AddXform(CIccProfile *pProfile,
                              icRenderingIntent nIntent /*=icUnknownIntent*/,
                              icXformInterp nInterp /*=icInterpLinear*/,
                              icXformLutType nLutType /*=icXformLutColor*/,
                              bool bUseMpeTags /*=true*/)
{
  icColorSpaceSignature nSrcSpace, nDstSpace;
  bool bInput = !m_bLastInput;

  if (!pProfile)
    return icCmmStatInvalidProfile;

  switch (nLutType) {
    case icXformLutColor:
    {
      //Check pProfile if nIntent and input can be found.
      if (bInput) {
        nSrcSpace = pProfile->m_Header.colorSpace;
        nDstSpace = pProfile->m_Header.pcs;
      }
      else {
        if (pProfile->m_Header.deviceClass == icSigLinkClass) {
          return icCmmStatBadSpaceLink;
        }
        nSrcSpace = pProfile->m_Header.pcs;
        nDstSpace = pProfile->m_Header.colorSpace;
        if (pProfile->m_Header.deviceClass == icSigAbstractClass) {
          bInput = true;
          nIntent = icPerceptual; // Note: icPerceptualIntent = 0
        }
      }
    }
    break;

    case icXformLutPreview:
      nSrcSpace = pProfile->m_Header.pcs;
      nDstSpace = pProfile->m_Header.pcs;
      bInput = false;
      break;

    case icXformLutGamut:
      nSrcSpace = pProfile->m_Header.pcs;
      nDstSpace = icSigGamutData;
      bInput = true;
      break;

    default:
      return icCmmStatBadLutType;
  }

  //Make sure colorspaces match with previous xforms
  if (!m_Xforms->size()) {
    if (m_nSrcSpace == icSigUnknownData) {
      m_nLastSpace = nSrcSpace;
      m_nSrcSpace = nSrcSpace;
    }
    else if (!IsCompatSpace(m_nSrcSpace, nSrcSpace)) {
      return icCmmStatBadSpaceLink;
    }
  }
  else if (!IsCompatSpace(m_nLastSpace, nSrcSpace)) {
    return icCmmStatBadSpaceLink;
  }

  if (nSrcSpace==icSigNamedData)
    return icCmmStatBadSpaceLink;
  
  //Automatic creation of intent from header/last profile
  if (nIntent==icUnknownIntent) {
    if (bInput) {
      nIntent = (icRenderingIntent)pProfile->m_Header.renderingIntent;
    }
    else {
      nIntent = m_nLastIntent;
    }
    if (nIntent == icUnknownIntent)
      nIntent = icPerceptual;
  }

  CIccXformPtr Xform;
  
  Xform.ptr = CIccXform::Create(pProfile, bInput, nIntent, nInterp, nLutType, bUseMpeTags);

  if (!Xform.ptr) {
    return icCmmStatBadXform;
  }

  m_nLastSpace = nDstSpace;
  m_nLastIntent = nIntent;
  m_bLastInput = bInput;

  m_Xforms->push_back(Xform);

  return icCmmStatOk;
}


/**
 **************************************************************************
 * Name: CIccCmm::AddXform
 * 
 * Purpose: 
 *  Adds a profile at the end of the Xform list 
 * 
 * Args: 
 *  Profile = reference a CIccProfile object that will be copies and added,
 *  nIntent = rendering intent to be used with the profile,
 *  nInterp = type of interpolation to be used with the profile,
 *  nLutType = selection of which transform lut to use
 * 
 * Return: 
 *  icCmmStatOk, if the profile was added to the list succesfully
 **************************************************************************
 */
icStatusCMM CIccCmm::AddXform(CIccProfile &Profile,
                              icRenderingIntent nIntent /*=icUnknownIntent*/,
                              icXformInterp nInterp /*=icInterpLinear*/,
                              icXformLutType nLutType /*=icXformLutColor*/,
                              bool bUseMpeTags /*=true*/)
{
  CIccProfile *pProfile = new CIccProfile(Profile);

  if (!pProfile) 
    return icCmmStatAllocErr;

 icStatusCMM stat = AddXform(pProfile, nIntent, nInterp, nLutType, bUseMpeTags);

  if (stat != icCmmStatOk)
    delete pProfile;

  return stat;
}


/**
 **************************************************************************
 * Name: CIccCmm::Begin
 * 
 * Purpose: 
 *  Does the initialization of the Xforms before Apply() is called.
 *  Must be called before Apply().
 *
 **************************************************************************
 */
icStatusCMM CIccCmm::Begin()
{
  if (m_nDestSpace==icSigUnknownData) {
    m_nDestSpace = m_nLastSpace;
  }
  else if (!IsCompatSpace(m_nDestSpace, m_nLastSpace)) {
    return icCmmStatBadSpaceLink;
  }

  if (m_nDestSpace==icSigNamedData)
    return icCmmStatBadSpaceLink;

  CIccXformList::iterator i;
  icStatusCMM rv;

  for (i=m_Xforms->begin(); i!=m_Xforms->end(); i++) {
    rv = i->ptr->Begin();
    if (rv != icCmmStatOk)
      return rv;
  }

  m_bValid = true;

  if (!m_pPCS)
    m_pPCS = GetPCS();

  if (!m_pPCS)
    return icCmmStatAllocErr;

  return icCmmStatOk;

}

/**
 **************************************************************************
 * Name: CIccCmm::Apply
 * 
 * Purpose: 
 *  Does the actual application of the Xforms in the list.
 *  
 * Args:
 *  DstPixel = Destination pixel where the result is stored,
 *  SrcPixel = Source pixel which is to be applied.
 **************************************************************************
 */
icStatusCMM CIccCmm::Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel)
{
  icFloatNumber Pixel[16], *pDst;
  const icFloatNumber *pSrc;
  CIccXformList::iterator i;
  int j, n = m_Xforms->size();

  if (!n)
    return icCmmStatBadXform;

  m_pPCS->Reset(m_nSrcSpace);

  pSrc = SrcPixel;
  pDst = Pixel;

  if (n>1) {
    for (j=0, i=m_Xforms->begin(); j<n-1 && i!=m_Xforms->end(); i++, j++) {
          
      i->ptr->Apply(pDst, m_pPCS->Check(pSrc, i->ptr));
      pSrc = pDst;
    }

    i->ptr->Apply(DstPixel, m_pPCS->Check(pSrc, i->ptr));
  }
  else if (n==1) {
    i = m_Xforms->begin();
    i->ptr->Apply(DstPixel, m_pPCS->Check(SrcPixel, i->ptr));
  }

  m_pPCS->CheckLast(DstPixel, m_nDestSpace);

  return icCmmStatOk;
}


/**
**************************************************************************
* Name: CIccCmm::RemoveAllIO()
* 
* Purpose: 
*  Remove any attachments to CIccIO objects associated with the profiles
*  related to the transforms attached to the CMM.
*  Must be called after Begin().
*
*  Return:
*   icCmmStatOK - All IO objects removed
*   icCmmStatBadXform - Begin() has not been performed.
**************************************************************************
*/
icStatusCMM CIccCmm::RemoveAllIO()
{
  if (!Valid())
    return icCmmStatBadXform;

  CIccXformList::iterator i;

  for (i=m_Xforms->begin(); i!=m_Xforms->end(); i++) {
    i->ptr->RemoveIO();
  }

  return icCmmStatOk;
}

/**
 *************************************************************************
 ** Name: CIccCmm::IsInGamut
 **
 ** Purpose:
 **  Function to check if internal representation of gamut is in gamut.  Note
 **  since gamut table is 8 bit and a color is considered to be in out of gamut
 **  if the value is not zero.  Then we need to check where the 8 bit representation
 **  of the internal value is not zero.
 **
 **  Args:
 **   pInternal = internal pixel representation of gamut value
 **
 **  Return:
 **    true if in gamut, false if out of gamut
 **************************************************************************/
bool CIccCmm::IsInGamut(icFloatNumber *pInternal)
{
  if (!((unsigned int)((*pInternal)*255.0)))
    return true;
  return false;
}


/**
 **************************************************************************
 * Name: CIccCmm::ToInternalEncoding
 * 
 * Purpose: 
 *  Functions for converting to Internal representation of pixel colors.
 *  
 * Args:
 *  nSpace = color space signature of the data,
 *  nEncode = icFloatColorEncoding type of the data,
 *  pInternal = converted data is stored here,
 *  pData = the data to be converted
 *  bClip = flag to clip to internal range
 **************************************************************************
 */
icStatusCMM CIccCmm::ToInternalEncoding(icColorSpaceSignature nSpace, icFloatColorEncoding nEncode,
                                        icFloatNumber *pInternal, const icFloatNumber *pData,
                                        bool bClip)
{
  int nSamples = icGetSpaceSamples(nSpace);
  if (!nSamples)
    return icCmmStatBadColorEncoding;

  icUInt16Number i;
  icFloatNumber pInput[16];
  memcpy(pInput, pData, nSamples*sizeof(icFloatNumber));
  bool bCLRspace = icIsSpaceCLR(nSpace);

  switch(nSpace) {

    case icSigLabData:
      {
        switch(nEncode) {
        case icEncodeValue:
          {
            icLabToPcs(pInput);
            break;
          }
        case icEncodeFloat:
          {
            break;
          }
        case icEncode8Bit:
          {
            pInput[0] = icU8toF((icUInt8Number)pInput[0]);
            pInput[1] = icU8toF((icUInt8Number)pInput[1]);
            pInput[2] = icU8toF((icUInt8Number)pInput[2]);
            break;
          }
        case icEncode16Bit:
          {
            pInput[0] = icU16toF((icUInt16Number)pInput[0]);
            pInput[1] = icU16toF((icUInt16Number)pInput[1]);
            pInput[2] = icU16toF((icUInt16Number)pInput[2]);
            break;
          }
        default:
            return icCmmStatBadColorEncoding;
            break;
        }
        break;
      }

    case icSigXYZData:
      {
        switch(nEncode) {
        case icEncodeValue:
          {
            pInput[0] = (icFloatNumber)pInput[0];
            pInput[1] = (icFloatNumber)pInput[1];
            pInput[2] = (icFloatNumber)pInput[2];
            icXyzToPcs(pInput);
            break;
          }
        case icEncodePercent:
          {
            pInput[0] = (icFloatNumber)(pInput[0] / 100.0);
            pInput[1] = (icFloatNumber)(pInput[1] / 100.0);
            pInput[2] = (icFloatNumber)(pInput[2] / 100.0);
            icXyzToPcs(pInput);
            break;
          }
        case icEncodeFloat:
          {
            icXyzToPcs(pInput);
            break;
          }
          
        case icEncode16Bit:
          {
            pInput[0] = icUSFtoD((icU1Fixed15Number)pInput[0]);
            pInput[1] = icUSFtoD((icU1Fixed15Number)pInput[1]);
            pInput[2] = icUSFtoD((icU1Fixed15Number)pInput[2]);
            break;
          }
          
        default:
            return icCmmStatBadColorEncoding;
            break;
        }
        break;
      }

    case icSigNamedData:
      return icCmmStatBadColorEncoding;

    default:
      {
        switch(nEncode) {
        case icEncodeValue:
          {
            if (!bCLRspace || nSamples<3) {
              return icCmmStatBadColorEncoding;
            }
            icLabToPcs(pInput);
            break;
          }

        case icEncodePercent:
          {
            if (bClip) {
              for(i=0; i<nSamples; i++) {
                pInput[i] = (icFloatNumber)(pInput[i]/100.0);
                if (pInput[i] < 0.0) pInput[i] = 0.0;
                if (pInput[i] > 1.0) pInput[i] = 1.0;
              }
            }
            else {
              for(i=0; i<nSamples; i++) {
                pInput[i] = (icFloatNumber)(pInput[i]/100.0);
              }
            }
            break;
          }
        
        case icEncodeFloat:
          {
            if (bClip) {
              for(i=0; i<nSamples; i++) {
                if (pInput[i] < 0.0) pInput[i] = 0.0;
                if (pInput[i] > 1.0) pInput[i] = 1.0;
              }
            }
            break;
          }
          
        case icEncode8Bit:
          {
            for(i=0; i<nSamples; i++) {
              pInput[i] = icU8toF((icUInt8Number)pInput[i]);
            }
            break;
          }
          
        case icEncode16Bit:
          {
            for(i=0; i<nSamples; i++) {
              pInput[i] = icU16toF((icUInt16Number)pInput[i]);
            }
            break;
          }
        
        default:
            return icCmmStatBadColorEncoding;
            break;
        }
        break;
      }
  }

  memcpy(pInternal, pInput, nSamples*sizeof(icFloatNumber));
  return icCmmStatOk;
}

icStatusCMM CIccCmm::ToInternalEncoding(icColorSpaceSignature nSpace, icFloatNumber *pInternal,
                                        const icUInt8Number *pData)
{
  switch(nSpace) {
    case icSigRgbData:
    {
      pInternal[0] = (icFloatNumber)((icFloatNumber)pData[0] / 255.0);
      pInternal[1] = (icFloatNumber)((icFloatNumber)pData[1] / 255.0);
      pInternal[2] = (icFloatNumber)((icFloatNumber)pData[2] / 255.0);

      return icCmmStatOk;
    }
    case icSigCmykData:
    {
      pInternal[0] = (icFloatNumber)((icFloatNumber)pData[0] / 255.0);
      pInternal[1] = (icFloatNumber)((icFloatNumber)pData[1] / 255.0);
      pInternal[2] = (icFloatNumber)((icFloatNumber)pData[2] / 255.0);
      pInternal[3] = (icFloatNumber)((icFloatNumber)pData[3] / 255.0);
      return icCmmStatOk;
    }
    default:
    {
      icFloatNumber FloatPixel[16];
      icUInt32Number i;
      icUInt32Number nSamples = icGetSpaceSamples(nSpace);
      for(i=0; i<nSamples; i++) {
        FloatPixel[i] = (icFloatNumber)pData[i];    
      }
      return ToInternalEncoding(nSpace, icEncode8Bit, pInternal, FloatPixel);
    }
  }

}

icStatusCMM CIccCmm::ToInternalEncoding(icColorSpaceSignature nSpace, icFloatNumber *pInternal,
                                        const icUInt16Number *pData)
{
  switch(nSpace) {
    case icSigRgbData:
    {
      pInternal[0] = (icFloatNumber)((icFloatNumber)pData[0] / 65535.0);
      pInternal[1] = (icFloatNumber)((icFloatNumber)pData[1] / 65535.0);
      pInternal[2] = (icFloatNumber)((icFloatNumber)pData[2] / 65535.0);

      return icCmmStatOk;
    }
    case icSigCmykData:
    {
      pInternal[0] = (icFloatNumber)((icFloatNumber)pData[0] / 65535.0);
      pInternal[1] = (icFloatNumber)((icFloatNumber)pData[1] / 65535.0);
      pInternal[2] = (icFloatNumber)((icFloatNumber)pData[2] / 65535.0);
      pInternal[3] = (icFloatNumber)((icFloatNumber)pData[3] / 65535.0);
      return icCmmStatOk;
    }
    default:
    {
      icUInt32Number i;
      icUInt32Number nSamples = icGetSpaceSamples(nSpace);
      icFloatNumber pFloatPixel[16];
      for(i=0; i<nSamples; i++) {
        pFloatPixel[i] = (icFloatNumber)pData[i];    
      }
      return ToInternalEncoding(nSpace, icEncode16Bit, pInternal, pFloatPixel);
    }
  }
}

/**
 **************************************************************************
 * Name: CIccCmm::FromInternalEncoding
 * 
 * Purpose: 
 *  Functions for converting from Internal representation of pixel colors.
 *  
 * Args:
 *  nSpace = color space signature of the data,
 *  nEncode = icFloatColorEncoding type of the data,
 *  pData = converted data is stored here,
 *  pInternal = the data to be converted
 *  bClip = flag to clip data to internal range
 **************************************************************************
 */
icStatusCMM CIccCmm::FromInternalEncoding(icColorSpaceSignature nSpace, icFloatColorEncoding nEncode,
                                          icFloatNumber *pData, const icFloatNumber *pInternal, bool bClip)
{
  int nSamples = icGetSpaceSamples(nSpace);
  if (!nSamples)
    return icCmmStatBadColorEncoding;

  icUInt16Number i;
  icFloatNumber pInput[16];
  memcpy(pInput, pInternal, nSamples*sizeof(icFloatNumber));
  bool bCLRspace = icIsSpaceCLR(nSpace);

  switch(nSpace) {

    case icSigLabData:
      {
        switch(nEncode) {
        case icEncodeValue:
          {
            icLabFromPcs(pInput);
            break;
          }
        case icEncodeFloat:
          {
            break;
          }
        case icEncode8Bit:
          {
            pInput[0] = icFtoU8(pInput[0]);
            pInput[1] = icFtoU8(pInput[1]);
            pInput[2] = icFtoU8(pInput[2]);
            break;
          }
        case icEncode16Bit:
          {
            pInput[0] = icFtoU16(pInput[0]);
            pInput[1] = icFtoU16(pInput[1]);
            pInput[2] = icFtoU16(pInput[2]);
            break;
          }
        default:
            return icCmmStatBadColorEncoding;
            break;
        }
        break;
      }

    case icSigXYZData:
      {
        switch(nEncode) {
        case icEncodeValue:
          {
            icXyzFromPcs(pInput);
            break;
          }
        case icEncodePercent:
          {
            icXyzFromPcs(pInput);
            pInput[0] = (icFloatNumber)(pInput[0] * 100.0);
            pInput[1] = (icFloatNumber)(pInput[1] * 100.0);
            pInput[2] = (icFloatNumber)(pInput[2] * 100.0);            
            break;
          }
        case icEncodeFloat:
          {
            icXyzFromPcs(pInput);
            break;
          }
          
        case icEncode16Bit:
          {
            pInput[0] = icDtoUSF(pInput[0]);
            pInput[1] = icDtoUSF(pInput[1]);
            pInput[2] = icDtoUSF(pInput[2]);
            break;
          }
          
        default:
            return icCmmStatBadColorEncoding;
            break;
        }
        break;
      }

    case icSigNamedData:
      return icCmmStatBadColorEncoding;

    default:
      {
        switch(nEncode) {
        case icEncodeValue:
          {
            if (!bCLRspace || nSamples<3) {
              return icCmmStatBadColorEncoding;
            }
            icLabFromPcs(pInput);
            break;
          }
        case icEncodePercent:
          {
            if (bClip) {
              for(i=0; i<nSamples; i++) {
                if (pInput[i] < 0.0) pInput[i] = 0.0;
                if (pInput[i] > 1.0) pInput[i] = 1.0;
                pInput[i] = (icFloatNumber)(pInput[i]*100.0);
              }
            }
            else {
              for(i=0; i<nSamples; i++) {
                pInput[i] = (icFloatNumber)(pInput[i]*100.0);
              }
            }
            break;
          }
        
        case icEncodeFloat:
          {
            if (bClip) {
              for(i=0; i<nSamples; i++) {
                if (pInput[i] < 0.0) pInput[i] = 0.0;
                if (pInput[i] > 1.0) pInput[i] = 1.0;
              }
            }
            break;
          }
          
        case icEncode8Bit:
          {
            for(i=0; i<nSamples; i++) {
              pInput[i] = icFtoU8(pInput[i]);
            }
            break;
          }
          
        case icEncode16Bit:
          {
            for(i=0; i<nSamples; i++) {
              pInput[i] = icFtoU16(pInput[i]);
            }
            break;
          }
        
        default:
            return icCmmStatBadColorEncoding;
            break;
        }
        break;
      }
  }

  memcpy(pData, pInput, nSamples*sizeof(icFloatNumber));
  return icCmmStatOk;
}

icStatusCMM CIccCmm::FromInternalEncoding(icColorSpaceSignature nSpace, icUInt8Number *pData,
                                          const icFloatNumber *pInternal)
{
  switch(nSpace) {
    case icSigRgbData:
    {
      pData[0] = icFtoU8(pInternal[0]);
      pData[1] = icFtoU8(pInternal[1]);
      pData[2] = icFtoU8(pInternal[2]);

      return icCmmStatOk;
    }
    case icSigCmykData:
    {
      pData[0] = icFtoU8(pInternal[0]);
      pData[1] = icFtoU8(pInternal[1]);
      pData[2] = icFtoU8(pInternal[2]);
      pData[3] = icFtoU8(pInternal[3]);

      return icCmmStatOk;
    }
    default:
    {
      icUInt32Number i;
      icUInt32Number nSamples = icGetSpaceSamples(nSpace);
      icFloatNumber pFloatPixel[16];
      icStatusCMM convertStat;
      convertStat = FromInternalEncoding(nSpace, icEncode8Bit, pFloatPixel, pInternal);
      if (convertStat)
        return convertStat;
      for(i=0; i<nSamples; i++) {
        pData[i] = (icUInt8Number)(pFloatPixel[i] + 0.5);    
      }

      return icCmmStatOk;
    }
  }
}

icStatusCMM CIccCmm::FromInternalEncoding(icColorSpaceSignature nSpace, icUInt16Number *pData,
                                          const icFloatNumber *pInternal)
{
  switch(nSpace) {
    case icSigRgbData:
    {
      pData[0] = icFtoU16(pInternal[0]);
      pData[1] = icFtoU16(pInternal[1]);
      pData[2] = icFtoU16(pInternal[2]);

      return icCmmStatOk;
    }
    case icSigCmykData:
    {
      pData[0] = icFtoU16(pInternal[0]);
      pData[1] = icFtoU16(pInternal[1]);
      pData[2] = icFtoU16(pInternal[2]);
      pData[3] = icFtoU16(pInternal[3]);

      return icCmmStatOk;
    }
    default:
    {
      icUInt32Number i;
      icUInt32Number nSamples = icGetSpaceSamples(nSpace);
      icFloatNumber pFloatPixel[16];
      icStatusCMM convertStat;
      convertStat = FromInternalEncoding(nSpace, icEncode16Bit, pFloatPixel, pInternal);
      if (convertStat)
        return convertStat;
      for(i=0; i<nSamples; i++) {
        pData[i] = (icUInt16Number)(pFloatPixel[i] + 0.5);    
      }

      return icCmmStatOk;
    }
  }
}

/**
 **************************************************************************
 * Name: CIccCmm::GetFloatColorEncoding
 * 
 * Purpose: 
 *  Converts the encoding type to characters for printing
 *  
 * Args:
 *  val = encoding type
 * 
 * Return:
 *  characters for printing
 **************************************************************************
 */
const icChar* CIccCmm::GetFloatColorEncoding(icFloatColorEncoding val)
{
  switch(val) {

    case icEncodeValue:
      return "icEncodeValue";

    case icEncodeFloat:
      return "icEncodeFloat";

    case icEncodePercent:
      return "icEncodePercent";

    case icEncode8Bit:
      return "icEncode8Bit";

    case icEncode16Bit:
      return "icEncode16Bit";

    default:
      return "icEncodeUnknown";
  }
}

/**
 **************************************************************************
 * Name: CIccCmm::GetFloatColorEncoding
 * 
 * Purpose: 
 *  Converts the string containing encoding type to icFloatColorEncoding
 *  
 * Args:
 *  val = string containing encoding type
 * 
 * Return:
 *  encoding type
 **************************************************************************
 */
icFloatColorEncoding CIccCmm::GetFloatColorEncoding(const icChar* val)
{
  if (!stricmp(val, "icEncodePercent")) {
    return icEncodePercent;
  }
  else if (!stricmp(val, "icEncodeFloat")) {
    return icEncodeFloat;
  }
  else if (!stricmp(val, "icEncode8Bit")) {
    return icEncode8Bit;
  }
  else if (!stricmp(val, "icEncode16Bit")) {
    return icEncode16Bit;
  }
  else if (!stricmp(val, "icEncodeValue")) {
    return icEncodeValue;
  }
  else {
    return icEncodeUnknown;
  }

}

/**
 **************************************************************************
 * Name: CIccCmm::GetNumXforms
 * 
 * Purpose: 
 *  Get number of xforms in the xform list
 *  
 * Return:
 * number of m_Xforms
 **************************************************************************
 */
icUInt32Number CIccCmm::GetNumXforms() const
{
  return m_Xforms->size();
}


/**
**************************************************************************
* Name: CIccCmm::GetFirstXformSource
* 
* Purpose: 
*  Get source colorspace of first transform (similar to m_nSrcSpace with differences in dev colorimetric spaces)
*  
* Return:
* colorspace
**************************************************************************
*/
icColorSpaceSignature CIccCmm::GetFirstXformSource()
{
  if (!m_Xforms->size())
    return m_nSrcSpace;

  return m_Xforms->begin()->ptr->GetSrcSpace();
}

/**
**************************************************************************
* Name: CIccCmm::GetNumXforms
* 
* Purpose: 
*  Get source colorspace of last transform (similar to m_nSrcSpace with differences in dev colorimetric spaces)
*  
* Return:
* colorspace
**************************************************************************
*/
icColorSpaceSignature CIccCmm::GetLastXformDest()
{
  if (!m_Xforms->size())
    return m_nDestSpace;

  return m_Xforms->rbegin()->ptr->GetDstSpace();
}

/**
 **************************************************************************
 * Name: CIccNamedColorCmm::CIccNamedColorCmm
 * 
 * Purpose: 
 *  Constructor
 *
 * Args:
 *  nSrcSpace = signature of the source color space,
 *  nDestSpace = signature of the destination color space,
 *  bFirstInput = true if the first profile added is an input profile
 **************************************************************************
 */
CIccNamedColorCmm::CIccNamedColorCmm(icColorSpaceSignature nSrcSpace, icColorSpaceSignature nDestSpace,
                                     bool bFirstInput) : CIccCmm(nSrcSpace, nDestSpace, bFirstInput)
{
  m_nApplyInterface = icApplyPixel2Pixel;
}

/**
 **************************************************************************
 * Name: CIccNamedColorCmm::~CIccNamedColorCmm
 * 
 * Purpose: 
 *  Destructor
 **************************************************************************
 */
CIccNamedColorCmm::~CIccNamedColorCmm()
{
}


/**
 **************************************************************************
 * Name: CIccNamedColorCmm::AddXform
 * 
 * Purpose: 
 *  Adds a profile at the end of the Xform list 
 * 
 * Args: 
 *  szProfilePath = file name of the profile to be added,
 *  nIntent = rendering intent to be used with the profile,
 *  nInterp = type of interpolation to be used with the profile
 * 
 * Return: 
 *  icCmmStatOk, if the profile was added to the list succesfully
 **************************************************************************
 */
icStatusCMM CIccNamedColorCmm::AddXform(const icChar *szProfilePath,
                                        icRenderingIntent nIntent /*=icUnknownIntent*/,
                                        icXformInterp nInterp /*icXformInterp*/,
                                        icXformLutType nLutType /*=icXformLutColor*/,
                                        bool bUseMpeTags /*=true*/)
{
  CIccProfile *pProfile = OpenIccProfile(szProfilePath);

  if (!pProfile) 
    return icCmmStatCantOpenProfile;

  icStatusCMM rv = AddXform(pProfile, nIntent, nInterp, nLutType, bUseMpeTags);

  if (rv != icCmmStatOk)
    delete pProfile;

  return rv;
}

/**
 **************************************************************************
 * Name: CIccNamedColorCmm::AddXform
 * 
 * Purpose: 
 *  Adds a profile at the end of the Xform list 
 * 
 * Args: 
 *  pProfile = pointer to the CIccProfile object to be added,
 *  nIntent = rendering intent to be used with the profile,
 *  nInterp = type of interpolation to be used with the profile
 *  nLutType = type of lut to use from the profile
 * 
 * Return: 
 *  icCmmStatOk, if the profile was added to the list succesfully
 **************************************************************************
 */
icStatusCMM CIccNamedColorCmm::AddXform(CIccProfile *pProfile,
                                        icRenderingIntent nIntent /*=icUnknownIntent*/,
                                        icXformInterp nInterp /*=icInterpLinear*/,
                                        icXformLutType nLutType /*=icXformLutColor*/,
                                        bool bUseMpeTags /*=true*/)
{
  icColorSpaceSignature nSrcSpace, nDstSpace;
  CIccXformPtr Xform;
  bool bInput = !m_bLastInput;
  icStatusCMM rv;

  Xform.ptr = NULL;
  switch (nLutType) {
    //Automatically choose which one
    case icXformLutColor:
    case icXformLutNamedColor:
    {
      CIccTagNamedColor2 *pTag = (CIccTagNamedColor2*)pProfile->FindTag(icSigNamedColor2Tag);

      if (pTag) {
        if (bInput) {
          nSrcSpace = icSigNamedData;
        }
        else {
          nSrcSpace = pProfile->m_Header.pcs;
        }

        if (!m_Xforms->size()) {
          if (m_nSrcSpace==icSigUnknownData) {
            m_nSrcSpace = nSrcSpace;
          }
          else {
            nSrcSpace = m_nSrcSpace;
          }
        }
        else {
          if (m_nLastSpace==icSigUnknownData) {
            m_nLastSpace = nSrcSpace;
          }
          else {
            nSrcSpace = m_nLastSpace;
          }
        }

        if (nSrcSpace==icSigNamedData) {
          nDstSpace = pProfile->m_Header.pcs;
          bInput = true;
        }
        else {
          nDstSpace = icSigNamedData;
          bInput = false;
        }

        Xform.ptr = CIccXform::Create(pProfile, bInput, nIntent, nInterp, icXformLutNamedColor, bUseMpeTags);
        if (!Xform.ptr) {
          return icCmmStatBadXform;
        }
        CIccXformNamedColor *pXform = (CIccXformNamedColor *)Xform.ptr;
        rv = pXform->SetSrcSpace(nSrcSpace);
        if (rv)
          return rv;

        rv = pXform->SetDestSpace(nDstSpace);
        if (rv)
          return rv;
      }
      else {
        //It isn't named color so make we will use color lut.
        nLutType = icXformLutColor;

        //Check pProfile if nIntent and input can be found.
        if (bInput) {
          nSrcSpace = pProfile->m_Header.colorSpace;
          nDstSpace = pProfile->m_Header.pcs;
        }
        else {
          if (pProfile->m_Header.deviceClass == icSigLinkClass) {
            return icCmmStatBadSpaceLink;
          }
          if (pProfile->m_Header.deviceClass == icSigAbstractClass) {
            bInput = true;
            nIntent = icPerceptual; // Note: icPerceptualIntent = 0
          }
          nSrcSpace = pProfile->m_Header.pcs;
          nDstSpace = pProfile->m_Header.colorSpace;
        }
      }
    }
    break;

    case icXformLutPreview:
      nSrcSpace = pProfile->m_Header.pcs;
      nDstSpace = pProfile->m_Header.pcs;
      bInput = false;
      break;

    case icXformLutGamut:
      nSrcSpace = pProfile->m_Header.pcs;
      nDstSpace = icSigGamutData;
      bInput = true;
      break;

    default:
      return icCmmStatBadLutType;
  }

  //Make sure color spaces match with previous xforms
  if (!m_Xforms->size()) {
    if (m_nSrcSpace == icSigUnknownData) {
      m_nLastSpace = nSrcSpace;
      m_nSrcSpace = nSrcSpace;
    }
    else if (!IsCompatSpace(m_nSrcSpace, nSrcSpace)) {
      return icCmmStatBadSpaceLink;
    }
  }
  else if (!IsCompatSpace(m_nLastSpace, nSrcSpace))  {
      return icCmmStatBadSpaceLink;
  }

  //Automatic creation of intent from header/last profile
  if (nIntent==icUnknownIntent) {
    if (bInput) {
      nIntent = (icRenderingIntent)pProfile->m_Header.renderingIntent;
    }
    else {
      nIntent = m_nLastIntent;
    }
    if (nIntent == icUnknownIntent)
      nIntent = icPerceptual;
  }

  if (!Xform.ptr)
    Xform.ptr = CIccXform::Create(pProfile, bInput, nIntent, nInterp, nLutType, bUseMpeTags);

  if (!Xform.ptr) {
    return icCmmStatBadXform;
  }

  m_nLastSpace = nDstSpace;
  m_nLastIntent = nIntent;
  m_bLastInput = bInput;

  m_Xforms->push_back(Xform);

  return icCmmStatOk;
}

/**
 **************************************************************************
 * Name: CIccNamedColorCmm::Begin
 * 
 * Purpose: 
 *  Does the initialization of the Xforms in the list before Apply() is called.
 *  Must be called before Apply().
 *
 **************************************************************************
 */
icStatusCMM CIccNamedColorCmm::Begin()
{
  icStatusCMM rv;

  if (m_nDestSpace==icSigUnknownData) {
    m_nDestSpace = m_nLastSpace;
  }
  else if (!IsCompatSpace(m_nDestSpace, m_nLastSpace)) {
    return icCmmStatBadSpaceLink;
  }

  if (m_nSrcSpace != icSigNamedData) {
    if (m_nDestSpace != icSigNamedData) {
      m_nApplyInterface = icApplyPixel2Pixel;
    }
    else {
      m_nApplyInterface = icApplyPixel2Named;
    }
  }
  else {
    if (m_nDestSpace != icSigNamedData) {
      m_nApplyInterface = icApplyNamed2Pixel;
    }
    else {
      m_nApplyInterface = icApplyNamed2Named;
    }
  }

  CIccXformList::iterator i;

  for (i=m_Xforms->begin(); i!=m_Xforms->end(); i++) {
    rv = i->ptr->Begin();
    if (rv != icCmmStatOk)
      return rv;
  }

  if (!m_pPCS)
    m_pPCS = GetPCS();

  if (!m_pPCS)
    return icCmmStatAllocErr;
 
  m_bValid = true;

  return icCmmStatOk;
}

/**
 **************************************************************************
 * Name: CIccNamedColorCmm::Apply
 * 
 * Purpose: 
 *  Does the actual application of the Xforms in the list.
 *  
 * Args:
 *  DstPixel = Destination pixel where the result is stored,
 *  SrcPixel = Source pixel which is to be applied.
 **************************************************************************
 */
icStatusCMM CIccNamedColorCmm::Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel)
{
  icFloatNumber Pixel[16], *pDst;
  const icFloatNumber *pSrc;
  CIccXformList::iterator i;
  int j, n = m_Xforms->size();

  if (!n)
    return icCmmStatBadXform;
  
  icChar NamedColor[256];
  icStatusCMM rv;

  m_pPCS->Reset(m_nSrcSpace);

  pSrc = SrcPixel;
  pDst = Pixel;

  if (n>1) {
    for (j=0, i=m_Xforms->begin(); j<n-1 && i!=m_Xforms->end(); i++, j++) {

      if (i->ptr->GetXformType()==icXformTypeNamedColor) {
        CIccXformNamedColor *pXform = (CIccXformNamedColor*)i->ptr;
        switch(pXform->GetInterface()) {
        case icApplyPixel2Pixel:
          pXform->Apply(pDst, m_pPCS->Check(pSrc, pXform));
          break;

        case icApplyPixel2Named:
          pXform->Apply(NamedColor, m_pPCS->Check(pSrc, pXform));
          break;
        
        case icApplyNamed2Pixel:
          if (j==0) {
            return icCmmStatIncorrectApply;
          }

          rv = pXform->Apply(pDst, NamedColor);

          if (rv) {
            return rv;
          }
          break;

        }
      }
      else {
        i->ptr->Apply(pDst, m_pPCS->Check(pSrc, i->ptr));
      }
      pSrc = pDst;
    }

    if (i->ptr->GetXformType()==icXformTypeNamedColor) {
      CIccXformNamedColor *pXform = (CIccXformNamedColor*)i->ptr;
      switch(pXform->GetInterface()) {
      case icApplyPixel2Pixel:
        pXform->Apply(DstPixel, m_pPCS->Check(pSrc, pXform));
        break;

      case icApplyPixel2Named:
      default:
        return icCmmStatIncorrectApply;
        break;
      
      case icApplyNamed2Pixel:
        rv = pXform->Apply(DstPixel, NamedColor);
        if (rv) {
          return rv;
        }
        break;

      }
    }
    else {
      i->ptr->Apply(DstPixel, m_pPCS->Check(pSrc, i->ptr));
    }

  }
  else if (n==1) {
    i = m_Xforms->begin();
    
    if (i->ptr->GetXformType()==icXformTypeNamedColor) {
      return icCmmStatIncorrectApply;
    }

    i->ptr->Apply(DstPixel, m_pPCS->Check(pSrc, i->ptr));
  }

  m_pPCS->CheckLast(DstPixel, m_nDestSpace);

  return icCmmStatOk;
}

/**
 **************************************************************************
 * Name: CIccNamedColorCmm::Apply
 * 
 * Purpose: 
 *  Does the actual application of the Xforms in the list.
 *  
 * Args:
 *  DstColorName = Destination string where the result is stored,
 *  SrcPixel = Source pixel which is to be applied.
 **************************************************************************
 */
icStatusCMM CIccNamedColorCmm::Apply(icChar* DstColorName, const icFloatNumber *SrcPixel)
{
  icFloatNumber Pixel[16], *pDst;
  const icFloatNumber *pSrc;
  CIccXformList::iterator i;
  int j, n = m_Xforms->size();

  if (!n)
    return icCmmStatBadXform;
  
  icChar NamedColor[256];
  icStatusCMM rv;

  m_pPCS->Reset(m_nSrcSpace);

  pSrc = SrcPixel;
  pDst = Pixel;

  if (n>1) {
    for (j=0, i=m_Xforms->begin(); j<n-1 && i!=m_Xforms->end(); i++, j++) {

      if (i->ptr->GetXformType()==icXformTypeNamedColor) {
        CIccXformNamedColor *pXform = (CIccXformNamedColor*)i->ptr;
        switch(pXform->GetInterface()) {
        case icApplyPixel2Pixel:
          pXform->Apply(pDst, m_pPCS->Check(pSrc, pXform));
          break;

        case icApplyPixel2Named:
          pXform->Apply(NamedColor, m_pPCS->Check(pSrc, pXform));
          break;
        
        case icApplyNamed2Pixel:
          if (j==0) {
            return icCmmStatIncorrectApply;
          }
          rv = pXform->Apply(pDst, NamedColor);
          if (rv) {
            return rv;
          }
          break;

        }
      }
      else {
        i->ptr->Apply(pDst, m_pPCS->Check(pSrc, i->ptr));
      }
      pSrc = pDst;
    }

    if (i->ptr->GetXformType()==icXformTypeNamedColor) {
      CIccXformNamedColor *pXform = (CIccXformNamedColor*)i->ptr;
      switch(pXform->GetInterface()) {

      case icApplyPixel2Named:
        pXform->Apply(DstColorName, m_pPCS->Check(pSrc, pXform));
        break;
      
      case icApplyPixel2Pixel:
      case icApplyNamed2Pixel:
      default:
        return icCmmStatIncorrectApply;
        break;

      }
    }
    else {
      return icCmmStatIncorrectApply;
    }

  }
  else if (n==1) {
    i = m_Xforms->begin();
    if (i->ptr->GetXformType()!=icXformTypeNamedColor) {
      return icCmmStatIncorrectApply;
    }

    CIccXformNamedColor *pXform = (CIccXformNamedColor*)i->ptr;
    pXform->Apply(DstColorName, m_pPCS->Check(pSrc, pXform));
  }

  return icCmmStatOk;
}


/**
 **************************************************************************
 * Name: CIccNamedColorCmm::Apply
 * 
 * Purpose: 
 *  Does the actual application of the Xforms in the list.
 *  
 * Args:
 *  DstPixel = Destination pixel where the result is stored,
 *  SrcColorName = Source color name which is to be searched.
 **************************************************************************
 */
icStatusCMM CIccNamedColorCmm::Apply(icFloatNumber *DstPixel, const icChar *SrcColorName)
{
  icFloatNumber Pixel[16], *pDst;
  const icFloatNumber *pSrc;
  CIccXformList::iterator i;
  int j, n = m_Xforms->size();

  if (!n)
    return icCmmStatBadXform;

  icChar NamedColor[256];
  icStatusCMM rv;

  i=m_Xforms->begin();
  if (i->ptr->GetXformType()!=icXformTypeNamedColor)
    return icCmmStatIncorrectApply;

  CIccXformNamedColor *pXform = (CIccXformNamedColor*)i->ptr;  
  m_pPCS->Reset(pXform->GetSrcSpace(), pXform->UseLegacyPCS());

  pDst = Pixel;

  if (n>1) {
    rv = pXform->Apply(pDst, SrcColorName);
    if (rv) {
      return rv;
    }

    pSrc = pDst;

    for (j=0, i++; j<n-2 && i!=m_Xforms->end(); i++, j++) {

      if (i->ptr->GetXformType()==icXformTypeNamedColor) {
        CIccXformNamedColor *pXform = (CIccXformNamedColor*)i->ptr;
        switch(pXform->GetInterface()) {
        case icApplyPixel2Pixel:
          pXform->Apply(pDst, m_pPCS->Check(pSrc, pXform));
          break;

        case icApplyPixel2Named:
          pXform->Apply(NamedColor, m_pPCS->Check(pSrc, pXform));
          break;
        
        case icApplyNamed2Pixel:
          rv = pXform->Apply(pDst, NamedColor);
          if (rv) {
            return rv;
          }
          break;

        }
      }
      else {
        i->ptr->Apply(pDst, m_pPCS->Check(pSrc, i->ptr));
      }
      pSrc = pDst;
    }

    if (i->ptr->GetXformType()==icXformTypeNamedColor) {
      CIccXformNamedColor *pXform = (CIccXformNamedColor*)i->ptr;
      switch(pXform->GetInterface()) {
      case icApplyPixel2Pixel:
        pXform->Apply(DstPixel, m_pPCS->Check(pSrc, pXform));
        break;

      case icApplyPixel2Named:
      default:
        return icCmmStatIncorrectApply;
        break;
      
      case icApplyNamed2Pixel:
        rv = pXform->Apply(DstPixel, NamedColor);
        if (rv) {
          return rv;
        }
        break;

      }
    }
    else {
      i->ptr->Apply(DstPixel, m_pPCS->Check(pSrc, i->ptr));
    }

  }
  else if (n==1) {
    rv = pXform->Apply(DstPixel, SrcColorName);
    if (rv) {
      return rv;
    }
    m_pPCS->Check(DstPixel, pXform);
  }

  m_pPCS->CheckLast(DstPixel, m_nDestSpace);

  return icCmmStatOk;
}

/**
 **************************************************************************
 * Name: CIccNamedColorCmm::Apply
 * 
 * Purpose: 
 *  Does the actual application of the Xforms in the list.
 *  
 * Args:
 *  DstColorName = Destination string where the result is stored, 
 *  SrcColorName = Source color name which is to be searched.
 **************************************************************************
 */
icStatusCMM CIccNamedColorCmm::Apply(icChar *DstColorName, const icChar *SrcColorName)
{
  icFloatNumber Pixel[16], *pDst;
  const icFloatNumber *pSrc;
  CIccXformList::iterator i;
  int j, n = m_Xforms->size();
  icChar NamedColor[256];
  icStatusCMM rv;

  if (!n)
    return icCmmStatBadXform;

  i=m_Xforms->begin();

  if (i->ptr->GetXformType()!=icXformTypeNamedColor)
    return icCmmStatIncorrectApply;

  CIccXformNamedColor *pXform = (CIccXformNamedColor*)i->ptr;

  m_pPCS->Reset(pXform->GetSrcSpace(), pXform->UseLegacyPCS());

  pDst = Pixel;

  if (n>1) {
    rv = pXform->Apply(pDst, SrcColorName);

    if (rv) {
      return rv;
    }

    pSrc = pDst;

    for (j=0, i++; j<n-2 && i!=m_Xforms->end(); i++, j++) {

      if (i->ptr->GetXformType()==icXformTypeNamedColor) {
        pXform = (CIccXformNamedColor*)i->ptr;
        switch(pXform->GetInterface()) {
        case icApplyPixel2Pixel:
          pXform->Apply(pDst, m_pPCS->Check(pSrc, pXform));
          break;


        case icApplyPixel2Named:
          pXform->Apply(NamedColor, m_pPCS->Check(pSrc, pXform));
          break;
        
        case icApplyNamed2Pixel:
          rv = pXform->Apply(pDst, NamedColor);
          if (rv) {
            return rv;
          }
          break;

        }
      }
      else {
        i->ptr->Apply(pDst, m_pPCS->Check(pSrc, i->ptr));
      }
      pSrc = pDst;
    }

    if (i->ptr->GetXformType()==icXformTypeNamedColor) {
      pXform = (CIccXformNamedColor*)i->ptr;
      switch(pXform->GetInterface()) {
      case icApplyPixel2Named:
        pXform->Apply(DstColorName, m_pPCS->Check(pSrc, i->ptr));
        break;
      
      case icApplyPixel2Pixel:
      case icApplyNamed2Pixel:
      default:
        return icCmmStatIncorrectApply;
        break;

      }
    }
    else {
      return icCmmStatIncorrectApply;
    }

  }
  else if (n==1) {
    return icCmmStatIncorrectApply;
  }

  return icCmmStatOk;
}


/**
 **************************************************************************
 * Name: CIccNamedColorCmm::SetLastXformDest
 * 
 * Purpose: 
 *  Sets the destination Color space of the last Xform in the list
 * 
 * Args: 
 *  nDestSpace = signature of the color space to be set
 **************************************************************************
 */
icStatusCMM CIccNamedColorCmm::SetLastXformDest(icColorSpaceSignature nDestSpace)
{
  int n = m_Xforms->size();
  CIccXformPtr *pLastXform;

  if (!n)
    return icCmmStatBadXform;

  pLastXform = &m_Xforms->back();
  
  if (pLastXform->ptr->GetXformType()==icXformTypeNamedColor) {
    CIccXformNamedColor *pXform = (CIccXformNamedColor *)pLastXform->ptr;
    if (pXform->GetSrcSpace() == icSigNamedData &&
        nDestSpace == icSigNamedData) {
      return icCmmStatBadSpaceLink;
    }

    if (nDestSpace != icSigNamedData &&
        pXform->GetDstSpace() == icSigNamedData) {
      return icCmmStatBadSpaceLink;
    }
    
    return pXform->SetDestSpace(nDestSpace);
  }

  return icCmmStatBadXform;
}

/**
****************************************************************************
* Name: CIccMruCmm::CIccMruCmm
* 
* Purpose: private constructor - Use Attach to create CIccMruCmm objects
*****************************************************************************
*/
CIccMruCmm::CIccMruCmm()
{
  m_pCmm = NULL;
}


/**
****************************************************************************
* Name: CIccMruCmm::~CIccMruCmm
* 
* Purpose: destructor
*****************************************************************************
*/
CIccMruCmm::~CIccMruCmm()
{
   if (m_pCmm)
     delete m_pCmm;

   if (m_cache)
     delete [] m_cache;

   if (m_pixelData)
     free(m_pixelData);
}


/**
****************************************************************************
* Name: CIccMruCmm::Attach
* 
* Purpose: Create a Cmm decorator object that implements a cache of most
*  recently used pixel transformations.
* 
* Args:
*  pCmm - pointer to cmm object that we are attaching to.
*  nCacheSize - number of most recently used transformations to cache
*
* Return:
*  A CIccMruCmm object that represents a cached form of the pCmm passed in.
*  The pCmm will be owned by the returned object.
*
*  If this function fails the pCmm object will be deleted.
*****************************************************************************
*/
CIccMruCmm* CIccMruCmm::Attach(CIccCmm *pCmm, icUInt8Number nCacheSize/* =4 */)
{
  if (!pCmm || !nCacheSize)
    return NULL;

  if (!pCmm->Valid()) {
    delete pCmm;
    return NULL;
  }

  CIccMruCmm *rv = new CIccMruCmm();

  if (!rv->Init(pCmm, nCacheSize)) {
    delete rv;
    return NULL;
  }

  return rv;
}

/**
****************************************************************************
* Name: CIccMruCmm::Init
* 
* Purpose: Initialize the object and set up the cache
* 
* Args:
*  pCmm - pointer to cmm object that we are attaching to.
*  nCacheSize - number of most recently used transformations to cache
*
* Return:
*  true if successful
*****************************************************************************
*/
bool CIccMruCmm::Init(CIccCmm *pCmm, icUInt8Number nCacheSize)
{
  m_pCmm = pCmm;

  m_bValid = true;

  m_nSrcSpace = pCmm->GetSourceSpace();
  m_nDestSpace = pCmm->GetDestSpace();

  m_nLastIntent = pCmm->GetLastIntent();

  m_nSrcSamples = GetSourceSamples();
  m_nSrcSize = m_nSrcSamples * sizeof(icFloatNumber);
  m_nDstSize = GetDestSamples() * sizeof(icFloatNumber);

  m_nTotalSamples = m_nSrcSamples + GetDestSamples();

  m_nNumPixel = 0;
  m_nCacheSize = nCacheSize;

  m_pFirst = NULL;
  m_cache = new CIccMruPixel[nCacheSize];

  if (!m_cache)
    return false;

  m_pixelData = (icFloatNumber*)malloc(nCacheSize * m_nTotalSamples * sizeof(icFloatNumber));

  if (!m_pixelData)
    return false;

  return true;
}

/**
****************************************************************************
* Name: CIccMruCmm::Apply
* 
* Purpose: Apply a transformation to a pixel.
* 
* Args:
*  DstPixel - Location to store pixel results
*  SrcPixel - Location to get pixel values from
*
* Return:
*  icCmmStatOk if successful
*****************************************************************************
*/
icStatusCMM CIccMruCmm::Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel)
{
  CIccMruPixel *ptr, *prev=NULL, *last=NULL;
  int i;
  icFloatNumber *pixel;

  for (ptr = m_pFirst, i=0; ptr; ptr=ptr->pNext, i++) {
    if (!memcmp(SrcPixel, ptr->pPixelData, m_nSrcSize)) {
      memcpy(DstPixel, &ptr->pPixelData[m_nSrcSamples], m_nDstSize);
      return icCmmStatOk;
    }
    prev = last;
    last = ptr;
  }

  //If we get here SrcPixel is not in the cache
  if (i<m_nCacheSize) {
    pixel = &m_pixelData[i*m_nTotalSamples];

    ptr = &m_cache[i];
    ptr->pPixelData = pixel;

    if (!last) {
      m_pFirst = ptr;
    }
    else {

      last->pNext =  ptr;
    }
  }
  else {  //Reuse oldest value and put it at the front of the list
    prev->pNext = NULL;
    last->pNext = m_pFirst;

    m_pFirst = last;
    pixel = last->pPixelData;
  }
  icFloatNumber *dest = &pixel[m_nSrcSamples];

  memcpy(pixel, SrcPixel, m_nSrcSize);

  m_pCmm->Apply(dest, pixel);

  memcpy(DstPixel, dest, m_nDstSize);

  return icCmmStatOk;
}

#ifdef USESAMPLEICCNAMESPACE
} //namespace sampleICC
#endif
