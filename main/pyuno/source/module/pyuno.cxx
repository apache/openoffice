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



#include "pyuno_impl.hxx"

#include <rtl/strbuf.hxx>
#include <rtl/ustrbuf.hxx>

#include <osl/thread.h>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XMaterialHolder.hpp>

#define TO_ASCII(x) OUStringToOString( x , RTL_TEXTENCODING_ASCII_US).getStr()

using rtl::OStringBuffer;
using rtl::OUStringBuffer;
using rtl::OUStringToOString;
using rtl::OUString;
using com::sun::star::uno::Sequence;
using com::sun::star::uno::Reference;
using com::sun::star::uno::XInterface;
using com::sun::star::uno::Any;
using com::sun::star::uno::makeAny;
using com::sun::star::uno::UNO_QUERY;
using com::sun::star::uno::Type;
using com::sun::star::uno::TypeClass;
using com::sun::star::uno::RuntimeException;
using com::sun::star::uno::Exception;
using com::sun::star::uno::XComponentContext;
using com::sun::star::lang::XSingleServiceFactory;
using com::sun::star::lang::XServiceInfo;
using com::sun::star::lang::XTypeProvider;
using com::sun::star::script::XTypeConverter;
using com::sun::star::script::XInvocation2;
using com::sun::star::beans::XMaterialHolder;

namespace pyuno
{

PyObject *PyUNO_str( PyObject * self );

void PyUNO_del (PyObject* self)
{
    PyUNO* me = reinterpret_cast< PyUNO* > (self);
    {
        PyThreadDetach antiguard;
        delete me->members;
    }
    PyObject_Del (self);
}



OUString val2str( const void * pVal, typelib_TypeDescriptionReference * pTypeRef , sal_Int32 mode ) SAL_THROW( () )
{
	OSL_ASSERT( pVal );
	if (pTypeRef->eTypeClass == typelib_TypeClass_VOID)
		return OUString( RTL_CONSTASCII_USTRINGPARAM("void") );
	
	OUStringBuffer buf( 64 );
	buf.append( (sal_Unicode)'(' );
	buf.append( pTypeRef->pTypeName );
	buf.append( (sal_Unicode)')' );
	
	switch (pTypeRef->eTypeClass)
	{
	case typelib_TypeClass_INTERFACE:
    {
		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("0x") );
		buf.append( reinterpret_cast< sal_IntPtr >(*(void **)pVal), 16 );
        if( VAL2STR_MODE_DEEP == mode )
        {
            buf.appendAscii( "{" );        Reference< XInterface > r = *( Reference< XInterface > * ) pVal;
            Reference< XServiceInfo > serviceInfo( r, UNO_QUERY);
            Reference< XTypeProvider > typeProvider(r,UNO_QUERY);
            if( serviceInfo.is() )
            {
                buf.appendAscii("implementationName=" );
                buf.append(serviceInfo->getImplementationName() );
                buf.appendAscii(", supportedServices={" );
                Sequence< OUString > seq = serviceInfo->getSupportedServiceNames();
                for( int i = 0 ; i < seq.getLength() ; i ++ )
                {
                    buf.append( seq[i] );
                    if( i +1 != seq.getLength() )
                        buf.appendAscii( "," );
                }
                buf.appendAscii("}");
            }

            if( typeProvider.is() )
            {
                buf.appendAscii(", supportedInterfaces={" );
                Sequence< Type > seq (typeProvider->getTypes());
                for( int i = 0 ; i < seq.getLength() ; i ++ )
                {
                    buf.append(seq[i].getTypeName());
                    if( i +1 != seq.getLength() )
                        buf.appendAscii( "," );
                }
                buf.appendAscii("}");
            }
            buf.appendAscii( "}" );
        }

		break;
    }
	case typelib_TypeClass_UNION:
	{
//  		typelib_TypeDescription * pTypeDescr = 0;
//  		TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
//  		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("{ ") );
//  		buf.append( val2str( (char *)pVal + ((typelib_UnionTypeDescription *)pTypeDescr)->nValueOffset,
//  							 union_getSetType( pVal, pTypeDescr ) ) );
//  		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(" }") );
//  		TYPELIB_DANGER_RELEASE( pTypeDescr );
		break;
	}
	case typelib_TypeClass_STRUCT:
	case typelib_TypeClass_EXCEPTION:
	{
		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("{ ") );
		typelib_TypeDescription * pTypeDescr = 0;
		TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
		OSL_ASSERT( pTypeDescr );
		
		typelib_CompoundTypeDescription * pCompType = (typelib_CompoundTypeDescription *)pTypeDescr;
		sal_Int32 nDescr = pCompType->nMembers;
		
		if (pCompType->pBaseTypeDescription)
		{
			buf.append( val2str( pVal, ((typelib_TypeDescription *)pCompType->pBaseTypeDescription)->pWeakRef,mode ) );
			if (nDescr)
				buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(", ") );
		}
		
