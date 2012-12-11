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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.lang.annotation.Annotation;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.junit.runner.Runner;
import org.junit.runner.notification.RunNotifier;
import org.junit.runners.BlockJUnit4ClassRunner;
import org.junit.runners.Suite;
import org.junit.runners.model.FrameworkMethod;
import org.junit.runners.model.InitializationError;
import org.junit.runners.model.Statement;

public class FileProvider extends Suite {

	private static final List<Runner> NO_RUNNERS = Collections.<Runner> emptyList();

	private final ArrayList<Runner> runners = new ArrayList<Runner>();

	private class TestClassRunnerForParameters extends BlockJUnit4ClassRunner {

		private final Object[] parameters;

		private int index;
		
		TestClassRunnerForParameters(Class<?> type, Object[] parameters, int index) throws InitializationError {
			super(type);
			this.parameters = parameters;
			this.index = index;
		}

		@Override
		public Object createTest() throws Exception {
			return getTestClass().getOnlyConstructor().newInstance(parameters);
		}

		@Override
		protected String getName() {
			return getTestClass().getJavaClass().getSimpleName() + "[" + index + "]" +  Arrays.toString(parameters);
		}

		@Override
		protected String testName(FrameworkMethod method) {
			return method.getName() + "[" + index + "]" + Arrays.toString(parameters);
		}

		@Override
		protected void validateConstructor(List<Throwable> errors) {
			validateOnlyOneConstructor(errors);
		}

		@Override
		protected Statement classBlock(RunNotifier notifier) {
			return childrenInvoker(notifier);
		}

		@Override
		protected Annotation[] getRunnerAnnotations() {
			return new Annotation[0];
		}
	}

	@Retention(RetentionPolicy.RUNTIME)
	@Target(ElementType.FIELD)
	public static @interface FileRepos {
	}

	@Retention(RetentionPolicy.RUNTIME)
	@Target(ElementType.FIELD)
	public static @interface FileFilter {
		
	}
	
	@Retention(RetentionPolicy.RUNTIME)
	@Target(ElementType.FIELD)
	public static @interface FileRepeat {
		
	}
	
	public FileProvider(Class<?> klass) throws Throwable {
		super(klass, NO_RUNNERS);

		String repos = null;
		String filter = null;
		int repeat = 1;
		Field fs[] = klass.getFields();
		for (Field f : fs) {
			Annotation a = f.getAnnotation(FileRepos.class);
			if (a != null) {
				Object v = f.get(null);
				if (!(v instanceof String))
					throw new InitializationError(String.format("Field annotated FileRepos '%s' must be String.", f.getName()));

				repos = (String) v;
			}

			a = f.getAnnotation(FileFilter.class);
			if (a != null) {
				Object v = f.get(null);
				if (v != null && !(v instanceof String))
					throw new InitializationError(String.format("Field annotated FileFilter '%s' must be String.", f.getName()));
				filter = (String) v;
			}
			
			a = f.getAnnotation(FileRepeat.class);
			if (a != null) {
				Object v = f.get(null);
				if (v != null && !(v instanceof Integer))
					throw new InitializationError(String.format("Field annotated FileFilter '%s' must be Integer.", f.getName()));
				repeat = (Integer) v;
			}
			
		}

		File reposFile = new File(repos);
//		if (!reposFile.exists())
//			
//			throw new InitializationError(String.format("repos '%s' does not exists ", repos));
		
		if(reposFile.exists()){
			ArrayList<ArrayList<String>> filterItems = new ArrayList<ArrayList<String>>();
			if (filter != null) {
				String[] args = SystemUtil.parseCommandLine(filter);
				ArrayList<String> filterItem = new ArrayList<String>();
				for (int i = 0; i < args.length; i++) {
					String a = args[i];
					if (a.equals("-f")) {
						if (filterItem.size() > 0) 
							filterItems.add(filterItem);
						filterItem = new ArrayList<String>();
					} else {
						filterItem.add(a);
					}
				}
				
				if (filterItem.size() > 0) 
					filterItems.add(filterItem);
			}
			
			
			ArrayList<Object[]> list = new ArrayList<Object[]>();
			if (!collectFromFile(reposFile, list, filterItems))
				if (!collectFromFiles(reposFile, list, filterItems))
					collectFromDir(reposFile, list, filterItems);
			
			for (int i = 0; i < list.size(); i++) {
				Object[] t = list.get(i);
				for  (int j = 0; j < repeat; j++) {
					TestClassRunnerForParameters runner = new TestClassRunnerForParameters(getTestClass().getJavaClass(), t, i * repeat + j);
					runners.add(runner);
				}
			}
		}

	}
	
	private static boolean collectFromFiles(File dir, ArrayList<Object[]> list, ArrayList<ArrayList<String>> filterItems) {
		if (!dir.isDirectory())
			return false;
		
		boolean hasListFile = false;
		File[] files = dir.listFiles();
		for (File f : files) {
			if (f.isFile() && f.getName().endsWith(".files")) {
				hasListFile = true;
				collectFromFile(f, list, filterItems);
			}
		}
		
		return hasListFile;
	}
	
	private static boolean collectFromDir(File dir, ArrayList<Object[]> list, ArrayList<ArrayList<String>> filterItems) {
		if (!dir.isDirectory())
			return false;
		
		File[] files = dir.listFiles();
		Arrays.sort(files);
		for (File file : files) {
			if (file.isDirectory()) {
				collectFromDir(file, list, filterItems);
				continue;
			}

			filter(file.getAbsolutePath(), list, filterItems);
		}
		
		return true;
	}

	private static boolean collectFromFile(File file, ArrayList<Object[]> list, ArrayList<ArrayList<String>> filterItems) {
		if (!file.isFile())
			return false;
		
		BufferedReader reader = null;
		try{	
			reader = new BufferedReader(new FileReader(file));
			String line = null;
			while((line = reader.readLine()) != null){
				filter(line, list, filterItems);
			}
		}catch(Exception e){
			//ignore
		}finally{
			try{
				if(reader != null){
					reader.close();
					reader = null;
				}
			
			}catch(Exception io){
				//ignore;
			}
		}
		
		return true;
	}
	
	private static void filter(String filePath, ArrayList<Object[]> list, ArrayList<ArrayList<String>> filterItems) {
		if (filterItems.size() == 0) {
			Object[] data = { filePath};
			list.add(data);
			return;
		}

		for (int i = 0; i < filterItems.size(); i++) {
			ArrayList<String> filterItem = filterItems.get(i);
			String pattern = filterItem.get(0);
			if (pattern != null && filePath.matches(pattern)) {
				Object[] data = new Object[filterItem.size()];
				data[0] = filePath;
				for (int j = 1; j < filterItem.size(); j++)
					data[j] = filterItem.get(j);
				
				list.add(data);
			}
		}
	}
	
	
	@Override
	protected List<Runner> getChildren() {
		return runners;
	}
}
