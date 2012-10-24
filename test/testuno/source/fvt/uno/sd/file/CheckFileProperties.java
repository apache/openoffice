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
package fvt.uno.sd.file;
import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.openoffice.test.uno.UnoApp;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;

import com.sun.star.beans.IllegalTypeException;
import com.sun.star.beans.Property;
import com.sun.star.beans.PropertyAttribute;
import com.sun.star.beans.PropertyExistException;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertyContainer;
import com.sun.star.beans.XPropertySet;
import com.sun.star.beans.XPropertySetInfo;
import com.sun.star.container.XNameAccess;
import com.sun.star.document.XDocumentProperties;
import com.sun.star.document.XDocumentPropertiesSupplier;
import java.util.Calendar;
import com.sun.star.util.DateTime;
import com.sun.star.util.Date;
import com.sun.star.util.Duration;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;

/**
 * @author LouQL
 *
 */
public class CheckFileProperties {

	private static final UnoApp app = new UnoApp();

	private XComponent m_xSDComponent = null;
	private static String m_filePath = null;

	@Before
	public void setUpDocument() throws Exception {
		if (FileUtil.fileExists(m_filePath)) {//load
			m_xSDComponent = app.loadDocument(m_filePath);
		} else {//new
			m_xSDComponent = (XComponent) UnoRuntime.queryInterface(
					XComponent.class, app.newDocument("simpress"));			
		}		
	}
	
	private String getUserName() throws com.sun.star.uno.Exception
	{
		Object configurationProvider = app.getServiceFactory().
				createInstance("com.sun.star.configuration.ConfigurationProvider");
		
		XMultiServiceFactory msFac = (XMultiServiceFactory)UnoRuntime.queryInterface(
				XMultiServiceFactory.class, configurationProvider);
		
		PropertyValue[] propValue = new PropertyValue[1];
		propValue[0] = new PropertyValue();
		propValue[0].Name = "nodepath";
		propValue[0].Value = "/org.openoffice.UserProfile/Data";
		
		Object configurationAccess = msFac.createInstanceWithArguments(
				"com.sun.star.configuration.ConfigurationAccess", propValue);
		XNameAccess nameAcc = (XNameAccess)UnoRuntime.queryInterface(XNameAccess.class, configurationAccess);
		String givenname = (String)nameAcc.getByName("givenname");
		String sn = (String)nameAcc.getByName("sn");	
		String name = null;
		if(givenname.length() == 0) name = sn;
		else	name = givenname+" "+sn;
				
		return name;
	}
	
	private XDocumentProperties getDocumentProperties(){
		XDocumentPropertiesSupplier xDocumentProSupplier = (XDocumentPropertiesSupplier)UnoRuntime.queryInterface(
				XDocumentPropertiesSupplier.class, this.m_xSDComponent);
		return xDocumentProSupplier.getDocumentProperties();
	}

	@After
	public void tearDownDocument() {
		app.closeDocument(m_xSDComponent);

	}

	@BeforeClass
	public static void setUpConnection() throws Exception {
		app.start();
		m_filePath = Testspace.getPath("temp/CheckFileProperties.odp");	
		FileUtil.deleteFile(m_filePath);
	}

	@AfterClass
	public static void tearDownConnection() throws InterruptedException,
			Exception {
		app.close();
		//remove the temp file
		FileUtil.deleteFile(Testspace.getPath("temp"));
	}

	/*
	 * UI entry: File->Properties->General->Created*/
	@Test
	public void testGeneralAuthor() throws Exception {
		String author = getUserName();
		XDocumentProperties xDocPro = getDocumentProperties();		
		xDocPro.setAuthor(author);

		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();
		assertEquals("Author should be "+ author, author, xDocPro2.getAuthor());		
	}
		
	private boolean DateTimeEquals(DateTime datetime1, DateTime datetime2){
		
		if(datetime1.Seconds == datetime2.Seconds &&
				datetime1.Minutes == datetime2.Minutes &&
				datetime1.Hours == datetime2.Hours &&
				datetime1.Day == datetime2.Day &&
				datetime1.Month == datetime2.Month &&
				datetime1.Year == datetime2.Year)
			return true;
		else
			return false;
	}
	
	private boolean DateEquals(Date date1, Date date2){
		
		if(date1.Day == date2.Day &&
		   date1.Month == date2.Month &&
			date1.Year == date2.Year)
			return true;
		else
			return false;
	}
	
