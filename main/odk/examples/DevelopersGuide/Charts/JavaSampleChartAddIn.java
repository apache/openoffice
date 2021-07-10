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



// base classes
import com.sun.star.uno.XInterface;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.Type;
import com.sun.star.container.XIndexAccess;
import com.sun.star.lib.uno.helper.WeakBase;

// factories
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XSingleServiceFactory;

// graphics stuff
import com.sun.star.drawing.*;
import com.sun.star.awt.Point;
import com.sun.star.awt.Rectangle;
import com.sun.star.awt.Size;

// chart stuff
import com.sun.star.chart.*;

// property access
import com.sun.star.beans.*;

// Add-In stuff
import com.sun.star.lang.XInitialization;
import com.sun.star.util.XRefreshable;
import com.sun.star.lang.XServiceName;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.lang.XTypeProvider;

// Exceptions
import com.sun.star.uno.Exception;
import com.sun.star.uno.RuntimeException;

import javax.swing.JOptionPane;

public class JavaSampleChartAddIn extends WeakBase implements
    XInitialization,
    XRefreshable,
    XDiagram,
    XServiceName,
    XServiceInfo
{
    public JavaSampleChartAddIn()
    {}

    // __________ interface methods __________

    // XInitialization
    public void initialize( Object[] aArguments )
        throws Exception, RuntimeException
    {
        if( aArguments.length > 0 )
        {
            maChartDocument = (XChartDocument) UnoRuntime.queryInterface(
                XChartDocument.class, aArguments[ 0 ]);

            XPropertySet aDocProp = (XPropertySet) UnoRuntime.queryInterface(
                XPropertySet.class, maChartDocument );
            if( aDocProp != null )
            {
                // set base diagram which will be extended in refresh()
                aDocProp.setPropertyValue( "BaseDiagram", "com.sun.star.chart.XYDiagram" );
            }

            // get the draw page
            XDrawPageSupplier aPageSupp = (XDrawPageSupplier) UnoRuntime.queryInterface(
                XDrawPageSupplier.class, maChartDocument );
            if( aPageSupp != null )
                maDrawPage = (XDrawPage) UnoRuntime.queryInterface(
                    XDrawPage.class, aPageSupp.getDrawPage() );

            // get a factory for creating shapes
            maShapeFactory = (XMultiServiceFactory) UnoRuntime.queryInterface(
                XMultiServiceFactory.class, maChartDocument );
        }
    }

    // XRefreshable
    public void refresh() throws RuntimeException
    {
        // recycle shapes in first call, if document was loaded
        if( maBottomLine == null ||
            maTopLine == null )
        {
            // try to recycle loaded shapes
            XPropertySet aDocProp = (XPropertySet) UnoRuntime.queryInterface(
                XPropertySet.class, maChartDocument );
            if( aDocProp != null )
            {
                try
                {
                    XIndexAccess aShapesIA = (XIndexAccess) UnoRuntime.queryInterface(
                        XIndexAccess.class, aDocProp.getPropertyValue( "AdditionalShapes" ));
                    if( aShapesIA != null &&
                        aShapesIA.getCount() > 0 )
                    {
                        XShape aShape;
                        String aName;
                        for( int i = aShapesIA.getCount() - 1; i >= 0; --i )
                        {
                            aShape = (XShape) UnoRuntime.queryInterface(
                                XShape.class, aShapesIA.getByIndex( i ));
                            if( aShape != null )
                            {
                                XPropertySet aProp = (XPropertySet) UnoRuntime.queryInterface(
                                    XPropertySet.class, aShape );
                                aName = (String) aProp.getPropertyValue( "Name" );

                                if( aName.equals( "top" ))
                                {
                                    maTopLine = aShape;
                                }
                                else if( aName.equals( "bottom" ))
                                {
                                    maBottomLine = aShape;
                                }
                            }
                        }
                    }
                }
                catch( Exception ex )
                {
                    JOptionPane.showMessageDialog( null, ex, "Exception caught", JOptionPane.WARNING_MESSAGE );
                }
            }
        }

        // create top line if it does not yet exist
        try
        {
            if( maTopLine == null )
            {
                maTopLine = (XShape) UnoRuntime.queryInterface(
                    XShape.class, maShapeFactory.createInstance( "com.sun.star.drawing.LineShape" ));
                maDrawPage.add( maTopLine );

                // make line red and thicker
                XPropertySet aShapeProp = (XPropertySet) UnoRuntime.queryInterface(
                    XPropertySet.class, maTopLine );

                aShapeProp.setPropertyValue( "LineColor", new Integer( 0xe01010 ));
                aShapeProp.setPropertyValue( "LineWidth", new Integer( 50 ));
                aShapeProp.setPropertyValue( "Name", "top" );
            }
        }
        catch( Exception ex )
        {
            JOptionPane.showMessageDialog( null, ex, "Exception caught", JOptionPane.WARNING_MESSAGE );
        }

        // create bottom line if it does not yet exist
        try
        {
            if( maBottomLine == null )
            {
                maBottomLine = (XShape) UnoRuntime.queryInterface(
                    XShape.class, maShapeFactory.createInstance( "com.sun.star.drawing.LineShape" ));
                maDrawPage.add( maBottomLine );

                // make line green and thicker
                XPropertySet aShapeProp = (XPropertySet) UnoRuntime.queryInterface(
                    XPropertySet.class, maBottomLine );

                aShapeProp.setPropertyValue( "LineColor", new Integer( 0x10e010 ));
                aShapeProp.setPropertyValue( "LineWidth", new Integer( 50 ));
                aShapeProp.setPropertyValue( "Name", "bottom" );
            }
        }
        catch( Exception ex )
        {
            JOptionPane.showMessageDialog( null, ex, "Exception caught", JOptionPane.WARNING_MESSAGE );
        }

        if( maTopLine == null ||
            maBottomLine == null )
        {
            JOptionPane.showMessageDialog( null, "One of the lines is still null", "Assertion", JOptionPane.WARNING_MESSAGE );
            return;
        }

        // position lines
        // --------------

        // get data
        XChartDataArray aDataArray = (XChartDataArray) UnoRuntime.queryInterface(
            XChartDataArray.class, maChartDocument.getData());
        double aData[][] = aDataArray.getData();

        // get axes
        XDiagram aDiagram = maChartDocument.getDiagram();
        XShape aXAxis = (XShape) UnoRuntime.queryInterface(
            XShape.class, ((XAxisXSupplier) UnoRuntime.queryInterface(
                               XAxisXSupplier.class, aDiagram )).getXAxis() );
        XShape aYAxis = (XShape) UnoRuntime.queryInterface(
            XShape.class, ((XAxisYSupplier) UnoRuntime.queryInterface(
                               XAxisYSupplier.class, aDiagram )).getYAxis() );

        // calculate points for hull
        final int nLength = aData.length;
        int i, j;
        double fMax, fMin;

        Point aMaxPtSeq[][] = new Point[ 1 ][];
        aMaxPtSeq[ 0 ] = new Point[ nLength ];
        Point aMinPtSeq[][] = new Point[ 1 ][];
        aMinPtSeq[ 0 ] = new Point[ nLength ];

        for( i = 0; i < nLength; i++ )
        {
            fMin = fMax = aData[ i ][ 1 ];
            for( j = 1; j < aData[ i ].length; j++ )
            {
                if( aData[ i ][ j ] > fMax )
                    fMax = aData[ i ][ j ];
                else if( aData[ i ][ j ] < fMin )
                    fMin = aData[ i ][ j ];
            }
            aMaxPtSeq[ 0 ][ i ] = new Point( getAxisPosition( aXAxis, aData[ i ][ 0 ], false ),
                                             getAxisPosition( aYAxis, fMax, true ));
            aMinPtSeq[ 0 ][ i ] = new Point( getAxisPosition( aXAxis, aData[ i ][ 0 ], false ),
                                             getAxisPosition( aYAxis, fMin, true ));
        }

        // apply point sequences to lines
        try
        {
            XPropertySet aShapeProp = (XPropertySet) UnoRuntime.queryInterface(
                XPropertySet.class, maTopLine );
            aShapeProp.setPropertyValue( "PolyPolygon", aMaxPtSeq );

            aShapeProp = (XPropertySet) UnoRuntime.queryInterface(
                XPropertySet.class, maBottomLine );
            aShapeProp.setPropertyValue( "PolyPolygon", aMinPtSeq );
        }
        catch( Exception ex )
        {
            JOptionPane.showMessageDialog( null, ex, "Exception caught", JOptionPane.WARNING_MESSAGE );
        }
    }

    public void addRefreshListener( com.sun.star.util.XRefreshListener aListener )
        throws RuntimeException
    {
        // we don't want this but we have to implement the interface
    }

    public void removeRefreshListener( com.sun.star.util.XRefreshListener aListener )
        throws RuntimeException
    {
        // we don't want this but we have to implement the interface
    }


    // XServiceName
    public String getServiceName() throws RuntimeException
    {
        return new String( smServiceName );
    }

    // XServiceInfo
    public boolean supportsService( String aServiceName )
    {
        String[] aServices = getSupportedServiceNames_Static();
        int i, nLength = aServices.length;
        boolean bResult = false;

        for( i = 0; !bResult && i < nLength; ++i )
            bResult = aServiceName.equals( aServices[ i ] );

        return bResult;
    }

    public String getImplementationName()
    {
        return( JavaSampleChartAddIn.class.getName() );
    }

    public String[] getSupportedServiceNames()
    {
        return getSupportedServiceNames_Static();
    }

    // XDiagram
    public String getDiagramType() throws RuntimeException
    {
        return new String( smServiceName );
    }

    public XPropertySet getDataRowProperties( int nRow )
        throws com.sun.star.lang.IndexOutOfBoundsException, RuntimeException
    {
        return maChartDocument.getDiagram().getDataRowProperties( nRow );
    }

    public XPropertySet getDataPointProperties( int nCol, int nRow )
        throws com.sun.star.lang.IndexOutOfBoundsException, RuntimeException
    {
        return maChartDocument.getDiagram().getDataPointProperties( nCol, nRow );
    }

    // XShape : XDiagram
    public Size getSize() throws RuntimeException
    {
        return ((XShape) UnoRuntime.queryInterface( XShape.class, maChartDocument.getDiagram())).getSize();
    }
    public void setSize( Size aSize ) throws RuntimeException, PropertyVetoException
    {
        ((XShape) UnoRuntime.queryInterface( XShape.class, maChartDocument.getDiagram())).setSize( aSize );
    }

    public Point getPosition() throws RuntimeException
    {
        return ((XShape) UnoRuntime.queryInterface( XShape.class, maChartDocument.getDiagram())).getPosition();
    }
    public void setPosition( Point aPos ) throws RuntimeException
    {
        ((XShape) UnoRuntime.queryInterface( XShape.class, maChartDocument.getDiagram())).setPosition( aPos );
    }

    // XShapeDescriptor : XShape : XDiagram
    public String getShapeType() throws RuntimeException
    {
        return new String( "com.sun.star.comp.Chart.JavaSampleDiagramShape" );
    }


    // __________ private members __________
    private com.sun.star.chart.XChartDocument       maChartDocument;
    private com.sun.star.drawing.XDrawPage          maDrawPage;
    private com.sun.star.lang.XMultiServiceFactory  maShapeFactory;

    // shapes added by add-in
    private com.sun.star.drawing.XShape             maTopLine;
    private com.sun.star.drawing.XShape             maBottomLine;

    // __________ private methods __________

    private int getAxisPosition( XShape aAxis, double fValue, boolean bVertical )
    {
        int nResult = 0;

        if( aAxis != null )
        {
            XPropertySet aAxisProp = (XPropertySet) UnoRuntime.queryInterface(
                XPropertySet.class, aAxis );

            try
            {
                double fMin, fMax;
                fMin = ((Double) aAxisProp.getPropertyValue( "Min" )).doubleValue();
                fMax = ((Double) aAxisProp.getPropertyValue( "Max" )).doubleValue();
                double fRange = fMax - fMin;

                if( fMin <= fValue && fValue <= fMax &&
                    fRange != 0 )
                {
                    if( bVertical )
                    {
                        nResult = aAxis.getPosition().Y +
                            (int)((double)(aAxis.getSize().Height) *
                                  (1.0 - (( fValue - fMin ) / fRange )));
                    }
                    else
                    {
                        nResult = aAxis.getPosition().X +
                            (int)((double)(aAxis.getSize().Width) *
                                  (( fValue - fMin ) / fRange ));
                    }
                }
			}
			catch( Exception ex )
			{
                JOptionPane.showMessageDialog( null, ex, "Exception caught", JOptionPane.WARNING_MESSAGE );
            }
		}
        return nResult;
	}

    // __________ static things __________

    private static final String smServiceName = "com.sun.star.comp.Chart.JavaSampleChartAddIn";

    public static String[] getSupportedServiceNames_Static()
    {
        String[] aResult = { smServiceName,
                             "com.sun.star.chart.Diagram",
                             "com.sun.star.chart.ChartAxisYSupplier" };
        return aResult;
    }


    /**
     * Returns a factory for creating the service.
     * This method is called by the <code>JavaLoader</code>
     * <p>
     * @return  returns a <code>XSingleServiceFactory</code> for creating the component
     * @param   implName     the name of the implementation for which a service is desired
     * @param   multiFactory the service manager to be used if needed
     * @param   regKey       the registryKey
     * @see                  com.sun.star.comp.loader.JavaLoader
     */
    public static XSingleServiceFactory __getServiceFactory(
        String implName,
        XMultiServiceFactory multiFactory,
        com.sun.star.registry.XRegistryKey regKey )
    {
        XSingleServiceFactory xSingleServiceFactory = null;

        if( implName.equals( JavaSampleChartAddIn.class.getName()) )
        {
            xSingleServiceFactory = com.sun.star.comp.loader.FactoryHelper.getServiceFactory(
                JavaSampleChartAddIn.class, smServiceName,
                multiFactory, regKey );
        }

        return xSingleServiceFactory;
    }

    /**
     * Writes the service information into the given registry key.
     * This method is called by the <code>JavaLoader</code>
     * <p>
     * @return  returns true if the operation succeeded
     * @param   regKey       the registryKey
     * @see                  com.sun.star.comp.loader.JavaLoader
     */
    // This method not longer necessary since OOo 3.4 where the component registration
    // was changed to passive component registration. For more details see
    // https://wiki.openoffice.org/wiki/Passive_Component_Registration

//    public static boolean __writeRegistryServiceInfo( com.sun.star.registry.XRegistryKey regKey )
//     {
//         boolean bResult = true;

//         String[] aServices = getSupportedServiceNames_Static();
//         int i, nLength = aServices.length;

//         for( i = 0; i < nLength; ++i )
//         {
//             bResult = bResult && com.sun.star.comp.loader.FactoryHelper.writeRegistryServiceInfo(
//                 JavaSampleChartAddIn.class.getName(), aServices[ i ], regKey );
//         }
//         return bResult;
//     }
}

