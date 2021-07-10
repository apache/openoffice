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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_desktop.hxx"

#include "dp_descriptioninfoset.hxx"

#include "dp_resource.h"
#include "sal/config.h"

#include "comphelper/sequence.hxx"
#include "comphelper/seqstream.hxx"
#include "comphelper/makesequence.hxx"
#include "comphelper/processfactory.hxx"
#include "boost/optional.hpp"
#include "com/sun/star/container/XNameAccess.hpp"
#include "com/sun/star/beans/Optional.hpp"
#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/beans/XPropertySet.hpp"
#include "com/sun/star/io/SequenceInputStream.hpp"
#include "com/sun/star/lang/XMultiComponentFactory.hpp"
#include "com/sun/star/lang/Locale.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "com/sun/star/xml/dom/DOMException.hpp"
#include "com/sun/star/xml/dom/XNode.hpp"
#include "com/sun/star/xml/dom/XNodeList.hpp"
#include "com/sun/star/xml/dom/XDocumentBuilder.hpp"
#include "com/sun/star/xml/xpath/XXPathAPI.hpp"
#include "com/sun/star/ucb/InteractiveAugmentedIOException.hpp"
#include "cppuhelper/implbase1.hxx"
#include "cppuhelper/implbase2.hxx"
#include "cppuhelper/weak.hxx"
#include "cppuhelper/exc_hlp.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"
#include "ucbhelper/content.hxx"

namespace {

namespace css = ::com::sun::star;
using css::uno::Reference;
using ::rtl::OUString;

class EmptyNodeList: public ::cppu::WeakImplHelper1< css::xml::dom::XNodeList >
{
public:
	EmptyNodeList();

	virtual ~EmptyNodeList();

	virtual ::sal_Int32 SAL_CALL getLength() throw (css::uno::RuntimeException);

	virtual css::uno::Reference< css::xml::dom::XNode > SAL_CALL
	item(::sal_Int32 index) throw (css::uno::RuntimeException);

private:
	EmptyNodeList(EmptyNodeList &); // not defined
	void operator =(EmptyNodeList &); // not defined
};

EmptyNodeList::EmptyNodeList() {}

EmptyNodeList::~EmptyNodeList() {}

::sal_Int32 EmptyNodeList::getLength() throw (css::uno::RuntimeException) {
	return 0;
}

css::uno::Reference< css::xml::dom::XNode > EmptyNodeList::item(::sal_Int32)
	throw (css::uno::RuntimeException)
{
	throw css::uno::RuntimeException(
		::rtl::OUString(
			RTL_CONSTASCII_USTRINGPARAM(
				"bad EmptyNodeList com.sun.star.xml.dom.XNodeList.item call")),
		static_cast< ::cppu::OWeakObject * >(this));
}

::rtl::OUString getNodeValue(
	css::uno::Reference< css::xml::dom::XNode > const & node)
{
    OSL_ASSERT(node.is());
    try {
        return node->getNodeValue();
    } catch (css::xml::dom::DOMException & e) {
        throw css::uno::RuntimeException(
            (::rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM(
                    "com.sun.star.xml.dom.DOMException: ")) +
             e.Message),
            css::uno::Reference< css::uno::XInterface >());
    }
}

/**The class uses the UCB to access the description.xml file in an
   extension. The UCB must have been initialized already. It also
   requires that the extension has already be unzipped to a particular
   location.
 */
class ExtensionDescription
{
public:
    /**throws an exception if the description.xml is not
		available, cannot be read, does not contain the expected data,
		or any other error occurred. Therefore it should only be used with
		new extensions.

        Throws com::sun::star::uno::RuntimeException,
        com::sun::star::deployment::DeploymentException,
        dp_registry::backend::bundle::NoDescriptionException.
     */
    ExtensionDescription(
        const css::uno::Reference<css::uno::XComponentContext>& xContext,
        const ::rtl::OUString& installDir,
        const css::uno::Reference< css::ucb::XCommandEnvironment >& xCmdEnv);

	~ExtensionDescription();

	css::uno::Reference<css::xml::dom::XNode> getRootElement() const
	{
		return m_xRoot;
	}

