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


#ifndef _XMLOFF_STYLEEXP_HXX_
#define _XMLOFF_STYLEEXP_HXX_

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _UNIVERSALL_REFERENCE_HXX
#include <bf_xmloff/uniref.hxx>
#endif

namespace com { namespace sun { namespace star
{
	namespace style
	{
		class XStyle;
	}
} } }
namespace binfilter {

class XMLPropertySetMapper;
class SvXMLExportPropertyMapper;
class SvXMLAutoStylePoolP;
class SvXMLExport;

class XMLStyleExport : public UniRefBase
{
	SvXMLExport& rExport;
protected:
	const ::rtl::OUString sIsPhysical;
	const ::rtl::OUString sIsAutoUpdate;
	const ::rtl::OUString sFollowStyle;
	const ::rtl::OUString sNumberingStyleName;

	SvXMLExport& GetExport() { return rExport; }
	const SvXMLExport& GetExport() const  { return rExport; }

private:

	const ::rtl::OUString sPoolStyleName;

	SvXMLAutoStylePoolP *pAutoStylePool;

protected:

	virtual sal_Bool exportStyle(
		const ::com::sun::star::uno::Reference<
				::com::sun::star::style::XStyle > & rStyle,
		const ::rtl::OUString& rXMLFamily,
		const UniReference < SvXMLExportPropertyMapper >& rPropMapper,
		const ::rtl::OUString* pPrefix = 0L );

	virtual void exportStyleAttributes(
		const ::com::sun::star::uno::Reference<
				::com::sun::star::style::XStyle > & rStyle );

	virtual void exportStyleContent(
		const ::com::sun::star::uno::Reference<
				::com::sun::star::style::XStyle > & rStyle );
public:
	XMLStyleExport(
		SvXMLExport& rExp,
		const ::rtl::OUString& rPoolStyleName,
		SvXMLAutoStylePoolP *pAutoStyleP=0 );
	virtual ~XMLStyleExport();

//	void exportStyleFamily(
//		const ::rtl::OUString& rFamily, const ::rtl::OUString& rXMLFamily,
//		const UniReference < XMLPropertySetMapper >& rPropMapper,
//		sal_Bool bUsed, sal_uInt16 nFamily = 0,
//		const ::rtl::OUString* pPrefix = 0L);

//	void exportStyleFamily(
//		const sal_Char *pFamily, const ::rtl::OUString& rXMLFamily,
//		const UniReference < XMLPropertySetMapper >& rPropMapper,
//		sal_Bool bUsed, sal_uInt16 nFamily = 0,
//		const ::rtl::OUString* pPrefix = 0L);

	virtual sal_Bool exportDefaultStyle(
		const ::com::sun::star::uno::Reference<
				::com::sun::star::beans::XPropertySet > & xPropSet,
		const ::rtl::OUString& rXMLFamily,
		const UniReference < SvXMLExportPropertyMapper >& rPropMapper );

	void exportStyleFamily(
		const ::rtl::OUString& rFamily, const ::rtl::OUString& rXMLFamily,
		const UniReference < SvXMLExportPropertyMapper >& rPropMapper,
		sal_Bool bUsed, sal_uInt16 nFamily = 0,
		const ::rtl::OUString* pPrefix = 0L);

	void exportStyleFamily(
		const sal_Char *pFamily, const ::rtl::OUString& rXMLFamily,
		const UniReference < SvXMLExportPropertyMapper >& rPropMapper,
		sal_Bool bUsed, sal_uInt16 nFamily = 0,
		const ::rtl::OUString* pPrefix = 0L);
};

}//end of namespace binfilter
#endif
