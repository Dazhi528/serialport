package com.dazhi.serialport;

import java.io.ByteArrayOutputStream;

/**
 * 功能：常用(common)扩展读写方式
 * 描述：此类是常用的读写方式,如果不适合需求,可继承SerialPort类自己实现
 * 作者：WangZezhi
 * 邮箱：wangzezhi528@163.com
 * 创建日期：2020/5/19 15:20
 * 修改日期：2020/5/19 15:20
 */
@SuppressWarnings("WeakerAccess")
public class CmnSerialPort extends SerialPort {

    @SuppressWarnings("ResultOfMethodCallIgnored")
    public synchronized void clear() {
        if (isOpen()) {
            try {
                while (fileInputStream.available() > 0) { //available可获得
                    fileInputStream.read();
                }
            }catch (Exception e){
                e.printStackTrace();
            }
        } else {
            reOpen();
        }
    }

    public synchronized byte[] read() {
        ByteArrayOutputStream arrayOut=null;
        try {
            if (fileInputStream.available() > 0) {
                arrayOut = new ByteArrayOutputStream();
                byte[] temp = new byte[256];
                while (fileInputStream.available() > 0) {
                    int dataSize = fileInputStream.read(temp);
                    arrayOut.write(temp, 0, dataSize);
                }
                return arrayOut.toByteArray();
            } else {
                return null;
            }
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        } finally {
            if(arrayOut!=null) {
                try {
                    arrayOut.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void send(byte[] bytes) {
        if(bytes==null || bytes.length==0) {
            return;
        }
        send(bytes, 0, bytes.length);
    }
    public synchronized void send(byte[] bytes, int offset, int len) {
        try {
            fileOutputStream.write(bytes, offset, len);
            fileOutputStream.flush();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}
