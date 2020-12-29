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

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.Properties;
import java.util.Set;
import java.util.UUID;
import java.util.logging.Logger;

import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.SystemUtil;

/**
 * It represents one OpenOffice instance.
 * By default, the instance will be started with automation and socket UNO connection enabled.
 *
 */
public class OpenOffice {
	
	private static Logger LOG = Logger.getLogger(OpenOffice.class.getName());
	
	private static final String[] DEFAULT_HOME = new String[] {
		"C:/Program Files/OpenOffice.org 3",
		"C:/Program Files (x86)/OpenOffice.org 3",
		"/Applications/OpenOffice.org.app/Contents",
		"/opt/openoffice.org3",
	};
	
	private static final String USERHOME = System.getProperty("user.home");
	private static final String BIN = SystemUtil.isWindows() ? "program/soffice.exe" : SystemUtil.isMac() ? "MacOS/soffice": "program/soffice"; 
	private static final String SYSUSERCONFIG = SystemUtil.isWindows()? System.getenv("APPDATA") : SystemUtil.isMac() ?  USERHOME + "/Library/Application Support" : USERHOME;
	public static final String DEFAULT_UNO_URL = "socket,host=127.0.0.1,port=2002;urp";
	public static final int DEFAULT_AUTOMATION_PORT = 12479;
	
	private static OpenOffice defaultInstance = null;

	private File userInstallation = null;

	private File defaultUserInstallation = null;
	
	private File home = null;

	private File bin = null;
	
	private String binPath = null;
	
	private ArrayList<String> args = new ArrayList<String>();

	private ArrayList<String> registryModifications = new ArrayList<String>();
	
	private int automationPort = 0;

	private Process process = null;
	
	private String unoUrl = null;
	
	private Properties versionProps = null;
	
	private String id = UUID.randomUUID().toString().replace("-", "");
	
	private String processPattern = SystemUtil.isMac() ? ".*soffice .*" + id + ".*" : ".*soffice\\.bin.*" + id + ".*|.*soffice\\.exe.*" + id + ".*-env.*";
	
	public OpenOffice() {
		this(null);
	}
	
	/**
	 * Construct Process with the home path of OpenOffice.
	 * 
	 * @param appHome
	 */
	public OpenOffice(String appHome) {
		if (appHome == null)
			appHome = System.getProperty("openoffice.home");
		if (appHome == null)
			appHome = System.getenv("OPENOFFICE_HOME");
		if (appHome == null) {
			// Search in the classpath
			try {
				URL url = getClass().getClassLoader().getResource(BIN);
				File file = new File(url.toURI());
				if (file.exists()) 
					appHome = file.getParentFile().getParentFile().getAbsolutePath();
			} catch (Exception e) {
				// ignore
			}
		}
		
		if (appHome == null) {
			for (int i = 0; i < DEFAULT_HOME.length; i++) 
				if (new File(DEFAULT_HOME[i]).exists()) 
					appHome = DEFAULT_HOME[i];
		}

		home = new File(appHome);
		bin = new File(appHome, BIN);
		try {
			binPath = bin.getCanonicalPath();
		} catch (IOException e1) {
		}
		File binParent = bin.getParentFile();
		File bootstrapFile = new File(binParent, "bootstraprc");
		if (!bootstrapFile.exists())
			bootstrapFile = new File(binParent, "bootstrap.ini");
		if (!bootstrapFile.exists())
			throw new Error("OpenOffice can not be found or it's broken. Testing can not be performed. " +
					"Use system property openoffice.home to specify the correct location of OpenOffice.");

		Properties props = FileUtil.loadProperties(bootstrapFile);
		String defaultUserInstallationPath = props.getProperty("UserInstallation").replace("$ORIGIN", binParent.getAbsolutePath()).replace("$SYSUSERCONFIG", SYSUSERCONFIG);
		defaultUserInstallation = new File(defaultUserInstallationPath);
		
		File versionFile = new File(binParent, "versionrc");
		if (!versionFile.exists())
			versionFile = new File(binParent, "version.ini");
		versionProps = FileUtil.loadProperties(versionFile);
		Set<Entry<Object, Object>> entries = versionProps.entrySet();
		for (Entry<Object, Object> e : entries) {
			System.setProperty("info.app." + e.getKey(), (String)e.getValue());
		}
		//
		SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd");
		System.setProperty("info.app.date", dateFormat.format(new Date(versionFile.lastModified())));
		addArgs("-" + id);
	}

	public static OpenOffice getDefault() {
		if (defaultInstance == null) {
			defaultInstance = new OpenOffice();
			defaultInstance.setAutomationPort(DEFAULT_AUTOMATION_PORT);
			defaultInstance.setUnoUrl(DEFAULT_UNO_URL);
			defaultInstance.addArgs("-nofirststartwizard", "-norestore", "-quickstart=no");
			defaultInstance.addRegistryModifications("<item oor:path=\"/org.openoffice.Office.Common/Misc\"><prop oor:name=\"UseSystemFileDialog\" oor:op=\"fuse\"><value>false</value></prop></item>",
					"<item oor:path=\"/org.openoffice.Office.Common/Security/Scripting\"><prop oor:name=\"MacroSecurityLevel\" oor:op=\"fuse\"><value>0</value></prop></item>");
		}

		return defaultInstance;
	}

