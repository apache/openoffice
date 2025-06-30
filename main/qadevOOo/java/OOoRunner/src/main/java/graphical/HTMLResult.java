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



package graphical;

import java.io.File;
import java.io.FileWriter;
// import util.utils;
// import helper.OSHelper;

public class HTMLResult
{
    private FileWriter m_aOut;
    // private String m_sFilename;
    // private String m_sNamePrefix;              // the HTML files used a suffix to build it's right name
    
    /**
     * ls is the current line separator (carriage return)
     */
    private String ls;

    public HTMLResult( String _sOutputPath, String _sHTMLFilename )
        {
            FileHelper.makeDirectories("", _sOutputPath);
            // HTMLResult a = new HTMLResult();
            String sFilename = FileHelper.appendPath(_sOutputPath, _sHTMLFilename);

            try
            {
                File outputFile = new File(sFilename);
                m_aOut = new FileWriter(outputFile.toString());
                ls = System.getProperty("line.separator");
            }
            catch (java.io.IOException e)
            {
                e.printStackTrace();
                GlobalLogWriter.println("ERROR: Can't create HTML Outputter");
                // return null;
            }
            // m_sFilename = sFilename;
            // a.m_sNamePrefix = _sNamePrefix;
            // return a;
        }
    
    // public String getFilename() {return m_sFilename;}
    
    private void writeln(String _sStr)
    {
            try
            {
                m_aOut.write( _sStr );
                m_aOut.write ( ls );
            }
            catch (java.io.IOException e)
            {
            }
    }
    private void flush()
    {
        try
        {
            m_aOut.flush();
        }
        catch (java.io.IOException e)
        {
        }
    }

    
    /**
     * create the HTML header
     * @param _sTitle
     */
    public void header(String _sTitle)
        {
                writeln( "<html>");
                writeln( "<head>" );
                writeln( "<title>" + _sTitle + "</title>");
                writeln( "<link rel=\"stylesheet\" type=\"text/css\" href=\"/gfxcmp_ui/xmloff.css\" media=\"screen\" />");
                writeln( "<link rel=\"stylesheet\" type=\"text/css\" href=\"/gfxcmp_ui/style.css\" media=\"screen\" />");
                writeln( "</head>");
                writeln( "<body bgcolor=white>");
                flush();            
        }
    
    final static String TEST_TABLETITLE = "Document";
    final static String VISUAL_STATUS_TABLETITLE = "Visual status";
    final static String VISUAL_STATUS_MESSAGE_TABLETITLE = "Message";
    final static String FIRSTGFX_TABLETITLE = "Original print file as jpeg";
    
    public void indexSection(String _sOfficeInfo)
        {
                writeln( "<h2>Results for " + _sOfficeInfo + "</h2>");
                writeln( "<p>This result was created at: " + DateHelper.getDateTimeForHumanreadableLog());
                writeln( "<p>Legend:<br>");
                writeln( stronghtml(FIRSTGFX_TABLETITLE) + " contains the output printed via 'ghostscript' as a jpeg picture.<br>");

                writeln( "<table class=\"infotable\">");
                writeln( "<tr>");
                writeln( tableHeaderCell(TEST_TABLETITLE));
                writeln( tableHeaderCell(""));
                writeln( tableHeaderCell(VISUAL_STATUS_TABLETITLE));
                writeln( tableHeaderCell(VISUAL_STATUS_MESSAGE_TABLETITLE));
                writeln( "</tr>");
                flush();
        }
/**
 * Returns the given _sHREF & _sPathInfo as a HTML String
 * <A HREF="_sHREF">_sPathInfo</A>
 * @param _sHREF
 * @param _sPathInfo
 * @return
 */
    private String getHREF(String _sHREF, String _sPathInfo)
        {
            StringBuffer a = new StringBuffer();
            a.append("<a href=\"");
            a.append(_sHREF);
            a.append("\">");
            a.append(_sPathInfo);
            a.append("</a>");
            return a.toString();
        }

    /**
     * Returns the given _sValue as a HTML Table cell with _sValue as content
     * @param _sValue
     * @return
     */
    private String tableDataCell(String _sValue)
        {
            StringBuffer a = new StringBuffer();
            a.append("<td>");
            a.append(_sValue);
            a.append("</td>");
            return a.toString();
        }
    
    /**
     * Returns the given _sValue as a HTML Table header cell with _sValue as content
     * @param _sValue
     * @return
     */
    private String tableHeaderCell(String _sValue)
        {
            StringBuffer a = new StringBuffer();
            a.append("<th>");
            a.append(_sValue);
            a.append("</th>");
            return a.toString();
        }
    
    public void indexLine(String _sHTMLFile, String _sHTMLName, String _sStatusRunThrough, String _sStatusMessage)
        {
                writeln( "<tr>");
                writeln(tableDataCell( getHREF(_sHTMLFile, _sHTMLName) ) );
                writeln(tableDataCell( "" ) );                
                writeln( tableDataCell(_sStatusRunThrough) );
                writeln( tableDataCell(_sStatusMessage) );
                writeln( "</tr>");
                flush();
        }
    
    public void close()
        {
            writeln( "</table>");
            writeln( "</body></html>");
            try
            {
                m_aOut.close();
            }
            catch (java.io.IOException e)
            {
            }
        }
    
// -----------------------------------------------------------------------------
    private String stronghtml(String _sValue)
        {
            StringBuffer a = new StringBuffer();
            a.append("<strong>");
            a.append(_sValue);
            a.append("</strong>");
            return a.toString();
        }
    
}
