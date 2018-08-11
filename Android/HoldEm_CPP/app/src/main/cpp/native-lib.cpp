#include <jni.h>
#include <string>
#include "hand_percentages.hpp"

extern "C"
{

/* More info here: https://www3.ntu.edu.sg/home/ehchua/programming/java/JavaNativeInterface.html#zz-4.3 */

// Convert info to double array of percentages
JNIEXPORT jdoubleArray JNICALL
Java_com_example_miles_holdem_1cpp_HandPercentages_getPercentages(JNIEnv* env, jobject, jintArray javaInfo, jint javaFlopTurnRiver) {
    jint* info = env->GetIntArrayElements(javaInfo, 0);

    HandPercentages hp{info, javaFlopTurnRiver};
    jdouble* cppPercentages = hp.getHandPercentages();

    env->ReleaseIntArrayElements(javaInfo, info, 0);

    jdoubleArray javaPercentages = env->NewDoubleArray(10);
    env->SetDoubleArrayRegion(javaPercentages, 0, 10, cppPercentages);
    return javaPercentages;
}



}

