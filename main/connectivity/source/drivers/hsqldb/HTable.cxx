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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_hsqldb.hxx"
#include "hsqldb/HTable.hxx"
#include "hsqldb/HTables.hxx"
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/sdbcx/KeyType.hpp>
#include <com/sun/star/sdbc/KeyRule.hpp>
#include <cppuhelper/typeprovider.hxx>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/sdbc/ColumnValue.hpp>
#include <com/sun/star/sdbcx/Privilege.hpp>
#include <comphelper/property.hxx>
#include <comphelper/extract.hxx>
#include <comphelper/types.hxx>
#include "connectivity/dbtools.hxx"
#include "connectivity/sdbcx/VColumn.hxx"
#include "connectivity/TKeys.hxx"
#include "connectivity/TIndexes.hxx"
#include "connectivity/TColumnsHelper.hxx"
#include "hsqldb/HCatalog.hxx"
#include "hsqldb/HColumns.hxx"
#include "TConnection.hxx"

#include <tools/diagnose_ex.h>


using namespace ::comphelper;
using namespace connectivity::hsqldb;
using namespace connectivity::sdbcx;
using namespace connectivity;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;

namespace
{
    class OHSQLIndexes : public OIndexesHelper
    {
        OHSQLTable* m_pTable;
    protected:
        virtual void dropObject(sal_Int32 _nPos,const ::rtl::OUString _sElementName);
    public:
        OHSQLIndexes(OHSQLTable* _pTable,::osl::Mutex& _rMutex,const TStringVector& _rVector)
            : OIndexesHelper(_pTable,_rMutex,_rVector)
            , m_pTable(_pTable)
        {
        }
    };

    // HSQLDB 2.x rejects the generic DROP INDEX ... ON <table>: index names belong to the schema
    void OHSQLIndexes::dropObject(sal_Int32 /*_nPos*/,const ::rtl::OUString _sElementName)
    {
        if ( m_pTable->isNew() )
            return;

        ::rtl::OUString aSchema,aName,sTemp;
        sal_Int32 nLen = _sElementName.indexOf('.');
        if ( nLen != -1 )
            aSchema = _sElementName.copy(0,nLen);
        aName = _sElementName.copy(nLen+1);

        ::rtl::OUString aSql( RTL_CONSTASCII_USTRINGPARAM( "DROP INDEX " ) );
        aSql += ::dbtools::composeTableName( m_pTable->getMetaData(), sTemp, aSchema, aName, sal_True, ::dbtools::eInIndexDefinitions );

        Reference< XStatement > xStmt = m_pTable->getConnection()->createStatement();
        if ( xStmt.is() )
        {
            xStmt->execute(aSql);
            ::comphelper::disposeComponent(xStmt);
        }
    }
}

