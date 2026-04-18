/** @file
    File:       IccProfSeqId.cpp

    Contains:   Implementation of prototype profileSequenceIdentifier Tag

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
// -Initial implementation by Max Derhak Oct-21-2006
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning( disable: 4786) //disable warning in <list.h>
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "IccTagProfSeqId.h"
#include "IccUtil.h"
#include "IccIO.h"

/**
****************************************************************************
* Name: sampleICC::CIccProfileIdDesc::CIccProfileIdDesc
* 
* Purpose: 
* 
* Args:
*
* Return:
*  
*****************************************************************************
*/
CIccProfileIdDesc::CIccProfileIdDesc()
{
  memset(&m_profileID, 0, sizeof(m_profileID));
}


/**
****************************************************************************
* Name: sampleICC::CIccProfileIdDesc::CIccProfileIdDesc
* 
* Purpose: 
* 
* Args:
*  CIccProfile &profile
*
* Return:
*  
*****************************************************************************
*/
CIccProfileIdDesc::CIccProfileIdDesc(CIccProfile &profile)
{
  m_profileID = profile.m_Header.profileID;
  CIccTag *pTag = profile.FindTag(icSigProfileDescriptionTag);

  if (pTag) {
    switch (pTag->GetType()) {
      case icSigMultiLocalizedUnicodeType:
      {
        m_desc = *((CIccTagMultiLocalizedUnicode*)pTag);
      }
      break;

      case icSigTextDescriptionType:
      {
        CIccTagTextDescription *pText = (CIccTagTextDescription*)pTag;

        m_desc.SetText(pText->GetText());
      }
      break;

      case icSigTextType:
      {
        CIccTagText *pText = (CIccTagText*)pTag;

        m_desc.SetText(pText->GetText());
      }
      break;
    }
  }
}


/**
****************************************************************************
* Name: sampleICC::CIccProfileIdDesc::CIccProfileIdDesc
* 
* Purpose: 
* 
* Args:
*  icProfileID id
*  CIccMultiLocalizedUnicode desc
*
* Return:
*  
*****************************************************************************
*/
CIccProfileIdDesc::CIccProfileIdDesc(icProfileID id, CIccTagMultiLocalizedUnicode desc)
{
  m_profileID = id;
  m_desc = desc;
}


/**
****************************************************************************
* Name: sampleICC::CIccProfileIdDesc::CIccProfileIdDesc
* 
* Purpose: 
* 
* Args:
*  const CIccProfileIdDesc &pid
*
* Return:
*  
*****************************************************************************
*/
CIccProfileIdDesc::CIccProfileIdDesc(const CIccProfileIdDesc &pid)
{
  m_profileID = pid.m_profileID;
  m_desc = pid.m_desc;
}


/**
****************************************************************************
* Name: sampleICC::CIccProfileIdDesc::operator=
* 
* Purpose: 
* 
* Args:
*  const CIccProfileIdDesc &pid
*
* Return:
*  CIccProfileIdDesc & 
*****************************************************************************
*/
CIccProfileIdDesc &CIccProfileIdDesc::operator=(const CIccProfileIdDesc &pid)
{
  if (&pid == this)
    return *this;

  m_profileID = pid.m_profileID;
  m_desc = pid.m_desc;

  return *this;
}


/**
****************************************************************************
* Name: sampleICC::CIccProfileIdDesc::Describe
* 
* Purpose: 
* 
* Args:
*  std::string &sDescription
*
* Return:
*  void 
*****************************************************************************
*/
void CIccProfileIdDesc::Describe(std::string &sDescription)
{
  std::string Dump;

  sDescription += "ProfileID:\n";

  int i;
  char buf[20];
  for (i=0; i<sizeof(icProfileID); i++) {
    if (i && i%4==0)
      sDescription += " ";
    sprintf(buf, "%2x", m_profileID.ID8[i]);
    sDescription += buf;
  }
  sDescription += "\n";

  sDescription += "Description:\n";
  m_desc.Describe(sDescription);

  sDescription += "\n";
}


