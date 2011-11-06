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


#ifndef _SVX_UNOWPAGE_HXX
#define _SVX_UNOWPAGE_HXX

#ifndef _COM_SUN_STAR_LANG_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGE_HPP_
#include <com/sun/star/drawing/XDrawPage.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPEGROUPER_HPP_
#include <com/sun/star/drawing/XShapeGrouper.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPECOMBINER_HPP_
#include <com/sun/star/drawing/XShapeCombiner.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPEBINDER_HPP_
#include <com/sun/star/drawing/XShapeBinder.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#ifndef _CPPUHELPER_WEAKAGG_HXX_
#include <cppuhelper/weakagg.hxx>
#endif

#ifndef _SFXLSTNER_HXX
#include <bf_svtools/lstner.hxx>
#endif

#include <cppuhelper/implbase4.hxx> 
#include <comphelper/servicehelper.hxx>

class List;
namespace binfilter {

class SdrPage;
class SdrModel;
class SdrView;
class SdrPageView;
class SdrObject;
class SvxShapeDescriptor;
class SvxShape;
class SvxShapeGroup;
class SvxShapeConnector;
class SvxShapeList;
class SvxDrawPageList;

/***********************************************************************
* Macros fuer Umrechnung Twips<->100tel mm                             *
***********************************************************************/
#define	TWIPS_TO_MM(val) ((val * 127 + 36) / 72)
#define	MM_TO_TWIPS(val) ((val * 72 + 63) / 127)

/***********************************************************************
*                                                                      *
***********************************************************************/
class SvxDrawPage : public ::cppu::WeakAggImplHelper4< ::com::sun::star::drawing::XDrawPage,
											   ::com::sun::star::drawing::XShapeGrouper,
											   ::com::sun::star::lang::XServiceInfo,
											   ::com::sun::star::lang::XUnoTunnel>,
					public SfxListener
{
 protected:

	SdrPage*  		pPage;
	SdrModel* 		pModel;
	SdrView*		pView;

	void	_SelectObjectsInView( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& aShapes, SdrPageView* 	pPageView ) throw ();
	void	_SelectObjectInView( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape, SdrPageView* 	pPageView ) throw();

 public:
	SvxDrawPage( SdrPage* pPage ) throw();
	virtual ~SvxDrawPage() throw();

	// Internals
	SdrPage* GetSdrPage() const { return pPage; }

	// Erzeugen eines SdrObjects und Einfugen in die SdrPage
	SdrObject *CreateSdrObject( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape ) throw();

	// Typ und Inventor bestimmen
	void GetTypeAndInventor( sal_uInt16& rType, sal_uInt32& rInventor, const ::rtl::OUString& aName ) const throw();

	// Erzeugen eines SdrObjects anhand einer Description. Kann von
	// abgeleiteten Klassen dazu benutzt werden, eigene Shapes zu
	// unterstuetzen (z.B. Controls)
	virtual SdrObject *_CreateSdrObject( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape ) throw();

	static SvxShape* CreateShapeByTypeAndInventor( sal_uInt16 nType, sal_uInt32 nInventor, SdrObject *pObj = NULL, SvxDrawPage *pPage = NULL ) throw();

	// Die folgende Methode wird gerufen, wenn ein SvxShape-Objekt angelegt
	// werden soll. abgeleitete Klassen koennen hier eine Ableitung oder
	// ein ein SvxShape aggregierenden Objekt anlegen.
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > _CreateShape( SdrObject *pObj ) const throw();

	UNO3_GETIMPLEMENTATION_DECL( SvxDrawPage )

	// SfxListener
	virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

	// XShapes
    virtual void SAL_CALL add( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL remove( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape ) throw(::com::sun::star::uno::RuntimeException);

	// XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType() throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasElements() throw(::com::sun::star::uno::RuntimeException);

	// XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() throw(::com::sun::star::uno::RuntimeException) ;
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index ) throw(::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

	// XShapeGrouper
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapeGroup > SAL_CALL group( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xShapes ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL ungroup( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapeGroup >& aGroup ) throw(::com::sun::star::uno::RuntimeException);

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName() throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException);
};

}//end of namespace binfilter
#endif