	private boolean DurationEquals(Duration d1, Duration d2){
		
		if(d1.Seconds == d2.Seconds &&
				d1.Minutes == d2.Minutes &&
				d1.Hours == d2.Hours &&
				d1.Days == d2.Days &&
				d1.Months == d2.Months &&
				d1.Years == d2.Years)
			return true;
		else
			return false;
	}

	private DateTime getCurrentDateTime(){
		Calendar ca = Calendar.getInstance();		
		DateTime currentDateTime = new DateTime();
		currentDateTime.Year = (short)ca.get(Calendar.YEAR);
		currentDateTime.Month = (short)ca.get(Calendar.MONTH);
		currentDateTime.Day = (short)ca.get(Calendar.DATE);
		currentDateTime.Minutes = (short)ca.get(Calendar.MINUTE);
		currentDateTime.Hours = (short)ca.get(Calendar.HOUR);
		currentDateTime.Seconds = (short)ca.get(Calendar.SECOND);
		
		return currentDateTime;
	}
	
	private Date getCurrentDate(){
		Calendar ca = Calendar.getInstance();		
		Date currentDate = new Date();
		currentDate.Year = (short)ca.get(Calendar.YEAR);
		currentDate.Month = (short)ca.get(Calendar.MONTH);
		currentDate.Day = (short)ca.get(Calendar.DATE);
		
		return currentDate;
	}
	
	/*
	 * UI entry: File->Properties->General->Created*/
	@Test
	public void testGeneralCreationDate() throws Exception {			
		DateTime creationDate = getCurrentDateTime();	
		
		XDocumentProperties xDocPro = getDocumentProperties();
		
		xDocPro.setCreationDate(creationDate);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();
		DateTime result = xDocPro2.getCreationDate();
		assertTrue("CreationDate should be the same as set", this.DateTimeEquals(creationDate, result));		
	}
	
	/*
	 * UI entry: File->Properties->General->Modified*/
	@Test
	//ModifiedBy will be set each time the file loaded. The value is the one set in Tools->options->User data->Last name
	public void testGeneralModifiedBy() throws Exception {
		String modifiedBy = this.getUserName();		
		XDocumentProperties xDocPro = getDocumentProperties();
		xDocPro.setModifiedBy(modifiedBy);
		
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();		
		assertEquals("The file is modified by "+ modifiedBy, modifiedBy, xDocPro2.getModifiedBy());
	}
	
	/*
	 * UI entry: File->Properties->General->Modified*/
	@Test
	public void testGeneralModificationDate() throws Exception {
		//modification date will be set each time the file saved, so I don't save after set.
		DateTime modificationDate = getCurrentDateTime();	
				
		XDocumentProperties xDocPro = getDocumentProperties();
		
		xDocPro.setModificationDate(modificationDate);
			
		DateTime result = xDocPro.getModificationDate();
		assertTrue("ModificationDate should be the same as set", this.DateTimeEquals(modificationDate, result));		
	}
	
	/*
	 * UI entry: File->Properties->General->Last printed*/
	@Test
	public void testGeneralPrintBy() throws Exception {
		String printBy = "PrintBy";
		XDocumentProperties xDocPro = getDocumentProperties();
		
		xDocPro.setPrintedBy(printBy);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();
		assertEquals("This document is printed by "+ printBy, printBy, xDocPro2.getPrintedBy());		
	}
	
	/*
	 * UI entry: File->Properties->General->Last printed*/
	@Test
	public void testGeneralPrintDate() throws Exception {	
		DateTime printDate = getCurrentDateTime();	
		
		XDocumentProperties xDocPro = getDocumentProperties();
		
		xDocPro.setPrintDate(printDate);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();
		DateTime result = xDocPro2.getPrintDate();
		assertTrue("PrintDate should be the same as set", this.DateTimeEquals(printDate, result));		
	}
	
	/*
	 * UI entry: File->Properties->General->Total editing time*/
	@Test
	public void testGeneralEditingDuration() throws Exception {
		int editingDuration = 60;
		
		XDocumentProperties xDocPro = getDocumentProperties();
		
		xDocPro.setEditingDuration(editingDuration);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();
		assertEquals("Totally editing time should be "+ editingDuration, editingDuration, xDocPro2.getEditingDuration());		
	}
	
