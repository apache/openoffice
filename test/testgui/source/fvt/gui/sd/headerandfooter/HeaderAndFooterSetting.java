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
package fvt.gui.sd.headerandfooter;

import static org.junit.Assert.*;
import static org.openoffice.test.common.Testspace.*;
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;

import testlib.gui.AppTool;
import testlib.gui.SDTool;

public class HeaderAndFooterSetting {

	@Before
	public void setUp() throws Exception {
		app.start(true); 
		AppTool.newPresentation();
		for (int i = 0; i < 5; i++) {
			sdInsertPageButtonOnToolbar.click();
		}
		// Pop up navigator panel
		if (!sdNavigatorDlg.exists()) {
			app.dispatch(".uno:Navigator");
		}
	}

	@After
	public void tearDown() throws Exception {
		sleep(3);
		if (sdNavigatorDlg.exists()) {
			app.dispatch(".uno:Navigator");
		}
		app.stop();
	}

	/**
	 * Test Copy slide with Apply Footer to same file and different file
	 * 
	 * @throws Exception
	 */
	@Test
	public void testCopySlideWithApplyFooter() throws Exception {

		// add header and footer
		app.dispatch(".uno:HeaderAndFooter");

		sdDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeOnSlideInput.setText("Fix Date: 20120323");
		sdFooterTextOnSlide.check();
		sdFooterTextOnSlideInput.setText("Footer Test");
		sdSlideNumAsFooterOnSlide.check();
		sdApplyToAllButtonOnSlideFooter.click();

		// Click slide 3
		impressSlideSorter.focus();
		typeKeys("<up><up><up>");
		sleep(1);
		app.dispatch(".uno:HeaderAndFooter");
		sdSlideNumAsFooterOnSlide.uncheck();
		sdApplyButtonOnSlideFooter.click();

		// paste to the same file
		impressSlideSorter.focus();
		app.dispatch(".uno:Copy");
		app.dispatch(".uno:Paste");

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(false, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterDlgSlideTab.cancel();

		// paste to different file
		impress.focus();
		app.dispatch("private:factory/simpress?slot=6686");
		presentationWizard.ok();
		app.dispatch(".uno:Paste");
		impressSlideSorter.focus();
		typeKeys("<down>");
		sleep(1);

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(false, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterDlgSlideTab.cancel();
		
	}

	/**
	 * Test Copy slide with Apply to all Footer to same file and different file
	 * 
	 * @throws Exception
	 */
	@Test
	public void testCopySlideWithApplyToAllFooter() throws Exception {

		// add header and footer
		app.dispatch(".uno:HeaderAndFooter");

		sdDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeOnSlideInput.setText("Fix Date: 20120323");
		sdFooterTextOnSlide.check();
		sdFooterTextOnSlideInput.setText("Footer Test");
		sdSlideNumAsFooterOnSlide.check();
		sdApplyToAllButtonOnSlideFooter.click();

		impressSlideSorter.focus();
		for (int j = 0; j <= 2; j++) {
			typeKeys("<up>");
		}
		app.dispatch(".uno:HeaderAndFooter");
		sdSlideNumAsFooterOnSlide.uncheck();
		sdApplyButtonOnSlideFooter.click();

		// paste to the same file
		impressSlideSorter.focus();
		typeKeys("<up>");
		app.dispatch(".uno:Copy");
		typeKeys("<down>");
		app.dispatch(".uno:Paste");

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterDlgSlideTab.cancel();

		// paste to different file
		app.dispatch("private:factory/simpress?slot=6686");
		presentationWizard.ok();
		app.dispatch(".uno:Paste");
		impressSlideSorter.focus();
		typeKeys("<down>");
		sleep(1); // If no sleep, error occur

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterDlgSlideTab.cancel();
	}

	/**
	 * Test Copy slide with Notes Footer to same file and different file
	 * 
	 * @throws Exception
	 */
	@Test
	public void testCopySlideWithNotesHeaderFooter() throws Exception {

		// add header and footer
		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		sdHeaderTextOnNotes.check();
		sdHeaderTextOnNotesInput.setText("Header Test");
		sdDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeOnSlideInput.setText("Fix Date: 20120329");
		sdFooterTextOnSlide.check();
		sdFooterTextOnSlideInput.setText("Footer Test");
		sdSlideNumAsFooterOnSlide.check();
		sdApplyToAllButtonOnSlideFooter.click();

		// paste to the same file
		impressSlideSorter.focus();
		typeKeys("<up>");
		app.dispatch(".uno:Copy");
		typeKeys("<down>");
		app.dispatch(".uno:Paste");

		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		assertEquals("Header Test", sdHeaderTextOnNotesInput.getText());
		assertEquals("Fix Date: 20120329", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterOnNotesTabPage.cancel();

		// paste to different file
		impress.focus();
		app.dispatch("private:factory/simpress?slot=6686");
		presentationWizard.ok();
		app.dispatch(".uno:Paste");
		impressSlideSorter.focus();
		typeKeys("<down>");
		SDTool.getActiveView().activate();
		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		assertEquals("Header Test", sdHeaderTextOnNotesInput.getText());
		assertEquals("Fix Date: 20120329", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterOnNotesTabPage.cancel();
	}

	/**
	 * Test duplicate slide with Apply to all Footer to same file
	 * 
	 * @throws Exception
	 */
	@Test
	public void testDuplicateSlideWithApplyToAllFooter() throws Exception {

		// add header and footer
		app.dispatch(".uno:HeaderAndFooter");

		sdDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeOnSlideInput.setText("Fix Date: 20120323");
		sdFooterTextOnSlide.check();
		sdFooterTextOnSlideInput.setText("Footer Test");
		sdSlideNumAsFooterOnSlide.check();
		sdApplyToAllButtonOnSlideFooter.click();

		impressSlideSorter.focus();
		for (int j = 0; j <= 2; j++) {
			typeKeys("<up>");
		}
		app.dispatch(".uno:DuplicatePage");
		app.dispatch(".uno:HeaderAndFooter");
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterDlgSlideTab.cancel();
	}

	/**
	 * Test footer not show on the first slide.
	 * 
	 * @throws Exception
	 */
	@Test
	public void testFooterNotShowOn1stSlide() throws Exception {

		// add header and footer
		app.dispatch(".uno:HeaderAndFooter");
		sdFooterTextOnSlide.check();
		sdFooterTextOnSlideInput.setText("Footer Test");
		sdFooterNotShowOn1stSlide.check();
		sdApplyToAllButtonOnSlideFooter.click();

		// Check slide 1
		impressSlideSorter.focus();
		// typeKeys("<up><up><up><up><up>"); // Not stable on ubuntu10.04
		// sleep(1); // If no sleep, error occur
		for (int i = 0; i < 5; i++) {
			typeKeys("<up>");
			sleep(1);
		}

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals(true, sdFooterNotShowOn1stSlide.isChecked());
		assertEquals(false, sdFooterTextOnSlide.isChecked());
		sdHeaderAndFooterDlgSlideTab.cancel();
	}

	/**
	 * Test Insert Footer to focus slide.
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertApplyFooterOnSlide() throws Exception {

		// add header and footer to focus slide.
		app.dispatch(".uno:HeaderAndFooter");

		sdDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeOnSlideInput.setText("Fix Date: 20120323");
		sdFooterTextOnSlide.check();
		sdFooterTextOnSlideInput.setText("Footer Test");
		sdSlideNumAsFooterOnSlide.check();
		sdApplyButtonOnSlideFooter.click();

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals(true, sdDateAndTimeFooterOnSlide.isChecked());
		assertEquals(true, sdFixedDateAndTimeFooterOnSlide.isChecked());
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals(true, sdFooterTextOnSlide.isChecked());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		// close header and footer dialog.
		sdApplyButtonOnSlideFooter.focus();
		typeKeys("<tab>");
		typeKeys("<enter>");
		// end close

		impressSlideSorter.focus();
		typeKeys("<up>");
		sleep(1); // If no sleep, error occur

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals("", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("", sdFooterTextOnSlideInput.getText());
		assertEquals(false, sdSlideNumAsFooterOnSlide.isChecked());
		// close header and footer dialog.
		sdApplyButtonOnSlideFooter.focus();
		typeKeys("<tab>");
		typeKeys("<enter>");
		// end close

		sdInsertPageButtonOnToolbar.click();
		app.dispatch(".uno:HeaderAndFooter");
		assertEquals("", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("", sdFooterTextOnSlideInput.getText());
		assertEquals(false, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterDlgSlideTab.cancel();
	}

	/**
	 * Test Insert Footer to Notes View
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertApplyToAllFooterOnNotes() throws Exception {

		// add header and footer to focus slide.
		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		sdHeaderTextOnNotes.check();
		sdHeaderTextOnNotesInput.setText("Header Test");
		sdDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeOnSlideInput.setText("Fix Date: 20120323");
		sdFooterTextOnSlide.check();
		sdFooterTextOnSlideInput.setText("Footer Test");
		sdSlideNumAsFooterOnSlide.check();
		sdApplyToAllButtonOnSlideFooter.click();

		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		assertEquals(true, sdHeaderTextOnNotes.isChecked());
		assertEquals("Header Test", sdHeaderTextOnNotesInput.getText());
		assertEquals(true, sdDateAndTimeFooterOnSlide.isChecked());
		assertEquals(true, sdFixedDateAndTimeFooterOnSlide.isChecked());
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals(true, sdFooterTextOnSlide.isChecked());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		// close header and footer dialog.
		sdApplyToAllButtonOnSlideFooter.focus();
		typeKeys("<tab>");
		typeKeys("<enter>");
		// end close

		impressSlideSorter.focus();
		typeKeys("<up>");

		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		assertEquals(true, sdHeaderTextOnNotes.isChecked());
		assertEquals("Header Test", sdHeaderTextOnNotesInput.getText());
		assertEquals(true, sdDateAndTimeFooterOnSlide.isChecked());
		assertEquals(true, sdFixedDateAndTimeFooterOnSlide.isChecked());
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals(true, sdFooterTextOnSlide.isChecked());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		// close header and footer dialog.
		sdApplyToAllButtonOnSlideFooter.focus();
		typeKeys("<tab>");
		typeKeys("<enter>");
		// end close

		sdInsertPageButtonOnToolbar.click();
		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		assertEquals(true, sdHeaderTextOnNotes.isChecked());
		assertEquals("Header Test", sdHeaderTextOnNotesInput.getText());
		assertEquals(true, sdDateAndTimeFooterOnSlide.isChecked());
		assertEquals(true, sdFixedDateAndTimeFooterOnSlide.isChecked());
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals(true, sdFooterTextOnSlide.isChecked());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterOnNotesTabPage.cancel();
	}

	/**
	 * Test Insert Footer to Slide, use Apply to All
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertApplyToAllFooterOnSlide() throws Exception {

		// add header and footer to focus slide.
		app.dispatch(".uno:HeaderAndFooter");

		sdDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeOnSlideInput.setText("Fix Date: 20120323");
		sdFooterTextOnSlide.check();
		sdFooterTextOnSlideInput.setText("Footer Test");
		sdSlideNumAsFooterOnSlide.check();
		sdApplyToAllButtonOnSlideFooter.click();

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals(true, sdDateAndTimeFooterOnSlide.isChecked());
		assertEquals(true, sdFixedDateAndTimeFooterOnSlide.isChecked());
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals(true, sdFooterTextOnSlide.isChecked());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		// close header and footer dialog.
		sdApplyButtonOnSlideFooter.focus();
		typeKeys("<tab>");
		typeKeys("<enter>");
		// end close

		impressSlideSorter.focus();
		typeKeys("<up>");

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals(true, sdDateAndTimeFooterOnSlide.isChecked());
		assertEquals(true, sdFixedDateAndTimeFooterOnSlide.isChecked());
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals(true, sdFooterTextOnSlide.isChecked());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		// close header and footer dialog.
		sdApplyButtonOnSlideFooter.focus();
		typeKeys("<tab>");
		typeKeys("<enter>");
		// end close

		sdInsertPageButtonOnToolbar.click();
		app.dispatch(".uno:HeaderAndFooter");
		assertEquals(true, sdDateAndTimeFooterOnSlide.isChecked());
		assertEquals(true, sdFixedDateAndTimeFooterOnSlide.isChecked());
		assertEquals("Fix Date: 20120323", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals(true, sdFooterTextOnSlide.isChecked());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterDlgSlideTab.cancel();
	}

	/**
	 * Test Insert update automatically time footer to slides.
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertAutoUpdateTimeFooter() throws Exception {

		// add header and footer
		app.dispatch(".uno:HeaderAndFooter");
		sdAutoUpdateTimeFooter.check();
		sdAutoUpdateTimeFooterType.select(7);
		String currentTime = sdAutoUpdateTimeFooterType.getItemText(7);
		sdApplyToAllButtonOnSlideFooter.click();
		sleep(1);

		app.dispatch(".uno:HeaderAndFooter");
		String updatedTime = sdAutoUpdateTimeFooterType.getItemText(7);
		assertNotSame("Time can not update", currentTime, updatedTime);
		sdHeaderAndFooterDlgSlideTab.cancel();
	}

	/**
	 * Test Insert update automatically time footer to Notes view.
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertAutoUpdateTimeFooterOnNotes() throws Exception {

		// add header and footer
		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		sdAutoUpdateTimeFooter.check();
		String currentTime = sdAutoUpdateTimeFooterType.getItemText(7);
		sdApplyToAllButtonOnSlideFooter.click();
		sleep(1); 

		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		String updatedTime = sdAutoUpdateTimeFooterType.getItemText(7);

		assertNotSame("Time can not update", currentTime, updatedTime);
		sdHeaderAndFooterOnNotesTabPage.cancel();
	}

	/**
	 * Test Insert Header and Footer to Notes view. Save and ReOpen
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertHeaderFooterOnNotes() throws Exception {

		// add header and footer
		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		sdHeaderTextOnNotes.check();
		sdHeaderTextOnNotesInput.setText("Header Test");
		sdDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeOnSlideInput.setText("Fix Date: 20120329");
		sdFooterTextOnSlide.check();
		sdFooterTextOnSlideInput.setText("Footer Test");
		sdSlideNumAsFooterOnSlide.check();
		sdApplyToAllButtonOnSlideFooter.click();

		impressSlideSorter.focus();
		typeKeys("<up>");

		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		assertEquals("Header Test", sdHeaderTextOnNotesInput.getText());
		assertEquals("Fix Date: 20120329", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterOnNotesTabPage.cancel();

		// save and reopen this file
		String saveTo = getPath("temp/" + "hello.odp");
		FileUtil.deleteFile(saveTo);
		saveAs(saveTo);
		close();
		open(saveTo);
		impress.waitForExistence(10, 2);

		// check after reopen
		app.dispatch(".uno:HeaderAndFooter");
		sdHeaderAndFooterOnNotesTabPage.select();
		assertEquals("Header Test", sdHeaderTextOnNotesInput.getText());
		assertEquals("Fix Date: 20120329", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterOnNotesTabPage.cancel();
	}

	/**
	 * Test Insert Header and Footer to Slide Save and Reopen
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertHeaderFooterOnSlide() throws Exception {

		// add header and footer
		app.dispatch(".uno:HeaderAndFooter");
		sdDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeFooterOnSlide.check();
		sdFixedDateAndTimeOnSlideInput.setText("Fix Date: 20120329");
		sdFooterTextOnSlide.check();
		sdFooterTextOnSlideInput.setText("Footer Test");
		sdSlideNumAsFooterOnSlide.check();
		sdApplyToAllButtonOnSlideFooter.click();

		impressSlideSorter.focus();
		typeKeys("<up>");

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals("Fix Date: 20120329", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());

		// close header and footer dialog.
		sdApplyButtonOnSlideFooter.focus();
		typeKeys("<tab>");
		typeKeys("<enter>");

		// save this file
		String saveTo = getPath("temp/" + "hello.odp");
		FileUtil.deleteFile(saveTo);
		saveAs(saveTo);
		close();
		open(saveTo);
		impress.waitForExistence(10, 2);

		// check after reopen
		app.dispatch(".uno:HeaderAndFooter");
		assertEquals("Fix Date: 20120329", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals("Footer Test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());
		sdHeaderAndFooterDlgSlideTab.cancel();
	}
}
