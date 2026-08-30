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


#ifndef _UNOFRAME_HXX
#define _UNOFRAME_HXX

#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/document/XEmbeddedObjectSupplier2.hpp>
#include <com/sun/star/text/XTextFrame.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/util/XModifyListener.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/document/XEventsSupplier.hpp>

#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase6.hxx>

#include <sfx2/objsh.hxx>

#include <flyenum.hxx>
#include <frmfmt.hxx>
#include <unoevtlstnr.hxx>
#include <unotext.hxx>


class SfxItemPropertSet;
class SdrObject;
class SwDoc;
class SwFmt;
class SwFlyFrmFmt;

class BaseFrameProperties_Impl;
class SwXFrame : public cppu::WeakImplHelper6
<
	::com::sun::star::lang::XServiceInfo,
	::com::sun::star::beans::XPropertySet,
	::com::sun::star::beans::XPropertyState,
	::com::sun::star::drawing::XShape,
	::com::sun::star::container::XNamed,
	::com::sun::star::lang::XUnoTunnel
>,
	public SwClient
{
	SwEventListenerContainer		aLstnrCntnr;
    const SfxItemPropertySet*       m_pPropSet;
    SwDoc*                          m_pDoc;

	const FlyCntType 				eType;

	// Descriptor-interface
	BaseFrameProperties_Impl*		pProps;
	sal_Bool 						bIsDescriptor;
	String 							sName;

    SwPaM*                          m_pCopySource;

protected:
	com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >	mxStyleData;
	com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >	mxStyleFamily;
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

	virtual ~SwXFrame();
public:
	SwXFrame(FlyCntType eSet,
                const SfxItemPropertySet*    pPropSet,
				SwDoc *pDoc ); //Descriptor-If
	SwXFrame(SwFrmFmt& rFrmFmt, FlyCntType eSet,
                const SfxItemPropertySet*    pPropSet);


	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

	//XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );

	TYPEINFO();

	//XNamed
	virtual rtl::OUString SAL_CALL getName(void);
	virtual void SAL_CALL setName(const rtl::OUString& Name_);

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

 	//XPropertyState
    virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName );
    virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName );

   //XShape
    virtual ::com::sun::star::awt::Point SAL_CALL getPosition(  );
    virtual void SAL_CALL setPosition( const ::com::sun::star::awt::Point& aPosition );
    virtual ::com::sun::star::awt::Size SAL_CALL getSize(  );
    virtual void SAL_CALL setSize( const ::com::sun::star::awt::Size& aSize );

	//XShapeDescriptor
	virtual rtl::OUString SAL_CALL getShapeType(void);

	//Basisimplementierung
	//XComponent
    virtual void SAL_CALL dispose(  );
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >  SAL_CALL getAnchor(void);

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

	void attachToRange(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & xTextRange);
    void attach( const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >& xTextRange );

	SwFrmFmt* 		GetFrmFmt() const
	{
		return PTR_CAST ( SwFrmFmt, GetRegisteredIn() );
	}
	FlyCntType 		GetFlyCntType()const {return eType;}

	sal_Bool 			IsDescriptor() const {return bIsDescriptor;}
	void			ResetDescriptor();
    //copy text from a given source PaM
    void            SetSelection(SwPaM& rCopySource);
	static SW_DLLPUBLIC SdrObject *GetOrCreateSdrObject( SwFlyFrmFmt *pFmt );
};

typedef cppu::WeakImplHelper3
<
	::com::sun::star::text::XTextFrame,
	::com::sun::star::container::XEnumerationAccess,
	::com::sun::star::document::XEventsSupplier
>
SwXTextFrameBaseClass;

