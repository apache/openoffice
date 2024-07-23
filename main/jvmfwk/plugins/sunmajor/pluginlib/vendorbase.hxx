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



#if !defined INCLUDED_JFW_PLUGIN_VENDORBASE_HXX
#define INCLUDED_JFW_PLUGIN_VENDORBASE_HXX

#include "rtl/ustring.hxx"
#include "rtl/ref.hxx"
#include "osl/endian.h"
#include "salhelper/simplereferenceobject.hxx"
#include <vector>

namespace jfw_plugin
{


//Used by subclasses of VendorBase to build paths to Java runtime
#if defined(__sparcv9)
#define JFW_PLUGIN_ARCH "sparcv9"
#elif defined SPARC
#define JFW_PLUGIN_ARCH "sparc"
#elif defined X86_64
#define JFW_PLUGIN_ARCH "amd64"
#elif defined INTEL
#define JFW_PLUGIN_ARCH "i386"
#elif defined POWERPC64
#define JFW_PLUGIN_ARCH "ppc64"
#elif defined POWERPC
#define JFW_PLUGIN_ARCH "ppc"
#elif defined MIPS
#ifdef OSL_BIGENDIAN
#  define JFW_PLUGIN_ARCH "mips"
#else
#  define JFW_PLUGIN_ARCH "mips32"
#endif
#elif defined S390X
#define JFW_PLUGIN_ARCH "s390x"
#elif defined S390
#define JFW_PLUGIN_ARCH "s390"
#elif defined ARM
#define JFW_PLUGIN_ARCH "arm"
#elif defined IA64
#define JFW_PLUGIN_ARCH "ia64"
#elif defined M68K
#define JFW_PLUGIN_ARCH "m68k"
#elif defined HPPA
#define JFW_PLUGIN_ARCH "parisc"
#elif defined AXP
#define JFW_PLUGIN_ARCH "alpha"
#else // SPARC, INTEL, POWERPC, MIPS, ARM, IA64, M68K, HPPA, ALPHA
#error unknown platform
#endif // SPARC, INTEL, POWERPC, MIPS, ARM, IA64, M68K, HPPA, ALPHA


class MalformedVersionException
{
public:
    MalformedVersionException();
    
    MalformedVersionException(const MalformedVersionException &);
    
    virtual ~MalformedVersionException();
    
    MalformedVersionException & operator =(const MalformedVersionException &);
};

class VendorBase: public salhelper::SimpleReferenceObject
{
public:
    VendorBase();
    /* returns relativ paths to the java executable as
       file URLs.

       For example "bin/java.exe". You need
       to implement this function in a derived class, if
       the paths differ. This implementation provides for
       Windows "bin/java.exe" and for Unix "bin/java".
       The paths are relative file URLs. That is, they always
       contain '/' even on windows. The paths are relative
       to the installation directory of a JRE.
      

       The signature of this function must correspond to
       getJavaExePaths_func.
     */
    static char const* const *  getJavaExePaths(int* size);

    /* creates an instance of this class. MUST be overridden
       in a derived class.
       ####################################################
       OVERRIDE in derived class
       ###################################################
       @param
       Key - value pairs of the system properties of the JRE.
     */
    static rtl::Reference<VendorBase> createInstance();

    /* called automatically on the instance created by createInstance.

       @return
       true - the object could completely initialize.
       false - the object could not completely initialize. In this case
       it will be discarded by the caller.
    */
    virtual bool initialize(
        std::vector<std::pair<rtl::OUString, rtl::OUString> > props);

    /* returns relative file URLs to the runtime library.
       For example         "/bin/client/jvm.dll"
    */
    virtual char const* const* getRuntimePaths(int* size);

    virtual char const* const* getLibraryPaths(int* size);

    virtual const rtl::OUString & getVendor() const;
    virtual const rtl::OUString & getVersion() const;
    virtual const rtl::OUString & getHome() const;
    virtual const rtl::OUString & getRuntimeLibrary() const;
    virtual const rtl::OUString & getLibraryPaths() const;
    virtual bool supportsAccessibility() const;
    /* determines if prior to running java something has to be done,
       like setting the LD_LIBRARY_PATH. This implementation checks
       if an LD_LIBRARY_PATH (getLD_LIBRARY_PATH) needs to be set and
       if so, needsRestart returns true.
     */
    virtual bool needsRestart() const;
    
    /* compares versions of this vendor. MUST be overridden
       in a derived class.
       ####################################################
       OVERRIDE in derived class
       ###################################################
      @return
      0 this.version == sSecond
      1 this.version > sSecond
      -1 this.version < sSEcond
      
      @throw
      MalformedVersionException if the version string was not recognized.
     */
    virtual int compareVersions(const rtl::OUString& sSecond) const;

protected:

    rtl::OUString m_sVendor;
    rtl::OUString m_sVersion;
    rtl::OUString m_sHome;
    rtl::OUString m_sRuntimeLibrary;
    rtl::OUString m_sLD_LIBRARY_PATH;
    bool m_bAccessibility;


    typedef rtl::Reference<VendorBase> (* createInstance_func) ();
    friend rtl::Reference<VendorBase> createInstance(
        createInstance_func pFunc,
        std::vector<std::pair<rtl::OUString, rtl::OUString> > properties);
};

}

#endif
