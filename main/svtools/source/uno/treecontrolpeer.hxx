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



#ifndef _TREE_CONTROL_PEER_HXX_
#define _TREE_CONTROL_PEER_HXX_

#include <com/sun/star/awt/tree/XTreeControl.hpp>
#include <com/sun/star/awt/tree/XTreeDataModel.hpp>
#include <com/sun/star/graphic/XGraphicProvider.hpp>

#include <toolkit/awt/vclxwindow.hxx>
#include <toolkit/awt/vclxwindows.hxx>

#include <vcl/image.hxx>

//#include <comphelper/uno3.hxx>
#include <cppuhelper/implbase2.hxx>


class UnoTreeListEntry;
class TreeControlPeer;
class UnoTreeListBoxImpl;
class TreeNodeMap;

//	----------------------------------------------------
//	class TreeControlPeer
//	----------------------------------------------------
class TreeControlPeer :	public ::cppu::ImplInheritanceHelper2< VCLXWindow, ::com::sun::star::awt::tree::XTreeControl, ::com::sun::star::awt::tree::XTreeDataModelListener >
{
	friend class UnoTreeListBoxImpl;
	friend class UnoTreeListEntry;
public:
	TreeControlPeer();
	virtual ~TreeControlPeer();

	Window* createVclControl( Window* pParent, sal_Int64 nWinStyle );

	// VCLXWindow
	virtual void SetWindow( Window* pWindow );

	// ::com::sun::star::view::XSelectionSupplier
    virtual ::sal_Bool SAL_CALL select( const ::com::sun::star::uno::Any& xSelection );
    virtual ::com::sun::star::uno::Any SAL_CALL getSelection(  );
    virtual void SAL_CALL addSelectionChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::view::XSelectionChangeListener >& xListener );
    virtual void SAL_CALL removeSelectionChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::view::XSelectionChangeListener >& xListener );

	// ::com::sun::star::view::XMultiSelectionSupplier
    virtual ::sal_Bool SAL_CALL addSelection( const ::com::sun::star::uno::Any& Selection );
    virtual void SAL_CALL removeSelection( const ::com::sun::star::uno::Any& Selection );
    virtual void SAL_CALL clearSelection(  );
    virtual ::sal_Int32 SAL_CALL getSelectionCount(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL createSelectionEnumeration(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL createReverseSelectionEnumeration(  );

    // ::com::sun::star::awt::XTreeControl
    virtual ::rtl::OUString SAL_CALL getDefaultExpandedGraphicURL();
    virtual void SAL_CALL setDefaultExpandedGraphicURL( const ::rtl::OUString& _defaultexpandedgraphicurl );
    virtual ::rtl::OUString SAL_CALL getDefaultCollapsedGraphicURL();
    virtual void SAL_CALL setDefaultCollapsedGraphicURL( const ::rtl::OUString& _defaultcollapsedgraphicurl );
    virtual ::sal_Bool SAL_CALL isNodeExpanded( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& Node );
    virtual ::sal_Bool SAL_CALL isNodeCollapsed( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& Node );
    virtual void SAL_CALL makeNodeVisible( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& Node );
    virtual ::sal_Bool SAL_CALL isNodeVisible( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& Node );
    virtual void SAL_CALL expandNode( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& Node );
    virtual void SAL_CALL collapseNode( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& Node );
    virtual void SAL_CALL addTreeExpansionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeExpansionListener >& Listener );
    virtual void SAL_CALL removeTreeExpansionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeExpansionListener >& Listener );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode > SAL_CALL getNodeForLocation( ::sal_Int32 x, ::sal_Int32 y );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode > SAL_CALL getClosestNodeForLocation( ::sal_Int32 x, ::sal_Int32 y );
    virtual ::com::sun::star::awt::Rectangle SAL_CALL getNodeRect( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& Node );
    virtual ::sal_Bool SAL_CALL isEditing(  );
    virtual ::sal_Bool SAL_CALL stopEditing(  );
    virtual void SAL_CALL cancelEditing(  );
    virtual void SAL_CALL startEditingAtNode( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& Node );
    virtual void SAL_CALL addTreeEditListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeEditListener >& Listener );
    virtual void SAL_CALL removeTreeEditListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeEditListener >& Listener );

    // ::com::sun::star::awt::tree::TreeDataModelListener
    virtual void SAL_CALL treeNodesChanged( const ::com::sun::star::awt::tree::TreeDataModelEvent& aEvent );
    virtual void SAL_CALL treeNodesInserted( const ::com::sun::star::awt::tree::TreeDataModelEvent& aEvent );
    virtual void SAL_CALL treeNodesRemoved( const ::com::sun::star::awt::tree::TreeDataModelEvent& aEvent );
    virtual void SAL_CALL treeStructureChanged( const ::com::sun::star::awt::tree::TreeDataModelEvent& aEvent );

    // XEventListener
    void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );

	// ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize();
    ::com::sun::star::awt::Size SAL_CALL getPreferredSize();
    ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& aNewSize );

	// ::com::sun::star::awt::XVclWindowPeer
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

