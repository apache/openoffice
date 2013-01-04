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

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;

/**
 * A testspace is a directory on your hard drive where stores the files generated during testing, e.g. test result, logs, temp files.
 *
 */
public class Testspace {

	private static final File testspace = initTestspace();
	
	private static final File testdata = new File(System.getProperty("testdata", "data")); 
	
	private static File initTestspace() {
		String v = System.getProperty("testspace");
		File file = null;
		if (v == null) {
			file = new File("../testspace");
			System.setProperty("testspace", file.getAbsolutePath());
		} else {
			file = new File(v);
		}
		
		return file;
	}
	
	public static String getPath() {
		return testspace.getAbsolutePath();
	}

	public static String getPath(String filePath) {
		return getFile(filePath).getAbsolutePath();
	}
	
	public static String getUrl(String filePath) {
		return FileUtil.getUrl(getFile(filePath));
	}
	
	public static File getFile() {
		return testspace;
	}
	
	public static File getFile(String filePath) {
		File file = new File(filePath);
		if (file.isAbsolute())
			return file;
		
		return new File(testspace, filePath);
	}
	
	public static String prepareData(String dataFilePath) {
		File dataFile = prepareDataFile(dataFilePath);
		return dataFile.getAbsolutePath();
	}
	
	public static String prepareData(String dataFilePath, String to) {
		File dataFile = prepareDataFile(dataFilePath, to);
		return dataFile.getAbsolutePath();
	}
	
	public static File prepareDataFile(String dataFilePath) {
		getFile("temp").mkdir();
		return prepareDataFile(dataFilePath, "temp");
	}
	
	public static File prepareDataFile(String dataFilePath, String to) {
		File workingFile = getFile(to);
		if (FileUtil.isUrl(dataFilePath)) {
			if ((workingFile = FileUtil.download(dataFilePath, workingFile)) == null) {
				throw new RuntimeException("Can not prepare data: " + dataFilePath);
			}
			return workingFile;
		}
		
		File dataFile = new File(dataFilePath);
		if (workingFile.isDirectory()) 
			workingFile = new File(workingFile, dataFile.getName());
		
		if (!dataFile.isAbsolute()) 
			dataFile = new File(testdata, dataFilePath);
		if (!dataFile.exists()) {
			InputStream in = Testspace.class.getClassLoader().getResourceAsStream(dataFilePath);
			if (in == null)
				throw new RuntimeException("Can not prepare data: " + dataFilePath);
			if (!FileUtil.writeToFile(in, workingFile))
				throw new RuntimeException("Can not prepare data: " + dataFilePath);
		} else {
			if (!FileUtil.copyFile(dataFile, workingFile))
				throw new RuntimeException("Can not prepare data: " + dataFilePath);
		}
		
		return workingFile;
	}
	
	public static File getDataFile(String dataFilePath) {
		File dataFile = new File(dataFilePath);
		if (!dataFile.isAbsolute())
			dataFile = new File(testdata, dataFilePath);
		if (!dataFile.exists()) {
			URL url = Testspace.class.getClassLoader().getResource(dataFilePath);
			if (url == null)
				return null;
			return new File(url.getFile());
		} else {
			return dataFile;
		}
	}
	
	public static String getDataPath(String dataFilePath) {
		File file = getDataFile(dataFilePath);
		if (file == null)
			return null;
		try {
			return file.getCanonicalPath();
		} catch (IOException e) {
			return null;
		}
	}
	
	public static boolean deleteFile(String path) {
		return FileUtil.deleteFile(getPath(path));
	}
}
