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



#ifndef __FRAMEWORK_QUERIES_H_
#define __FRAMEWORK_QUERIES_H_

//_________________________________________________________________________________________________________________
//	own includes
//_________________________________________________________________________________________________________________

#include <general.h>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/uno/Sequence.hxx>
#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
//	const
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
	@short			These values describe our supported queries for type, filter ... properties.
					They are used by our FilterFactory or our TypeDetection to return
					subsets of our cached configuration.
*//*-*************************************************************************************************************/
#define BASE_QUERY_ALL                                  DECLARE_ASCII("_query_all"     )
#define BASE_QUERY_WRITER                               DECLARE_ASCII("_query_Writer"  )
#define BASE_QUERY_WEB                                  DECLARE_ASCII("_query_web"     )
#define BASE_QUERY_GLOBAL                               DECLARE_ASCII("_query_global"  )
#define BASE_QUERY_CHART                                DECLARE_ASCII("_query_chart"   )
#define BASE_QUERY_CALC                                 DECLARE_ASCII("_query_calc"    )
#define BASE_QUERY_IMPRESS                              DECLARE_ASCII("_query_impress" )
#define BASE_QUERY_DRAW                                 DECLARE_ASCII("_query_draw"    )
#define BASE_QUERY_MATH                                 DECLARE_ASCII("_query_math"    )
#define BASE_QUERY_GRAPHICS                             DECLARE_ASCII("_query_graphics")

/*-************************************************************************************************************//**
    @short          These parameters can be used in combination with BASE_QUERY_... defines to
                    specialize it.
                    use follow syntax to do so:     "<query>[:<param>[=<value>]]"
                    e.g.:                           "_query_writer:default_first:use_order:sort_prop=uiname"

                    argument                        description                                     default
                    -----------------------------------------------------------------------------------------------
                    iflags=<mask>                   include filters by given mask                   0
                    eflags=<mask>                   exclude filters by given mask                   0
                    sort_prop=<[name,uiname]>       sort by internal name or uiname                 name
                    descending                      sort descending                                 false
                    use_order                       use order flag of filters for sorting           false
                    default_first                   set default filter on top of return list        false
                    case_sensitive                  compare "sort_prop" case sensitive              false
*//*-*************************************************************************************************************/
#define SEPERATOR_QUERYPARAM                            ((sal_Unicode)':')
#define SEPERATOR_QUERYPARAMVALUE                       ((sal_Unicode)'=')

#define QUERYPARAM_IFLAGS                               DECLARE_ASCII("iflags"          )
#define QUERYPARAM_EFLAGS                               DECLARE_ASCII("eflags"          )
#define QUERYPARAM_SORT_PROP                            DECLARE_ASCII("sort_prop"       )

#define QUERYPARAM_DESCENDING                           DECLARE_ASCII("descending"      )
#define QUERYPARAM_USE_ORDER                            DECLARE_ASCII("use_order"       )
#define QUERYPARAM_DEFAULT_FIRST                        DECLARE_ASCII("default_first"   )
#define QUERYPARAM_CASE_SENSITIVE                       DECLARE_ASCII("case_sensitive"  )

#define QUERYPARAMVALUE_SORT_PROP_NAME                  DECLARE_ASCII("name"            )
#define QUERYPARAMVALUE_SORT_PROP_UINAME                DECLARE_ASCII("uiname"          )

/*-************************************************************************************************************//**
    @short          Helper class to support easy building of a query statements.
*//*-*************************************************************************************************************/
class QueryBuilder
{
    public:
        //---------------------------------------------------------------------------------------------------------
        // start with empty query
        //---------------------------------------------------------------------------------------------------------
        QueryBuilder()
        {
            resetAll();
        }

        //---------------------------------------------------------------------------------------------------------
        // returns full query as copy of internal set values
        //---------------------------------------------------------------------------------------------------------
        ::rtl::OUString getQuery()
        {
            ::rtl::OUStringBuffer sCopy( m_sParams );
            sCopy.insert( 0, m_sBase );
            return sCopy.makeStringAndClear();
        }

        //---------------------------------------------------------------------------------------------------------
        // set new or change existing base query part
        //---------------------------------------------------------------------------------------------------------
        void setBase( const ::rtl::OUString& sBase )
        {
            m_sBase = sBase;
        }