private:
	UnoTreeListEntry* getEntry( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& xNode, bool bThrow = true );

	void disposeControl();

	bool onEditingEntry( UnoTreeListEntry* pEntry );
	bool onEditedEntry( UnoTreeListEntry* pEntry, const XubString& rNewText );

	void eraseTree( UnoTreeListBoxImpl& rTree );
	void fillTree( UnoTreeListBoxImpl& rTree, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeDataModel >& xDataModel );
	void addNode( UnoTreeListBoxImpl& rTree, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& xNode, UnoTreeListEntry* pParentEntry );

	UnoTreeListEntry* createEntry( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& xNode, UnoTreeListEntry* pParent, sal_uLong nPos = LIST_APPEND );
	bool updateEntry( UnoTreeListEntry* pEntry );

	void updateTree( const ::com::sun::star::awt::tree::TreeDataModelEvent& rEvent, bool bRecursive );
	void updateNode( UnoTreeListBoxImpl& rTree, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& xNode, bool bRecursive );
	void updateChildNodes( UnoTreeListBoxImpl& rTree, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& xParentNode, UnoTreeListEntry* pParentEntry );

	::rtl::OUString getEntryString( const ::com::sun::star::uno::Any& rValue );

	UnoTreeListBoxImpl& getTreeListBoxOrThrow() const;
	void ChangeNodesSelection( const ::com::sun::star::uno::Any& rSelection, bool bSelect, bool bSetSelection );

	void onChangeDataModel( UnoTreeListBoxImpl& rTree, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeDataModel >& xDataModel );

	void onSelectionChanged();
	void onRequestChildNodes( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& xNode );
	bool onExpanding( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& xNode, bool bExpanding );
	void onExpanded( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeNode >& xNode, bool bExpanding );

	void onChangeRootDisplayed( sal_Bool bIsRootDisplayed );

	void addEntry( UnoTreeListEntry* pEntry );
	void removeEntry( UnoTreeListEntry* pEntry );

	bool loadImage( const ::rtl::OUString& rURL, Image& rImage );

private:
	::com::sun::star::uno::Reference< ::com::sun::star::awt::tree::XTreeDataModel >mxDataModel;
	TreeSelectionListenerMultiplexer maSelectionListeners;
	TreeExpansionListenerMultiplexer maTreeExpansionListeners;
	TreeEditListenerMultiplexer maTreeEditListeners;
	sal_Bool mbIsRootDisplayed;
	UnoTreeListBoxImpl* mpTreeImpl;
	sal_Int32 mnEditLock;
	::rtl::OUString msDefaultCollapsedGraphicURL;
	::rtl::OUString msDefaultExpandedGraphicURL;
	Image maDefaultExpandedImage;
	Image maDefaultCollapsedImage;
	TreeNodeMap* mpTreeNodeMap;
	::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphicProvider > mxGraphicProvider;
};

#endif // _TREE_CONTROL_PEER_HXX_
