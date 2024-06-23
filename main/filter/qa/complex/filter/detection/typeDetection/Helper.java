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
import com.sun.star.io.NotConnectedException;
import com.sun.star.io.XInputStream;
import com.sun.star.lang.XMultiServiceFactory;

import helper.StreamSimulator;
import java.io.*;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.StringTokenizer;
import java.util.Properties;
import lib.TestParameters;
import share.LogWriter;
import util.utils;

import org.openoffice.test.Argument;

/** Helper class for "TypeDetection"
 * This class do file hanlding.
 */
public class Helper {

    /** The runner log writer
     * @member m_sTestDocPath   directory for searching files to load
     * @member m_vFiles         list of all files described in "files.csv"
     * @member m_hFileURLs      contains the position of a file name in the m_vFiles ArrayList
     * @member m_hFileTypes     contains the position of a file type in the m_vFiles ArrayList
     */

    String m_sTestDocPath = null;

    ArrayList<ArrayList<String>> m_vFiles = null;

    Hashtable m_hFileURLs = new Hashtable();

    Hashtable m_hFileTypes = new Hashtable();

    XMultiServiceFactory m_xMSF;

    /**
     * construct a new instance of this class
     * It creates the "todo" list and position lists for <code>URL</code> and
     * and <code>Type</code> inside the "todo" list
     *
     * @param   param the test parameters
     *
     * @param   log the log writer
     */

    public Helper(XMultiServiceFactory xMSF) throws IOException {

        m_xMSF = xMSF;

        // get all files from the given directory
        m_sTestDocPath = Argument.get("tdoc");

        // get all files from "files.csv"
        m_vFiles = getToDoList(Argument.get("files.csv"));

        createFilesList();
    }


     /** Reads a comma separated file (CSV). Every line of the file is
      * represented by an <code>ArrayList</code> entry. Every data entry of a row is
      * also stored in a <code>ArrayList</code>. So the returned value is a
      * <code>ArrayList<ArrayList<>></code> where the first dimension represents a row
      * and the second dimension includes the data values.
      * @param csvFileName the name of the csv file
      * @return ArrayList filled with ArrayList filled with data of a row
      */
     public ArrayList<ArrayList<String>> getToDoList(String csvFileName) throws IOException {

         ArrayList<ArrayList<String>> vAll = new ArrayList<>();
         ArrayList<String> vFields = new ArrayList<>();

         // get content of file
         ArrayList<String> content = getCSVFileContent(csvFileName);

         // remove superfluous content like "#" started lines
         content = removeSuperfluousContent(content);

         // replace all place holders in file
         content = replacePlaceHolder(content);

         // the first line contains field names of the columns
         // split line by ";"
         StringTokenizer fields = new StringTokenizer(
             content.get(0), ";");
         int fieldCount = 0;
         while (fields.hasMoreElements()){
             vFields.add((String)fields.nextElement());
             fieldCount++;
         }

         // fill vData with data of CSV-row
         for (int row = 1; row < content.size(); row++){
             ArrayList<String> vData = new ArrayList<>();
             String[] tokens = content.get(row).split(";");
             for (String token : tokens) {
                 vData.add(token);
             }
             for (int i = tokens.length; i < fieldCount; i++)
                  vData.add("");
             vAll.add(vData);
         }

         return vAll;
     }

