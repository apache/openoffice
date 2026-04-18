/** @file
    File:       IccProfile.cpp

    Contains:   Implementation of the CIccProfile class.

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
#include <time.h>
#include <string.h>
#include "IccProfile.h"
#include "IccTag.h"
#include "IccIO.h"
#include "IccUtil.h"
#include "md5.h"


#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 **************************************************************************
 * Name: CIccProfile::CIccProfile
 * 
 * Purpose: 
 *  Constructor
 **************************************************************************
 */
CIccProfile::CIccProfile()
{
  m_pAttachIO = NULL;
  memset(&m_Header, 0, sizeof(m_Header));
  m_Tags = new(TagEntryList);
  m_TagVals = new(TagPtrList);
}

/**
 **************************************************************************
 * Name: CIccProfile::CIccProfile
 * 
 * Purpose: 
 *  Copy Constructor. The copy constructor makes the copy of the 
 *  CIccProfile object in it's present state. It DOES NOT make a 
 *  copy of the m_pAttachIO member variable. Any operation with the
 *  IO object should be done before making a copy.
 * 
 * Args:
 *  Profile = CIccProfile object which is to be copied.
 **************************************************************************
 */
CIccProfile::CIccProfile(const CIccProfile &Profile)
{
  m_pAttachIO = NULL;
  memset(&m_Header, 0, sizeof(m_Header));
  m_Tags = new(TagEntryList);
  m_TagVals = new(TagPtrList);
  memcpy(&m_Header, &Profile.m_Header, sizeof(m_Header));

  if (!Profile.m_Tags->empty()) {
    TagEntryList::const_iterator i;
    IccTagEntry entry;
    for (i=Profile.m_Tags->begin(); i!=Profile.m_Tags->end(); i++) {
      entry.pTag = i->pTag->NewCopy();
      memcpy(&entry.TagInfo, &i->TagInfo, sizeof(icTag));
      m_Tags->push_back(entry);
    }
  }

  if (!Profile.m_TagVals->empty()) {
    TagPtrList::const_iterator i;
    IccTagPtr tagptr;
    for (i=Profile.m_TagVals->begin(); i!=Profile.m_TagVals->end(); i++) {
      tagptr.ptr = i->ptr->NewCopy();
      m_TagVals->push_back(tagptr);
    }
  }

  m_pAttachIO = NULL;  
}

/**
 **************************************************************************
 * Name: CIccProfile::operator=
 * 
 * Purpose: 
 *  Copy Operator. The copy operator makes the copy of the 
 *  CIccProfile object in it's present state. It DOES NOT make a 
 *  copy of the m_pAttachIO member variable. Any operation with the
 *  IO object should be done before making a copy.
 * 
 * Args:
 *  Profile = CIccProfile object which is to be copied.
 **************************************************************************
 */
CIccProfile &CIccProfile::operator=(const CIccProfile &Profile)
{
  if (&Profile == this)
    return *this;

  Cleanup();

  memcpy(&m_Header, &Profile.m_Header, sizeof(m_Header));

  if (!Profile.m_Tags->empty()) {
    m_Tags->clear();
    TagEntryList::const_iterator i;
    IccTagEntry entry;
    for (i=Profile.m_Tags->begin(); i!=Profile.m_Tags->end(); i++) {
      entry.pTag = i->pTag->NewCopy();
      memcpy(&entry.TagInfo, &i->TagInfo, sizeof(icTag));
      m_Tags->push_back(entry);
    }
  }

  if (!Profile.m_TagVals->empty()) {
    m_TagVals->clear();
    TagPtrList::const_iterator i;
    IccTagPtr tagptr;
    for (i=Profile.m_TagVals->begin(); i!=Profile.m_TagVals->end(); i++) {
      tagptr.ptr = i->ptr->NewCopy();
      m_TagVals->push_back(tagptr);
    }
  }

  m_pAttachIO = NULL;

  return *this;  
}

/**
 **************************************************************************
 * Name: CIccProfile::CIccProfile
 * 
 * Purpose: 
 *  Destructor
 **************************************************************************
 */
CIccProfile::~CIccProfile()
{
  Cleanup();

  delete m_Tags;
  delete m_TagVals;
}

/**
 ***************************************************************************
 * Name: CIccProfile::Cleanup
 * 
 * Purpose: Detach from a pending IO object
 ***************************************************************************
 */
void CIccProfile::Cleanup()
{
  if (m_pAttachIO) {
    delete m_pAttachIO;
    m_pAttachIO = NULL;
  }

  TagPtrList::iterator i;

  for (i=m_TagVals->begin(); i!=m_TagVals->end(); i++) {
    if (i->ptr)
      delete i->ptr;
  }
  m_Tags->clear();
  m_TagVals->clear();
  memset(&m_Header, 0, sizeof(m_Header));
}

/**
 ****************************************************************************
 * Name: CIccProfile::GetTag
 * 
 * Purpose: Get a tag entry with a given signature
 * 
 * Args: 
 *  sig - signature id to find in tag directory
 * 
 * Return: 
 *  Pointer to desired tag directory entry, or NULL if not found.
 *****************************************************************************
 */
IccTagEntry* CIccProfile::GetTag(icSignature sig) const
{
  TagEntryList::const_iterator i;

  for (i=m_Tags->begin(); i!=m_Tags->end(); i++) {
    if (i->TagInfo.sig==sig)
      return (IccTagEntry*)&(i->TagInfo);
  }

  return NULL;
}


/**
 ******************************************************************************
 * Name: CIccProfile::AreTagsUnique
 * 
 * Purpose: For each tag it checks to see if any other tags have the same
 *  signature.
 * 
 * 
 * Return: 
 *  true if all tags have unique signatures, or false if there are duplicate
 *  tag signatures.
 *******************************************************************************
 */
bool CIccProfile::AreTagsUnique() const
{
  TagEntryList::const_iterator i, j;

  for (i=m_Tags->begin(); i!=m_Tags->end(); i++) {
    j=i;
    for (j++; j!= m_Tags->end(); j++) {
      if (i->TagInfo.sig == j->TagInfo.sig)
        return false;
    }
  }

  return true;
}


/**
******************************************************************************
* Name: CIccProfile::GetTag
* 
* Purpose: Finds the first tag entry that points to the indicated tag object
* 
* Args: 
*  pTag - pointer to tag object desired to be found
* 
* Return: 
*  pointer to first tag directory entry that points to the desired tag object,
*  or NULL if tag object is not pointed to by any tag directory entries.
*******************************************************************************
*/
IccTagEntry* CIccProfile::GetTag(CIccTag *pTag) const
{
  TagEntryList::const_iterator i;

  for (i=m_Tags->begin(); i!=m_Tags->end(); i++) {
    if (i->pTag==pTag)
      return (IccTagEntry*)&(i->TagInfo);
  }

  return NULL;
}


/**
 ******************************************************************************
 * Name: CIccProfile::FindTag
 * 
 * Purpose: Finds the tag object associated with the directory entry with the
 *  given signature.  If the profile object is attached to an IO object then
 *  the tag may need to be loaded first.
 * 
 * Args: 
 *  sig - tag signature to find in profile
 * 
 * Return: 
 *  The desired tag object, or NULL if unable to find in the directory or load
 *  tag object.
 *******************************************************************************
 */