	/*
	 * UI entry: File->Properties->General->Revision number*/
	@Test
	public void testGeneralRevisionNumber() throws Exception {
		short revisionNumber = 10;
		
		XDocumentProperties xDocPro = getDocumentProperties();
		
		xDocPro.setEditingCycles(revisionNumber);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();
		assertEquals("Revision number should be "+ revisionNumber, revisionNumber, xDocPro2.getEditingCycles());		
	}
	
	/*
	 * UI entry: File->Properties->General->template*/
	@Test
	public void testGeneralTemplateName() throws Exception {
		String templateName = "I'm a template";
		
		XDocumentProperties xDocPro = getDocumentProperties();
		
		xDocPro.setTemplateName(templateName);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();
		assertEquals("Template name should be "+ templateName, templateName, xDocPro2.getTemplateName());		
	}
	
	/*
	 * UI entry: File->Properties->General->Reset*/
	@Test
	public void testGeneralReset() throws Exception {
		String author = "ResetAuthor";
		XDocumentProperties xDocPro = getDocumentProperties();
		xDocPro.resetUserData(author);
			
		assertEquals("Author should be "+ author, author, xDocPro.getAuthor());
		assertEquals("Modified should be empty", "", xDocPro.getModifiedBy());
		assertTrue("ModificationDate should be empty", 
				DateTimeEquals(new DateTime(), xDocPro.getModificationDate()));
		assertEquals("PrintBy should be empty", "", xDocPro.getPrintedBy());
		assertTrue("PrintDate should be empty", 
				DateTimeEquals(new DateTime(), xDocPro.getPrintDate()));
		assertEquals("Totally editing time should be empty", 0, xDocPro.getEditingDuration());
		assertEquals("Revision number should be empty", 1, xDocPro.getEditingCycles());
	}
	
	// UI entry: File->Properties->General->Apply user data
	
	// UI entry: File->Properties->General->digital signature
	
	//Description begin
	/*
	 * UI entry: File->Properties->Description->Title*/
	@Test
	public void testDescriptionTitle() throws Exception{
		String title = "titleForTest";
		XDocumentProperties xDocPro = getDocumentProperties();
		xDocPro.setTitle(title);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();
		assertEquals("Title should be "+ title, title, xDocPro2.getTitle());
	}
	
	/*
	 * UI entry: File->Properties->Description->Subject*/
	@Test
	public void testDescriptionSubject() throws Exception{
		String subject = "subjectForTest";
		XDocumentProperties xDocPro = getDocumentProperties();
		xDocPro.setSubject(subject);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();
		assertEquals("Subject should be "+ subject, subject, xDocPro2.getSubject());
	}
	
	/*
	 * UI entry: File->Properties->Description->Keywords*/
	@Test
	public void testDescriptionKeywords() throws Exception{
		String[] keywords = {"keyword1", "keyword2"};
		XDocumentProperties xDocPro = getDocumentProperties();
		xDocPro.setKeywords(keywords);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();
		String[] keywordsResult = xDocPro2.getKeywords();
		assertEquals("There should be 2 Keywords", 2, keywordsResult.length);
		for(int i=0;i<keywordsResult.length;i++)
		{
			String num = Integer.toString(i+1);
			assertEquals("The keywords should be keyword"+num, "keyword"+num, keywordsResult[i]);
		}
	}
	
	/*
	 * UI entry: File->Properties->Description->Comments*/
	@Test
	public void testDescriptionComments() throws Exception{
		String comments = "This is the comment.";
		XDocumentProperties xDocPro = getDocumentProperties();
		xDocPro.setDescription(comments);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		
		m_xSDComponent = app.loadDocument(m_filePath);
		XDocumentProperties xDocPro2 = getDocumentProperties();
		
		assertEquals("Comments should be "+comments, comments, xDocPro2.getDescription());
	}
	//Description end
	
	//custom properties begin
	//UI entry: File->Properties->Custom properties
		private void addCustomPro(String propertyName, Object value) throws PropertyExistException, IllegalTypeException, IllegalArgumentException{
			XDocumentProperties xDocPro = getDocumentProperties();
			XPropertyContainer proContainer = xDocPro.getUserDefinedProperties();
			proContainer.addProperty(propertyName, PropertyAttribute.REMOVEABLE, value);
		}
		