	public static void restoreDefault() {
		if (defaultInstance != null) {
			
		}
	}
	
	public Properties getVersionProps() {
		return versionProps;
	}

	/**
	 * Set UserInstallation directory. When openoffice is launched, the argument
	 * "-env:UserInstallation" will be enabled.
	 * 
	 * @param dir
	 *            user installation directory. If null is given, the default
	 *            will be used.
	 */
	public void setUserInstallation(File dir) {
		userInstallation = dir;
	}

	/**
	 * Get UserInstallation directory
	 * 
	 * @return
	 */
	public File getUserInstallation() {
		return userInstallation == null ? defaultUserInstallation : userInstallation;
	}

	/**
	 * Get default UserInstallation directory
	 * 
	 * @return
	 */
	public File getDefaultUserInstallation() {
		return defaultUserInstallation;
	}
	
	/**
	 * Clean the user installation
	 */
	public void cleanUserInstallation() {
		FileUtil.deleteFile(getUserInstallation());
	}
	
	/**
	 * Get installation directory of OpenOffice.
	 * 
	 * @return
	 */
	public File getHome() {
		return home;
	}

	/**
	 * Set other command line arguments
	 * 
	 * @param args
	 */
	public void addArgs(String... arguments) {
		for (String a : arguments)
			args.add(a);
	}
	
	public void addRegistryModifications(String... items) {
		for (String i : items)
			registryModifications.add(i);
	}
	
	public int getAutomationPort() {
		return automationPort;
	}

	/**
	 * Set automation server listening port
	 * @param automationPort If 0, automation server will be disabled.
	 */
	public void setAutomationPort(int automationPort) {
		this.automationPort = automationPort;
	}

	public String getUnoUrl() {
		return unoUrl;
	}

	/**
	 * Set UNO connection listening url
	 * @param unoUrl If null, UNO connection will be disabled.
	 */
	public void setUnoUrl(String unoUrl) {
		this.unoUrl = unoUrl;
	}

	/**
	 * destroy OpenOffice
	 */
	public void kill() {
		SystemUtil.killProcess(".*soffice.*" + id + ".*");
		SystemUtil.sleep(1);
		process = null;
	}

	/**
	 * Kill all openoffice instances
	 */
	public static void killAll() {
		SystemUtil.killProcess(".*soffice.*");
		SystemUtil.sleep(1);
	}
	
	public boolean isRunning() {
		if (process == null)
			return false;
		
		try {
			process.exitValue();
		} catch (Exception e1) {
			return true;
		}
		
		return false;
	}
																																																														
	/**
	 * Start OpenOffice
	 * 
	 * @return return true when the process is created.
	 */
	@SuppressWarnings("all")
	public boolean start() {
		if (isRunning())
			return false;
		
		ArrayList<String> cmds = new ArrayList<String>();
		cmds.add(binPath);
		if (automationPort > 0) {
			cmds.add("-automationport=" + automationPort);
			cmds.add("-enableautomation");
		}

		if (unoUrl != null)
			cmds.add("-accept=" + unoUrl);
		
		if (userInstallation != null) {
			try {
				String url = userInstallation.toURL().toString();
				url = url.replace("file:/", "file:///");
				cmds.add("-env:UserInstallation=" + url);
			} catch (MalformedURLException e) {
				// ignore never to occur
			}
		}

		if (args != null)
			cmds.addAll(args);

		if (registryModifications.size() > 0) {
			File registry = new File(getUserInstallation(), "user/registrymodifications.xcu");
			String content = FileUtil.readFileAsString(registry);
			String newContent = "";
			for (String item : registryModifications) {
				if (!content.contains(item)) {
					newContent += item;
				}
			}
			content = content.replace("</oor:items>", "");
			if (content.length() == 0)
				content += "<?xml version=\"1.0\" encoding=\"UTF-8\"?><oor:items xmlns:oor=\"http://openoffice.org/2001/registry\" xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">";
			content += newContent + "</oor:items>";
			FileUtil.writeStringToFile(registry.getAbsolutePath(), content);
		}
		
		process = SystemUtil.backgroundExec(cmds.toArray(new String[]{}), null, null, null, null);
		
		String cmdLine="";
		for (String s : cmds)
			cmdLine += "\""+ s + "\" ";
	
		LOG.info(cmdLine);
		if (process == null)
			throw new RuntimeException("OpenOffice can't be started!");
		return true;
	}
	
	public HashMap<String, Object> getPerfData() {
		HashMap<String, Object> proccessInfo = SystemUtil.findProcess(processPattern);
		String pid = (String) proccessInfo.get("pid");
		if (pid == null)
			throw new RuntimeException("Can not find performance data");
		return SystemUtil.getProcessPerfData(pid);
	}
}
