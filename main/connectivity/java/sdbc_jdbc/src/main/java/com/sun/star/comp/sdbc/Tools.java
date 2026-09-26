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
package com.sun.star.comp.sdbc;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.openoffice.comp.sdbc.dbtools.comphelper.ResourceBasedEventLogger;
import org.apache.openoffice.comp.sdbc.dbtools.util.StandardSQLState;

import com.sun.star.beans.NamedValue;
import com.sun.star.beans.PropertyValue;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.logging.LogLevel;
import com.sun.star.sdbc.SQLException;
import com.sun.star.uno.Any;
import com.sun.star.uno.AnyConverter;

public class Tools {
    private static final int MAX_EXCEPTION_NESTING = 8;

    private static final Logger LOGGER = Logger.getLogger(Tools.class.getName());

    /**
     * URL schemes that resolve to the local filesystem or the running JVM image.
     *
     * <p>jvmaccess/source/classpath.cxx enforces the same allow-list in C++ for
     * the UNO bootstrap class path; keep the two in sync.</p>
     */
    private static final Set<String> LOCAL_PROTOCOLS =
            Collections.unmodifiableSet(new HashSet<>(Arrays.asList("file", "jrt", "jmod")));

    public static SQLException toUnoException(Object source, Throwable throwable) {
        return toUnoException(source, throwable, 0);
    }

    private static SQLException toUnoException(Object source, Throwable throwable, int nesting) {
        // FIXME: use SQLException.getNextException() instead of getCause()?
        // There are up to 3 dimensions of exception chaining of warnings in Java,
        // getCause(), getNextException(), and getNextWarning().
        // The C++ implementation used getNextException() only,
        // but I am using the widely used and more helpful getCause().
        Throwable cause = throwable.getCause();
        Object unoCause = Any.VOID;
        // Avoid loops and limit recursion to prevent stack overflows:
        if (cause != null && cause != throwable && nesting < MAX_EXCEPTION_NESTING) {
            unoCause = toUnoException(source, throwable, nesting + 1);
        }
        if (throwable instanceof SQLException) {
            return (SQLException)throwable;
        } else if (throwable instanceof java.sql.SQLException) {
            java.sql.SQLException sqlException = (java.sql.SQLException) throwable;
            return new SQLException(sqlException.getMessage(), source,
                    sqlException.getSQLState(), sqlException.getErrorCode(), unoCause);
        } else if (throwable instanceof com.sun.star.uno.Exception) {
            // General UNO exception. Wrap in an SQLException and rethrow:
            com.sun.star.uno.Exception exception = (com.sun.star.uno.Exception) throwable;
            return new SQLException(exception.getMessage(), source, StandardSQLState.SQL_GENERAL_ERROR.text(),
                    0, exception);
        } else {
            // General Java exception. We can't pass this to UNO, so convert it to an UNO SQLException:
            String message = throwable.getMessage();
            if (message.isEmpty()) {
                message = throwable.getLocalizedMessage();
            }
            if (message.isEmpty()) {
                message = throwable.toString();
            }
            return new SQLException(message, source, "", -1, unoCause);
        }
    }

    public static SQLException toUnoExceptionLogged(Object source, ResourceBasedEventLogger logger, Throwable throwable) {

        SQLException exception = toUnoException(source, throwable);
        logger.log(LogLevel.SEVERE, exception);
        return exception;
    }

    public static String getOrDefault(PropertyValue[] properties, String name, String defaultValue) throws IllegalArgumentException {
        String ret = defaultValue;
        for (PropertyValue property : properties) {
            if (property.Name.equals(name)) {
                ret = AnyConverter.toString(property.Value);
                break;
            }
        }
        return ret;
    }

    public static boolean getOrDefault(PropertyValue[] properties, String name, boolean defaultValue) throws IllegalArgumentException {
        boolean ret = defaultValue;
        for (PropertyValue property : properties) {
            if (property.Name.equals(name)) {
                ret = AnyConverter.toBoolean(property.Value);
                break;
            }
        }
        return ret;
    }

