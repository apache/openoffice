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



#ifndef SD_PAGE_CACHE_MANAGER_HXX
#define SD_PAGE_CACHE_MANAGER_HXX

#include <sal/types.h>
#include <com/sun/star/uno/XInterface.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>
#include <vector>

class Size;
class SdDrawDocument;
class SdrPage;

namespace sd { namespace slidesorter { namespace view {
class SlideSorterView;
class PageObjectViewObjectContact;
} } }

namespace sd { namespace slidesorter { namespace model {
class SlideSorterModel;
} } }

namespace sd { namespace slidesorter { namespace cache {

namespace css = ::com::sun::star;

class BitmapCache;


/** Provide and manage the preview bitmap caches for all slide sorter
    instances.  There is one cache per active slide sorter plus a small
    number of caches that are no longer in use.  The later are kept to speed
    up the switching between views.
*/
class PageCacheManager
{
public:
    typedef BitmapCache Cache;
    typedef ::std::vector< ::std::pair<Size, ::boost::shared_ptr<BitmapCache> > > BestFittingPageCaches;
    typedef css::uno::Reference<css::uno::XInterface> DocumentKey;

    /** Return the one instance of the PageCacheManager class.
    */
    static ::boost::shared_ptr<PageCacheManager> Instance (void);

    /** Look up the cache for the given model in which the previews have the
        specified size.  If no such cache exists, then one is created.  When
        a new BitmapCache is created its Recycle() method is called with a
        sorted list of existing caches from which the new one initialize its
        previews.
        @return
            The returned cache lives as long as somebody keeps a shared
            pointer and the ReleaseCache() method has not been called.
    */
    ::boost::shared_ptr<Cache> GetCache (
        DocumentKey pDocument,
        const Size& rPreviewSize);

    /** Tell the cache manager to release its own reference to the specified
        cache.  After that the cache will live as long as the caller (and
        maybe others) holds its reference.
    */
    void ReleaseCache (const ::boost::shared_ptr<Cache>& rpCache);

    /** This is an information to the cache manager that the size of preview
        bitmaps in the specified cache has changed.

    */
    ::boost::shared_ptr<Cache> ChangeSize (
        const ::boost::shared_ptr<Cache>& rpCache,
        const Size& rOldPreviewSize,
        const Size& rNewPreviewSize);

    /** Invalidate the preview bitmap for one slide that belongs to the
        specified document.  The bitmaps for this slide in all caches are
        marked as out-of-date and will be re-created when they are requested
        the next time.
    */
    bool InvalidatePreviewBitmap (
        DocumentKey pDocument,
        const SdrPage* pPage);

    /** Invalidate the preview bitmaps for all slides that belong to the
        specified document.  This is necessary after model changes that
        affect e.g. page number fields.
    */
    void InvalidateAllPreviewBitmaps (DocumentKey pDocument);

    /** Invalidate all the caches that are currently in use and destroy
        those that are not.  This is used for example when the high contrast
        mode is turned on or off.
    */
    void InvalidateAllCaches (void);

    /** Call this method when a page has been deleted and its preview
        is not needed anymore.
    */
    void ReleasePreviewBitmap (const SdrPage* pPage);

private:
    /** Singleton instance of the cache manager.  Note that this is a weak
        pointer.  The (implementation class of) ViewShellBase holds a
        shared_ptr so that the cache manager has the same life time as the
        ViewShellBase.
    */
    static ::boost::weak_ptr<PageCacheManager> mpInstance;

    /// List of active caches.
    class PageCacheContainer;
    ::std::auto_ptr<PageCacheContainer> mpPageCaches;

    /// List of inactive, recently used caches.
    class RecentlyUsedPageCaches;
    ::std::auto_ptr<RecentlyUsedPageCaches> mpRecentlyUsedPageCaches;

    /** The maximal number of recently used caches that are kept alive after
        they have become inactive, i.e. after they are not used anymore by a
        slide sorter.
    */
    const sal_uInt32 mnMaximalRecentlyCacheCount;

    PageCacheManager (void);
    ~PageCacheManager (void);

	class Deleter;
	friend class Deleter;

    ::boost::shared_ptr<Cache> GetRecentlyUsedCache(
        DocumentKey pDocument,
        const Size& rSize);

    /** Add the given cache to the list of recently used caches for the
        document.  There is one such list per document.  Each least has at
        most mnMaximalRecentlyCacheCount members.
    */
    void PutRecentlyUsedCache(
        DocumentKey pDocument,
        const Size& rPreviewSize,
        const ::boost::shared_ptr<Cache>& rpCache);

    /** Return a sorted list of the available caches, both active caches and
        those recently used, for the given document.  The sort order is so
        that an exact match of the preview size is at the front.  Other
        caches follow with the largest size first.
    */
    BestFittingPageCaches GetBestFittingCaches (
        DocumentKey pDocument,
        const Size& rPreviewSize);

    /** This method is used internally to initialize a newly created
        BitmapCache with already existing previews.
    */
    void Recycle (
        const ::boost::shared_ptr<Cache>& rpCache,
        DocumentKey pDocument,
        const Size& rPreviewSize);
};

} } } // end of namespace ::sd::slidesorter::cache

#endif
