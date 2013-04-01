/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright IBM Corporation 2009.
 * Copyright 2009. by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: nbdtmgfact.hxx,v $
 * $Revision: 1.00.00.0 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

/********************************************************************************
 *
 *	Tool to Visualize Internal Data Structure for Productive Development
 *	It is a developer's tool from Lotus Symphony Documents Editor Team
 *
 *	This file implements the base abstract exporter and 2 inherited sub-classes for 
 *	the internal data structure tracer. 
 *
 *	Date: 05/25/2009
 *
 ********************************************************************************/

#ifndef _NBDTMGFACT_HXX
#define _NBDTMGFACT_HXX
#ifndef _NBDTMG_HXX
#include "nbdtmg.hxx"
#endif
namespace svx { namespace sidebar {
class SVX_DLLPUBLIC NBOutlineTypeMgrFact
{
public:
	static NBOTypeMgrBase* CreateInstance(const NBOType aType);
	NBOutlineTypeMgrFact();
	virtual ~NBOutlineTypeMgrFact() {}
};
}}
#endif

