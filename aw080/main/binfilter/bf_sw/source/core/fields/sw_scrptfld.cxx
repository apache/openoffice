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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _DOCUFLD_HXX
#include <docufld.hxx>
#endif
#ifndef _UNOFLDMID_H
#include <unofldmid.h>
#endif
namespace binfilter {

extern String& GetString( const ::com::sun::star::uno::Any& rAny, String& rStr ); //STRIP008

using namespace ::com::sun::star;
using namespace ::rtl;
/*--------------------------------------------------------------------
	Beschreibung: ScriptField
 --------------------------------------------------------------------*/

/*N*/ SwScriptFieldType::SwScriptFieldType( SwDoc* pD )
/*N*/ 	: SwFieldType( RES_SCRIPTFLD ), pDoc( pD )
/*N*/ {}

/*N*/ SwFieldType* SwScriptFieldType::Copy() const
/*N*/ {
        return new SwScriptFieldType( pDoc );
/*N*/ }


/*--------------------------------------------------------------------
	Beschreibung: SwScriptField
 --------------------------------------------------------------------*/

/*N*/ SwScriptField::SwScriptField( SwScriptFieldType* pType,
/*N*/ 								const String& rType, const String& rCode,
/*N*/ 								BOOL bURL )
/*N*/ 	: SwField( pType ), sType( rType ), sCode( rCode ), bCodeURL( bURL )
/*N*/ {
/*N*/ }

/*N*/ String SwScriptField::Expand() const
/*N*/ {
/*N*/ 	return aEmptyStr;
/*N*/ }

SwField* SwScriptField::Copy() const
{
    return new SwScriptField( (SwScriptFieldType*)GetTyp(), sType, sCode, bCodeURL );
}

/*--------------------------------------------------------------------
	Beschreibung: Type setzen
 --------------------------------------------------------------------*/

void SwScriptField::SetPar1( const String& rStr )
{
    sType = rStr;
}

const String& SwScriptField::GetPar1() const
{
    return sType;
}

/*--------------------------------------------------------------------
	Beschreibung: Code setzen
 --------------------------------------------------------------------*/

void SwScriptField::SetPar2( const String& rStr )
{
    sCode = rStr;
}


String SwScriptField::GetPar2() const
{
    return sCode;
}
/*-----------------05.03.98 15:00-------------------

--------------------------------------------------*/
BOOL SwScriptField::QueryValue( uno::Any& rAny, BYTE nMId ) const
{
    nMId &= ~CONVERT_TWIPS;
    switch( nMId )
    {
    case FIELD_PROP_PAR1:
        rAny <<= OUString( sType );
        break;
    case FIELD_PROP_PAR2:
        rAny <<= OUString( sCode );
        break;
    case FIELD_PROP_BOOL1:
        rAny.setValue(&bCodeURL, ::getBooleanCppuType());
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return TRUE;
}
/*-----------------05.03.98 15:00-------------------

--------------------------------------------------*/
BOOL SwScriptField::PutValue( const uno::Any& rAny, BYTE nMId )
{
    nMId &= ~CONVERT_TWIPS;
    switch( nMId )
    {
    case FIELD_PROP_PAR1:
        ::binfilter::GetString( rAny, sType );
        break;
    case FIELD_PROP_PAR2:
        ::binfilter::GetString( rAny, sCode );
        break;
    case FIELD_PROP_BOOL1:
        bCodeURL = *(sal_Bool*)rAny.getValue();
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return TRUE;
}

}
