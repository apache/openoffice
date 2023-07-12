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



//+-------------------------------------------------------------------------
//
//  File:       propspec.hxx
//
//  Contents:   C++ wrapper(s) for FULLPROPSPEC
//
//-------------------------------------------------------------------------
#pragma once
#if defined _MSC_VER
#pragma warning(push, 1)
#endif
#include <windows.h>
#include <ole2.h>
#include <ntquery.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif
//+-------------------------------------------------------------------------
//
//  Declare:    CLSID_SummaryInforation, GUID
//              CLSID_Storage, GUID
//
//  Contents:   Definitions of OpenOffice Document properties
//
//--------------------------------------------------------------------------

//extern GUID CLSID_Storage;
//
//extern GUID CLSID_SummaryInformation;
//const PID_TITLE     = PIDSI_TITLE; // 2;
//const PID_SUBJECT   = PIDSI_SUBJECT; // 3;
//const PID_AUTHOR    = PIDSI_AUTHOR; // 4;
//const PID_KEYWORDS  = PIDSI_KEYWORDS; // 5;
//const PID_COMMENTS  = PIDSI_COMMENTS; //6;
//const PID_REVNUMBER = PIDSI_REVNUMBER; //9;
//const PID_WORDCOUNT = PIDSI_WORDCOUNT; //f;
//+-------------------------------------------------------------------------
//
//  Class:      CFullPropertySpec
//
//  Purpose:    Describes full (PropertySet\Property) name of a property.
//
//--------------------------------------------------------------------------

class CFullPropSpec
{
public:
    CFullPropSpec();
    CFullPropSpec( GUID const & guidPropSet, PROPID pidProperty );
    CFullPropSpec( GUID const & guidPropSet, WCHAR const * wcsProperty );
    // Validity check
    inline BOOL IsValid() const;

    // Copy constructors/assignment/clone
    CFullPropSpec( CFullPropSpec const & Property );
    CFullPropSpec & operator=( CFullPropSpec const & Property );
    ~CFullPropSpec();
    // Memory allocation
    void * operator new( size_t size );
    inline void * operator new( size_t size, void * p );
    void   operator delete( void * p );
    inline FULLPROPSPEC * CastToStruct();
    inline FULLPROPSPEC const * CastToStruct() const;
    // Comparators
    int operator==( CFullPropSpec const & prop ) const;
    int operator!=( CFullPropSpec const & prop ) const;
    // Member variable access
    inline void SetPropSet( GUID const & guidPropSet );
    inline GUID const & GetPropSet() const;

    void SetProperty( PROPID pidProperty );
    BOOL SetProperty( WCHAR const * wcsProperty );
    inline WCHAR const * GetPropertyName() const;
    inline PROPID GetPropertyPropid() const;
    inline PROPSPEC GetPropSpec() const;
    inline BOOL IsPropertyName() const;
    inline BOOL IsPropertyPropid() const;
private:
    GUID     _guidPropSet;
    PROPSPEC _psProperty;
};
// Inline methods for CFullPropSpec
inline void * CFullPropSpec::operator new( size_t size )
{
    void * p = CoTaskMemAlloc( size );
    return( p );
}
inline void * CFullPropSpec::operator new( size_t /*size*/, void * p )
{
    return( p );
}
inline void CFullPropSpec::operator delete( void * p )
{
    if ( p )
        CoTaskMemFree( p );
}
inline BOOL CFullPropSpec::IsValid() const
{
    return ( _psProperty.ulKind == PRSPEC_PROPID ||
             0 != _psProperty.lpwstr );
}
inline void CFullPropSpec::SetPropSet( GUID const & guidPropSet )
{
    _guidPropSet = guidPropSet;
}
inline GUID const & CFullPropSpec::GetPropSet() const
{
    return( _guidPropSet );
}
inline PROPSPEC CFullPropSpec::GetPropSpec() const
{
    return( _psProperty );
}
inline WCHAR const * CFullPropSpec::GetPropertyName() const
{
    return( _psProperty.lpwstr );
}
inline PROPID CFullPropSpec::GetPropertyPropid() const
{
    return( _psProperty.propid );
}
inline BOOL CFullPropSpec::IsPropertyName() const
{
    return( _psProperty.ulKind == PRSPEC_LPWSTR );
}
inline BOOL CFullPropSpec::IsPropertyPropid() const
{
    return( _psProperty.ulKind == PRSPEC_PROPID );
}
