# sot — Bazel Migration

Produces `sot.dll` (OLE/compound document storage library).

## Targets

| Target | Output |
|--------|--------|
| `//main/sot:sot` | `sot.dll` |
| `//main/sot:sot_headers` | public headers (`inc/sot/`) |
| `//main/sot:sot_implib` | `sot.if.lib` (import library) |

## Source layout

- `source/base/` — data format exchange, factory, file list, object base
- `source/sdstor/` — structured data storage (OLE compound document format)
- `source/unoolestorage/` — UNO `OLESimpleStorage` component wrapper

## Key notes

- `/Imain/sot/source/sdstor` and `/Imain/sot/source/unoolestorage` required so private
  headers (`stgcache.hxx`, `stgelem.hxx`, `xolesimplestorage.hxx`, etc.) resolve during
  compilation of their sibling `.cxx` files.
- `/Zc:wchar_t-` required for `sal_Unicode` compatibility (VS2008 native `wchar_t`).
- Links `ole32.lib` for Win32 compound document / OLE storage APIs.
- No DEF file — all exports via `SOT_DLLPUBLIC` macro (`SAL_DLLPUBLIC_EXPORT`).
- Dependencies: `tools` (tl.dll), `unotools` (utl.dll), `ucbhelper`, `comphelper`, `cppu`, `cppuhelper`, `sal`.

# warnings during build:
INFO: From Compiling main/unotools/source/config/eventcfg.cxx:
eventcfg.cxx
C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\INCLUDE\../../VC/include/functional(110) : warning C4800: 'sal_Bool' : forcing value to bool 'true' or 'false' (performance warning)
        C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\INCLUDE\../../VC/include/functional(109) : while compiling class template member function 'bool std::equal_to<_Ty>::operator ()(const _Ty &,const _Ty &) const'
        with
        [
            _Ty=rtl::OUString
        ]
        C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\INCLUDE\xhash(184) : see reference to class template instantiation 'std::equal_to<_Ty>' being compiled
        with
        [
            _Ty=rtl::OUString
        ]
        C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\INCLUDE\../../VC/include/unordered_map(74) : see reference to class template instantiation 'stdext::_Hash_compare<_Kty,_Hasher,_Keyeq>' being compiled
        with
        [
            _Kty=rtl::OUString,
            _Hasher=rtl::OUStringHash,
            _Keyeq=std::equal_to<rtl::OUString>
        ]
        C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\INCLUDE\xhash(191) : see reference to class template instantiation 'std::tr1::_Umap_traits<_Kty,_Ty,_Tr,_Alloc,_Mfl>' being compiled
        with
        [
            _Kty=rtl::OUString,
            _Ty=rtl::OUString,
            _Tr=stdext::_Hash_compare<rtl::OUString,rtl::OUStringHash,std::equal_to<rtl::OUString>>,
            _Alloc=std::allocator<std::pair<const rtl::OUString,rtl::OUString>>,
            _Mfl=false
        ]
        C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\INCLUDE\../../VC/include/unordered_map(86) : see reference to class template instantiation 'stdext::_Hash<_Traits>' being compiled
        with
        [
            _Traits=std::tr1::_Umap_traits<rtl::OUString,rtl::OUString,stdext::_Hash_compare<rtl::OUString,rtl::OUStringHash,std::equal_to<rtl::OUString>>,std::allocator<std::pair<const rtl::OUString,rtl::OUString>>,false>
        ]
        C:\users\legin\_bazel_legin\avzk3pwo\execroot\_main\main\stlport\systemstl\hash_map(61) : see reference to class template instantiation 'std::tr1::unordered_map<_Kty,_Ty,_Hasher,_Keyeq>' being compiled
        with
        [
            _Kty=rtl::OUString,
            _Ty=rtl::OUString,
            _Hasher=rtl::OUStringHash,
            _Keyeq=std::equal_to<rtl::OUString>
        ]
        C:\users\legin\_bazel_legin\avzk3pwo\execroot\_main\bazel-out\x64_windows-fastbuild\bin\main\unotools\_virtual_includes\unotools_headers\unotools/eventcfg.hxx(71) : see reference to class template instantiation 'std::hash_map<__K,__T,__H,__E>' being compiled      
        with
        [
            __K=rtl::OUString,
            __T=rtl::OUString,
            __H=rtl::OUStringHash,
            __E=std::equal_to<rtl::OUString>
        ]