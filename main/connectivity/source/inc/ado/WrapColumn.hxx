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


#ifndef CONNECTIVITY_ADO_WRAP_COLUMN_HXX
#define CONNECTIVITY_ADO_WRAP_COLUMN_HXX

#include "ado/Aolewrap.hxx"

#include "ado_pre_sys_include.h"
// A modern Windows SDK ships the ADO interfaces twice: adoint.h, which has
// dropped the <Enum>_Param typedefs, and adoint_Backcompat.h, which keeps
// them.  This driver uses PositionEnum_Param, so it needs the latter:
//
//     AResultSet.cxx(277): error C2065: 'PositionEnum_Param': undeclared
//
// INSTEAD OF and not in addition to.  Both files open with
//
//     #ifndef _ADOINT_H_
//
// so whichever is included second is skipped entirely -- adding the
// back-compat header after adoint.h looks right and does exactly nothing.
// The two declare the same interfaces (ADOConnection, ADORecordset, ...);
// only the typedefs differ.
//
// Gated on the compiler generation rather than __has_include, which is C++17
// and this build is /std:c++14.  On this branch a UCRT-era compiler always
// pairs with the Windows 10 SDK, and VC9 always with an SDK whose adoint.h
// still carries the typedefs itself.
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <adoint_Backcompat.h>
#else
#include <adoint.h>
#endif
#include "ado_post_sys_include.h"

namespace connectivity
{
	namespace ado
	{
		class WpADOColumn : public WpOLEBase<_ADOColumn>
		{
		public:
			WpADOColumn(_ADOColumn* pInt=NULL)	:	WpOLEBase<_ADOColumn>(pInt){}
			WpADOColumn(const WpADOColumn& rhs){operator=(rhs);}

			void Create();

			inline WpADOColumn& operator=(const WpADOColumn& rhs)
				{WpOLEBase<_ADOColumn>::operator=(rhs); return *this;}

			::rtl::OUString get_Name() const;
			::rtl::OUString get_RelatedColumn() const;
			void put_Name(const ::rtl::OUString& _rName);
			void put_RelatedColumn(const ::rtl::OUString& _rName);
			DataTypeEnum get_Type() const;
			void put_Type(const DataTypeEnum& _eNum) ;
			sal_Int32 get_Precision() const;
			void put_Precision(sal_Int32 _nPre) ;
			sal_uInt8 get_NumericScale() const;
			void put_NumericScale(sal_Int8 _nScale);
			SortOrderEnum get_SortOrder() const;
			void put_SortOrder(SortOrderEnum _nScale);
			sal_Int32 get_DefinedSize() const;
			ColumnAttributesEnum get_Attributes() const;
			sal_Bool put_Attributes(const ColumnAttributesEnum& _eNum);
			WpADOProperties get_Properties() const;
			void put_ParentCatalog(/* [in] */ _ADOCatalog __RPC_FAR *ppvObject);
		};
	}
}

#endif //CONNECTIVITY_ADO_WRAP_COLUMN_HXX
