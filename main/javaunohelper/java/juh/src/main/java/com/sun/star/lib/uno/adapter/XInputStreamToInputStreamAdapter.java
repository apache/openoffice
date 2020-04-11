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


package com.sun.star.lib.uno.adapter;

import java.io.IOException;
import com.sun.star.io.XInputStream;
import java.io.InputStream;

/**
 * The <code>XInputStreamToInputStreamAdapter</code> wraps
 * the UNO <code>XInputStream</code> object in a Java
 * <code>InputStream</code>.  This allows users to access
 * an <code>XInputStream</code> as if it were an
 * <code>InputStream</code>.
 *
 * @author  Brian Cameron
 */
public class XInputStreamToInputStreamAdapter extends InputStream {

    /** 
     *  Internal handle to the XInputStream
     */
    private XInputStream xin;
 
    /**
     *  Constructor.
     *
     *  @param  in  The <code>XInputStream</code> to be 
     *              accessed as an <code>InputStream</code>.
     */
    public XInputStreamToInputStreamAdapter (XInputStream in) {
        xin = in;
    }

    public int available() throws IOException {

        int bytesAvail;

        try {
            bytesAvail = xin.available();
        } catch (Exception e) {
            throw new IOException(e.toString());
        }

        return(bytesAvail);
    }

    public void close() throws IOException {
        try {
            xin.closeInput();
        } catch (Exception e) {
            throw new IOException(e.toString());
        }
    }

    public int read () throws IOException {	
        byte [][] tmp = new byte [1][1]; 
        try {
            long bytesRead = xin.readBytes(tmp, 1);
	  
            if (bytesRead <= 0) {
               return (-1);
            } else {
		int tmpInt = tmp[0][0];
		if (tmpInt< 0 ){
		    tmpInt = 256 +tmpInt;
		}
                return(tmpInt);
            }

        } catch (Exception e) {
            throw new IOException(e.toString());
        }
    }

    public int read (byte[] b) throws IOException {
	
        byte [][] tmp = new byte [1][b.length]; 
        int bytesRead;

        try {
            bytesRead = xin.readBytes(tmp, b.length);
            if (bytesRead <= 0) {
                return(-1);
            } else if (bytesRead < b.length) {
                System.arraycopy(tmp[0], 0, b, 0, bytesRead);
            } else {
                System.arraycopy(tmp[0], 0, b, 0, b.length);
            }
        } catch (Exception e) {
            throw new IOException(e.toString());
        }

        return (bytesRead);
    }

    public int read(byte[] b, int off, int len) throws IOException {
        int count = 0;
        byte [][] tmp = new byte [1][b.length];
        try {
	    long bytesRead=0;
            int av = xin.available();
	    if ( av != 0 && len > av) {
		  bytesRead = xin.readBytes(tmp, av);
	    }
	    else{
		bytesRead = xin.readBytes(tmp,len);
	    }
            // Casting bytesRead to an int is okay, since the user can
            // only pass in an integer length to read, so the bytesRead 
            // must <= len.
            //
            if (bytesRead <= 0) {
                return(-1);
	    } else if (bytesRead < len) {
		System.arraycopy(tmp[0], 0, b, off, (int)bytesRead);
	    } else {
                System.arraycopy(tmp[0], 0, b, off, len);
	    }
	    
	    return ((int)bytesRead);
	    
		
        } catch (Exception e) {
            throw new IOException("reader error: "+e.toString());
        }
    }

    public long skip(long n) throws IOException {

        int avail;
        long tmpLongVal = n;
        int  tmpIntVal;

        try {
            avail = xin.available();
        } catch (Exception e) {
            throw new IOException(e.toString());
        }

        do {
            if (tmpLongVal >= Integer.MAX_VALUE) {
               tmpIntVal = Integer.MAX_VALUE;
            } else {
               // Casting is safe here.
               tmpIntVal = (int)tmpLongVal;
            }
            tmpLongVal -= tmpIntVal;
 
            try {
                xin.skipBytes(tmpIntVal);
            } catch (Exception e) {
                throw new IOException(e.toString());
            }
        } while (tmpLongVal > 0);

        if ( avail != 0 && avail < n) {
            return(avail);
        } else {
            return(n);
        }
    }

   /**
    *  Tests if this input stream supports the mark and reset methods.
    *  The markSupported method of
    *  <code>XInputStreamToInputStreamAdapter</code> returns false.
    *
    *  @returns  false
    */
    public boolean markSupported() {
       return false;
    }

    public void mark(int readlimit) {
        // Not supported.
    }

    public void reset() throws IOException {
        // Not supported.
    }
}

