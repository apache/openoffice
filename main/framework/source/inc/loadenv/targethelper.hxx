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



#ifndef __FRAMEWORK_TARGETHELPER_HXX_
#define __FRAMEWORK_TARGETHELPER_HXX_

//_______________________________________________
// own includes

#include <sal/types.h>
#include <rtl/ustring.hxx>
#include <targets.h>
//_______________________________________________
// namespace

namespace framework{

//_______________________________________________
// definitions

/** @short  can be used to detect, if a target name (used e.g. for XFrame.findFrame())
            has a special meaning or can be used as normal frame name (e.g. for XFrame.setName()).

    @author as96863
 */
class TargetHelper
{
    //___________________________________________
    // const

    public:

        /** @short  its used at the following interfaces to classify
                    target names.
         */
        enum ESpecialTarget
        {
            E_NOT_SPECIAL   ,
            E_SELF          ,
            E_PARENT        ,
            E_TOP           ,
            E_BLANK         ,
            E_DEFAULT       ,
            E_BEAMER        ,
            E_MENUBAR       ,
            E_HELPAGENT     ,
            E_HELPTASK
        };

    //___________________________________________
    // interface

    public:

        //___________________________________________

        /** @short  it checks the given unknown target name,
                    if it's the expected special one.

            @note   An empty target is similar to "_self"!

            @param  sCheckTarget
                    must be the unknown target name, which should be checked.

            @param  eSpecialTarget
                    represent the expected target.

            @return It returns <TRUE/> if <var>sCheckTarget</var> represent
                    the expected <var>eSpecialTarget</var> value; <FALSE/> otherwise.
         */
        static sal_Bool matchSpecialTarget(const ::rtl::OUString& sCheckTarget  ,
                                                 ESpecialTarget   eSpecialTarget);

        //___________________________________________

        /** @short  it checks, if the given name can be used
                    to set it at a frame using XFrame.setName() method.

            @descr  Because we handle special targets in a hard coded way
                    (means we do not check the real name of a frame then)
                    such named frames will never be found!

                    And in case such special names can exists one times only
                    by definition inside the same frame tree (e.g. _beamer and
                    OFFICE_HELP_TASK) its not a good idea to allow anything here :-)

                    Of course we can't check unknown names, which are not special ones.
                    But we decide, that it's not allowed to use "_" as first sign
                    (because we reserve this letter for our own purposes!)
                    and the value must not a well known special target.

            @param  sName
                    the new frame name, which should be checked.
         */
        static sal_Bool isValidNameForFrame(const ::rtl::OUString& sName);
};

} // namespace framework

#endif // #ifndef __FRAMEWORK_TARGETHELPER_HXX_
