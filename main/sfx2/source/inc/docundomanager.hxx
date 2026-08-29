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



#ifndef DOCUMENT_UNDO_MANAGER_HXX
#define DOCUMENT_UNDO_MANAGER_HXX

#include "sfx2/sfxbasemodel.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/document/XUndoManager.hpp>
/** === end UNO includes === **/

#include <cppuhelper/implbase1.hxx>

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace svl
{
    class IUndoManager;
}

//......................................................................................................................
namespace sfx2
{
//......................................................................................................................

	//==================================================================================================================
	//= DocumentUndoManager
	//==================================================================================================================
    typedef ::cppu::ImplHelper1 <   ::com::sun::star::document::XUndoManager
                                >   DocumentUndoManager_Base;
    struct DocumentUndoManager_Impl;
    class DocumentUndoManager   :public DocumentUndoManager_Base
                                ,public SfxModelSubComponent
                                ,public ::boost::noncopyable
	{
        friend struct DocumentUndoManager_Impl;

    public:
        DocumentUndoManager( SfxBaseModel& i_document );
        virtual ~DocumentUndoManager();

        // SfxModelSubComponent overridables
        virtual void    disposing();

        // non-UNO API for our owner
        /** determines whether we have an open Undo context. No mutex locking within this method, no disposal check - this
            is the responsibility of the owner.
        */
        bool    isInContext() const;

        // XInterface
        virtual void SAL_CALL acquire(  ) throw ();
        virtual void SAL_CALL release(  ) throw ();

        // XUndoManager
        virtual void SAL_CALL enterUndoContext( const ::rtl::OUString& i_title );
        virtual void SAL_CALL enterHiddenUndoContext(  );
        virtual void SAL_CALL leaveUndoContext(  );
        virtual void SAL_CALL addUndoAction( const ::com::sun::star::uno::Reference< ::com::sun::star::document::XUndoAction >& i_action );
        virtual void SAL_CALL undo(  );
        virtual void SAL_CALL redo(  );
        virtual ::sal_Bool SAL_CALL isUndoPossible(  );
        virtual ::sal_Bool SAL_CALL isRedoPossible(  );
        virtual ::rtl::OUString SAL_CALL getCurrentUndoActionTitle(  );
        virtual ::rtl::OUString SAL_CALL getCurrentRedoActionTitle(  );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getAllUndoActionTitles(  );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getAllRedoActionTitles(  );
        virtual void SAL_CALL clear(  );
        virtual void SAL_CALL clearRedo(  );
        virtual void SAL_CALL reset(  );
        virtual void SAL_CALL addUndoManagerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::document::XUndoManagerListener >& i_listener );
        virtual void SAL_CALL removeUndoManagerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::document::XUndoManagerListener >& i_listener );

        // XLockable, base of XUndoManager
        virtual void SAL_CALL lock(  );
        virtual void SAL_CALL unlock(  );
        virtual ::sal_Bool SAL_CALL isLocked(  );

        // XChild, base of XUndoManager
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getParent(  );
        virtual void SAL_CALL setParent( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& Parent );

    private:
        ::boost::scoped_ptr< DocumentUndoManager_Impl > m_pImpl;
	};

//......................................................................................................................
} // namespace sfx2
//......................................................................................................................

#endif // DOCUMENT_UNDO_MANAGER_HXX
