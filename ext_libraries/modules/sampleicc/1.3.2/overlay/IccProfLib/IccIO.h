/** @file
    File:       IccIO.h

    Contains:   Implementation of the CIccIO class.

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

#if !defined(_ICCIO_H)
#define _ICCIO_H

#include "IccDefs.h"
#include "memory"
#include "stdio.h"

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

///Seek types
typedef enum {
  icSeekSet=0,  //Seek to an absolute position
  icSeekCur,    //Seek to relative position
  icSeekEnd,     //Seek relative to the ending
} icSeekVal;

/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: 
 *  This is the base object that handles the IO with an ICC profile.
 **************************************************************************
 */
class ICCPROFLIB_API CIccIO  
{
public:

  virtual ~CIccIO() {}

  virtual void Close() {}

  virtual icInt32Number Read8(void *pBuf8, icInt32Number nNum=1) { return 0; }
  virtual icInt32Number Write8(void *pBuf8, icInt32Number nNum=1) { return 0; }

  icInt32Number ReadLine(void *pBuf8, icInt32Number nNum=256);

  icInt32Number Read16(void *pBuf16, icInt32Number nNum=1);
  icInt32Number Write16(void *pBuf16, icInt32Number nNum=1);

  icInt32Number Read32(void *pBuf32, icInt32Number nNum=1);
  icInt32Number Write32(void *pBuf32, icInt32Number nNum=1);

  icInt32Number Read64(void *pBuf64, icInt32Number nNum=1);
  icInt32Number Write64(void *pBuf64, icInt32Number nNum=1);

  icInt32Number Read8Float(void *pBufFloat, icInt32Number nNum=1);
  icInt32Number Write8Float(void *pBuf16, icInt32Number nNum=1);

  icInt32Number Read16Float(void *pBufFloat, icInt32Number nNum=1);
  icInt32Number Write16Float(void *pBuf16, icInt32Number nNum=1);

  icInt32Number ReadFloat32Float(void *pBufFloat, icInt32Number nNum=1);
  icInt32Number WriteFloat32Float(void *pBuf16, icInt32Number nNum=1);

  virtual icInt32Number GetLength() {return 0;}

  virtual icInt32Number Seek(icInt32Number nOffset, icSeekVal pos) {return -1;}
  virtual icInt32Number Tell() {return 0;}

  ///Write operation to make sure that filelength is evenly divisible by 4
  bool Align32(); 

  ///Operation to make sure read position is evenly divisible by 4
  bool Sync32(icUInt32Number nOffset=0); 
};

/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: Handles generic File IO
 **************************************************************************
 */
class ICCPROFLIB_API CIccFileIO : public CIccIO
{
public:
  CIccFileIO();
  virtual ~CIccFileIO();

  bool Open(const icChar *szFilename, const icChar *szAttr);
  virtual void Close();

  virtual icInt32Number Read8(void *pBuf, icInt32Number nNum=1);
  virtual icInt32Number Write8(void *pBuf, icInt32Number nNum=1);

  virtual icInt32Number GetLength();

  virtual icInt32Number Seek(icInt32Number nOffset, icSeekVal pos);
  virtual icInt32Number Tell();

protected:
  FILE *m_fFile;
};

/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: Handles generic memory IO
 **************************************************************************
 */
class ICCPROFLIB_API CIccMemIO : public CIccIO
{
public:
  CIccMemIO();
  virtual ~CIccMemIO();

  bool Alloc(icUInt32Number nSize, bool bWrite = false);

  bool Attach(icUInt8Number *pData, icUInt32Number nSize, bool bWrite=false);
  virtual void Close();

  virtual icInt32Number Read8(void *pBuf, icInt32Number nNum=1);
  virtual icInt32Number Write8(void *pBuf, icInt32Number nNum=1);

  virtual icInt32Number GetLength();

  virtual icInt32Number Seek(icInt32Number nOffset, icSeekVal pos);
  virtual icInt32Number Tell();

  icUInt8Number *GetData() { return m_pData; }

protected:
  icUInt8Number *m_pData;
  icUInt32Number m_nSize;
  icUInt32Number m_nAvail;
  icUInt32Number m_nPos;

  bool m_bFreeData;
};

#ifdef USESAMPLEICCNAMESPACE
} //namespace sampleICC
#endif

#endif // !defined(_ICCIO_H)