/**
****************************************************************************
* Name: sampleICC::CIccProfileIdDesc::Read
* 
* Purpose: 
* 
* Args:
*  icUInt32Number size
*  CIccIO *pIO
*
* Return:
*  bool 
*****************************************************************************
*/
bool CIccProfileIdDesc::Read(icUInt32Number size, CIccIO *pIO)
{
  if (sizeof (icProfileID) > size)
    return false;

  if (pIO->Read8(&m_profileID, sizeof(icProfileID))!=sizeof(icProfileID))
    return false;

  if (!m_desc.Read(size - sizeof(icProfileID), pIO))
    return false;

  return true;
}


/**
****************************************************************************
* Name: sampleICC::CIccProfileIdDesc::Write
* 
* Purpose: 
* 
* Args:
*  CIccIO *pIO
*
* Return:
*  bool 
*****************************************************************************
*/
bool CIccProfileIdDesc::Write(CIccIO *pIO)
{
  pIO->Write8(&m_profileID, sizeof(icProfileID));
  m_desc.Write(pIO);

  return true;
}


/**
****************************************************************************
* Name: sampleICC::CIccProfileIdDesc::Validate
* 
* Purpose: 
* 
* Args:
*  std::string &sReport
*
* Return:
*  icValidateStatus 
*****************************************************************************
*/
icValidateStatus CIccProfileIdDesc::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile) const
{
  return m_desc.Validate(sig, sReport, pProfile);
}



