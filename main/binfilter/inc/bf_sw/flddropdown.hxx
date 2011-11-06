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


#ifndef _FLDDROPDOWN_HXX
#define _FLDDROPDOWN_HXX

#include <bf_svtools/bf_solar.h>

#include <com/sun/star/uno/Sequence.hxx>
#include <fldbas.hxx>
#include <vector>
namespace binfilter {

using namespace ::com::sun::star::uno;
using namespace ::rtl;
using namespace std;

/** 
    Field type for dropdown boxes.
*/
class SwDropDownFieldType : public SwFieldType
{
public:
    /**
       Constructor
    */
    SwDropDownFieldType();

    /**
       Destructor
    */
    virtual ~SwDropDownFieldType();

    /**
       Create a copy of this field type.

       @return a copy of this type
    */
    virtual SwFieldType * Copy () const;
};

/**
   Dropdown field.

   The dropdown field contains a list of strings. At most one of them
   can be selected.
*/
class SwDropDownField : public SwField
{
    /**
       the possible values (aka items) of the dropdown box
    */
    vector<String> aValues;

    /**
      the selected item
    */
    String aSelectedItem;

    /**
      the name of the field
    */
    String aName;

public:
    /**
       Constructor

       @param pTyp field type for this field
    */
    SwDropDownField(SwFieldType * pTyp);

    /**
       Copy constructor

       @param rSrc dropdown field to copy
    */
    SwDropDownField(const SwDropDownField & rSrc);

    /**
       Destructor
    */
    virtual ~SwDropDownField();

    /**
       Expands the field.

       The expanded value of the field is the value of the selected
       item. If no item is selected, an empty string is returned.

       @return the expanded value of the field
    */
    virtual String Expand() const;

    /**
       Creates a copy of this field.

       @return the copy of this field
    */
    virtual SwField * Copy() const;

    /**
       Returns the selected value.

       @see Expand

       @return the selected value
    */
    virtual const String & GetPar1() const;

    /**
       Returns the name of the field.
       
       @return the name of the field
    */
    virtual String GetPar2() const;

    /**
       Sets the selected value.

       If rStr is an item of the field that item will be
       selected. Otherwise no item will be selected, i.e. the
       resulting selection will be empty.
    */
    virtual void SetPar1(const String & rStr);

    /**
       Sets the name of the field.

       @param rStr the new name of the field
    */
    virtual void SetPar2(const String & rStr);

    /**
       Sets the items of the dropdown box.

       After setting the items the selection will be empty.

       @param rItems the new items
    */
    void SetItems(const Sequence<OUString> & rItems);

    /** 
        Returns the items of the dropdown box.
        
        @return the items of the dropdown box
    */
    Sequence<OUString> GetItemSequence() const;

    /**
       Returns the selected item.

       @return the selected item
    */
    const String & GetSelectedItem() const;

    /**
       Returns the name of the field.

       @return the name of the field
    */
    const String & GetName() const;

    /**
       Sets the selected item.

       If rItem is found in this dropdown field it is selected. If
       rItem is not found the selection will be empty.

       @param rItem the item to be set

       @retval TRUE the selected item was successfully set
       @retval TRUE failure (empty selection)
    */
    BOOL SetSelectedItem(const String & rItem);

    /**
       Sets the name of the field.

       @param rName the new name of the field
    */
    void SetName(const String & rName);

    /**
       API: Gets a property value from the dropdown field.

       @param rVal return value
       @param nMId 
          - FIELD_PROP_PAR1 Get selected item (String)
          - FIELD_PROP_STRINGS Get all items (Sequence)
    */
    virtual BOOL QueryValue(Any &rVal, BYTE nMId) const;

    /**
       API: Sets a property value on the dropdown field.

       @param rVal value to set
       @param nMId 
          - FIELD_PROP_PAR1 Set selected item (String)
          - FIELD_PROP_STRINGS Set all items (Sequence)
    */
        virtual BOOL PutValue(const Any &rVal, BYTE nMId);
};

} //namespace binfilter
#endif
