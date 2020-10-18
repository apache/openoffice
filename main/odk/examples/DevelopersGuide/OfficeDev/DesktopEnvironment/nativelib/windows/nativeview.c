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



#include <windows.h>

#include "jawt.h"
#include "jawt_md.h"
#include "NativeView.h"

#define MY_ASSERT(X,S) if (!X) { fprintf(stderr,"%s\n",S); return 0L;}

#define SYSTEM_WIN32   1
#define SYSTEM_WIN16   2
#define SYSTEM_JAVA    3
#define SYSTEM_OS2     4
#define SYSTEM_MAC     5
#define SYSTEM_XWINDOW 6

// property name to register own window procedure on hwnd
#define OLD_PROC_KEY "oldwindowproc"
// signature of this window procedure
static LRESULT APIENTRY NativeViewWndProc( HWND , UINT , WPARAM , LPARAM );

/*****************************************************************************
 *
 * Class      : NativeView
 * Method     : getNativeWindowSystemType
 * Signature  : ()I
 * Description: returns an identifier for the current operating system
 */
JNIEXPORT jint JNICALL Java_NativeView_getNativeWindowSystemType
  (JNIEnv * env, jobject obj_this)
{
    return (SYSTEM_WIN32);
}

/*****************************************************************************
 *
 * Class      : NativeView
 * Method     : getNativeWindow
 * Signature  : ()J
 * Description: returns the native systemw window handle of this object
 */
JNIEXPORT jlong JNICALL Java_NativeView_getNativeWindow
  (JNIEnv * env, jobject obj_this)
{
    jboolean                      result  ;
    jint                          lock    ;
    JAWT                          awt     ;
    JAWT_DrawingSurface*          ds      ;
    JAWT_DrawingSurfaceInfo*      dsi     ;
    JAWT_Win32DrawingSurfaceInfo* dsi_win ;
    HDC                           hdc     ;
    HWND                          hWnd    ;
    LONG                          hFuncPtr;

	/* Get the AWT */
	awt.version = JAWT_VERSION_1_3;
    result      = JAWT_GetAWT(env, &awt);
    MY_ASSERT(result!=JNI_FALSE,"wrong jawt version");

    /* Get the drawing surface */
	if ((ds = awt.GetDrawingSurface(env, obj_this)) == NULL)
		return 0L;

	/* Lock the drawing surface */
	lock = ds->Lock(ds);
    MY_ASSERT((lock & JAWT_LOCK_ERROR)==0,"can't lock the drawing surface");

	/* Get the drawing surface info */
	dsi = ds->GetDrawingSurfaceInfo(ds);

	/* Get the platform-specific drawing info */
	dsi_win = (JAWT_Win32DrawingSurfaceInfo*)dsi->platformInfo;
    hdc     = dsi_win->hdc;
    hWnd    = dsi_win->hwnd;

	/* Free the drawing surface info */
	ds->FreeDrawingSurfaceInfo(dsi);
	/* Unlock the drawing surface */
	ds->Unlock(ds);
	/* Free the drawing surface */
	awt.FreeDrawingSurface(ds);

    /* Register own window procedure
       Do it one time only! Otherwise
       multiple instances will be registered
       and calls on such construct produce
       a stack overflow.
     */
    if (GetProp( hWnd, OLD_PROC_KEY )==0)
    {
        hFuncPtr = SetWindowLong( hWnd, GWL_WNDPROC, (DWORD)NativeViewWndProc );
        SetProp( hWnd, OLD_PROC_KEY, (HANDLE)hFuncPtr );
    }

    return ((jlong)hWnd);
}

/*****************************************************************************
 *
 * Class      : -
 * Method     : NativeViewWndProc
 * Signature  : -
 * Description: registered window handler to intercept window messages between
 *              java and office process
 */
static LRESULT APIENTRY NativeViewWndProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HANDLE hFuncPtr;

    /* resize new created child window to fill out the java window complete */
    if (uMsg==WM_PARENTNOTIFY)
    {
        if (wParam == WM_CREATE)
        {
            RECT rect;
            HWND hChild = (HWND) lParam;

            GetClientRect(hWnd, &rect);

            SetWindowPos(hChild,
                        NULL,
                        rect.left,
                        rect.top,
                        rect.right - rect.left,
                        rect.bottom - rect.top,
                        SWP_NOZORDER);
        }
    }
    /* handle normal resize events */
    else if(uMsg==WM_SIZE)
    {
        WORD newHeight = HIWORD(lParam);
        WORD newWidth  = LOWORD(lParam);
        HWND hChild    = GetWindow(hWnd, GW_CHILD);

        if (hChild != NULL)
            SetWindowPos(hChild, NULL, 0, 0, newWidth, newHeight, SWP_NOZORDER);
    }

    /* forward request to original handler which is intercepted by this window procedure */
    hFuncPtr = GetProp(hWnd, OLD_PROC_KEY);
    MY_ASSERT(hFuncPtr,"lost original window proc handler");
    return CallWindowProc( hFuncPtr, hWnd, uMsg, wParam, lParam);
}