CIccTag* CIccProfile::FindTag(icSignature sig)
{
  IccTagEntry *pEntry = GetTag(sig);

  if (pEntry) {
    if (!pEntry->pTag && m_pAttachIO)
      LoadTag(pEntry, m_pAttachIO);
    return pEntry->pTag;
  }

  return NULL;
}


/**
 ******************************************************************************
 * Name: CIccProfile::AttachTag
 * 
 * Purpose: Assign a tag object to a directory entry in the profile.  This
 *  will assume ownership of the tag object.
 * 
 * Args: 
 *  sig - signature of tag 'name' to use to assign tag object with,
 *  pTag - pointer to tag object to attach to profile.
 * 
 * Return: 
 *  true = tag assigned to profile,
 *  false - tag not assigned to profile (tag already exists).  
 *******************************************************************************
 */
bool CIccProfile::AttachTag(icSignature sig, CIccTag *pTag)
{
  IccTagEntry *pEntry = GetTag(sig);

  if (pEntry) {
    if (pEntry->pTag == pTag)
      return true;

    return false;
  }

  IccTagEntry Entry;
  Entry.TagInfo.sig = (icTagSignature)sig;
  Entry.TagInfo.offset = 0;
  Entry.TagInfo.size = 0;
  Entry.pTag = pTag;

  m_Tags->push_back(Entry);

  TagPtrList::iterator i;

  for (i=m_TagVals->begin(); i!=m_TagVals->end(); i++)
    if (i->ptr == pTag)
      break;

  if (i==m_TagVals->end()) {
    IccTagPtr TagPtr;
    TagPtr.ptr = pTag;
    m_TagVals->push_back(TagPtr);
  }

  return true;
}


/**
 ******************************************************************************
 * Name: CIccProfile::DeleteTag
 * 
 * Purpose: Delete tag directory entry with given signature.  If no other tag
 *  directory entries use the tag object, the tag object will also be deleted.
 * 
 * Args: 
 *  sig - signature of tag directory entry to remove
 * 
 * Return: 
 *  true - desired tag directory entry was found and deleted,
 *  false - desired tag directory entry was not found
 *******************************************************************************
 */
bool CIccProfile::DeleteTag(icSignature sig)
{
  TagEntryList::iterator i;

  for (i=m_Tags->begin(); i!=m_Tags->end(); i++) {
    if (i->TagInfo.sig==sig)
      break;
  }
  if (i!=m_Tags->end()) {
    CIccTag *pTag = i->pTag;
    m_Tags->erase(i);

    if (!GetTag(pTag)) {
      DetachTag(pTag);
      delete pTag;
    }
    return true;
  }

  return false;
}


/**
 ******************************************************************************
 * Name: CIccProfile::Attach
 * 
 * Purpose: This allows for deferred IO with a profile.  The profile header and
 *  tag directory will be read, but tag data will not be read.  The IO object
 *  will remain attached to the profile for the purpose of reading data in as
 *  needed.
 * 
 * Args: 
 *  pIO - pointer to IO object to begin reading profile file with.
 * 
 * Return: 
 *  true - the IO object (file) is an ICC profile, and the CIccProfile object
 *    is now attached to the object,
 *  false - the IO object (file) is not an ICC profile.
 *******************************************************************************
 */
bool CIccProfile::Attach(CIccIO *pIO)
{
  if (m_Tags->size())
    Cleanup();

  if (!ReadBasic(pIO)) {
    Cleanup();
    return false;
  }

  m_pAttachIO = pIO;

  return true;
}

/**
******************************************************************************
* Name: CIccProfile::Detach
* 
* Purpose: Discontinues the use of defferred IO with a profile.  This can be done
*  once all the information needed for performing a transform has been extracted
*  from the profile.
* 
* Args: 
*  true - If an IO object was attached to the profile
*  false - if no IO object was attached to the profile
*******************************************************************************
*/
bool CIccProfile::Detach()
{
  if (m_pAttachIO) {
    delete m_pAttachIO;

    m_pAttachIO = NULL;
    return true;
  }

  return false;
}

/**
 ******************************************************************************
 * Name: CIccProfile::Read
 * 
 * Purpose: This will read the entire ICC profile from the IO object into the
 *  CIccProfile object
 * 
 * Args: 
 *  pIO - pointer to IO object to read ICC profile from
 * 
 * Return: 
 *  true - the IO object (file) is an ICC profile, and the CIccProfile object
 *   now contains all its data,
 *  false - the IO object (file) is not an ICC profile.
 *******************************************************************************
 */
bool CIccProfile::Read(CIccIO *pIO)
{
  if (m_Tags->size())
    Cleanup();

  if (!ReadBasic(pIO)) {
    Cleanup();
    return false;
  }

  TagEntryList::iterator i;

  for (i=m_Tags->begin(); i!=m_Tags->end(); i++) {
    if (!LoadTag((IccTagEntry*)&(i->TagInfo), pIO)) {
      Cleanup();
      return false;
    }
  }

  return true;
}

