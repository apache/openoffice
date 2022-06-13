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



#ifndef _SV_SVDATA_HXX
#define _SV_SVDATA_HXX

#include "rtl/ref.hxx"

#include "vos/thread.hxx"

#include "tools/string.hxx"
#include "tools/gen.hxx"
#include "tools/shl.hxx"
#include "tools/link.hxx"
#include "tools/fldunit.hxx"
#include "tools/color.hxx"
#include "tools/debug.hxx"

#include "vcl/vclevent.hxx"
#include "vcl/sv.h"
#include "vcl/dllapi.h"

#include "unotools/options.hxx"

#include "xconnection.hxx"

#include "com/sun/star/uno/Reference.hxx"

#include <hash_map>

namespace com {
namespace sun {
namespace star {
namespace lang {
    class XMultiServiceFactory;
}
namespace frame {
	class XSessionManagerClient;
}
}}}

struct ImplTimerData;
struct ImplFileImageCacheData;
struct ImplConfigData;
class ImplDirectFontSubstitution;
struct ImplHotKey;
struct ImplEventHook;
class Point;
class Rectangle;
class ImpResMgr;
class ResMgr;
class UniqueIndex;
class ImplAccelManager;
class ImplDevFontList;
class ImplFontCache;
class HelpTextWindow;
class ImplTBDragMgr;
class ImplButtonList;
class ImplIdleMgr;
class DbgWindow;
class FloatingWindow;
class AllSettings;
class KeyCode;
class NotifyEvent;
class Timer;
class AutoTimer;
class Help;
class ImageList;
class Image;
class PopupMenu;
class Application;
class OutputDevice;
class Window;
class SystemWindow;
class WorkWindow;
class Dialog;
class VirtualDevice;
class Printer;
class SalFrame;
class SalInstance;
class SalSystem;
class SalProcessWindowList;
class SalTrayList;
class UniqueIdContainer;
class List;
class ImplPrnQueueList;
class ImplVDevCache;
class UnoWrapperBase;
class GraphicConverter;
class ImplWheelWindow;
class SalTimer;
class SalI18NImeStatus;
class DockingManager;
class VclEventListeners2;

namespace vos { class OMutex; }
namespace vos { class OCondition; }
namespace vcl { class DisplayConnection; class SettingsConfigItem; class DeleteOnDeinitBase; }
namespace utl { class DefaultFontConfiguration; class FontSubstConfiguration; }

// -----------------
// - ImplSVAppData -
// -----------------
class LocaleConfigurationListener : public utl::ConfigurationListener
{
public:
	virtual void ConfigurationChanged( utl::ConfigurationBroadcaster*, sal_uInt32 );
};

