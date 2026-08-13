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


#ifndef _I18N_CALENDARIMPL_HXX_
#define _I18N_CALENDARIMPL_HXX_

#include <com/sun/star/i18n/XExtendedCalendar.hpp>
#include <com/sun/star/i18n/CalendarDisplayCode.hpp>
#include <com/sun/star/i18n/CalendarFieldIndex.hpp>
#include <com/sun/star/i18n/CalendarDisplayIndex.hpp>
#include <cppuhelper/implbase2.hxx> // helper for implementations
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <vector>

//	----------------------------------------------------
//	class CalendarImpl
//	----------------------------------------------------

namespace com { namespace sun { namespace star { namespace i18n {

class CalendarImpl : public cppu::WeakImplHelper2
<
	com::sun::star::i18n::XExtendedCalendar,
	com::sun::star::lang::XServiceInfo
>
{
public:

    // Constructors
    CalendarImpl() {};
    CalendarImpl(const com::sun::star::uno::Reference < com::sun::star::lang::XMultiServiceFactory >& rxMSF);

    /**
    * Destructor
    */
    ~CalendarImpl();


    // Methods
    virtual void SAL_CALL loadDefaultCalendar(const com::sun::star::lang::Locale& rLocale);
    virtual void SAL_CALL loadCalendar(const rtl::OUString& uniqueID, const com::sun::star::lang::Locale& rLocale);
    virtual Calendar SAL_CALL getLoadedCalendar();
    virtual com::sun::star::uno::Sequence < rtl::OUString > SAL_CALL getAllCalendars(const com::sun::star::lang::Locale& rLocale);
    virtual rtl::OUString SAL_CALL getUniqueID();
    virtual void SAL_CALL setDateTime(double nTimeInDays);
    virtual double SAL_CALL getDateTime();
    virtual void SAL_CALL setValue( sal_Int16 nFieldIndex, sal_Int16 nValue );
    virtual sal_Int16 SAL_CALL getValue(sal_Int16 nFieldIndex);
    virtual sal_Bool SAL_CALL isValid();
    virtual void SAL_CALL addValue(sal_Int16 nFieldIndex, sal_Int32 nAmount);
    virtual sal_Int16 SAL_CALL getFirstDayOfWeek();
    virtual void SAL_CALL setFirstDayOfWeek(sal_Int16 nDay);
    virtual void SAL_CALL setMinimumNumberOfDaysForFirstWeek(sal_Int16 nDays);
    virtual sal_Int16 SAL_CALL getMinimumNumberOfDaysForFirstWeek();
    virtual sal_Int16 SAL_CALL getNumberOfMonthsInYear();
    virtual sal_Int16 SAL_CALL getNumberOfDaysInWeek();
    virtual com::sun::star::uno::Sequence < CalendarItem > SAL_CALL getMonths();
    virtual com::sun::star::uno::Sequence < CalendarItem > SAL_CALL getDays();
    virtual rtl::OUString SAL_CALL getDisplayName(sal_Int16 nCalendarDisplayIndex, sal_Int16 nIdx, sal_Int16 nNameType);

    // Methods in XExtendedCalendar
    virtual rtl::OUString SAL_CALL getDisplayString( sal_Int32 nCalendarDisplayCode, sal_Int16 nNativeNumberMode );

    //XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
    virtual com::sun::star::uno::Sequence < rtl::OUString > SAL_CALL getSupportedServiceNames();

private:
    struct lookupTableItem {
	lookupTableItem(const rtl::OUString& _uniqueID, com::sun::star::uno::Reference < com::sun::star::i18n::XExtendedCalendar >& _xCalendar) :
		uniqueID(_uniqueID), xCalendar(_xCalendar) {}
	rtl::OUString uniqueID;
	com::sun::star::uno::Reference < com::sun::star::i18n::XExtendedCalendar > xCalendar;
    };
	std::vector<lookupTableItem*> lookupTable;
    com::sun::star::uno::Reference < com::sun::star::lang::XMultiServiceFactory > xMSF;
    com::sun::star::uno::Reference < com::sun::star::i18n::XExtendedCalendar > xCalendar;
};

} } } }

#endif
