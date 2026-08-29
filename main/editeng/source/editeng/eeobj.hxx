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



#ifndef _DATAOBJ_HXX
#define _DATAOBJ_HXX

#include <cppuhelper/weak.hxx>
#include <com/sun/star/datatransfer/XTransferable.hpp>

#include <tools/stream.hxx>

class EditDataObject :	public ::com::sun::star::datatransfer::XTransferable,
						public ::cppu::OWeakObject

{
private:
	SvMemoryStream	maBinData;
	SvMemoryStream	maRTFData;
	String			maText;

	String			maOfficeBookmark;

//	String			maNetscapeBookmark;
//	SvMemoryStream	maRTFData;

public:
					EditDataObject();
					~EditDataObject();

	SvMemoryStream&	GetStream() { return maBinData; }
	SvMemoryStream&	GetRTFStream() { return maRTFData; }
	String&			GetString()	{ return maText; }
	String&			GetURL()	{ return maOfficeBookmark; }


	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }

	// ::com::sun::star::datatransfer::XTransferable
    ::com::sun::star::uno::Any SAL_CALL getTransferData( const ::com::sun::star::datatransfer::DataFlavor& aFlavor );
    ::com::sun::star::uno::Sequence< ::com::sun::star::datatransfer::DataFlavor > SAL_CALL getTransferDataFlavors(  );
    sal_Bool SAL_CALL isDataFlavorSupported( const ::com::sun::star::datatransfer::DataFlavor& aFlavor );
};

#endif	// _DATAOBJ_HXX
