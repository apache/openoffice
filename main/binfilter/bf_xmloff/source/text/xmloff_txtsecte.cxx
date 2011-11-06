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







#include <vector>



 


#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUES_HPP_ 
#include <com/sun/star/beans/PropertyValues.hpp>
#endif



#ifndef _COM_SUN_STAR_TEXT_XTEXTSECTION_HPP_ 
#include <com/sun/star/text/XTextSection.hpp>
#endif








#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif


#ifndef _XMLOFF_XMLTEXTNUMRULEINFO_HXX
#include "XMLTextNumRuleInfo.hxx"
#endif

#ifndef _XMLOFF_XMLSECTIONEXPORT_HXX_
#include "XMLSectionExport.hxx"
#endif

#ifndef _XMLOFF_XMLREDLINEEXPORT_HXX
#include "XMLRedlineExport.hxx"
#endif
#ifndef _XMLOFF_MULTIPROPERTYSETHELPER_HXX
#include "MultiPropertySetHelper.hxx"
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::uno;
using namespace ::std;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::beans::PropertyValue;
using ::com::sun::star::beans::PropertyValues;
using ::com::sun::star::beans::PropertyState;
using ::com::sun::star::container::XIndexReplace;
using ::com::sun::star::container::XNamed;
using ::com::sun::star::lang::XServiceInfo;

void XMLTextParagraphExport::exportListAndSectionChange( 
	Reference<XTextSection> & rPrevSection,
	const Reference<XTextContent> & rNextSectionContent,
	const XMLTextNumRuleInfo& rPrevRule,
	const XMLTextNumRuleInfo& rNextRule,
	sal_Bool bAutoStyles)
{
	Reference<XTextSection> xNextSection;

	// first: get current XTextSection
	Reference<XPropertySet> xPropSet(rNextSectionContent, UNO_QUERY);
	if (xPropSet.is())
	{
		if (xPropSet->getPropertySetInfo()->hasPropertyByName(sTextSection))
		{
			Any aAny = xPropSet->getPropertyValue(sTextSection);
			aAny >>= xNextSection;
		}
		// else: no current section
	}

	exportListAndSectionChange(rPrevSection, xNextSection, 
							   rPrevRule, rNextRule, bAutoStyles);
}

void XMLTextParagraphExport::exportListAndSectionChange( 
	Reference<XTextSection> & rPrevSection,
	MultiPropertySetHelper& rPropSetHelper,
	sal_Int16 nTextSectionId,	
	const Reference<XTextContent> & rNextSectionContent,
	const XMLTextNumRuleInfo& rPrevRule,
	const XMLTextNumRuleInfo& rNextRule,
	sal_Bool bAutoStyles)
{
	Reference<XTextSection> xNextSection;

	// first: get current XTextSection
	Reference<XPropertySet> xPropSet(rNextSectionContent, UNO_QUERY);
	if (xPropSet.is())
	{
		if( !rPropSetHelper.checkedProperties() )
			rPropSetHelper.hasProperties( xPropSet->getPropertySetInfo() );
		if( rPropSetHelper.hasProperty( nTextSectionId ))
		{
			Any aAny = rPropSetHelper.getValue( nTextSectionId , xPropSet,
													sal_True );
			aAny >>= xNextSection;
		}
		// else: no current section
	}

	exportListAndSectionChange(rPrevSection, xNextSection, 
							   rPrevRule, rNextRule, bAutoStyles);
}

void XMLTextParagraphExport::exportListAndSectionChange( 
	Reference<XTextSection> & rPrevSection,
	const Reference<XTextSection> & rNextSection,
	const XMLTextNumRuleInfo& rPrevRule,
	const XMLTextNumRuleInfo& rNextRule,
	sal_Bool bAutoStyles)
{
	// old != new? -> maybe we have to start or end a new section
	if (rPrevSection != rNextSection)
	{
		// a new section started, or an old one gets closed!

		// close old list
		XMLTextNumRuleInfo aEmptyNumRule;
		if ( !bAutoStyles )
			exportListChange(rPrevRule, aEmptyNumRule);

		// Build stacks of old and new sections
		// Sections on top of mute sections should not be on the stack
		vector<Reference<XTextSection> > aOldStack;
		Reference<XTextSection> aCurrent = rPrevSection;
		while(aCurrent.is())
		{
			// if we have a mute section, ignore all its children
			// (all previous ones)
			if (pSectionExport->IsMuteSection(aCurrent))
				aOldStack.clear();

			aOldStack.push_back(aCurrent);
			aCurrent = aCurrent->getParentSection();
		}

		vector<Reference<XTextSection> > aNewStack;
		aCurrent = rNextSection;
		sal_Bool bMute = sal_False;
		while(aCurrent.is())
		{
			// if we have a mute section, ignore all its children
			// (all previous ones)
			if (pSectionExport->IsMuteSection(aCurrent))
			{
				aNewStack.clear();
				bMute = sal_True;
			}

			aNewStack.push_back(aCurrent);
			aCurrent = aCurrent->getParentSection();
		}

		// compare the two stacks
		vector<Reference<XTextSection> > ::reverse_iterator aOld = 
			aOldStack.rbegin();
		vector<Reference<XTextSection> > ::reverse_iterator aNew = 
			aNewStack.rbegin();
		// compare bottom sections and skip equal section
		while ( (aOld != aOldStack.rend()) &&
				(aNew != aNewStack.rend()) &&
				(*aOld) == (*aNew) )
		{
			aOld++;
			aNew++;
		}

		// close all elements of aOld ...
		// (order: newest to oldest)
		if (aOld != aOldStack.rend())
		{
			vector<Reference<XTextSection> > ::iterator aOldForward = 
				aOldStack.begin();
			while ((aOldForward != aOldStack.end()) &&
				   (*aOldForward != *aOld))
			{
				if ( !bAutoStyles && (NULL != pRedlineExport) )
					pRedlineExport->ExportStartOrEndRedline(*aOldForward,
																sal_False);
				pSectionExport->ExportSectionEnd(*aOldForward, bAutoStyles);
				aOldForward++;
			}
			if (aOldForward != aOldStack.end())
			{
				if ( !bAutoStyles && (NULL != pRedlineExport) )
					pRedlineExport->ExportStartOrEndRedline(*aOldForward,
															sal_False);
				pSectionExport->ExportSectionEnd(*aOldForward, bAutoStyles);
			}
		}

		// ...then open all of aNew
		// (order: oldest to newest)
		while (aNew != aNewStack.rend())
		{
            if ( !bAutoStyles && (NULL != pRedlineExport) )
				pRedlineExport->ExportStartOrEndRedline(*aNew, sal_True);
			pSectionExport->ExportSectionStart(*aNew, bAutoStyles);
			aNew++;
		}

		// start new list
		if ( !bAutoStyles && !bMute )
			exportListChange(aEmptyNumRule, rNextRule);
	}
	else
	{
		// list change, if sections have not changed
		if ( !bAutoStyles )
			exportListChange(rPrevRule, rNextRule);
	}

	// save old section (old numRule gets saved in calling method)
	rPrevSection = rNextSection;
}

}//end of namespace binfilter