    public static Object getOrDefault(PropertyValue[] properties, String name, Object defaultValue) throws IllegalArgumentException {
        Object ret = defaultValue;
        for (PropertyValue property : properties) {
            if (property.Name.equals(name)) {
                ret = property.Value;
                break;
            }
        }
        return ret;
    }

    public static NamedValue[] getOrDefault(PropertyValue[] properties, String name, NamedValue[] defaultValue) throws IllegalArgumentException {
        NamedValue[] ret = defaultValue;
        for (PropertyValue property : properties) {
            if (property.Name.equals(name)) {
                Object[] array = (Object[]) AnyConverter.toArray(property.Value);
                ret = new NamedValue[array.length];
                for (int i = 0; i < array.length; i++) {
                    ret[i] = (NamedValue) array[i];
                }
                break;
            }
        }
        return ret;
    }

    /**
     * Appends a class path entry to the list of URLs used to build a class loader.
     *
     * <p>Only local entries or a jar: wrapping a local entry are added; a file: entry must
     * in addition name a path on this machine. A malformed or non-local entry is logged and
     * skipped.</p>
     *
     * @param urls the list of class path URLs to append to
     * @param url  the class path entry to parse and validate
     */
    public static void addClassPathURL(Collection<URL> urls, String url) {
        URL javaURL;
        URL effectiveURL;
        try {
            javaURL = new URL(url);
            effectiveURL = getEffectiveURL(javaURL);
        } catch (MalformedURLException e) {
            LOGGER.log(Level.WARNING, e, () -> "Skipping malformed class path entry: " + url);
            return;
        }
        String protocol = effectiveURL.getProtocol();
        if (LOCAL_PROTOCOLS.contains(protocol)
                && (!"file".equals(protocol) || isLocalFileLocation(effectiveURL))) {
            LOGGER.fine(() -> "Adding class path entry: " + url);
            urls.add(javaURL);
        } else {
            LOGGER.warning(() -> "Skipping non-local class path entry: " + url);
        }
    }

    /**
     * Returns the URL that actually locates the resource.
     *
     * <p>Since {@code jar:} only wraps another URL, that wrapped URL is returned.
     * For any other URL the URL itself is returned.</p>
     *
     * @param url the class path URL to inspect
     * @return the effective URL
     * @throws MalformedURLException if the wrapped jar: URL cannot be parsed
     */
    private static URL getEffectiveURL(URL url) throws MalformedURLException {
        if (!"jar".equals(url.getProtocol())) {
            return url;
        }
        String path = url.getPath();
        int separator = path.lastIndexOf("!/");
        return new URL(separator == -1 ? path : path.substring(0, separator));
    }

    /**
     * Tells whether a file: URL names a path on this machine.
     *
     * <p>The host must be empty or {@code localhost}, and the path must not name another
     * machine by itself. The path is judged decoded, because the file: handler
     * percent-decodes it and, on Windows, turns slashes into backslashes before opening
     * it; a leading escaped slash or backslash would otherwise reach the file system as a
     * reference to a share.</p>
     *
     * @param url a URL with the file: scheme
     * @return whether the URL names a local path
     */
    private static boolean isLocalFileLocation(URL url) {
        String host = url.getHost();
        if (host != null && !host.isEmpty() && !"localhost".equalsIgnoreCase(host)) {
            return false;
        }
        String path = percentDecoded(url.getPath());
        return path.length() >= 2 && path.charAt(0) == '/' && path.charAt(1) != '/'
                && path.indexOf('\\') == -1;
    }

    /**
     * Undoes one level of %HH escapes, as the file: handler does. Only the ASCII
     * separators matter to the caller, so each escape becomes the character of its byte
     * value; a malformed escape is kept.
     */
    private static String percentDecoded(String s) {
        StringBuilder buf = new StringBuilder(s.length());
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            if (c == '%' && s.length() - i > 2) {
                int hi = Character.digit(s.charAt(i + 1), 16);
                int lo = Character.digit(s.charAt(i + 2), 16);
                if (hi != -1 && lo != -1) {
                    c = (char) (hi * 16 + lo);
                    i += 2;
                }
            }
            buf.append(c);
        }
        return buf.toString();
    }
}