	::rtl::OUString getExtensionRootUrl() const
	{
		return m_sExtensionRootUrl;
	}


private:
	css::uno::Reference<css::xml::dom::XNode> m_xRoot;
	::rtl::OUString m_sExtensionRootUrl;
};

class NoDescriptionException
{
};

class FileDoesNotExistFilter
    : public ::cppu::WeakImplHelper2< css::ucb::XCommandEnvironment,
                                      css::task::XInteractionHandler >

{
    //css::uno::Reference<css::task::XInteractionHandler> m_xHandler;
	bool m_bExist;
	css::uno::Reference< css::ucb::XCommandEnvironment > m_xCommandEnv;

public:
    virtual ~FileDoesNotExistFilter();
	FileDoesNotExistFilter(
		const css::uno::Reference< css::ucb::XCommandEnvironment >& xCmdEnv);

	bool exist();
    // XCommandEnvironment
    virtual css::uno::Reference<css::task::XInteractionHandler > SAL_CALL
    getInteractionHandler() throw (css::uno::RuntimeException);
    virtual css::uno::Reference<css::ucb::XProgressHandler >
    SAL_CALL getProgressHandler() throw (css::uno::RuntimeException);

    // XInteractionHandler
    virtual void SAL_CALL handle(
        css::uno::Reference<css::task::XInteractionRequest > const & xRequest )
        throw (css::uno::RuntimeException);
};

ExtensionDescription::ExtensionDescription(
    const Reference<css::uno::XComponentContext>& xContext,
    const OUString& installDir,
    const Reference< css::ucb::XCommandEnvironment >& xCmdEnv)
{
    try {
        m_sExtensionRootUrl = installDir;
        //may throw ::com::sun::star::ucb::ContentCreationException
        //If there is no description.xml then ucb will start an interaction which
        //brings up a dialog.We want to prevent this. Therefore we wrap the xCmdEnv
        //and filter the respective exception out.
        OUString sDescriptionUri(installDir + OUSTR("/description.xml"));
        Reference<css::ucb::XCommandEnvironment> xFilter =
            static_cast<css::ucb::XCommandEnvironment*>(
                new FileDoesNotExistFilter(xCmdEnv));
        ::ucbhelper::Content descContent(sDescriptionUri, xFilter);

        //throws an com::sun::star::uno::Exception if the file is not available
        Reference<css::io::XInputStream> xIn;
        try
        {	//throws com.sun.star.ucb.InteractiveAugmentedIOException
            xIn = descContent.openStream();
        }
        catch (css::uno::Exception& )
        {
            if ( ! static_cast<FileDoesNotExistFilter*>(xFilter.get())->exist())
                throw NoDescriptionException();
            throw;
        }
        if (!xIn.is())
        {
            throw css::uno::Exception(
                OUSTR("Could not get XInputStream for description.xml of extension ") +
                sDescriptionUri, 0);
        }

        //get root node of description.xml
        Reference<css::xml::dom::XDocumentBuilder> xDocBuilder(
            xContext->getServiceManager()->createInstanceWithContext(
                OUSTR("com.sun.star.xml.dom.DocumentBuilder"),
                xContext ), css::uno::UNO_QUERY);
        if (!xDocBuilder.is())
            throw css::uno::Exception(OUSTR(" Could not create service com.sun.star.xml.dom.DocumentBuilder"), 0);

        if (xDocBuilder->isNamespaceAware() == sal_False)
        {
            throw css::uno::Exception(
                OUSTR("Service com.sun.star.xml.dom.DocumentBuilder is not namespace aware."), 0);
        }

        Reference<css::xml::dom::XDocument> xDoc = xDocBuilder->parse(xIn);
        if (!xDoc.is())
        {
            throw css::uno::Exception(sDescriptionUri + OUSTR(" contains data which cannot be parsed. "), 0);
        }

        //check for proper root element and namespace
        Reference<css::xml::dom::XElement> xRoot = xDoc->getDocumentElement();
        if (!xRoot.is())
        {
            throw css::uno::Exception(
                sDescriptionUri + OUSTR(" contains no root element."), 0);
        }

        if ( ! xRoot->getTagName().equals(OUSTR("description")))
        {
            throw css::uno::Exception(
                sDescriptionUri + OUSTR(" does not contain the root element <description>."), 0);
        }

        m_xRoot = Reference<css::xml::dom::XNode>(
            xRoot, css::uno::UNO_QUERY_THROW);
        OUString nsDescription = xRoot->getNamespaceURI();

        //check if this namespace is supported
        if ( ! nsDescription.equals(OUSTR("http://openoffice.org/extensions/description/2006")))
        {
            throw css::uno::Exception(sDescriptionUri + OUSTR(" contains a root element with an unsupported namespace. "), 0);
        }
    } catch (css::uno::RuntimeException &) {
        throw;
    } catch (css::deployment::DeploymentException &) {
        throw;
    } catch (css::uno::Exception & e) {
        css::uno::Any a(cppu::getCaughtException());
        throw css::deployment::DeploymentException(
			e.Message, Reference< css::uno::XInterface >(), a);
    }
}

