/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright IBM Corporation 2010.
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifndef __ACCCOMMON_H_
#define __ACCCOMMON_H_


//using namespace com::sun::star::accessibility;
//using namespace com::sun::star::uno;


//for MAccessible.cpp
struct ltComp
{
    bool operator()(REFGUID  rguid1, REFGUID  rguid2) const
    {
        if(((PLONG) &rguid1)[0] < ((PLONG) &rguid2)[0])
            return TRUE;
        else if(((PLONG) &rguid1)[0] > ((PLONG) &rguid2)[0])
            return FALSE;
        if(((PLONG) &rguid1)[1] < ((PLONG) &rguid2)[1])
            return TRUE;
        else if(((PLONG) &rguid1)[1] > ((PLONG) &rguid2)[1])
            return FALSE;
        if(((PLONG) &rguid1)[2] < ((PLONG) &rguid2)[2])
            return TRUE;
        else if(((PLONG) &rguid1)[2] > ((PLONG) &rguid2)[2])
            return FALSE;
        if(((PLONG) &rguid1)[3] < ((PLONG) &rguid2)[3])
            return TRUE;
        else if(((PLONG) &rguid1)[3] > ((PLONG) &rguid2)[3])
            return FALSE;
        return FALSE;
    }
};

#define BEGIN_AGGOBJECT_MAP(x) \
	static _UNO_AGGMAP_ENTRY* WINAPI _GetAggEntries() { \
	static _UNO_AGGMAP_ENTRY _aggentries[] = {
#define END_AGGOBJECT_MAP()   {NULL, NULL,NULL}}; \
	return _aggentries;}
#define AGGOBJECT_ENTRY(iid, clsid, ifindex) {&iid, \
	CComAggregateCreator< CComObject<CMAccessible>, &clsid >::CreateInstance,\
	XI_##ifindex},

enum DM_NIR {
    DM_FIRSTCHILD = 0x00,
    DM_LASTCHILD = 0x01,
    DM_NEXTCHILD = 0x02,
    DM_PREVCHILD = 0x03
};

inline BOOL ImplIsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
    return (
               ((PLONG) &rguid1)[0] == ((PLONG) &rguid2)[0] &&
               ((PLONG) &rguid1)[1] == ((PLONG) &rguid2)[1] &&
               ((PLONG) &rguid1)[2] == ((PLONG) &rguid2)[2] &&
               ((PLONG) &rguid1)[3] == ((PLONG) &rguid2)[3]);
}
#define InlineIsEqualGUID ImplIsEqualGUID
//End

//for AccAction.cpp
#define CODEENTRY(key) \
	{KEYCODE_##key, L#key}
#define countof(x)	(sizeof(x)/sizeof(x[0]))

#define SELECT       L"Select"
#define PRESS        L"Press"
#define UNCHECK      L"UnCheck"
#define CHECK        L"Check"
//End

static DWORD GetMSAAStateFromUNO(short xState);

#endif
