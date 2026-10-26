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



package com.sun.star.help;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;
import java.util.zip.CRC32;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.analysis.cjk.CJKAnalyzer;
import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.IndexWriterConfig;
import org.apache.lucene.store.Directory;
import org.apache.lucene.store.NIOFSDirectory;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.file.InvalidPathException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Date;

public class HelpIndexerTool
{
    public HelpIndexerTool()
	{
    }

    /** Converts a path to a java.nio Path. On Windows the path may start with the
        long path prefix \\?\ (as created by osl_getSystemPathFromFileURL); if the
        runtime rejects it, retry without the prefix.
    */
    public static Path toPath( String aPath )
	{
        try
		{
            return Paths.get( aPath );
        }
        catch( InvalidPathException e )
		{
            if( aPath.startsWith( "\\\\?\\" ) )
                return Paths.get( aPath.substring( 4 ) );
            throw e;
        }
    }


    /**
     * @param args the command line arguments
     */
    public static void main( String[] args )
	{
		boolean bExtensionMode = false;
		mainImpl( args, bExtensionMode );
	}

    public static void mainImpl( String[] args, boolean bExtensionMode )
	{
        String aDirToZipStr = "";
        String aSrcDirStr = "";
        String aLanguageStr = "";
        String aModule = "";
        String aTargetZipFileStr = "";

        // Scan arguments
        //If this tool is invoked in the build process for extensions help,
        //then -extension must be set.
        boolean bExtension = false;
        boolean bLang = false;
        boolean bMod = false;
        boolean bZipDir = false;
        boolean bSrcDir = false;
        boolean bOutput = false;
        boolean bCheckIndex = false;

        int nArgCount = args.length;
        for( int i = 0 ; i < nArgCount ; i++ )
		{
            if( "-extension".equals(args[i]) )
			{
                bExtension = true;
            }
            else if( "-lang".equals(args[i]) )
			{
                if( i + 1 < nArgCount )
				{
                    aLanguageStr = args[i + 1];
                    bLang = true;
                }
                i++;
            }
			else if( "-mod".equals(args[i]) )
			{
                if( i + 1 < nArgCount )
				{
                    aModule = args[i + 1];
                    bMod = true;
                }
                i++;
            }
			else if( "-zipdir".equals(args[i]) )
			{
                if( i + 1 < nArgCount )
				{
                    aDirToZipStr = args[i + 1];
                    bZipDir = true;
                }
                i++;
            }
			else if( "-srcdir".equals(args[i]) )
			{
                if( i + 1 < nArgCount )
				{
                    aSrcDirStr = args[i + 1];
                    bSrcDir = true;
                }
                i++;
            }
			else if( "-o".equals(args[i]) )
			{
                if( i + 1 < nArgCount )
				{
                    aTargetZipFileStr = args[i + 1];
                    bOutput = true;
                }
                i++;
            }
			else if( "-checkindex".equals(args[i]) )
			{
                bCheckIndex = true;
            }
        }

        if( !bLang || !bMod || !bZipDir || (!bOutput && !bExtensionMode && !bExtension) )
		{
			if( bExtensionMode )
				return;

			System.out.println("Usage: HelpIndexer -lang ISOLangCode -mod HelpModule -zipdir TempZipDir -o OutputZipFile");
            System.out.println("Usage: HelpIndexer -extension -lang ISOLangCode -mod HelpModule -zipdir PathToLangDir");
			System.exit( -1 );
        }

        String aIndexDirName = aModule + ".idxl";
        File aIndexDir = new File( aDirToZipStr + File.separator + aIndexDirName );
		if( !bSrcDir )
			aSrcDirStr = aDirToZipStr;
        File aCaptionFilesDir = new File( aSrcDirStr + File.separator + "caption" );
        File aContentFilesDir = new File( aSrcDirStr + File.separator + "content" );

        try
		{
            Date start = new Date();
	    Analyzer analyzer = aLanguageStr.equals("ja") ? (Analyzer)new CJKAnalyzer() : (Analyzer)new StandardAnalyzer();
	    // never mix with the files of an index written by another Lucene version
	    deleteRecursively( aIndexDir );
	    IndexWriterConfig aWriterConfig = new IndexWriterConfig( analyzer );
	    aWriterConfig.setOpenMode( IndexWriterConfig.OpenMode.CREATE );
	    aWriterConfig.getMergePolicy().setNoCFSRatio( 1.0 );
	    Directory aLuceneDir = new NIOFSDirectory( toPath( aIndexDir.getPath() ) );
	    IndexWriter writer = new IndexWriter( aLuceneDir, aWriterConfig );
			if( !bExtensionMode )
	            System.out.println( "Lucene: Indexing to directory '" + aIndexDir + "'..." );
            int nRet = indexDocs( writer, aModule, bExtensionMode, aCaptionFilesDir, aContentFilesDir );
            if( nRet != -1 )
			{
				if( !bExtensionMode )
				{
					System.out.println();
					System.out.println( "Optimizing ..." );
				}
                writer.forceMerge( 1 );
            }
            writer.close();
            aLuceneDir.close();
            // the lock file outlives the writer and would end up in the help zip
            new File( aIndexDir, "write.lock" ).delete();

			boolean bCfsFileOk = true;
			boolean bSegmentFileOk = true;
			if( bCheckIndex && !bExtensionMode && nRet != -1 )
			{
				// the segment names depend on the corpus, so count files instead
				int nCfsFiles = 0;
				int nSegmentFiles = 0;
				String[] aIndexFiles = aIndexDir.list();
				for( int i = 0 ; aIndexFiles != null && i < aIndexFiles.length ; i++ )
				{
					if( aIndexFiles[i].endsWith( ".cfs" ) )
						nCfsFiles++;
					else if( aIndexFiles[i].startsWith( "segments_" ) )
						nSegmentFiles++;
				}
				bCfsFileOk = ( nCfsFiles == 1 );
				bSegmentFileOk = ( nSegmentFiles == 1 );
				System.out.println( "Checking cfs file: " + nCfsFiles + " found, 1 expected" );
				System.out.println( "Checking segment file: " + nSegmentFiles + " found, 1 expected" );
			}

			if( bExtensionMode || bExtension)
			{
				if( !bSrcDir )
				{
					deleteRecursively( aCaptionFilesDir );
					deleteRecursively( aContentFilesDir );
				}
			}
			else
			{
				if( nRet == -1 )
					deleteRecursively( aIndexDir );

				if( bCfsFileOk && bSegmentFileOk )
					System.out.println( "Zipping ..." );
				File aDirToZipFile = new File( aDirToZipStr );
				createZipFile( aDirToZipFile, aTargetZipFileStr );
				deleteRecursively( aDirToZipFile );
			}

			if( !bCfsFileOk )
			{
				System.out.println( "cfs file check failed, terminating..." );
				System.exit( -1 );
			}

			if( !bSegmentFileOk )
			{
				System.out.println( "segment file check failed, terminating..." );
				System.exit( -1 );
			}

			Date end = new Date();
			if( !bExtensionMode )
				System.out.println(end.getTime() - start.getTime() + " total milliseconds");
        }
		catch (IOException e)
		{
			if( bExtensionMode )
				return;

			System.out.println(" caught a " + e.getClass() +
				"\n with message: " + e.getMessage());
			System.exit( -1 );
        }
    }