ExtensionDescription::~ExtensionDescription()
{
}

//======================================================================
FileDoesNotExistFilter::FileDoesNotExistFilter(
	const Reference< css::ucb::XCommandEnvironment >& xCmdEnv):
	m_bExist(true), m_xCommandEnv(xCmdEnv)
{}

FileDoesNotExistFilter::~FileDoesNotExistFilter()
{
};

bool FileDoesNotExistFilter::exist()
{
	return m_bExist;
}
    // XCommandEnvironment
Reference<css::task::XInteractionHandler >
    FileDoesNotExistFilter::getInteractionHandler() throw (css::uno::RuntimeException)
{
	return static_cast<css::task::XInteractionHandler*>(this);
}

Reference<css::ucb::XProgressHandler >
    FileDoesNotExistFilter::getProgressHandler() throw (css::uno::RuntimeException)
{
	return m_xCommandEnv.is()
        ? m_xCommandEnv->getProgressHandler()
        : Reference<css::ucb::XProgressHandler>();
}

// XInteractionHandler
//If the interaction was caused by a non-existing file which is specified in the ctor
//of FileDoesNotExistFilter, then we do nothing
void  FileDoesNotExistFilter::handle(
        Reference<css::task::XInteractionRequest > const & xRequest )
        throw (css::uno::RuntimeException)
{
	css::uno::Any request( xRequest->getRequest() );

	css::ucb::InteractiveAugmentedIOException ioexc;
	if ((request>>= ioexc) && ioexc.Code == css::ucb::IOErrorCode_NOT_EXISTING )
	{
		m_bExist = false;
		return;
	}
	Reference<css::task::XInteractionHandler> xInteraction;
    if (m_xCommandEnv.is()) {
        xInteraction = m_xCommandEnv->getInteractionHandler();
    }
    if (xInteraction.is()) {
        xInteraction->handle(xRequest);
    }
}

}

namespace dp_misc {

DescriptionInfoset getDescriptionInfoset(OUString const & sExtensionFolderURL)
{
    Reference< css::xml::dom::XNode > root;
    Reference<css::uno::XComponentContext> context =
        comphelper_getProcessComponentContext();
    OSL_ASSERT(context.is());
    try {
        root =
            ExtensionDescription(
                context, sExtensionFolderURL,
                Reference< css::ucb::XCommandEnvironment >()).
            getRootElement();
    } catch (NoDescriptionException &) {
    } catch (css::deployment::DeploymentException & e) {
        throw css::uno::RuntimeException(
            (OUString(
                RTL_CONSTASCII_USTRINGPARAM(
                    "com.sun.star.deployment.DeploymentException: ")) +
             e.Message), 0);
    }
    return DescriptionInfoset(context, root);
}

DescriptionInfoset::DescriptionInfoset(
    css::uno::Reference< css::uno::XComponentContext > const & context,
    css::uno::Reference< css::xml::dom::XNode > const & element):
    m_context(context),
    m_element(element)
{
    css::uno::Reference< css::lang::XMultiComponentFactory > manager(
        context->getServiceManager(), css::uno::UNO_QUERY_THROW);
    if (m_element.is()) {
        m_xpath = css::uno::Reference< css::xml::xpath::XXPathAPI >(
            manager->createInstanceWithContext(
                ::rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM(
                        "com.sun.star.xml.xpath.XPathAPI")),
                context),
            css::uno::UNO_QUERY_THROW);
        m_xpath->registerNS(
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("desc")),
            element->getNamespaceURI());
        m_xpath->registerNS(
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("xlink")),
            ::rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM("http://www.w3.org/1999/xlink")));
    }
}

