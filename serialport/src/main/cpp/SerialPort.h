#include <jni.h>

#ifndef _Included_com_dazhi_serialport_JniSerialPort
#define _Included_com_dazhi_serialport_JniSerialPort

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 作者：WangZezhi  (2020/5/19  11:46)
 * 功能：打开串口
 * 描述：
 */
JNIEXPORT jobject JNICALL Java_com_dazhi_serialport_JniSerialPort_open
  (JNIEnv *, jobject, jstring, jint, jint, jint, jchar);

/**
 * 作者：WangZezhi  (2020/5/19  11:49)
 * 功能：关闭串口
 * 描述：
 */
JNIEXPORT void JNICALL Java_com_dazhi_serialport_JniSerialPort_close
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif

#endif //_Included_com_dazhi_serialport_JniSerialPort