		private Object getCustomPro(String propertyName) throws UnknownPropertyException, WrappedTargetException{
			XDocumentProperties xDocPro = getDocumentProperties();
			XPropertyContainer proContainer = xDocPro.getUserDefinedProperties();
			XPropertySet xProSet = (XPropertySet)UnoRuntime.queryInterface(
					XPropertySet.class, proContainer);
			
			return xProSet.getPropertyValue(propertyName);
		}
		
		@Test
		public void testCustomAddPro_Text() throws Exception{	
			String addedProName = "TextPro";
			String addedProDefaultValue = "testUser";
			
			addCustomPro(addedProName, addedProDefaultValue);
			
			app.saveDocument(m_xSDComponent, m_filePath);
			app.closeDocument(m_xSDComponent);
			
			m_xSDComponent = app.loadDocument(m_filePath);
			
			String result = (String)getCustomPro(addedProName);
					
			assertTrue("added Text property \""+addedProName+"\" should exist", result != null);		
			assertEquals("value of added property should be "+addedProDefaultValue, 
					addedProDefaultValue, result);
		}
		
		@Test
		public void testCustomAddPro_DateTime() throws Exception{	
			String addedProName = "DateTimePro";
			DateTime addedProDefaultValue = getCurrentDateTime();	
			
			addCustomPro(addedProName, addedProDefaultValue);
			
			app.saveDocument(m_xSDComponent, m_filePath);
			app.closeDocument(m_xSDComponent);
			
			m_xSDComponent = app.loadDocument(m_filePath);
					
			DateTime result = (DateTime)getCustomPro(addedProName);
			assertTrue("added DateTime property \""+addedProName+"\" should exist", result != null);		
			assertTrue("value of added property should be the same as set", 
					this.DateTimeEquals(result, addedProDefaultValue));
		}
		
		@Test
		public void testCustomAddPro_Date() throws Exception{	
			String addedProName = "DatePro";
			Date addedProDefaultValue = getCurrentDate();	

			addCustomPro(addedProName, addedProDefaultValue);
			
			app.saveDocument(m_xSDComponent, m_filePath);
			app.closeDocument(m_xSDComponent);
			
			m_xSDComponent = app.loadDocument(m_filePath);
					
			Date result = (Date)getCustomPro(addedProName);
			assertTrue("added Date property \""+addedProName+"\" should exist", result != null);		
			assertTrue("value of added property should be the same as set", 
					this.DateEquals(result, addedProDefaultValue));
		}
		
		@Test
		public void testCustomAddPro_Duration() throws Exception{			
			String addedProName = "DurationPro";
			Duration addedProDefaultValue = new Duration();	
			addedProDefaultValue.Days = 1;

			addCustomPro(addedProName, addedProDefaultValue);
			
			app.saveDocument(m_xSDComponent, m_filePath);
			app.closeDocument(m_xSDComponent);
			
			m_xSDComponent = app.loadDocument(m_filePath);
					
			Duration result = (Duration)getCustomPro(addedProName);
			assertTrue("added Date property \""+addedProName+"\" should exist", result != null);		
			assertTrue("value of added property should the same as set", DurationEquals(addedProDefaultValue, result));
		}
		
		@Test
		public void testCustomAddPro_Number() throws Exception{	
			String addedProName = "NumberPro";
			Double addedProDefaultValue = (double)10;	
			
			addCustomPro(addedProName, addedProDefaultValue);
			
			app.saveDocument(m_xSDComponent, m_filePath);
			app.closeDocument(m_xSDComponent);
			
			m_xSDComponent = app.loadDocument(m_filePath);
				
			Object oResult = getCustomPro(addedProName);
			
			Double result = (Double)oResult;
			assertTrue("added Number property \""+addedProName+"\" should exist", oResult != null);		
			assertEquals("value of added property should be "+Double.toString(addedProDefaultValue), 
					addedProDefaultValue, result);
		}
		
		@Test
		public void testCustomAddPro_Boolean() throws Exception{	
			String addedProName = "BooleanPro";
			Boolean addedProDefaultValue = true;	
			
			addCustomPro(addedProName, addedProDefaultValue);
			
			app.saveDocument(m_xSDComponent, m_filePath);
			app.closeDocument(m_xSDComponent);
			
			m_xSDComponent = app.loadDocument(m_filePath);
				
			Object oResult = getCustomPro(addedProName);
			
			boolean result = (Boolean)oResult;
			assertTrue("added Number property \""+addedProName+"\" should exist", oResult != null);		
			assertEquals("value of added property should be "+Boolean.toString(addedProDefaultValue), 
					addedProDefaultValue, result);
		}
		