DescriptionInfoset::~DescriptionInfoset() {}

::boost::optional< ::rtl::OUString > DescriptionInfoset::getIdentifier() const {
    return getOptionalValue(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("desc:identifier/@value")));
}

::rtl::OUString DescriptionInfoset::getNodeValueFromExpression(::rtl::OUString const & expression) const
{
    css::uno::Reference< css::xml::dom::XNode > n;
    if (m_element.is()) {
        try {
            n = m_xpath->selectSingleNode(m_element, expression);
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }
    }
    return n.is() ? getNodeValue(n) : ::rtl::OUString();
}

void DescriptionInfoset::checkBlacklist() const
{
    if (m_element.is()) {
        boost::optional< OUString > id(getIdentifier());
        if (!id)
            return; // nothing to check
        OUString currentversion(getVersion());
        if (currentversion.getLength() == 0)
            return; // nothing to check

        css::uno::Reference< css::lang::XMultiComponentFactory > manager(
            m_context->getServiceManager(), css::uno::UNO_QUERY_THROW);
        css::uno::Reference< css::lang::XMultiServiceFactory> provider(
            manager->createInstanceWithContext(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.configuration.ConfigurationProvider")), m_context),
                css::uno::UNO_QUERY_THROW);

        css::uno::Sequence< css::uno::Any > args = css::uno::Sequence< css::uno::Any >(1);
        css::beans::PropertyValue prop;
        prop.Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("nodepath"));
        prop.Value <<= ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Office.ExtensionDependencies/Extensions"));
        args[0] <<= prop;

        css::uno::Reference< css::container::XNameAccess > blacklist(
            provider->createInstanceWithArguments(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.configuration.ConfigurationAccess")), args),
                css::uno::UNO_QUERY_THROW);

        // check first if a blacklist entry is available
        if (blacklist.is() && blacklist->hasByName(*id)) {
            css::uno::Reference< css::beans::XPropertySet > extProps(
                blacklist->getByName(*id), css::uno::UNO_QUERY_THROW);

            css::uno::Any anyValue = extProps->getPropertyValue(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Versions")));

            css::uno::Sequence< ::rtl::OUString > blversions;
            anyValue >>= blversions;

            // check if the current version requires further dependency checks from the blacklist
            if (checkBlacklistVersion(currentversion, blversions)) {
                anyValue = extProps->getPropertyValue(
                    ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Dependencies")));
                ::rtl::OUString udeps;
                anyValue >>= udeps;

                if (udeps.getLength() == 0)
                    return; // nothing to do

                ::rtl::OString xmlDependencies = ::rtl::OUStringToOString(udeps, RTL_TEXTENCODING_UNICODE);

                css::uno::Reference< css::xml::dom::XDocumentBuilder> docbuilder(
                    manager->createInstanceWithContext(
                        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.xml.dom.DocumentBuilder")), m_context),
                    css::uno::UNO_QUERY_THROW);

                css::uno::Sequence< sal_Int8 > byteSeq((const sal_Int8*)xmlDependencies.getStr(), xmlDependencies.getLength());

                css::uno::Reference< css::io::XInputStream> inputstream( css::io::SequenceInputStream::createStreamFromSequence(m_context, byteSeq),
                                                                         css::uno::UNO_QUERY_THROW);

                css::uno::Reference< css::xml::dom::XDocument > xDocument(docbuilder->parse(inputstream));
                css::uno::Reference< css::xml::dom::XElement > xElement(xDocument->getDocumentElement());
                css::uno::Reference< css::xml::dom::XNodeList > xDeps(xElement->getChildNodes());
                sal_Int32 nLen = xDeps->getLength();

                // get the parent xml document of current description info for the import
                css::uno::Reference< css::xml::dom::XDocument > xCurrentDescInfo(m_element->getOwnerDocument());

                // get dependency node of current description info to merge the new dependencies from the blacklist
                css::uno::Reference< css::xml::dom::XNode > xCurrentDeps(
                    m_xpath->selectSingleNode(m_element, ::rtl::OUString(
                                                  RTL_CONSTASCII_USTRINGPARAM("desc:dependencies"))));

                // if no dependency node exists, create a new one in the current description info
                if (!xCurrentDeps.is()) {
                    css::uno::Reference< css::xml::dom::XNode > xNewDepNode(
                        xCurrentDescInfo->createElementNS(
                            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("http://openoffice.org/extensions/description/2006")),
                            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("dependencies"))), css::uno::UNO_QUERY_THROW);
                    m_element->appendChild(xNewDepNode);
                    xCurrentDeps = m_xpath->selectSingleNode(m_element, ::rtl::OUString(
                                                  RTL_CONSTASCII_USTRINGPARAM("desc:dependencies")));
                }

                for (sal_Int32 i=0; i<nLen; i++) {
                    css::uno::Reference< css::xml::dom::XNode > xNode(xDeps->item(i));
                    css::uno::Reference< css::xml::dom::XElement > xDep(xNode, css::uno::UNO_QUERY);
                    if (xDep.is()) {
                        // found valid blacklist dependency, import the node first and append it to the existing dependency node
                        css::uno::Reference< css::xml::dom::XNode > importedNode = xCurrentDescInfo->importNode(xNode, true);
                        xCurrentDeps->appendChild(importedNode);
                    }
                }
            }
        }
    }
}

bool DescriptionInfoset::checkBlacklistVersion(
    ::rtl::OUString currentversion,
    ::com::sun::star::uno::Sequence< ::rtl::OUString > const & versions) const
{
    sal_Int32 nLen = versions.getLength();
    for (sal_Int32 i=0; i<nLen; i++) {
        if (currentversion.equals(versions[i]))
            return true;
    }

    return false;
}

::rtl::OUString DescriptionInfoset::getVersion() const
{
    return getNodeValueFromExpression( ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("desc:version/@value")));
}

css::uno::Sequence< ::rtl::OUString > DescriptionInfoset::getSupportedPlaforms() const
{
    //When there is no description.xml then we assume that we support all platforms
    if (! m_element.is())
    {
        return comphelper::makeSequence(
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("all")));
    }

    //Check if the <platform> element was provided. If not the default is "all" platforms
    css::uno::Reference< css::xml::dom::XNode > nodePlatform(
        m_xpath->selectSingleNode(m_element, ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("desc:platform"))));
    if (!nodePlatform.is())
    {
        return comphelper::makeSequence(
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("all")));
    }

    //There is a platform element.
    const ::rtl::OUString value = getNodeValueFromExpression(::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("desc:platform/@value")));
    //parse the string, it can contain multiple strings separated by commas
    ::std::vector< ::rtl::OUString> vec;
    sal_Int32 nIndex = 0;
    do
    {
        ::rtl::OUString aToken = value.getToken( 0, ',', nIndex );
        aToken = aToken.trim();
        if (aToken.getLength())
            vec.push_back(aToken);

    }
    while (nIndex >= 0);

    return comphelper::containerToSequence(vec);
}

