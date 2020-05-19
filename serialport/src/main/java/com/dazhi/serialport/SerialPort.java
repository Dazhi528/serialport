package com.dazhi.serialport;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * 功能：公共串口工具类
 * 描述：收/发数据即操作输入/出文件流,根据需要自己实现
 * 作者：WangZezhi
 * 邮箱：wangzezhi528@163.com
 * 创建日期：2020/5/19 14:59
 * 修改日期：2020/5/19 14:59
 */
@SuppressWarnings({"unused", "WeakerAccess"})
public class SerialPort {
    protected FileInputStream fileInputStream; // protected方便子类扩展
    protected FileOutputStream fileOutputStream;
    //串口默认配置参数:
    private JniSerialPort jniSerialPort;
    private String sDevice = "/dev/ttyS1";  //默认串口设备文件名
    private int iBaudRate = 9600; //波特率
    private int iDatabits = 8; //数据位; 7; 8;
    private int iStopbits = 1; //停止位; 1; 2;
    private char cParity = 'N'; //奇偶校验; N无;O奇;E偶;

    /*
     * 作者：WangZezhi
     * 功能：打开串口
     * 详情：
     * @param:sDevice 串口设备文件名（路径）
     * @param:iBaudRate波特率;
     * @param:iDatabits数据位; 7; 8;
     * @param:iStopbits停止位; 1; 2;
     * @param:cParity奇偶校验; N无;O奇;E偶;
     * @returun:如果打开成功,该方法会返回path串口的文件描述实例（java/io/FileDescriptor）
     */
    public void setParameter(String sDevice, int iBaudRate, int iDatabits, int iStopbits, char cParity) {
        this.sDevice=sDevice;
        this.iBaudRate=iBaudRate;
        this.iDatabits=iDatabits;
        this.iStopbits=iStopbits;
        this.cParity=cParity;
    }

    public void open() throws Exception {
        if (sDevice == null || sDevice.length() == 0) {
            throw new SecurityException("Device path cannot be null");
        }
        File device = new File(sDevice);
        if (!device.exists()) {
            throw new SecurityException("Device path setting error");
        }
        //如果没有读/写权限,尝试去修改访问权限
        if (!device.canRead() || !device.canWrite()) {
            Process su = Runtime.getRuntime().exec("/system/bin/su");
            String cmd = "chmod 666 " + device.getAbsolutePath() + "\n" + "exit\n";
            su.getOutputStream().write(cmd.getBytes());
            if ((su.waitFor() != 0) || !device.canRead() || !device.canWrite()) {
                //手动抛出自指定异常
                throw new SecurityException("No read-write permission");
            }
        }
        //调用jni打开串口函数,返回文件描述符类
        jniSerialPort=new JniSerialPort();
        FileDescriptor fileDescriptor = jniSerialPort.open(sDevice, iBaudRate, iDatabits, iStopbits, cParity);
        if (fileDescriptor == null) {
            //手动抛出自指定异常
            throw new SecurityException("open fail");
        }
        fileInputStream = new FileInputStream(fileDescriptor);
        fileOutputStream = new FileOutputStream(fileDescriptor);
    }

    public boolean isOpen() {
        return jniSerialPort != null && fileInputStream != null && fileOutputStream != null;
    }

    public void close() {
        if(fileInputStream!=null) {
            try {
                fileInputStream.close();
                fileInputStream=null;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        if(fileOutputStream!=null) {
            try {
                fileOutputStream.close();
                fileOutputStream=null;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        if(jniSerialPort!=null) {
            jniSerialPort.close();
            jniSerialPort=null;
        }
    }

    public boolean reOpen() {
        close();
        try {
            open();
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    /*=======================================
     * 作者：WangZezhi  (2020/5/19  14:54)
     * 功能：get/set
     * 描述：
     *=======================================*/
    public FileInputStream getFileInputStream() {
        return fileInputStream;
    }

    public FileOutputStream getFileOutputStream() {
        return fileOutputStream;
    }

    public String getsDevice() {
        return sDevice;
    }

    public SerialPort setsDevice(String sDevice) {
        this.sDevice = sDevice;
        return this;
    }

    public int getiBaudRate() {
        return iBaudRate;
    }

    public SerialPort setiBaudRate(int iBaudRate) {
        this.iBaudRate = iBaudRate;
        return this;
    }

    public int getiDatabits() {
        return iDatabits;
    }

    public SerialPort setiDatabits(int iDatabits) {
        this.iDatabits = iDatabits;
        return this;
    }

    public int getiStopbits() {
        return iStopbits;
    }

    public SerialPort setiStopbits(int iStopbits) {
        this.iStopbits = iStopbits;
        return this;
    }

    public char getcParity() {
        return cParity;
    }

    public SerialPort setcParity(char cParity) {
        this.cParity = cParity;
        return this;
    }

}
