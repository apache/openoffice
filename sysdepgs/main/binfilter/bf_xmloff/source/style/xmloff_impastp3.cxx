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



#ifndef _XMLOFF_XMLASTPL_IMPL_HXX
#include "impastpl.hxx"
#endif
namespace binfilter {

using namespace std;
using namespace rtl;

//#############################################################################
//
// Class SvXMLAutoStylePoolParent_Impl
//

///////////////////////////////////////////////////////////////////////////////
//
// dtor class SvXMLAutoStylePoolParent_Impl
//

SvXMLAutoStylePoolParentP_Impl::~SvXMLAutoStylePoolParentP_Impl()
{
	while( maPropertiesList.Count() )
		delete maPropertiesList.Remove( maPropertiesList.Count() -1 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Adds a array of XMLPropertyState ( vector< XMLPropertyState > ) to list
// if not added, yet.
//

sal_Bool SvXMLAutoStylePoolParentP_Impl::Add( XMLFamilyData_Impl* pFamilyData, const vector< XMLPropertyState >& rProperties, OUString& rName )
{
	sal_Bool bAdded = sal_False;

	sal_Int32 nProperties = rProperties.size();
	sal_uInt32 nCount = maPropertiesList.Count();
	SvXMLAutoStylePoolPropertiesP_Impl *pProperties = 0;
	sal_uInt32 i;

	for( i=0; i < nCount; i++ )
	{
		SvXMLAutoStylePoolPropertiesP_Impl *pIS = maPropertiesList.GetObject( i );
		if( nProperties > pIS->GetProperties().size() ) 
		{
			continue;
		}
		else if( nProperties < pIS->GetProperties().size() )
		{
			break;
		}
		else if( pFamilyData->mxMapper->Equals( pIS->GetProperties(), rProperties ) ) 
		{
			pProperties = pIS;
			break;
		}
	}
	
	if( !pProperties )
	{
		pProperties = new SvXMLAutoStylePoolPropertiesP_Impl( pFamilyData, rProperties );
		maPropertiesList.Insert( pProperties, i );
		bAdded = sal_True;
	}
	
	rName = pProperties->GetName();

	return bAdded;
}

///////////////////////////////////////////////////////////////////////////////
//
// Search for a array of XMLPropertyState ( vector< XMLPropertyState > ) in list
//

OUString SvXMLAutoStylePoolParentP_Impl::Find( const XMLFamilyData_Impl* pFamilyData, const vector< XMLPropertyState >& rProperties ) const
{
	OUString sName;
	sal_uInt16 nItems = rProperties.size();
	sal_uInt32 nCount = maPropertiesList.Count();
	for( sal_uInt32 i=0; i < nCount; i++ )
	{
		SvXMLAutoStylePoolPropertiesP_Impl *pIS = maPropertiesList.GetObject( i );
		if( nItems > pIS->GetProperties().size() ) 
		{
			continue;
		}
		else if( nItems < pIS->GetProperties().size() )
		{
			break;
		}
		else if( pFamilyData->mxMapper->Equals( pIS->GetProperties(), rProperties ) ) 
		{
			sName = pIS->GetName();
			break;
		}
	}
	
	return sName;
}

///////////////////////////////////////////////////////////////////////////////
//
// Sort-function for sorted list of SvXMLAutoStylePoolParent_Impl-elements
//

int SvXMLAutoStylePoolParentPCmp_Impl( const SvXMLAutoStylePoolParentP_Impl& r1,
									   const SvXMLAutoStylePoolParentP_Impl& r2)
{
	return (int)r1.GetParent().compareTo( r2.GetParent() );
}

///////////////////////////////////////////////////////////////////////////////
//
// Implementation of sorted list of SvXMLAutoStylePoolParent_Impl-elements
//

IMPL_CONTAINER_SORT( SvXMLAutoStylePoolParentsP_Impl, 
					 SvXMLAutoStylePoolParentP_Impl,
				     SvXMLAutoStylePoolParentPCmp_Impl )
}//end of namespace binfilter
