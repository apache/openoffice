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
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.logging.Level;

import org.junit.runner.Description;
import org.junit.runner.Result;
import org.junit.runner.notification.RunListener;

public class ReportUploader extends RunListener {
	
	private Logger log = Logger.getLogger(this);
	
	private File outputDir = Testspace.getFile("output");
	
	private String suiteName = null;
	
	private File reportZip = null;
	
	private String url = null;
	
	public ReportUploader() {
		url = System.getProperty("report.to");
		System.setProperty("report.to", ""); // 
	}
	
	@Override
	public void testRunStarted(Description description) throws Exception {
		suiteName = description.getDisplayName();
	}
	
	@Override
	public void testRunFinished(Result result) throws Exception {
		if (!FileUtil.isUrl(url))
			return;
		
		// upload
		File output = new File(outputDir.getAbsolutePath() + "." + suiteName);
		if (!output.exists())
			return;
		try {
			reportZip = File.createTempFile("aoo_test_report", ".zip");
			FileUtil.zip(output, reportZip);
			upload();
		} finally {
			if (reportZip != null)
				reportZip.delete();
		}
	}



	private static final String boundary = "-----AOOAOOAOO1d226f500d0";
	private static final String prefix = "--";
	private static final String newLine = "\r\n";

	private void upload() {
		InputStream fis = null;
		OutputStream out = null;
		try {
			HttpURLConnection connection = (HttpURLConnection)  new URL(url).openConnection();
			connection.setDoInput(true);
			connection.setDoOutput(true);
			connection.setConnectTimeout(20 * 1000);
			connection.setRequestMethod("POST");
			connection.setRequestProperty("Content-type", "multipart/form-data;boundary=" + boundary);
			
			out = connection.getOutputStream();
			fis = new FileInputStream(reportZip);
			
			StringBuffer params = new StringBuffer();
			params.append(prefix + boundary + newLine);
			params.append("Content-Disposition: form-data; name=\"name\"");
			params.append(newLine + newLine);
			params.append("aoo");
			params.append(newLine);
		
			params.append(prefix + boundary + newLine);
			params.append("Content-Disposition: form-data; name=\"file\"; filename=\"report.zip\"");
			params.append(newLine);
			params.append("Content-Type: application/zip");
			params.append(newLine + newLine);
			
			out.write(params.toString().getBytes());
			
			byte[] buffer = new byte[1024];
			int len = 0;
			while ((len = fis.read(buffer)) != -1) {
				out.write(buffer, 0, len);
			}
			
			params.setLength(0);
			params.append(newLine);
			params.append(prefix + boundary + prefix + newLine);
			
			out.write(params.toString().getBytes());
			out.flush();
			out.close();
		
			String response = FileUtil.readStreamAsString(connection.getInputStream(), "UTF-8");
			log.log(Level.INFO, "Upload report: \n" + response);
		} catch (Exception e) {
			log.log(Level.SEVERE, "Fail to upload report!", e);
		} finally {
			if (fis != null)
				try {
					fis.close();
				} catch (IOException e) {
				}
			if (out != null)
				try {
					out.close();
				} catch (IOException e) {
				}
		}
	}



}
