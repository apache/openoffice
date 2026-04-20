# avmedia — Bazel migration

## Targets

| Target | Output | Notes |
|--------|--------|-------|
| `//main/avmedia:avmedia` | `avmedia.dll` | Media framework + SoundHandler UNO component |
| `//main/avmedia:avmedia_implib` | `avmedia.lib` | Import library for consumers |
| `//main/avmedia:avmediawin` | `avmediawin.dll` | Windows DirectShow (DirectX) media backend |
| `//main/avmedia:avmediawin_implib` | `avmediawin.lib` | Import library for consumers |
| `//main/avmedia:avmedia_headers` | (headers) | Public `inc/avmedia/` headers |

## Source layout

- `source/framework/` — MediaControl, MediaItem, MediaMisc, MediaPlayer, MediaToolbox, SoundHandler
- `source/viewer/` — MediaWindow, MediaWindowBase/Impl, MediaEvent
- `source/win/` — Windows DirectShow Manager, Player, Window, FrameGrabber, WinUno
- `source/inc/` — private headers (mediamisc.hxx)
- `inc/avmedia/` — public API headers (AVMEDIA_DLLPUBLIC)

## Platform scope

Only `avmedia.dll` and `avmediawin.dll` are built — Windows-only target.
GStreamer, MacAVF, Xine, and Java backends are excluded (non-Windows).

## Dependencies

`avmedia.dll`: sal, cppu, cppuhelper, comphelper, tools, unotools, svl, svtools, sfx2, vos, vcl, toolkit, i18npool  
`avmediawin.dll`: sal, cppu, cppuhelper, comphelper, tools, unotools, vcl, i18npool + gdi32, ole32, oleaut32, user32, strmiids

## Notes

- No DEF file needed: all public symbols use `SAL_DLLPUBLIC_EXPORT` (`__declspec(dllexport)`)
- `.hrc`/`.src` resource files declared as `textual_hdrs` — included at compile time; `rsc` localization pipeline skipped
- `snwprintf=_snwprintf` required (VS2008 MSVCRT)