struct ImplSVAppData
{
    enum ImeStatusWindowMode
    {
        ImeStatusWindowMode_UNKNOWN,
        ImeStatusWindowMode_HIDE,
        ImeStatusWindowMode_SHOW
    };

    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >    mxMSF;
    String*                 mpMSFTempFileName;
    AllSettings*            mpSettings;         // Application settings
	LocaleConfigurationListener* mpCfgListener;
    VclEventListeners*      mpEventListeners;   // listeners for vcl events (eg, extended toolkit)
    VclEventListeners*      mpKeyListeners;     // listeners for key events only (eg, extended toolkit)
    ImplAccelManager*       mpAccelMgr;         // Accelerator Manager
    XubString*              mpAppName;          // Application name
    XubString*              mpAppFileName;      // Abs. Application FileName
    XubString*              mpDisplayName;      // Application Display Name
    String*                 mpFontPath;         // Additional Fontpath
    Help*                   mpHelp;             // Application help
    PopupMenu*              mpActivePopupMenu;  // Actives Popup-Menu (in Execute)
    UniqueIdContainer*      mpUniqueIdCont;     // Fuer Eindeutige Id's
    ImplIdleMgr*            mpIdleMgr;          // Idle-Manager
    ImplWheelWindow*        mpWheelWindow;      // WheelWindow
    ImplHotKey*             mpFirstHotKey;      // HotKey-Verwaltung
    ImplEventHook*          mpFirstEventHook;   // Event-Hooks
    VclEventListeners2*     mpPostYieldListeners;           // post yield listeners
    sal_uLong               mnLastInputTime;                // GetLastInputTime()
    sal_uInt16              mnDispatchLevel;                // DispatchLevel
    sal_uInt16              mnModalMode;                    // ModalMode Count
    sal_uInt16              mnModalDialog;                  // ModalDialog Count
    sal_uInt16              mnAccessCount;                  // AccessHdl Count
    sal_uInt16              mnSysWinMode;                   // Modus, wann SystemWindows erzeugt werden sollen
    sal_uInt16              mnLayout;                       // --- RTL-Flags --- currently not used, only for testing
    short                   mnDialogScaleX;                 // Scale X-Positions and sizes in Dialogs
    sal_Bool                mbInAppMain;                    // is Application::Main() on stack
    sal_Bool                mbInAppExecute;                 // is Application::Execute() on stack
    sal_Bool                mbAppQuit;                      // is Application::Quit() called
    sal_Bool                mbSettingsInit;                 // sal_True: Settings are initialized
    sal_Bool                mbDialogCancel;                 // sal_True: Alle Dialog::Execute()-Aufrufe werden mit return sal_False sofort beendet
    sal_Bool                mbNoYield;                      // Application::Yield will not wait for events if the queue is empty
                                                            // essentially that makes it the same as Application::Reschedule
    long                    mnDefaultLayoutBorder;          // default value in pixel for layout distances used
                                                            // in window arrangers

    bool                    mbConversionMode;               // true allows further optimizations during document conversion in different code areas.

    /** Controls whether showing any IME status window is toggled on or off.

        Only meaningful if showing IME status windows can be toggled on and off
        externally (see Application::CanToggleImeStatusWindow).
     */
    ImeStatusWindowMode meShowImeStatusWindow;

	long m_bEnableAccessInterface;
                            DECL_STATIC_LINK( ImplSVAppData, ImplQuitMsg, void* );

};


// -----------------
// - ImplSVGDIData -
// -----------------

struct ImplSVGDIData
{
    OutputDevice*           mpFirstWinGraphics; // First OutputDevice with a Frame Graphics
    OutputDevice*           mpLastWinGraphics;  // Last OutputDevice with a Frame Graphics
    OutputDevice*           mpFirstVirGraphics; // First OutputDevice with a VirtualDevice Graphics
    OutputDevice*           mpLastVirGraphics;  // Last OutputDevice with a VirtualDevice Graphics
    OutputDevice*           mpFirstPrnGraphics; // First OutputDevice with a InfoPrinter Graphics
    OutputDevice*           mpLastPrnGraphics;  // Last OutputDevice with a InfoPrinter Graphics
    VirtualDevice*          mpFirstVirDev;      // First VirtualDevice
    VirtualDevice*          mpLastVirDev;       // Last VirtualDevice
    Printer*                mpFirstPrinter;     // First Printer
    Printer*                mpLastPrinter;      // Last Printer
    ImplPrnQueueList*       mpPrinterQueueList; // List of all printer queue
    ImplDevFontList*        mpScreenFontList;   // Screen-Font-List
    ImplFontCache*          mpScreenFontCache;  // Screen-Font-Cache
    ImplDirectFontSubstitution* mpDirectFontSubst;// Font-Substitutions defined in Tools - Options - OpenOffice - Fonts
    GraphicConverter*       mpGrfConverter;     // Converter for graphics
    long                    mnRealAppFontX;     // AppFont X-Numenator for 40/tel Width
    long                    mnAppFontX;         // AppFont X-Numenator for 40/tel Width + DialogScaleX
    long                    mnAppFontY;         // AppFont Y-Numenator for 80/tel Height
    sal_Bool                    mbFontSubChanged;   // sal_True: FontSubstitution wurde zwischen Begin/End geaendert
    utl::DefaultFontConfiguration* mpDefaultFontConfiguration;
    utl::FontSubstConfiguration* mpFontSubstConfiguration;
    bool                    mbNativeFontConfig; // true: do not override UI font
    bool                    mbNoXORClipping;    // true: do not use XOR to achieve clipping effects
};


