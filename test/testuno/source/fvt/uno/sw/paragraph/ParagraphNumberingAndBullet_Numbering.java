package fvt.uno.sw.paragraph;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import com.sun.star.style.NumberingType;
import com.sun.star.text.*;
import com.sun.star.beans.*;
import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XIndexReplace;
import com.sun.star.frame.XStorable;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;

public class ParagraphNumberingAndBullet_Numbering {
	private static final UnoApp app = new UnoApp();
	XText xText = null;

	@Before
	public void setUp() throws Exception {
		app.start();

	}

	@After
	public void tearDown() throws Exception {
		app.close();
	}
	/*
	 * test paragraph background color
	 * 1.new a text document
	 * 2.insert some text
	 * 3.set paragraph numbering and bullet with numbering
	 * 4.save and close the document
	 * 5.reload the saved document and check the paragraph numbering bullet
	 */	
	@Test
	public void testNumberingBullet_ARABIC() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		//create cursor to select paragraph and formating paragraph
		XTextCursor xTextCursor = xText.createTextCursor();    
		//create paragraph property set
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//create document service factory
		XMultiServiceFactory  xWriterFactory= (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);		
		//set numbering character
		XIndexAccess xNumRule = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,xWriterFactory.createInstance("com.sun.star.text.NumberingRules"));
		PropertyValue[] propsRule = {new PropertyValue()};
		propsRule[0].Name = "NumberingType";
		propsRule[0].Value = NumberingType.ARABIC;
		XIndexReplace xReplaceRule = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule);
		xReplaceRule.replaceByIndex(0, propsRule);  
		//set paragraph numbering and bullet character
		xTextProps.setPropertyValue("NumberingRules", xNumRule);
		//save to odt 
		XStorable xStorable_odt = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_odt = new PropertyValue[2];
		aStoreProperties_odt[0] = new PropertyValue();
		aStoreProperties_odt[1] = new PropertyValue();
		aStoreProperties_odt[0].Name = "Override";
		aStoreProperties_odt[0].Value = true;
		aStoreProperties_odt[1].Name = "FilterName";
		aStoreProperties_odt[1].Value = "writer8";
		xStorable_odt.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.odt")), aStoreProperties_odt);
		//save to doc 
		XStorable xStorable_doc = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_doc = new PropertyValue[2];
		aStoreProperties_doc[0] = new PropertyValue();
		aStoreProperties_doc[1] = new PropertyValue();
		aStoreProperties_doc[0].Name = "Override";
		aStoreProperties_doc[0].Value = true;
		aStoreProperties_doc[1].Name = "FilterName";
		aStoreProperties_doc[1].Value = "MS Word 97";
		xStorable_doc.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.doc")), aStoreProperties_doc);	
		app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		XIndexAccess xNumRule_assert_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_odt.getPropertyValue("NumberingRules"));
		XIndexReplace xReplaceRule_assert_odt = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule_assert_odt);
		PropertyValue[] propsRule_assert_odt=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xReplaceRule_assert_odt.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_odt[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_odt[11].Value);

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		XIndexAccess xNumRule_assert_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_doc.getPropertyValue("NumberingRules"));
		PropertyValue[] propsRule_assert_doc=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xNumRule_assert_doc.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_doc[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_doc[11].Value);
	}
	@Test
	public void testNumberingBullet_ARABIC_Suffix() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		//create cursor to select paragraph and formating paragraph
		XTextCursor xTextCursor = xText.createTextCursor();    
		//create paragraph property set
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//create document service factory
		XMultiServiceFactory  xWriterFactory= (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);		
		//set numbering character
		XIndexAccess xNumRule = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,xWriterFactory.createInstance("com.sun.star.text.NumberingRules"));
		PropertyValue[] propsRule = {new PropertyValue(),new PropertyValue()};
		propsRule[0].Name = "NumberingType";
		propsRule[0].Value = NumberingType.ARABIC;
		propsRule[1].Name = "Suffix";
		propsRule[1].Value = ")";
		XIndexReplace xReplaceRule = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule);
		xReplaceRule.replaceByIndex(0, propsRule);  
		//set paragraph numbering and bullet character
		xTextProps.setPropertyValue("NumberingRules", xNumRule);
		//save to odt 
		XStorable xStorable_odt = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_odt = new PropertyValue[2];
		aStoreProperties_odt[0] = new PropertyValue();
		aStoreProperties_odt[1] = new PropertyValue();
		aStoreProperties_odt[0].Name = "Override";
		aStoreProperties_odt[0].Value = true;
		aStoreProperties_odt[1].Name = "FilterName";
		aStoreProperties_odt[1].Value = "writer8";
		xStorable_odt.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.odt")), aStoreProperties_odt);
		//save to doc 
		XStorable xStorable_doc = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_doc = new PropertyValue[2];
		aStoreProperties_doc[0] = new PropertyValue();
		aStoreProperties_doc[1] = new PropertyValue();
		aStoreProperties_doc[0].Name = "Override";
		aStoreProperties_doc[0].Value = true;
		aStoreProperties_doc[1].Name = "FilterName";
		aStoreProperties_doc[1].Value = "MS Word 97";
		xStorable_doc.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.doc")), aStoreProperties_doc);	
		app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		XIndexAccess xNumRule_assert_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_odt.getPropertyValue("NumberingRules"));
		XIndexReplace xReplaceRule_assert_odt = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule_assert_odt);
		PropertyValue[] propsRule_assert_odt=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xReplaceRule_assert_odt.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_odt[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_odt[11].Value);
		assertEquals("assert numbering and bullet","Suffix",propsRule_assert_odt[3].Name);
		assertEquals("assert numbering and bullet",")",propsRule_assert_odt[3].Value);

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		XIndexAccess xNumRule_assert_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_doc.getPropertyValue("NumberingRules"));
		PropertyValue[] propsRule_assert_doc=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xNumRule_assert_doc.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_doc[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_doc[11].Value);
		assertEquals("assert numbering and bullet","Suffix",propsRule_assert_doc[3].Name);
		assertEquals("assert numbering and bullet",")",propsRule_assert_doc[3].Value);
	}
	@Test
	public void testNumberingBullet_ARABIC_Preffix_Suffix() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		//create cursor to select paragraph and formating paragraph
		XTextCursor xTextCursor = xText.createTextCursor();    
		//create paragraph property set
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//create document service factory
		XMultiServiceFactory  xWriterFactory= (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);		
		//set numbering character
		XIndexAccess xNumRule = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,xWriterFactory.createInstance("com.sun.star.text.NumberingRules"));
		PropertyValue[] propsRule = {new PropertyValue(),new PropertyValue(),new PropertyValue()};
		propsRule[0].Name = "NumberingType";
		propsRule[0].Value = NumberingType.ARABIC;
		propsRule[1].Name = "Suffix";
		propsRule[1].Value = ")";
		propsRule[2].Name = "Prefix";
		propsRule[2].Value = "(";
		XIndexReplace xReplaceRule = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule);
		xReplaceRule.replaceByIndex(0, propsRule);  
		//set paragraph numbering and bullet character
		xTextProps.setPropertyValue("NumberingRules", xNumRule);
		//save to odt 
		XStorable xStorable_odt = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_odt = new PropertyValue[2];
		aStoreProperties_odt[0] = new PropertyValue();
		aStoreProperties_odt[1] = new PropertyValue();
		aStoreProperties_odt[0].Name = "Override";
		aStoreProperties_odt[0].Value = true;
		aStoreProperties_odt[1].Name = "FilterName";
		aStoreProperties_odt[1].Value = "writer8";
		xStorable_odt.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.odt")), aStoreProperties_odt);
		//save to doc 
		XStorable xStorable_doc = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_doc = new PropertyValue[2];
		aStoreProperties_doc[0] = new PropertyValue();
		aStoreProperties_doc[1] = new PropertyValue();
		aStoreProperties_doc[0].Name = "Override";
		aStoreProperties_doc[0].Value = true;
		aStoreProperties_doc[1].Name = "FilterName";
		aStoreProperties_doc[1].Value = "MS Word 97";
		xStorable_doc.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.doc")), aStoreProperties_doc);	
		app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		XIndexAccess xNumRule_assert_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_odt.getPropertyValue("NumberingRules"));
		XIndexReplace xReplaceRule_assert_odt = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule_assert_odt);
		PropertyValue[] propsRule_assert_odt=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xReplaceRule_assert_odt.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_odt[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_odt[11].Value);
		assertEquals("assert numbering and bullet","Suffix",propsRule_assert_odt[3].Name);
		assertEquals("assert numbering and bullet",")",propsRule_assert_odt[3].Value);
		assertEquals("assert numbering and bullet","Prefix",propsRule_assert_odt[2].Name);
		assertEquals("assert numbering and bullet","(",propsRule_assert_odt[2].Value);

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		XIndexAccess xNumRule_assert_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_doc.getPropertyValue("NumberingRules"));
		PropertyValue[] propsRule_assert_doc=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xNumRule_assert_doc.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_doc[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_doc[11].Value);
		assertEquals("assert numbering and bullet","Suffix",propsRule_assert_doc[3].Name);
		assertEquals("assert numbering and bullet",")",propsRule_assert_doc[3].Value);
		assertEquals("assert numbering and bullet","Prefix",propsRule_assert_odt[2].Name);
		assertEquals("assert numbering and bullet","(",propsRule_assert_odt[2].Value);
	}
	@Test
	public void testNumberingBullet_ROMAN_LOWER() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		//create cursor to select paragraph and formating paragraph
		XTextCursor xTextCursor = xText.createTextCursor();    
		//create paragraph property set
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//create document service factory
		XMultiServiceFactory  xWriterFactory= (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);		
		//set numbering character
		XIndexAccess xNumRule = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,xWriterFactory.createInstance("com.sun.star.text.NumberingRules"));
		PropertyValue[] propsRule = {new PropertyValue()};
		propsRule[0].Name = "NumberingType";
		propsRule[0].Value = NumberingType.ROMAN_LOWER;
		XIndexReplace xReplaceRule = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule);
		xReplaceRule.replaceByIndex(0, propsRule);  
		//set paragraph numbering and bullet character
		xTextProps.setPropertyValue("NumberingRules", xNumRule);
		//save to odt 
		XStorable xStorable_odt = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_odt = new PropertyValue[2];
		aStoreProperties_odt[0] = new PropertyValue();
		aStoreProperties_odt[1] = new PropertyValue();
		aStoreProperties_odt[0].Name = "Override";
		aStoreProperties_odt[0].Value = true;
		aStoreProperties_odt[1].Name = "FilterName";
		aStoreProperties_odt[1].Value = "writer8";
		xStorable_odt.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.odt")), aStoreProperties_odt);
		//save to doc 
		XStorable xStorable_doc = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_doc = new PropertyValue[2];
		aStoreProperties_doc[0] = new PropertyValue();
		aStoreProperties_doc[1] = new PropertyValue();
		aStoreProperties_doc[0].Name = "Override";
		aStoreProperties_doc[0].Value = true;
		aStoreProperties_doc[1].Name = "FilterName";
		aStoreProperties_doc[1].Value = "MS Word 97";
		xStorable_doc.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.doc")), aStoreProperties_doc);	
		app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		XIndexAccess xNumRule_assert_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_odt.getPropertyValue("NumberingRules"));
		XIndexReplace xReplaceRule_assert_odt = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule_assert_odt);
		PropertyValue[] propsRule_assert_odt=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xReplaceRule_assert_odt.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_odt[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ROMAN_LOWER,propsRule_assert_odt[11].Value);

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		XIndexAccess xNumRule_assert_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_doc.getPropertyValue("NumberingRules"));
		PropertyValue[] propsRule_assert_doc=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xNumRule_assert_doc.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_doc[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ROMAN_LOWER,propsRule_assert_doc[11].Value);
	}
	@Test
	public void testNumberingBullet_ROMAN_UPPER() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		//create cursor to select paragraph and formating paragraph
		XTextCursor xTextCursor = xText.createTextCursor();    
		//create paragraph property set
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//create document service factory
		XMultiServiceFactory  xWriterFactory= (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);		
		//set numbering character
		XIndexAccess xNumRule = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,xWriterFactory.createInstance("com.sun.star.text.NumberingRules"));
		PropertyValue[] propsRule = {new PropertyValue()};
		propsRule[0].Name = "NumberingType";
		propsRule[0].Value = NumberingType.ROMAN_UPPER;
		XIndexReplace xReplaceRule = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule);
		xReplaceRule.replaceByIndex(0, propsRule);  
		//set paragraph numbering and bullet character
		xTextProps.setPropertyValue("NumberingRules", xNumRule);
		//save to odt 
		XStorable xStorable_odt = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_odt = new PropertyValue[2];
		aStoreProperties_odt[0] = new PropertyValue();
		aStoreProperties_odt[1] = new PropertyValue();
		aStoreProperties_odt[0].Name = "Override";
		aStoreProperties_odt[0].Value = true;
		aStoreProperties_odt[1].Name = "FilterName";
		aStoreProperties_odt[1].Value = "writer8";
		xStorable_odt.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.odt")), aStoreProperties_odt);
		//save to doc 
		XStorable xStorable_doc = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_doc = new PropertyValue[2];
		aStoreProperties_doc[0] = new PropertyValue();
		aStoreProperties_doc[1] = new PropertyValue();
		aStoreProperties_doc[0].Name = "Override";
		aStoreProperties_doc[0].Value = true;
		aStoreProperties_doc[1].Name = "FilterName";
		aStoreProperties_doc[1].Value = "MS Word 97";
		xStorable_doc.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.doc")), aStoreProperties_doc);	
		app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		XIndexAccess xNumRule_assert_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_odt.getPropertyValue("NumberingRules"));
		XIndexReplace xReplaceRule_assert_odt = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule_assert_odt);
		PropertyValue[] propsRule_assert_odt=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xReplaceRule_assert_odt.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_odt[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ROMAN_UPPER,propsRule_assert_odt[11].Value);

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		XIndexAccess xNumRule_assert_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_doc.getPropertyValue("NumberingRules"));
		PropertyValue[] propsRule_assert_doc=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xNumRule_assert_doc.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_doc[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ROMAN_UPPER,propsRule_assert_doc[11].Value);
	}
	@Test@Ignore("Bug #120826 - [testUNO patch]A,B,C numbering bullet will change to A,AA,AAA when save to doc.")
	public void testNumberingBullet_CHARS_UPPER_LETTER() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		//create cursor to select paragraph and formating paragraph
		XTextCursor xTextCursor = xText.createTextCursor();    
		//create paragraph property set
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//create document service factory
		XMultiServiceFactory  xWriterFactory= (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);		
		//set numbering character
		XIndexAccess xNumRule = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,xWriterFactory.createInstance("com.sun.star.text.NumberingRules"));
		PropertyValue[] propsRule = {new PropertyValue(),new PropertyValue()};
		propsRule[0].Name = "NumberingType";
		propsRule[0].Value = NumberingType.CHARS_UPPER_LETTER;
		propsRule[1].Name = "Suffix";
		propsRule[1].Value = ")";
		XIndexReplace xReplaceRule = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule);
		xReplaceRule.replaceByIndex(0, propsRule);  
		//set paragraph numbering and bullet character
		xTextProps.setPropertyValue("NumberingRules", xNumRule);
		//save to odt 
		XStorable xStorable_odt = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_odt = new PropertyValue[2];
		aStoreProperties_odt[0] = new PropertyValue();
		aStoreProperties_odt[1] = new PropertyValue();
		aStoreProperties_odt[0].Name = "Override";
		aStoreProperties_odt[0].Value = true;
		aStoreProperties_odt[1].Name = "FilterName";
		aStoreProperties_odt[1].Value = "writer8";
		xStorable_odt.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.odt")), aStoreProperties_odt);
		//save to doc 
		XStorable xStorable_doc = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_doc = new PropertyValue[2];
		aStoreProperties_doc[0] = new PropertyValue();
		aStoreProperties_doc[1] = new PropertyValue();
		aStoreProperties_doc[0].Name = "Override";
		aStoreProperties_doc[0].Value = true;
		aStoreProperties_doc[1].Name = "FilterName";
		aStoreProperties_doc[1].Value = "MS Word 97";
		xStorable_doc.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.doc")), aStoreProperties_doc);	
		app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		XIndexAccess xNumRule_assert_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_odt.getPropertyValue("NumberingRules"));
		XIndexReplace xReplaceRule_assert_odt = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule_assert_odt);
		PropertyValue[] propsRule_assert_odt=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xReplaceRule_assert_odt.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_odt[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.CHARS_UPPER_LETTER,propsRule_assert_odt[11].Value);
		assertEquals("assert numbering and bullet","Suffix",propsRule_assert_odt[3].Name);
		assertEquals("assert numbering and bullet",")",propsRule_assert_odt[3].Value);

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		XIndexAccess xNumRule_assert_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_doc.getPropertyValue("NumberingRules"));
		PropertyValue[] propsRule_assert_doc=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xNumRule_assert_doc.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_doc[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.CHARS_UPPER_LETTER,propsRule_assert_doc[11].Value);
		assertEquals("assert numbering and bullet","Suffix",propsRule_assert_doc[3].Name);
		assertEquals("assert numbering and bullet",")",propsRule_assert_doc[3].Value);
	}
	@Test@Ignore("Bug #120826 - [testUNO patch]A,B,C numbering bullet will change to A,AA,AAA when save to doc.")
	public void testNumberingBullet_CHARS_LOWER_LETTER_Suffix() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		//create cursor to select paragraph and formating paragraph
		XTextCursor xTextCursor = xText.createTextCursor();    
		//create paragraph property set
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//create document service factory
		XMultiServiceFactory  xWriterFactory= (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);		
		//set numbering character
		XIndexAccess xNumRule = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,xWriterFactory.createInstance("com.sun.star.text.NumberingRules"));
		PropertyValue[] propsRule = {new PropertyValue(),new PropertyValue()};
		propsRule[0].Name = "NumberingType";
		propsRule[0].Value = NumberingType.CHARS_LOWER_LETTER;
		propsRule[1].Name = "Suffix";
		propsRule[1].Value = ")";
		XIndexReplace xReplaceRule = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule);
		xReplaceRule.replaceByIndex(0, propsRule);  
		//set paragraph numbering and bullet character
		xTextProps.setPropertyValue("NumberingRules", xNumRule);
		//save to odt 
		XStorable xStorable_odt = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_odt = new PropertyValue[2];
		aStoreProperties_odt[0] = new PropertyValue();
		aStoreProperties_odt[1] = new PropertyValue();
		aStoreProperties_odt[0].Name = "Override";
		aStoreProperties_odt[0].Value = true;
		aStoreProperties_odt[1].Name = "FilterName";
		aStoreProperties_odt[1].Value = "writer8";
		xStorable_odt.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.odt")), aStoreProperties_odt);
		//save to doc 
		XStorable xStorable_doc = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_doc = new PropertyValue[2];
		aStoreProperties_doc[0] = new PropertyValue();
		aStoreProperties_doc[1] = new PropertyValue();
		aStoreProperties_doc[0].Name = "Override";
		aStoreProperties_doc[0].Value = true;
		aStoreProperties_doc[1].Name = "FilterName";
		aStoreProperties_doc[1].Value = "MS Word 97";
		xStorable_doc.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.doc")), aStoreProperties_doc);	
		app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		XIndexAccess xNumRule_assert_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_odt.getPropertyValue("NumberingRules"));
		XIndexReplace xReplaceRule_assert_odt = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule_assert_odt);
		PropertyValue[] propsRule_assert_odt=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xReplaceRule_assert_odt.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_odt[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.CHARS_LOWER_LETTER,propsRule_assert_odt[11].Value);
		assertEquals("assert numbering and bullet","Suffix",propsRule_assert_odt[3].Name);
		assertEquals("assert numbering and bullet",")",propsRule_assert_odt[3].Value);

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		XIndexAccess xNumRule_assert_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_doc.getPropertyValue("NumberingRules"));
		PropertyValue[] propsRule_assert_doc=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xNumRule_assert_doc.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_doc[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.CHARS_LOWER_LETTER,propsRule_assert_doc[11].Value);
		assertEquals("assert numbering and bullet","Suffix",propsRule_assert_doc[3].Name);
		assertEquals("assert numbering and bullet",")",propsRule_assert_doc[3].Value);
	}
	@Test@Ignore("Bug #120826 - [testUNO patch]A,B,C numbering bullet will change to A,AA,AAA when save to doc.")
	public void testNumberingBullet_CHARS_LOWER_LETTER_Suffix_Prefix() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		//create cursor to select paragraph and formating paragraph
		XTextCursor xTextCursor = xText.createTextCursor();    
		//create paragraph property set
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//create document service factory
		XMultiServiceFactory  xWriterFactory= (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);		
		//set numbering character
		XIndexAccess xNumRule = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,xWriterFactory.createInstance("com.sun.star.text.NumberingRules"));
		PropertyValue[] propsRule = {new PropertyValue(),new PropertyValue(),new PropertyValue()};
		propsRule[0].Name = "NumberingType";
		propsRule[0].Value = NumberingType.CHARS_LOWER_LETTER;
		propsRule[1].Name = "Suffix";
		propsRule[1].Value = ")";
		propsRule[2].Name = "Prefix";
		propsRule[2].Value = "(";
		XIndexReplace xReplaceRule = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule);
		xReplaceRule.replaceByIndex(0, propsRule);  
		//set paragraph numbering and bullet character
		xTextProps.setPropertyValue("NumberingRules", xNumRule);
		//save to odt 
		XStorable xStorable_odt = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_odt = new PropertyValue[2];
		aStoreProperties_odt[0] = new PropertyValue();
		aStoreProperties_odt[1] = new PropertyValue();
		aStoreProperties_odt[0].Name = "Override";
		aStoreProperties_odt[0].Value = true;
		aStoreProperties_odt[1].Name = "FilterName";
		aStoreProperties_odt[1].Value = "writer8";
		xStorable_odt.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.odt")), aStoreProperties_odt);
		//save to doc 
		XStorable xStorable_doc = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_doc = new PropertyValue[2];
		aStoreProperties_doc[0] = new PropertyValue();
		aStoreProperties_doc[1] = new PropertyValue();
		aStoreProperties_doc[0].Name = "Override";
		aStoreProperties_doc[0].Value = true;
		aStoreProperties_doc[1].Name = "FilterName";
		aStoreProperties_doc[1].Value = "MS Word 97";
		xStorable_doc.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.doc")), aStoreProperties_doc);	
		app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		XIndexAccess xNumRule_assert_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_odt.getPropertyValue("NumberingRules"));
		XIndexReplace xReplaceRule_assert_odt = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule_assert_odt);
		PropertyValue[] propsRule_assert_odt=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xReplaceRule_assert_odt.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_odt[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.CHARS_LOWER_LETTER,propsRule_assert_odt[11].Value);
		assertEquals("assert numbering and bullet","Suffix",propsRule_assert_odt[3].Name);
		assertEquals("assert numbering and bullet",")",propsRule_assert_odt[3].Value);
		assertEquals("assert numbering and bullet","Prefix",propsRule_assert_odt[2].Name);
		assertEquals("assert numbering and bullet","(",propsRule_assert_odt[2].Value);

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		XIndexAccess xNumRule_assert_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_doc.getPropertyValue("NumberingRules"));
		PropertyValue[] propsRule_assert_doc=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xNumRule_assert_doc.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_doc[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.CHARS_LOWER_LETTER,propsRule_assert_doc[11].Value);
		assertEquals("assert numbering and bullet","Suffix",propsRule_assert_doc[3].Name);
		assertEquals("assert numbering and bullet",")",propsRule_assert_doc[3].Value);
		assertEquals("assert numbering and bullet","Prefix",propsRule_assert_doc[2].Name);
		assertEquals("assert numbering and bullet","(",propsRule_assert_doc[2].Value);
	}
}
