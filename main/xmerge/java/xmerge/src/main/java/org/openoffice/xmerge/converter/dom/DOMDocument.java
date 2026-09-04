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



package org.openoffice.xmerge.converter.dom;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;


import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.ParserConfigurationException;

import javax.xml.transform.TransformerFactory;
import javax.xml.transform.Transformer;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.dom.DOMSource;

import org.w3c.dom.Node;
import org.w3c.dom.Element;
import org.w3c.dom.Document;
import org.xml.sax.SAXException;

import org.openoffice.xmerge.util.Resources;
import org.openoffice.xmerge.util.Debug;

/**
 *  An implementation of <code>Document</code> for
 *  StarOffice documents.
 */
public class DOMDocument
    implements org.openoffice.xmerge.Document {

    /** Factory for <code>DocumentBuilder</code> objects. */
    private static DocumentBuilderFactory factory =
       DocumentBuilderFactory.newInstance();

    /** DOM <code>Document</code> of content.xml. */
    private Document contentDoc = null;

    private String documentName = null;
    private String fileName = null;
    private String fileExt = null;

    /** Resources object. */
    private Resources res = null;


    /**
     *  Default constructor.
     *
     *  @param  name  <code>Document</code> name.
     *  @param  ext   <code>Document</code> extension.
     */
    public DOMDocument(String name,String ext)
    {
	this(name,ext,true, false);
    }

     /**
     *  Returns the file extension of the <code>Document</code>
     *  represented.
     *
     *  @return  file extension of the <code>Document</code>.
     */
    protected String getFileExtension() {
        return fileExt;
    }


    /**
     *  Constructor with arguments to set <code>namespaceAware</code>
     *  and <code>validating</code> flags.
     *
     *  @param  name            <code>Document</code> name (may or may not
     *                          contain extension).
     *  @param  ext             <code>Document</code> extension.
     *  @param  namespaceAware  Value for <code>namespaceAware</code> flag.
     *  @param  validating      Value for <code>validating</code> flag.
     */
    public DOMDocument(String name, String ext,boolean namespaceAware, boolean validating) {

        res = Resources.getInstance();
        factory.setValidating(validating);
        factory.setNamespaceAware(namespaceAware);
        this.fileExt = ext;
	this.documentName = trimDocumentName(name);
        this.fileName = documentName + getFileExtension();
    }


    /**
     *  Removes the file extension from the <code>Document</code>
     *  name.
     *
     *  @param  name  Full <code>Document</code> name with extension.
     *
     *  @return  Name of <code>Document</code> without the extension.
     */
    private String trimDocumentName(String name) {
        String temp = name.toLowerCase();
        String ext = getFileExtension();

        if (temp.endsWith(ext)) {
            // strip the extension
            int nlen = name.length();
            int endIndex = nlen - ext.length();
            name = name.substring(0,endIndex);
        }

        return name;
    }


    /**
     *  Return a DOM <code>Document</code> object of the document content
     *  file.  Note that a content DOM is not created when the constructor
     *  is called.  So, either the <code>read</code> method or the
     *  <code>initContentDOM</code> method will need to be called ahead
     *  on this object before calling this method.
     *
     *  @return  DOM <code>Document</code> object.
     */
    public Document getContentDOM() {

        return contentDoc;
    }

    /**
     *  Sets the Content of the <code>Document</code> to the contents of the
     *  supplied <code>Node</code> list.
     *
     *  @param newDom  DOM <code>Document</code> object.
     */
    public void setContentDOM( Node newDom) {
	contentDoc=(Document)newDom;
    }


    /**
     *  Return the name of the <code>Document</code>.
     *
     *  @return  The name of <code>Document</code>.
     */
    public String getName() {

        return documentName;
    }


    /**
     *  Return the file name of the <code>Document</code>, possibly
     *  with the standard extension.
     *
     *  @return  The file name of <code>Document</code>.
     */
    public String getFileName() {

        return fileName;
    }


    /**
     *  Read the Office <code>Document</code> from the specified
     *  <code>InputStream</code>.
     *
     *  @param  is  Office document <code>InputStream</code>.
     *
     *  @throws  IOException  If any I/O error occurs.
     */
    public void read(InputStream is) throws IOException {
	 Debug.log(Debug.INFO, "reading file");
        DocumentBuilder builder = null;
        try {
            builder = factory.newDocumentBuilder();
        } catch (ParserConfigurationException ex) {
	    System.out.println("Error:"+ ex);
            //throw new OfficeDocumentException(ex);
        }
	try {

	    contentDoc=  builder.parse(is);


        } catch (SAXException ex) {
	    System.out.println("Error:"+ ex);
            //throw new OfficeDocumentException(ex);
        }
    }


    /**
     *  Write out content to the supplied <code>OutputStream</code>.
     *
     *  @param  os  XML <code>OutputStream</code>.
     *
     *  @throws  IOException  If any I/O error occurs.
     */
    public void write(OutputStream os) throws IOException {

        // set bytes for writing to output stream
        byte contentBytes[] = docToBytes(contentDoc);

        os.write(contentBytes);
    }


    /**
     *  <p>Write out a <code>org.w3c.dom.Document</code> object into a
     *  <code>byte</code> array.</p>
     *
     *  @param  doc  DOM <code>Document</code> object.
     *
     *  @return  <code>byte</code> array of DOM <code>Document</code>
     *           object.
     *
     *  @throws  IOException  If any I/O error occurs.
     */
    private byte[] docToBytes(Document doc)
        throws IOException {
        try {
            DOMSource domSource = new DOMSource(doc);
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            StreamResult result = new StreamResult(output);
            TransformerFactory tf = TransformerFactory.newInstance();
            Transformer transformer = tf.newTransformer();
            transformer.transform(domSource, result);
            return output.toByteArray();
        }
        catch (Exception e) {
            throw new IOException("Unable to serialize XML document: " + e);
        }
    }


    /**
     *  Initializes a new DOM <code>Document</code> with the content
     *  containing minimum XML tags.
     *
     *  @throws  IOException  If any I/O error occurs.
     */
    public final void initContentDOM() throws IOException {
        contentDoc = createDOM("");

    }

    /**
     *  <p>Creates a new DOM <code>Document</code> containing minimum
     *  OpenOffice.org XML tags.</p>
     *
     *  <p>This method uses the subclass
     *  <code>getOfficeClassAttribute</code> method to get the
     *  attribute for <i>office:class</i>.</p>
     *
     *  @param  rootName  root name of <code>Document</code>.
     *
     *  @throws  IOException  If any I/O error occurs.
     */
    private final Document createDOM(String rootName) throws IOException {

        Document doc = null;

        try {

            DocumentBuilder builder = factory.newDocumentBuilder();
            doc = builder.newDocument();

        } catch (ParserConfigurationException ex) {
             System.out.println("Error:"+ ex);


        }

        Element root = (Element) doc.createElement(rootName);
        doc.appendChild(root);


        return doc;
    }

}