// -----------------
// - ImplSVWinData -
// -----------------

struct ImplSVWinData
{
    Window*                 mpFirstFrame;       // First FrameWindow
    Window*                 mpDefDialogParent;  // Default Dialog Parent
    WorkWindow*             mpAppWin;           // Application-Window
    Window*                 mpFocusWin;         // window, that has the focus
    Window*                 mpActiveApplicationFrame; // the last active application frame, can be used as DefModalDialogParent if no focuswin set
    Window*                 mpCaptureWin;       // window, that has the mouse capture
    Window*                 mpLastDeacWin;      // Window, that need a deactivate (FloatingWindow-Handling)
    DbgWindow*              mpDbgWin;           // debug window
    FloatingWindow*         mpFirstFloat;       // First FloatingWindow in PopupMode
    Dialog*                 mpLastExecuteDlg;   // Erster Dialog, der sich in Execute befindet
    Window*                 mpExtTextInputWin;  // Window, which is in ExtTextInput
    Window*                 mpTrackWin;         // window, that is in tracking mode
    AutoTimer*              mpTrackTimer;       // tracking timer
    ImageList*              mpMsgBoxImgList;    // ImageList for MessageBox
    ImageList*              mpMsgBoxHCImgList;  // ImageList for MessageBox (high contrast mode)
    Window*                 mpAutoScrollWin;    // window, that is in AutoScrollMode mode
    sal_uInt16                  mnTrackFlags;       // tracking flags
    sal_uInt16                  mnAutoScrollFlags;  // auto scroll flags
    sal_Bool                    mbNoDeactivate;     // sal_True: keine Deactivate durchfuehren
	sal_Bool					mbNoSaveFocus;		// sal_True: menus must not save/restore focus
    sal_Bool                    mbNoSaveBackground; // sal_True: save background is unnecessary or even less performant
};


// ------------------
// - ImplSVCtrlData -
// ------------------

typedef std::vector< std::pair< String, FieldUnit > > FieldUnitStringList;

struct ImplSVCtrlData
{
    ImageList*              mpCheckImgList;     // ImageList for CheckBoxes
    ImageList*              mpRadioImgList;     // ImageList for RadioButtons
    ImageList*              mpPinImgList;       // ImageList for PIN
    ImageList*              mpSplitHPinImgList; // ImageList for Horizontal SplitWindows
    ImageList*              mpSplitVPinImgList; // ImageList for Vertical SplitWindows (PIN's)
    ImageList*              mpSplitHArwImgList; // ImageList for Horizontal SplitWindows (Arrows)
    ImageList*              mpSplitVArwImgList; // ImageList for Vertical SplitWindows (Arrows)
    Image*                  mpDisclosurePlus;
    Image*                  mpDisclosurePlusHC;
    Image*                  mpDisclosureMinus;
    Image*                  mpDisclosureMinusHC;
    ImplTBDragMgr*          mpTBDragMgr;        // DragMgr for ToolBox
    sal_uInt16                  mnCheckStyle;       // CheckBox-Style for ImageList-Update
    sal_uInt16                  mnRadioStyle;       // Radio-Style for ImageList-Update
    sal_uLong                   mnLastCheckFColor;  // Letzte FaceColor fuer CheckImage
    sal_uLong                   mnLastCheckWColor;  // Letzte WindowColor fuer CheckImage
    sal_uLong                   mnLastCheckWTextColor;  // Letzte WindowTextColor fuer CheckImage
    sal_uLong                   mnLastCheckLColor;  // Letzte LightColor fuer CheckImage
    sal_uLong                   mnLastRadioFColor;  // Letzte FaceColor fuer RadioImage
    sal_uLong                   mnLastRadioWColor;  // Letzte WindowColor fuer RadioImage
    sal_uLong                   mnLastRadioLColor;  // Letzte LightColor fuer RadioImage
    FieldUnitStringList*    mpFieldUnitStrings; // list with field units
    FieldUnitStringList*    mpCleanUnitStrings; // same list but with some "fluff" like spaces removed
};


// ------------------
// - ImplSVHelpData -
// ------------------