OHSQLTable::OHSQLTable(	sdbcx::OCollection* _pTables,
						   const Reference<	XConnection >& _xConnection)
	:OTableHelper(_pTables,_xConnection,sal_True)
{
	// we create a new table here, so we should have all the rights or ;-)
	m_nPrivileges = Privilege::DROP			|
					Privilege::REFERENCE	|
					Privilege::ALTER		|
					Privilege::CREATE		|
					Privilege::READ			|
					Privilege::DELETE		|
					Privilege::UPDATE		|
					Privilege::INSERT		|
					Privilege::SELECT;
	construct();
}
// -------------------------------------------------------------------------
OHSQLTable::OHSQLTable(	sdbcx::OCollection* _pTables,
						   const Reference< XConnection >& _xConnection,
					const ::rtl::OUString& _Name,
					const ::rtl::OUString& _Type,
					const ::rtl::OUString& _Description ,
					const ::rtl::OUString& _SchemaName,
					const ::rtl::OUString& _CatalogName,
					sal_Int32 _nPrivileges
				) : OTableHelper(	_pTables,
									_xConnection,
									sal_True,
									_Name,
									_Type,
									_Description,
									_SchemaName,
									_CatalogName)
 , m_nPrivileges(_nPrivileges)
{
	construct();
}
// -------------------------------------------------------------------------
void OHSQLTable::construct()
{
	OTableHelper::construct();
	if ( !isNew() )
		registerProperty(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PRIVILEGES),	PROPERTY_ID_PRIVILEGES,PropertyAttribute::READONLY,&m_nPrivileges,	::getCppuType(&m_nPrivileges));
}
// -----------------------------------------------------------------------------
::cppu::IPropertyArrayHelper* OHSQLTable::createArrayHelper( sal_Int32 /*_nId*/ ) const
{
    return doCreateArrayHelper();
}
// -------------------------------------------------------------------------
::cppu::IPropertyArrayHelper & OHSQLTable::getInfoHelper()
{
	return *static_cast<OHSQLTable_PROP*>(const_cast<OHSQLTable*>(this))->getArrayHelper(isNew() ? 1 : 0);
}
// -----------------------------------------------------------------------------
sdbcx::OCollection* OHSQLTable::createColumns(const TStringVector& _rNames)
{
	OHSQLColumns* pColumns = new OHSQLColumns(*this,sal_True,m_aMutex,_rNames);
	pColumns->setParent(this);
	return pColumns;
}
// -----------------------------------------------------------------------------
sdbcx::OCollection* OHSQLTable::createKeys(const TStringVector& _rNames)
{
	return new OKeysHelper(this,m_aMutex,_rNames);
}
// -----------------------------------------------------------------------------
sdbcx::OCollection* OHSQLTable::createIndexes(const TStringVector& _rNames)
{
	return new OHSQLIndexes(this,m_aMutex,_rNames);
}
//--------------------------------------------------------------------------
Sequence< sal_Int8 > OHSQLTable::getUnoTunnelImplementationId()
{
	static ::cppu::OImplementationId * pId = 0;
	if (! pId)
	{
		::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
		if (! pId)
		{
			static ::cppu::OImplementationId aId;
			pId = &aId;
		}
	}
	return pId->getImplementationId();
}

