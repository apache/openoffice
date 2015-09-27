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
import java.io.IOException;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;
import java.text.MessageFormat;
import java.util.logging.Level;

/**
 * Install openoffice from installation package before running test
 * 
 */
public class Installer implements Runnable {
	private static Logger log = Logger.getLogger(Installer.class);
	File downloadDir = Testspace.getFile("download");
	File downloadUrl = Testspace.getFile("download/url");
	File installDir = Testspace.getFile("install");
	File installTempDir = Testspace.getFile("install_temp");

	@Override
	public void run() {
		String prop = System.getProperty("singleton");
		if ("true".equalsIgnoreCase(prop) || "yes".equalsIgnoreCase(prop)) {
			if (SystemUtil.findProcesses(".*org\\.openoffice\\.test\\.common\\.Installer.*").size() > 1) {
				throw new RuntimeException("Only allow one running test instance!");
			}
		}
		if ((prop = System.getProperty("openoffice.pack")) != null) {
			String onlyNewProp = System.getProperty("only.new");
			File packFile = null;
			if (FileUtil.isUrl(prop)) {
				log.log(Level.INFO, MessageFormat.format("Try to download {0}...", prop));
				String url = FileUtil.readFileAsString(downloadUrl);
				if (!prop.equals(url)) {
					FileUtil.deleteFile(downloadDir);
					downloadDir.mkdirs();
					packFile = FileUtil.download(prop, downloadDir);
					if (packFile == null)
						throw new RuntimeException(MessageFormat.format("{0} can not be downloaded!", prop));
					FileUtil.writeStringToFile(downloadUrl, prop);
				} else {
					boolean[] skipped = { false };
					packFile = FileUtil.download(prop, downloadDir, true, skipped);
					if (packFile == null)
						throw new RuntimeException(MessageFormat.format("{0} can not be downloaded!", prop));
					if (("true".equalsIgnoreCase(onlyNewProp) || "yes".equalsIgnoreCase(onlyNewProp)) && skipped[0])
						throw new RuntimeException(MessageFormat.format("{0} is old. Test is allowed only on new build.", prop));
				}
			} else {
				packFile = new File(prop);
				if (!packFile.isFile())
					throw new RuntimeException(MessageFormat.format("{0} does not exists or is not a file!", prop));
			}

			try {
				FileUtil.deleteFile(installDir);
				FileUtil.deleteFile(installTempDir);
				installTempDir.mkdirs();
				if (packFile.getName().endsWith(".gz")) {
					StringBuffer output = new StringBuffer();
					if (SystemUtil.exec(new String[] { "tar", "-zxpf", packFile.getAbsolutePath(), "-C", installTempDir.getAbsolutePath() }, output) != 0)
						throw new RuntimeException(MessageFormat.format("{0} can not be installed! Cause: {1}", packFile, output));
				} else {
					if (!FileUtil.unzip(packFile, installTempDir))
						throw new RuntimeException(MessageFormat.format("{0} can not be installed!", packFile));
				}
				// On windows, if path is too long, openoffice can not be
				// started.
				File[] files = installTempDir.listFiles();
				if (files != null && files.length == 1 && files[0].isDirectory()) {
					files[0].renameTo(installDir);
				}
				System.setProperty("openoffice.home", installDir.getAbsolutePath());
			} finally {
				FileUtil.deleteFile(installTempDir);
			}
		}

		prop = System.getProperty("openoffice.home", installDir.getAbsolutePath());
		File sofficeBin = FileUtil.findFile(new File(prop), "soffice.bin", false);
		if (sofficeBin == null)
			throw new RuntimeException(MessageFormat.format(
					"No valid OpenOffice is found in {0}! Use system property openoffice.home to specify a valid OpenOffice installation directory.", prop));
		try {
			prop = sofficeBin.getParentFile().getParentFile().getCanonicalPath();
			System.setProperty("openoffice.home", prop);
			log.log(Level.INFO, MessageFormat.format("OpenOffice in {0} will be tested.", prop));
		} catch (IOException e) {
			// ignore, never occurs
		}

		String[] jars = { "juh.jar", "unoil.jar", "ridl.jar", "jurt.jar" };
		for (String jar : jars) {
			File file = FileUtil.findFile(prop, jar);

			if (file != null)
				addToClassPath(file);
		}

	}

	public static boolean addToClassPath(File file) {
		try {
			URL url = file.toURI().toURL();
			URLClassLoader classLoader = (URLClassLoader) ClassLoader.getSystemClassLoader();
			Method method = URLClassLoader.class.getDeclaredMethod("addURL", new Class[] { URL.class });
			method.setAccessible(true);
			method.invoke(classLoader, new Object[] { url });
			return true;
		} catch (Throwable t) {
			t.printStackTrace();
		}

		return false;
	}

	public static void main(String... args) {
		new Installer().run();
	}
}
