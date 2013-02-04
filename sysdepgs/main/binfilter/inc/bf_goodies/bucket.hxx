/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef _B3D_BUCKET_HXX
#define _B3D_BUCKET_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif

namespace binfilter {

/*************************************************************************
|*
|* Bucket deklarator
|*
\************************************************************************/

#define BASE3D_DECL_BUCKET(TheClassName,TheExtension) \
	SV_DECL_VARARR(TheClassName##TheExtension##MemArr, char*, 32, 32) \
	class TheClassName##TheExtension { \
	private: \
	TheClassName##TheExtension##MemArr	aMemArray; \
		UINT32			nMask; \
		UINT32			nCount; \
		INT16			nFreeMemArray; \
		INT16			nActMemArray; \
		UINT16			nFreeEntry; \
		UINT16			nShift; \
		UINT16			nBlockShift; \
		UINT16			nEntriesPerArray; \
		UINT16			nSlotSize; \
		UINT16			nNext; \
		UINT16			nMemArray; \
	public: \
		TheClassName##TheExtension(UINT16 TheSize); \
		/* Zu verwendende Groesse der Speicherarrays setzen */ \
		/* Bitte NUR verwenden, falls sich der Leerkonstruktor */ \
		/* nicht vermeiden laesst! Nicht nachtraeglich anwenden!  */ \
		void InitializeSize(UINT16 TheSize); \
		/* Destruktor */ \
		~TheClassName##TheExtension(); \
		/* Anhaengen und kopieren */ \
		BOOL Append(TheClassName& rVec) \
			{ if(CareForSpace()) return ImplAppend(rVec); return FALSE; } \
		/* nur neuen Eintrag anhaengen, ohne ausfuellen */ \
		BOOL Append() \
			{ if(CareForSpace()) return ImplAppend(); return FALSE; } \
		/* Letzten Eintrag entfernen */ \
		BOOL Remove() { if(nCount) return ImplRemove(); return FALSE; } \
		/* leeren und Speicher freigeben */ \
		void Empty(); \
		/* leeren aber Speicher behalten */ \
		void Erase(); \
		TheClassName& operator[] (UINT32 nPos); \
		UINT32 Count() { return nCount; } \
		UINT32 GetNumAllocated() { return aMemArray.Count() * nEntriesPerArray; } \
		void operator=(const TheClassName##TheExtension&); \
		UINT16 GetBlockShift() { return nBlockShift; } \
		UINT16 GetSlotSize() { return nSlotSize; } \
	private: \
		BOOL CareForSpace() \
			{ if(nFreeEntry == nEntriesPerArray) \
			return ImplCareForSpace(); return TRUE; } \
		BOOL ImplCareForSpace(); \
		/* Anhaengen und kopieren */ \
		BOOL ImplAppend(TheClassName& rVec); \
		/* nur neuen Eintrag anhaengen, ohne ausfuellen */ \
		BOOL ImplAppend(); \
		BOOL ImplRemove(); \
	};

/*************************************************************************
|*
|* Bucket implementor
|*
\************************************************************************/

#define BASE3D_IMPL_BUCKET(TheClassName,TheExtension) \
	SV_IMPL_VARARR(TheClassName##TheExtension##MemArr, char*) \
	TheClassName##TheExtension::TheClassName##TheExtension(UINT16 TheSize) { \
		InitializeSize(TheSize); \
	} \
	void TheClassName##TheExtension::InitializeSize(UINT16 TheSize) { \
		UINT16 nSiz; \
		for(nShift=0,nSiz=1;nSiz<sizeof(TheClassName);nSiz<<=1,nShift++); \
		nBlockShift = TheSize - nShift; \
		nMask = (1L << nBlockShift)-1L; \
		nSlotSize = 1<<nShift; \
		nEntriesPerArray = (UINT16)((1L << TheSize) >> nShift); \
		Empty(); \
	} \
	void TheClassName##TheExtension::operator=(const TheClassName##TheExtension& rObj) { \
		Erase(); \
		TheClassName##TheExtension& rSrc = (TheClassName##TheExtension&)rObj; \
		for(UINT32 a=0;a<rSrc.Count();a++) \
			Append(rSrc[a]); \
	} \
	void TheClassName##TheExtension::Empty() { \
		for(UINT16 i=0;i<aMemArray.Count();i++) \
			/*#90353#*/ delete [] aMemArray[i]; \
		if(aMemArray.Count()) \
			aMemArray.Remove(0, aMemArray.Count()); \
		nFreeMemArray = 0; \
		nActMemArray = -1; \
		Erase(); \
	} \
	void TheClassName##TheExtension::Erase() { \
		nFreeEntry = nEntriesPerArray; \
		nCount = 0; \
		nActMemArray = -1; \
	} \
	TheClassName##TheExtension::~TheClassName##TheExtension() { \
		Empty(); \
	} \
	BOOL TheClassName##TheExtension::ImplAppend(TheClassName& rVec) { \
		*((TheClassName*)(aMemArray[nActMemArray] + (nFreeEntry++ << nShift))) = rVec; \
		nCount++; \
		return TRUE; \
	} \
	BOOL TheClassName##TheExtension::ImplAppend() { \
		nFreeEntry++; \
		nCount++; \
		return TRUE; \
	} \
	BOOL TheClassName##TheExtension::ImplRemove() { \
		if(nFreeEntry == 1) { \
			nFreeEntry = nEntriesPerArray + 1; \
			if(nActMemArray == -1) \
				return FALSE; \
			nActMemArray--; \
		} \
		nFreeEntry--; \
		nCount--; \
		return TRUE; \
	} \
	BOOL TheClassName##TheExtension::ImplCareForSpace() { \
		/* neues array bestimmem */ \
		if(nActMemArray + 1 < nFreeMemArray) { \
			/* ist scon allokiert, gehe auf naechstes */ \
			nActMemArray++; \
		} else { \
			/* neues muss allokiert werden */ \
			char* pNew = new char[nEntriesPerArray << nShift]; \
			if(!pNew) \
				return FALSE; \
			aMemArray.Insert((const char*&) pNew, aMemArray.Count()); \
			nActMemArray = nFreeMemArray++; \
		} \
		nFreeEntry = 0; \
		return TRUE; \
	} \
	TheClassName& TheClassName##TheExtension::operator[] (UINT32 nPos) { \
		if(nPos >= nCount) { \
			DBG_ERROR("Access to Bucket out of range!"); \
			return *((TheClassName*)aMemArray[0]); \
		} \
		return *((TheClassName*)(aMemArray[(UINT16)(nPos >> nBlockShift)] + ((nPos & nMask) << nShift))); \
	}


}//end of namespace binfilter

#endif          // _B3D_BUCKET_HXX
