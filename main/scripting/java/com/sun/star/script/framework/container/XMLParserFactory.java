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



package com.sun.star.script.framework.container;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;

import javax.xml.parsers.*;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Document;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

public class XMLParserFactory {

    private static XMLParser parser = null;
    private static String officedtdurl = null;

    private XMLParserFactory() {}

    public static XMLParser getParser() {
        if (parser == null) {
            synchronized (XMLParserFactory.class) {
                if (parser == null)
                    parser = new DefaultParser();
            }
        }
        return parser;
    }

    public static void setParser(XMLParser p) {
        parser = p;
    }

    public static void setOfficeDTDURL(String url) {
        officedtdurl = url;
    }

    private static class DefaultParser implements XMLParser {

        private DocumentBuilderFactory factory;

        public DefaultParser() {
            factory = DocumentBuilderFactory.newInstance();
        }

        public Document parse(InputStream inputStream) throws IOException {
            Document result = null;
            InputSource is = null;

            try {
                DocumentBuilder builder = factory.newDocumentBuilder();

                is = new InputSource(inputStream);

                if (officedtdurl != null) {
                    is.setSystemId(officedtdurl);
                }

                result = builder.parse(is);
            }
            catch (SAXParseException spe) {
                throw new IOException(spe.getMessage());
            }
            catch (SAXException se) {
                throw new IOException(se.getMessage());
            }
            catch (ParserConfigurationException pce) {
                throw new IOException(pce.getMessage());
            }
            return result;
        }

        public void write(Document doc, OutputStream out) throws IOException {
            try {
                Transformer transformer = TransformerFactory.newInstance().newTransformer();
                transformer.transform(new DOMSource(doc), new StreamResult(out));
            } catch (Exception ex) {
                throw new IOException(ex.getMessage());
            }
        }
    }
}