     /** The csv files "files", "preselectedFilter", "preselectedType" and
      * "serviceName" are read with this class. This function seeks for
      * the csv files and read them.
      * @param csvFileName the name of the csv file
      * @return an ArrayList<String> containing the content of the file.
      */
    private static ArrayList<String> getCSVFileContent(String csvFileName) throws IOException {
        ArrayList<String> content = new ArrayList<>();
        if ( Boolean.parseBoolean(Argument.get("DEBUG_IS_ACTIVE")) ) {
            System.out.println("Looking for "+csvFileName);
        }
        String line;
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(csvFileName), "UTF-8"))) {
            while ( ( line = reader.readLine() ) != null ) {
                content.add( line );
            }
            return content;
        }
    }

    /** returns a XInputStream of given file
     * @param filePath the path to the file which should be loaded
     * @return the XInputStream, <null/> if the
     * file cannot be read
     * @throws NotConnectedException was thrown if it was not possible to open <CODE>filePath</CODE>
     */
    public XInputStream getFileStream( String filePath )
                                                throws NotConnectedException {
        return new StreamSimulator(filePath, true, m_xMSF);
    }

    /** replaces place holder in preselectedFilter.
     * Because of filter names depend on StarOffice version like
     * "StarOffice 6.0 Textdokument" or ""StarSuite 7 Textdokument"
     * The filter names must be changed. The place holder will be replaced
     * by an equivalent in "typeDetection.props"
     * @param content the content of a csv file
     * @return changed file content
     */
    private static ArrayList<String> replacePlaceHolder(ArrayList<String> content) throws IOException {

        ArrayList<String> vReturn = new ArrayList<>();

        ArrayList<String> placeHolders = new ArrayList<>();
        Properties properties = new Properties();
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(Argument.get("properties")), "UTF-8"))) {
            properties.load(reader);
        }
        Enumeration keys = properties.keys();
        String placeHolder = properties.getProperty("placeHolder");

        // get all placeholders from typeDetection.csv
        while (keys.hasMoreElements()){
            String holderKey = (String) keys.nextElement();
            if (holderKey.startsWith(placeHolder)){
                placeHolders.add(holderKey);
            }
        }

        // replace all occurrences of placeholders in 'CSVData'
        for( String line : content ) {

            String newLine = line;

            for( String holder : placeHolders ) {

                int startPos = line.indexOf(holder);

                if (startPos > -1){
                    try{
                        String holderValue = properties.getProperty(holder);

                        newLine = newLine.substring(0,startPos) + holderValue +
                                newLine.substring(startPos + holder.length());

                    } catch (java.lang.IndexOutOfBoundsException e){
                        System.out.println("ERROR: problems while creating placeholder" +
                                    " replaced list: "+ e);
                    }
                }
           }
           vReturn.add(newLine);
        }
        return vReturn;
    }

    /** Removes lines of an ascii file content which starts with "#"
     * or are empty
     * @param content content of a csv file
     * @return a stripped ArrayList<String>
     */
    private static ArrayList<String> removeSuperfluousContent(ArrayList<String> content){
        ArrayList<String> newContent = new ArrayList<>();
        for( String line : content ) {
            if (( ! line.startsWith( "#" ))&& ( line.length() != 0 )) {
                newContent.add( line );
            }
        }
        return newContent;
    }

    /** returns a <code>MediaDescriptor</code> filled with given properties and
     * values.
     * @param propNames String Array of property names
     * @param values Object Array of property values
     * @return <code>PropertyValue[]<code>
     * @see com.sun.star.beans.PropertyValue
     * @see com.sun.star.document.MediaDescriptor
     */
    public static PropertyValue[] createMediaDescriptor(String[] propNames, Object[] values) {
        PropertyValue[] props = new PropertyValue[propNames.length] ;

        for (int i = 0; i < props.length; i++) {
            props[i] = new PropertyValue() ;
            props[i].Name = propNames[i] ;
            if (values != null && i < values.length) {
                props[i].Value = values[i] ;
            }
        }

        return props ;
    }

    /** Appends system file separator if needed
     * @param s the system path
     * @return system path with ending system file separator
     */
    public static String ensureEndingFileSep(String s){
        if(s != null && !s.equals("") && !s.endsWith(File.separator)){
            s = s.trim() + File.separator;
        }else if(s == null)
            s = "";
        return s;
    }

    /** Returns the file URL for the given file name assembled by
     * "TestDocumentPath" of typeDetection.props and "fileURL" of files.csv
     * @param fileAlias the alias name of the file
     * @return file URL
     * @throws FileAliasNotFoundException was thrown if alias does not exist
     */
    public String getURLforfileAlias(String fileAlias)
                                        throws FileAliasNotFoundException{
        try{
            String fileURL = (String) m_hFileURLs.get(fileAlias).toString();
            return utils.getFullURL(ensureEndingFileSep(m_sTestDocPath) + fileURL);
        } catch (java.lang.NullPointerException e){
            throw new FileAliasNotFoundException(fileAlias);
        }
    }

    /** Returns the file type for the given file name containing in files.csv
     * @param fileAlias the alias name of the file
     * @return file type
     * @throws FileAliasNotFoundException was thrown if not alias was thrown
     */
    public String getTypeforfileAlias(String fileAlias)
                                        throws FileAliasNotFoundException{
        try{
            return (String) m_hFileTypes.get(fileAlias).toString();
        } catch (java.lang.NullPointerException e){
            throw new FileAliasNotFoundException(fileAlias);
       }
    }

    /**
     *  Fills the Hashtable m_hFileURLs with all file names and their URL
     *  and the Hashtable m_hFilesTypes with all file names and their file
     *  type name. This informations are extracted from "files.csv"
     *  This is for faster access to get fileURL and fileType of fileAlias
     */
    private void createFilesList(){
        for (int i = 0; i < m_vFiles.size();i++){
            ArrayList<String> toDo = m_vFiles.get(i);
            m_hFileURLs.put(toDo.get(0), toDo.get(1));
            m_hFileTypes.put(toDo.get(0), toDo.get(2));
        }
    }


    /**  Validate the returned file type for the file alias with the
     *  possible file types
     * @param currentFileType the returned file type
     * @param fileTypes all possible file types
     * @return true if valid
     */
    public static boolean checkFileType(String currentFileType, String fileTypes){

        StringTokenizer data = new StringTokenizer(fileTypes,":", true);

        boolean found = false;
        while (data.hasMoreElements()) {

            String actualFileType = data.nextElement().toString();

            found = found || currentFileType.equals(actualFileType);
        }
        return found;
    }

    /** creates an input/output parameter of <code>PropertyValue[]<code>.
     * @return PropertyValue[][]
     * @param PropVal a PropertyValue
     */
    public static PropertyValue[][] createInOutPropertyValue(PropertyValue[] PropVal){
        PropertyValue[][] dummy = new PropertyValue[1][];
        dummy[0] = PropVal;
        return dummy;
    }
}

/** This exception should be thrown if a method seeks for an invalid alias name */
class FileAliasNotFoundException extends java.lang.Exception{
    /** throws error message with wrong alias name
     * @param fileAlias the alias name
     */
    public FileAliasNotFoundException(String fileAlias){
        super("Could not get '"+fileAlias +"'");
    }
}

