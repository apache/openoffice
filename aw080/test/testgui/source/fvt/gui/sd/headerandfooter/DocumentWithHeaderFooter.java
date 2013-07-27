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
import static org.openoffice.test.vcl.Tester.sleep;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Logger;

public class DocumentWithHeaderFooter {
	@Rule
	public Logger log = Logger.getLogger(this);
	

	@Before
	public void setUp() throws Exception {
		app.start(true); 
	}

	@After
	public void tearDown() throws Exception {
		app.stop();

	}

	/**
	 * Test open AOO3.4 presentation with header and footer. edit and save to
	 * ODP
	 * 
	 * @throws Exception
	 */
	@Test
	public void testOpenAOO34WithHeaderFooter() throws Exception {
		// open sample file
		String file = prepareData("sd/AOO3.4HeaderFooter.odp");
		open(file);
		impress.waitForExistence(10, 2);

		// check after reopen
		app.dispatch(".uno:HeaderAndFooter");
		assertEquals(true, sdDateAndTimeFooterOnSlide.isChecked());
		assertEquals("fixed date", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals(true, sdFooterTextOnSlide.isChecked());
		assertEquals("footer test", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());

		sdSlideNumAsFooterOnSlide.uncheck();
		sdApplyToAllButtonOnSlideFooter.click();

		// save to odp and reopen
		String saveTo2 = getPath("temp/" + "AOO3.4HeaderFooter.odp");
		FileUtil.deleteFile(saveTo2);
		saveAs(saveTo2);
		close();
		open(saveTo2);
		impress.waitForExistence(10, 2);

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals(false, sdSlideNumAsFooterOnSlide.isChecked());
	}

	/**
	 * Test open ppt file with header and footer. edit and save to PPT/ODP
	 * 
	 * @throws Exception
	 */
	@Test
	public void testOpenPPTWithHeaderFooter() throws Exception {
		// open sample file
		String file = prepareData("sd/gfdd.ppt");
		open(file);
		impress.waitForExistence(10, 2);

		// check after reopen
		app.dispatch(".uno:HeaderAndFooter");
		assertEquals(true, sdDateAndTimeFooterOnSlide.isChecked());
		assertEquals("testdte", sdFixedDateAndTimeOnSlideInput.getText());
		assertEquals(true, sdFooterTextOnSlide.isChecked());
		assertEquals("yesy", sdFooterTextOnSlideInput.getText());
		assertEquals(true, sdSlideNumAsFooterOnSlide.isChecked());

		sdSlideNumAsFooterOnSlide.uncheck();
		sdApplyToAllButtonOnSlideFooter.click();

		// save to ppt and reopen
		String saveTo = getPath("temp/" + "gfdd.ppt");
		FileUtil.deleteFile(saveTo);
		saveAs(saveTo);
		close();
		open(saveTo);
		impress.waitForExistence(10, 2);

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals(false, sdSlideNumAsFooterOnSlide.isChecked());

		// close Header and Footer dialog.
		sdApplyButtonOnSlideFooter.focus();
		typeKeys("<tab>");
		typeKeys("<enter>");

		// save to odp and reopen
		String saveTo2 = getPath("temp/" + "gfdd.odp");
		FileUtil.deleteFile(saveTo);
		saveAs(saveTo2);
		close();
		open(saveTo2);
		impress.waitForExistence(10, 2);

		app.dispatch(".uno:HeaderAndFooter");
		assertEquals(false, sdSlideNumAsFooterOnSlide.isChecked());
	}
}
