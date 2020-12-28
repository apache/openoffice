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

import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.Transferable;
import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.StringReader;
import java.net.InetAddress;
import java.net.URL;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.List;
import java.util.StringTokenizer;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * Utilities related to system
 * 
 */
public class SystemUtil {

	private static Logger LOG = Logger.getLogger(SystemUtil.class.getName());

	private static Clipboard sysClipboard = Toolkit.getDefaultToolkit().getSystemClipboard();

	private static final File SCRIPT_TEMP_DIR = Testspace.getFile("bin");

	private static String platform = System.getProperty("os.name");
	
	private static String osName = System.getProperty("os.name");
	
	private static String osVersion  = System.getProperty("os.version");
	
	private static String osArch = System.getProperty("os.arch");
	
	static {
		if (isLinux()) {
			StringBuffer output = new StringBuffer();
			if (exec(new String[]{"lsb_release", "-is"}, output) == 0)
				osName = output.toString().trim();
			output.setLength(0);
			if (exec(new String[]{"lsb_release", "-rs"}, output) == 0)
				osVersion = output.toString().trim();	
		}
	}

	/**
	 * Play beep sound! The method doesn't work, if the code is executed on
	 * Eclipse IDE.
	 * 
	 */
	public static void beep() {
		System.out.print("\007\007\007");
		System.out.flush();
	}

	public static boolean isWindows() {
		return platform.startsWith("Windows");
	}

	public static boolean isLinux() {
		return platform.startsWith("Linux");
	}

	public static boolean isMac() {
		return platform.startsWith("Mac");
	}
	
	public static boolean isBSD() {
		return platform.endsWith("BSD");
	}

	public static String getOSName() {
		return osName;
	}
	
	public static String getOSVersion() {
		return osVersion;
	}
	
	public static String getOSArch() {
		return osArch;
	}
	
	/**
	 * Set the contents of the clipboard to the provided text
	 */
	public static void setClipboardText(String s) {
		StringSelection ss = new StringSelection(s);

		// if (OS.get() == OS.MACOSX) {
		// // workaround MAC OS X has a bug. After setting a text into
		// clipboard, the java program will not
		// // receive the data written by other apllications.
		// File file = null;
		// try {
		// file = File.createTempFile("SystemUtil", "SystemUtil");
		// FileUtil.writeStringToFile(file.getAbsolutePath(), s);
		// if (exec("pbcopy < \""+ file.getAbsolutePath() + "\"", false) == 0)
		// return;
		// } catch (IOException e) {
		// // TODO Auto-generated catch block
		// e.printStackTrace();
		// } finally {
		// if (file != null)
		// file.delete();
		// }
		//
		// }
		//
		sysClipboard.setContents(ss, ss);
	}

	/**
	 * Get plain text from clipboard
	 * 
	 * @return
	 */
	public static String getClipboardText() {
		Transferable contents = getTransferable();
		if (contents == null || !contents.isDataFlavorSupported(DataFlavor.stringFlavor))
			return "";
		try {
			return (String) contents.getTransferData(DataFlavor.stringFlavor);
		} catch (Exception ex) {
			return "";
		}
	}

	private static Transferable getTransferable() {
		// To avoid IllegalStateException, we try 25 times to access clipboard.
		for (int i = 0; i < 25; i++) {
			try {
				return sysClipboard.getContents(null);
			} catch (IllegalStateException e) {
				try {
					Thread.sleep(200);
				} catch (InterruptedException e1) {
				}
			}
		}
		throw new RuntimeException("System Clipboard is not ready");
	}

	/**
	 * Execute a script and waiting it for finishing
	 * 
	 * @param content
	 * @return
	 */
	public static int execScript(String content) {
		StringBuffer output = new StringBuffer();
		int code = execScript(content, output, output);
		LOG.info(content + "\n" + "Exit Code: " + code + "\n" + output);
		return code;
	}
	
