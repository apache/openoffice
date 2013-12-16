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
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Map.Entry;
import java.util.Set;

import org.junit.Ignore;
import org.junit.runner.Description;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;
import org.junit.runner.notification.RunListener;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

/**
 * Generate XML test report
 *
 */
public class XMLReporter extends RunListener {

	private File outputDir = Testspace.getFile("output");
	
	private File file = null;
	
	private Document doc = null;
	
	private Element testsuiteEl = null;
	
	private Element testcaseEl = null;
	
	private String suiteName = null;
	
	private long suiteStart = 0;
	
	private long failures = 0;
	
	private long errors = 0;
	
	private long tests = 0;
	
	private long ignored = 0;
	
	private long testStart = 0;
	
	@Override
	public void testStarted(Description description) throws Exception {
//		if (!description.getClassName().equals(testClassName)) {
//			finishSuite();
//			startSuite(description);
//			testClassName = description.getClassName();
//		}
		testcaseEl = doc.createElement("testcase");
		testcaseEl.setAttribute("name", description.getDisplayName());
		testcaseEl.setAttribute("classname", description.getClassName());
		testcaseEl.setAttribute("methodname", description.getMethodName());
		testsuiteEl.appendChild(testcaseEl);
		testStart = System.currentTimeMillis();
	}
	
	@Override
	public void testAssumptionFailure(Failure failure) {

	}

	@Override
	public void testFailure(Failure failure) throws Exception {
		if (failure.getException() instanceof AssertionError) {
			failures++;
			Element failureEl = doc.createElement("failure");
			failureEl.setAttribute("message", failure.getMessage());
			failureEl.setAttribute("type", failure.getTestHeader());
			failureEl.setTextContent(failure.getTrace());
			testcaseEl.appendChild(failureEl);
		} else {
			errors++;
			Element errorEl = doc.createElement("error");
			errorEl.setAttribute("message", failure.getMessage());
			errorEl.setAttribute("type", failure.getTestHeader());
			errorEl.setTextContent(failure.getTrace());
			testcaseEl.appendChild(errorEl);
		}
	}

	@Override
	public void testFinished(Description description) throws Exception {
		tests++;
		testcaseEl.setAttribute("time", Double.toString((System.currentTimeMillis() - testStart) / 1000.0));
		store();
	}

	@Override
	public void testIgnored(Description description) throws Exception {
		testStarted(description);
		ignored++;
		Ignore ignore = description.getAnnotation(Ignore.class);
		Element ignoredEl = doc.createElement("ignored");
		ignoredEl.setAttribute("message", ignore.value());
		testcaseEl.appendChild(ignoredEl);
		testFinished(description);
	}

	@Override
	public void testRunFinished(Result result) throws Exception {
		finishSuite();
		File outputBackupDir = new File(outputDir.getAbsolutePath() + "." + suiteName);
		if (outputBackupDir.exists()) {
			outputBackupDir.renameTo(new File(outputBackupDir.getAbsolutePath() + "." + System.currentTimeMillis()));
			FileUtil.deleteFile(outputBackupDir);
		}
		
		outputDir.renameTo(outputBackupDir);
	}

	@Override
	public void testRunStarted(Description description) throws Exception {
		suiteName = description.getDisplayName();
		FileUtil.deleteFile(outputDir);//clear all old output
		startSuite();
	}

	private void startSuite() {
		suiteStart = System.currentTimeMillis();
		failures = 0;
		errors = 0;
		tests = 0;
		ignored = 0;
		
		file = new File(outputDir, "result.xml");
		doc = FileUtil.newXML();
	
		testsuiteEl = doc.createElement("testsuite");
		testsuiteEl.setAttribute("name", suiteName);
		testsuiteEl.setAttribute("start", Long.toString(suiteStart));
		doc.appendChild(testsuiteEl);
	}
	
	private void finishSuite() {
		store();
	}

	private void store() {
		if (doc != null) {
			testsuiteEl.setAttribute("time", Double.toString((System.currentTimeMillis() - testStart) / 1000.0));
			testsuiteEl.setAttribute("failures", Long.toString(failures));
			testsuiteEl.setAttribute("errors", Long.toString(errors));
			testsuiteEl.setAttribute("tests", Long.toString(tests));
			testsuiteEl.setAttribute("ignored", Long.toString(ignored));
			NodeList els = testsuiteEl.getElementsByTagName("properties");
			if (els.getLength() > 0) 
				testsuiteEl.removeChild(els.item(0));
			
			Element props = doc.createElement("properties");
			testsuiteEl.appendChild(props);
			// Add some extra information
			System.setProperty("info.os.name", SystemUtil.getOSName());
			System.setProperty("info.os.version", SystemUtil.getOSVersion());
			System.setProperty("info.os.arch", SystemUtil.getOSArch());
			String ipaddrStr = SystemUtil.getIPAddress();
			System.setProperty("info.ip", (ipaddrStr!=null) ? ipaddrStr : "UNKNOWN");
			String hostnameStr = SystemUtil.getHostName();
			System.setProperty("info.hostname", (hostnameStr!=null) ? hostnameStr : "UNKNOWN");
			Set<Entry<Object, Object>> entries = System.getProperties().entrySet();
			for (Entry<Object, Object> e : entries) {
				Element prop = doc.createElement("property");
				prop.setAttribute("name", "" + e.getKey());
				prop.setAttribute("value", "" + e.getValue());
				props.appendChild(prop);
			}

			SimpleDateFormat dateFormat = new SimpleDateFormat( "yyyy-MM-dd HH-mm-ss");
			System.setProperty( "info.test.date", dateFormat.format( new Date()));

			FileUtil.storeXML(doc, file);
			File htmlFile = new File(outputDir, "result.html");
			InputStream is = getClass().getResourceAsStream("XMLReporter.xsl");
			if (is != null) {
				FileUtil.storeXML(doc, htmlFile, is);
			}
		}
	}

}

