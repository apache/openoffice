package org.openoffice.test.common;

import java.io.File;
import java.io.IOException;
import java.text.MessageFormat;
import java.util.logging.Level;

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
			File packFile = null;
			if (FileUtil.isUrl(prop)) {
				String url = FileUtil.readFileAsString(downloadUrl);
				if (!prop.equals(url)) {
					FileUtil.deleteFile(downloadDir);
					downloadDir.mkdirs();
					packFile = FileUtil.download(prop, downloadDir);
					if (packFile == null)
						throw new RuntimeException(MessageFormat.format("{0} can not be downloaded!", prop));
					FileUtil.writeStringToFile(downloadUrl, prop);
				} else {
					packFile = FileUtil.download(prop, downloadDir, true);
					if (packFile == null)
						throw new RuntimeException(MessageFormat.format("{0} can not be downloaded!", prop));
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
					if (SystemUtil.exec(new String[]{"tar", "-zxpf", packFile.getAbsolutePath(), "-C", installTempDir.getAbsolutePath()}, output) != 0)
						throw new RuntimeException(MessageFormat.format("{0} can not be installed! Cause: {1}" , packFile, output));
				} else {
					if (!FileUtil.unzip(packFile, installTempDir))
						throw new RuntimeException(MessageFormat.format("{0} can not be installed!", packFile));
				}
				// On windows, if path is too long, openoffice can not be started.
				File[] files = installTempDir.listFiles();
				if (files != null && files.length == 1 && files[0].isDirectory()) {
					files[0].renameTo(installDir);
				}
				File sofficeBin = FileUtil.findFile(installDir, "soffice.bin");
				if (sofficeBin == null) 
					throw new RuntimeException(MessageFormat.format("{0} is not a valid openoffice installation package!" , packFile));
				try {
					String openofficeHome = sofficeBin.getParentFile().getParentFile().getCanonicalPath();
					log.log(Level.INFO, MessageFormat.format("{0} is installed to {1}", prop, openofficeHome));
					System.setProperty("openoffice.home", openofficeHome);
				} catch (IOException e) {
					//ignore, never occurs
				}
		
			} finally {
				FileUtil.deleteFile(installTempDir);
			}
		}
	}

	public static void main(String... args) {
		new Installer().run();
	}
}
