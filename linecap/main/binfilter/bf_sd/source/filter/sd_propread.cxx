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



#ifndef _PROPREAD_HXX_
#include <propread.hxx>
#endif
#include "rtl/tencinfo.h"
#include "rtl/textenc.h"
namespace binfilter {

// ------------------------------------------------------------------------

PropItem& PropItem::operator=( PropItem& rPropItem )
{
	if ( this != &rPropItem )
	{
		Seek( STREAM_SEEK_TO_BEGIN );
		delete[] SwitchBuffer();

		mnTextEnc = rPropItem.mnTextEnc;
		sal_uInt32 nPos = rPropItem.Tell();
		rPropItem.Seek( STREAM_SEEK_TO_END );
		SvMemoryStream::Write( rPropItem.GetData(), rPropItem.Tell() );
		rPropItem.Seek( nPos );
	}
	return *this;
}

//	-----------------------------------------------------------------------

struct Dict
{
	sal_uInt32	mnId;
	String		aString;

			Dict( sal_uInt32 nId, String rString ) { mnId = nId; aString = rString; };
};

//	-----------------------------------------------------------------------

Dictionary::~Dictionary()
{
	for ( void* pPtr = First(); pPtr; pPtr = Next() )
		delete (Dict*)pPtr;
}

//	-----------------------------------------------------------------------

Dictionary& Dictionary::operator=( Dictionary& rDictionary )
{
	if ( this != &rDictionary )
	{
		void* pPtr; for ( pPtr = First(); pPtr; pPtr = Next() )
			delete (Dict*)pPtr;

		for ( pPtr = rDictionary.First(); pPtr; pPtr = rDictionary.Next() )
			Insert( new Dict( ((Dict*)pPtr)->mnId, ((Dict*)pPtr)->aString ), LIST_APPEND );
	}
	return *this;
}
}