        //---------------------------------------------------------------------------------------------------------
        // add new parameter (with optional value) to param list
        //---------------------------------------------------------------------------------------------------------
        void addParam( const ::rtl::OUString& sParam, const ::rtl::OUString& sValue = ::rtl::OUString() )
        {
            m_sParams.append( SEPERATOR_QUERYPARAM );
            m_sParams.append( sParam               );
            if( sValue.getLength() > 0 )
            {
                m_sParams.append( SEPERATOR_QUERYPARAMVALUE );
                m_sParams.append( sValue                    );
            }
        }

        //---------------------------------------------------------------------------------------------------------
        // forget all set params and start with empty ones
        // Attention: base of query isn't changed!
        //---------------------------------------------------------------------------------------------------------
        void resetParams()
        {
            m_sParams.makeStringAndClear();
            m_sParams.ensureCapacity( 256 );
        }

        //---------------------------------------------------------------------------------------------------------
        // start with new empty query
        //---------------------------------------------------------------------------------------------------------
        void resetAll()
        {
            m_sBase = ::rtl::OUString();
            resetParams();
        }

    private:
        ::rtl::OUString         m_sBase     ;
        ::rtl::OUStringBuffer   m_sParams   ;

};      // class QueryBuilder

/*-************************************************************************************************************//**
    @short          Helper class to analyze queries and split into his different parts (base, params and values).
*//*-*************************************************************************************************************/
class QueryAnalyzer
{
    public:

        //---------------------------------------------------------------------------------------------------------
        // it's will not perform to compare strings as query type ...
        // so we convert it into these enum values.
        // default = E_ALL!
        //---------------------------------------------------------------------------------------------------------
        enum EQuery
        {
            E_ALL       ,
            E_WRITER    ,
            E_WEB       ,
            E_GLOBAL    ,
            E_CHART     ,
            E_CALC      ,
            E_IMPRESS   ,
            E_DRAW      ,
            E_MATH      ,
            E_GRAPHICS
        };

        //---------------------------------------------------------------------------------------------------------
        // these are valid values for param "sort_prop".
        // other ones are not supported!
        // default = E_NAME
        //---------------------------------------------------------------------------------------------------------
        enum ESortProp
        {
            E_NAME      ,
            E_UINAME
        };