	private static int indexDocs(IndexWriter writer, String aModule, boolean bExtensionMode,
		File aCaptionFilesDir, File aContentFilesDir) throws IOException
	{
        if( !aCaptionFilesDir.canRead() || !aCaptionFilesDir.isDirectory() )
		{
			if( !bExtensionMode )
	            System.out.println( "Not found: " + aCaptionFilesDir );
            return -1;
        }
        if( !aContentFilesDir.canRead() || !aContentFilesDir.isDirectory() )
		{
			if( !bExtensionMode )
	            System.out.println( "Not found: " + aContentFilesDir );
            return -1;
        }

        String[] aCaptionFiles = aCaptionFilesDir.list();
        List aCaptionFilesList = Arrays.asList( aCaptionFiles );
        HashSet aCaptionFilesHashSet = new HashSet( aCaptionFilesList );

        String[] aContentFiles = aContentFilesDir.list();
        List aContentFilesList = Arrays.asList( aContentFiles );
        HashSet aContentFilesHashSet = new HashSet( aContentFilesList );

        // Loop over caption files and find corresponding content file
		if( !bExtensionMode )
	        System.out.println( "Indexing, adding files" );
        int nCaptionFilesLen = aCaptionFiles.length;
        for( int i = 0 ; i < nCaptionFilesLen ; i++ )
		{
            String aCaptionFileStr = aCaptionFiles[i];
            File aCaptionFile = new File( aCaptionFilesDir, aCaptionFileStr );
            File aContentFile = null;
            if( aContentFilesHashSet.contains( aCaptionFileStr ) )
                aContentFile = new File( aContentFilesDir, aCaptionFileStr );

			if( !bExtensionMode )
				System.out.print( "." );
            writer.addDocument( HelpFileDocument.Document( aModule, aCaptionFile, aContentFile ) );
        }

        // Loop over content files to find remaining files not mapped to caption files
        int nContentFilesLen = aContentFiles.length;
        for( int i = 0 ; i < nContentFilesLen ; i++ )
		{
            String aContentFileStr = aContentFiles[i];
            if( !aCaptionFilesHashSet.contains( aContentFileStr ) )
			{
                // Not already handled in caption files loop
                File aCaptionFile = null;
                File aContentFile = new File( aContentFilesDir, aContentFileStr );
				if( !bExtensionMode )
					System.out.print( "." );
                writer.addDocument( HelpFileDocument.Document( aModule, aCaptionFile, aContentFile ) );
            }
        }
        return 0;
    }

