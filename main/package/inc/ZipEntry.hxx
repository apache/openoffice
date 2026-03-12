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


#ifndef _ZIP_ENTRY_HXX_
#define _ZIP_ENTRY_HXX_

#include <rtl/ustring.hxx>

/// Data from a central directory header.
struct ZipEntry
{
    /// version needed to extract
    sal_Int16 nVersion;
    /// general purpose bit flag
    sal_Int16 nFlag;
    /// compression method
    sal_Int16 nMethod;
    /// last mod file date & time
    sal_Int32 nTime;
    /// crc-32
    sal_Int32 nCrc;
    /// compressed size
    sal_Int32 nCompressedSize;
    /// uncompressed size
    sal_Int32 nSize;
    /// (Absolute) offset to the local file header
    sal_Int32 nFileHeaderOffset;
    /// (Absolute) offset to the file data
    sal_Int32 nFileDataOffset;
    /// file name length
    sal_Int16 nPathLen;
    /// extra field length of local file header
    sal_Int16 nLOCExtraLen;
    /// extra field length of central directory header
    sal_Int16 nCENExtraLen;
    /// file name
    ::rtl::OUString sPath;
    /// True if the entry has a data descriptor after the file data
    sal_Bool bHasDataDescriptor;
    /// Default constructor
    ZipEntry():
        nVersion(-1),
        nFlag(0),
        nMethod(0),
        nTime(-1),
        nCrc(0),
        nCompressedSize(0),
        nSize(0),
        nFileHeaderOffset(-1),
        nFileDataOffset(-1),
        nPathLen(0),
        nLOCExtraLen(0),
        nCENExtraLen(0),
        bHasDataDescriptor(sal_False) {
    }
};
#endif 