		@Test
		public void testCustomRemovePro() throws Exception{					
			XDocumentProperties xDocPro = getDocumentProperties();
			XPropertyContainer proContainer = xDocPro.getUserDefinedProperties();
			XPropertySet xProSet = (XPropertySet)UnoRuntime.queryInterface(
					XPropertySet.class, proContainer);
			XPropertySetInfo xproSetInfo = xProSet.getPropertySetInfo();
			Property[] pros = xproSetInfo.getProperties();
			
			if(pros.length == 0) //if there is no custom property, add one
			{
				addCustomPro("testPro", "value");
			}
			
			for(int i=0; i< pros.length;i++)
			{
				proContainer.removeProperty(pros[i].Name);
			}
			
			app.saveDocument(m_xSDComponent, m_filePath);
			app.closeDocument(m_xSDComponent);
			
			m_xSDComponent = app.loadDocument(m_filePath);
				
			XDocumentProperties xDocPro2 = getDocumentProperties();
			XPropertyContainer proContainer2 = xDocPro2.getUserDefinedProperties();
			XPropertySet xProSet2 = (XPropertySet)UnoRuntime.queryInterface(
					XPropertySet.class, proContainer2);
			XPropertySetInfo xproSetInfo2 = xProSet2.getPropertySetInfo();
			Property[] pros2 = xproSetInfo2.getProperties();
					
			assertEquals("number of custom property should be zero ", 
					0, pros2.length);
		}	
	//custom properties end
		
		//Internet begin
		private void setAutoLoad(String URL, int secs) throws IllegalArgumentException
		{
			XDocumentProperties xDocPro = getDocumentProperties();
			xDocPro.setAutoloadURL(URL);
			xDocPro.setAutoloadSecs(secs);
			xDocPro.setDefaultTarget("_blank");
		}
		
		@Test
		public void testNoRefresh() throws Exception{
			String autoLoadURL = "";
			int autoLoadSecs = 0;
			setAutoLoad(autoLoadURL, autoLoadSecs);
			
			app.saveDocument(m_xSDComponent, m_filePath);
			app.closeDocument(m_xSDComponent);
			
			m_xSDComponent = app.loadDocument(m_filePath);
			XDocumentProperties xDocPro2 = getDocumentProperties();
			
			assertEquals("AutoLoadURL should be empty", autoLoadURL, xDocPro2.getAutoloadURL());
			assertEquals("AutoLoadSecs should be 0", autoLoadSecs, xDocPro2.getAutoloadSecs());
		}
		
		@Test
		public void testRefreshEvery60Secs() throws Exception{
			String autoLoadURL = "";
			int autoLoadSecs = 60;
			setAutoLoad(autoLoadURL, autoLoadSecs);
			
			app.saveDocument(m_xSDComponent, m_filePath);
			app.closeDocument(m_xSDComponent);
			
			m_xSDComponent = app.loadDocument(m_filePath);
			XDocumentProperties xDocPro2 = getDocumentProperties();
			
			assertEquals("AutoLoadURL should be empty", autoLoadURL, xDocPro2.getAutoloadURL());
			assertEquals("AutoLoadSecs should be "+Integer.toString(autoLoadSecs), autoLoadSecs, xDocPro2.getAutoloadSecs());
		}
		
		@Test
		public void testRedirect() throws Exception{
			String autoLoadURL = "http://www.openoffice.com/";
			int autoLoadSecs = 5;
			setAutoLoad(autoLoadURL, autoLoadSecs);
			
			app.saveDocument(m_xSDComponent, m_filePath);
			app.closeDocument(m_xSDComponent);
			
			m_xSDComponent = app.loadDocument(m_filePath);
			XDocumentProperties xDocPro2 = getDocumentProperties();
			
			assertEquals("AutoLoadURL should be empty", autoLoadURL, xDocPro2.getAutoloadURL());
			assertEquals("AutoLoadSecs should be "+Integer.toString(autoLoadSecs), autoLoadSecs, xDocPro2.getAutoloadSecs());
		}
		//Internet end
}