		typelib_TypeDescriptionReference ** ppTypeRefs = pCompType->ppTypeRefs;
		sal_Int32 * pMemberOffsets = pCompType->pMemberOffsets;
		rtl_uString ** ppMemberNames = pCompType->ppMemberNames;
		
		for ( sal_Int32 nPos = 0; nPos < nDescr; ++nPos )
		{
			buf.append( ppMemberNames[nPos] );
			buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(" = ") );
			typelib_TypeDescription * pMemberType = 0;
			TYPELIB_DANGER_GET( &pMemberType, ppTypeRefs[nPos] );
			buf.append( val2str( (char *)pVal + pMemberOffsets[nPos], pMemberType->pWeakRef, mode ) );
			TYPELIB_DANGER_RELEASE( pMemberType );
			if (nPos < (nDescr -1))
				buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(", ") );
		}
		
		TYPELIB_DANGER_RELEASE( pTypeDescr );
		
		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(" }") );
		break;
	}
	case typelib_TypeClass_SEQUENCE:
	{
		typelib_TypeDescription * pTypeDescr = 0;
		TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
		
		uno_Sequence * pSequence = *(uno_Sequence **)pVal;
		typelib_TypeDescription * pElementTypeDescr = 0;
		TYPELIB_DANGER_GET( &pElementTypeDescr, ((typelib_IndirectTypeDescription *)pTypeDescr)->pType );
		
		sal_Int32 nElementSize = pElementTypeDescr->nSize;
		sal_Int32 nElements	   = pSequence->nElements;
		
		if (nElements)
		{
			buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("{ ") );
			char * pElements = pSequence->elements;
			for ( sal_Int32 nPos = 0; nPos < nElements; ++nPos )
			{
				buf.append( val2str( pElements + (nElementSize * nPos), pElementTypeDescr->pWeakRef, mode ) );
				if (nPos < (nElements -1))
					buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(", ") );
			}
			buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(" }") );
		}
		else
		{
			buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("{}") );
		}
		TYPELIB_DANGER_RELEASE( pElementTypeDescr );
		TYPELIB_DANGER_RELEASE( pTypeDescr );
		break;
	}
	case typelib_TypeClass_ANY:
		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("{ ") );
		buf.append( val2str( ((uno_Any *)pVal)->pData,
							 ((uno_Any *)pVal)->pType ,
                             mode) );
		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(" }") );
		break;
	case typelib_TypeClass_TYPE:
		buf.append( (*(typelib_TypeDescriptionReference **)pVal)->pTypeName );
		break;
	case typelib_TypeClass_STRING:
		buf.append( (sal_Unicode)'\"' );
		buf.append( *(rtl_uString **)pVal );
		buf.append( (sal_Unicode)'\"' );
		break;
	case typelib_TypeClass_ENUM:
	{
		typelib_TypeDescription * pTypeDescr = 0;
		TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
		
		sal_Int32 * pValues = ((typelib_EnumTypeDescription *)pTypeDescr)->pEnumValues;
		sal_Int32 nPos = ((typelib_EnumTypeDescription *)pTypeDescr)->nEnumValues;
		while (nPos--)
		{
			if (pValues[nPos] == *(int *)pVal)
				break;
		}
		if (nPos >= 0)
			buf.append( ((typelib_EnumTypeDescription *)pTypeDescr)->ppEnumNames[nPos] );
		else
			buf.append( (sal_Unicode)'?' );
		
		TYPELIB_DANGER_RELEASE( pTypeDescr );
		break;
	}
	case typelib_TypeClass_BOOLEAN:
		if (*(sal_Bool *)pVal)
			buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("true") );
		else
			buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("false") );
		break;
	case typelib_TypeClass_CHAR:
		buf.append( (sal_Unicode)'\'' );
		buf.append( *(sal_Unicode *)pVal );
		buf.append( (sal_Unicode)'\'' );
		break;
	case typelib_TypeClass_FLOAT:
		buf.append( *(float *)pVal );
		break;
	case typelib_TypeClass_DOUBLE:
		buf.append( *(double *)pVal );
		break;
	case typelib_TypeClass_BYTE:
		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("0x") );
		buf.append( (sal_Int32)*(sal_Int8 *)pVal, 16 );
		break;
	case typelib_TypeClass_SHORT:
		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("0x") );
		buf.append( (sal_Int32)*(sal_Int16 *)pVal, 16 );
		break;
	case typelib_TypeClass_UNSIGNED_SHORT:
		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("0x") );
		buf.append( (sal_Int32)*(sal_uInt16 *)pVal, 16 );
		break;
	case typelib_TypeClass_LONG:
		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("0x") );
		buf.append( *(sal_Int32 *)pVal, 16 );
		break;
	case typelib_TypeClass_UNSIGNED_LONG:
		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("0x") );
		buf.append( (sal_Int64)*(sal_uInt32 *)pVal, 16 );
		break;
	case typelib_TypeClass_HYPER:
	case typelib_TypeClass_UNSIGNED_HYPER:
		buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("0x") );
