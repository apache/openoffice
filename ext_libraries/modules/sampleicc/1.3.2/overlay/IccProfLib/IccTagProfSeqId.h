/** @file
File:       IccTagProfSeqId.h

Contains:   Header for implementation of CIccTagProfSeqId
and supporting classes

Version:    V1

Copyright:  © see ICC Software License
*/

/*
* The ICC Software License, Version 0.1
*
*
* Copyright (c) 2005 The International Color Consortium. All rights 
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
// -Jun 3, 2007 
//  Initial CIccTagProfSeqId development
//
//////////////////////////////////////////////////////////////////////

#ifndef _ICCTAGPROFSEQID_H
#define _ICCTAGPROFSEQID_H

#include "IccProfile.h"
#include "IccTag.h"
#include <memory>
#include <list>

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

class ICCPROFLIB_API CIccProfileIdDesc
{
public:
  CIccProfileIdDesc();
  CIccProfileIdDesc(CIccProfile &profile);
  CIccProfileIdDesc(icProfileID id, CIccTagMultiLocalizedUnicode desc);
  CIccProfileIdDesc(const CIccProfileIdDesc &pid);
  CIccProfileIdDesc &operator=(const CIccProfileIdDesc &pid);

  void Describe(std::string &sDescription);

  bool Read(icUInt32Number size, CIccIO *pIO);
  bool Write(CIccIO *pIO);

  icValidateStatus Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL) const;

  CIccTagMultiLocalizedUnicode m_desc;
  icProfileID m_profileID;
};

typedef std::list<CIccProfileIdDesc> CIccProfileIdDescList;

/**
****************************************************************************
* Class: CIccTagProfileSequenceId
* 
* Purpose: The ProfileSequenceId tag 
*****************************************************************************
*/
class ICCPROFLIB_API CIccTagProfileSequenceId : public CIccTag
{
public:
  CIccTagProfileSequenceId();
  CIccTagProfileSequenceId(const CIccTagProfileSequenceId &lut);
  CIccTagProfileSequenceId &operator=(const CIccTagProfileSequenceId &lut);
  virtual CIccTag *NewCopy() const { return new CIccTagProfileSequenceId(*this);}
  virtual ~CIccTagProfileSequenceId();

  static CIccTagProfileSequenceId *ParseMem(icUInt8Number *pMem, icUInt32Number size);

  virtual icTagTypeSignature GetType() { return icSigProfileSequceIdType; }
  virtual const icChar *GetClassName() { return "CIccTagProfileSequenceId"; }

  virtual void Describe(std::string &sDescription);

  virtual bool Read(icUInt32Number size, CIccIO *pIO);
  virtual bool Write(CIccIO *pIO);

  virtual icValidateStatus Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL) const;

  bool AddProfileDescription(CIccProfile &profile) { return AddProfileDescription(CIccProfileIdDesc(profile)); }
  bool AddProfileDescription(const CIccProfileIdDesc &profileDesc);

  CIccProfileIdDesc *GetFirst();
  CIccProfileIdDesc *GetLast();

  CIccProfileIdDescList::iterator begin() { return m_list->begin(); }
  CIccProfileIdDescList::iterator end() { return m_list->end(); }

protected:
  void Cleanup();

  CIccProfileIdDescList *m_list;
};



//CIccTagProfSeq support
#ifdef USESAMPLEICCNAMESPACE
}
#endif

#endif //_ICCTAGPROFSEQID_H
