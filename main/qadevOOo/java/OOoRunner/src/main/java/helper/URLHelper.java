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



package helper;

// __________ Imports __________

// exceptions
import java.net.MalformedURLException;

// interfaces
import com.sun.star.util.XURLTransformer;

// others
import java.io.File;
import java.util.Vector;
import java.util.Enumeration;


/**
 * It collects some static helper functons to handle URLs.
 * Sometimes it's necessary to convert URL from/to system paths.
 * Or from string to strutural notations (e.g. com.sun.star.util.URL).
 * And sometimes java had another notation then the office it has.
 *
 */
public class URLHelper
{
    // ____________________

    /**
     * Because the office need URLs for loading/saving documents
     * we must convert used system paths.
     * And java use another notation for file URLs ... correct it.
     *
     * @param aSystemPath
     *          represent the file in system notation
     *
     * @return [String]
     *          a file url which represent the given system path
     */
    public static String getFileURLFromSystemPath( File aSystemPath )
    {
        String sFileURL = null;
        try
        {
            //sFileURL = aSystemPath.toURI().toURL().toString();
            sFileURL = aSystemPath.toURL().toString();
        }
        catch( MalformedURLException exWrong )
        {
            sFileURL = null;
        }

        // problem of java: file URL's are coded with 1 slash instead of 2 or 3 ones!
        // => correct this problem first, otherwise office can't use these URL's
        if(
            (sFileURL                       != null ) &&
            (sFileURL.startsWith("file:/")  == true ) &&
            (sFileURL.startsWith("file://") == false)
          )
        {
            StringBuffer sWorkBuffer = new StringBuffer(sFileURL);
            sWorkBuffer.insert(6,"//");
            sFileURL = sWorkBuffer.toString();
        }

        return sFileURL;
    }

    // ____________________

    /**
     * The same as getFileURLFromSystemPath() before but uses string parameter instead
     * of a File type. It exists to suppress converting of necessary parameters in the
     * outside code. But of course getFileURLFromSystemPath(File) will be a little bit faster
     * then this method ...
     *
     * @param sSystemPath
     *          represent the file in system notation
     *
     * @return [String]
     *          a file url which represent the given system path
     */
    public static String getFileURLFromSystemPath( String sSystemPath )
    {
        return getFileURLFromSystemPath(new File(sSystemPath));
    }

    // ____________________

    /**
     * Does the same as getFileURLFromSystemPath() before ... but uses
     * the given protocol string (e.g."http://") instead of "file:///".
     *
     * @param aSystemPath
     *          represent the file in system notation
     *
     * @param aBasePath
     *          define the base path of the aSystemPath value,
     *          which must be replaced with the value of "sServerPath".
     *
     * @param sServerURL
     *          Will be used to replace sBasePath.
     *
     * @example
     *          System Path = "d:\test\file.txt"
     *          Base Path   = "d:\test"
     *          Server Path = "http://alaska:8000"
     *          => "http://alaska:8000/file.txt"
     *
     * @return [String]
     *          an url which represent the given system path
     *          and uses the given protocol
     */
    public static String getURLWithProtocolFromSystemPath( File aSystemPath, File aBasePath, String sServerURL )
    {
        String sFileURL = URLHelper.getFileURLFromSystemPath(aSystemPath);
        String sBaseURL = URLHelper.getFileURLFromSystemPath(aBasePath  );

        // cut last '/'!
        if (sBaseURL.lastIndexOf('/')==(sBaseURL.length()-1))
            sBaseURL = sBaseURL.substring(0,sBaseURL.length()-1);

        // cut last '/'!
        if (sServerURL.lastIndexOf('/')==(sServerURL.length()-1))
            sServerURL = sServerURL.substring(0,sServerURL.length()-1);
            
        int index = sFileURL.indexOf(sBaseURL);
        String sURL = sFileURL.substring(0,index) + sServerURL + 
                                sFileURL.substring(index+sBaseURL.length());
        //String sURL = sFileURL.replaceFirst(sBaseURL,sServerURL);
        return sURL;
    }

    // ____________________