#if defined(GCC) && defined(SPARC)
		{
			sal_Int64 aVal;
			*(sal_Int32 *)&aVal = *(sal_Int32 *)pVal;
			*((sal_Int32 *)&aVal +1)= *((sal_Int32 *)pVal +1);
			buf.append( aVal, 16 );
		}
#else
		buf.append( *(sal_Int64 *)pVal, 16 );
#endif
		break;
		
	case typelib_TypeClass_VOID:
	case typelib_TypeClass_ARRAY:
	case typelib_TypeClass_UNKNOWN:
	case typelib_TypeClass_SERVICE:
	case typelib_TypeClass_MODULE:
	default:
		buf.append( (sal_Unicode)'?' );
	}
	
	return buf.makeStringAndClear();
}


PyObject *PyUNO_repr( PyObject  * self )
{
    PyUNO *me = (PyUNO * ) self;
    PyObject * ret = 0;
    
    if( me->members->wrappedObject.getValueType().getTypeClass()
        == com::sun::star::uno::TypeClass_EXCEPTION )
    {
        Reference< XMaterialHolder > rHolder(me->members->xInvocation,UNO_QUERY);
        if( rHolder.is() )
        {
            Any a = rHolder->getMaterial();
            Exception e;
            a >>= e;
            ret = ustring2PyUnicode(e.Message ).getAcquired();
        }
    }
    else
    {
        ret = PyUNO_str( self );
    }
    return ret;
}

