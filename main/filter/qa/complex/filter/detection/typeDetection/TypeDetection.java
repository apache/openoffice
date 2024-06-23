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


package complex.filter.detection.typeDetection;

import com.sun.star.beans.PropertyValue;
import com.sun.star.document.XTypeDetection;
import com.sun.star.io.NotConnectedException;
import com.sun.star.io.XInputStream;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import java.io.File;
import java.io.IOException;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import util.utils;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import org.openoffice.test.Argument;
import org.openoffice.test.OfficeConnection;



/** Check "TypeDetection"
 * <p>
 * This test will check the file type detection. This will be done by filling
 * properties of a <code>MediaDescriptor</code>.
 *
 * In the test method <code>checkByURLonly</code> the
 * <code>MediaDescriptor</code> was filled at once with the URL of a test
 * file. At second it was filled with a <code>XInputStream</code> from test
 * file. In both subtests the returned file type must match with an expected
 * type.
 *
 * In the test method <code>checkPreselectedType</code> the
 * <code>MediaDescriptor</code> was filled with the URL of the test file and
 * with the name of a type which should be used. The returned type of the
 * <code>TypeDetection<code> must match with an expected type.
 *
 * In the test method <code>checkPreselectedFilter</code> the
 * <code>MediaDescriptor</code> was filled with the URL of the test file and
 * with the name of a filter which should be used. The returned type of the
 * <code>TypeDetection<code> must match with an expected type.
 *
 * In the test method <code>checkPreselectedDocService</code> the
 * <code>MediaDescriptor</code> was filled with the URL of the test file and
 * with the name of a document service which should be used. The returned type
 * of the <code>TypeDetection<code> must match with an expected type.
 *
 *
 * To get information which test file should support which type, filter and
 * document service, this information was collect from configuration files:
 * <UL>
 * <LI><a href="#TypeDetection.props">TypeDetection.props</a></LI>
 * <LI><a href="#files.csv">files.csv</a></LI>
 * <LI><a href="#preselectedFilter.csv">preselectedFilter.csv</a></LI>
 * <LI><a href="#preselectedType.csv">preselectedType.csv</a></LI>
 * <LI><a href="#serviceName.csv">serviceName.csv</a></LI>
 * </UL>
 * <p>
 * <h3><A name="TypeDetection.props"></A>
 * <code>typeDetection.props</code></h3>
 * At fist there will be the <code>typeDetection.props</code>. Here the following
 * properties should be set (with example values):
 *
 * TestDocumentPath=file:///path/to/my/testdocuments
 * placeHolder=%
 * %SO6productname=StarOffice
 * %SO6formatversion=6.0
 *
 * <code>TestDocumentPath</code>: this is the path to your test documents. If
 * you have grouped your documents ie. by writer, calc, ... then it should be
 * the root path. To specify the particular sub folders you have to use
 * <code>csv.files</code>
 * <p>
 * <code>files.csv</code>: In this file all test documents are listed.
 * Syntax: fileAlias;fileURL;defaultURLFileType;StreamFileTypes
 * Example:
 *
 * Writer6_1;Writer/Writer6.sxw;writer_StarOffice_XML_Writer;writer_StarOffice_XML_Writer
 * text1;Writer/Text.txt;writer_Text_encoded:writer_Text;writer_Text_encoded:writer_Text
 *
 * The first example shows you the following:
 * <code>Writer6_1</code> is a free chosen name
 * <code>Writer/Writer6.sxw</code> is the document path. This will be assembled
 * by <code>TestDocumentPath</code> from <code>typeDetection.props</code>.
 * <code>writer_StarOffice_XML_Writer</code>: this is the default file type of
 * this file
 *
 * The second example displays two document types for <code>XInputStream</CODE>
 * (<code>writer_Text_encoded</CODE> and <code>writer_Text</CODE>. This two
 * document types are listed by a colon ':' as separator. This is needed because
 * XInputStream can detect a text file as writer_Text_encoded as well as
 * writer_Text.
 * <p>
 *
 * <H3><A name="preselectedFilter.csv"</a>
 * <code>preselectedFilter.csv</code></H3>
 * In this file you can choose a special
 * filter to detect the document. This make sense ie. for csv-files: You can
 * open csv files as Writer or as Calc. To check this case you have to specify
 * in <code>csv.files</code> a fileAlias like ?csv_writer? and ?csv_calc? with
 * the same fileURL and it's specific defaultFileType.
 * The returned file type by <code>TypeDetection</code> must equal to
 * correspond <code>defaultFileType</code> from <code>csv.files</code>
 *
 * Syntax: fileAlias;FilterName;FilterOptions;FilterData
 * Example: Writer6_1;%SO6productname %SO6formatversion Textdokument;;
 *
 * The example shows the following:
 * <code>Writer6_1</code> is the same as in <code>csv.files</code>
 * <code>%SO6productname %SO6formatversion Textdokument</code> is the filter
 * name which should be used. Here we have a special: <code>%SO6productname
 * %SO6formatversion</code> will be replaced by the equals of
 * <code>typeDetection.props</code>. The filter names depends on the Office
 * name and version. So a future Office could called ?StarSuite 8?.
 * <code>FilterOptions</code> is not relevant for this filter. But ie. for csv
 * filter this entry could be used to specify the separator of the csv file.
 * <code>FilterData<code> if filter needs some FilterData arguments you can
 * specify it here
 *
 * <p>
 * <H3><a name="preselectedType.csv"></A>
 * <code>preselectedType.csv</code></H3>
 * In this file you can preselect the type
 * <code>TypeDetection</code> should use.
 * The returned file type by <code>TypeDetection</code> must equal to the
 * preselected file type.
 * Note: If you try to use invalid types you will get a failed test because
 * <code>TypeDetection</code> tries to find out the type itself.
 *
 * Syntax: fileAlias;fileType
 * Example: Writer6_1;writer_StarOffice_XML_Writer
 *
 * This example shows the following:
 * <code>Writer6_1</code> is the same as in <code>csv.files</code>
 * <code>writer_StarOffice_XML_Writer</code> is the file type which was used as
 * parameter in <code>MediaDescriptor</code>. This type must be returned from
 * <code>TypeDetection</code>
 *
 * <p>
 * <H3><a name="serviceName.csv"></A>
 * <code>serviceName.csv</code></H3> In this file you can preselect a service name
 * to detect the file type. The returned file type by
 * <code>TypeDetection</code> must equal to correspond
 * <code>defaultFileType</code> from <code>csv.files</code>
 *
 * Syntax: fileAlias;serviceName
 * Example: Writer6_1;com.sun.star.text.FormatDetector
 *
 * This example shows the following:
 * <code>Writer6_1</code> is the same as in <code>csv.files</code>
 * <code>com.sun.star.text.FormatDetector</code> is the service name which was
 * used as parameter in <code>MediaDescriptor</code>.
 *
 *
 * <p>
 * All these files will be copied by make file beside of
 * <code>typeDetection.class</code>.
 * @see com.sun.star.document.XTypeDetection
 * @see com.sun.star.document.MediaDescriptor
 */
