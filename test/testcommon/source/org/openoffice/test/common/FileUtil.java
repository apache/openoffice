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



package org.openoffice.test.common;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URL;
import java.net.URLConnection;
import java.text.DateFormat;
import java.text.MessageFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Properties;
import java.util.logging.Level;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import org.w3c.dom.Document;


/**
 * Utilities related to the file system
 *
 */
public class FileUtil {
	 
	private final static DateFormat FILENAME_FORMAT = new SimpleDateFormat("yyMMddHHmm");
	
	private final static Logger log = Logger.getLogger(FileUtil.class);
	
	private FileUtil(){
		
	}
	
	/**
	 * Parse XML file to Document model
	 * @param path
	 * @return
	 */
	public static Document parseXML(String path) {
		try {
			DocumentBuilderFactory dbfac = DocumentBuilderFactory.newInstance();
			dbfac.setNamespaceAware(true);
			DocumentBuilder docBuilder = dbfac.newDocumentBuilder();
			return docBuilder.parse(path);
		} catch (Exception e) {
			return null;
		}
	}
	
	/**
	 * Create a new xml document
	 * @return
	 */
	public static Document newXML() {
		try {
			DocumentBuilderFactory dbfac = DocumentBuilderFactory.newInstance();
			dbfac.setNamespaceAware(true);
			DocumentBuilder docBuilder = dbfac.newDocumentBuilder();
			return docBuilder.newDocument();
		} catch (Exception e) {
			return null;
		}
	}
	
	public static boolean storeXML(Document doc, File file) {
		try {
			file.getParentFile().mkdirs();
			TransformerFactory.newInstance().newTransformer().transform(new DOMSource(doc), new StreamResult(file));
			return true;
		} catch (Exception e) {
			return false;
		}
	}
	
	public static boolean storeXML(Document doc, File file, File xls) {
		try {
			file.getParentFile().mkdirs();
			TransformerFactory.newInstance().newTransformer(new StreamSource(xls)).transform(new DOMSource(doc), new StreamResult(file));
			return true;
		} catch (Exception e) {
			return false;
		}
	}
	
	public static boolean storeXML(Document doc, File file, InputStream xls) {
		try {
			file.getParentFile().mkdirs();
			TransformerFactory.newInstance().newTransformer(new StreamSource(xls)).transform(new DOMSource(doc), new StreamResult(file));
			return true;
		} catch (Exception e) {
			return false;
		}
	}
	  
	/**
	 * Get a string by XPATH from a xml file
	 * @param xml
	 * @param xpathStr
	 * @return
	 */
	public static String getStringByXPath(String xml, String xpathStr) {
		Document doc = parseXML(xml);
		if (doc == null)
			return null;
	
	    try {
	    	XPathFactory factory = XPathFactory.newInstance();
			XPath xpath = factory.newXPath();
			XPathExpression expr = xpath.compile(xpathStr);
			return (String) expr.evaluate(doc);
		} catch (XPathExpressionException e) {
			e.printStackTrace();
			return null;
		}


	}

	
	
	/**
	 * Update the given property in a properties file
	 * @param file the properties file path
	 * @param key the key
	 * @param value the value
	 */
	public static void updateProperty(String file, String key, String value) {
		Properties map = new Properties();
		map.put(key, value);
		updateProperty(file, map);
	}
	
	/**
	 * Update the given properties in a properties file
	 * @param file the properties file path
	 * @param props properties updated
	 */
	public static void updateProperty(String file, Properties props) {
		Properties properties = loadProperties(file);
		properties.putAll(props);
		storeProperties(file, properties);
	}
	
	/**
	 * Load a properties file to Properties class
	 * @param file the properties file path
	 * @return
	 */
	public static Properties loadProperties(String file) {
		return loadProperties(new File(file));
	}
	
	/**
	 * Load a properties file to Properties class
	 * @param file the properties file
	 * @return
	 */
	public static Properties loadProperties(File file) {
		Properties properties = new Properties();
		FileInputStream fis = null;
		try {
			fis = new FileInputStream(file);
			properties.load(fis);
		} catch (IOException e) {
			//System.out.println("Can't read properties file.");
		} finally {
			if (fis != null) {
				try {
					fis.close();
				} catch (IOException e) {
					// ignore
				}
			}
		}
		
		return properties;
	}
	