struct ImplSVHelpData
{
    sal_Bool                    mbContextHelp       : 1;    // is ContextHelp enabled
    sal_Bool                    mbExtHelp           : 1;    // is ExtendedHelp enabled
    sal_Bool                    mbExtHelpMode       : 1;    // is in ExtendedHelp Mode
    sal_Bool                    mbOldBalloonMode    : 1;    // BallonMode, befor ExtHelpMode started
    sal_Bool                    mbBalloonHelp       : 1;    // is BalloonHelp enabled
    sal_Bool                    mbQuickHelp         : 1;    // is QuickHelp enabled
    sal_Bool                    mbSetKeyboardHelp   : 1;    // tiphelp was activated by keyboard
    sal_Bool                    mbKeyboardHelp      : 1;    // tiphelp was activated by keyboard
    sal_Bool                    mbAutoHelpId        : 1;    // generate HelpIds
    sal_Bool                    mbRequestingHelp    : 1;    // In Window::RequestHelp
    HelpTextWindow*         mpHelpWin;                  // HelpWindow
    sal_uLong                   mnLastHelpHideTime;         // ticks of last show
};

struct ImplSVNWFData
{
    bool                    mbMenuBarDockingAreaCommonBG; // e.g. WinXP default theme
    bool                    mbDockingAreaSeparateTB;      // individual toolbar backgrounds
                                                          // instead of one for docking area
    bool                    mbToolboxDropDownSeparate;    // two adjacent buttons for
                                                          // toolbox dropdown buttons
    int                     mnMenuFormatExtraBorder;      // inner popup menu border
    bool                    mbFlatMenu;                   // no popup 3D border
    Color                   maMenuBarHighlightTextColor;  // override higlight text color
                                                          // in menubar if not transparent
    bool                    mbOpenMenuOnF10;              // on gnome the first menu opens on F10
    bool                    mbNoFocusRects;               // on Aqua focus rects are not used
    bool                    mbNoBoldTabFocus;             // on Aqua and Gnome the focused tab has not bold text
    bool                    mbCenteredTabs;               // on Aqua, tabs are centered
    bool                    mbNoActiveTabTextRaise;       // on Aqua the text for the selected tab
                                                          // should not "jump up" a pixel
    bool                    mbProgressNeedsErase;         // set true for platforms that should draw the
                                                          // window background before drawing the native
                                                          // progress bar
    bool                    mbCheckBoxNeedsErase;         // set true for platforms that should draw the
                                                          // window background before drawing the native
                                                          // checkbox
    bool                    mbScrollbarJumpPage;          // true for "jump to here" behavior
    int                     mnStatusBarLowerRightOffset;  // amount in pixel to avoid in the lower righthand corner
    bool                    mbCanDrawWidgetAnySize;       // set to true currently on gtk
};


// --------------
// - ImplSVData -
// --------------

struct ImplSVData
{
    void*                   mpSalData;          // SalData
    SalInstance*            mpDefInst;          // Default SalInstance
    Application*            mpApp;              // pApp
    WorkWindow*             mpDefaultWin;       // Default-Window
    sal_Bool                    mbDeInit;             // Is VCL deinitializing
    sal_uLong                   mnThreadCount;      // is VCL MultiThread enabled
    ImplConfigData*         mpFirstConfigData;  // Zeiger auf ersten Config-Block
    ImplTimerData*          mpFirstTimerData;   // list of all running timers
    SalTimer*				mpSalTimer;			// interface to sal event loop/timers
    SalI18NImeStatus*		mpImeStatus;		// interface to ime status window
    SalSystem*				mpSalSystem;		// SalSystem interface
    ResMgr*                 mpResMgr;           // SV-Resource-Manager
    sal_uLong                   mnTimerPeriod;      // current timer period
    sal_uLong                   mnTimerUpdate;      // TimerCallbackProcs on stack
    sal_Bool                    mbNotAllTimerCalled;// sal_True: Es muessen noch Timer abgearbeitet werden
    sal_Bool                    mbNoCallTimer;      // sal_True: No Timeout calls
    ImplSVAppData           maAppData;          // indepen data for class Application
    ImplSVGDIData           maGDIData;          // indepen data for Output classes
    ImplSVWinData           maWinData;          // indepen data for Windows classes
    ImplSVCtrlData          maCtrlData;         // indepen data for Control classes
    ImplSVHelpData          maHelpData;         // indepen data for Help classes
    ImplSVNWFData           maNWFData;
    UnoWrapperBase*         mpUnoWrapper;
    Window*                 mpIntroWindow;      // the splash screen
    DockingManager*         mpDockingManager;
    sal_Bool                    mbIsTestTool;