PyObject *PyUNO_invoke( PyObject *object, const char *name , PyObject *args )
{
    PyRef ret;
    try
    {
        Runtime runtime;

        PyRef paras,callable;
        if( PyObject_IsInstance( object, getPyUnoClass().get() ) )
        {
            PyUNO* me = (PyUNO*) object;
            OUString attrName = OUString::createFromAscii(name);
            if (! me->members->xInvocation->hasMethod (attrName))
            {
                OUStringBuffer buf;
                buf.appendAscii( "Attribute " );
                buf.append( attrName );
                buf.appendAscii( " unknown" );
                throw RuntimeException( buf.makeStringAndClear(), Reference< XInterface > () );
            }
            callable = PyUNO_callable_new (
                me->members->xInvocation,
                attrName,
                ACCEPT_UNO_ANY);
            paras = args;
        }
        else
        {
            // clean the tuple from uno.Any !
            int size = PyTuple_Size( args );
            { // for CC, keeping ref-count of tuple being 1
            paras = PyRef(PyTuple_New( size ), SAL_NO_ACQUIRE);
            }
            for( int i = 0 ; i < size ;i ++ )
            {
                PyObject * element = PyTuple_GetItem( args , i );
                if( PyObject_IsInstance( element , getAnyClass( runtime ).get() ) )
                {
                    element = PyObject_GetAttrString(
                        element, const_cast< char * >("value") );
                }
                else
                {
                    Py_XINCREF( element );
                }
                PyTuple_SetItem( paras.get(), i , element );
            }
            callable = PyRef( PyObject_GetAttrString( object , (char*)name ), SAL_NO_ACQUIRE );
            if( !callable.is() )
                return 0;
        }
        ret = PyRef( PyObject_CallObject( callable.get(), paras.get() ), SAL_NO_ACQUIRE );
    }
    catch (::com::sun::star::lang::IllegalArgumentException &e)
    {
        raisePyExceptionWithAny( com::sun::star::uno::makeAny( e ) );
    }
    catch (::com::sun::star::script::CannotConvertException &e)
    {
        raisePyExceptionWithAny( com::sun::star::uno::makeAny( e ) );
    }
    catch (::com::sun::star::uno::RuntimeException &e)
    {
        raisePyExceptionWithAny( com::sun::star::uno::makeAny( e ) );
    }
    catch (::com::sun::star::uno::Exception &e)
    {
        raisePyExceptionWithAny( com::sun::star::uno::makeAny( e ) );
    }
 
    return ret.getAcquired();
}

PyObject *PyUNO_str( PyObject * self )
{
    PyUNO *me = ( PyUNO * ) self;

    OStringBuffer buf;

    
    if( me->members->wrappedObject.getValueType().getTypeClass()
        == com::sun::star::uno::TypeClass_STRUCT ||
        me->members->wrappedObject.getValueType().getTypeClass()
        == com::sun::star::uno::TypeClass_EXCEPTION)
    {
        Reference< XMaterialHolder > rHolder(me->members->xInvocation,UNO_QUERY);
        if( rHolder.is() )
        {
            PyThreadDetach antiguard;
            Any a = rHolder->getMaterial();
            OUString s = val2str( (void*) a.getValue(), a.getValueType().getTypeLibType() );
            buf.append( OUStringToOString(s,RTL_TEXTENCODING_ASCII_US) );
        }
    }
    else
    {
        // a common UNO object
        PyThreadDetach antiguard;
        buf.append( "pyuno object " );
        
        OUString s = val2str( (void*)me->members->wrappedObject.getValue(),
                              me->members->wrappedObject.getValueType().getTypeLibType() );
        buf.append( OUStringToOString(s,RTL_TEXTENCODING_ASCII_US) );
    }

    return PYSTR_FROMSTR( buf.getStr() );
}

