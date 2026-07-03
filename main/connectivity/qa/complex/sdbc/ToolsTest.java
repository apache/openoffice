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
package complex.sdbc;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import org.junit.Assume;
import org.junit.Test;

import com.sun.star.comp.sdbc.Tools;

/**
 * Tests for the {@link Tools} helper class.
 */
public final class ToolsTest {

    /** Runs each entry through addClassPathURL and returns what was accepted. */
    private static List<URL> collect(String... entries) {
        List<URL> urls = new ArrayList<URL>();
        for (String entry : entries) {
            Tools.addClassPathURL(urls, entry);
        }
        return urls;
    }

    /** True if the running JRE has a URL stream handler for the given scheme. */
    private static boolean schemeSupported(String scheme) {
        try {
            new URL(scheme + ":/probe");
            return true;
        } catch (MalformedURLException e) {
            return false;
        }
    }

    @Test
    public void testAddClassPathURLAddsLocalFileEntry() {
        List<URL> urls = collect("file:/opt/a.jar");
        assertEquals(1, urls.size());
        assertEquals("file", urls.get(0).getProtocol());
    }

    @Test
    public void testAddClassPathURLAddsJarWrappedLocalFile() {
        assertEquals(1, collect("jar:file:/opt/a.jar!/").size());
    }

    @Test
    public void testAddClassPathURLTreatsJarInnerSchemeCaseInsensitively() {
        // URL.getPath() does not normalize the wrapped URL.
        assertEquals(1, collect("jar:FILE:/opt/a.jar!/").size());
    }

    @Test
    public void testAddClassPathURLSkipsRemoteEntries() {
        assertTrue(collect(
                "http://host/a.jar",
                "https://host/b.jar",
                "ftp://host/c.jar").isEmpty());
    }

    @Test
    public void testAddClassPathURLSkipsJarWrappedRemoteEntry() {
        assertTrue(collect("jar:http://host/a.jar!/").isEmpty());
    }

    @Test
    public void testAddClassPathURLSkipsMalformedEntry() {
        assertTrue(collect("::not-a-url::").isEmpty());
    }

    @Test
    public void testAddClassPathURLSkipsUnknownScheme() {
        // A scheme with no registered URL handler cannot be constructed, so the
        // entry is treated as malformed and skipped.
        assertTrue(collect(
                "wibble:/opt/a.jar",
                "classpath:/opt/b.jar",
                "foo://host/c.jar").isEmpty());
    }

    @Test
    public void testAddClassPathURLSkipsJarWrappedUnknownScheme() {
        assertTrue(collect("jar:wibble:/opt/a.jar!/").isEmpty());
    }

    @Test
    public void testAddClassPathURLKeepsOnlyLocalEntriesInOrder() {
        List<URL> urls = collect(
                "http://host/a.jar",
                "file:/opt/b.jar",
                "jar:http://host/c.jar!/",
                "file:/opt/d.jar");
        assertEquals(2, urls.size());
        assertEquals("/opt/b.jar", urls.get(0).getPath());
        assertEquals("/opt/d.jar", urls.get(1).getPath());
    }

    @Test
    public void testAddClassPathURLAddsJrtSchemeWhenSupported() {
        Assume.assumeTrue(schemeSupported("jrt"));
        assertEquals(1, collect("jrt:/java.base/module-info.class").size());
    }

    @Test
    public void testAddClassPathURLAddsJmodSchemeWhenSupported() {
        Assume.assumeTrue(schemeSupported("jmod"));
        assertEquals(1, collect("jmod:/x").size());
    }
}
