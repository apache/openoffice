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

/**
 * 
 */
package bvt.gui;

import static org.openoffice.test.common.Testspace.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.ListIterator;
import java.util.concurrent.TimeoutException;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.Reader;
import java.io.InputStreamReader;
import java.lang.RuntimeException;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Condition;
import org.openoffice.test.common.DataSheet;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.vcl.widgets.VclDialog;


public class FileExport {
	private static class TestType {

		public TestType(boolean doc, boolean spread, boolean slide) {
			documentT = doc;
			spreadsheetT = spread;
			slideT = slide;
		}

		public boolean documentT;
		public boolean spreadsheetT;
		public boolean slideT;
	};

	private static class ContinuePoint {
		public String path;
		public int i;

		ContinuePoint() {
			path = "";
			i = 0;
		}
	}

	// for example
	// the path is "D:\\aoo\\utaoo\\testspace\\ooxmlsamples"
	String samplespath = "";// a dir
	String outpath = ""; // a dir
	static double timeout = 100;
	static double interval = 0.1;
	double sleeptime = 1;
	boolean bContinue = false;// if failed,next execute from the continue point
	TestType atest = new TestType(true, true, true);// doc,spreadsheet,slide
	ContinuePoint thepoint = null;
	BufferedWriter fwContinue = null;
	String testedlogfile = "";

	private static DataSheet result;
	private String scenario = null;
	private File sourceFile = null;

	public static final VclDialog passwdDlg = dialog("");

	public List<String> recursionfiles(File path, List<String> resultFileName) {
		File[] files = path.listFiles();
		if (files == null)
			return resultFileName;
		for (File f : files) {
			if (f.isDirectory()) {// a path
				if (!f.isHidden() && !f.getName().startsWith(".")) {
					sampledirs.add(f.getPath());
					recursionfiles(f, resultFileName);
				}
			} else {// a file
				if (!f.isHidden() && !f.getName().startsWith(".")) {
					String apath = f.getPath();

					int sepIndex = apath.indexOf(File.separatorChar);
					String userpath = apath.substring(sepIndex);
					String newpath = outpath + userpath;

					File file = new File(newpath);
					File parent = file.getParentFile();
					if (parent != null && !parent.exists()) {
						parent.mkdirs();
					}
					resultFileName.add(f.getPath());
				}
			}
		}
		return resultFileName;
	}

	private String getrealoutpath(String p) {
		String apath = p;

		int sepIndex = apath.indexOf(File.separatorChar);
		int sepIndexLast = apath.lastIndexOf(File.separatorChar);
		String userpath = apath.substring(sepIndex, sepIndexLast);
		String newpath = outpath + userpath;
		File tempFolderFile = new File(newpath);
		if (!tempFolderFile.exists()) {
			tempFolderFile.mkdirs();
		}
		return newpath;
	}

	private List<String> samplelist = null;
	private List<String> sampledirs = null;

	@Rule
	public Logger log = Logger.getLogger(this);

	@BeforeClass
	public static void beforeClass() {
		app.clean();
	}

	@AfterClass
	public static void afterClass() {
		app.stop();
	}

	@Before
	public void before() {

	}

	@After
	public void after() throws Exception {
		app.stop();
	}