#if PY_MAJOR_VERSION >= 3
PyObject* PyUNO_getattr (PyObject* self, PyObject* attr_name)
#else
PyObject* PyUNO_getattr (PyObject* self, char* name)
#endif
{
    PyUNO* me;

#if PY_VERSION_HEX >= 0x03030000 && PY_VERSION_HEX < 0x03060000
    char *name = PyUnicode_AsUTF8(attr_name);
#elif PY_MAJOR_VERSION >= 3
    PyRef pUtf8(PyUnicode_AsUTF8String(attr_name), SAL_NO_ACQUIRE);
    char *name = PyBytes_AsString(pUtf8.get());
#endif
    try
    {

        Runtime runtime;
    
        me = (PyUNO*) self;
#if PY_MAJOR_VERSION < 3
        //Handle Python dir () stuff first...
        if (strcmp (name, "__members__") == 0)
        {
            PyObject* member_list;
            Sequence<OUString> oo_member_list;

            oo_member_list = me->members->xInvocation->getMemberNames ();
            member_list = PyList_New (oo_member_list.getLength ());
            for (int i = 0; i < oo_member_list.getLength (); i++)
            {
                // setitem steals a reference
                PyList_SetItem (member_list, i, ustring2PyString(oo_member_list[i]).getAcquired() );
            }
            return member_list;
        }
#endif

        if (strcmp (name, "__dict__") == 0)
        {
            Py_INCREF (Py_None);
            return Py_None;
        }
#if PY_MAJOR_VERSION < 3
        if (strcmp (name, "__methods__") == 0)
        {
            Py_INCREF (Py_None);
            return Py_None;
        }
#endif
        if (strcmp (name, "__class__") == 0)
        {
            if( me->members->wrappedObject.getValueTypeClass() ==
                com::sun::star::uno::TypeClass_STRUCT ||
                me->members->wrappedObject.getValueTypeClass() ==
                com::sun::star::uno::TypeClass_EXCEPTION )
            {
                return getClass(
                    me->members->wrappedObject.getValueType().getTypeName(), runtime ).getAcquired();
            }
            Py_INCREF (Py_None);
            return Py_None;
        }

        OUString attrName( OUString::createFromAscii( name ) );
        //We need to find out if it's a method...
        if (me->members->xInvocation->hasMethod (attrName))
        {
            //Create a callable object to invoke this...
            PyRef ret = PyUNO_callable_new (
                me->members->xInvocation,
                attrName);
            Py_XINCREF( ret.get() );
            return ret.get();
      
        }

        //or a property
        if (me->members->xInvocation->hasProperty ( attrName))
        {
            //Return the value of the property
            Any anyRet;
            {
                PyThreadDetach antiguard;
                anyRet = me->members->xInvocation->getValue (attrName);
            }
            PyRef ret = runtime.any2PyObject(anyRet);
            Py_XINCREF( ret.get() );
            return ret.get();
        }

        //or else...
        PyErr_SetString (PyExc_AttributeError, name);
    }
    catch( com::sun::star::reflection::InvocationTargetException & e )
    {
        raisePyExceptionWithAny( makeAny(e.TargetException) );
    }
    catch( com::sun::star::beans::UnknownPropertyException & e )
    {
        raisePyExceptionWithAny( makeAny(e) );
    }
    catch( com::sun::star::lang::IllegalArgumentException &e )
    {
        raisePyExceptionWithAny( makeAny(e) );
    }
    catch( com::sun::star::script::CannotConvertException &e )
    {
        raisePyExceptionWithAny( makeAny(e) );
    }
    catch( RuntimeException &e )
    {
        raisePyExceptionWithAny( makeAny(e) );
    }

    return NULL;
}

#if PY_MAJOR_VERSION >= 3
int PyUNO_setattr (PyObject* self, PyObject* attr_name, PyObject* value)
#else
int PyUNO_setattr (PyObject* self, char* name, PyObject* value)
#endif
{
    PyUNO* me;

#if PY_VERSION_HEX >= 0x03030000 && PY_VERSION_HEX < 0x03060000
    char *name = PyUnicode_AsUTF8(attr_name);
#elif PY_MAJOR_VERSION >= 3
    PyRef pUtf8(PyUnicode_AsUTF8String(attr_name), SAL_NO_ACQUIRE);
    char *name = PyBytes_AsString(pUtf8.get());
#endif
    me = (PyUNO*) self;
    try
    {
        Runtime runtime;
        Any val= runtime.pyObject2Any(value, ACCEPT_UNO_ANY);

        OUString attrName( OUString::createFromAscii( name ) );
        {
            PyThreadDetach antiguard;
            if (me->members->xInvocation->hasProperty (attrName))
            {
                me->members->xInvocation->setValue (attrName, val);
                return 0; //Keep with Python's boolean system
            }
        }
    }
    catch( com::sun::star::reflection::InvocationTargetException & e )
    {
        raisePyExceptionWithAny( makeAny(e.TargetException) );
        return 1;
    }
    catch( com::sun::star::beans::UnknownPropertyException & e )
    {
        raisePyExceptionWithAny( makeAny(e) );
        return 1;
    }
    catch( com::sun::star::script::CannotConvertException &e )
    {
        raisePyExceptionWithAny( makeAny(e) );
        return 1;
    }
    catch( RuntimeException & e )
    {
        raisePyExceptionWithAny( makeAny( e ) );
        return 1;
    }
    PyErr_SetString (PyExc_AttributeError, name);
    return 1; //as above.
}

