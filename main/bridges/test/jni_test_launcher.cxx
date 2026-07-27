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

// Minimal in-process JVM host for native-bridge tests.
//
// Why this exists: the UNO JNI bridge loads its native libraries (java_uno.dll,
// sal3.dll, ...) at runtime via osl::Module — outside any load-time activation
// context. Those libraries are 32-bit /MD builds that need a VC90-CRT activation
// context for every load. A stock java.exe provides none, so loading them raises
// R6034. This launcher carries the VC90-CRT manifest EMBEDDED (resource id 1),
// which sets a *process-default* activation context covering every later DLL the
// JVM loads — exactly how soffice.bin hosts the office JVM.
//
// Usage: jni_test_launcher <classpath> <java.library.path> <main.class.Name> [args...]
// jvm.dll is located via PATH (the test launcher sets PATH to the JDK dirs).

#include "jni.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

typedef jint(JNICALL * CreateJavaVM_t)(JavaVM **, void **, void *);

int main(int argc, char ** argv)
{
    if (argc < 4)
    {
        fprintf(stderr,
            "usage: jni_test_launcher <classpath> <library-path> <main-class> [args...]\n");
        return 2;
    }
    const char * classpath = argv[1];
    const char * libpath   = argv[2];
    const char * mainclass = argv[3];   // slash form, e.g. test/java_uno/anytest/TestJni

    HMODULE jvmlib = LoadLibraryA("jvm.dll");
    if (!jvmlib)
    {
        fprintf(stderr, "jni_test_launcher: cannot load jvm.dll (error %lu)\n", GetLastError());
        return 2;
    }
    CreateJavaVM_t pCreateJavaVM = (CreateJavaVM_t) GetProcAddress(jvmlib, "JNI_CreateJavaVM");
    if (!pCreateJavaVM)
    {
        fprintf(stderr, "jni_test_launcher: JNI_CreateJavaVM not found in jvm.dll\n");
        return 2;
    }

    char cpOpt[32768];
    char lpOpt[32768];
    _snprintf(cpOpt, sizeof(cpOpt) - 1, "-Djava.class.path=%s", classpath);
    _snprintf(lpOpt, sizeof(lpOpt) - 1, "-Djava.library.path=%s", libpath);
    cpOpt[sizeof(cpOpt) - 1] = 0;
    lpOpt[sizeof(lpOpt) - 1] = 0;

    JavaVMOption opts[2];
    opts[0].optionString = cpOpt;
    opts[1].optionString = lpOpt;

    JavaVMInitArgs vmArgs;
    memset(&vmArgs, 0, sizeof(vmArgs));
    vmArgs.version            = JNI_VERSION_1_2;
    vmArgs.nOptions           = 2;
    vmArgs.options            = opts;
    vmArgs.ignoreUnrecognized = JNI_FALSE;

    JavaVM * jvm = 0;
    JNIEnv * env = 0;
    if (pCreateJavaVM(&jvm, (void **) &env, &vmArgs) != JNI_OK)
    {
        fprintf(stderr, "jni_test_launcher: JNI_CreateJavaVM failed\n");
        return 2;
    }

    int rc = 0;
    jclass cls = env->FindClass(mainclass);
    if (!cls)
    {
        env->ExceptionDescribe();
        rc = 2;
    }
    else
    {
        jmethodID mid = env->GetStaticMethodID(cls, "main", "([Ljava/lang/String;)V");
        if (!mid)
        {
            env->ExceptionDescribe();
            rc = 2;
        }
        else
        {
            jclass strCls = env->FindClass("java/lang/String");
            int nApp = argc - 4;
            jobjectArray jargs = env->NewObjectArray(nApp, strCls, 0);
            for (int i = 0; i < nApp; ++i)
                env->SetObjectArrayElement(jargs, i, env->NewStringUTF(argv[4 + i]));

            // A failing test calls System.exit(1), terminating this process with
            // that code before the call returns. A passing test returns normally.
            env->CallStaticVoidMethod(cls, mid, jargs);
            if (env->ExceptionCheck())
            {
                env->ExceptionDescribe();
                rc = 1;
            }
        }
    }

    jvm->DestroyJavaVM();
    return rc;
}