css::uno::Reference< css::xml::dom::XNodeList >
DescriptionInfoset::getDependencies() const {
    if (m_element.is()) {
        try {
            // check the extension blacklist first and expand the dependencies if applicable
            checkBlacklist();

            return m_xpath->selectNodeList(m_element, ::rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM("desc:dependencies/*")));
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }
    }
    return new EmptyNodeList;
}

css::uno::Sequence< ::rtl::OUString >
DescriptionInfoset::getUpdateInformationUrls() const {
    return getUrls(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM(
                "desc:update-information/desc:src/@xlink:href")));
}

css::uno::Sequence< ::rtl::OUString >
DescriptionInfoset::getUpdateDownloadUrls() const
{
    return getUrls(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM(
                "desc:update-download/desc:src/@xlink:href")));
}

::rtl::OUString DescriptionInfoset::getIconURL( sal_Bool bHighContrast ) const
{
    css::uno::Sequence< ::rtl::OUString > aStrList = getUrls( ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "desc:icon/desc:default/@xlink:href")));
    css::uno::Sequence< ::rtl::OUString > aStrListHC = getUrls( ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "desc:icon/desc:high-contrast/@xlink:href")));

    if ( bHighContrast && aStrListHC.hasElements() && aStrListHC[0].getLength() )
        return aStrListHC[0];

    if ( aStrList.hasElements() && aStrList[0].getLength() )
        return aStrList[0];

    return ::rtl::OUString();
}