/**
******************************************************************************
* Name: CIccProfile::ReadValidate
* 
* Purpose: This will read the entire ICC profile from the IO object into the
*  CIccProfile object
* 
* Args: 
*  pIO - pointer to IO object to read ICC profile from
*  sReport - string to put validation report info into. String should be initialized
*  before calling
* 
* Return: 
*  icValidateOK if file can be read, bad status otherwise.
*******************************************************************************
*/
icValidateStatus CIccProfile::ReadValidate(CIccIO *pIO, std::string &sReport)
{
  icValidateStatus rv = icValidateOK;

  if (m_Tags->size())
    Cleanup();

  if (!ReadBasic(pIO)) {
    sReport += icValidateCriticalErrorMsg;
    sReport += " - Unable to read profile!**\n\tProfile has invalid structure!\n";
    Cleanup();

    return icValidateCriticalError;
  }

  // Check profile header
  if (!CheckFileSize(pIO)) {
    sReport += icValidateNonCompliantMsg;
    sReport += "Bad Header File Size\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  CIccInfo Info;
  icProfileID profileID;

  // Check profile ID
  if (Info.IsProfileIDCalculated(&m_Header.profileID)) {
    CalcProfileID(pIO, &profileID);
    if (strncmp((char*)profileID.ID8, (char*)m_Header.profileID.ID8, 16) != 0) {
      sReport += icValidateNonCompliantMsg;
      sReport += "Bad Profile ID\n";

      rv = icMaxStatus(rv, icValidateNonCompliant);
    }
  }

  TagEntryList::iterator i;

  for (i=m_Tags->begin(); i!=m_Tags->end(); i++) {
    if (!LoadTag((IccTagEntry*)&(i->TagInfo), pIO)) {
      sReport += icValidateCriticalErrorMsg;
      sReport += " - ";
      sReport += Info.GetTagSigName(i->TagInfo.sig);
      sReport += " - Tag has invalid structure!\n";

      rv = icMaxStatus(rv, icValidateCriticalError);
    }
  }

  if (rv==icValidateCriticalError)
    Cleanup();

  return rv;
}


/**
 ******************************************************************************
 * Name: CIccProfile::Write
 * 
 * Purpose: Write the data associated with the CIccProfile object to an IO
 *  IO object.
 * 
 * Args: 
 *  pIO - pointer to IO object to write data to
 * 
 * Return: 
 *  true - success, false - failure
 *******************************************************************************
 */
bool CIccProfile::Write(CIccIO *pIO)
{
  //Write Header
  pIO->Seek(0, icSeekSet);

  pIO->Write32(&m_Header.size);
  pIO->Write32(&m_Header.cmmId);
  pIO->Write32(&m_Header.version);
  pIO->Write32(&m_Header.deviceClass);
  pIO->Write32(&m_Header.colorSpace);
  pIO->Write32(&m_Header.pcs);
  pIO->Write16(&m_Header.date.year);
  pIO->Write16(&m_Header.date.month);
  pIO->Write16(&m_Header.date.day);
  pIO->Write16(&m_Header.date.hours);
  pIO->Write16(&m_Header.date.minutes);
  pIO->Write16(&m_Header.date.seconds);
  pIO->Write32(&m_Header.magic);
  pIO->Write32(&m_Header.platform);
  pIO->Write32(&m_Header.flags);
  pIO->Write32(&m_Header.manufacturer);
  pIO->Write32(&m_Header.model);
  pIO->Write64(&m_Header.attributes);
  pIO->Write32(&m_Header.renderingIntent);
  pIO->Write32(&m_Header.illuminant.X);
  pIO->Write32(&m_Header.illuminant.Y);
  pIO->Write32(&m_Header.illuminant.Z);
  pIO->Write32(&m_Header.creator);
  pIO->Write8(&m_Header.profileID, sizeof(m_Header.profileID));
  pIO->Write8(&m_Header.reserved[0], sizeof(m_Header.reserved));

  TagEntryList::iterator i, j;
  icUInt32Number count;

  for (count=0, i=m_Tags->begin(); i!= m_Tags->end(); i++) {
    if (i->pTag)
      count++;
  }

  pIO->Write32(&count);

  icUInt32Number dirpos = pIO->GetLength();

  //Write Unintialized TagDir
  for (i=m_Tags->begin(); i!= m_Tags->end(); i++) {
    if (i->pTag) {
      i->TagInfo.offset = 0;
      i->TagInfo.size = 0;

      pIO->Write32(&i->TagInfo.sig);
      pIO->Write32(&i->TagInfo.offset);
      pIO->Write32(&i->TagInfo.size);
    }
  }

  //Write Tags
  for (i=m_Tags->begin(); i!= m_Tags->end(); i++) {
    if (i->pTag) {
      for (j=m_Tags->begin(); j!=i; j++) {
        if (i->pTag == j->pTag)
          break;
      }

      if (i==j) {
        i->TagInfo.offset = pIO->GetLength();
        i->pTag->Write(pIO);
        i->TagInfo.size = pIO->GetLength() - i->TagInfo.offset;

        pIO->Align32();
      }
      else {
        i->TagInfo.offset = j->TagInfo.offset;
        i->TagInfo.size = j->TagInfo.size;
      }
    }
  }

  pIO->Seek(dirpos, icSeekSet);

  //Write TagDir with offsets and sizes
  for (i=m_Tags->begin(); i!= m_Tags->end(); i++) {
    if (i->pTag) {
      pIO->Write32(&i->TagInfo.sig);
      pIO->Write32(&i->TagInfo.offset);
      pIO->Write32(&i->TagInfo.size);
    }
  }

  //Update header with size
  m_Header.size = pIO->GetLength();
  pIO->Seek(0, icSeekSet);
  pIO->Write32(&m_Header.size);

  //Write the profile ID if version 4 profile
  if(m_Header.version>=icVersionNumberV4) {
    CalcProfileID(pIO, &m_Header.profileID);
    pIO->Seek(84, icSeekSet);
    pIO->Write8(&m_Header.profileID, sizeof(m_Header.profileID));
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccProfile::InitHeader
 * 
 * Purpose: Initializes the data to be written in the profile header.
 * 
 *******************************************************************************
 */
void CIccProfile::InitHeader()
{
  m_Header.size = 0;
  m_Header.cmmId = icSigSampleICC;
  m_Header.version=icVersionNumberV4;
  m_Header.deviceClass = (icProfileClassSignature)0;
  m_Header.colorSpace = (icColorSpaceSignature)0;
  m_Header.pcs = icSigLabData;
  
  struct tm *newtime;
  time_t long_time;

  time( &long_time );                /* Get time as long integer. */
  newtime = gmtime( &long_time ); 

  m_Header.date.year = newtime->tm_year+1900;
  m_Header.date.month = newtime->tm_mon+1;
  m_Header.date.day = newtime->tm_mday;
  m_Header.date.hours = newtime->tm_hour;
  m_Header.date.minutes = newtime->tm_min;
  m_Header.date.seconds = newtime->tm_sec;

  m_Header.magic = icMagicNumber;
  m_Header.platform = (icPlatformSignature)0;
  m_Header.flags = 0;
  m_Header.manufacturer=0;
  m_Header.model=0;
  m_Header.attributes=0;
  m_Header.renderingIntent=icPerceptual;
  m_Header.illuminant.X = icDtoF((icFloatNumber)0.9642);
  m_Header.illuminant.Y = icDtoF((icFloatNumber)1.0000);
  m_Header.illuminant.Z = icDtoF((icFloatNumber)0.8249);
  m_Header.creator = icSigSampleICC;

  memset(&m_Header.profileID, 0, sizeof(m_Header.profileID));
  memset(&m_Header.reserved[0], 0, sizeof(m_Header.reserved));
}


/**
 *****************************************************************************
 * Name: CIccProfile::ReadBasic
 * 
 * Purpose: Read in ICC header and tag directory entries.
 * 
 * Args: 
 *  pIO - pointer to IO object to read data with
 * 
 * Return: 
 *  true - valid ICC header and tag directory, false - failure
 ******************************************************************************
 */
bool CIccProfile::ReadBasic(CIccIO *pIO)
{
  //Read Header
  if (pIO->Seek(0, icSeekSet)<0 ||
      !pIO->Read32(&m_Header.size) ||
      !pIO->Read32(&m_Header.cmmId) ||
      !pIO->Read32(&m_Header.version) ||
      !pIO->Read32(&m_Header.deviceClass) ||
      !pIO->Read32(&m_Header.colorSpace) ||
      !pIO->Read32(&m_Header.pcs) ||
      !pIO->Read16(&m_Header.date.year) ||
      !pIO->Read16(&m_Header.date.month) ||
      !pIO->Read16(&m_Header.date.day) ||
      !pIO->Read16(&m_Header.date.hours) ||
      !pIO->Read16(&m_Header.date.minutes) ||
      !pIO->Read16(&m_Header.date.seconds) ||
      !pIO->Read32(&m_Header.magic) ||
      !pIO->Read32(&m_Header.platform) ||
      !pIO->Read32(&m_Header.flags) ||
      !pIO->Read32(&m_Header.manufacturer) ||
      !pIO->Read32(&m_Header.model) ||
      !pIO->Read64(&m_Header.attributes) ||
      !pIO->Read32(&m_Header.renderingIntent) ||
      !pIO->Read32(&m_Header.illuminant.X) ||
      !pIO->Read32(&m_Header.illuminant.Y) ||
      !pIO->Read32(&m_Header.illuminant.Z) ||
      !pIO->Read32(&m_Header.creator) ||
      pIO->Read8(&m_Header.profileID, sizeof(m_Header.profileID))!=sizeof(m_Header.profileID) ||
      pIO->Read8(&m_Header.reserved[0], sizeof(m_Header.reserved))!=sizeof(m_Header.reserved)) {
    return false;
  }

  if (m_Header.magic != icMagicNumber)
    return false;

  icUInt32Number count, i;
  IccTagEntry TagEntry;

  TagEntry.pTag = NULL;

  if (!pIO->Read32(&count))
    return false;

  //Read TagDir
  for (i=0; i<count; i++) {
    if (!pIO->Read32(&TagEntry.TagInfo.sig) ||
        !pIO->Read32(&TagEntry.TagInfo.offset) ||
        !pIO->Read32(&TagEntry.TagInfo.size)) {
      return false;
    }
    m_Tags->push_back(TagEntry);
  }


  return true;
}


/**
 ******************************************************************************
 * Name: CIccProfile::LoadTag
 * 
 * Purpose: This will load from the indicated IO object and associate a tag
 *  object to a tag directory entry.  Nothing happens if tag directory entry
 *  is associated with a tag object.
 * 
 * Args: 
 *  pTagEntry - pointer to tag directory entry,
 *  pIO - pointer to IO object to read tag object data from
 * 
 * Return: 
 *  true - tag directory object associated with tag directory entry,
 *  false - failure
 *******************************************************************************
 */
bool CIccProfile::LoadTag(IccTagEntry *pTagEntry, CIccIO *pIO)
{
  if (!pTagEntry)
    return false;

  if (pTagEntry->pTag)
    return true;

  if (pTagEntry->TagInfo.offset<sizeof(m_Header) ||
    !pTagEntry->TagInfo.size) {
    return false;
  }

  icTagTypeSignature sigType;

  //First we need to get the tag type to create the right kind of tag
  if (pIO->Seek(pTagEntry->TagInfo.offset, icSeekSet)!=pTagEntry->TagInfo.offset)
    return false;

  if (!pIO->Read32(&sigType))
    return false;

  CIccTag *pTag = CIccTag::Create(sigType);

  if (!pTag)
    return false;

  //Now seek back to where the tag starts so the created tag object can read
  //in its data.
  //First we need to get the tag type to create the right kind of tag
  if (pIO->Seek(pTagEntry->TagInfo.offset, icSeekSet)!=pTagEntry->TagInfo.offset) {
    delete pTag;
    return false;
  }

  if (!pTag->Read(pTagEntry->TagInfo.size, pIO)) {
    delete pTag;
    return false;
  }

  switch(pTagEntry->TagInfo.sig) {
  case icSigAToB0Tag:
  case icSigAToB1Tag:
  case icSigAToB2Tag:
    if (pTag->IsMBBType())
      ((CIccMBB*)pTag)->SetColorSpaces(m_Header.colorSpace, m_Header.pcs);
    break;

  case icSigBToA0Tag:
  case icSigBToA1Tag:
  case icSigBToA2Tag:
    if (pTag->IsMBBType())
      ((CIccMBB*)pTag)->SetColorSpaces(m_Header.pcs, m_Header.colorSpace);
    break;
  
  case icSigGamutTag:
    if (pTag->IsMBBType())
      ((CIccMBB*)pTag)->SetColorSpaces(m_Header.pcs, icSigGamutData);
    break;

  case icSigNamedColor2Tag:
    ((CIccTagNamedColor2*)pTag)->SetColorSpaces(m_Header.pcs, m_Header.colorSpace);
  }

  pTagEntry->pTag = pTag;

  IccTagPtr TagPtr;

  TagPtr.ptr = pTag;

  m_TagVals->push_back(TagPtr);

  TagEntryList::iterator i;

  for (i=m_Tags->begin(); i!= m_Tags->end(); i++) {
    if (i->TagInfo.offset == pTagEntry->TagInfo.offset &&
        i->pTag != pTag)
      i->pTag = pTag; 
  }
  
  return true;
}


/**
 ******************************************************************************
 * Name: CIccProfile::DetachTag
 * 
 * Purpose: Remove association of a tag object from all tag directory entries.
 *  Associated tag directory entries will be removed from the tag directory.
 *  The tag object is NOT deleted from memory, but is considered to be
 *  no longer associated with the CIccProfile object.  The caller assumes
 *  ownership of the tag object.
 * 
 * Args: 
 *  pTag - pointer to tag object unassociate with the profile object
 * 
 * Return: 
 *  true - tag object found and unassociated with profile object,
 *  false - tag object not found
 *******************************************************************************
 */
bool CIccProfile::DetachTag(CIccTag *pTag)
{
  if (!pTag)
    return false;
  
  TagPtrList::iterator i;

  for (i=m_TagVals->begin(); i!=m_TagVals->end(); i++) {
    if (i->ptr == pTag)
      break;
  }

  if (i==m_TagVals->end())
    return false;

  m_TagVals->erase(i);

  TagEntryList::iterator j;
  for (j=m_Tags->begin(); j!=m_Tags->end();) {
    if (j->pTag == pTag) {
      j=m_Tags->erase(j);
    }
    else
      j++;
  }
  return true;
}


/**
****************************************************************************
* Name: CIccProfile::CheckHeader
* 
* Purpose: Validates profile header.
* 
* Return: 
*  icValidateOK if valid, or other error status.
*****************************************************************************
*/
icValidateStatus CIccProfile::CheckHeader(std::string &sReport) const
{
  icValidateStatus rv = icValidateOK;

  icChar buf[128];
  CIccInfo Info;

  switch(m_Header.deviceClass) {
  case icSigInputClass:
  case icSigDisplayClass:
  case icSigOutputClass:
  case icSigLinkClass:
  case icSigColorSpaceClass:
  case icSigAbstractClass:
  case icSigNamedColorClass:
    break;

  default:
    sReport += icValidateCriticalErrorMsg;
    sprintf(buf, " - %s: Unknown profile class!\n", Info.GetProfileClassSigName(m_Header.deviceClass));
    sReport += buf;
    rv = icMaxStatus(rv, icValidateCriticalError);
  }

  if (!Info.IsValidSpace(m_Header.colorSpace)) {
    sReport += icValidateCriticalErrorMsg;
    sprintf(buf, " - %s: Unknown color space!\n", Info.GetColorSpaceSigName(m_Header.colorSpace));
    sReport += buf;
    rv = icMaxStatus(rv, icValidateCriticalError);
  }

  if (m_Header.deviceClass==icSigLinkClass) {
    if (!Info.IsValidSpace(m_Header.pcs)) {
      sReport += icValidateCriticalErrorMsg;
      sprintf(buf, " - %s: Unknown pcs color space!\n", Info.GetColorSpaceSigName(m_Header.pcs));
      sReport += buf;
      rv = icMaxStatus(rv, icValidateCriticalError);
    }
  }
  else {
    if (m_Header.pcs!=icSigXYZData && m_Header.pcs!=icSigLabData) {
      sReport += icValidateCriticalErrorMsg;
      sprintf(buf, " - %s: Invalid pcs color space!\n", Info.GetColorSpaceSigName(m_Header.pcs));
      sReport += buf;
      rv = icMaxStatus(rv, icValidateCriticalError);
    }
  }

  rv = icMaxStatus(rv, Info.CheckData(sReport, m_Header.date));

  switch(m_Header.platform) {
  case icSigMacintosh:
  case icSigMicrosoft:
  case icSigSolaris:
  case icSigSGI:
  case icSigTaligent:
  case icSigUnkownPlatform:
    break;
  
  default:
    sReport += icValidateWarningMsg;
    sprintf(buf, " - %s: Unknown platform signature.\n", Info.GetPlatformSigName(m_Header.platform));
    sReport += buf;
    rv = icMaxStatus(rv, icValidateWarning);
  }


  switch((icCmmSignature)m_Header.cmmId) {
  //Account for registered CMM's as well:
  case icSigAdobe:
  case icSigApple:
  case icSigColorGear:
  case icSigColorGearLite:
  case icSigFujiFilm:
  case icSigHarlequinRIP:
  case icSigArgyllCMS:
  case icSigLogoSync:
  case icSigHeidelberg:
  case icSigLittleCMS:
  case icSigKodak:
  case icSigKonicaMinolta:
  case icSigMutoh:
  case icSigSampleICC:
  case icSigTheImagingFactory:
    break;

  default:
    sReport += icValidateWarningMsg;
    sprintf(buf, " - %s: Unregisterd CMM signature.\n", Info.GetCmmSigName((icCmmSignature)m_Header.cmmId));
    sReport += buf;
    rv = icMaxStatus(rv, icValidateWarning);
  }

  switch(m_Header.renderingIntent) {
  case icPerceptual:
  case icRelativeColorimetric:
  case icSaturation:
  case icAbsoluteColorimetric:
    break;

  default:
    sReport += icValidateCriticalErrorMsg;
    sprintf(buf, " - %s: Unknown rendering intent!\n", Info.GetRenderingIntentName((icRenderingIntent)m_Header.renderingIntent));
    sReport += buf;
    rv = icMaxStatus(rv, icValidateCriticalError);
  }

  rv = icMaxStatus(rv, Info.CheckData(sReport, m_Header.illuminant));
  icFloatNumber X = icFtoD(m_Header.illuminant.X);
  icFloatNumber Y = icFtoD(m_Header.illuminant.Y);
  icFloatNumber Z = icFtoD(m_Header.illuminant.Z);
  if (X<0.9640 || X>0.9644 || Y!=1.0 || Z<0.8247 || Z>0.8251) {
    sReport += icValidateNonCompliantMsg;
    sReport += " - Non D50 Illuminant XYZ values.\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  int sum=0, num = sizeof(m_Header.reserved) / sizeof(m_Header.reserved[0]);
  for (int i=0; i<num; i++) {
    sum += m_Header.reserved[i];
  }
  if (sum) {
    sReport += icValidateNonCompliantMsg;
    sReport += " - Reserved value must be zero.\n";
    rv = icMaxStatus(rv, icValidateNonCompliant);
  }

  return rv;
}


/**
****************************************************************************
* Name: CIccProfile::CheckTagExclusion
* 
* Purpose: Some tags does not have a defined interpretation for a profile 
*           of a specific class. This function does these tests.
* 
* Return: 
*  true if test successful, else false.
*****************************************************************************
*/
bool CIccProfile::CheckTagExclusion(std::string &sReport) const
{
  bool rv = true;

  CIccInfo Info;
  icChar buf[128];
  sprintf(buf, "%s", Info.GetSigName(m_Header.deviceClass));
  if (m_Header.deviceClass!=icSigInputClass && m_Header.deviceClass!=icSigDisplayClass) {
    if (GetTag(icSigGrayTRCTag) || GetTag(icSigRedTRCTag) || GetTag(icSigGreenTRCTag) ||
       GetTag(icSigBlueTRCTag) || GetTag(icSigRedColorantTag) || GetTag(icSigGreenColorantTag) ||
       GetTag(icSigBlueColorantTag))
    {
      sReport += icValidateWarningMsg;
      sReport += buf;
      sReport += " - Tag exclusion test failed.\n";
      rv = false;
    }
  }

  switch(m_Header.deviceClass) {
  case icSigNamedColorClass:
    {
      if (GetTag(icSigAToB0Tag) || GetTag(icSigAToB1Tag) || GetTag(icSigAToB2Tag) ||
        GetTag(icSigBToA0Tag) || GetTag(icSigBToA1Tag) || GetTag(icSigBToA2Tag) ||
        GetTag(icSigProfileSequenceDescTag) || GetTag(icSigGamutTag))
      {
        sReport += icValidateWarningMsg;
        sReport += buf;
        sReport += " - Tag exclusion test failed.\n";
        rv = false;
      }
      break;
    }

  case icSigAbstractClass:
  case icSigLinkClass:
    {
      if (GetTag(icSigMediaWhitePointTag) || GetTag(icSigNamedColor2Tag) ||
        GetTag(icSigAToB1Tag) || GetTag(icSigAToB2Tag) ||
        GetTag(icSigBToA1Tag) || GetTag(icSigBToA2Tag) || GetTag(icSigGamutTag))
      {
        sReport += icValidateWarningMsg;
        sReport += buf;
        sReport += " - Tag exclusion test failed.\n";
        rv = false;
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
* Name: CIccProfile::CheckTagTypes
* 
* Purpose: Check if tags have allowed tag types.
* 
* Return: 
*  icValidateOK if valid, or other error status.
*****************************************************************************
*/
icValidateStatus CIccProfile::CheckTagTypes(std::string &sReport) const
{
  icValidateStatus rv = icValidateOK;

  icChar buf[128];
  CIccInfo Info;

  icTagSignature tagsig;
  icTagTypeSignature typesig;
  TagEntryList::const_iterator i;
  for (i=m_Tags->begin(); i!=m_Tags->end(); i++) {
    tagsig = i->TagInfo.sig;
    typesig = i->pTag->GetType();
    sprintf(buf, "%s", Info.GetSigName(tagsig));
    if (!IsTypeValid(tagsig, typesig)) {
      sReport += icValidateNonCompliantMsg;
      sReport += buf;
      sprintf(buf," - %s: Invalid tag type (Might be critical!).\n", Info.GetTagTypeSigName(typesig));
      sReport += buf;
      rv = icMaxStatus(rv, icValidateNonCompliant);
    }
  }

  return rv;  
}


/**
****************************************************************************
* Name: CIccProfile::IsTypeValid
* 
* Purpose: Check if tags have allowed tag types.
* 
* Return: 
*  true if valid, else false.
*****************************************************************************
*/
bool CIccProfile::IsTypeValid(icTagSignature tagSig, icTagTypeSignature typeSig) const
{
  switch(tagSig) {
    // A to B tags
  case icSigAToB0Tag:
  case icSigAToB1Tag:
  case icSigAToB2Tag:
    {
      switch(typeSig) {
      case icSigLut8Type:
      case icSigLut16Type:
        return true;

      case icSigLutAtoBType:
        if (m_Header.version >= 0x04000000L)
          return true;
        else
          return false;

      default:
        return false;
      }
    }

    // B to A tags
  case icSigBToA0Tag:
  case icSigBToA1Tag:
  case icSigBToA2Tag:
  case icSigGamutTag:
  case icSigPreview0Tag:
  case icSigPreview1Tag:
  case icSigPreview2Tag:  
    {
      switch(typeSig) {
      case icSigLut8Type:
      case icSigLut16Type:
        return true;

      case icSigLutBtoAType:
        if (m_Header.version >= 0x04000000L)
          return true;
        else
          return false;

      default:
        return false;
      }
    }

    // Matrix column tags - XYZ types
  case icSigBlueMatrixColumnTag:
  case icSigGreenMatrixColumnTag:
  case icSigRedMatrixColumnTag:
  case icSigLuminanceTag:
  case icSigMediaWhitePointTag:
  case icSigMediaBlackPointTag:
    {
      if (typeSig!=icSigXYZType) {
        return false;
      }
      else return true;
    }

    // TRC tags
  case icSigBlueTRCTag:
  case icSigGreenTRCTag:
  case icSigRedTRCTag:
  case icSigGrayTRCTag:
    {
      switch(typeSig) {
      case icSigCurveType:
      case icSigParametricCurveType:
        return true;

      default:
        return false;
      }
    }

  case icSigCalibrationDateTimeTag:
    {
      if (typeSig!=icSigDateTimeType)
        return false;
      else return true;
    }

  case icSigCharTargetTag:
    {
      if (typeSig!=icSigTextType)
        return false;
      else
        return true;
    }

  case icSigChromaticAdaptationTag:
    {
      if (typeSig!=icSigS15Fixed16ArrayType)
        return false;
      else return true;
    }

  case icSigChromaticityTag:
    {
      if (typeSig!=icSigChromaticityType)
        return false;
      else return true;
    }

  case icSigColorantOrderTag:
    {
      if (typeSig!=icSigColorantOrderType)
        return false;
      else return true;
    }

  case icSigColorantTableTag:
  case icSigColorantTableOutTag:
    {
      if (typeSig!=icSigColorantTableType)
        return false;
      else return true;
    }

    // Multi-localized Unicode type tags
  case icSigCopyrightTag:
    {
      if (m_Header.version>=0x04000000L) {
        if (typeSig!=icSigMultiLocalizedUnicodeType)
          return false;
        else return true;
      }
      else {
        if (typeSig!=icSigTextType)
          return false;
        else return true;
      }
    }

  case icSigViewingCondDescTag:
  case icSigDeviceMfgDescTag:
  case icSigDeviceModelDescTag:
  case icSigProfileDescriptionTag:
    {
      if (m_Header.version>=0x04000000L) {
        if (typeSig!=icSigMultiLocalizedUnicodeType)
          return false;
        else return true;
      }
      else {
        if (typeSig!=icSigTextDescriptionType)
          return false;
        else return true;
      }
    }

  case icSigMeasurementTag:
    {
      if (typeSig!=icSigMeasurementType)
        return false;
      else return true;
    }

  case icSigNamedColor2Tag:
    {
      if (typeSig!=icSigNamedColor2Type)
        return false;
      else return true;
    }

  case icSigOutputResponseTag:
    {
      if (typeSig!=icSigResponseCurveSet16Type)
        return false;
      else return true;
    }

  case icSigProfileSequenceDescTag:
    {
      if (typeSig!=icSigProfileSequenceDescType)
        return false;
      else return true;
    }

  case icSigTechnologyTag:
  case icSigPerceptualRenderingIntentGamutTag:
  case icSigSaturationRenderingIntentGamutTag:
    {
      if (typeSig!=icSigSignatureType)
        return false;
      else return true;
    }

  case icSigViewingConditionsTag:
    {
      if (typeSig!=icSigViewingConditionsType)
        return false;
      else return true;
    }

  //The Private Tag case
  default:
    {
      return true;
    }
  }
}


/**
 ****************************************************************************
 * Name: CIccProfile::CheckRequiredTags
 * 
 * Purpose: Check if the Profile has the required tags 
 *  for the specified Profile/Device class.
 * 
 * Return: 
 *  icValidateOK if valid, or other error status.
 *****************************************************************************
 */
icValidateStatus CIccProfile::CheckRequiredTags(std::string &sReport) const
{
  if (m_Tags->size() <= 0) {
    sReport += icValidateCriticalErrorMsg;
    sReport += "No tags present.\n";
    return icValidateCriticalError;
  }

  icValidateStatus rv = icValidateOK;

  if (!GetTag(icSigProfileDescriptionTag) ||
     !GetTag(icSigCopyrightTag)) {
       sReport += icValidateNonCompliantMsg;
       sReport += "Required tags missing.\n";
       rv = icMaxStatus(rv, icValidateNonCompliant);
  }
  
  icProfileClassSignature sig = m_Header.deviceClass;

  if (sig != icSigLinkClass) {
    if (!GetTag(icSigMediaWhitePointTag)) {
      sReport += icValidateCriticalErrorMsg;
      sReport += "Media white point tag missing.\n";
      rv = icMaxStatus(rv, icValidateCriticalError);
    }
  }

  switch(sig) {
    case icSigInputClass:
      if (m_Header.colorSpace == icSigGrayData) {
        if (!GetTag(icSigGrayTRCTag)) {
          sReport += icValidateCriticalErrorMsg;
          sReport += "Gray TRC tag missing.\n";
          rv = icMaxStatus(rv, icValidateCriticalError);
        }
      }
      else {
        if (!GetTag(icSigAToB0Tag)) {
          if (!GetTag(icSigRedMatrixColumnTag) || !GetTag(icSigGreenMatrixColumnTag) ||
             !GetTag(icSigBlueMatrixColumnTag) || !GetTag(icSigRedTRCTag) ||
             !GetTag(icSigGreenTRCTag) || !GetTag(icSigBlueTRCTag)) {
               sReport += icValidateCriticalErrorMsg;
               sReport += "Critical tag(s) missing.\n";
               rv = icMaxStatus(rv, icValidateCriticalError);
             }
        }
      }
      break;

    case icSigDisplayClass:
      if (m_Header.colorSpace == icSigGrayData) {
        if (!GetTag(icSigGrayTRCTag)) {
          sReport += icValidateCriticalErrorMsg;
          sReport += "Gray TRC tag missing.\n";
          rv = icMaxStatus(rv, icValidateCriticalError);
        }
      }
      else {
        if (!GetTag(icSigAToB0Tag) || !GetTag(icSigBToA0Tag)) {
          if (!GetTag(icSigRedMatrixColumnTag) || !GetTag(icSigGreenMatrixColumnTag) ||
             !GetTag(icSigBlueMatrixColumnTag) || !GetTag(icSigRedTRCTag) ||
             !GetTag(icSigGreenTRCTag) || !GetTag(icSigBlueTRCTag)) {
               sReport += icValidateCriticalErrorMsg;
               sReport += "Critical tag(s) missing.\n";
               rv = icMaxStatus(rv, icValidateCriticalError);
             }
        }
      }
      break;

    case icSigOutputClass:
      if (m_Header.colorSpace == icSigGrayData) {
        if (!GetTag(icSigGrayTRCTag)) {
          sReport += icValidateCriticalErrorMsg;
          sReport += "Gray TRC tag missing.\n";
          rv = icMaxStatus(rv, icValidateCriticalError);
        }
      }
      else {
        if (!GetTag(icSigAToB0Tag) || !GetTag(icSigBToA0Tag) ||
           !GetTag(icSigAToB1Tag) || !GetTag(icSigBToA1Tag) ||
           !GetTag(icSigAToB2Tag) || !GetTag(icSigBToA2Tag)) {
             sReport += icValidateCriticalErrorMsg;
             sReport += "Critical tag(s) missing.\n";
             rv = icMaxStatus(rv, icValidateCriticalError);
           }

        if (!GetTag(icSigGamutTag)) {
          sReport += icValidateNonCompliantMsg;
          sReport += "Gamut tag missing.\n";
          rv = icMaxStatus(rv, icValidateNonCompliant);
        }

        if (m_Header.version >= 0x04000000L) {
          switch (m_Header.colorSpace) {
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
            case icSig16colorData:
              if (!GetTag(icSigColorantTableTag)) {
                sReport += icValidateNonCompliantMsg;
                sReport += "xCLR output profile is missing colorantTableTag\n";
                rv = icMaxStatus(rv, icValidateNonCompliant);
              }

            default:
              break;
          }
        }
      }
      break;

    case icSigLinkClass:
      if (!GetTag(icSigAToB0Tag) || !GetTag(icSigProfileSequenceDescTag)) {
        sReport += icValidateCriticalErrorMsg;
        sReport += "Critical tag(s) missing.\n";
        rv = icMaxStatus(rv, icValidateCriticalError);
      }

      if (icIsSpaceCLR(m_Header.colorSpace)) {
        if (!GetTag(icSigColorantTableTag)) {
          sReport += icValidateNonCompliantMsg;
          sReport += "Required tag(s) missing.\n";
          rv = icMaxStatus(rv, icValidateNonCompliant);
        }
      }

      if (icIsSpaceCLR(m_Header.pcs)) {
        if (!GetTag(icSigColorantTableOutTag)) {
          sReport += icValidateNonCompliantMsg;
          sReport += "Required tag(s) missing.\n";
          rv = icMaxStatus(rv, icValidateNonCompliant);
        }
      }
      break;

    case icSigColorSpaceClass:
      if (!GetTag(icSigAToB0Tag) || !GetTag(icSigBToA0Tag)) {
        sReport += icValidateCriticalErrorMsg;
        sReport += "Critical tag(s) missing.\n";
        rv = icMaxStatus(rv, icValidateCriticalError);
      }
      break;

    case icSigAbstractClass:
      if (!GetTag(icSigAToB0Tag)) {
        sReport += icValidateCriticalErrorMsg;
        sReport += "Critical tag(s) missing.\n";
        rv = icMaxStatus(rv, icValidateCriticalError);
      }
      break;

    case icSigNamedColorClass:
      if (!GetTag(icSigNamedColor2Tag)) {
        sReport += icValidateCriticalErrorMsg;
        sReport += "Critical tag(s) missing.\n";
        rv = icMaxStatus(rv, icValidateCriticalError);
      }

      break;

    default:
      sReport += icValidateCriticalErrorMsg;
      sReport += "Unknown Profile Class.\n";
      rv = icMaxStatus(rv, icValidateCriticalError);
      break;
  }

  if (!CheckTagExclusion(sReport)) {
    rv = icMaxStatus(rv, icValidateWarning);
  }

  return rv;
}

/**
 *****************************************************************************
 * Name: CIccProfile::CheckFileSize()
 * 
 * Purpose: Check if the Profile file size matches with the size mentioned
 *  in the header and is evenly divisible by four.
 * 
 * Args: 
 * 
 * Return: 
 *  true - size matches,
 *  false - size mismatches
 ******************************************************************************
 */
bool CIccProfile::CheckFileSize(CIccIO *pIO) const
{
  icUInt32Number FileSize;
  icUInt32Number curPos = pIO->Tell();

  if (!pIO->Seek(0, icSeekEnd))
    return false;

  FileSize = pIO->Tell();

  if (!FileSize)
    return false;

  if (!pIO->Seek(curPos, icSeekSet))
    return false;

  if (FileSize != m_Header.size)
    return false;

  if ((FileSize%4 != 0) || (m_Header.size%4 != 0))
    return false;


  return true;
}


/**
 ****************************************************************************
 * Name: CIccProfile::Validate
 * 
 * Purpose: Check the data integrity of the profile, and conformance to
 *  the ICC specification
 * 
 * Args:
 *  sReport = String to put report into
 * 
 * Return: 
 *  icValidateOK if profile is valid, warning/error level otherwise
 *****************************************************************************
 */
icValidateStatus CIccProfile::Validate(std::string &sReport) const
{
  icValidateStatus rv = icValidateOK;

  //Check header
  rv = icMaxStatus(rv, CheckHeader(sReport));

  // Check for duplicate tags
  if (!AreTagsUnique()) {
    sReport += icValidateWarning;
    sReport += " - There are duplicate tags.\n";
    rv =icMaxStatus(rv, icValidateWarning);
  }

  // Check Required Tags which includes exclusion tests
  rv = icMaxStatus(rv, CheckRequiredTags(sReport));

  // Per Tag tests
  rv = icMaxStatus(rv, CheckTagTypes(sReport));
  TagEntryList::iterator i;
  for (i=m_Tags->begin(); i!=m_Tags->end(); i++) {
    rv = icMaxStatus(rv, i->pTag->Validate(i->TagInfo.sig, sReport, this));
  }

  return rv;
}

/**
 ****************************************************************************
 * Name: CIccProfile::GetSpaceSamples
 * 
 * Purpose: Get the number of device channels from the color space
 *  of data.
 * 
 * Return: Number of device channels.
 *  
 *****************************************************************************
 */
icUInt16Number CIccProfile::GetSpaceSamples() const
{
  switch(m_Header.colorSpace) {
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

  default:
    return 0;
  }

}

//////////////////////////////////////////////////////////////////////
//  Function Definitions
//////////////////////////////////////////////////////////////////////

/**
 *****************************************************************************
 * Name: ReadIccProfile
 * 
 * Purpose: Read an ICC profile file.
 * 
 * Args: 
 *  szFilename - zero terminated string with filename of ICC profile to read 
 * 
 * Return: 
 *  Pointer to icc profile object, or NULL on failure
 ******************************************************************************
 */
CIccProfile* ReadIccProfile(const icChar *szFilename)
{
  CIccFileIO *pFileIO = new CIccFileIO;

  if (!pFileIO->Open(szFilename, "rb")) {
    delete pFileIO;
    return NULL;
  }

  CIccProfile *pIcc = new CIccProfile;

  if (!pIcc->Read(pFileIO)) {
    delete pIcc;
    delete pFileIO;
    return NULL;
  }
  delete pFileIO;

  return pIcc;
}


/**
*****************************************************************************
* Name: ReadIccProfile
* 
* Purpose: Read an ICC profile file.
* 
* Args: 
*  pMem = pointer to memory containing profile data
*  nSize = size of memory related to profile
* 
* Return: 
*  Pointer to icc profile object, or NULL on failure
******************************************************************************
*/
CIccProfile* ReadIccProfile(const icUInt8Number *pMem, icUInt32Number nSize)
{
  CIccMemIO *pMemIO = new CIccMemIO();

  if (!pMemIO->Attach((icUInt8Number*)pMem, nSize)) {
    delete pMemIO;
    return NULL;
  }

  CIccProfile *pIcc = new CIccProfile;

  if (!pIcc->Read(pMemIO)) {
    delete pIcc;
    delete pMemIO;
    return NULL;
  }
  delete pMemIO;

  return pIcc;
}


/**
 ******************************************************************************
 * Name: OpenIccProfile
 * 
 * Purpose: Open an ICC profile file.  This will only read the profile header
 *  and tag directory.  Loading of actual tags will be deferred until the
 *  tags are actually referenced by FindTag().
 * 
 * Args: 
 *  szFilename - zero terminated string with filename of ICC profile to read 
 * 
 * Return: 
 *  Pointer to icc profile object, or NULL on failure
 *******************************************************************************
 */
CIccProfile* OpenIccProfile(const icChar *szFilename)
{
  CIccFileIO *pFileIO = new CIccFileIO;

  if (!pFileIO->Open(szFilename, "rb")) {
    delete pFileIO;
    return NULL;
  }

  CIccProfile *pIcc = new CIccProfile;

  if (!pIcc->Attach(pFileIO)) {
    delete pIcc;
    delete pFileIO;
    return NULL;
  }

  return pIcc;
}

/**
******************************************************************************
* Name: OpenIccProfile
* 
* Purpose: Open an ICC profile file.  This will only read the profile header
*  and tag directory.  Loading of actual tags will be deferred until the
*  tags are actually referenced by FindTag().
* 
* Args: 
*  pMem = pointer to memory containing profile data
*  nSize = size of memory related to profile
* 
* Return: 
*  Pointer to icc profile object, or NULL on failure
*******************************************************************************
*/
CIccProfile* OpenIccProfile(const icUInt8Number *pMem, icUInt32Number nSize)
{
  CIccMemIO *pMemIO = new CIccMemIO;

  if (!pMemIO->Attach((icUInt8Number*)pMem, nSize)) {
    delete pMemIO;
    return NULL;
  }

  CIccProfile *pIcc = new CIccProfile;

  if (!pIcc->Attach(pMemIO)) {
    delete pIcc;
    delete pMemIO;
    return NULL;
  }

  return pIcc;
}

/**
******************************************************************************
* Name: ValidateIccProfile
* 
* Purpose: Open an ICC profile file.  This will only read the profile header
*  and tag directory.  Loading of actual tags will be deferred until the
*  tags are actually referenced by FindTag().
* 
* Args: 
*  szFilename - zero terminated string with filename of ICC profile to read 
* 
* Return: 
*  Pointer to icc profile object, or NULL on failure
*******************************************************************************
*/
CIccProfile* ValidateIccProfile(const icChar *szFilename, std::string &sReport, icValidateStatus &nStatus)
{
  CIccFileIO *pFileIO = new CIccFileIO;

  if (!pFileIO->Open(szFilename, "rb")) {
    sReport = icValidateCriticalErrorMsg;
    sReport += " - ";
    sReport += szFilename;
    sReport += "- Invalid Filename\n";
    delete pFileIO;
    return NULL;
  }

  CIccProfile *pIcc = new CIccProfile;

  if (!pIcc) {
    delete pFileIO;
    return NULL;
  }

  nStatus = pIcc->ReadValidate(pFileIO, sReport);

  if (nStatus>=icValidateCriticalError) {
    delete pIcc;
    delete pFileIO;
    return NULL;
  }

  delete pFileIO;

  nStatus = pIcc->Validate(sReport);

  return pIcc;
}

/**
 ******************************************************************************
 * Name: SaveIccProfile
 * 
 * Purpose: Save an ICC profile file.  
 * 
 * Args: 
 *  szFilename - zero terminated string with filename of ICC profile to create
 * 
 * Return: 
 *  true = success, false = failure
 *******************************************************************************
 */
bool SaveIccProfile(const icChar *szFilename, CIccProfile *pIcc)
{
  CIccFileIO FileIO;

  if (!pIcc)
    return false;

  if (!FileIO.Open(szFilename, "w+b")) {
    return false;
  }

  if (!pIcc->Write(&FileIO)) {
    return false;
  }

  return true;
}

/**
 ****************************************************************************
 * Name: CalcProfileID
 * 
 * Purpose: Calculate the Profile ID using MD5 Fingerprinting method. 
 * 
 * Args: 
 *  pIO = The CIccIO object,
 *  pProfileID = array where the profileID will be stored
 *
 ****************************************************************************
 */
void CalcProfileID(CIccIO *pIO, icProfileID *pProfileID)
{
  icUInt32Number len, num, nBlock, pos;
  MD5_CTX context;
  icUInt8Number buffer[1024];

  //remember where we are
  pos = pIO->Tell();

  //Get length and set up to read entire file
  len = pIO->GetLength();
  pIO->Seek(0, icSeekSet);

  //read file updating checksum as we go
  MD5Init(&context);
  nBlock = 0;
  while(len) {
    num = pIO->Read8(&buffer[0],1024);
    if (!nBlock) {  // Zero out 3 header contents in Profile ID calculation
      memset(buffer+44, 0, 4); //Profile flags
      memset(buffer+64, 0, 4);  //Rendering Intent
      memset(buffer+84, 0, 16); //Profile Id
    }
    MD5Update(&context,buffer,num);
    nBlock++;
    len -=num;
  }
  MD5Final(&pProfileID->ID8[0],&context);

  //go back where we were
  pIO->Seek(pos, icSeekSet);
}

/**
 ****************************************************************************
 * Name: CalcProfileID
 * 
 * Purpose: Calculate the Profile ID using MD5 Fingerprinting method. 
 * 
 * Args: 
 *  szFileName = name of the file whose profile ID has to be calculated,
 *  pProfileID = array where the profileID will be stored
 *****************************************************************************
 */
bool CalcProfileID(const icChar *szFilename, icProfileID *pProfileID)
{
  CIccFileIO FileIO;

  if (!FileIO.Open(szFilename, "rb")) {
    memset(pProfileID, 0, sizeof(icProfileID));
    return false;
  }

  CalcProfileID(&FileIO, pProfileID);
  return true;
}



#ifdef USESAMPLEICCNAMESPACE
} //namespace sampleICC
#endif
