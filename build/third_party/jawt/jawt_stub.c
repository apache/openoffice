/*
 * Stub whose only product is an import library for the JDK's jawt.dll; see
 * BUILD.bazel.  Never staged, never called.  The declaration comes from the
 * JDK's own jawt.h, so the calling convention -- and therefore the decorated
 * export name on x86 -- is the one the JDK uses.
 */
#include "jawt.h"

JNIEXPORT jboolean JNICALL JAWT_GetAWT(JNIEnv* env, JAWT* awt)
{
    (void)env;
    (void)awt;
    return JNI_FALSE;
}