	/**
	 * Store properties into a file
	 * @param file the properties file path
	 * @param properties the properties to be stored
	 */
	public static void storeProperties(String file, Properties properties) {
		FileOutputStream fos = null;
		try {
			fos = new FileOutputStream(file);
			properties.store(fos, "Generated By PropertyFileUtils");
		} catch (IOException e) {
			throw new RuntimeException(e);
		} finally {
			if (fos != null) {
				try {
					fos.close();
				} catch (IOException e) {
					// ignore
				}
			}
		}
	}
	
	
	
	/**
	 * Delete a property in a properties file
	 * @param file the properties file path
	 * @param key the key to be deleted
	 */
	public static void deleteProperty(String file, String key) {
		Properties properties = loadProperties(file);
		properties.remove(key);
		storeProperties(file, properties);
	}

	/**
	 * Load a file as string
	 * @param file the file path
	 * @return 
	 */
	public static String readFileAsString(String file) {
		return readFileAsString(new File(file));
	}
	
	/**
	 * Load a file as string
	 * @param file the file path
	 * @return 
	 */
	public static String readFileAsString(File file) {
		try {
			return readStreamAsString(new FileInputStream(file), null);
		} catch (FileNotFoundException e) {
			return "";
		}
	}
	
	public static String readStreamAsString(InputStream inputStream, String charsetName) {
		StringBuffer strBuffer = new StringBuffer(10240);
		BufferedReader reader = null;
		try {
			reader = new BufferedReader(charsetName == null ? new InputStreamReader(inputStream) : new InputStreamReader(inputStream, charsetName));
			char[] buf = new char[1024];
			int count = 0;
			while ((count = reader.read(buf)) != -1) {
				strBuffer.append(buf, 0, count);
			}
		} catch (IOException e) {
			
		} finally {
			if (reader != null)
				try {
					reader.close();
				} catch (IOException e) {
					// ignore
				}
		}

		return strBuffer.toString();
	}
	

	public static boolean isSymbolicLink(File file) {
		try {
			File parent = file.getParentFile();
			String name = file.getName() ;
			File toTest = parent != null ? new File(parent.getCanonicalPath(), name) : new File(name);
			return !toTest.getAbsolutePath().equals(toTest.getCanonicalPath());
		} catch (Exception e) {
			return false;
		}
	}

	/**
	 * Find the first file matching the given name.
	 * @param dir The directory to search in
	 * @param name Regular Expression to match the file name
	 * @return
	 */
	public static File findFile(File dir, String name) {
		if (!dir.isDirectory())
			return null;
		File[] files = dir.listFiles();
		for (int i = 0; i < files.length; i++) {
			if (files[i].isDirectory()) {
				File ret = findFile(files[i], name);
				if (ret != null)
					return ret;
			} else if (files[i].getName().matches(name)) {
				return files[i];
			}
		}
		
		return null;
	}
	
	public static File findFile(File dir, String name, boolean followSymbolicLink) {
		if (!dir.isDirectory())
			return null;
		File[] files = dir.listFiles();
		for (int i = 0; i < files.length; i++) {
			if (files[i].isDirectory() && (followSymbolicLink || !isSymbolicLink(files[i]))) {
				File ret = findFile(files[i], name);
				if (ret != null)
					return ret;
			} else if (files[i].getName().matches(name)) {
				return files[i];
			}
		}
		
		return null;
	}
	
	/**
	 * Find the last file matching the given name.
	 * @param dir The directory to search in
	 * @param name Regular Expression to match the file name
	 * @return
	 */
	public static File findLastFile(File dir, String name) {
		if (!dir.isDirectory())
			return null;
		File[] files = dir.listFiles();
		File file = null;
		for (int i = 0; i < files.length; i++) {
			if (files[i].isDirectory()) {
				File ret = findFile(files[i], name);
				if (ret != null)
					file = ret;
			} else if (files[i].getName().matches(name)) {
				file = files[i];
			}
		}
		
		return file;
	}
	
