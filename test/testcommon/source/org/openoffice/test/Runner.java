package org.openoffice.test;

import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;

import org.junit.runner.Description;
import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;
import org.junit.runner.notification.RunListener;
import org.openoffice.test.common.CSVReporter;
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.XMLReporter;

public class Runner {
	private static Logger log = Logger.getLogger(Runner.class);

	public static void main(String... args) {
		JUnitCore core = new JUnitCore();
		core.addListener(new XMLReporter());
		core.addListener(new CSVReporter());
		String prop = System.getProperty("listener");
		if (prop != null) {
			String[] listeners = prop.split(",");
			for (String l : listeners) {
				try {
					l = l.trim();
					Class<?> klass = Class.forName(l);
					Object obj = klass.newInstance();
					core.addListener((RunListener) obj);
				} catch (Exception e) {
					log.log(Level.SEVERE, "Listener is not added!", e);
				}
			}
		}

		List<Class<?>> classes = new ArrayList<Class<?>>();
		List<Failure> missingClasses = new ArrayList<Failure>();
		for (String each : args)
			try {
				classes.add(Class.forName(each));
			} catch (ClassNotFoundException e) {
				Description description = Description.createSuiteDescription(each);
				Failure failure = new Failure(description, e);
				missingClasses.add(failure);
			}

		Result result = core.run(classes.toArray(new Class[0]));
		for (Failure each : missingClasses)
			result.getFailures().add(each);

		System.exit(result.wasSuccessful() ? 0 : 1);
	}
}
