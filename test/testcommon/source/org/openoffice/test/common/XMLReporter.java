package org.openoffice.test.common;

import java.io.File;

import org.junit.Ignore;
import org.junit.runner.Description;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;
import org.junit.runner.notification.RunListener;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

public class XMLReporter extends RunListener {

	private File file = null;
	
	private Document doc = null;
	
	private Element root = null;
	 
	@Override
	public void testAssumptionFailure(Failure failure) {
		System.out.println("testAssumptionFailure");
	}

	@Override
	public void testFailure(Failure failure) throws Exception {
		System.out.println("testFailure");
	}

	@Override
	public void testFinished(Description description) throws Exception {
		System.out.println("testFinished");
	}

	@Override
	public void testIgnored(Description description) throws Exception {
		Ignore a = description.getAnnotation(Ignore.class);
		System.out.println("testIgnored" + a.value());
	}

	@Override
	public void testRunFinished(Result result) throws Exception {
		System.out.println("testRunFinished");
		FileUtil.storeXML(doc, file);
	}

	@Override
	public void testRunStarted(Description description) throws Exception {
		System.out.println("testRunStarted");
		file = Testspace.getFile("output/result/" + description.getClassName() + ".xml");
		doc = FileUtil.newXML();
		root = doc.createElement("testsuites");
		doc.appendChild(root);
		
		System.out.println(SystemUtil.getOSName());
		System.out.println(SystemUtil.getOSVersion());
		System.out.println(SystemUtil.getOSArch());
	}

	@Override
	public void testStarted(Description description) throws Exception {
		System.out.println("testStarted");
	}

	
}