	/**
	 * find the first file matching the given name.
	 * @param dirs The directories to search in. Use ';' separate each directory.
	 * @param name Regular Expression to match the file name
	 * @return
	 */
	public static File findFile(String dirs, String name) {
		String[] directories = dirs.split(";");
		for (String s : directories) {
			File dir = new File(s);
			if (!dir.exists())
				continue;
			File file = findFile(dir, name);
			if (file != null)
				return file;
		}
		
		return null;
	}
	
	
	/**
	 * find the last file matching the given name.
	 * @param dirs The directories to search in. Use ';' separate each directory.
	 * @param name Regular Expression to match the file name
	 * @return
	 */
	public static File findLastFile(String dirs, String name) {
		String[] directories = dirs.split(";");
		for (String s : directories) {
			File dir = new File(s);
			if (!dir.exists())
				continue;
			File file = findLastFile(dir, name);
			if (file != null)
				return file;
		}
		
		return null;
	}
	
	/**
	 * find the directory matching the given name.
	 * @param dir The directory to search in
	 * @param name Regular Expression to match the file name
	 * @return
	 */
	public static File findDir(String dir, String dirName) {
		File[] files = new File(dir).listFiles();
		for (int i = 0; i < files.length; i++) {
			if (!files[i].isDirectory()) {
				File ret = findFile(files[i], dirName);
				if (ret != null)
					return ret;
			} else if (files[i].getName().matches(dirName)) {
				return files[i];
			}
		}
		
		return null;
	}
	

	/**
	 * Write string into a file
	 * @param path
	 * @param contents
	 */
	public static void writeStringToFile(String path, String contents) {
		writeStringToFile(new File(path), contents);
	}
	
	/**
	 * Write string into a file
	 * @param file
	 * @param contents
	 */
	public static void writeStringToFile(File file, String contents) {
		FileWriter writer = null;
		try {
			file.getParentFile().mkdirs();
			writer = new FileWriter(file);
			if (contents != null)
				writer.write(contents);
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			if (writer != null)
				try {
					writer.close();
				} catch (IOException e) {
				}
		}
	}
	
	/**
	 * Appeand a string to the tail of a file
	 * @param file
	 * @param contents
	 */
	public static void appendStringToFile(String file, String contents) {
		FileWriter writer = null;
		try {
			writer = new FileWriter(file, true);
			writer.write(contents);
		} catch (IOException e) {
			System.out.println("Warning:" + e.getMessage());
		} finally {
			if (writer != null)
				try {
					writer.close();
				} catch (IOException e) {
				}
		}
	}
	
	/**
	 * Replace string in the file use regular expression
	 * @param file
	 * @param expr
	 * @param substitute
	 */
	public static void replace(String file, String expr, String substitute) {
		String str = readFileAsString(file);
		str = str.replaceAll(expr, substitute);
		writeStringToFile(file, str);
	}
	
    /**
     * Recursively copy all files in the source dir into the destination dir
     * @param fromDirName the source dir 
     * @param toDirName the destination dir
     * @return
     */
    public static boolean copyDir(String fromDirName, String toDirName)  {
    	return copyDir(new File(fromDirName), new File(toDirName), true);
    }
    
    /**
     * Copy all files in the source dir into the destination dir
     * @param fromDir
     * @param toDir
     * @param recursive
     * @return
     */
    public static boolean copyDir(File fromDir, File toDir, boolean recursive) {
    	if (!fromDir.exists() || !fromDir.isDirectory()) {
    		System.err.println("The source dir doesn't exist, or isn't dir.");
    		return false;
    	}
    	if (toDir.exists() && !toDir.isDirectory())
    		return false;
    	boolean result = true;
    	toDir.mkdirs();
    	File[] files = fromDir.listFiles();
    	for (int i = 0; i < files.length; i++) {
    		if (files[i].isDirectory() && recursive)
    			result &= copyDir(files[i], new File(toDir, files[i].getName()), true);
    		else
    			result &= copyFile(files[i], toDir);
    	}
    	
    	return result;
    }
    
