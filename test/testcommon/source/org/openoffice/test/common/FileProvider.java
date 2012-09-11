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

		TestClassRunnerForParameters(Class<?> type, Object[] parameters) throws InitializationError {
			super(type);
			this.parameters = parameters;
		}

		@Override
		public Object createTest() throws Exception {
			return getTestClass().getOnlyConstructor().newInstance(parameters);
		}

		@Override
		protected String getName() {
			return getTestClass().getJavaClass().getSimpleName() +  Arrays.toString(parameters);
		}

		@Override
		protected String testName(FrameworkMethod method) {
			return method.getName() + Arrays.toString(parameters);
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

	/**
	 * 
	 * Only called reflectively. Do not use programmatically.
	 */
	public FileProvider(Class<?> klass) throws Throwable {
		super(klass, NO_RUNNERS);

		String repos = null;
		String filter = null;
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
		}

		File reposFile = new File(repos);
		if (!reposFile.exists())
			throw new InitializationError(String.format("repos '%s' does not exists ", repos));

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
		if (reposFile.isDirectory()) {
			collectFromDir(reposFile, list, filterItems);
		} else {
			collectFromFile(reposFile, list, filterItems);
		}
		
		for (Object[] t : list) {
			TestClassRunnerForParameters runner = new TestClassRunnerForParameters(getTestClass().getJavaClass(), t);
			runners.add(runner);
		}

	}

	/**
	 * @param dir
	 * @param list
	 */
	public static void collectFromDir(File dir, ArrayList<Object[]> list, ArrayList<ArrayList<String>> filterItems) {
		File[] files = dir.listFiles();
		if (files == null)
			return;
		Arrays.sort(files);
		for (File file : files) {
			if (file.isDirectory()) {
				collectFromDir(file, list, filterItems);
				continue;
			}

			filter(file.getAbsolutePath(), list, filterItems);
		}
	}

	public static void collectFromFile(File file, ArrayList<Object[]> list, ArrayList<ArrayList<String>> filterItems) {
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