// com::sun::star::lang::XUnoTunnel
//------------------------------------------------------------------
sal_Int64 OHSQLTable::getSomething( const Sequence< sal_Int8 > & rId )
{
	return (rId.getLength() == 16 && 0 == rtl_compareMemory(getUnoTunnelImplementationId().getConstArray(),  rId.getConstArray(), 16 ) )
				? reinterpret_cast< sal_Int64 >( this )
				: OTable_TYPEDEF::getSomething(rId);
}
// -------------------------------------------------------------------------
// XAlterTable
void SAL_CALL OHSQLTable::alterColumnByName( const ::rtl::OUString& colName, const Reference< XPropertySet >& descriptor )
{
	::osl::MutexGuard aGuard(m_aMutex);
	checkDisposed(
#ifdef GCC
		::connectivity::sdbcx::OTableDescriptor_BASE::rBHelper.bDisposed
#else
		rBHelper.bDisposed
#endif
		);

	if ( m_pColumns && !m_pColumns->hasByName(colName) )
		throw NoSuchElementException(colName,*this);


	if ( !isNew() )
	{
		// first we have to check what should be altered
		Reference<XPropertySet> xProp;
		m_pColumns->getByName(colName) >>= xProp;
		// first check the types
		sal_Int32 nOldType = 0,nNewType = 0,nOldPrec = 0,nNewPrec = 0,nOldScale = 0,nNewScale = 0;
        ::rtl::OUString sOldTypeName, sNewTypeName;

		::dbtools::OPropertyMap& rProp = OMetaConnection::getPropMap();

        // type/typename
        xProp->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_TYPE))			>>= nOldType;
		descriptor->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_TYPE))	>>= nNewType;
        xProp->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_TYPENAME))		>>= sOldTypeName;
		descriptor->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_TYPENAME))>>= sNewTypeName;

        // and precsions and scale
		xProp->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_PRECISION))	>>= nOldPrec;
		descriptor->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_PRECISION))>>= nNewPrec;
		xProp->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_SCALE))		>>= nOldScale;
		descriptor->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_SCALE))	>>= nNewScale;

        // second: check the "is nullable" value
		sal_Int32 nOldNullable = 0,nNewNullable = 0;
		xProp->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_ISNULLABLE))		>>= nOldNullable;
		descriptor->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_ISNULLABLE))	>>= nNewNullable;

		// check also the auto_increment
		sal_Bool bOldAutoIncrement = sal_False,bAutoIncrement = sal_False;
		xProp->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_ISAUTOINCREMENT))		>>= bOldAutoIncrement;
		descriptor->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_ISAUTOINCREMENT))	>>= bAutoIncrement;

        // now we should look if the name of the column changed
		::rtl::OUString sNewColumnName;
		descriptor->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_NAME)) >>= sNewColumnName;
		if ( !sNewColumnName.equals(colName) )
		{
			const ::rtl::OUString sQuote = getMetaData()->getIdentifierQuoteString(  );

            ::rtl::OUString sSql = getAlterTableColumnPart();
			sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" ALTER COLUMN "));
			sSql += ::dbtools::quoteName(sQuote,colName);

            sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" RENAME TO "));
            sSql += ::dbtools::quoteName(sQuote,sNewColumnName);

            executeStatement(sSql);
		}

		if  (   nOldType != nNewType
            ||  sOldTypeName != sNewTypeName
			||	nOldPrec != nNewPrec
			||	nOldScale != nNewScale
			||	bOldAutoIncrement != bAutoIncrement )
		{
			// special handling because they change the type names to distinguish
			// if a column should be an auto_incmrement one
			if ( bOldAutoIncrement != bAutoIncrement )
			{
				/// TODO: insert special handling for auto increment "IDENTITY" and primary key
			}
			alterColumnType(nNewType,sNewColumnName,descriptor);
			if ( bAutoIncrement && !bOldAutoIncrement )
				restartIdentityAfterMax(sNewColumnName);
		}

		// HSQLDB 2.x only changes nullability with SET [NOT] NULL, not in a column definition
		if ( nNewNullable != nOldNullable )
		{
			::rtl::OUString sSql = getAlterTableColumnPart();
			sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" ALTER COLUMN "));
			sSql += ::dbtools::quoteName(getMetaData()->getIdentifierQuoteString(),sNewColumnName);
			sSql += ( nNewNullable == ColumnValue::NO_NULLS )
				? ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" SET NOT NULL"))
				: ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" SET NULL"));
			executeStatement(sSql);
		}

		// third: check the default values
		::rtl::OUString sNewDefault,sOldDefault;
		xProp->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_DEFAULTVALUE))		>>= sOldDefault;
		descriptor->getPropertyValue(rProp.getNameByIndex(PROPERTY_ID_DEFAULTVALUE)) >>= sNewDefault;

		if ( sOldDefault != sNewDefault )
		{
			if ( sNewDefault.getLength() )
				alterDefaultValue(sNewDefault,sNewColumnName);
			else
				dropDefaultValue(sNewColumnName);
		}

		m_pColumns->refresh();
	}
	else
	{
		if(m_pColumns)
		{
			m_pColumns->dropByName(colName);
			m_pColumns->appendByDescriptor(descriptor);
		}
	}

}
// -----------------------------------------------------------------------------
void OHSQLTable::alterColumnType(sal_Int32 nNewType,const ::rtl::OUString& _rColName, const Reference<XPropertySet>& _xDescriptor)
{
	::rtl::OUString sSql = getAlterTableColumnPart();

    sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" ALTER COLUMN "));
