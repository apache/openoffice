package org.openoffice.test.common;

import java.io.File;
import java.io.InputStream;
import java.util.Map.Entry;
import java.util.Set;

import org.junit.Ignore;
import org.junit.runner.Description;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;
import org.junit.runner.notification.RunListener;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

public class XMLReporter extends RunListener {

	private File outputDir = Testspace.getFile("output");
	
	private File reportDir = new File(outputDir, "result");
	
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
		
		file = new File(reportDir, suiteName + ".xml");
		doc = FileUtil.newXML();
	
		testsuiteEl = doc.createElement("testsuite");
		testsuiteEl.setAttribute("name", suiteName);
		testsuiteEl.setAttribute("start", Long.toString(suiteStart));
		doc.appendChild(testsuiteEl);
	}
	
	private void finishSuite() {
		testsuiteEl.setAttribute("time", Double.toString((System.currentTimeMillis() - testStart) / 1000.0));
		testsuiteEl.setAttribute("failures", Long.toString(failures));
		testsuiteEl.setAttribute("errors", Long.toString(errors));
		testsuiteEl.setAttribute("tests", Long.toString(tests));
		testsuiteEl.setAttribute("ignored", Long.toString(ignored));
		Element props = doc.createElement("properties");
		testsuiteEl.appendChild(props);
		Set<Entry<Object, Object>> entries = System.getProperties().entrySet();
		for (Entry<Object, Object> e : entries) {
			Element prop = doc.createElement("property");
			prop.setAttribute("name", "" + e.getKey());
			prop.setAttribute("value", "" + e.getValue());
			props.appendChild(prop);
		}
		
		store();
	}

	private void store() {
		if (doc != null) {
			FileUtil.storeXML(doc, file);
			File htmlFile = new File(reportDir, file.getName() + ".html");
			InputStream is = getClass().getResourceAsStream("XMLReporter.xsl");
			if (is != null) {
				FileUtil.storeXML(doc, htmlFile, is);
			}
		}
	}

}
