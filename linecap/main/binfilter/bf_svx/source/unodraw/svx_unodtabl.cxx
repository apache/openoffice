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



#ifndef _COM_SUN_STAR_DRAWING_LINEDASH_HPP_
#include <com/sun/star/drawing/LineDash.hpp>
#endif



#include <vector>

#ifndef _SVX_UNONAMEITEMTABLE_HXX_
#include "UnoNameItemTable.hxx"
#endif

#ifndef _SVX_XLNDSIT_HXX
#include "xlndsit.hxx"
#endif


#ifndef _XDEF_HXX
#include "xdef.hxx"
#endif

namespace binfilter {

using namespace ::com::sun::star;
using namespace ::rtl;
using namespace ::cppu;

class SvxUnoDashTable : public SvxUnoNameItemTable
{
public:
	SvxUnoDashTable( SdrModel* pModel ) throw();
	virtual	~SvxUnoDashTable() throw();

	virtual NameOrIndex* createItem() const throw();

    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName(  ) throw( uno::RuntimeException );
    virtual uno::Sequence<  OUString > SAL_CALL getSupportedServiceNames(  ) throw( uno::RuntimeException);

	// XElementAccess
    virtual uno::Type SAL_CALL getElementType(  ) throw( uno::RuntimeException);
};

SvxUnoDashTable::SvxUnoDashTable( SdrModel* pModel ) throw()
: SvxUnoNameItemTable( pModel, XATTR_LINEDASH )
{
}

SvxUnoDashTable::~SvxUnoDashTable() throw()
{
}

OUString SAL_CALL SvxUnoDashTable::getImplementationName() throw( uno::RuntimeException )
{
	return OUString( RTL_CONSTASCII_USTRINGPARAM("SvxUnoDashTable") );
}

uno::Sequence< OUString > SAL_CALL SvxUnoDashTable::getSupportedServiceNames(  )
	throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aSNS( 1 );
    aSNS.getArray()[0] = OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.DashTable" ));
    return aSNS;
}

NameOrIndex* SvxUnoDashTable::createItem() const throw()
{
	XLineDashItem* pNewItem = new XLineDashItem();
	pNewItem->SetWhich( XATTR_LINEDASH ); // set which id for pooling
	return pNewItem;
}

// XElementAccess
uno::Type SAL_CALL SvxUnoDashTable::getElementType(  )
	throw( uno::RuntimeException )
{
	return ::getCppuType((const struct drawing::LineDash*)0);
}

/**
 * Create a gradienttable
 */
uno::Reference< uno::XInterface > SAL_CALL SvxUnoDashTable_createInstance( SdrModel* pModel )
{
	return *new SvxUnoDashTable(pModel);
}



}