#if PY_MAJOR_VERSION >= 3
static PyObject *PyUNO_dir( PyObject *self, PyObject *that )
{
    PyUNO* me;
    PyObject* member_list;
    Sequence<OUString> oo_member_list;
    
    me = (PyUNO*) self;
    oo_member_list = me->members->xInvocation->getMemberNames ();
    member_list = PyList_New (oo_member_list.getLength ());
    for (int i = 0; i < oo_member_list.getLength (); i++)
    {
        // setitem steals a reference
        PyList_SetItem (member_list, i, ustring2PyUnicode(oo_member_list[i]).getAcquired() );
    }
    return member_list;
}

static PyObject *PyUNO_richcompare( PyObject *self, PyObject *that, int op )
{
    switch (op)
    {
    case Py_EQ:
    case Py_NE:
        if( self == that )
        {
            if (op == Py_EQ)
                Py_RETURN_TRUE;
            else
                Py_RETURN_FALSE;
        }
        try
        {
            Runtime runtime;
            if( PyObject_IsInstance( that, getPyUnoClass().get() ) )
            {
                PyUNO *me = reinterpret_cast< PyUNO*> ( self );
                PyUNO *other = reinterpret_cast< PyUNO *> (that );
                com::sun::star::uno::TypeClass tcMe = me->members->wrappedObject.getValueTypeClass();
                com::sun::star::uno::TypeClass tcOther = other->members->wrappedObject.getValueTypeClass();
            
                if( tcMe == tcOther )
                {
                    if( tcMe == com::sun::star::uno::TypeClass_STRUCT ||
                        tcMe == com::sun::star::uno::TypeClass_EXCEPTION )
                    {
                        Reference< XMaterialHolder > xMe( me->members->xInvocation,UNO_QUERY);
                        Reference< XMaterialHolder > xOther( other->members->xInvocation,UNO_QUERY );
                        if( xMe->getMaterial() == xOther->getMaterial() )
                        {
                            if (op == Py_EQ)
                                Py_RETURN_TRUE;
                            else
                                Py_RETURN_FALSE;
                        }
                    }
                    else if( tcMe == com::sun::star::uno::TypeClass_INTERFACE )
                    {
                        if( me->members->wrappedObject == other->members->wrappedObject )
                        {
                            if (op == Py_EQ)
                                Py_RETURN_TRUE;
                            else
                                Py_RETURN_FALSE;
                        }
                    }
                }
            }
            if (op == Py_EQ)
                Py_RETURN_FALSE;
            else
                Py_RETURN_TRUE;
        }
        catch( com::sun::star::uno::RuntimeException & e)
        {
            raisePyExceptionWithAny( makeAny( e ) );
        }
        break;
    default:
        PyErr_SetString(Py_NotImplemented, "not implemented");
        break;
    }
    
    return NULL;
}


static struct PyMethodDef PyUNO_methods[] = {
    { "__dir__", (PyCFunction)PyUNO_dir, METH_VARARGS, NULL},
    { NULL, NULL }
};

