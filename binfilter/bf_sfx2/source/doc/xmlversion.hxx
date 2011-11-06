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


#ifndef _SFX_XMLVERSION_HXX
#define _SFX_XMLVERSION_HXX

#ifndef _SVSTOR_HXX 
#include <bf_so3/svstor.hxx>
#endif


//#ifndef _XMLOFF_XMLITMAP_HXX
//#include <bf_xmloff/xmlitmap.hxx>
//#endif

#ifndef _XMLOFF_XMLEXP_HXX
#include <bf_xmloff/xmlexp.hxx>
#endif

#ifndef _XMLOFF_XMLIMP_HXX
#include <bf_xmloff/xmlimp.hxx>
#endif



class DateTime;
namespace binfilter {

class SfxVersionTableDtor;

// ------------------------------------------------------------------------
class SfxXMLVersListImport_Impl : public SvXMLImport
{
private:
	SfxVersionTableDtor *mpVersions;

protected:

	// This method is called after the namespace map has been updated, but
	// before a context for the current element has been pushed.
	virtual SvXMLImportContext *CreateContext( sal_uInt16 nPrefix,
					const ::rtl::OUString& rLocalName,
					const ::com::sun::star::uno::Reference<
					::com::sun::star::xml::sax::XAttributeList > & xAttrList );

public:

	// #110680#
	SfxXMLVersListImport_Impl( 
		const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
		SfxVersionTableDtor *pVersions );
	~SfxXMLVersListImport_Impl() throw();

	SfxVersionTableDtor*	 GetList() { return mpVersions; }
};

// ------------------------------------------------------------------------
class SfxXMLVersListContext_Impl : public SvXMLImportContext
{
private:
	SfxXMLVersListImport_Impl & rLocalRef;

public:

	SfxXMLVersListContext_Impl( SfxXMLVersListImport_Impl& rImport,
						   sal_uInt16 nPrefix, 
						   const ::rtl::OUString& rLocalName,
						   const ::com::sun::star::uno::Reference< 
						   ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

	~SfxXMLVersListContext_Impl();

	virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
						   const ::rtl::OUString& rLocalName,
						   const ::com::sun::star::uno::Reference< 
						   ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

};

// ------------------------------------------------------------------------
class SfxXMLVersionContext_Impl : public SvXMLImportContext
{
private:
	SfxXMLVersListImport_Impl&	rLocalRef;

	static sal_Bool			ParseISODateTimeString(
								const ::rtl::OUString& rString,
								DateTime& rDateTime );

public:
	
	SfxXMLVersionContext_Impl( SfxXMLVersListImport_Impl& rImport,
						  sal_uInt16 nPrefix, 
						  const ::rtl::OUString& rLocalName,
						  const ::com::sun::star::uno::Reference< 
						  ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

	~SfxXMLVersionContext_Impl();
};


// ------------------------------------------------------------------------
class SfxXMLVersList_Impl
{
public:
	static sal_Bool	ReadInfo( SvStorageRef xRoot, SfxVersionTableDtor *pList );
};



}//end of namespace binfilter
#endif
