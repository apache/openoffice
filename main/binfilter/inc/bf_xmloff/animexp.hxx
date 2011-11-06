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



#ifndef _XMLOFF_ANIMEXP_HXX
#define _XMLOFF_ANIMEXP_HXX

#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_ 
#include <com/sun/star/drawing/XShape.hpp>
#endif

#ifndef _UNIVERSALL_REFERENCE_HXX
#include <bf_xmloff/uniref.hxx>
#endif
namespace binfilter {

class AnimExpImpl;
class SvXMLExport;
class XMLShapeExport;

class XMLAnimationsExporter : public UniRefBase
{
	AnimExpImpl*	mpImpl;	

public:
	XMLAnimationsExporter( XMLShapeExport* pShapeExp );
	virtual ~XMLAnimationsExporter();

	void prepare( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
	void collect( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
	void exportAnimations( SvXMLExport& rExport );
};

}//end of namespace binfilter
#endif	//  _XMLOFF_ANIMEXP_HXX

