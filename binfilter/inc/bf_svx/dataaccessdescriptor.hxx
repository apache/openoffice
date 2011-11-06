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



#ifndef _SVX_DATACCESSDESCRIPTOR_HXX_
#define _SVX_DATACCESSDESCRIPTOR_HXX_

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#include <tools/debug.hxx>
namespace binfilter {
//........................................................................
namespace svx
{
//........................................................................

	class ODADescriptorImpl;

	//====================================================================
	//= DataAccessDescriptorProperty
	//====================================================================
	enum DataAccessDescriptorProperty
	{
		daDataSource,			/// data source name			(string)
		daConnection,			/// connection					(XConnection)

		daCommand,				/// command						(string)
		daCommandType,			/// command type				(long)
		daEscapeProcessing,		/// escape processing			(boolean)
		daFilter,				/// additional filter			(string)
		daCursor,				/// the cursor					(XResultSet)

		daColumnName,			/// column name					(string)
		daColumnObject,			/// column object				(XPropertySet)

		daSelection,			/// selection					(sequence< any >)
		daBookmarkSelection		/// selection are bookmarks?	(boolean)
	};

	//====================================================================
	//= ODataAccessDescriptor
	//====================================================================
	/** class encapsulating the <type scope="com::sun::star::sdb">DataAccessDescriptor</type> service.
	*/
	class ODataAccessDescriptor
	{
	public:
		ODataAccessDescriptor(){DBG_BF_ASSERT(0, "STRIP"); }//STRIP001 ;
		ODataAccessDescriptor( const ODataAccessDescriptor& _rSource ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 ;
		ODataAccessDescriptor( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rValues ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 ;
		ODataAccessDescriptor( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& _rValues ){DBG_BF_ASSERT(0, "STRIP"); }//STRIP001 ;

		// allows to construct a descriptor from an Any containing either an XPropertySet or a property value sequence
		ODataAccessDescriptor( const ::com::sun::star::uno::Any& _rValues ){DBG_BF_ASSERT(0, "STRIP"); }//STRIP001 ;


		~ODataAccessDescriptor(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 
		::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >
			createPropertyValueSequence(){DBG_BF_ASSERT(0, "STRIP");return ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >();} //STRIP001 ;
		::com::sun::star::uno::Any& operator [] ( DataAccessDescriptorProperty _eWhich ){DBG_BF_ASSERT(0, "STRIP"); static ::com::sun::star::uno::Any aDummy; return aDummy;}//STRIP001 ;
	};

//........................................................................
}	// namespace svx
//........................................................................

}//end of namespace binfilter
#endif // _SVX_DATACCESSDESCRIPTOR_HXX_

