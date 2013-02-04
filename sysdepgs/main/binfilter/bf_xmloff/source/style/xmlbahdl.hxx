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



#ifndef _XMLOFF_PROPERTYHANDLER_BASICTYPES_HXX
#define _XMLOFF_PROPERTYHANDLER_BASICTYPES_HXX

#ifndef _XMLOFF_PROPERTYHANDLERBASE_HXX
#include <xmlprhdl.hxx>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _XMLOFF_XMLTOKEN_HXX
#include "xmltoken.hxx"
#endif
namespace binfilter {


/**
    PropertyHandler for the XML-data-type: XML_TYPE_NUMBER
*/
class XMLNumberPropHdl : public XMLPropertyHandler
{
	sal_Int8 nBytes;

public:
	XMLNumberPropHdl( sal_Int8 nB=4 ) : nBytes( nB ) {}
	virtual ~XMLNumberPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_NUMBER_NONE
*/
class XMLNumberNonePropHdl : public XMLPropertyHandler
{
	::rtl::OUString	sZeroStr;
	sal_Int8		nBytes;
public:
	XMLNumberNonePropHdl( sal_Int8 nB = 4 );
	XMLNumberNonePropHdl( enum ::binfilter::xmloff::token::XMLTokenEnum eZeroString, sal_Int8 nB = 4 );
	virtual ~XMLNumberNonePropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_MEASURE
*/
class XMLMeasurePropHdl : public XMLPropertyHandler
{
	sal_Int8 nBytes;
public:
	XMLMeasurePropHdl( sal_Int8 nB=4 ) : nBytes( nB ) {}
	virtual ~XMLMeasurePropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_PERCENT
*/
class XMLPercentPropHdl : public XMLPropertyHandler
{
	sal_Int8 nBytes;
public:
	XMLPercentPropHdl( sal_Int8 nB=4 ) : nBytes( nB ) {}
	virtual ~XMLPercentPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_PERCENT
*/
class XMLMeasurePxPropHdl : public XMLPropertyHandler
{
	sal_Int8 nBytes;
public:
	XMLMeasurePxPropHdl( sal_Int8 nB=4 ) : nBytes( nB ) {}
	virtual ~XMLMeasurePxPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_BOOL
*/
class XMLBoolPropHdl : public XMLPropertyHandler
{
public:
	virtual ~XMLBoolPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_COLOR
*/
class XMLColorPropHdl : public XMLPropertyHandler
{
public:
	virtual ~XMLColorPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_STRING
*/
class XMLStringPropHdl : public XMLPropertyHandler
{
public:
	virtual ~XMLStringPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
	PropertyHandler for the XML-data-type: XML_TYPE_DOUBLE
*/
class XMLDoublePropHdl : public XMLPropertyHandler
{
public:
	virtual ~XMLDoublePropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_NBOOL
*/
class XMLNBoolPropHdl : public XMLPropertyHandler
{
public:
	virtual ~XMLNBoolPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_COLORTRANSPARENT
*/
class XMLColorTransparentPropHdl : public XMLPropertyHandler
{
	const ::rtl::OUString sTransparent;

public:
	XMLColorTransparentPropHdl( enum ::binfilter::xmloff::token::XMLTokenEnum eTransparent = xmloff::token::XML_TOKEN_INVALID );
	virtual ~XMLColorTransparentPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_ISTRANSPARENT
*/
class XMLIsTransparentPropHdl : public XMLPropertyHandler
{
	const ::rtl::OUString sTransparent;
	sal_Bool bTransPropValue;

public:
	XMLIsTransparentPropHdl( enum ::binfilter::xmloff::token::XMLTokenEnum eTransparent = xmloff::token::XML_TOKEN_INVALID,
						     sal_Bool bTransPropValue = sal_True );
	virtual ~XMLIsTransparentPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_COLORAUTO
*/
class XMLColorAutoPropHdl : public XMLPropertyHandler
{
public:
	XMLColorAutoPropHdl();
	virtual ~XMLColorAutoPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

/**
    PropertyHandler for the XML-data-type: XML_TYPE_COLORISAUTO
*/
class XMLIsAutoColorPropHdl : public XMLPropertyHandler
{
public:
	XMLIsAutoColorPropHdl();
	virtual ~XMLIsAutoColorPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};


/**
    PropertyHandler for properties that cannot make use of importXML
	and exportXML methods, but can make use of the default comparison
*/
class XMLCompareOnlyPropHdl : public XMLPropertyHandler
{
public:
	virtual ~XMLCompareOnlyPropHdl();

	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};


}//end of namespace binfilter
#endif		// _XMLOFF_PROPERTYHANDLER_BASICTYPES_HXX
