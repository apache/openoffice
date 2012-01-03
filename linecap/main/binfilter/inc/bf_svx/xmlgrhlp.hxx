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



#ifndef _XMLGRHLP_HXX
#define _XMLGRHLP_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _CPPUHELPER_COMPBASE2_HXX_
#include <cppuhelper/compbase2.hxx>
#endif

#include <bf_so3/svstor.hxx>

#ifndef _BF_GOODIES_GRAPHICOBJECT_HXX
#include <bf_goodies/graphicobject.hxx>
#endif
#include <vector>
#include <set>
#include <utility>

#ifndef _COM_SUN_STAR_DOCUMENT_XGRAPHICOBJECTRESOLVER_HPP_
#include <com/sun/star/document/XGraphicObjectResolver.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XBINARYSTREAMRESOLVER_HPP_
#include <com/sun/star/document/XBinaryStreamResolver.hpp>
#endif
class SotStorage;
namespace binfilter {

// ----------------------
// - SvXMLGraphicHelper -
// ----------------------

enum SvXMLGraphicHelperMode
{
	GRAPHICHELPER_MODE_READ = 0,
	GRAPHICHELPER_MODE_WRITE = 1
};

// ----------------------
// - SvXMLGraphicHelper -
// ----------------------


class SvXMLGraphicHelper : public ::cppu::WeakComponentImplHelper2<	::com::sun::star::document::XGraphicObjectResolver,
                                                                    ::com::sun::star::document::XBinaryStreamResolver >
{
private:

	typedef ::std::pair< ::rtl::OUString, ::rtl::OUString >	                                            URLPair;
	typedef ::std::vector< URLPair >							                                        URLPairVector;
	typedef ::std::vector< BfGraphicObject >						                                        GraphicObjectVector;
	typedef ::std::set< ::rtl::OUString >						                                        URLSet;
    typedef ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > >    GraphicOutputStreamVector;

	::osl::Mutex				maMutex;
	SotStorage*					mpRootStorage;
	SotStorageRef				mxGraphicStorage;
	::rtl::OUString				maCurStorageName;
	URLPairVector				maGrfURLs;
	GraphicObjectVector			maGrfObjs;
    GraphicOutputStreamVector   maGrfStms;
	URLSet						maURLSet;
	SvXMLGraphicHelperMode		meCreateMode;
	sal_Bool                    mbDirect;

	sal_Bool					ImplGetStreamNames( const ::rtl::OUString& rURLStr, 
												    ::rtl::OUString& rPictureStorageName, 
												    ::rtl::OUString& rPictureStreamName );
	SotStorageRef				ImplGetGraphicStorage( const ::rtl::OUString& rPictureStorageName );
	SotStorageStreamRef			ImplGetGraphicStream( const ::rtl::OUString& rPictureStorageName, 
													  const ::rtl::OUString& rPictureStreamName, 
													  BOOL bTruncate );
    String                      ImplGetGraphicMimeType( const String& rFileName ) const;
	Graphic						ImplReadGraphic( const ::rtl::OUString& rPictureStorageName, 
												 const ::rtl::OUString& rPictureStreamName );
	sal_Bool					ImplWriteGraphic( const ::rtl::OUString& rPictureStorageName, 
												  const ::rtl::OUString& rPictureStreamName,
												  const ::rtl::OUString& rGraphicId );
	void						ImplInsertGraphicURL( const ::rtl::OUString& rURLStr, sal_uInt32 nInsertPos );

protected:						
								SvXMLGraphicHelper();							
								~SvXMLGraphicHelper();
	void						Init( SotStorage* pXMLStorage, 
									  SvXMLGraphicHelperMode eCreateMode,
									  BOOL bDirect );

    virtual void SAL_CALL		disposing();

public:							
								SvXMLGraphicHelper( SvXMLGraphicHelperMode eCreateMode );

	static SvXMLGraphicHelper*	Create( SotStorage& rXMLStorage, 
										SvXMLGraphicHelperMode eCreateMode,
										BOOL bDirect = TRUE );
	static SvXMLGraphicHelper*	Create( SvXMLGraphicHelperMode eCreateMode );

	static void					Destroy( SvXMLGraphicHelper* pSvXMLGraphicHelper );

	void						Flush();

public:

	// XGraphicObjectResolver
    virtual ::rtl::OUString SAL_CALL resolveGraphicObjectURL( const ::rtl::OUString& aURL ) throw(::com::sun::star::uno::RuntimeException);

    // XBinaryStreamResolver
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getInputStream( const ::rtl::OUString& rURL ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > SAL_CALL createOutputStream(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL resolveOutputStream( const ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream >& rxBinaryStream ) throw (::com::sun::star::uno::RuntimeException);
};

}//end of namespace binfilter
#endif