        //---------------------------------------------------------------------------------------------------------
        // analyze given query and split it into his different parts; <base>:<param1>:<param2=value>...
        //---------------------------------------------------------------------------------------------------------
        QueryAnalyzer( const ::rtl::OUString& sQuery )
            // Don't forget to set default values for non given params!
            :   m_eQuery        ( E_ALL      )   // return ALL filter ...
            ,   m_nIFlags       ( 0          )   // which has set ANY flag ... (we remove all entries which match with these mask .. => 0!)
            ,   m_nEFlags       ( 0          )   // (only used, if nIFlags==0 and himself!=0!)
            ,   m_eSortProp     ( E_NAME     )   // sort it by internal name ...
            ,   m_bDescending   ( sal_False  )   // in ascending order ...
            ,   m_bCaseSensitive( sal_False  )   // ignore case ...
            ,   m_bUseOrder     ( sal_False  )   // don't use order flag ...
            ,   m_bDefaultFirst ( sal_False  )   // and don't handle default entries in special case!
        {
            // Translate old query format to new one first!
            ::rtl::OUString sNewQuery( sQuery );
            if( sQuery == DECLARE_ASCII("_filterquery_textdocument_withdefault") )
                sNewQuery=DECLARE_ASCII("_query_writer:default_first:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_webdocument_withdefault") )
                sNewQuery=DECLARE_ASCII("_query_web:default_first:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_globaldocument_withdefault") )
                sNewQuery=DECLARE_ASCII("_query_global:default_first:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_chartdocument_withdefault") )
                sNewQuery=DECLARE_ASCII("_query_chart:default_first:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_spreadsheetdocument_withdefault") )
                sNewQuery=DECLARE_ASCII("_query_calc:default_first:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_presentationdocument_withdefault") )
                sNewQuery=DECLARE_ASCII("_query_impress:default_first:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_drawingdocument_withdefault") )
                sNewQuery=DECLARE_ASCII("_query_draw:default_first:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_formulaproperties_withdefault") )
                sNewQuery=DECLARE_ASCII("_query_math:default_first:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_textdocument") )
                sNewQuery=DECLARE_ASCII("_query_writer:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_webdocument") )
                sNewQuery=DECLARE_ASCII("_query_web:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_globaldocument") )
                sNewQuery=DECLARE_ASCII("_query_global:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_chartdocument") )
                sNewQuery=DECLARE_ASCII("_query_chart:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_spreadsheetdocument") )
                sNewQuery=DECLARE_ASCII("_query_calc:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_presentationdocument") )
                sNewQuery=DECLARE_ASCII("_query_impress:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_drawingdocument") )
                sNewQuery=DECLARE_ASCII("_query_draw:use_order:sort_prop=uiname");
            else
            if( sQuery == DECLARE_ASCII("_filterquery_formulaproperties") )
                sNewQuery=DECLARE_ASCII("_query_math:use_order:sort_prop=uiname");

            // Analyze query ...
            // Try to find base of it and safe it for faster access as enum value!
            sal_Int32       nToken = 0;
            ::rtl::OUString sParam    ;
            ::rtl::OUString sBase  = sNewQuery.getToken( 0, SEPERATOR_QUERYPARAM, nToken );

            if( sBase.equalsIgnoreAsciiCase( BASE_QUERY_ALL      ) == sal_True ) m_eQuery = E_ALL      ;  else
            if( sBase.equalsIgnoreAsciiCase( BASE_QUERY_WRITER   ) == sal_True ) m_eQuery = E_WRITER   ;  else
            if( sBase.equalsIgnoreAsciiCase( BASE_QUERY_WEB      ) == sal_True ) m_eQuery = E_WEB      ;  else
            if( sBase.equalsIgnoreAsciiCase( BASE_QUERY_GLOBAL   ) == sal_True ) m_eQuery = E_GLOBAL   ;  else
            if( sBase.equalsIgnoreAsciiCase( BASE_QUERY_CHART    ) == sal_True ) m_eQuery = E_CHART    ;  else
            if( sBase.equalsIgnoreAsciiCase( BASE_QUERY_CALC     ) == sal_True ) m_eQuery = E_CALC     ;  else
            if( sBase.equalsIgnoreAsciiCase( BASE_QUERY_IMPRESS  ) == sal_True ) m_eQuery = E_IMPRESS  ;  else
            if( sBase.equalsIgnoreAsciiCase( BASE_QUERY_DRAW     ) == sal_True ) m_eQuery = E_DRAW     ;  else
            if( sBase.equalsIgnoreAsciiCase( BASE_QUERY_MATH     ) == sal_True ) m_eQuery = E_MATH     ;  else
            if( sBase.equalsIgnoreAsciiCase( BASE_QUERY_GRAPHICS ) == sal_True ) m_eQuery = E_GRAPHICS ;

            // Try to get additional parameters ...
            while( nToken >= 0 )
            {
                sParam = sNewQuery.getToken( 0, SEPERATOR_QUERYPARAM, nToken );
                // "default_first"
                if( sParam.compareTo( QUERYPARAM_DEFAULT_FIRST, QUERYPARAM_DEFAULT_FIRST.getLength() ) == 0 )
                {
                    m_bDefaultFirst = sal_True;
                }
                else
                // "use_order"
                if( sParam.compareTo( QUERYPARAM_USE_ORDER, QUERYPARAM_USE_ORDER.getLength() ) == 0 )
                {
                    m_bUseOrder = sal_True;
                }
                else
                // "descending"
                if( sParam.compareTo( QUERYPARAM_DESCENDING, QUERYPARAM_DESCENDING.getLength() ) == 0 )
                {
                    m_bDescending = sal_True;
                }
                else
                // "case_sensitive"
                if( sParam.compareTo( QUERYPARAM_CASE_SENSITIVE, QUERYPARAM_CASE_SENSITIVE.getLength() ) == 0 )
                {
                    m_bCaseSensitive = sal_True;
                }
                else
                // "iflags=<mask>"
                if( sParam.compareTo( QUERYPARAM_IFLAGS, QUERYPARAM_IFLAGS.getLength() ) == 0 )
                {
                    sal_Int32       nSubToken  = 0;
                    ::rtl::OUString sParamName = sParam.getToken( 0, SEPERATOR_QUERYPARAMVALUE, nSubToken );
                    if( nSubToken > 0 )
                    {
                        m_nIFlags = sParam.getToken( 0, SEPERATOR_QUERYPARAMVALUE, nSubToken ).toInt32();
                    }
                }
                else
                // "eflags=<mask>"
                if( sParam.compareTo( QUERYPARAM_EFLAGS, QUERYPARAM_EFLAGS.getLength() ) == 0 )
                {
                    sal_Int32       nSubToken  = 0;
                    ::rtl::OUString sParamName = sParam.getToken( 0, SEPERATOR_QUERYPARAMVALUE, nSubToken );
                    if( nSubToken > 0 )
                    {
                        m_nEFlags = sParam.getToken( 0, SEPERATOR_QUERYPARAMVALUE, nSubToken ).toInt32();
                    }
                }
                else
                // "sort_prop=<[name,uiname]>"
                if( sParam.compareTo( QUERYPARAM_SORT_PROP, QUERYPARAM_SORT_PROP.getLength() ) == 0 )
                {
                    sal_Int32       nSubToken  = 0;
                    ::rtl::OUString sParamName = sParam.getToken( 0, SEPERATOR_QUERYPARAMVALUE, nSubToken );
                    if( nSubToken > 0 )
                    {
                        ::rtl::OUString sParamValue = sParam.getToken( 0, SEPERATOR_QUERYPARAMVALUE, nSubToken );
                        if( sParamValue.compareTo( QUERYPARAMVALUE_SORT_PROP_NAME, QUERYPARAMVALUE_SORT_PROP_NAME.getLength() ) == 0 )
                            m_eSortProp = E_NAME;
                        else
                        if( sParamValue.compareTo( QUERYPARAMVALUE_SORT_PROP_UINAME, QUERYPARAMVALUE_SORT_PROP_UINAME.getLength() ) == 0 )
                            m_eSortProp = E_UINAME;
                    }
                }
            }
        }

        //---------------------------------------------------------------------------------------------------------
        // return type of query. User can decide then, which action should be started.
        // For faster work we converted query string into corresponding enum value!
        //---------------------------------------------------------------------------------------------------------
        EQuery getQueryType() const { return m_eQuery; }

        //---------------------------------------------------------------------------------------------------------
        // access to additional parameter values
        // Methods return default of really set values!
        //---------------------------------------------------------------------------------------------------------
        sal_uInt32 getIFlags       () const { return m_nIFlags       ; }
        sal_uInt32 getEFlags       () const { return m_nEFlags       ; }
        ESortProp  getSortProp     () const { return m_eSortProp     ; }
        sal_Bool   getDescending   () const { return m_bDescending   ; }
        sal_Bool   getCaseSensitive() const { return m_bCaseSensitive; }
        sal_Bool   getUseOrder     () const { return m_bUseOrder     ; }
        sal_Bool   getDefaultFirst () const { return m_bDefaultFirst ; }

        //---------------------------------------------------------------------------------------------------------
        // this method checks if given string match any supported query.
        // (ignore additional parameters!)
        //---------------------------------------------------------------------------------------------------------
        static sal_Bool isQuery( const ::rtl::OUString& sQuery )
        {
            return(
                    ( sQuery.compareToAscii( "_query_"      , 7  ) == 0 ) ||    // new style
                    ( sQuery.compareToAscii( "_filterquery_", 13 ) == 0 )       // old style!
                  );
        }

    private:
        EQuery      m_eQuery         ;
        sal_uInt32  m_nIFlags        ;
        sal_uInt32  m_nEFlags        ;
        ESortProp   m_eSortProp      ;
        sal_Bool    m_bDescending    ;
        sal_Bool    m_bCaseSensitive ;
        sal_Bool    m_bUseOrder      ;
        sal_Bool    m_bDefaultFirst  ;

};      // class QueryAnalyzer

}		//	namespace framework

#endif	//	#ifndef __FRAMEWORK_QUERIES_H_
