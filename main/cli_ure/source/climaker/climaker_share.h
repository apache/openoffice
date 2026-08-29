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



#using <mscorlib.dll>
#using <cli_basetypes.dll>

#include <vcclr.h>

#include "osl/diagnose.h"
#include "com/sun/star/reflection/XConstantTypeDescription.hpp"
#include "com/sun/star/reflection/XConstantsTypeDescription.hpp"
#include "com/sun/star/reflection/XEnumTypeDescription.hpp"
#include "com/sun/star/reflection/XInterfaceTypeDescription2.hpp"
#include "com/sun/star/reflection/XCompoundTypeDescription.hpp"
#include "com/sun/star/reflection/XServiceTypeDescription2.hpp"
#include "com/sun/star/reflection/XSingletonTypeDescription2.hpp"
#include "com/sun/star/reflection/XInterfaceMethodTypeDescription.hpp"

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )


namespace css = ::com::sun::star;

namespace climaker
{

//------------------------------------------------------------------------------
extern bool g_verbose;

ref struct Constants
{
    // literal, not "static const": in C++/CLI a compile-time constant field is
    // spelled literal, and it is substituted at the use site exactly as the
    // S"..." initialised statics were.
    literal ::System::String ^ sUnoVoid = "void";
    literal ::System::String ^ sUnoType = "type";
    literal ::System::String ^ sUnoAny =  "any";
    literal ::System::String ^ sUnoBool = "boolean";
    literal ::System::String ^ sUnoByte = "byte";
    literal ::System::String ^ sUnoChar = "char";
    literal ::System::String ^ sUnoShort = "short";
    literal ::System::String ^ sUnoUShort = "unsigned short";
    literal ::System::String ^ sUnoLong = "long";
    literal ::System::String ^ sUnoULong = "unsigned long";
    literal ::System::String ^ sUnoHyper = "hyper";
    literal ::System::String ^ sUnoUHyper = "unsigned hyper";
    literal ::System::String ^ sUnoString = "string";
    literal ::System::String ^ sUnoFloat = "float";
    literal ::System::String ^ sUnoDouble = "double";
    literal ::System::String ^ sUnoXInterface = "com.sun.star.uno.XInterface";
    literal ::System::String ^ sBrackets = "[]";

    literal System::String ^ sObject = "System.Object";
    literal System::String ^ sType = "System.Type";
    literal System::String ^ sUnoidl = "unoidl.";
    literal System::String ^ sVoid = "System.Void";
    literal System::String ^ sAny = "uno.Any";
    literal System::String ^ sBoolean = "System.Boolean";
    literal System::String ^ sChar = "System.Char";
    literal System::String ^ sByte = "System.Byte";
    literal System::String ^ sInt16 = "System.Int16";
    literal System::String ^ sUInt16 = "System.UInt16";
    literal System::String ^ sInt32 = "System.Int32";
    literal System::String ^ sUInt32 = "System.UInt32";
    literal System::String ^ sInt64 = "System.Int64";
    literal System::String ^ sUInt64 = "System.UInt64";
    literal System::String ^ sString = "System.String";
    literal System::String ^ sSingle = "System.Single";
    literal System::String ^ sDouble = "System.Double";
    literal System::String ^ sComma = ",";

};

//------------------------------------------------------------------------------
inline ::System::String ^ ustring_to_String( ::rtl::OUString const & ustr )
{
    return gcnew ::System::String( ustr.getStr(), 0, ustr.getLength() );
}

//------------------------------------------------------------------------------
inline ::rtl::OUString String_to_ustring( ::System::String ^ str )
{
    OSL_ASSERT( sizeof (wchar_t) == sizeof (sal_Unicode) );
    pin_ptr< wchar_t const > chars = PtrToStringChars( str );
    return ::rtl::OUString( chars, str->Length );
}

/* If the argument type is a typedef for an interface then the interface
   type description is returned, otherwise an exception is thrown.
*/
css::uno::Reference< css::reflection::XInterfaceTypeDescription2 >
resolveInterfaceTypedef(const css::uno::Reference<css::reflection::XTypeDescription>& type);

const ::System::Reflection::MethodAttributes c_ctor_method_attr =
(::System::Reflection::MethodAttributes)
    (::System::Reflection::MethodAttributes::Public |
     ::System::Reflection::MethodAttributes::HideBySig |
     ::System::Reflection::MethodAttributes::SpecialName |
     ::System::Reflection::MethodAttributes::RTSpecialName
     /* | xxx todo: ??? compiler does not know Instance ???
        ::System::Reflection::MethodAttributes::Instance*/);

//==============================================================================
ref class TypeEmitter : public ::System::IDisposable
{
    ::System::Reflection::Emit::ModuleBuilder ^ m_module_builder;
    cli::array< ::System::Reflection::Assembly ^ > ^ m_extra_assemblies;

    ::System::Reflection::MethodInfo ^ m_method_info_Type_GetTypeFromHandle;

    ::System::Type ^ m_type_Exception;
    ::System::Type ^ get_type_Exception();
    ::System::Type ^ m_type_RuntimeException;
    ::System::Type ^ get_type_RuntimeException();

    ::System::Reflection::Emit::CustomAttributeBuilder ^ get_service_exception_attribute(
        const css::uno::Reference<css::reflection::XServiceConstructorDescription> & ctorDesc);
    ::System::Reflection::Emit::CustomAttributeBuilder ^ get_iface_method_exception_attribute(
        const css::uno::Reference< css::reflection::XInterfaceMethodTypeDescription >& xMethod );
    ::System::Reflection::Emit::CustomAttributeBuilder ^ get_exception_attribute(
        const css::uno::Sequence<css::uno::Reference<
        css::reflection::XCompoundTypeDescription > >& seq_exceptionsTd );
/* Creates ::System::Type object for UNO exceptions. The UNO exceptions are
       obtained by
       com::sun::star::reflection::XServiceConstructorDescription::getExceptions
       In a first step the respective CLI types are created. Then it is examined
       if a Type represents a super class of another class. If so the Type of the
       derived class is discarded. For example there are a uno RuntimeException and
       a DeploymentException which inherits RuntimeException. Then only the cli Type
       of the RuntimeException is returned.
       The purpose of this function is to provide exceptions for which catch blocks
       are generated in the service constructor code.

       It is always an instance of an ArrayList returned, even if the sequence argument
       does not contain elements.
    */
    ::System::Collections::ArrayList ^ get_service_ctor_method_exceptions_reduced(
        const css::uno::Sequence<
        css::uno::Reference<css::reflection::XCompoundTypeDescription> > & seqExceptionsTd);


    // NOTE on the m_xType members below: css::reflection::* are NATIVE UNO
    // interface pointers, not managed handles, and stay pointers.  Only the
    // System::* members become handles.
    ref class iface_entry
    {
    public:
        css::reflection::XInterfaceTypeDescription2 * m_xType;
        ::System::Reflection::Emit::TypeBuilder ^ m_type_builder;
    };
    ::System::Collections::Hashtable ^ m_incomplete_ifaces;
    // Names type_resolve is part-way through answering.  AppDomain's
    // TypeResolve event has no re-entrancy guard of its own.
    ::System::Collections::Hashtable ^ m_resolving;
    ::System::Type ^ complete_iface_type( iface_entry ^ entry );

    ref class struct_entry
    {
    public:
         css::reflection::XCompoundTypeDescription * m_xType;
        ::System::Reflection::Emit::TypeBuilder ^ m_type_builder;
        ::System::Type ^ m_base_type;

        cli::array< ::System::String ^ > ^ m_member_names;
        cli::array< ::System::Type ^ > ^ m_param_types;
        ::System::Reflection::ConstructorInfo ^ m_default_ctor;
        ::System::Reflection::ConstructorInfo ^ m_ctor;
    };
    ::System::Collections::Hashtable ^ m_incomplete_structs;
    ::System::Type ^ complete_struct_type( struct_entry ^ entry );

    /*  returns the type for the name. If it is a struct then it may
        complete the struct if not already done. This also refers to its
        base types.

        @param sName
            the full name of the type.
        @return the type object for sName. Not necessarily a struct.
    */
    ::System::Type ^ get_complete_struct( ::System::String ^ sName);

    ref class service_entry
    {
    public:
        ::System::Reflection::Emit::TypeBuilder ^ m_type_builder;
        css::reflection::XServiceTypeDescription2 * m_xType;
    };
    ::System::Collections::Hashtable ^ m_incomplete_services;
    ::System::Type ^ complete_service_type(service_entry ^ entry);

    ref class singleton_entry
    {
    public:
        ::System::Reflection::Emit::TypeBuilder ^ m_type_builder;
        css::reflection::XSingletonTypeDescription2 * m_xType;
    };


    ::System::Collections::Hashtable ^ m_incomplete_singletons;
    ::System::Type ^ complete_singleton_type(singleton_entry ^ entry);


    ::System::Collections::Hashtable ^ m_generated_structs;

    ::System::Type ^ get_type(
        ::System::String ^ cli_name, bool throw_exc );
    ::System::Type ^ get_type(
        css::uno::Reference<
        css::reflection::XConstantTypeDescription > const & xType );
    ::System::Type ^ get_type(
        css::uno::Reference<
        css::reflection::XConstantsTypeDescription > const & xType );
    ::System::Type ^ get_type(
        css::uno::Reference<
        css::reflection::XEnumTypeDescription > const & xType );
    /* returns the type for a struct or exception. In case of a polymorphic struct it may
        return a ::uno::PolymorphicType (cli_basetypes.dll) only if the struct is already
        complete.
    */
    ::System::Type ^ get_type(
        css::uno::Reference<
        css::reflection::XCompoundTypeDescription > const & xType );
    ::System::Type ^ get_type(
        css::uno::Reference<
        css::reflection::XInterfaceTypeDescription2 > const & xType );
    ::System::Type ^ get_type(
        css::uno::Reference<
        css::reflection::XSingletonTypeDescription2 > const & xType );

    /*
      May return NULL if the service description is an obsolete. See
      description of
      com.sun.star.reflection.XServiceTypeDescription2.isSingleInterfaceBased
     */
    ::System::Type ^ get_type(
        css::uno::Reference<
        css::reflection::XServiceTypeDescription2 > const & xType );
public:
    TypeEmitter(
        ::System::Reflection::Emit::ModuleBuilder ^ module_builder,
        cli::array< ::System::Reflection::Assembly ^ > ^ assemblies );
    // Dispose() is reserved in a ref class -- the compiler generates it from
    // the destructor, which is what "delete emitter" then calls.  The body
    // that used to be Dispose() lives in the destructor now.
    ~TypeEmitter();

    ::System::Reflection::Assembly ^ type_resolve(
        ::System::Object ^ sender, ::System::ResolveEventArgs ^ args );

    ::System::Type ^ get_type(
        css::uno::Reference<
        css::reflection::XTypeDescription > const & xType );
};

}