class SwXTextFrame : public SwXTextFrameBaseClass,
	public SwXText,
	public SwXFrame
{
    const SfxItemPropertSet*    _pPropSet;

protected:
	virtual const SwStartNode *GetStartNode() const;

    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextCursor >
        CreateCursor();

	virtual ~SwXTextFrame();
public:
	SwXTextFrame(SwDoc *pDoc);
	SwXTextFrame(SwFrmFmt& rFmt);

    // FIXME: EVIL HACK:  make available for SwXFrame::attachToRange
    void SetDoc(SwDoc *const pDoc) { SwXText::SetDoc(pDoc); };

    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
    virtual void SAL_CALL acquire(  ) throw();
    virtual void SAL_CALL release(  ) throw();

	//XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	//XTextFrame
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >  SAL_CALL getText(void);

	//XText
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor >  SAL_CALL createTextCursor(void);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor >  SAL_CALL createTextCursorByRange(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & aTextPosition);

	//XEnumerationAccess - frueher XParagraphEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration >  SAL_CALL createEnumeration(void);

	//XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );

	//XTextContent
    virtual void SAL_CALL attach( const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >& xTextRange );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL getAnchor(  );

	//XComponent
    virtual void SAL_CALL dispose(  );
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

    // XEventsSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameReplace > SAL_CALL getEvents(  );

	//XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );

    //XPropertySet
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );

    void * SAL_CALL operator new( size_t ) throw();
	void SAL_CALL operator delete( void * ) throw();
};

typedef cppu::WeakImplHelper2
<
	::com::sun::star::text::XTextContent,
	::com::sun::star::document::XEventsSupplier
>
SwXTextGraphicObjectBaseClass;
class SwXTextGraphicObject : public SwXTextGraphicObjectBaseClass,
							public SwXFrame
{
protected:
	virtual ~SwXTextGraphicObject();
public:
	SwXTextGraphicObject( SwDoc *pDoc );
	SwXTextGraphicObject(SwFrmFmt& rFmt);


    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
    virtual void SAL_CALL acquire(  ) throw();
    virtual void SAL_CALL release(  ) throw();

	//XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	//XTextContent
	virtual void SAL_CALL attach(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & xTextRange);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >  SAL_CALL getAnchor(void);

	//XComponent
	virtual void SAL_CALL dispose(void);
	virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener);
	virtual void SAL_CALL removeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener);

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

    // XEventsSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameReplace > SAL_CALL getEvents(  );
	void * SAL_CALL operator new( size_t ) throw();
	void SAL_CALL operator delete( void * ) throw();
};
/*-----------------20.02.98 11:28-------------------

--------------------------------------------------*/
class SwOLENode;
typedef cppu::WeakImplHelper3
<
	::com::sun::star::text::XTextContent,
	::com::sun::star::document::XEmbeddedObjectSupplier2,
	::com::sun::star::document::XEventsSupplier
>SwXTextEmbeddedObjectBaseClass;

class SwXTextEmbeddedObject : public SwXTextEmbeddedObjectBaseClass,
								public SwXFrame
{
protected:
	virtual ~SwXTextEmbeddedObject();

public:
	SwXTextEmbeddedObject( SwDoc *pDoc );
	SwXTextEmbeddedObject(SwFrmFmt& rFmt);


    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
    virtual void SAL_CALL acquire(  ) throw();
    virtual void SAL_CALL release(  ) throw();

	//XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	//XTextContent
	virtual void SAL_CALL attach(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & xTextRange);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >  SAL_CALL getAnchor(void);

	//XComponent
	virtual void SAL_CALL dispose(void);
	virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener);
	virtual void SAL_CALL removeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener);

	//XEmbeddedObjectSupplier2
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >  SAL_CALL getEmbeddedObject(void);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::embed::XEmbeddedObject > SAL_CALL getExtendedControlOverEmbeddedObject(void);
    virtual ::sal_Int64 SAL_CALL getAspect();
    virtual void SAL_CALL setAspect( ::sal_Int64 _aspect );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > SAL_CALL getReplacementGraphic();

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

    // XEventsSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameReplace > SAL_CALL getEvents(  );
	void * SAL_CALL operator new( size_t ) throw();
	void SAL_CALL operator delete( void * ) throw();
};



class SwXOLEListener : public cppu::WeakImplHelper1
<
	::com::sun::star::util::XModifyListener
>,
	public SwClient
{
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > xOLEModel;
//    SfxObjectShell* GetObjShell( const SwFmt& rFmt,
//                                    SwOLENode** ppNd = 0 ) const;
    SwFmt*       GetFmt() const    {  return (SwFmt*)GetRegisteredIn(); }
public:
    SwXOLEListener(SwFmt& rOLEFmt, ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > xOLE);
    ~SwXOLEListener();
    TYPEINFO();

// ::com::sun::star::lang::XEventListener
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );

// ::com::sun::star::util::XModifyListener
    virtual void SAL_CALL modified( const ::com::sun::star::lang::EventObject& aEvent );

protected:
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);
};



#endif