    public static void createZipFile( File aDirToZip, String aTargetZipFileStr )
            throws FileNotFoundException, IOException
	{
        FileOutputStream fos = new FileOutputStream( aTargetZipFileStr );
        ZipOutputStream zos = new ZipOutputStream( fos );

        File[] aChildrenFiles = aDirToZip.listFiles();
        int nFileCount = aChildrenFiles.length;
        for( int i = 0 ; i < nFileCount ; i++ )
            addToZipRecursively( zos, aChildrenFiles[i], null );

        zos.close();
    }

    public static void addToZipRecursively( ZipOutputStream zos, File aFile, String aBasePath )
            throws FileNotFoundException, IOException
	{
        if( aFile.isDirectory() )
		{
            String aDirName = aFile.getName();
            if( aDirName.equalsIgnoreCase( "caption" ) || aDirName.equalsIgnoreCase( "content" ) )
                return;

            File[] aChildrenFiles = aFile.listFiles();
            String aNewBasePath = "";
            if( aBasePath != null )
                aNewBasePath += aBasePath + File.separator;
            aNewBasePath += aDirName;

            int nFileCount = aChildrenFiles.length;
            for( int i = 0 ; i < nFileCount ; i++ )
                addToZipRecursively( zos, aChildrenFiles[i], aNewBasePath );

            return;
        }

        // No directory
        // read contents of file we are going to put in the zip
        int fileLength = (int) aFile.length();
        FileInputStream fis = new FileInputStream( aFile );
        byte[] wholeFile = new byte[fileLength];
        int bytesRead = fis.read( wholeFile, 0, fileLength );
        fis.close();

        String aFileName = aFile.getName();
        String aEntryName = "";
        if( aBasePath != null )
            aEntryName += aBasePath + "/";
        aEntryName += aFileName;
        ZipEntry aZipEntry = new ZipEntry( aEntryName );
        aZipEntry.setTime( aFile.lastModified() );
        aZipEntry.setSize( fileLength );

        int nMethod = ( aFileName.toLowerCase().endsWith( ".jar" ) )
                ? ZipEntry.STORED : ZipEntry.DEFLATED;
        aZipEntry.setMethod( nMethod );

        CRC32 tempCRC = new CRC32();
        tempCRC.update( wholeFile, 0, wholeFile.length );
        aZipEntry.setCrc( tempCRC.getValue() );

        // write the contents into the zip element
        zos.putNextEntry( aZipEntry );
        zos.write( wholeFile, 0, fileLength );
        zos.closeEntry();
    }

    static public boolean deleteRecursively( File aFile )
	{
        if( aFile.isDirectory() )
		{
            File[] aChildrenFiles = aFile.listFiles();
            int nFileCount = aChildrenFiles.length;
            for( int i = 0 ; i < nFileCount ; i++ )
			{
                File aChildrenFile = aChildrenFiles[i];
                boolean bSuccess = deleteRecursively( aChildrenFile );
                if( !bSuccess )
                    return false;
            }
        }

        return aFile.delete();
    }
}
