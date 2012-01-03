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



#ifndef _BF_SVTOOLS_ID_HELPER_HXX_
#define _BF_SVTOOLS_ID_HELPER_HXX_

#include <unotools/idhelper.hxx>

#define DECLARE_IMPLEMENTATIONID_HELPER_VSMALLSTART(_namespace, classname)		\
namespace _namespace {												\
class classname														\
{																	\
	friend class classname##Ref;								\
																	\
	static sal_Int32	s_nReferenced;								\
	static void*		s_pMap;										\
																	\
	static ::osl::Mutex	s_aMutex;									\
																	\
public:																\
	static void acquire();											\
	static void release();											\
																	\
																\

#define DECLARE_IMPLEMENTATIONID_HELPER_VSMALLEND(_namespace, classname)              \
	classname() { }													\
};																	\
																	\
/*=======================================================================*/	\
class classname##Ref												\
{																	\
public:																\
	classname##Ref() { classname::acquire(); }	\
	~classname##Ref() { classname::release(); }	\
};																	\
																	\
}	/* _namespace */												\
																	\

#define DECLARE_IMPLEMENTATIONID_HELPER_VSMALL(_namespace, classname)            \
DECLARE_IMPLEMENTATIONID_HELPER_VSMALLSTART(_namespace, classname) \
DECLARE_IMPLEMENTATIONID_HELPER_VSMALLEND(_namespace, classname)


#define DECLARE_IMPLEMENTATIONID_HELPER_SMALL(_namespace, classname)		\
DECLARE_IMPLEMENTATIONID_HELPER_VSMALLSTART(_namespace, classname) \
	static ::com::sun::star::uno::Sequence< sal_Int8 > getImplementationId(	\
		const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >& _rTypes);	\
private:															\
	static void implCreateMap();									\
DECLARE_IMPLEMENTATIONID_HELPER_VSMALLEND(_namespace, classname)


/** implement an id helper
*/
#define IMPLEMENT_IMPLEMENTATIONID_HELPER_VSMALL(_namespace, classname)		\
namespace _namespace {	\
	\
/*=======================================================================*/	\
	\
sal_Int32		classname::s_nReferenced(0);	\
void*			classname::s_pMap = NULL;	\
::osl::Mutex	classname::s_aMutex;	\
	\
/*-----------------------------------------------------------------------*/	\
void classname::acquire()	\
{	\
	::osl::MutexGuard aGuard(s_aMutex);	\
	++s_nReferenced;	\
}	\
	\
/*-----------------------------------------------------------------------*/	\
void classname::release()	\
{	\
	::osl::MutexGuard aGuard(s_aMutex);	\
	if (!--s_nReferenced)	\
	{	\
		delete static_cast< ::utl::MapType2Id *>( s_pMap );	\
		s_pMap = NULL;	\
	}	\
}	\
}	/* _namespace */

#define IMPLEMENT_IMPLEMENTATIONID_HELPER_SMALL(_namespace, classname)		\
IMPLEMENT_IMPLEMENTATIONID_HELPER_VSMALL(_namespace, classname)		\
namespace _namespace {	\
/*-----------------------------------------------------------------------*/	\
void classname::implCreateMap()	\
{	\
	if (s_pMap)	\
		return;	\
	s_pMap = new ::utl::MapType2Id();	\
}	\
	\
	\
::com::sun::star::uno::Sequence< sal_Int8 > classname::getImplementationId(	\
		const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >& _rTypes)	\
{	\
	::osl::MutexGuard aGuard(s_aMutex);	\
	DBG_ASSERT(s_nReferenced,	\
		"classname::getImplementationId : you need to hold a reference to this class in order to use it !");	\
		/* give the calling class a member of type classname##Ref and all is fine .... */	\
	\
	implCreateMap();	\
	\
	::utl::MapType2Id* pMap = static_cast< ::utl::MapType2Id *>(s_pMap);	\
	\
	::cppu::OImplementationId& rId = (*pMap)[_rTypes];	\
	/* this will create an entry for the given type sequence, if neccessary */	\
	\
	return rId.getImplementationId();	\
}	\
}	

#endif // _BF_SVTOOLS_ID_HELPER_HXX_

