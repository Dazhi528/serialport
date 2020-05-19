package com.dazhi.serialport;

import java.io.FileDescriptor;

/**
 * 功能：JNI类
 * 描述：此类只用在本库中,统一出口为SerialPort类
 * 作者：WangZezhi
 * 邮箱：wangzezhi528@163.com
 * 创建日期：2020/5/19 11:17
 * 修改日期：2020/5/19 11:17
 */
class JniSerialPort {

    static {
        System.loadLibrary("serial-port-lib");
    }

    native FileDescriptor open(String path, int baudrate, int databits, int stopbits, char parity);

    native void close();

}
