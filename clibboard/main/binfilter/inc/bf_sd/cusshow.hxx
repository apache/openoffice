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



#ifndef _SD_CUSSHOW_HXX
#define _SD_CUSSHOW_HXX


#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif


#ifndef _CPPUHELPER_WEAKREF_HXX_
#include <cppuhelper/weakref.hxx>
#endif
namespace binfilter {

class SdDrawDocument;

/*************************************************************************
|*
|* CustomShow
|*
\************************************************************************/
class SdCustomShow : public List
{
private:
	String          aName;
	SdDrawDocument* pDoc;

	// this is a weak reference to a possible living api wrapper for this custom show
	::com::sun::star::uno::WeakReference< ::com::sun::star::uno::XInterface > mxUnoCustomShow;

public:
	SdCustomShow(SdDrawDocument* pDrawDoc);
	SdCustomShow(SdDrawDocument* pDrawDoc, ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xShow );

	SdCustomShow( const SdCustomShow& rShow );
	virtual ~SdCustomShow();

	void   SetName(const String& rName) { aName = rName; }
	String GetName() const              { return aName; }

	SdDrawDocument* GetDoc() const { return pDoc; }

	friend SvStream& operator << (SvStream& rOut, const SdCustomShow& rCustomShow);
	friend SvStream& operator >> (SvStream& rIn, SdCustomShow& rCustomShow);

	::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > getUnoCustomShow();
};

} //namespace binfilter
#endif      // _SD_CUSSHOW_HXX