/**
 ******************************************************************************
 * Name: CIccTagProfileSequenceId::CIccTagProfileSequenceId
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccTagProfileSequenceId::CIccTagProfileSequenceId()
{
  m_list = new CIccProfileIdDescList();
}

/**
 ******************************************************************************
 * Name: CIccTagProfileSequenceId::CIccTagProfileSequenceId
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccTagProfileSequenceId::CIccTagProfileSequenceId(const CIccTagProfileSequenceId &psi)
{
  m_list = new CIccProfileIdDescList();

  *m_list = *psi.m_list;
}

/**
 ******************************************************************************
 * Name: &operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccTagProfileSequenceId &CIccTagProfileSequenceId::operator=(const CIccTagProfileSequenceId &psi)
{
  if (&psi == this)
    return *this;

  *m_list = *psi.m_list;

  return *this;
}

/**
 ******************************************************************************
 * Name: CIccTagProfileSequenceId::~CIccTagProfileSequenceId
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccTagProfileSequenceId::~CIccTagProfileSequenceId()
{
  delete m_list;
}


/**
 ******************************************************************************
 * Name: CIccTagProfileSequenceId::ParseMem
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccTagProfileSequenceId* CIccTagProfileSequenceId::ParseMem(icUInt8Number *pMem, icUInt32Number size)
{
  CIccMemIO IO;

  if (!IO.Attach(pMem, size))
    return NULL;

  CIccTagProfileSequenceId *pProSeqId = new CIccTagProfileSequenceId;

  if (!pProSeqId->Read(size, &IO)) {
    delete pProSeqId;
    return NULL;
  }

  return pProSeqId;
}


/**
 ******************************************************************************
 * Name: CIccTagProfileSequenceId::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccTagProfileSequenceId::Describe(std::string &sDescription)
{
  icChar buf[128];

  sprintf(buf, "BEGIN ProfileSequenceIdentification_TAG\n");
  sDescription += buf;
  sDescription += "\n";

  int i;
  CIccProfileIdDescList::iterator j;
  for (i=0, j=m_list->begin(); j!=m_list->end(); i++, j++) {
    sprintf(buf, "ProfileDescription_%d:\n", i+1);
    sDescription += buf;
    j->Describe(sDescription);
  }

  sprintf(buf, "END ProfileSequenceIdentification_TAG\n");
  sDescription += buf;
  sDescription += "\n";
}


/**
 ******************************************************************************
 * Name: CIccTagProfileSequenceId::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccTagProfileSequenceId::Read(icUInt32Number size, CIccIO *pIO)
{
  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt32Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  m_list->empty();

  icUInt32Number sig;
  icUInt32Number tagStart = pIO->Tell();

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  icUInt32Number count, i;

  if (!pIO->Read32(&count))
    return false;

  if (headerSize + count*sizeof(icUInt32Number)*2 > size)
    return false;

  if (!count) {
    return true;
  }

  icPositionNumber *pos = new icPositionNumber[count];
  if (!pos)
    return false;

  //Read TagDir
  for (i=0; i<count; i++) {
    if (!pIO->Read32(&pos[i].offset) ||
        !pIO->Read32(&pos[i].size)) {
      delete [] pos;
      return false;
    }
  }

  CIccProfileIdDesc pid;

  for (i=0; i<count; i++) {
    if (pos[i].offset + pos[i].size > size) {
      delete [] pos;
      return false;
    }
    pIO->Seek(tagStart + pos[i].offset, icSeekSet);

    if (!pid.Read(pos[i].size, pIO)) {
      delete [] pos;
      return false;
    }

    m_list->push_back(pid);
  }

  delete [] pos;

  return true;
}

/**
 ******************************************************************************
 * Name: CIccTagProfileSequenceId::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccTagProfileSequenceId::Write(CIccIO *pIO)
{
  icTagTypeSignature sig = GetType();

  if (!pIO)
    return false;

  icUInt32Number tagStart = pIO->Tell();

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  icUInt32Number i, count = m_list->size();

  pIO->Write32(&count);

  icPositionNumber *pos = new icPositionNumber[count];
  if (!pos)
    return false;

  icUInt32Number dirpos = pIO->Tell();

  //Write Unintialized TagDir
  for (i=0; i<count; i++) {
    pos[i].offset = 0;
    pos[i].size = 0;
    pIO->Write32(&pos[i].offset);
    pIO->Write32(&pos[i].size);
  }

  CIccProfileIdDescList::iterator j;

  //Write Tags
  for (i=0, j=m_list->begin(); j!= m_list->end(); i++, j++) {
    pos[i].offset = pIO->Tell();

    j->Write(pIO);
    pos[i].size = pIO->Tell() - pos[i].offset;
    pos[i].offset -= tagStart;

    pIO->Align32();
  }

  icUInt32Number endpos = pIO->Tell();

  pIO->Seek(dirpos, icSeekSet);

  //Write TagDir with offsets and sizes
  for (i=0; i<count; i++) {
    pIO->Write32(&pos[i].offset);
    pIO->Write32(&pos[i].size);
  }

  pIO->Seek(endpos, icSeekSet);

  return true;
}


/**
 ******************************************************************************
 * Name: CIccTagProfileSequenceId::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccTagProfileSequenceId::Validate(icTagSignature sig, std::string &sReport,
                                           const CIccProfile* pProfile /*=NULL*/) const
{ 
  icValidateStatus rv = CIccTag::Validate(sig, sReport, pProfile);

  CIccInfo Info;
  std::string sSigName = Info.GetSigName(sig);

  CIccProfileIdDescList::iterator i;
  
  for (i=m_list->begin(); i!=m_list->end(); i++) {
    rv = icMaxStatus(rv, i->Validate(sig, sReport, pProfile));
  }

  return rv;
}


/**
****************************************************************************
* Name: sampleICC::CIccTagProfileSequenceId::AddProfileDescription
* 
* Purpose: 
* 
* Args:
*  CIccProfileIdDesc &profileDesc
*
* Return:
*  bool 
*****************************************************************************
*/
bool CIccTagProfileSequenceId::AddProfileDescription(const CIccProfileIdDesc &profileDesc)
{
  m_list->push_back(profileDesc);

  return true;
}


/**
****************************************************************************
* Name: sampleICC::CIccTagProfileSequenceId::GetFirst
* 
* Purpose: 
* 
* Args:
*
* Return:
*  CIccProfileIdDesc * 
*****************************************************************************
*/
CIccProfileIdDesc *CIccTagProfileSequenceId::GetFirst()
{
  if (m_list->size())
    return &(*(m_list->begin()));

  return NULL;
}


/**
****************************************************************************
* Name: sampleICC::CIccTagProfileSequenceId::GetLast
* 
* Purpose: 
* 
* Args:
*
* Return:
*  CIccProfileIdDesc * 
*****************************************************************************
*/
CIccProfileIdDesc *CIccTagProfileSequenceId::GetLast()
{
  if (m_list->size())
    return &(*(m_list->rbegin()));

  return NULL;
}

