package fvt.uno.sd.animation;

import junit.framework.Assert;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.ShapeUtil;

import com.sun.star.awt.Point;
import com.sun.star.awt.Size;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexContainer;
import com.sun.star.container.XNameContainer;
import com.sun.star.container.XNamed;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XDrawPages;
import com.sun.star.drawing.XDrawPagesSupplier;
import com.sun.star.drawing.XShape;
import com.sun.star.drawing.XShapes;
import com.sun.star.frame.XStorable;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XSingleServiceFactory;
import com.sun.star.presentation.AnimationEffect;
import com.sun.star.presentation.XCustomPresentationSupplier;
import com.sun.star.presentation.XPresentation;
import com.sun.star.presentation.XPresentationSupplier;
import com.sun.star.text.ControlCharacter;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextRange;
import com.sun.star.uno.UnoRuntime;

public class TextAnimation {
	XPresentationSupplier sdDocument = null;
	XPresentation pre = null;
	XComponent precomp = null;
	XComponent impressDocument = null;
	XComponent reLoadFile = null;
	XDrawPagesSupplier drawsupplier = null;
	XDrawPages drawpages = null;

	String filePath = null;

	UnoApp unoApp = new UnoApp();

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		unoApp.start();
		createDocumentAndSlide();
	}

	@After
	public void tearDown() throws Exception {
		unoApp.closeDocument(impressDocument);
		unoApp.closeDocument(reLoadFile);
		unoApp.close();
	}

	@Test
	public void testTextAnimation() throws Exception {

		XShapes xShapes;
		XPropertySet xShapePropSet;
		Point po = new Point(5000, 5000);

		// create pages, so that three are available
		drawpages.insertNewByIndex(0);
		// get the shape container for page one
		xShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class,
				drawpages.getByIndex(0));
		// create a rectangle that is placed on the top left of the page
		XShape xRectangle = ShapeUtil.createShape(impressDocument, po,
				new Size(21000, 12500), "com.sun.star.drawing.RectangleShape");
		xShapes.add(xRectangle);

		XPropertySet xTextPropSet = addPortion(xRectangle, "Text Animation",
				false);

		xTextPropSet.setPropertyValue("TextEffect",
				AnimationEffect.MOVE_FROM_RIGHT);

		saveAndLoadSlide();

		// assert if Text Animation is set
		Assert.assertEquals("Text Animation Effect is MOVE_FROM_RIGHT",
				AnimationEffect.MOVE_FROM_RIGHT,
				xTextPropSet.getPropertyValue("TextEffect"));

	}

	public XPropertySet addPortion(XShape xShape, String sText,
			boolean bNewParagraph)
			throws com.sun.star.lang.IllegalArgumentException {
		XText xText = (XText) UnoRuntime.queryInterface(XText.class, xShape);

		XTextCursor xTextCursor = xText.createTextCursor();
		xTextCursor.gotoEnd(false);
		if (bNewParagraph == true) {
			xText.insertControlCharacter(xTextCursor,
					ControlCharacter.PARAGRAPH_BREAK, false);
			xTextCursor.gotoEnd(false);
		}
		XTextRange xTextRange = (XTextRange) UnoRuntime.queryInterface(
				XTextRange.class, xTextCursor);
		xTextRange.setString(sText);
		xTextCursor.gotoEnd(true);
		XPropertySet xPropSet = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xText);
		return xPropSet;
	}

	/**
	 * create a new presentation document and insert a new slide.
	 * 
	 * @throws Exception
	 */
	public void createDocumentAndSlide() throws Exception {
		impressDocument = (XComponent) UnoRuntime.queryInterface(
				XComponent.class, unoApp.newDocument("simpress"));
		drawsupplier = (XDrawPagesSupplier) UnoRuntime.queryInterface(
				XDrawPagesSupplier.class, impressDocument);
		drawpages = drawsupplier.getDrawPages();

		sdDocument = (XPresentationSupplier) UnoRuntime.queryInterface(
				XPresentationSupplier.class, impressDocument);
		pre = sdDocument.getPresentation();
	}

	/**
	 * Save presentation and reLoad the slide.
	 * 
	 * @param no
	 * @return void
	 * @throws Exception
	 */
	public void saveAndLoadSlide() throws Exception {
		reLoadFile = saveAndReloadDoc(impressDocument,
				"StarOffice XML (Impress)", "odp");
		drawsupplier = (XDrawPagesSupplier) UnoRuntime.queryInterface(
				XDrawPagesSupplier.class, reLoadFile);
		drawpages = drawsupplier.getDrawPages();

		sdDocument = (XPresentationSupplier) UnoRuntime.queryInterface(
				XPresentationSupplier.class, reLoadFile);
		pre = sdDocument.getPresentation();
	}

	/**
	 * save and reload Presentation document.
	 * 
	 * @param presentationDocument
	 * @param sFilter
	 * @param sExtension
	 * @return
	 * @throws Exception
	 */
	private XComponent saveAndReloadDoc(XComponent presentationDocument,
			String sFilter, String sExtension) throws Exception {
		filePath = Testspace.getPath("tmp/customshow." + sExtension);
		PropertyValue[] aStoreProperties = new PropertyValue[2];
		aStoreProperties[0] = new PropertyValue();
		aStoreProperties[1] = new PropertyValue();
		aStoreProperties[0].Name = "Override";
		aStoreProperties[0].Value = true;
		aStoreProperties[1].Name = "FilterName";
		aStoreProperties[1].Value = sFilter;
		XStorable xStorable = (XStorable) UnoRuntime.queryInterface(
				XStorable.class, presentationDocument);
		xStorable.storeToURL(FileUtil.getUrl(filePath), aStoreProperties);

		return UnoRuntime.queryInterface(XComponent.class,
				unoApp.loadDocument(filePath));
	}
}
