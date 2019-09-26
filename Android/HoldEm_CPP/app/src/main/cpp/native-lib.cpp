#include <jni.h>
#include <string>
#include "hand_percentages.hpp"
#include "win_percentages.hpp"

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

    JNIEXPORT jobjectArray JNICALL
    Java_com_example_miles_holdem_1cpp_WinPercentagesBackgroundTask_getWinPercentages(JNIEnv* env, jobject, jintArray javaInfo) {
        jint* info = env->GetIntArrayElements(javaInfo, 0);
        jint numPlayers = info[0];

        WinPercentages wp{info};
        wp.getWinAndTiePercentages();

        env->ReleaseIntArrayElements(javaInfo, info, 0);

        jclass doubleArray1DClass = env->FindClass("[D");
        jobjectArray javaPercentages = env->NewObjectArray(2, doubleArray1DClass, NULL);
        jdoubleArray javaWinPercentages = env->NewDoubleArray(numPlayers);
        jdoubleArray javaTiePercentages = env->NewDoubleArray(numPlayers);

        env->SetDoubleArrayRegion(javaWinPercentages, 0, numPlayers, wp.winPercentages_);
        env->SetDoubleArrayRegion(javaTiePercentages, 0, numPlayers, wp.tiePercentages_);

        env->SetObjectArrayElement(javaPercentages, 0, javaWinPercentages);
        env->SetObjectArrayElement(javaPercentages, 1, javaTiePercentages);

        return javaPercentages;
    }

}