    /**
     * Copy a file
     * @param fromFile
     * @param toFile
     * @return
     */
    public static boolean copyFile(File fromFile, File toFile) {
         if (toFile.isDirectory())
           toFile = new File(toFile, fromFile.getName());

         FileInputStream from = null;
         FileOutputStream to = null;
         try {
           from = new FileInputStream(fromFile);
           File p = toFile.getParentFile();
           if (p != null && !p.exists())
        	   p.mkdirs();
           to = new FileOutputStream(toFile);
           byte[] buffer = new byte[4096];
           int bytesRead;
           while ((bytesRead = from.read(buffer)) != -1)
             to.write(buffer, 0, bytesRead);
           
           return true;
         } catch (IOException e) {
         	//Can't copy
         	e.printStackTrace();
         	return false;
         } finally {
           if (from != null)
             try {
               from.close();
             } catch (IOException e) {
             }
           if (to != null)
             try {
               to.close();
             } catch (IOException e) {
             }
         }
    }
    

    /**
     * Pump data from an inputstream into a file
     * @param from
     * @param toFile
     * @return
     */
	public static boolean writeToFile(InputStream from, File toFile) {
		FileOutputStream to = null;
		try {
			File p = toFile.getParentFile();
			if (p != null && !p.exists())
				p.mkdirs();
			to = new FileOutputStream(toFile);
			byte[] buffer = new byte[4096];
			int bytesRead;
			while ((bytesRead = from.read(buffer)) != -1)
				to.write(buffer, 0, bytesRead);

			return true;
		} catch (IOException e) {
			// Can't copy
			e.printStackTrace();
			return false;
		} finally {
			if (from != null)
				try {
					from.close();
				} catch (IOException e) {
				}
			if (to != null)
				try {
					to.close();
				} catch (IOException e) {
				}
		}
	}
    
	/**
	 * Pump data from an inputstream into an output stream
	 * @param from
	 * @param to
	 * @return
	 */
	public static boolean pump(InputStream from, OutputStream to) {
		try {
			byte[] buffer = new byte[4096];
			int bytesRead;
			while ((bytesRead = from.read(buffer)) != -1)
				to.write(buffer, 0, bytesRead);

			return true;
		} catch (IOException e) {
			return false;
		} finally {
			if (from != null)
				try {
					from.close();
				} catch (IOException e) {
				}
			if (to != null)
				try {
					to.close();
				} catch (IOException e) {
				}
		}
	}

    /**
     * Copy a file
     * @param fromFileName
     * @param toFileName
     * @return
     */
    public static boolean copyFile(String fromFileName, String toFileName) {
    	return copyFile(new File(fromFileName), new File(toFileName));
    }
    
    /**
     * Copy all the files under fromDirName to toDirName
     * @param fromDirName
     * @param toDirName
     * @return
     */
    public static boolean copyFiles(String fromDirName, String toDirName) {
    	boolean res = true;
    	
    	File fromDir = new File(fromDirName);
    	if (!fromDir.exists() || !fromDir.isDirectory() || !fromDir.canRead()) {
         	System.err.println(fromDir.getAbsolutePath() + "doesn't exist, or isn't file, or can't be read");
         	return false;
         }
    	File [] files = fromDir.listFiles();
    	for(int i=0; i<files.length; i++){
    		if(files[i].isDirectory()){
    			res = res && copyDir(fromDirName + "/" + files[i].getName(), toDirName + "/" + files[i].getName());
    		}
    		else 
    			res = res && copyFile(fromDirName + "/" + files[i].getName(), toDirName + "/" + files[i].getName());
    	}
    	return res;
    }
    
    /**
     * Delete a file or directory
     * @param file
     * @return
     */
    public static boolean deleteFile(File path) {
    	if (!path.exists())
    		return true;
    	
		if (path.isDirectory()) {
			File[] files = path.listFiles();
			for (int i = 0; i < files.length; i++) {
				if (files[i].isDirectory()) {
					deleteFile(files[i]);
				} else {
					files[i].delete();
				}
			}
		}
		
		return path.delete();
	}
    
