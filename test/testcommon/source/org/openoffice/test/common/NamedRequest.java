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

import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;

import org.junit.runner.Description;
import org.junit.runner.Request;
import org.junit.runner.Runner;
import org.junit.runner.notification.RunNotifier;
import org.junit.runners.ParentRunner;
import org.junit.runners.model.InitializationError;
import org.openoffice.test.Run;

public class NamedRequest extends Request {
	
	private static Logger log = Logger.getLogger(Run.class);
	
	private Suite suite = null;
	
	protected NamedRequest() {
		
	}
	
	private String init(String arg) throws InitializationError {
		String name = null;
		int ci = arg.indexOf(":");
		if (ci >= 0) {
			name = arg.substring(0, ci);
			arg = arg.substring(++ci);
		} else {
			name = arg;
			if (name.length() > 128) {
				name = name.substring(0, 128) + "...";
			}
		}
		suite = new Suite(name);
		return arg;
	}
	
	private void addRunner(Runner runner) {
		suite.getChildren().add(runner);
	}
	
	public static NamedRequest tc(String arg) {
		NamedRequest namedRequest = new NamedRequest();
		try {
			arg = namedRequest.init(arg);
		} catch (InitializationError e1) {
			return null;
		}
		
		String[] klasses = arg.split(",");
		List<Class<?>> classes = new ArrayList<Class<?>>();
		for (String k : klasses) {
			try {
				classes.add(Class.forName(k));
			} catch (Throwable e) {
				log.log(Level.WARNING, MessageFormat.format("Test class {0} can not be tested!", k), e);
			}
		}
		
		namedRequest.addRunner(Request.classes(classes.toArray(new Class[0])).getRunner());
		return namedRequest;
	}
	
	public static NamedRequest tp(String arg) {
		NamedRequest namedRequest = new NamedRequest();
		try {
			arg = namedRequest.init(arg);
		} catch (InitializationError e1) {
			return null;
		}
		
		String[] packages = arg.split(",");
		List<Class<?>> classes = new ArrayList<Class<?>>();
		for (String p : packages) {
			List<String> klasses = SystemUtil.getClassesInPackage(p);
			for (String k : klasses) {
				if (k.matches(".+[$]{1}.*"))
					continue;
				try {
					classes.add(Class.forName(k));
				} catch (Throwable e) {
					log.log(Level.WARNING, MessageFormat.format("Test class {0} can not be tested!", k), e);
				}
			}
		}
		
		namedRequest.addRunner(Request.classes(classes.toArray(new Class[0])).getRunner());
		return namedRequest;
	}

	public static NamedRequest tm(String arg) {
		NamedRequest namedRequest = new NamedRequest();
		try {
			arg = namedRequest.init(arg);
		} catch (InitializationError e1) {
			return null;
		}
		
		String[] methods = arg.split(",");
		for (String m : methods) {
			int i = m.lastIndexOf(".");
			String className = m.substring(0, i);
			String methodName = m.substring(++i);
			try {
				namedRequest.addRunner(Request.method(Class.forName(className), methodName).getRunner());
			} catch (Throwable e) {
				log.log(Level.WARNING, MessageFormat.format("Test method {0} can not be tested!", m), e);
			}
			
		}
		return namedRequest;
	}

	public void merge(NamedRequest other) {
		suite.getChildren().addAll(other.suite.getChildren());
	}

	public String getName() {
		return suite.getName();
	}

	@Override
	public Runner getRunner() {
		return suite;
	}
	
	public static class Suite extends ParentRunner<Runner> {
		
		protected final List<Runner> fRunners = new ArrayList<Runner>();

		protected String name;
		
		protected Suite(String name) throws InitializationError {
			super(null);
			this.name = name;
		}

		protected String getName() {
			return name;
		}
		
		@Override
		protected List<Runner> getChildren() {
			return fRunners;
		}

		@Override
		protected Description describeChild(Runner child) {
			return child.getDescription();
		}

		@Override
		protected void runChild(Runner runner, final RunNotifier notifier) {
			runner.run(notifier);
		}
	}

}