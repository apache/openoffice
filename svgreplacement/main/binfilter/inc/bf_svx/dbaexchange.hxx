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



#ifndef _SVX_DBAEXCHANGE_HXX_
#define _SVX_DBAEXCHANGE_HXX_

#ifndef _TRANSFER_HXX
#include <bf_svtools/transfer.hxx>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
namespace binfilter {

//........................................................................
namespace svx
{
//........................................................................

// column transfer formats
#define CTF_FIELD_DESCRIPTOR		0x0001		// the field descriptor format
#define CTF_CONTROL_EXCHANGE		0x0002		// the control exchange format
#define CTF_COLUMN_DESCRIPTOR		0x0004		// data access descriptor for a column

	//====================================================================
	//= OColumnTransferable
	//====================================================================
	class OColumnTransferable : public TransferableHelper
	{
	protected:
		::rtl::OUString			m_sCompatibleFormat;
		sal_Int32				m_nFormatFlags;

	public:
		/** construct the transferable
		*/
		OColumnTransferable(
			const ::rtl::OUString&	_rDatasource,
			const sal_Int32			_nCommandType,
			const ::rtl::OUString&	_rCommand,
			const ::rtl::OUString&	_rFieldName,
			sal_Int32	_nFormats
		);

		/** construct the transferable from a DatabaseForm component and a field name

			@param _rxForm
				the form which is bound to the data source which's field is to be dragged

			@param _rFieldName
				the name of the field to be dragged

			@param _rxColumn
				the column object. Won't be used if <arg>_nFormats</arg> does not include the CTF_COLUMN_DESCRIPTOR
				flag.<br/>
				May be <NULL/>.

			@param _rxConnection
				the connection the column belongs to. Won't be used if <arg>_nFormats</arg> does not include the CTF_COLUMN_DESCRIPTOR
				flag.<br/>
				May be <NULL/>.

			@param _nFormats
				supported formats. Must be a combination of the CTF_XXX flags
		*/
		OColumnTransferable(
			const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxForm,
			const ::rtl::OUString& _rFieldName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxColumn,
			const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
			sal_Int32	_nFormats
		);

		/** checks whether or not a column descriptor can be extracted from the data flavor vector given
			@param _rFlavors
				available flavors
			@param _nFormats
				formats to accept
		*/
		static sal_Bool canExtractColumnDescriptor(const DataFlavorExVector& _rFlavors, sal_Int32 _nFormats);

		/** extracts a column descriptor from the transferable given
		*/
		static sal_Bool extractColumnDescriptor(
			const TransferableDataHelper&	_rData,
			::rtl::OUString&				_rDatasource,
			sal_Int32&						_nCommandType,
			::rtl::OUString&				_rCommand,
			::rtl::OUString&				_rFieldName
		);

		/** extracts a column descriptor from the transferable given
		*/

		/** adds the data contained in the object to the given data container
			<p>This method helps you treating this class as simple container class:<br/>
			At the moment, it is a data container and a transferable.
			Using <method>addDataToContainer</method>, you can treat the class as dumb data container,
			doing the Drag'n'Drop with a <type>TransferDataContainer</type> instance (which may contain
			additional formats)</p>
			@TODO
				split this class into a two separate classes: one for the data container aspect, one for
				the transfer aspect
		*/
		void addDataToContainer( TransferDataContainer* _pContainer );

	protected:
		// TransferableHelper overridables
		virtual void		AddSupportedFormats();
		virtual sal_Bool	GetData( const ::com::sun::star::datatransfer::DataFlavor& rFlavor );

		static sal_uInt32	getDescriptorFormatId();

	private:
		void implConstruct(
			const ::rtl::OUString&	_rDatasource,
			const sal_Int32			_nCommandType,
			const ::rtl::OUString&	_rCommand,
			const ::rtl::OUString&	_rFieldName
		);
	};

	//====================================================================
	//= ODataAccessObjectTransferable
	//====================================================================
	/** class for transfering data access objects (tables, queries, statements ...)
	*/
	class ODataAccessObjectTransferable : public TransferableHelper
	{
		::rtl::OUString			m_sCompatibleObjectDescription;
			// needed to provide a SOT_FORMATSTR_ID_SBA_DATAEXCHANGE format

	public:
		/** should be used copying and the connection is needed.
			@param	_rDatasource
				The data source name.
			@param	_nCommandType
				The kind of command. @see com.sun.star.sdbc.CommandType
			@param	_rCommand
				The command, either a name of a table or query or a SQL statement.
		*/
		ODataAccessObjectTransferable(
			const ::rtl::OUString&	_rDatasource,
			const sal_Int32			_nCommandType,
			const ::rtl::OUString&	_rCommand,
			const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection
		);

		/** should be used when copying a query object and no connection is available.
			@param	_rDatasource
				The data source name.
			@param	_nCommandType
				The kind of command. @see com.sun.star.sdbc.CommandType
			@param	_rCommand
				The command, either a name of a table or query or a SQL statement.
		*/
		ODataAccessObjectTransferable(
			const ::rtl::OUString&	_rDatasource,
			const sal_Int32			_nCommandType,
			const ::rtl::OUString&	_rCommand
		);

		/** with this ctor, only the object descriptor format will be provided
		*/
		ODataAccessObjectTransferable(
			const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxLivingForm
		);

		/** checks whether or not an object descriptor can be extracted from the data flavor vector given
			@param _rFlavors
				available flavors
			@param _nFormats
				formats to accept
		*/
		static sal_Bool canExtractObjectDescriptor(const DataFlavorExVector& _rFlavors);

		/** extracts a object descriptor from the transferable given
		*/

	protected:
		virtual void		AddSupportedFormats();
		virtual sal_Bool	GetData( const ::com::sun::star::datatransfer::DataFlavor& rFlavor );
		virtual void		ObjectReleased();

	protected:
	protected:
		void	addCompatibleSelectionDescription(
			const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& _rSelRows
		);
			// normally, a derived class could simply access getDescriptor[daSelection] and place the sequence therein
			// but unfortunately, we have this damned compatible format, and this can't be accessed in
			// derived classes (our class is the only one which should be contaminated with this)

	private:
		void construct(	const ::rtl::OUString&	_rDatasource,
						const sal_Int32			_nCommandType,
						const ::rtl::OUString&	_rCommand,
						const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
						sal_Bool _bAddCommand,
						const ::rtl::OUString& _sActiveCommand);
	};

//........................................................................
}	// namespace svx
//........................................................................

}//end of namespace binfilter
#endif // _SVX_DBAEXCHANGE_HXX_

