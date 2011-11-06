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



#ifndef _SC_XMLDETECTIVECONTEXT_HXX
#define _SC_XMLDETECTIVECONTEXT_HXX

#ifndef _XMLOFF_XMLIMP_HXX
#include <bf_xmloff/xmlimp.hxx>
#endif

#ifndef _COM_SUN_STAR_TABLE_CELLRANGEADDRESS_HPP_
#include <com/sun/star/table/CellRangeAddress.hpp>
#endif

#ifndef SC_DETFUNC_HXX
#include "detfunc.hxx"
#endif
#ifndef SC_DETDATA_HXX
#include "detdata.hxx"
#endif

#ifndef __SGI_STL_LIST
#include <list>
#endif
namespace binfilter {

class ScXMLImport;


//___________________________________________________________________

struct ScMyImpDetectiveObj
{
	ScRange						aSourceRange;
	ScDetectiveObjType			eObjType;
	sal_Bool					bHasError : 1;

								ScMyImpDetectiveObj();
};

typedef ::std::vector< ScMyImpDetectiveObj > ScMyImpDetectiveObjVec;


//___________________________________________________________________

struct ScMyImpDetectiveOp
{
	ScAddress					aPosition;
	ScDetOpType					eOpType;
	sal_Int32					nIndex;

	inline						ScMyImpDetectiveOp() : nIndex( -1 )	{}
	sal_Bool					operator<(const ScMyImpDetectiveOp& rDetOp) const;
};

typedef ::std::list< ScMyImpDetectiveOp > ScMyImpDetectiveOpList;

class ScMyImpDetectiveOpArray
{
private:
	ScMyImpDetectiveOpList		aDetectiveOpList;

public:
	inline						ScMyImpDetectiveOpArray() :
									aDetectiveOpList()	{}

	inline void					AddDetectiveOp( const ScMyImpDetectiveOp& rDetOp )
									{ aDetectiveOpList.push_back( rDetOp ); }

	void						Sort();
	sal_Bool					GetFirstOp( ScMyImpDetectiveOp& rDetOp );
};


//___________________________________________________________________

class ScXMLDetectiveContext : public SvXMLImportContext
{
private:
	ScMyImpDetectiveObjVec*		pDetectiveObjVec;

	const ScXMLImport&			GetScImport() const	{ return (const ScXMLImport&)GetImport(); }
	ScXMLImport&				GetScImport()		{ return (ScXMLImport&)GetImport(); }

public:
								ScXMLDetectiveContext(
									ScXMLImport& rImport,
									USHORT nPrfx,
									const ::rtl::OUString& rLName,
									ScMyImpDetectiveObjVec* pNewDetectiveObjVec
									);
	virtual						~ScXMLDetectiveContext();

	virtual SvXMLImportContext*	CreateChildContext(
									USHORT nPrefix,
									const ::rtl::OUString& rLocalName,
									const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
									);
	virtual void				EndElement();
};


//___________________________________________________________________

class ScXMLDetectiveHighlightedContext : public SvXMLImportContext
{
private:
	ScMyImpDetectiveObjVec*		pDetectiveObjVec;
	ScMyImpDetectiveObj			aDetectiveObj;
	sal_Bool					bValid : 1;

	const ScXMLImport&			GetScImport() const	{ return (const ScXMLImport&)GetImport(); }
	ScXMLImport&				GetScImport()		{ return (ScXMLImport&)GetImport(); }

public:
								ScXMLDetectiveHighlightedContext(
									ScXMLImport& rImport,
									USHORT nPrfx,
									const ::rtl::OUString& rLName,
									const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList,
									ScMyImpDetectiveObjVec* pNewDetectiveObjVec
									);
	virtual						~ScXMLDetectiveHighlightedContext();

	virtual SvXMLImportContext*	CreateChildContext(
									USHORT nPrefix,
									const ::rtl::OUString& rLocalName,
									const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
									);
	virtual void				EndElement();
};


//___________________________________________________________________

class ScXMLDetectiveOperationContext : public SvXMLImportContext
{
private:
	ScMyImpDetectiveOp			aDetectiveOp;
	sal_Bool					bHasType : 1;

	const ScXMLImport&			GetScImport() const	{ return (const ScXMLImport&)GetImport(); }
	ScXMLImport&				GetScImport()		{ return (ScXMLImport&)GetImport(); }

public:
								ScXMLDetectiveOperationContext(
									ScXMLImport& rImport,
									USHORT nPrfx,
									const ::rtl::OUString& rLName,
									const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
									);
	virtual						~ScXMLDetectiveOperationContext();

	virtual SvXMLImportContext*	CreateChildContext(
									USHORT nPrefix,
									const ::rtl::OUString& rLocalName,
									const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
									);
	virtual void				EndElement();
};


} //namespace binfilter
#endif