::boost::optional< ::rtl::OUString > DescriptionInfoset::getLocalizedUpdateWebsiteURL()
    const
{
    bool bParentExists = false;
    const ::rtl::OUString sURL (getLocalizedHREFAttrFromChild(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
        "/desc:description/desc:update-website")), &bParentExists ));

    if (sURL.getLength() > 0)
        return ::boost::optional< ::rtl::OUString >(sURL);
    else
        return bParentExists ? ::boost::optional< ::rtl::OUString >(::rtl::OUString()) :
            ::boost::optional< ::rtl::OUString >();
}

::boost::optional< ::rtl::OUString > DescriptionInfoset::getOptionalValue(
    ::rtl::OUString const & expression) const
{
    css::uno::Reference< css::xml::dom::XNode > n;
    if (m_element.is()) {
        try {
            n = m_xpath->selectSingleNode(m_element, expression);
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }
    }
    return n.is()
        ? ::boost::optional< ::rtl::OUString >(getNodeValue(n))
        : ::boost::optional< ::rtl::OUString >();
}

css::uno::Sequence< ::rtl::OUString > DescriptionInfoset::getUrls(
    ::rtl::OUString const & expression) const
{
    css::uno::Reference< css::xml::dom::XNodeList > ns;
    if (m_element.is()) {
        try {
            ns = m_xpath->selectNodeList(m_element, expression);
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }
    }
    css::uno::Sequence< ::rtl::OUString > urls(ns.is() ? ns->getLength() : 0);
    for (::sal_Int32 i = 0; i < urls.getLength(); ++i) {
        urls[i] = getNodeValue(ns->item(i));
    }
    return urls;
}

::std::pair< ::rtl::OUString, ::rtl::OUString > DescriptionInfoset::getLocalizedPublisherNameAndURL() const
{
    css::uno::Reference< css::xml::dom::XNode > node =
        getLocalizedChild(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("desc:publisher")));

    ::rtl::OUString sPublisherName;
    ::rtl::OUString sURL;
    if (node.is())
    {
        const ::rtl::OUString exp1(RTL_CONSTASCII_USTRINGPARAM("text()"));
        css::uno::Reference< css::xml::dom::XNode > xPathName;
        try {
            xPathName = m_xpath->selectSingleNode(node, exp1);
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }
        OSL_ASSERT(xPathName.is());
        if (xPathName.is())
            sPublisherName = xPathName->getNodeValue();

        const ::rtl::OUString exp2(RTL_CONSTASCII_USTRINGPARAM("@xlink:href"));
        css::uno::Reference< css::xml::dom::XNode > xURL;
        try {
            xURL = m_xpath->selectSingleNode(node, exp2);
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }
        OSL_ASSERT(xURL.is());
        if (xURL.is())
           sURL = xURL->getNodeValue();
    }
    return ::std::make_pair(sPublisherName, sURL);
}

::rtl::OUString DescriptionInfoset::getLocalizedReleaseNotesURL() const
{
    return getLocalizedHREFAttrFromChild(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
        "/desc:description/desc:release-notes")), NULL);
}

::rtl::OUString DescriptionInfoset::getLocalizedDisplayName() const
{
    css::uno::Reference< css::xml::dom::XNode > node =
        getLocalizedChild(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("desc:display-name")));
    if (node.is())
    {
        const ::rtl::OUString exp(RTL_CONSTASCII_USTRINGPARAM("text()"));
        css::uno::Reference< css::xml::dom::XNode > xtext;
        try {
            xtext = m_xpath->selectSingleNode(node, exp);
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }
        if (xtext.is())
            return xtext->getNodeValue();
    }
    return ::rtl::OUString();
}