    /**
     * The same as getURLWithProtocolFromSystemPath() before but uses string parameter instead
     * of a File types. It exists to suppress converting of necessary parameters in the
     * outside code. But of course getURLWithProtocolFromSystemPath(File,File,String) will be
     * a little bit faster then this method ...
     *
     * @param sSystemPath
     *          represent the file in system notation
     *
     * @param sBasePath
     *          define the base path of the aSystemPath value,
     *          which must be replaced with the value of "sServerPath".
     *
     * @param sServerPath
     *          Will be used to replace sBasePath.
     *
     * @example
     *          System Path = "d:\test\file.txt"
     *          Base Path   = "d:\test"
     *          Server Path = "http://alaska:8000"
     *          => "http://alaska:8000/file.txt"
     *
     * @return [String]
     *          an url which represent the given system path
     *          and uses the given protocol
     */
    public static String getURLWithProtocolFromSystemPath( String sSystemPath, String sBasePath, String sServerPath )
    {
        return getURLWithProtocolFromSystemPath(new File(sSystemPath), new File(sBasePath), sServerPath);
    }

    // ____________________

    /**
     * This convert an URL (formatted as a string) to a struct com.sun.star.util.URL.
     * It use a special service to do that: the URLTransformer.
     * Because some API calls need it and it's not allowed to set "Complete"
     * part of the util struct only. The URL must be parsed.
     *
     * @param sURL
     *          URL for parsing in string notation
     *
     * @return [com.sun.star.util.URL]
     *              URL in UNO struct notation
     */
    public static com.sun.star.util.URL parseURL(XURLTransformer xParser, String sURL)
    {
        com.sun.star.util.URL aURL = null;

        if (sURL==null || sURL.equals(""))
            return null;

        try
        {
            // Create special service for parsing of given URL.
/*            com.sun.star.util.XURLTransformer xParser = (com.sun.star.util.XURLTransformer)OfficeConnect.createRemoteInstance(
                                                            com.sun.star.util.XURLTransformer.class,
                                                            "com.sun.star.util.URLTransformer");
*/
            // Because it's an in/out parameter we must use an array of URL objects.
            com.sun.star.util.URL[] aParseURL = new com.sun.star.util.URL[1];
            aParseURL[0]          = new com.sun.star.util.URL();
            aParseURL[0].Complete = sURL;

            // Parse the URL
            xParser.parseStrict(aParseURL);

            aURL = aParseURL[0];
        }
        catch(com.sun.star.uno.RuntimeException exRuntime)
        {
            // Any UNO method of this scope can throw this exception.
            // Reset the return value only.
            aURL = null;
        }

        return aURL;
    }

    //_________________________________
    /**
     * Return a name list of all available files of a directory.
     * We filter pure sub directories names. All other files
     * are returned as full qualified URL strings. So they can be
     * used for further purposes. One parameter define the start directory,
     * another one describe the url protocol, which the return URL names should have.
     *
     * @param   sStartDir
     *              the start directory, which should include all test files
     *
     * @return  [Vector]
     *              a filtered list of java File objects of all available files of the start dir
     *              and all accessible subdirectories.
     */
    public static Vector getSystemFilesFromDir(String sStartDir)
    {
        File aRoot = new File(sStartDir);

        if (! aRoot.exists())
            return null;

        if (! aRoot.isDirectory())
            return null;

        File[] lAllFiles = aRoot.listFiles();
        if (lAllFiles == null )
            return null;

        Vector lFilteredFiles = new Vector(lAllFiles.length);

        for (int i=0; i<lAllFiles.length; ++i)
        {
            if (lAllFiles[i].isFile())
                lFilteredFiles.add(lAllFiles[i]);
            else
            if (lAllFiles[i].isDirectory())
            {
                // recursion!
                Vector lSubFiles = URLHelper.getSystemFilesFromDir(lAllFiles[i].getPath());
                if (lSubFiles != null)
                {
                    Enumeration aSnapshot = lSubFiles.elements();
                    while (aSnapshot.hasMoreElements())
                        lFilteredFiles.add(aSnapshot.nextElement());
                }
            }
        }

        return lFilteredFiles;
    }
}
