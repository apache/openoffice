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



package test.java_uno.anytest;

import java.util.Arrays;

// Round-trips large sequences of several element widths through the real JNI
// bridge (XTransport.mapAny). The Java array argument is marshalled into a UNO
// sequence on the native side, which is where the sequence buffer is allocated
// from the element count and element size; sending element widths of 1/2/4/8
// bytes with a non-trivial count and verifying the data survives confirms that
// allocation handles a large, legitimate sequence and copies it intact.
//
// The element count is kept to a few million so the arrays fit a default 32-bit
// JVM heap. Triggering the size computation's upper limit would need a >4 GB
// array, which a 32-bit JVM cannot allocate, so this exercises the accepted side
// only; the rejection side is reviewed in the native guard.
final class TestSeqSize {
    private static final int N = 1000000;

    // A SeqSizeBig element is 1024 bytes in the UNO binary layout, so a sequence
    // of this many elements has a size (count * 1024) that exceeds the 32-bit
    // allocation ceiling (> 4 GiB). The Java array holds only null references
    // (~20 MB), so the oversized condition is reached without allocating the
    // gigabytes of element data the size would otherwise imply.
    private static final int OVERFLOW_N = 5000000;

    public static boolean test(XTransport transport) {
        boolean success = true;
        success &= roundTripByte(transport);
        success &= roundTripShort(transport);
        success &= roundTripInt(transport);
        success &= roundTripLong(transport);
        success &= roundTripDouble(transport);
        success &= rejectsOversizedSequence(transport);
        if (!success) {
            System.err.println("TestSeqSize: large-sequence round-trip failed!");
        }
        return success;
    }

    private static boolean rejectsOversizedSequence(XTransport transport) {
        // count * sizeof(SeqSizeBig) overflows the 32-bit allocation ceiling;
        // the bridge must reject it (RuntimeException) rather than wrap the size
        // and under-allocate. Elements stay null — the size is checked before
        // any element is read.
        SeqSizeBig[] in = new SeqSizeBig[OVERFLOW_N];
        try {
            transport.mapAny(in);
            System.err.println(
                "TestSeqSize: oversized sequence was NOT rejected!");
            return false;
        } catch (com.sun.star.uno.RuntimeException e) {
            // Must be the sequence-size guard, not some unrelated failure (e.g.
            // a type-resolution error, which is also a RuntimeException).
            String msg = e.getMessage();
            System.out.println("TestSeqSize: oversized sequence rejected: " + msg);
            boolean fromGuard = msg != null && msg.indexOf("out of range") >= 0;
            if (!fromGuard) {
                System.err.println(
                    "TestSeqSize: rejected, but not by the size guard!");
            }
            return fromGuard;
        }
    }

    private static boolean roundTripByte(XTransport transport) {
        byte[] in = new byte[N];
        for (int i = 0; i < N; ++i) {
            in[i] = (byte) (i * 31 + 7);
        }
        Object out = transport.mapAny(in);
        return (out instanceof byte[]) && Arrays.equals(in, (byte[]) out);
    }

    private static boolean roundTripShort(XTransport transport) {
        short[] in = new short[N];
        for (int i = 0; i < N; ++i) {
            in[i] = (short) (i * 131 + 7);
        }
        Object out = transport.mapAny(in);
        return (out instanceof short[]) && Arrays.equals(in, (short[]) out);
    }

    private static boolean roundTripInt(XTransport transport) {
        int[] in = new int[N];
        for (int i = 0; i < N; ++i) {
            in[i] = i * 16807 + 7;
        }
        Object out = transport.mapAny(in);
        return (out instanceof int[]) && Arrays.equals(in, (int[]) out);
    }

    private static boolean roundTripLong(XTransport transport) {
        long[] in = new long[N];
        for (int i = 0; i < N; ++i) {
            in[i] = (long) i * 2862933555777941757L + 7L;
        }
        Object out = transport.mapAny(in);
        return (out instanceof long[]) && Arrays.equals(in, (long[]) out);
    }

    private static boolean roundTripDouble(XTransport transport) {
        double[] in = new double[N];
        for (int i = 0; i < N; ++i) {
            in[i] = i * 0.5 - 3.0;
        }
        Object out = transport.mapAny(in);
        return (out instanceof double[]) && Arrays.equals(in, (double[]) out);
    }
}