#else
// ensure object identity and struct equality
static int PyUNO_cmp( PyObject *self, PyObject *that )
{
    if( self == that )
        return 0;
    int retDefault = self > that ? 1 : -1;
    try
    {
        Runtime runtime;
        if( PyObject_IsInstance( that, getPyUnoClass().get() ) )
        {

            PyUNO *me = reinterpret_cast< PyUNO*> ( self );
            PyUNO *other = reinterpret_cast< PyUNO *> (that );
            com::sun::star::uno::TypeClass tcMe = me->members->wrappedObject.getValueTypeClass();
            com::sun::star::uno::TypeClass tcOther = other->members->wrappedObject.getValueTypeClass();
        
            if( tcMe == tcOther )
            {
                if( tcMe == com::sun::star::uno::TypeClass_STRUCT ||
                    tcMe == com::sun::star::uno::TypeClass_EXCEPTION )
                {
                    Reference< XMaterialHolder > xMe( me->members->xInvocation,UNO_QUERY);
                    Reference< XMaterialHolder > xOther( other->members->xInvocation,UNO_QUERY );
                    if( xMe->getMaterial() == xOther->getMaterial() )
                        return 0;
                }
                else if( tcMe == com::sun::star::uno::TypeClass_INTERFACE )
                {
                    if( me->members->wrappedObject == other->members->wrappedObject )
//                     if( me->members->xInvocation == other->members->xInvocation )
                        return 0;
                }
            }
        }
    }
    catch( com::sun::star::uno::RuntimeException & e)
    {
        raisePyExceptionWithAny( makeAny( e ) );
    }
    return retDefault;
}
#endif

static PyTypeObject PyUNOType =
{
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    const_cast< char * >("pyuno"),
    sizeof (PyUNO),
    0,
    (destructor) PyUNO_del,
    (printfunc) 0,
#if PY_MAJOR_VERSION >= 3
    (getattrfunc) 0,
    (setattrfunc) 0,
    0, 
#else
    (getattrfunc) PyUNO_getattr, /* tp_getattr */
    (setattrfunc) PyUNO_setattr, /* tp_setattr */
    (cmpfunc) PyUNO_cmp,
#endif
    (reprfunc) PyUNO_repr,
    0,
    0,
    0,
    (hashfunc) 0,
    (ternaryfunc) 0,
    (reprfunc) PyUNO_str,
#if PY_MAJOR_VERSION >= 3
    (getattrofunc)PyUNO_getattr, /* tp_getattro */
    (setattrofunc)PyUNO_setattr, /* tp_setattro */
#else
    (getattrofunc)0,
    (setattrofunc)0,
#endif
    NULL,
    0,
    NULL,
    (traverseproc)0,
    (inquiry)0,
#if PY_MAJOR_VERSION >= 3
    PyUNO_richcompare, /* tp_richcompare */
#else
    (richcmpfunc)0,
#endif
    0,
    (getiterfunc)0,
    (iternextfunc)0,
#if PY_MAJOR_VERSION >= 3
    PyUNO_methods, /* tp_methods */
#else
    NULL,
#endif
    NULL,
    NULL,
    NULL,
    NULL,
    (descrgetfunc)0,
    (descrsetfunc)0,
    0,
    (initproc)0,
    (allocfunc)0,
    (newfunc)0,
    (freefunc)0,
    (inquiry)0,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    (destructor)0
#if PY_VERSION_HEX >= 0x02060000 && PY_VERSION_HEX < 0x03060000
    , 0
#endif
};

PyRef getPyUnoClass()
{
    return PyRef( reinterpret_cast< PyObject * > ( &PyUNOType ) );
}

PyObject* PyUNO_new (
    const Any & targetInterface, const Reference<XSingleServiceFactory> &ssf)
{
    Reference<XInterface> tmp_interface;
  
    targetInterface >>= tmp_interface;
    if (!tmp_interface.is ())
    {
        // empty reference !
        Py_INCREF( Py_None );
        return Py_None;
    }

    return PyUNO_new_UNCHECKED (targetInterface, ssf);
}


PyObject* PyUNO_new_UNCHECKED (
    const Any &targetInterface,
    const Reference<XSingleServiceFactory> &ssf )
{
    PyUNO* self;
    Sequence<Any> arguments (1);
    Reference<XInterface> tmp_interface;

    self = PyObject_New (PyUNO, &PyUNOType);
    if (self == NULL)
        return NULL; //NULL == error
    self->members = new PyUNOInternals();

    arguments[0] <<= targetInterface;
    {
        PyThreadDetach antiguard;
        tmp_interface = ssf->createInstanceWithArguments (arguments);
        Reference<XInvocation2> tmp_invocation (tmp_interface, UNO_QUERY);
        self->members->xInvocation = tmp_invocation;
        self->members->wrappedObject = targetInterface;
    }
    return (PyObject*) self;
}

}
