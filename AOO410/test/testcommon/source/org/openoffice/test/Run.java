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
package org.openoffice.test;

import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.logging.Level;

import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.notification.RunListener;
import org.junit.runner.notification.Failure;
import org.openoffice.test.common.FileUtil;
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
		System.out.print(FileUtil.readStreamAsString(Run.class.getResourceAsStream("Run.help"), "utf-8"));
		System.exit(code);
	}
	
	public static void main(String... args) {
		ArrayList<String> runnableClasses = new ArrayList<String>();
		ArrayList<String> listenerClasses = new ArrayList<String>();
		
		ArrayList<String> tcs = new ArrayList<String>();
		ArrayList<String> tps = new ArrayList<String>();
		ArrayList<String> tms = new ArrayList<String>();
		
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
			} else if (arg.equals("-propertyfile")) {
				if (++i >= args.length)
					printUsage("Invalid arguments", 1);
				Properties props = FileUtil.loadProperties(args[i]);
				System.getProperties().putAll(props);
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
				tcs.add(args[i]);
			} else if (arg.equals("-tp")) {
				if (++i >= args.length)
					printUsage("Invalid arguments", 1);
				tps.add(args[i]);
			} else if (arg.equals("-tm")) {
				if (++i >= args.length)
					printUsage("Invalid arguments", 1);
				tms.add(args[i]);
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
		for (String l : listenerClasses)  {
			try {
				core.addListener((RunListener) Class.forName(l).newInstance());
			} catch (Exception e) {
				// Ignore
				log.log(Level.WARNING, "Listener is not added!", e);
			}
		}
		
		for (String s : tcs) {
			NamedRequest request = NamedRequest.tc(s);
			addRequest(requests, request);
		}
		
		for (String s : tps) {
			NamedRequest request = NamedRequest.tp(s);
			addRequest(requests, request);
		}
		
		for (String s : tms) {
			NamedRequest request = NamedRequest.tm(s);
			addRequest(requests, request);
		}
		
		int code = 0;
		for (NamedRequest request : requests) {
			Result result = core.run(request.getRunner());
			if (!result.wasSuccessful()) {
				code = 1;
				java.util.List<Failure> failureList = result.getFailures();
				for( Failure f: failureList)
					log.log( Level.SEVERE, "Failure in "+request.getName()+" :"+ f.toString(), f.getException());
			}
		}
		
		System.exit(code);
	}
}

