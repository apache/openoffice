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

#ifndef __ACCOBJECTMANAGERAGENT_HXX
#define __ACCOBJECTMANAGERAGENT_HXX

#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_HPP_
#include <com/sun/star/accessibility/XAccessible.hpp>
#endif

struct IMAccessible;
struct IAccessible;
class AccObjectWinManager;

/****************************************************************
AccObjectMangerAgent is used for manaing AccObjects,which encapsulates
platform differences,and call AccObjectWinManager for Windows platform. To do for
Linux platform
*****************************************************************/
class AccObjectManagerAgent
{
private:

    AccObjectWinManager* pWinManager;

public:

    AccObjectManagerAgent();
    virtual ~AccObjectManagerAgent();

    virtual unsigned char InsertAccObj( com::sun::star::accessibility::XAccessible* pXAcc,
                                        com::sun::star::accessibility::XAccessible* pParentXAcc,
                                        long pWnd=0);
    virtual void GetIAccessibleFromResID(long childID,IMAccessible**);
    virtual unsigned char GetIAccessibleFromXAccessible(com::sun::star::accessibility::XAccessible* pXAcc, IAccessible** ppIA);

    virtual void DeleteAccObj( com::sun::star::accessibility::XAccessible* pXAcc );
    virtual IMAccessible*  GetIMAccByXAcc(com::sun::star::accessibility::XAccessible* pXAcc);

    unsigned char NotifyAccEvent( short pEvent = 0, com::sun::star::accessibility::XAccessible* pXAcc = 0 );

    unsigned char InsertChildrenAccObj( com::sun::star::accessibility::XAccessible* pXAcc,
                                        long pWnd=0);
    void DeleteChildrenAccObj( com::sun::star::accessibility::XAccessible* pXAcc );

    void  DecreaseState( com::sun::star::accessibility::XAccessible* pXAcc,unsigned short pState );
    void  IncreaseState( com::sun::star::accessibility::XAccessible* pXAcc,unsigned short pState );
    void  UpdateState( com::sun::star::accessibility::XAccessible* pXAcc );

    void  UpdateLocation( com::sun::star::accessibility::XAccessible* pXAcc,
                          long Top = 0,long left = 0,long width = 0,long height = 0 );
    void  UpdateAction( com::sun::star::accessibility::XAccessible* pXAcc );

    void  UpdateValue( com::sun::star::accessibility::XAccessible* pXAcc );
    void  UpdateValue( com::sun::star::accessibility::XAccessible* pXAcc, com::sun::star::uno::Any );

    void  UpdateAccName( com::sun::star::accessibility::XAccessible* pXAcc,	com::sun::star::uno::Any newName);
    void  UpdateAccName( com::sun::star::accessibility::XAccessible* pXAcc);

    void  UpdateDescription( com::sun::star::accessibility::XAccessible* pXAcc,	com::sun::star::uno::Any newDesc );
    void  UpdateDescription( com::sun::star::accessibility::XAccessible* pXAcc );

    void NotifyDestroy(com::sun::star::accessibility::XAccessible* pXAcc);

    com::sun::star::accessibility::XAccessible* GetParentXAccessible(
        com::sun::star::accessibility::XAccessible* pXAcc );
    short GetParentRole(com::sun::star::accessibility::XAccessible* pXAcc );
    unsigned short IsContainer(com::sun::star::accessibility::XAccessible* pXAcc);

    void SaveTopWindowHandle(long hWnd, com::sun::star::accessibility::XAccessible* pXAcc);

    void UpdateChildState(com::sun::star::accessibility::XAccessible* pXAcc);

    bool IsSpecialToolboItem(com::sun::star::accessibility::XAccessible* pXAcc);

    short GetRole(com::sun::star::accessibility::XAccessible* pXAcc);

    com::sun::star::accessibility::XAccessible* GetAccDocByAccTopWin( com::sun::star::accessibility::XAccessible* pXAcc );
    bool IsTopWinAcc( com::sun::star::accessibility::XAccessible* pXAcc );

    bool IsStateManageDescendant(com::sun::star::accessibility::XAccessible* pXAcc);
};

#endif