    /**
     * Delete a file or directory.
     * @param path
     * @return
     */
    public static boolean deleteFile(String path) {
		return deleteFile(new File(path));
	}
    
    /**
     * Check if a file exists
     * @param file
     * @return
     */
    public static boolean fileExists(String file) {
    	if (file == null)
    		return false;
    	return new File(file).exists();
    }
   
    /**
     * Get the extension name of a file
     * @param file
     * @return
     */
	public static String getFileExtName(String file) {
		if (file == null)
			return null;
		int i = file.lastIndexOf('.');
		if (i < 0 && i >= file.length() - 1)
			return null;
		return file.substring(i+1);
	}

	
	/**
	 * Get file size. If it's a directory, it calculates the total size of files
	 * @param filePath
	 * @return
	 */
	public static long getFileSize(String filePath){
		return getFileSize(new File(filePath));
	}

	/**
	 * Get file size. If it's a directory, it calculates the total size of files
	 * @param file
	 * @return
	 */
	public static long getFileSize(File file){
		if (file.isFile())
			return file.length();
		
		long size = 0;
		File[] files = file.listFiles();
		if (files == null)
			return size;
		
		for (File f : files) {
			size += getFileSize(f);
		}
		return size;
	}
	
	/**
	 * Unzip a zip file into the destination directory
	 * @param zip
	 * @param dest
	 * @return
	 */
	public static boolean unzip(String zip, String dest) {
		return unzip(new File(zip), new File(dest));
	}
	
	/**
	 * Unzip a zip file into the destination directory
	 * @param zipFile
	 * @param destFile
	 * @return
	 */
	public static boolean unzip(File zipFile, File destFile) {
		ZipInputStream zin = null;
		FileOutputStream fos = null;
		try {
			zin = new ZipInputStream(new FileInputStream(zipFile));
			ZipEntry entry;
			while ((entry = zin.getNextEntry()) != null) {
				File entryFile = new File(destFile, entry.getName());
				if (entry.isDirectory()) {
					entryFile.mkdirs();
				} else {
					fos = new FileOutputStream(entryFile);
					byte[] b = new byte[1024];
					int len = 0;
					while ((len = zin.read(b)) != -1) {
						fos.write(b, 0, len);
					}
					fos.close();
					zin.closeEntry();
				}
				
				if (entry.getTime() >= 0)
					entryFile.setLastModified(entry.getTime());
			}
			return true;
		} catch (IOException e) {
			log.log(Level.SEVERE, MessageFormat.format(" Fail! Unzip [{0}] -> [{1}]", zipFile, destFile), e);
			return false;
		} finally {
			if (zin != null)
				try {
					zin.close();
				} catch (IOException e) {
				}
			if (fos != null)
				try {
					fos.close();
				} catch (IOException e) {
				}
		}
	}
	
	private static void zip(File dir, ZipOutputStream out, String prefix) throws Exception {
		File[] files = dir.listFiles();
		for (File f : files) {
			if (f.isFile()) {
				BufferedInputStream bis = null;
				try {
					bis = new BufferedInputStream(new FileInputStream(f));
					ZipEntry entry = new ZipEntry(prefix + f.getName());
					out.putNextEntry(entry);
					int count;
					byte data[] = new byte[8192];
					while ((count = bis.read(data)) != -1) {
						out.write(data, 0, count);
					}
					bis.close();
				} finally {
					if (bis != null)
						bis.close();
				}
				
			} else {
				zip(f, out, prefix + f.getName() + "/");
			}
		}
	}
	
	public static void zip(File workingDir, File zipFile) {
		zip(workingDir, zipFile, null);
	}
	