::rtl::OUString DescriptionInfoset::getLocalizedLicenseURL() const
{
    return getLocalizedHREFAttrFromChild(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
        "/desc:description/desc:registration/desc:simple-license")), NULL);

}

::boost::optional<SimpleLicenseAttributes>
DescriptionInfoset::getSimpleLicenseAttributes() const
{
    // Check if the node exists
    css::uno::Reference< css::xml::dom::XNode > n;
    if (m_element.is()) {
        try {
            n = m_xpath->selectSingleNode(m_element,
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                "/desc:description/desc:registration/desc:simple-license/@accept-by")));
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }
        if (n.is())
        {
            SimpleLicenseAttributes attributes;
            attributes.acceptBy =
                getNodeValueFromExpression(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                "/desc:description/desc:registration/desc:simple-license/@accept-by")));

            ::boost::optional< ::rtl::OUString > suppressOnUpdate = getOptionalValue(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                "/desc:description/desc:registration/desc:simple-license/@suppress-on-update")));
            if (suppressOnUpdate)
                attributes.suppressOnUpdate = (*suppressOnUpdate).trim().equalsIgnoreAsciiCase(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("true")));
            else
                attributes.suppressOnUpdate = false;

            ::boost::optional< ::rtl::OUString > suppressIfRequired = getOptionalValue(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                "/desc:description/desc:registration/desc:simple-license/@suppress-if-required")));
            if (suppressIfRequired)
                attributes.suppressIfRequired = (*suppressIfRequired).trim().equalsIgnoreAsciiCase(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("true")));
            else
                attributes.suppressIfRequired = false;

            return ::boost::optional<SimpleLicenseAttributes>(attributes);
        }
    }
    return ::boost::optional<SimpleLicenseAttributes>();
}

::rtl::OUString DescriptionInfoset::getLocalizedDescriptionURL() const
{
    return getLocalizedHREFAttrFromChild(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
        "/desc:description/desc:extension-description")), NULL);
}

css::uno::Reference< css::xml::dom::XNode >
DescriptionInfoset::getLocalizedChild( const ::rtl::OUString & sParent) const
{
    if ( ! m_element.is() || !sParent.getLength())
        return css::uno::Reference< css::xml::dom::XNode > ();

    css::uno::Reference< css::xml::dom::XNode > xParent;
    try {
        xParent = m_xpath->selectSingleNode(m_element, sParent);
    } catch (css::xml::xpath::XPathException &) {
        // ignore
    }
    css::uno::Reference<css::xml::dom::XNode> nodeMatch;
    if (xParent.is())
    {
        const ::rtl::OUString sLocale = getOfficeLocaleString();
        nodeMatch = matchFullLocale(xParent, sLocale);

        //office: en-DE, en, en-DE-altmark
        if (! nodeMatch.is())
        {
            const css::lang::Locale officeLocale = getOfficeLocale();
            nodeMatch = matchCountryAndLanguage(xParent, officeLocale);
            if ( ! nodeMatch.is())
            {
                nodeMatch = matchLanguage(xParent, officeLocale);
                if (! nodeMatch.is())
                    nodeMatch = getChildWithDefaultLocale(xParent);
            }
        }
    }

    return nodeMatch;
}

css::uno::Reference<css::xml::dom::XNode>
DescriptionInfoset::matchFullLocale(css::uno::Reference< css::xml::dom::XNode >
                                    const & xParent, ::rtl::OUString const & sLocale) const
{
    OSL_ASSERT(xParent.is());
    const ::rtl::OUString exp1(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("*[@lang=\""))
        + sLocale +
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\"]")));
    try {
        return m_xpath->selectSingleNode(xParent, exp1);
    } catch (css::xml::xpath::XPathException &) {
        // ignore
        return 0;
    }
}

