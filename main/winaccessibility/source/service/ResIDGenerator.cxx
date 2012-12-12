/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright IBM Corporation 2010.
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include "ResIDGenerator.hxx"

/**
   * Destructor 
   *
   * @param   
   * @return  
   */
ResIDGenerator::~ResIDGenerator()
{
}

/**
   * SubList stores those IDs that were ever generated and deleted, the method 
   * return the ID from subList first if subList is not empty,else return ++max. 
   * Add the obsolete IDs by calling SetSub method
   * 
   * @param   
   * @return  new resource ID.
   */
long ResIDGenerator::GenerateNewResID()
{
    if (max == LONG_MAX)
    {
        if (!subList.empty())
        {
            long nRes = *(subList.begin());
            subList.pop_front();
            return nRes;
        }
    }
    return -(++max);
}