	/**
	 * Execute a script and waiting it for finishing
	 * @param content
	 * @param output
	 * @param error
	 * @return
	 */
	public static int execScript(String content, StringBuffer output, StringBuffer error) {
		File file = null;
		try {
			file = FileUtil.getUniqueFile(SCRIPT_TEMP_DIR, "tempscript", ".bat");
			FileUtil.writeStringToFile(file.getAbsolutePath(), content);
			String[] cmd;
			if (isWindows())
				cmd = new String[] { file.getAbsolutePath() };
			else
				cmd = new String[] { "sh", file.getAbsolutePath() };
			return exec(cmd, null, null, output, error);
		} catch (Exception e) {
			return -1;
		} finally {
			if (file != null && !file.delete()) 
				file.deleteOnExit();
		}
	}
	
	/**
	 * Start a background process
	 * @param cmd
	 * @param env
	 * @param dir
	 * @param output
	 * @param error
	 * @return
	 */
	public static Process backgroundExec(String[] cmd, String[] env, File dir, StringBuffer output, StringBuffer error) {
		try {
			Process process = Runtime.getRuntime().exec(cmd, env, dir);
			StreamPump inputPump = new StreamPump(output, process.getInputStream());
			StreamPump errorPump = new StreamPump(error, process.getErrorStream());
			inputPump.start();
			errorPump.start();
			return process;
		} catch (Exception e) {
			return null;
		}
	}

	/**
	 * Execute the command and wait for its finishing
	 * @param cmd
	 * @param env
	 * @param dir
	 * @param output
	 * @param error
	 * @return
	 */
	public static int exec(String[] cmd, String[] env, File dir, StringBuffer output, StringBuffer error) {
		Process process = null;
		try {
			LOG.log(Level.FINE,  "exec: " + Arrays.toString(cmd));
			process = Runtime.getRuntime().exec(cmd, env, dir);
		} catch (Exception e) {
			e.printStackTrace();
			return -1;
		}
		
		StreamPump inputPump = new StreamPump(output, process.getInputStream());
		StreamPump errorPump = new StreamPump(error, process.getErrorStream());
		inputPump.start();
		errorPump.start();
		
		try {
			int code = process.waitFor();
			inputPump.join();
			errorPump.join();
			return code;
		} catch (InterruptedException e) {
			return -2;
		}
	}

	public static int exec(String[] cmd, StringBuffer output) {
		return exec(cmd, null, null, output, output);
	}
	
	/**
	 * Make the current thread sleep some seconds.
	 * 
	 * @param second
	 */
	public static void sleep(double second) {
		try {
			if (second > 0)
				Thread.sleep((long) (second * 1000));
		} catch (InterruptedException e) {
		}
	}

	/**
	 * Get Information of running processes
	 * 
	 */
	public static List<HashMap<String, Object>> getProcesses() {
		List<HashMap<String, Object>> ret = new ArrayList<HashMap<String, Object>>();
		try {
			StringBuffer output = new StringBuffer();
			if (isWindows()) {
				File file = new File(SCRIPT_TEMP_DIR, "ps.vbs");
//				if (!file.exists()) {
					String contents = "Set wmi=GetObject(\"Winmgmts:\")\n\r"
							+ "Set ps = wmi.ExecQuery(\"Select * from Win32_Process\")\n\r"
							+ "WScript.Echo \"PID COMMAND\" \n\r"
							+ "For Each p in ps\n\r"
							+ "WScript.Echo p.ProcessId & \" \" & p.CommandLine\n\r"
							+ "Next";
					FileUtil.writeStringToFile(file.getAbsolutePath(), contents);
//				}
				exec(new String[] { "cscript", "//Nologo", file.getAbsolutePath()}, null, null, output, output);
			} else if (isBSD()) {
				exec(new String[] {"ps", "-wwo", "pid,command"}, null, null, output, output);
			} else {
				exec(new String[] {"ps", "-eo", "pid,command"}, null, null, output, output);
			}

			BufferedReader reader = new BufferedReader(new StringReader(output.toString()));
			String line = null;
			reader.readLine();
			while ((line = reader.readLine()) != null) {
				HashMap<String, Object> p = new HashMap<String, Object>();
				StringTokenizer tokenizer = new StringTokenizer(line, " ", true);
				StringBuffer last = new StringBuffer();
				int col = 0;
				while (tokenizer.hasMoreTokens()) {
					String token = tokenizer.nextToken();
					switch (col) {
					case 0:
						if (!" ".equals(token)) {
							// 
							p.put("pid", token);
							col++;
						}
						break;
					default:
						last.append(token);
						break;
					}
				}
				
				p.put("command", last.toString().trim());
				ret.add(p);
			}
		} catch (IOException e) {

		}

		return ret;
	}

