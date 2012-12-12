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

#ifndef __ACCOBJECTWINMANAGER_HXX
#define __ACCOBJECTWINMANAGER_HXX

#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_HPP_
#include <com/sun/star/accessibility/XAccessible.hpp>
#endif
#include <map>
#include <windows.h>
#include <vos/mutex.hxx>
#include "ResIDGenerator.hxx"
#include "UAccCOM2.h"

class ResIDGenerator;
class AccObjectManagerAgent;
class AccEventListener;
class AccObject;

/*******************************************************************
AccObjectWinManager complete the functions:
1. Insert, delete,query,update Acc objects
2. Create, delete,fire AccEventLister, the lifecycle of AccEventListener is as same as 
   Acc Object
 3.Return COM interface for AT,by the call back in salframe
 4.Pass accessible information to Acc objects
 *******************************************************************/
class AccObjectWinManager
{
    friend class AccObjectManagerAgent;

private:
    struct ltstr1
    {
        bool operator()(const void*  s1, const void*  s2) const
        {
            return long(s1)<long(s2);
        }
    };
    struct ltstr2
    {
        bool operator()(const HWND  s1, const HWND  s2) const
        {
            return long(s1)<long(s2);
        }
    };
    struct ltstr3
    {
        bool operator()(const long  s1, const long  s2) const
        {
            return long(s1)<long(s2);
        }
    };
    typedef std::map< void*, AccObject,ltstr1 > XIdToAccObjHash;
    typedef std::map< HWND,void*,ltstr2 > XHWNDToXAccHash;
    typedef std::map< const long, AccObject*,ltstr3 > XResIdToAccObjHash;

    typedef std::map< const long, com::sun::star::accessibility::XAccessible*,ltstr3 > XHWNDToDocumentHash;


    //XAccessible to AccObject
    XIdToAccObjHash  XIdAccList;

    //HWND to All XAccessible pointer
    XHWNDToXAccHash  HwndXAcc;

    //id_Child to AccObject
    XResIdToAccObjHash XResIdAccList;

    //for file name support
    XHWNDToDocumentHash XHWNDDocList;

    com::sun::star::accessibility::XAccessible* oldFocus;

    AccObjectManagerAgent*	 pAgent;
    static AccObjectWinManager* me;
    ResIDGenerator ResIdGen;
    mutable ::vos::OMutex aDeleteMutex;
    mutable ::vos::OMutex aNotifyMutex;
    mutable ::vos::OMutex maATInterfaceMutex;

    AccObjectWinManager(AccObjectManagerAgent* Agent=NULL);
	bool m_bBridgeRegistered;

private:
    long ImpleGenerateResID();
    AccObject* GetAccObjByXAcc( com::sun::star::accessibility::XAccessible* pXAcc);

    com::sun::star::accessibility::XAccessible* GetXAccByAccObj(AccObject* pAccObj);

    AccObject* GetTopWindowAccObj(HWND hWnd);

    com::sun::star::accessibility::XAccessible* GetAccDocByHWND( long pWnd );

    void       DeleteAccListener( AccObject* pAccObj );
    void       InsertAccChildNode(AccObject* pCurObj,AccObject* pParentObj,HWND pWnd);
    void       DeleteAccChildNode(AccObject* pChild);
    void       DeleteFromHwndXAcc(com::sun::star::accessibility::XAccessible* pXAcc );
    int  UpdateAccSelection(com::sun::star::accessibility::XAccessible* pXAcc);

    AccEventListener* createAccEventListener(com::sun::star::accessibility::XAccessible* pXAcc, AccObjectManagerAgent* Agent);
public:
    virtual ~AccObjectWinManager();
    sal_Bool InsertAccObj( com::sun::star::accessibility::XAccessible* pXAcc,com::sun::star::accessibility::XAccessible* pParentXAcc,HWND pWnd);
    sal_Bool InsertChildrenAccObj( com::sun::star::accessibility::XAccessible* pXAcc,HWND pWnd=0);
    void DeleteAccObj( com::sun::star::accessibility::XAccessible* pXAcc );
    void DeleteChildrenAccObj(com::sun::star::accessibility::XAccessible* pAccObj);

public:
    static  AccObjectWinManager* CreateAccObjectWinManagerInstance(AccObjectManagerAgent* Agent);

public:

    sal_Bool NotifyAccEvent( com::sun::star::accessibility::XAccessible* pXAcc,short state = 0 );

    long Get_ToATInterface( HWND hWnd, long lParam, long wParam);

    void  DecreaseState( com::sun::star::accessibility::XAccessible* pXAcc,unsigned short pState );
    void  IncreaseState( com::sun::star::accessibility::XAccessible* pXAcc,unsigned short pState );
    void  UpdateState( com::sun::star::accessibility::XAccessible* pXAcc );
    void  SetLocation( com::sun::star::accessibility::XAccessible* pXAcc,
                       long Top = 0,long left = 0,long width = 0,long height = 0);

    void  SetValue( com::sun::star::accessibility::XAccessible* pXAcc, com::sun::star::uno::Any pAny );
    void  UpdateValue( com::sun::star::accessibility::XAccessible* pXAcc );

    void  SetAccName( com::sun::star::accessibility::XAccessible* pXAcc, com::sun::star::uno::Any newName);
    void  UpdateAccName( com::sun::star::accessibility::XAccessible* pXAcc );

    void  SetDescription( com::sun::star::accessibility::XAccessible* pXAcc, com::sun::star::uno::Any newDesc );
    void UpdateDescription( com::sun::star::accessibility::XAccessible* pXAcc );

    void  SetRole( com::sun::star::accessibility::XAccessible* pXAcc, long Role );

    void  UpdateAccFocus( com::sun::star::accessibility::XAccessible* newFocus );
    void  UpdateAction( com::sun::star::accessibility::XAccessible* pXAcc );

    sal_Bool IsContainer( com::sun::star::accessibility::XAccessible* pAccessible );

    IMAccessible* GetIMAccByXAcc( com::sun::star::accessibility::XAccessible* pXAcc );
    IMAccessible* GetIAccessibleFromResID(long resID);

    void NotifyDestroy( com::sun::star::accessibility::XAccessible* pXAcc );
    com::sun::star::accessibility::XAccessible* GetParentXAccessible( com::sun::star::accessibility::XAccessible* pXAcc );
    short GetParentRole( com::sun::star::accessibility::XAccessible* pXAcc );

    void SaveTopWindowHandle(HWND hWnd, com::sun::star::accessibility::XAccessible* pXAcc);

    void UpdateChildState(com::sun::star::accessibility::XAccessible* pXAcc);

    bool IsSpecialToolboItem(com::sun::star::accessibility::XAccessible* pXAcc);

    short GetRole(com::sun::star::accessibility::XAccessible* pXAcc);

    com::sun::star::accessibility::XAccessible* GetAccDocByAccTopWin( com::sun::star::accessibility::XAccessible* pXAcc );
    bool IsTopWinAcc( com::sun::star::accessibility::XAccessible* pXAcc );

    bool IsStateManageDescendant(com::sun::star::accessibility::XAccessible* pAccessible);

};

#endif
