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


#ifndef _CONNECTIVITY_BLOBHELPER_HXX_
#define _CONNECTIVITY_BLOBHELPER_HXX_

#include "connectivity/dbtoolsdllapi.hxx"
#include <com/sun/star/sdbc/XBlob.hpp>
#include <cppuhelper/implbase1.hxx>

namespace connectivity
{
    class OOO_DLLPUBLIC_DBTOOLS BlobHelper : public ::cppu::WeakImplHelper1< com::sun::star::sdbc::XBlob >
    {
        ::com::sun::star::uno::Sequence< sal_Int8 > m_aValue;
    public:
        BlobHelper(const ::com::sun::star::uno::Sequence< sal_Int8 >& _val);
    private:
        virtual ::sal_Int64 SAL_CALL length(  );
        virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL getBytes( ::sal_Int64 pos, ::sal_Int32 length );
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getBinaryStream(  );
        virtual ::sal_Int64 SAL_CALL position( const ::com::sun::star::uno::Sequence< ::sal_Int8 >& pattern, ::sal_Int64 start );
        virtual ::sal_Int64 SAL_CALL positionOfBlob( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XBlob >& pattern, ::sal_Int64 start );
    };
}

#endif //_CONNECTIVITY_BLOBHELPER_HXX_