#if OSL_DEBUG_LEVEL > 0
    try
    {
        ::rtl::OUString sDescriptorName;
        OSL_ENSURE( _xDescriptor.is()
                &&  ( _xDescriptor->getPropertyValue( OMetaConnection::getPropMap().getNameByIndex( PROPERTY_ID_NAME ) ) >>= sDescriptorName )
                &&  ( sDescriptorName == _rColName ),
                "OHSQLTable::alterColumnType: unexpected column name!" );
    }
    catch( const Exception& )
    {
    	DBG_UNHANDLED_EXCEPTION();
    }
#else
    (void)_rColName;
#endif

    OHSQLColumn* pColumn = new OHSQLColumn(sal_True);
	Reference<XPropertySet> xProp = pColumn;
	::comphelper::copyProperties(_xDescriptor,xProp);
	xProp->setPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE),makeAny(nNewType));
	// 2.x rejects DEFAULT and NOT NULL here and keeps the existing ones; IDENTITY must stay or it is dropped
	xProp->setPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISNULLABLE),makeAny(ColumnValue::NULLABLE));
	xProp->setPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_DEFAULTVALUE),makeAny(::rtl::OUString()));

	sSql += ::dbtools::createStandardColumnPart(xProp,getConnection());
	executeStatement(sSql);
}
// -----------------------------------------------------------------------------
// 2.x starts a new IDENTITY at 0, which collides with the rows already in the table
void OHSQLTable::restartIdentityAfterMax(const ::rtl::OUString& _rColName)
{
	const ::rtl::OUString sQuote = getMetaData()->getIdentifierQuoteString(  );
	const ::rtl::OUString sColumn = ::dbtools::quoteName(sQuote,_rColName);

	::rtl::OUString sQuery( RTL_CONSTASCII_USTRINGPARAM( "SELECT MAX(" ) );
	sQuery += sColumn;
	sQuery += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(") FROM "));
	sQuery += ::dbtools::composeTableName( getMetaData(), m_CatalogName, m_SchemaName, m_Name, sal_True, ::dbtools::eInDataManipulation );

	sal_Int64 nMax = 0;
	bool bHaveRows = false;
	Reference< XStatement > xStmt = getConnection()->createStatement(  );
	if ( xStmt.is() )
	{
		Reference< XResultSet > xRes = xStmt->executeQuery(sQuery);
		Reference< XRow > xRow(xRes,UNO_QUERY);
		if ( xRes.is() && xRow.is() && xRes->next() )
		{
			nMax = xRow->getLong(1);
			bHaveRows = !xRow->wasNull();
		}
		::comphelper::disposeComponent(xStmt);
	}
	if ( !bHaveRows )
		return;

	::rtl::OUString sSql = getAlterTableColumnPart();
	sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" ALTER COLUMN "));
	sSql += sColumn;
	sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" RESTART WITH "));
	sSql += ::rtl::OUString::valueOf( nMax + 1 );
	executeStatement(sSql);
}
// -----------------------------------------------------------------------------
void OHSQLTable::alterDefaultValue(const ::rtl::OUString& _sNewDefault,const ::rtl::OUString& _rColName)
{
	::rtl::OUString sSql = getAlterTableColumnPart();
	sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" ALTER COLUMN "));

	const ::rtl::OUString sQuote = getMetaData()->getIdentifierQuoteString(  );
	sSql += ::dbtools::quoteName(sQuote,_rColName);
	sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" SET DEFAULT '")) + _sNewDefault;
	sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("'"));

	executeStatement(sSql);
}
// -----------------------------------------------------------------------------
void OHSQLTable::dropDefaultValue(const ::rtl::OUString& _rColName)
{
	::rtl::OUString sSql = getAlterTableColumnPart();
	sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" ALTER COLUMN "));

	const ::rtl::OUString sQuote = getMetaData()->getIdentifierQuoteString(  );
	sSql += ::dbtools::quoteName(sQuote,_rColName);
	sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" DROP DEFAULT"));

	executeStatement(sSql);
}
// -----------------------------------------------------------------------------
::rtl::OUString OHSQLTable::getAlterTableColumnPart()
{
	::rtl::OUString sSql = ::rtl::OUString::createFromAscii("ALTER TABLE ");
	const ::rtl::OUString sQuote = getMetaData()->getIdentifierQuoteString(  );

	::rtl::OUString sComposedName( ::dbtools::composeTableName( getMetaData(), m_CatalogName, m_SchemaName, m_Name, sal_True, ::dbtools::eInTableDefinitions ) );
	sSql += sComposedName;

	return sSql;
}
// -----------------------------------------------------------------------------
void OHSQLTable::executeStatement(const ::rtl::OUString& _rStatement )
{
	::rtl::OUString sSQL = _rStatement;
	if(sSQL.lastIndexOf(',') == (sSQL.getLength()-1))
		sSQL = sSQL.replaceAt(sSQL.getLength()-1,1,::rtl::OUString::createFromAscii(")"));

	Reference< XStatement > xStmt = getConnection()->createStatement(  );
	if ( xStmt.is() )
	{
        try { xStmt->execute(sSQL); }
        catch( const Exception& )
        {
		    ::comphelper::disposeComponent(xStmt);
            throw;
        }
		::comphelper::disposeComponent(xStmt);
	}
}
// -----------------------------------------------------------------------------
Sequence< Type > SAL_CALL OHSQLTable::getTypes(  )
{
	if ( ! m_Type.compareToAscii("VIEW") )
	{
		Sequence< Type > aTypes = OTableHelper::getTypes();
		::std::vector<Type> aOwnTypes;
		aOwnTypes.reserve(aTypes.getLength());
		const Type* pIter = aTypes.getConstArray();
		const Type* pEnd = pIter + aTypes.getLength();
		for(;pIter != pEnd;++pIter)
		{
			if( *pIter != ::getCppuType((const Reference<XRename>*)0) )
			{
				aOwnTypes.push_back(*pIter);
			}
		}
		Type *pTypes = aOwnTypes.empty() ? 0 : &aOwnTypes[0];
		return Sequence< Type >(pTypes, aOwnTypes.size());
	}
	return OTableHelper::getTypes();
}
// -------------------------------------------------------------------------
// XRename
void SAL_CALL OHSQLTable::rename( const ::rtl::OUString& newName )
{
	::osl::MutexGuard aGuard(m_aMutex);
	checkDisposed(
#ifdef GCC
		::connectivity::sdbcx::OTableDescriptor_BASE::rBHelper.bDisposed
#else
		rBHelper.bDisposed
#endif
		);

	if(!isNew())
	{
		::rtl::OUString sSql = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ALTER "));
		if ( m_Type == ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("VIEW")) )
			sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" VIEW "));
		else
			sSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" TABLE "));

		::rtl::OUString sQuote = getMetaData()->getIdentifierQuoteString(  );

		::rtl::OUString sCatalog,sSchema,sTable;
		::dbtools::qualifiedNameComponents(getMetaData(),newName,sCatalog,sSchema,sTable,::dbtools::eInDataManipulation);

		::rtl::OUString sComposedName(
            ::dbtools::composeTableName( getMetaData(), m_CatalogName, m_SchemaName, m_Name, sal_True, ::dbtools::eInDataManipulation ) );
		sSql += sComposedName
            + ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" RENAME TO "));
		sSql += ::dbtools::composeTableName( getMetaData(), sCatalog, sSchema, sTable, sal_True, ::dbtools::eInDataManipulation );

        executeStatement(sSql);

		::connectivity::OTable_TYPEDEF::rename(newName);
	}
	else
		::dbtools::qualifiedNameComponents(getMetaData(),newName,m_CatalogName,m_SchemaName,m_Name,::dbtools::eInTableDefinitions);
}

// -------------------------------------------------------------------------
Any SAL_CALL OHSQLTable::queryInterface( const Type & rType )
{
	if( !m_Type.compareToAscii("VIEW") && rType == ::getCppuType((const Reference<XRename>*)0) )
		return Any();

	return OTableHelper::queryInterface(rType);
}
// -------------------------------------------------------------------------