public class TypeDetection {
    private static final OfficeConnection connection = new OfficeConnection();

    /** 
     * @member m_xDetection     the object to test
     * @member helper           instacne of helper class
     */
    static XTypeDetection   m_xDetection;
    static Helper helper = null;

    @BeforeClass
    public static void before()
        throws Exception
    {
        connection.setUp();

        // create TypeDetection
        XMultiServiceFactory xMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, connection.getComponentContext().getServiceManager());
        assertTrue("Could not get XMultiServiceFactory", xMSF != null);
        
        Object oInterface = xMSF.createInstance(
                                        "com.sun.star.document.TypeDetection");
                                        
        if (oInterface == null) {
            fail("Service wasn't created") ;
        }

        XInterface oObj = (XInterface) oInterface ;
        System.out.println("ImplName: "+utils.getImplName(oObj));
        
        m_xDetection = (XTypeDetection)
                UnoRuntime.queryInterface(XTypeDetection.class, oInterface);
        // create instrace of helper class
        helper = new Helper(xMSF);
    }

    @AfterClass
    public static void tearDownConnection()
        throws InterruptedException, com.sun.star.uno.Exception
    {
        connection.tearDown();
    }

    /**
     * The <code>MediaDescriptor</code> was filled with the URL of a file. The 
     * <code>type</code> of the file is kown and must be returned by 
     * <code>MediaDescriptor</code>
     *
     * Syntax of files.csv:
     * fileAlias;fileURL;fileType
     *
     */
    @Test
    public void checkByURLonly() throws Exception {
        System.out.println("### checkByURLonly() ###");
        ArrayList<ArrayList<String>> CSVData =  helper.getToDoList(Argument.get("files.csv"));

        for (ArrayList<String> toDo : CSVData){
            String fileAlias = toDo.get(0);
            String fileURL  = toDo.get(1);
            String URLfileType = toDo.get(2);
            String StreamfileType = toDo.get(3);

            fileURL =  utils.getFullURL(Helper.ensureEndingFileSep(
                          Argument.get("tdoc")) + fileURL);

            System.out.println("actual '"+ fileAlias + 
                               "' ['" + URLfileType + "']: '" + fileURL);

            checkMediaDescriptorURL(fileAlias, fileURL, URLfileType);
            checkMediaDescriptorXInputStream(fileAlias, fileURL, StreamfileType);
        }
    }

    /** To check the <CODE>TypeDedection</CODE> by URL the <CODE>MediaDescriptor</CODE>
     * was filled at fist with the URL only, at second with <CODE>XInputStream</CODE>
     * only. The <CODE>TypeDetection</CODE> must return the expected value
     * @param fileAlias the alias name of the test file
     * @param fileURL the URL of the test file
     * @param fileType the expected type of the test file
     * @see com.sun.star.document.MediaDescriptor
     */
    private void checkMediaDescriptorURL(
                            String fileAlias, String fileURL, String fileType){
                                
        PropertyValue[] MediaDescriptor = Helper.createMediaDescriptor(
            new String[] {"URL"},
            new Object[] {fileURL}
        );
        System.out.println("check only by URL...");

        String type = m_xDetection.queryTypeByDescriptor(
                            Helper.createInOutPropertyValue(MediaDescriptor), true);

        boolean fileTypeOK = Helper.checkFileType(type, fileType);

        assertTrue("\nURL-test         : " + fileAlias + ":\n\treturned type: '" + type +
                   "'\n\texpected type: '" + fileType + "'",fileTypeOK);
    }

    /** Filles a MediaDescriptor with a <code>XInputStream</code> of the test
     * file given by URL.
     * Then the MediaDescriptor was used as parameter for TypeDetection.
     * The TypeDetection must return expected type
     * @param fileAlias the alias name of the test file
     * @param fileURL the URL of the test file
     * @param fileType the expected type of the test file
     * @see com.sun.star.document.MediaDescriptor
     * @see com.sun.star.io.XInputStream
     */
    private void checkMediaDescriptorXInputStream(
                             String fileAlias, String fileURL, String fileType){
        
        XInputStream xStream = null;
        
        try{
            xStream = helper.getFileStream( fileURL );
        } catch (NotConnectedException e) {
            fail("Could not get XInputStream from file :'" + fileURL + "'");
            return;
        }
            
        PropertyValue[] MediaDescriptor = Helper.createMediaDescriptor(
            new String[] {"InputStream"},
            new Object[] {xStream}
        );
        System.out.println("check only by XInputStream...");

        String type = m_xDetection.queryTypeByDescriptor(
                           Helper.createInOutPropertyValue(MediaDescriptor), true);

        boolean fileTypeOK = Helper.checkFileType(type, fileType);

        assertTrue("\nXInputStream-test: " + fileAlias + ":\n\treturned type: '" + type +
                   "'\n\texpected type: '" + fileType + "'", fileTypeOK);
            
    }

    /**
     * The <code>MediaDescriptor</code> was filled with the URL of a file. The 
     * <code>type</code> of the file is kown and must be returned by 
     * <code>MediaDescriptor</code>
     *
     * Syntax of files.csv:
     * fileAlias;fileURL;fileType
     *
     */
    @Test
    public void checkPreselectedType() throws Exception {
        System.out.println("### checkPreselectedType() ###");

        ArrayList<ArrayList<String>> CSVData =  helper.getToDoList(Argument.get("preselectedType.csv"));

        for (ArrayList<String> toDo : CSVData){
            String fileAlias = toDo.get(0);
            String fileURL  = helper.getURLforfileAlias(fileAlias);
            String preselectFileType = toDo.get(1);
            String expectedFileType = toDo.get(2);

            PropertyValue[] MediaDescriptor = Helper.createMediaDescriptor(
                new String[] {"URL", "TypeName"}, // FIXME: "TypeName" was "MediaType", but "MediaType" doesn't work.
                new Object[] {fileURL, preselectFileType});
            System.out.println("check '" + fileAlias + "' with TypeName: '" +
                               preselectFileType + "'");

            String type = m_xDetection.queryTypeByDescriptor(
                           Helper.createInOutPropertyValue(MediaDescriptor), true);

            boolean fileTypeOK = Helper.checkFileType(type, expectedFileType);

            assertTrue("\n" + fileAlias + ":\n\treturned type: '" + type +
                       "'\n\texpected type: '" + expectedFileType + "'",
                       fileTypeOK);
        }
    }


    /**
     * Check loading from a stream. The source for the stream is the
     * first fileAlias that matches "*.txt" in the file list
     * of the given directory.
     */
    @Test
    public void checkPreselectedFilter() throws Exception {
        System.out.println("### checkPreselectedFilter() ###");

        ArrayList<ArrayList<String>> CSVData =  helper.getToDoList(Argument.get("preselectedFilter.csv"));

        for (ArrayList<String> toDo : CSVData){
            String fileAlias = toDo.get(0);
            String fileURL  = helper.getURLforfileAlias(fileAlias);
            String filterName = toDo.get(1);
            String filterOptions = toDo.get(2);
            String filterData = toDo.get(3);
            String expectedType = toDo.get(4);

            PropertyValue[] MediaDescriptor = Helper.createMediaDescriptor(
                new String[] {"URL","FilterName", "FilterOptions","FilterData"},
                new Object[] {fileURL, filterName, filterOptions, filterData});

            System.out.println("check '" + fileAlias + "' with filter: '" + 
                               filterName + "'");

            String type = m_xDetection.queryTypeByDescriptor(
                       Helper.createInOutPropertyValue(MediaDescriptor), true);

            boolean fileTypeOK = Helper.checkFileType(type, expectedType);

            assertTrue("\n" + fileAlias + ":\n\treturned type: '" + type +
                       "'\n\texpected type: '" + expectedType + "'",
                       fileTypeOK);
        }
    }

    /**
     * Check URL encoding. The first fileAlias that matches "*.sxw"
     * is used as source for several encodings.
     */
     @Test
     public void checkPreselectedDocService() throws Exception {
         System.out.println("### checkPreselectedDocService() ###");

         ArrayList<ArrayList<String>> CSVData =  helper.getToDoList(Argument.get("serviceName.csv"));

         for (ArrayList<String> toDo : CSVData){
             String fileAlias = toDo.get(0);
             String fileURL  = helper.getURLforfileAlias(fileAlias);
             String serviceName = toDo.get(1);
             String fileType = helper.getTypeforfileAlias(fileAlias);

             PropertyValue[] MediaDescriptor = Helper.createMediaDescriptor(
                 new String[] {"URL", "DocumentSerivce"},
                 new Object[] {fileURL, serviceName});
             System.out.println("check " + fileAlias);

             String type = m_xDetection.queryTypeByDescriptor(
                            Helper.createInOutPropertyValue(MediaDescriptor), true);

             boolean fileTypeOK = Helper.checkFileType(type, fileType);

             assertTrue("\n" + fileAlias + ":\n\treturned type: '" + type +
                        "'\t\nexpected type: '" + fileType + "'",
                        fileTypeOK);
         }
     }

     @Test
     public void checkStreamLoader(){
         /*
          *as files that used typeDetection.props and one of the csv files.
          *These can simply be set to other permissions using dmake
          *
          */
         System.out.println("### checkStreamLoader() ###");
         String[] urls = new String[2];

         urls[0] = Argument.get("properties");
         urls[1] = Argument.get("files.csv");

         for (int j=0; j<urls.length; j++){
             String fileURL  = urls[j];
             File file = new File(fileURL);
             fileURL =  utils.getFullURL(fileURL);

             PropertyValue[] MediaDescriptor = Helper.createMediaDescriptor(
                 new String[] {"URL"},
                 new Object[] {fileURL});

             if (file.canWrite()) System.out.println("check writable file...");
             else System.out.println("check readonly file...");

             PropertyValue[][] inOut = Helper.createInOutPropertyValue(MediaDescriptor);
             PropertyValue[] in = inOut[0];
             System.out.println("in-Parameter:");
             for (int i=0; i < in.length; i++){
                 System.out.println("["+i+"] '" + in[i].Name + "':'" + in[i].Value.toString()+"'");
             }

             String type = m_xDetection.queryTypeByDescriptor(inOut, true);

             PropertyValue[] out = inOut[0];

             boolean bStream = false;
             System.out.println("out-Parameter");
             boolean bReadOnly = false;
             for (int i=0; i < out.length; i++){
                 if ((out[i].Name.equals("ReadOnly")) && (out[i].Value.toString().equals("true"))) bReadOnly = true;
                 System.out.println("["+i+"] '" + out[i].Name + "':'" + out[i].Value.toString()+"'");
             }

             if (file.canWrite() && bReadOnly)
                 assertTrue("\nStreamLoader: file '"+ fileURL +"' is writable but out-Parameter does contain 'ReadOnly' property",false);
             else if ((!file.canWrite()) && (!bReadOnly))
                 assertTrue("\nStreamLoader: file '"+ fileURL +"'is readonly but out-Parameter does not contain 'ReadOnly' property",false);
             else assertTrue("all ok",true);
         }
     }
}