	public static void killProcess(String pattern) {
		List<HashMap<String, Object>> processes = SystemUtil.getProcesses();
		for (HashMap<String, Object> p : processes) {
			String command = (String) p.get("command");
			String pid = (String) p.get("pid");
			if (command != null && command.matches(pattern)) {
				if (isWindows()) {
					exec(new String[] { "taskkill", "/F", "/PID", pid }, null, null, null, null);
				} else {
					exec(new String[] { "kill", "-9", pid }, null, null, null, null);
				}
			}
		}
	}
	
	public static boolean hasProcess(String pattern) {
		return findProcess(pattern) != null;
	}
	
	public static HashMap<String, Object> findProcess(String pattern) {
		List<HashMap<String, Object>> processes = SystemUtil.getProcesses();
		for (HashMap<String, Object> p : processes) {
			String command = (String) p.get("command");
			if (command != null && command.matches(pattern)) {
				return p;
			}
		}
		
		return null;
	}
	
	public static List<HashMap<String, Object>> findProcesses(String pattern) {
		List<HashMap<String, Object>> result = new ArrayList<HashMap<String, Object>>();
		List<HashMap<String, Object>> processes = SystemUtil.getProcesses();
		for (HashMap<String, Object> p : processes) {
			String command = (String) p.get("command");
			if (command != null && command.matches(pattern)) {
				result.add(p);
			}
		}
		
		return result;
	}
	
	/**
	 * Get Information of running processes
	 * 
	 */
	public static HashMap<String, Object> getProcessPerfData(String processId) {
		try {
			StringBuffer output = new StringBuffer();
			if (isWindows()) {
				File file = new File(SCRIPT_TEMP_DIR, "pps.vbs");
				String contents = "Set wmi=GetObject(\"Winmgmts:\")\n\r"
						+ "Set pps = wmi.ExecQuery(\"Select * from Win32_PerfFormattedData_PerfProc_Process Where IDProcess='" + processId+"'\")\n\r"
						+ "WScript.Echo \"pcpu PrivateBytes WorkingSet HandleCount\" \n\r"
						+ "For Each pp in pps \n\r"
						+ "WScript.Echo pp.PercentProcessorTime & \" \" & Round(pp.PrivateBytes/1024) & \" \" & Round(pp.WorkingSet/1024) & \" \" & pp.HandleCount \n\r"
						+ "Next";
//				String contents = "var wmi = GetObject(\"Winmgmts:\");\n"
//						+ "var pps = new Enumerator(wmi.ExecQuery(\"Select * from Win32_PerfFormattedData_PerfProc_Process Where IDProcess='" + processId+"'\"));\n"
//						+ "WScript.Echo(\"pcpu rss\");\n"
//						+ "for ( ; !pps.atEnd(); pps.moveNext()) {\n"
//						+ "var pp = pps.item();\n"
//						+ "WScript.Echo(pp.PercentProcessorTime + \" \" + (pp.WorkingSet/1024));\n"
//						+ "}";
				FileUtil.writeStringToFile(file.getAbsolutePath(), contents);
				exec(new String[] { "cscript", "//Nologo", file.getAbsolutePath()}, null, null, output, output);
			} else {
				exec(new String[] {"ps", "-p", processId ,"-o", "pcpu,vsz,rss,tty"}, null, null, output, output);
			}
			BufferedReader reader = new BufferedReader(new StringReader(output.toString()));
			String line = null;
			reader.readLine();
			if ((line = reader.readLine()) != null) {
				HashMap<String, Object> p = new HashMap<String, Object>();
				StringTokenizer tokenizer = new StringTokenizer(line, " ", true);
				int col = 0;
				while (tokenizer.hasMoreTokens()) {
					String token = tokenizer.nextToken();
					switch (col) {
					case 0:
						if (!" ".equals(token)) {
							// 
							p.put("pcpu", Double.parseDouble(token));
							col++;
						}
						break;
					case 1:
						if (!" ".equals(token)) {
							// 
							p.put("vsz", Long.parseLong(token));
							col++;
						}
						break;
					case 2:
						if (!" ".equals(token)) {
							// 
							p.put("rss", Long.parseLong(token));
							col++;
						}
						break;
					case 3:
						if (!" ".equals(token)) {
							//
							try {
								p.put("handles", Long.parseLong(token));
							} catch (Exception e) {
								p.put("handles", 0l);
							}
							
							col++;
						}
						break;
					}
				}
				return p;
			}
		} catch (IOException e) {

		}

		return null;
	}
	
