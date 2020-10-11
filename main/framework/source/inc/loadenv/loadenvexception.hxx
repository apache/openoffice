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



#ifndef __FRAMEWORK_LOADENV_LOADENVEXCEPTION_HXX_
#define __FRAMEWORK_LOADENV_LOADENVEXCEPTION_HXX_

//_______________________________________________
// includes of own project

//_______________________________________________
// includes of uno interface

#include <com/sun/star/uno/Any.h>
#include <com/sun/star/uno/Exception.hpp>

//_______________________________________________
// includes of an other project
#include <rtl/string.hxx>

//_______________________________________________
// namespace

namespace framework{

#ifndef css // conflict with define :-(
namespace css = ::com::sun::star;
#endif

//_______________________________________________
// definitions

/** @short  specify an exception, which can be used inside the
            load environment only.

    @descr  Of course outside code must wrapp it, to transport
            the occurred information to its caller.

    @author as96863
 */
class LoadEnvException
{
    //___________________________________________
    // const

    public:

        /** @short  Can be used as an ID for an instance of a LoadEnvException.
            @descr  To prevent errors on adding/removing/changing such IDs here,
                    an enum field is used. Its int values are self organized ...
         */
        enum EIDs
        {
            /** @short  The specified URL/Stream/etcpp. can not be handled by a LoadEnv instance. */
            ID_UNSUPPORTED_CONTENT,

            /** @short  It was not possible to get access to global filter configuration.
                @descr  Might som neccsessary services could not be created. */
            ID_NO_CONFIG_ACCESS,

            /** @short  Some data obtained from the filter configuration seems to incorrect.
                @descr  Might a filter-type relation ship seem to be damaged. */
            ID_INVALID_FILTER_CONFIG,

            /** @short  indicates a corrupted media descriptor.
                @descr  Some parts are required - some other ones are optional. Such exception
                        should be thrown, if a required item does not exists. */
            ID_INVALID_MEDIADESCRIPTOR,

            /** @short  Its similar to an uno::RuntimeException ....
                @descr  But such runtime exception can break the whole office code.
                        So its capsulated to this specialized load environment only.
                        Mostly it indicates a missing but needed resource ... e.g the
                        global desktop reference! */
            ID_INVALID_ENVIRONMENT,

            /** @short  indicates a failed search for the right target frame. */
            ID_NO_TARGET_FOUND,

            /** @short  An already existing document was found inside a target frame.
                        But its controller could not be suspended successfully. That's
                        why the new load request was cancelled. The document could not
                        be replaced. */
            ID_COULD_NOT_SUSPEND_CONTROLLER,

            /** @short  TODO */
            ID_COULD_NOT_REACTIVATE_CONTROLLER,

            /** @short  inidcates an already running load operation. Of yourse the same
                        instance can't be used for multiple load requests at the same time.
             */
            ID_STILL_RUNNING,

            /** @short  sometiems we can't specify the reason for an error, because we
                        was interrupted by an called code in an unexpected way ...
             */
            ID_GENERAL_ERROR
        };

    //___________________________________________
    // member

    public:

        /** @short  contains a suitable message, which describes the reason for this
                    exception. */
        ::rtl::OString m_sMessage;

        /** @short  An ID, which make this exception unique among others. */
        sal_Int32 m_nID;

        /** @short  Contains the original exception, if any occurred. */
        css::uno::Any m_exOriginal;

        /** TODO
             Experimental use! May it can be usefully to know, if an exception was already
             catched and handled by an interaction and was might be rethrowed! */
        sal_Bool m_bHandled;

    //___________________________________________
    // interface

    public:

        /** @short  initialize a new instance with an ID.
            @descr  Some other items of this exception
                    (e.g. a suitable message) will be generated
                    automatically.

            @param  nID
                    One of the defined const IDs of this class.
         */
        LoadEnvException(sal_Int32 nID)
        {
            m_nID = nID;
        }

        //_______________________________________

        /** @short  initialize a new instance with an ID
                    an wrap a detected exception into this one.
            @descr  Some other items of this exception
                    (e.g. a suitable message) will be generated
                    automatically.

            @param  nID
                    One of the defined const IDs of this class.

            @param  exUno
                    the original catched uno exception.
         */
        LoadEnvException(      sal_Int32        nID  ,
                         const css::uno::Any&   exUno)
        {
            m_nID        = nID  ;
            m_exOriginal = exUno;
        }

        //_______________________________________

        /** @short  destruct an instance of this exception.
         */
        ~LoadEnvException()
        {
            m_sMessage = ::rtl::OString();
            m_nID      = 0;
            m_bHandled = false;
            m_exOriginal.clear();
        }
};

} // namespace framework

#endif // __FRAMEWORK_LOADENV_LOADENVEXCEPTION_HXX_
