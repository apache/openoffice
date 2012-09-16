package org.openoffice.test;

import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;

import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.notification.RunListener;
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.NamedRequest;

public class Run {
	
	private static void addRequest(List<NamedRequest> requests, NamedRequest request) {
		for (NamedRequest r : requests) {
			if (r.getName().equals(request.getName())) {
				r.merge(request);
				return;
			}
		}
		
		requests.add(request);
	}
	
	private static void printUsage(String msg, int code) {
		if (msg != null)
			System.out.println(msg);
		System.out.println("Usage: [options]");
		System.out.println("	-D<property>=<value>	Set system property");
		System.out.println("	-r	Set a runnable which will be executed before all tests start.");
		System.out.println("	-l	Set a test listeners.");
		System.out.println("	-tp	Define a test suite with test packages separated by comma. Format: [name]:org.package1,org.package2...");
		System.out.println("	-tc	Define a test suite with test classes separated by comma. Format: [name]:org.package.Class1,org.package.Class2...");
		System.out.println("	-tc	Define a test suite with test methods separated by comma. Format: [name]:org.package.Class1.method1,org.package.Class2.method1...");
		System.out.println("Example:");
		System.out.println("-tp mytest:com.test");
		System.out.println("-tc mytest:com.test.TestName1,com.test.TestName2");
		System.exit(code);
	}
	
	public static void main(String... args) {
		ArrayList<String> runnableClasses = new ArrayList<String>();
		ArrayList<String> listenerClasses = new ArrayList<String>();
		List<NamedRequest> requests = new ArrayList<NamedRequest>();
		for (int i = 0; i < args.length; i++) {
			String arg = args[i];
			if ("-help".equals(arg)) {
				printUsage(null, 0);
			} else if (arg.startsWith("-D")) {
				String propEntry = arg.substring(2);
				String key = propEntry;
				String value = null;
				int in = propEntry.indexOf("=");
				if (in >= 0) {
					key = propEntry.substring(0, in);
					value = propEntry.substring(++in);
				}
				System.setProperty(key, value);
			} else if (arg.equals("-r")) {
				if (++i >= args.length)
					printUsage("Invalid arguments", 1);
				runnableClasses.add(args[i]);
			} else if (arg.equals("-l")) {
				if (++i >= args.length)
					printUsage("Invalid arguments", 1);
				listenerClasses.add(args[i]);
			} else if (arg.equals("-tc")) {
				if (++i >= args.length)
					printUsage("Invalid arguments", 1);
				NamedRequest request = NamedRequest.tc(args[i]);
				addRequest(requests, request);
			} else if (arg.equals("-tp")) {
				if (++i >= args.length)
					printUsage("Invalid arguments", 1);
				NamedRequest request = NamedRequest.tp(args[i]);
				addRequest(requests, request);
			} else if (arg.equals("-tm")) {
				if (++i >= args.length)
					printUsage("Invalid arguments", 1);
				NamedRequest request = NamedRequest.tm(args[i]);
				addRequest(requests, request);
			}
		}
		
		Logger log = Logger.getLogger(Run.class);
		for (String r : runnableClasses) {
			try {
				((Runnable) Class.forName(r).newInstance()).run();
			} catch (Exception e) {
				log.log(Level.SEVERE, "Runnable is failed!", e);
				System.exit(2);
			}
		}
		
		JUnitCore core = new JUnitCore();
//		core.addListener(new XMLReporter());
//		core.addListener(new CSVReporter());
		for (String l : listenerClasses)  {
			try {
				core.addListener((RunListener) Class.forName(l).newInstance());
			} catch (Exception e) {
				// Ignore
				log.log(Level.WARNING, "Listener is not added!", e);
			}
		}
		
		int code = 0;
		for (NamedRequest request : requests) {
			Result result = core.run(request.getRunner());
			if (!result.wasSuccessful())
				code = 1;
		}
		
		System.exit(code);
	}
}