	public static void zip(File workingDir, File zipFile, String prefix) {
		if (!workingDir.isDirectory())
			return;
		
		if (prefix == null)
			prefix = "";
		
		ZipOutputStream out = null;
		try {
			out = new ZipOutputStream(new BufferedOutputStream(new FileOutputStream(zipFile)));
			zip(workingDir, out, prefix);
		} catch (Exception e) {

		} finally {
			if (out != null)
				try {
					out.close();
				} catch (IOException e) {
				}
		}
	       
	 }
	/**
	 * Get an unique name under the specified directory
	 * @param dir
	 * @param prefix
	 * @param suffix
	 * @return
	 */
	public static File getUniqueFile(File dir, String prefix, String suffix) {
		String name = prefix + "." + FILENAME_FORMAT.format(new Date()) + ".";
		for (int i = 0; i < Integer.MAX_VALUE; i++) {
			File file = new File(dir, name + i + suffix);
			if (!file.exists()) {
				return file;
			}
		}
		
		return null;
	}
	
	/**
	 * Get an unique name under the specified directory
	 * @param dir
	 * @param prefix
	 * @param suffix
	 * @return
	 */
	public static File getUniqueFile(String dir, String prefix, String suffix) {
		return getUniqueFile(new File(dir), prefix, suffix);
	}
	
	/**
	 * Download a file from a url to the local file system
	 * @param urlString
	 * @param output
	 * @return
	 */
	public static File download(String urlString, File output) {
		return download(urlString, output, false);
	}
	
	public static File download(String urlString, File output, boolean usetimestamp) {
		return download(urlString, output, false, null);
	}
	
	/**
	 * Download a file from a url to the local file system
	 * @param urlString
	 * @param output
	 * @param usetimestamp
	 * @return
	 */
	public static File download(String urlString, File output, boolean usetimestamp, boolean[] skip) {
		InputStream in = null;
		OutputStream out = null;
		try {
			URI url = new URI(urlString);
			URLConnection urlConnection = url.toURL().openConnection();
			int totalSize = urlConnection.getContentLength();
			in = urlConnection.getInputStream();
			if (output.isDirectory()) 
				output = new File(output, new File(url.getPath()).getName());
			output.getParentFile().mkdirs();
			if (usetimestamp && output.exists()) {
				if (output.lastModified() == urlConnection.getLastModified()) {
					log.info(MessageFormat.format(" Skip! Download {0} -> {1}", urlString, output));
					if (skip != null && skip.length > 0)
						skip[0] = true;
					return output;
				}
			}
			
			out = new FileOutputStream(output);
			byte[] buffer = new byte[1024 * 100]; // 100k
			int count = 0;
			int totalCount = 0;
			int progress = 0;
			while ((count = in.read(buffer)) > 0) {
				out.write(buffer, 0, count);
				totalCount += count;
				
				if (totalSize > 0) {
					int nowProgress = totalCount * 10 / totalSize;
					if (nowProgress > progress) {
						progress = nowProgress;
					}
				}
				
			}
			out.close();
			if (urlConnection.getLastModified() >= 0)
				output.setLastModified(urlConnection.getLastModified());
			log.info(MessageFormat.format("OK! Download {0} -> {1}", urlString, output));
			if (skip != null && skip.length > 0)
				skip[0] = false;
			return output;
		} catch (Exception e) {
			log.log(Level.SEVERE, MessageFormat.format("Fail! Download {0} -> {1}", urlString, output), e);
			return null;
		} finally {
			if (in != null)
				try {
					in.close();
				} catch (IOException e) {
				}
			if (out != null)
				try {
					out.close();
				} catch (IOException e) {

				}
		}
	}


	/**
	 * Convert a file path to URL like "file:///dir/some.file"
	 * @param file
	 * @return
	 */
	public static String getUrl(File file) {
		try {
			String url = file.getCanonicalFile().toURI().toASCIIString();
			return url.replace("file:/", "file:///");
		} catch(Exception e) {
			
		}
		
		return null;
	}
	
	
	/**
	 * Convert a file path to URL like "file:///dir/some.file"
	 * @param file
	 * @return
	 */
	public static String getUrl(String path) {
		return getUrl(new File(path));
	}
	
	/**
	 * Check if the given address is valid URL
	 * @param address
	 * @return
	 */
	public static boolean isUrl(String address) {
		if (address == null)
			return false;
		try {
			new URL(address);
			return true;
		} catch (MalformedURLException e) {
			return false;
		}
		
	}
}