	void getcontinuepoint() {

		if (bContinue == false) {
			thepoint.path = "";
			thepoint.i = 0;
			return;
		}
		File ftestedlog = new File(testedlogfile);
		Reader reader = null;
		try {
			reader = new InputStreamReader(new FileInputStream(ftestedlog));
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		BufferedReader br = new BufferedReader(reader);

		String line = null;
		int countline = 0;
		int m = 0;
		try {
			if ((line = br.readLine()) != null) {
				if (countline == 0) {
					thepoint.path = line;
				} else {

					m = Integer.parseInt(line);
					if (m > 0)
						thepoint.i = m;
				}
			}
		} catch (NumberFormatException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
	}

	/**
	 * Test Open/SaveAs ooxml file by Aoo
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSaveAs() throws Exception {
		samplelist = new ArrayList<String>();
		sampledirs = new ArrayList<String>();
		thepoint = new ContinuePoint();
		File spacepath = new File(Testspace.getPath());
		File absspath = spacepath.getAbsoluteFile();
		String abspre = absspath.getParent();

		result = new DataSheet(getFile("outputlog" + File.separatorChar
				+ FileExport.class.getName() + ".xml"));
		result.addRow("data", "File Path", "File Size", "Scenario",
				"Exported File Path", "Exported File Size", "Result", "Error");

		testedlogfile = abspre + "testgui" + File.separatorChar + "cases_tested.txt";
		samplespath = "samples";
		
		if (outpath.length() == 0) {
			File workspacepath = Testspace.getFile("output");// ..\\testspace\\output
			outpath = workspacepath.getAbsolutePath();

			// outpath = "D:\\AOOautomation\\Docs sample files\\out";
		}

		if (bContinue)
			getcontinuepoint();

		File samplesDir = Testspace.getFile(samplespath);
		recursionfiles(samplesDir, samplelist);
		ListIterator<String> it = sampledirs.listIterator();

		boolean bstartfromthis = false;
		while (it.hasNext()) {

			String str = (String) it.next();
			if (!bContinue) {
				File afiledir = new File(str);
				dotest(afiledir);
			} else {
				File file = new File(thepoint.path);
				File parent = file.getParentFile();
				if (parent != null) {
					String pathbegin = parent.getAbsolutePath();
					if (pathbegin.equalsIgnoreCase(str)) {
						bstartfromthis = true;

					}
				}
				if (bstartfromthis == true) {
					File afiledir = new File(str);
					dotest(afiledir);
				}
			}
		}
	}

	public void dotest(File samplesDir) throws Exception {
		FilenameFilter testFilter = new FilenameFilter() {
			public boolean accept(File file, String name) {
				if (name.endsWith(".doc") || name.endsWith(".docx")
						|| name.endsWith(".dot") || name.endsWith(".xls")
						|| name.endsWith(".xlsx") || name.endsWith(".ods")
						|| name.endsWith(".ppt") || name.endsWith(".pptx")
						|| name.endsWith(".odp")) {
					// filters files
					return true;
				} else {
					return false;
				}
			}
		};
		File[] files = samplesDir.listFiles(testFilter);
		Arrays.sort(files);
		int nfiles = files.length;
		if (nfiles == 0)
			return;

		int i = thepoint.i;
		for (; i < nfiles; i++) {
			File afile = files[i];
			String path = afile.getAbsolutePath();

			String extName = FileUtil.getFileExtName(path).toLowerCase();
			boolean bShouldTest = false;
			if (extName.equals("doc") || extName.equals("docx")
					|| extName.equals("odt")) {
				bShouldTest = true;
				if (atest.documentT == false)
					continue;
			}
			if (extName.equals("ppt") || extName.equals("pptx")
					|| extName.equals("odp")) {
				bShouldTest = true;
				if (atest.slideT == false)
					continue;
			}
			if (extName.equals("xls") || extName.equals("xlsx")
					|| extName.equals("ods")) {
				bShouldTest = true;
				if (atest.spreadsheetT == false)
					continue;
			}
			if (!bShouldTest)
				continue;
			String exportname = "aoo_" + afile.getName();

			sourceFile = new File(path);

			app.stop();
			app.start();

			if(!Open(path)){
				continue;
			}

			String newpath = getrealoutpath(path);

			// do testing
			if (!savetosameformat(exportname, newpath)) {
				continue;
			}

			if(!Open(path)) {
				continue;

			}
			if (!savetodiffformat(exportname, newpath)) {
				continue;
			}

			if(!Open(path)) {
				continue;

			}

			if (!savetopdfformat(exportname, newpath)) {
				continue;
			}			
		}
	}

	private boolean Open(String path) throws Exception {
		try {
			open(path);
			if (!app.exists())
				throw new RuntimeException();
			HandleBlockers(false);
			if(statusBar.exists(timeout))
				statusBar.waitForEnabled(timeout, interval);
			else
				throw new TimeoutException("time out");
			HandleBlockers(false);
			if (!app.exists())
				throw new RuntimeException();
			return true;
		} catch (Exception e) {
			try {
				String reason = e.getMessage();
				if (reason == null || reason.isEmpty())
					reason = "Opening";
				result.addRow("data", sourceFile.getCanonicalPath(),
						sourceFile.length(), scenario, "", "", "Fail", reason);
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			return false;
		}
	}

	private boolean savetosameformat(String file, String outpath) {
		try {
			File reportDir = Testspace.getFile(outpath);

			String extName = FileUtil.getFileExtName(file).toLowerCase();

			boolean formatchanged = false;
			if (extName.equals("docx")) {
				extName = "doc";
				formatchanged = true;
			} else if (extName.equals("pptx")) {
				extName = "ppt";
				formatchanged = true;
			} else if (extName.equals("xlsx")) {
				extName = "xls";
				formatchanged = true;
			}
			
			scenario = FileUtil.getFileExtName(file).toLowerCase() + " to " + extName;

			int dotIndex = file.lastIndexOf(".");
			String pre = file.substring(0, dotIndex + 1);
			String newfile = pre + extName;

			String saveTo = reportDir + File.separator + file;
			if (formatchanged)
				saveTo = reportDir + File.separator + newfile;
			// Save the text document
			deleteFile(saveTo);
			SaveAs(saveTo);
			Close();
			if(!Open(saveTo))
				return false;

			String exception = "";
			String resultflag = "";
			try {
				Close();
				resultflag = "Pass";
			} catch (Exception e) {
				exception = e.getMessage();
				resultflag = "Fail";
			}

			File targetFile = new File(saveTo);
			result.addRow("data", sourceFile.getCanonicalPath(),
					sourceFile.length(), scenario, saveTo, targetFile.length(),
					resultflag, exception);

			return true;
		} catch (Exception e) {
			try {
				String exception = e.getMessage();
				if (exception == null || exception.isEmpty())
					exception = "Saving to the same format";
				result.addRow("data", sourceFile.getCanonicalPath(),
						sourceFile.length(), scenario, "", "", "Fail", exception);
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}

			return false;
		}
	}

	private boolean savetodiffformat(String file, String outpath) {
		try {
			File reportDir = Testspace.getFile(outpath);

			String extName = FileUtil.getFileExtName(file).toLowerCase();

			String targetExtName = null;

			if (extName.equals("doc") || extName.equals("docx"))
				targetExtName = "odt";
			else if (extName.equals("ppt") || extName.equals("pptx"))
				targetExtName = "odp";
			else if (extName.equals("xls") || extName.equals("xlsx"))
				targetExtName = "ods";
			else if (extName.equals("odt"))
				targetExtName = "doc";
			else if (extName.equals("odp"))
				targetExtName = "ppt";
			else if (extName.equals("ods"))
				targetExtName = "xls";
			
			scenario = extName + " to " + targetExtName;
			
			int dotIndex = file.lastIndexOf(".");
			String pre = file.substring(0, dotIndex + 1);
			String saveTo = reportDir + File.separator + pre + targetExtName;
			deleteFile(saveTo);
			// long base = System.currentTimeMillis();
			SaveAs(saveTo);
			Close();
			if(!Open(saveTo))
				return false;

			String exception = "";
			String resultflag = "";
			try {
				Close();
				resultflag = "Pass";
			} catch (Exception e) {
				exception = e.getMessage();
				resultflag = "Fail";
			}

			File targetFile = new File(saveTo);
			result.addRow("data", sourceFile.getCanonicalPath(),
					sourceFile.length(), scenario, saveTo, targetFile.length(),
					resultflag, exception);

			return true;
		} catch (Exception e) {
			try {
				String exception = e.getMessage();
				if (exception == null || exception.isEmpty())
					exception = "Saving to a different format";
				result.addRow("data", sourceFile.getCanonicalPath(),
						sourceFile.length(), scenario, "", "", "Fail", exception);
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}

			return false;
		}

	}

	private void Close() throws Exception {
		close();
		HandleBlockers(false);
	}

	public static void HandleBlockers(final boolean Positive) throws Exception {
		new Condition() {
			@Override
			public boolean value() {
				while (activeMsgBox.exists()) {

					String context = activeMsgBox.getMessage();
					if (context.toLowerCase().indexOf("has been modified") >= 0
							&& context.toLowerCase().indexOf(
									"do you want to save your changes") >= 0)
						throw new RuntimeException("A wrong dirty flag");
					if (context.toLowerCase().indexOf("read-error") >= 0)
						throw new RuntimeException("Read Error");
					if (context.toLowerCase().indexOf("does not exist") >= 0)
						throw new RuntimeException("File not exist");

					try {
						if (Positive)
							activeMsgBox.ok();
						else
							activeMsgBox.no();
					} catch (Exception e) {
						try {
							if (Positive)
								activeMsgBox.yes();
							else
								activeMsgBox.no();
						} catch (Exception e1) {
							try {
								activeMsgBox.doDefault();
							} catch (Exception e2) {
								try {
									activeMsgBox.ok();
								} catch (Exception e3) {
									activeMsgBox.yes();
								}
							}
						}
					}
				}
				if (passwdDlg.exists()) {
					String caption = passwdDlg.getCaption();
					if (caption.toLowerCase().indexOf(
							"enter password to open file") >= 0)
						throw new RuntimeException("A password protected file");
					if (caption.toLowerCase().indexOf("properties") >= 0)
						throw new RuntimeException("An unsupported format");
					if (SupportedFormats(caption))
						throw new RuntimeException("An unreadable file");
				}
				return true;
			}
		
		}.test(timeout, interval);
	}

	private static boolean SupportedFormats(String filename) {
		if (filename.endsWith(".doc") || filename.endsWith(".docx")
				|| filename.endsWith(".dot") || filename.endsWith(".xls")
				|| filename.endsWith(".xlsx") || filename.endsWith(".ods")
				|| filename.endsWith(".ppt") || filename.endsWith(".pptx")
				|| filename.endsWith(".odp")) {
			return true;
		} else {
			return false;
		}
	}

	private void SaveAs(String newfile) throws Exception {
		saveAs(newfile);
		HandleBlockers(true);
		if(statusBar.exists(timeout))
			statusBar.waitForEnabled(timeout, interval);
		else
			throw new TimeoutException("time out");
	}

	private boolean savetopdfformat(String file, String outpath) {
		try {
			File reportDir = Testspace.getFile(outpath);
			String extName = "pdf";

			int dotIndex = file.lastIndexOf(".");
			String pre = file.substring(0, dotIndex + 1);
			String newfile = pre + extName;

			scenario = FileUtil.getFileExtName(file).toLowerCase() + " to pdf";
			
			String saveTo = reportDir + File.separator + newfile;
			// Save the text document
			app.dispatch(".uno:ExportToPDF");
			pdfGeneralPage.ok();

			submitSaveDlg(saveTo);
			HandleBlockers(true);
			
			if(statusBar.exists(timeout))
				statusBar.waitForEnabled(timeout, interval);
			else
				throw new TimeoutException("time out");

			String outcome = "Pass";
			try {
				Close();
			} catch (Exception e) {
				if (!e.getMessage().matches("A wrong dirty flag"))
					outcome = e.getMessage();
				else
					throw e;
			}

			
			File targetFile = new File(saveTo);
			result.addRow("data", sourceFile.getCanonicalPath(),
					sourceFile.length(), scenario, saveTo, targetFile.length(),
					outcome);

			return true;
		} catch (Exception e) {
			try {
				String reason = e.getMessage();
				if (reason == null || reason.isEmpty())
					reason = "Exporting to pdf format";
				result.addRow("data", sourceFile.getCanonicalPath(),
						sourceFile.length(), scenario, "", "", "Fail", reason);
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}

			return false;
		}
	}

}