    vos::OThread::TThreadIdentifier                     mnMainThreadId;
    rtl::Reference< vcl::DisplayConnection >            mxDisplayConnection;

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > mxAccessBridge;
    com::sun::star::uno::Reference< com::sun::star::frame::XSessionManagerClient > xSMClient;
    ::vcl::SettingsConfigItem*			mpSettingsConfigItem;
    std::list< vcl::DeleteOnDeinitBase* >*   mpDeinitDeleteList;
    std::hash_map< int, rtl::OUString >*     mpPaperNames;
};

void        ImplInitSVData();
void        ImplDeInitSVData();
void        ImplDestroySVData();
Window*     ImplGetDefaultWindow();
VCL_PLUGIN_PUBLIC ResMgr*     ImplGetResMgr();
VCL_PLUGIN_PUBLIC ResId VclResId( sal_Int32 nId ); // throws std::bad_alloc if no res mgr
DockingManager*     ImplGetDockingManager();
void        ImplWindowAutoMnemonic( Window* pWindow );

void        ImplUpdateSystemProcessWindow();
Window*     ImplFindWindow( const SalFrame* pFrame, Point& rSalFramePos );

// SVAPP.CXX
sal_Bool        ImplCallHotKey( const KeyCode& rKeyCode );
void        ImplFreeHotKeyData();
void        ImplFreeEventHookData();

// WINPROC.CXX
long        ImplCallPreNotify( NotifyEvent& rEvt );
long        ImplCallEvent( NotifyEvent& rEvt );

extern VCL_PLUGIN_PUBLIC ImplSVData* pImplSVData;
inline VCL_PLUGIN_PUBLIC ImplSVData* ImplGetSVData() { return pImplSVData; }
inline ImplSVData* ImplGetAppSVData() { return ImplGetSVData(); }

bool ImplInitAccessBridge( sal_Bool bAllowCancel, sal_Bool &rCancelled );

FieldUnitStringList* ImplGetFieldUnits();
FieldUnitStringList* ImplGetCleanedFieldUnits();


// -----------------------------------------------------------------------

// -----------------
// - ImplSVEmpyStr -
// -----------------

// Empty-SV-String

inline const String& ImplGetSVEmptyStr()
    { return String::EmptyString(); }
inline const ByteString& ImplGetSVEmptyByteStr()
    { return ByteString::EmptyString(); }

// -----------------------------------------------------------------------

// ----------------------
// - struct ImplDelData -
// ----------------------
// ImplDelData is used as a "dog tag" by a window when it
// does something that could indirectly destroy the window
// TODO: wild destruction of a window should not be possible

struct ImplDelData
{
    ImplDelData*    mpNext;
    const Window*   mpWindow;
    sal_Bool            mbDel;

                    ImplDelData( const Window* pWindow = NULL )
                    : mpNext( NULL ), mpWindow( NULL ), mbDel( sal_False )
                    { if( pWindow ) AttachToWindow( pWindow ); }

    virtual         ~ImplDelData();

    bool            IsDead() const
    {
        DBG_ASSERT( mbDel == sal_False, "object deleted while in use !" );
        return (mbDel!=sal_False);
    }
    sal_Bool /*deprecated */IsDelete() const { return (sal_Bool)IsDead(); }

private:
    void            AttachToWindow( const Window* );
};

// ---------------
// - ImplSVEvent -
// ---------------

struct ImplSVEvent
{
    sal_uLong               mnEvent;
    void*               mpData;
    Link*               mpLink;
    Window*             mpWindow;
    ImplDelData         maDelData;
    sal_Bool                mbCall;
};

#endif  // _SV_SVDATA_HXX