	/**
	 * parse a string to arguments array.
	 * 
	 * @param line
	 * @return
	 */
	public static String[] parseCommandLine(String line) {
		ArrayList<String> arguments = new ArrayList<String>();
		StringTokenizer tokenizer = new StringTokenizer(line, "\"\' ", true);
		int state = 0;
		StringBuffer current = new StringBuffer();
		while (tokenizer.hasMoreTokens()) {
			String token = tokenizer.nextToken();
			switch (state) {
			case 1:
				if ("\'".equals(token)) {
					state = 3;
				} else {
					current.append(token);
				}
				break;
			case 2:
				if ("\"".equals(token)) {
					state = 3;
				} else {
					current.append(token);
				}
				break;
			default:
				if ("\'".equals(token)) {
					state = 1;
				} else if ("\"".equals(token)) {
					state = 2;
				} else if (" ".equals(token)) {
					if (current.length() > 0) {
						arguments.add(current.toString());
						current = new StringBuffer();
					}
				} else {
					current.append(token);
				}
				break;
			}
		}
		if (current.length() > 0)
			arguments.add(current.toString());
		return arguments.toArray(new String[arguments.size()]);
	}

	/**
	 * Get local host's IP
	 * @return
	 */
	public static String getIPAddress() {
		try {
			return InetAddress.getLocalHost().getHostAddress().toString();
		} catch (UnknownHostException e) {
			return null;
		}
	}
	
	/**
	 * Get local host name
	 * @return
	 */
	public static String getHostName() {
		try {
			return InetAddress.getLocalHost().getHostName();
		} catch (UnknownHostException e) {
			return null;
		}
		
	}
	
	public static List<String> getClassesInPackage(String packageName) {
		ArrayList<String> classes = new ArrayList<String>();
		ClassLoader classLoader = Thread.currentThread().getContextClassLoader();
		String path = packageName.replace('.', '/');
		try {
			Enumeration<URL> urls = classLoader.getResources(path);
			while (urls.hasMoreElements()) {
				URL url = urls.nextElement();
				if ("file".equals(url.getProtocol())) {
					findClasses(packageName, new File(url.toURI()), classes);
				} else if ("jar".equals(url.getProtocol())) {
					String urlStr = url.toString();
					int i = urlStr.indexOf('!');
					if (i > 0)
						findClasses(packageName, new URL(urlStr.substring(4, i)), classes);
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
//		TreeSet classes = new TreeSet();
//		for (String directory : dirs) {
//			classes.addAll(findClasses(directory, packageName));
//		}
//		ArrayList classList = new ArrayList();
//		for (String clazz : classes) {
//			classList.add(Class.forName(clazz));
//		}
//		return classList;
		return classes;
	}
	private static void findClasses(String packageName, File dir, List<String> classes) {
		if (!dir.isDirectory())
			return;
		File[] files = dir.listFiles();
		for (File file : files) {
			String name = file.getName();
			if (file.isDirectory()) {
				findClasses(packageName + '.'+ name, file, classes);
			}  else if (name.endsWith(".class")) {
				String className = packageName + '.' + name.substring(0, name.length() - 6);
				classes.add(className);
			}
		}
	}
	
	
	private static void findClasses(String packageName, URL jar, List<String> classes) {
		try {
			ZipInputStream zip = new ZipInputStream(jar.openStream());
			ZipEntry entry;
			while ((entry = zip.getNextEntry()) != null) {
				String name = entry.getName();
				if (name.endsWith(".class")) {
					name = name.replace('/', '.').substring(0, name.length() - 6);
					if (name.startsWith(packageName)) {
						classes.add(name);
					}
				}
			}

		} catch (Exception e) {

		}
	}
}
