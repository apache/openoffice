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



#ifndef _XMLOFF_ATTRLIST_HXX
#define _XMLOFF_ATTRLIST_HXX

#include "sal/config.h"
#include "xmloff/dllapi.h"
#include <com/sun/star/util/XCloneable.hpp>
#include <com/sun/star/xml/sax/SAXParseException.hpp>
#include <com/sun/star/xml/sax/XExtendedDocumentHandler.hpp>
#include <com/sun/star/xml/sax/SAXException.hpp>
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <com/sun/star/xml/sax/XLocator.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>

#include <cppuhelper/implbase3.hxx>

/** Implementation of the XML attribute list.
 *
 * It is based on std::vector.
 */
struct SvXMLAttributeList_Impl;

/** Container for XML tag attributes. */
class XMLOFF_DLLPUBLIC SvXMLAttributeList : public ::cppu::WeakImplHelper3<
		::com::sun::star::xml::sax::XAttributeList,
		::com::sun::star::util::XCloneable,
		::com::sun::star::lang::XUnoTunnel>
{
	SvXMLAttributeList_Impl *m_pImpl;

public:
	SvXMLAttributeList();
	SvXMLAttributeList( const SvXMLAttributeList& );
	SvXMLAttributeList( const ::com::sun::star::uno::Reference< 
		::com::sun::star::xml::sax::XAttributeList> & rAttrList );
	~SvXMLAttributeList();

	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId() throw();
	static SvXMLAttributeList* getImplementation( ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > ) throw();

	// XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);			 

    /** Return the number of attributes.
     *
     * Required by ::com::sun::star::xml::sax::XAttributeList
     */
	virtual sal_Int16 SAL_CALL getLength(void) throw( ::com::sun::star::uno::RuntimeException );
    /** Return the name of the i-th attribute.
     *
     * @param i index starting from zero.
     *
     * @return the name of the i-th attribute, or empty string if i is
     * not a valid index.
     */
	virtual ::rtl::OUString SAL_CALL getNameByIndex(sal_Int16 i) throw( ::com::sun::star::uno::RuntimeException );
    /** Returns the type of the i-th attribute.
     *
     * @param i index starting from zero.
     *
     * @return CDATA for any value of i.
     */
	virtual ::rtl::OUString SAL_CALL getTypeByIndex(sal_Int16 i) throw( ::com::sun::star::uno::RuntimeException );
    /** Returns the type of an attribute.
     *
     * @param aName name of the attribute to look for.
     *
     * @return CDATA for any value of aName.
     */
	virtual ::rtl::OUString SAL_CALL getTypeByName(const ::rtl::OUString& aName) throw( ::com::sun::star::uno::RuntimeException );
    /** Return the value of the i-th attribute.
     *
     * @param i index starting from zero.
     *
     * @return the value of the i-th attribute, or empty string if i is
     * not a valid index.
     */
	virtual ::rtl::OUString SAL_CALL getValueByIndex(sal_Int16 i) throw( ::com::sun::star::uno::RuntimeException );
    /** Return the value of an attribute.
     *
     * @param aName name of the attribute to look for.
     *
     * @return the value of the attribute named aName, or empty string
     * if no attributes are named aName.
     */
	virtual ::rtl::OUString SAL_CALL getValueByName(const ::rtl::OUString& aName) throw( ::com::sun::star::uno::RuntimeException );

	/** Make a clone of this object.
     *
     * Required by ::com::sun::star::util::XCloneable
     *
     * @return a clone of the current object.
     */
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone()	throw( ::com::sun::star::uno::RuntimeException );

	// methods that are not contained in any interface

    /** Add an attribute.
     *
     * @param sName name of the attribute.
     * @param sValue value of the attribute.
     *
     * @exception com::sun::star::uno::RuntimeException thrown if the
     * attribute already exists.
     */
	void AddAttribute( const ::rtl::OUString &sName , const ::rtl::OUString &sValue );
    /** Clear the attributes list. */
	void Clear();
    /** Remove an attribute from the list.
     *
     * @param sName name of the attribute to remove.
     *
     * If no attribute named sName is found, this method does not do anything.
     */
	void RemoveAttribute( const ::rtl::OUString sName );
    /** Reset the attributes list.
     *
     * @param r list to read the attributes from.
     *
     * The internal list is cleared and reset from r.
     */
	void SetAttributeList( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > &r );
    /** Add attributes.
     *
     * @param r attributes to add.
     *
     * @exception com::sun::star::uno::RuntimeException thrown if any
     * of the attributes in r already exists. Previous attributes may
     * have been added.
     */
	void AppendAttributeList( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > &r );
    /** Set the value of the i-th attribute.
     *
     * @param i index starting from zero.
     * @param rValue value to set.
     *
     * If i is out of range, this method does nothing.
     */
	void SetValueByIndex( sal_Int16 i, const ::rtl::OUString& rValue );
    /** Remove the i-th attribute.
     *
     * @param i index starting from zero.
     *
     * If i is out of range, this method does nothing.
     */
	void RemoveAttributeByIndex( sal_Int16 i );
    /** Rename the i-th attribute.
     *
     * @param i index starting from zero.
     * @param rNewName new name to set.
     *
     * If i is out of range, this method does nothing.
     */
	void RenameAttributeByIndex( sal_Int16 i, const ::rtl::OUString& rNewName );
    /** Find an attribute from its name.
     *
     * @param rName attribute name to look for.
     *
     * @return the index of attribute named rName (starting from
     * zero), or -1 if no attributes were found.
     */
	sal_Int16 GetIndexByName( const ::rtl::OUString& rName ) const;

 private:
    /** "CDATA" string */
    const ::rtl::OUString sType;
};


#endif	//  _XMLOFF_ATTRLIST_HXX
