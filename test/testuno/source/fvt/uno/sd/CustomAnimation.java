package fvt.uno.sd;

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
import com.sun.star.table.XCell;
import com.sun.star.text.ControlCharacter;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextRange;
import com.sun.star.uno.UnoRuntime;

public class CustomAnimation {
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
	public void testShapeAnimation() throws Exception {

		XShapes xShapes = null;
		XPropertySet xShapePropSet[] = new XPropertySet[23];

		drawpages.insertNewByIndex(0);

		String aNameArray[] = { "Page 1", "Page 2", "Page 3", "Page 4",
				"Page 5", "Page 6", "Page 7", "Page 8", "Page 9", "Page 10",
				"Page 11", "Page 12", "Page 13", "Page 14", "Page 15",
				"Page 16", "Page 17", "Page 18", "Page 19", "Page 20",
				"Page 21", "Page 22", "Page 23", };
		AnimationEffect AnimationEffectArray[] = { 
				AnimationEffect.NONE,
				AnimationEffect.FADE_FROM_LEFT, 
				AnimationEffect.FADE_FROM_TOP,
				AnimationEffect.FADE_FROM_RIGHT,
				AnimationEffect.FADE_FROM_BOTTOM,
				AnimationEffect.FADE_TO_CENTER,
				AnimationEffect.FADE_FROM_CENTER,
				AnimationEffect.MOVE_FROM_LEFT, 
				AnimationEffect.MOVE_FROM_TOP,
				AnimationEffect.MOVE_FROM_RIGHT,
				AnimationEffect.MOVE_FROM_BOTTOM,
				AnimationEffect.VERTICAL_STRIPES,
				AnimationEffect.HORIZONTAL_STRIPES, 
				AnimationEffect.CLOCKWISE,
				AnimationEffect.COUNTERCLOCKWISE,
				AnimationEffect.FADE_FROM_UPPERLEFT,
				AnimationEffect.FADE_FROM_UPPERRIGHT,
				AnimationEffect.FADE_FROM_LOWERLEFT,
				AnimationEffect.FADE_FROM_LOWERRIGHT,
				AnimationEffect.CLOSE_VERTICAL,
				AnimationEffect.CLOSE_HORIZONTAL,
				AnimationEffect.OPEN_VERTICAL, 
				AnimationEffect.OPEN_HORIZONTAL,
				AnimationEffect.PATH, 
/*				AnimationEffect.MOVE_TO_LEFT,
				AnimationEffect.MOVE_TO_TOP, 
				AnimationEffect.MOVE_TO_RIGHT,
				AnimationEffect.MOVE_TO_BOTTOM, 
				AnimationEffect.SPIRALIN_LEFT,
				AnimationEffect.SPIRALIN_RIGHT, 
				AnimationEffect.SPIRALOUT_LEFT,
				AnimationEffect.SPIRALOUT_RIGHT, 
				AnimationEffect.DISSOLVE,
				AnimationEffect.WAVYLINE_FROM_LEFT, 
				AnimationEffect.RANDOM,
				AnimationEffect.VERTICAL_LINES,
				AnimationEffect.LASER_FROM_LEFT, 
				AnimationEffect.APPEAR,
				AnimationEffect.HIDE, 
				AnimationEffect.MOVE_FROM_UPPERLEFT,
				AnimationEffect.VERTICAL_CHECKERBOARD,
				AnimationEffect.HORIZONTAL_ROTATE,
				AnimationEffect.VERTICAL_STRETCH, 
				AnimationEffect.ZOOM_IN,
				AnimationEffect.ZOOM_OUT_FROM_LEFT, */
				};

		// insert  pages
		while (drawpages.getCount() < aNameArray.length)
			drawpages.insertNewByIndex(0);

		// add text shape to each page
		for (int i = 0; i < aNameArray.length; i++) {
			XDrawPage xDrawPage = (XDrawPage) UnoRuntime.queryInterface(
					XDrawPage.class, drawpages.getByIndex(i));
			XNamed xPageName = (XNamed) UnoRuntime.queryInterface(XNamed.class,
					xDrawPage);
			xPageName.setName(aNameArray[i]);

			xShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class,
					drawpages.getByIndex(i));

			// create a rectangle that is placed on the top left of the page
			xShapePropSet[i] = ShapeUtil.createAndInsertShape(impressDocument,
					xShapes, new Point(1000, 1000), new Size(5000, 5000),
					"com.sun.star.drawing.RectangleShape");
		}

		for (int i = 0; i < aNameArray.length; i++) {
			// set an object animation for each slide
			xShapePropSet[i]
					.setPropertyValue("Effect", AnimationEffectArray[i]);
		}

		// If this property and the property DimPrevious are both true, the shape is hidden on the next click after the AnimationEffect has finished. 
		xShapePropSet[1].setPropertyValue("DimHide", new Boolean(false));
		// If true, this shape is painted using DimColor on the next click after finishing the AnimationEffect. 
		xShapePropSet[1].setPropertyValue("DimPrevious", new Boolean(true));
		// his color is used to paint the shape on the next click after the animation effect has finished
		xShapePropSet[1].setPropertyValue("DimColor", new Integer(0xff0000));
		// the sound file specified in Sound is played while the animation effect is executed
		xShapePropSet[1].setPropertyValue("SoundOn", new Boolean(false));
		// the sound specified in the Sound property of this shape is played completely
		xShapePropSet[1].setPropertyValue("PlayFull", new Boolean(false));

		saveAndLoadSlide();

		for (int i = 0; i < aNameArray.length; i++) {
			// assert if Shape Animation is set
			Assert.assertEquals(
					"Text Animation Effect is AnimationEffectArray[]",
					AnimationEffectArray[i],
					xShapePropSet[i].getPropertyValue("Effect"));
		}
		// assert if DimHide is false
		Assert.assertEquals("Shape Animation DimHide is false", Boolean.FALSE,
				xShapePropSet[1].getPropertyValue("DimHide"));
		// assert if DimPrevious is true
		Assert.assertEquals("Shape Animation DimPrevious is true",
				Boolean.TRUE, xShapePropSet[1].getPropertyValue("DimPrevious"));
		// assert if DimColor is red
		Assert.assertEquals("Shape Animation DimColor is Red", 0xff0000,
				xShapePropSet[1].getPropertyValue("DimColor"));
		// assert the sound file specified in Sound don't play while the
		// animation effect is executed.
		Assert.assertEquals("Shape SoundOn is false", Boolean.FALSE,
				xShapePropSet[1].getPropertyValue("SoundOn"));
		// assert the sound specified in the Sound property of this shape is
		// played completely
		Assert.assertEquals("Shape PlayFull is false", Boolean.FALSE,
				xShapePropSet[1].getPropertyValue("PlayFull"));

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
		filePath = Testspace.getPath("tmp/shapeanimation." + sExtension);
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

		return (XComponent) UnoRuntime.queryInterface(XComponent.class,
				unoApp.loadDocument(filePath));
	}
}