css::uno::Reference<css::xml::dom::XNode>
DescriptionInfoset::matchCountryAndLanguage(
    css::uno::Reference< css::xml::dom::XNode > const & xParent, css::lang::Locale const & officeLocale) const
{
    OSL_ASSERT(xParent.is());
    css::uno::Reference<css::xml::dom::XNode> nodeMatch;

    if (officeLocale.Country.getLength())
    {
        const ::rtl::OUString sLangCountry(officeLocale.Language +
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("-")) +
            officeLocale.Country);
        // first try exact match for lang-country
        const ::rtl::OUString exp1(
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("*[@lang=\""))
            + sLangCountry +
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\"]")));
        try {
            nodeMatch = m_xpath->selectSingleNode(xParent, exp1);
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }

        //try to match in strings that also have a variant, for example en-US matches in
        //en-US-montana
        if (!nodeMatch.is())
        {
            const ::rtl::OUString exp2(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("*[starts-with(@lang,\""))
                + sLangCountry +
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("-\")]")));
            try {
                nodeMatch = m_xpath->selectSingleNode(xParent, exp2);
            } catch (css::xml::xpath::XPathException &) {
                // ignore
            }
        }
    }

    return nodeMatch;
}


css::uno::Reference<css::xml::dom::XNode>
DescriptionInfoset::matchLanguage(
    css::uno::Reference< css::xml::dom::XNode > const & xParent, css::lang::Locale const & officeLocale) const
{
    OSL_ASSERT(xParent.is());
    css::uno::Reference<css::xml::dom::XNode> nodeMatch;

    //first try exact match for lang
    const ::rtl::OUString exp1(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("*[@lang=\""))
        + officeLocale.Language
        + ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\"]")));
    try {
        nodeMatch = m_xpath->selectSingleNode(xParent, exp1);
    } catch (css::xml::xpath::XPathException &) {
        // ignore
    }

    //try to match in strings that also have a country and/or variant, for example en matches in
    //en-US-montana, en-US, en-montana
    if (!nodeMatch.is())
    {
        const ::rtl::OUString exp2(
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("*[starts-with(@lang,\""))
            + officeLocale.Language
            + ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("-\")]")));
        try {
            nodeMatch = m_xpath->selectSingleNode(xParent, exp2);
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }
    }
    return nodeMatch;
}

css::uno::Reference<css::xml::dom::XNode>
DescriptionInfoset::getChildWithDefaultLocale(css::uno::Reference< css::xml::dom::XNode >
                                    const & xParent) const
{
    OSL_ASSERT(xParent.is());
    if (xParent->getNodeName().equals(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("simple-license"))))
    {
        css::uno::Reference<css::xml::dom::XNode> nodeDefault;
        try {
            nodeDefault = m_xpath->selectSingleNode(xParent, ::rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM("@default-license-id")));
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }
        if (nodeDefault.is())
        {
            //The old way
            const ::rtl::OUString exp1(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("desc:license-text[@license-id = \""))
                + nodeDefault->getNodeValue()
                + ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\"]")));
            try {
                return m_xpath->selectSingleNode(xParent, exp1);
            } catch (css::xml::xpath::XPathException &) {
                // ignore
            }
        }
    }

    const ::rtl::OUString exp2(RTL_CONSTASCII_USTRINGPARAM("*[1]"));
    try {
        return m_xpath->selectSingleNode(xParent, exp2);
    } catch (css::xml::xpath::XPathException &) {
        // ignore
        return 0;
    }
}

::rtl::OUString DescriptionInfoset::getLocalizedHREFAttrFromChild(
    ::rtl::OUString const & sXPathParent, bool * out_bParentExists)
    const
{
    css::uno::Reference< css::xml::dom::XNode > node =
        getLocalizedChild(sXPathParent);

    ::rtl::OUString sURL;
    if (node.is())
    {
        if (out_bParentExists)
            *out_bParentExists = true;
        const ::rtl::OUString exp(RTL_CONSTASCII_USTRINGPARAM("@xlink:href"));
        css::uno::Reference< css::xml::dom::XNode > xURL;
        try {
            xURL = m_xpath->selectSingleNode(node, exp);
        } catch (css::xml::xpath::XPathException &) {
            // ignore
        }
        OSL_ASSERT(xURL.is());
        if (xURL.is())
            sURL = xURL->getNodeValue();
    }
    else
    {
        if (out_bParentExists)
            *out_bParentExists = false;
    }
    return sURL;
}

}
