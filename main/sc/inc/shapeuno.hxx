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



#ifndef SC_SHAPEUNO_HXX
#define SC_SHAPEUNO_HXX

#include <tools/solar.h>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/text/XTextContent.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/document/XEventsSupplier.hpp>
#include <com/sun/star/container/XChild.hpp>

#include <cppuhelper/implbase5.hxx>
#include <cppuhelper/implbase1.hxx>

namespace com { namespace sun { namespace star {
	namespace uno {
		class XAggregation;
	}
	namespace drawing {
		class XShape;
	}
}}}

class SdrObject;
struct SvEventDescription;
class ShapeUnoEventAccessImpl;

//------------------------------------------------------------------------

//	object which aggregates all svx shape objects,
//	to add own properties

typedef ::cppu::WeakImplHelper5 <   ::com::sun::star::beans::XPropertySet
					            ,   ::com::sun::star::beans::XPropertyState
					            ,   ::com::sun::star::text::XTextContent
					            ,   ::com::sun::star::document::XEventsSupplier
                                ,   ::com::sun::star::lang::XServiceInfo
                                >   ScShapeObj_Base;
typedef ::cppu::ImplHelper1     <   ::com::sun::star::text::XText
                                >   ScShapeObj_TextBase;
typedef ::cppu::ImplHelper1     <   ::com::sun::star::container::XChild
                                >   ScShapeObj_ChildBase;
class ScShapeObj    :public ScShapeObj_Base
                    ,public ScShapeObj_TextBase
                    ,public ScShapeObj_ChildBase
{
private:
	::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation > mxShapeAgg;
    // cached pointers to avoid repeated queryAggregation calls:
    ::com::sun::star::beans::XPropertySet*                                  pShapePropertySet;
    ::com::sun::star::beans::XPropertyState*                                pShapePropertyState;
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > mxPropSetInfo;
    com::sun::star::uno::Sequence< sal_Int8 >*                              pImplementationId;
    bool																	bIsTextShape;
    bool                                                                    bIsNoteCaption;
    bool                                                                    bInitializedNotifier;

	SdrObject* GetSdrObject() const throw();

    void                    GetShapePropertySet();
    void                    GetShapePropertyState();

friend class ShapeUnoEventAccessImpl;

public:
	static const SvEventDescription* GetSupportedMacroItems();

							// ctor modifies xShape parameter
							ScShapeObj( ::com::sun::star::uno::Reference<
											::com::sun::star::drawing::XShape > & xShape );
	virtual					~ScShapeObj();

							// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
								const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();
	virtual void SAL_CALL	setPropertyValue( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Any& aValue );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
									const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL	addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& xListener );
	virtual void SAL_CALL	removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& aListener );
	virtual void SAL_CALL	addVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );
	virtual void SAL_CALL	removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );

							// XPropertyState
	virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState(
									const ::rtl::OUString& PropertyName );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL
							getPropertyStates( const ::com::sun::star::uno::Sequence<
										::rtl::OUString >& aPropertyName );
	virtual void SAL_CALL	setPropertyToDefault( const ::rtl::OUString& PropertyName );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault(
									const ::rtl::OUString& aPropertyName );

							// XTextContent
	virtual void SAL_CALL	attach(const ::com::sun::star::uno::Reference<
									::com::sun::star::text::XTextRange > & xTextRange);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL
							getAnchor(void);

							// XComponent
	virtual void SAL_CALL	dispose(void);
	virtual void SAL_CALL	addEventListener(const ::com::sun::star::uno::Reference<
									::com::sun::star::lang::XEventListener > & aListener);
	virtual void SAL_CALL	removeEventListener(const ::com::sun::star::uno::Reference<
									::com::sun::star::lang::XEventListener > & aListener);

							// XText
	virtual void SAL_CALL	insertTextContent( const ::com::sun::star::uno::Reference<
									::com::sun::star::text::XTextRange >& xRange,
								const ::com::sun::star::uno::Reference<
									::com::sun::star::text::XTextContent >& xContent,
								sal_Bool bAbsorb );
	virtual void SAL_CALL	removeTextContent( const ::com::sun::star::uno::Reference<
								::com::sun::star::text::XTextContent >& xContent );

							// XSimpleText
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor > SAL_CALL
							createTextCursor();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor > SAL_CALL
							createTextCursorByRange( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& aTextPosition );
	virtual void SAL_CALL	insertString( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& xRange,
										const ::rtl::OUString& aString, sal_Bool bAbsorb );
	virtual void SAL_CALL	insertControlCharacter( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& xRange,
										sal_Int16 nControlCharacter, sal_Bool bAbsorb );

							// XTextRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > SAL_CALL
							getText();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL
							getStart();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL
							getEnd();
	virtual ::rtl::OUString SAL_CALL getString();
	virtual void SAL_CALL	setString( const ::rtl::OUString& aString );

    // XChild
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getParent();
    virtual void SAL_CALL setParent( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& xParent );

							// XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();

                            // XEventsSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameReplace > SAL_CALL getEvents();

                            // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  ) ;
    virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );
};

#endif
