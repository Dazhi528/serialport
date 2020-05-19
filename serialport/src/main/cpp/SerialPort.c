#include <jni.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "SerialPort.h"


// 私有方法,没有在头文件暴漏出去
static speed_t getBaudrate(jint baudrate);
static void setParameter(jint fd, jint databits, jint stopbits, jchar parity);

/*
 * 作者：WangZezhi
 * 功能：打开串口
 * 详情：
 * @param:path 串口设备文件名（路径）
 * @param:baudrate波特率;
 * @param:databits数据位; 7; 8;
 * @param:stopbits停止位; 1; 2;
 * @param:parity奇偶校验; N无;O奇;E偶;
 * @returun:如果打开成功,该方法会返回path串口的文件描述实例（java/io/FileDescriptor）
 */
#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_ovl_ambiguous_call"
JNIEXPORT jobject JNICALL Java_com_dazhi_serialport_JniSerialPort_open (JNIEnv *env, jclass thiz,
                    jstring path, jint baudrate, jint databits, jint stopbits, jchar parity)
{
    int fd;
    speed_t speed;
    jobject mFileDescriptor;
    /* Check arguments */
    {
        speed = getBaudrate(baudrate);
        if (speed == NULL) {
            return NULL;
        }
    }
    /* Opening device */
    {
        jboolean iscopy;
        const char *path_utf = (*env)->GetStringUTFChars(env, path, &iscopy);
        fd = open(path_utf, O_RDWR);
        (*env)->ReleaseStringUTFChars(env, path, path_utf);
        if (fd == -1)
        {
            return NULL;
        }
    }
    /* Configure device */
    {
        struct termios cfg;
        if (tcgetattr(fd, &cfg))
        {
            close(fd);
            return NULL;
        }
        cfmakeraw(&cfg);
        cfsetispeed(&cfg, speed);
        cfsetospeed(&cfg, speed);
        if (tcsetattr(fd, TCSANOW, &cfg))
        {
            close(fd);
            return NULL;
        }
        setParameter(fd, databits, stopbits, parity);
    }
    /* Create a corresponding file descriptor */
    {
        jclass cFileDescriptor = (*env)->FindClass(env, "java/io/FileDescriptor");
        jmethodID iFileDescriptor = (*env)->GetMethodID(env, cFileDescriptor, "<init>", "()V");
        jfieldID descriptorID = (*env)->GetFieldID(env, cFileDescriptor, "descriptor", "I");
        mFileDescriptor = (*env)->NewObject(env, cFileDescriptor, iFileDescriptor);
        (*env)->SetIntField(env, mFileDescriptor, descriptorID, (jint)fd);
    }
    return mFileDescriptor;
}

/**
 * 作者：WangZezhi  (2020/5/19  11:56)
 * 功能：关闭串口
 * 描述：
 */
JNIEXPORT void JNICALL Java_com_dazhi_serialport_JniSerialPort_close (JNIEnv *env, jobject thiz)
{
    jclass SerialPortClass = (*env)->GetObjectClass(env, thiz);
    jclass FileDescriptorClass = (*env)->FindClass(env, "java/io/FileDescriptor");
    //fileDescriptor要和java文件里的对应
    jfieldID mFdID = (*env)->GetFieldID(env, SerialPortClass, "fileDescriptor", "Ljava/io/FileDescriptor;");
    jfieldID descriptorID = (*env)->GetFieldID(env, FileDescriptorClass, "descriptor", "I");
    jobject mFd = (*env)->GetObjectField(env, thiz, mFdID);
    jint descriptor = (*env)->GetIntField(env, mFd, descriptorID);
    close(descriptor);
}

/*
 * 作者：WangZezhi (16-11-10 下午2:30)
 * 功能：私有方法；获得波特率
 */
static speed_t getBaudrate(jint baudrate)
{
    switch(baudrate)
    {
        case 0: return B0;
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        case 200: return B200;
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        case 460800: return B460800;
        case 500000: return B500000;
        case 576000: return B576000;
        case 921600: return B921600;
        case 1000000: return B1000000;
        case 1152000: return B1152000;
        case 1500000: return B1500000;
        case 2000000: return B2000000;
        case 2500000: return B2500000;
        case 3000000: return B3000000;
        case 3500000: return B3500000;
        case 4000000: return B4000000;
        default: return NULL;
    }
}

/*
 * 作者：WangZezhi
 * 功能：@brief 设置串口数据位，停止位和效验位
 * 详情：
 *@param  fd       类型   int  打开的串口文件句柄
 *@param  databits 类型   int  数据位   取值为 7 或者 8
 *@param  stopbits 类型   int  停止位   取值为 1 或者 2
 *@param  parity   类型   int  效验类型 取值为 N,E,O,S
 */
static void setParameter(jint fd, jint databits, jint stopbits, jchar parity)
{
    struct termios options;
    if (tcgetattr(fd, &options)!=0)
    {
//        perror("SetupSerial 1");
        return;
    }
    options.c_cflag &= ~CSIZE;
    options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    options.c_oflag  &= ~OPOST;   /*Output*/
    /* 设置数据位数 */
    switch (databits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            return;
    }
    /* 设置停止位 */
    switch (stopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            return;
    }
    /* 设置奇偶校验位 */
    switch (parity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;   /* Clear parity enable */
            options.c_iflag &= ~INPCK;     /* Enable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验 */
            options.c_iflag |= INPCK;             /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;     /* Enable parity */
            options.c_cflag &= ~PARODD;   /* 转换为偶效验 */
            options.c_iflag |= INPCK;       /* Disnable parity checking */
            break;
        case 'S':
        case 's':  /*as no parity*/
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;break;
        default:
            return;
    }
    /* Set input parity option */
    if (parity != 'n') options.c_iflag |= INPCK;
    options.c_cc[VTIME]=150; /* options.c_cc[VTIME] = 0;设置超时0s*/
    options.c_cc[VMIN]=0; /* options.c_cc[VMIN] = 13;定义要读取的最小字节数据*/
    tcflush(fd,TCIFLUSH);
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
//        perror("SetupSerial 3");
        return;
    }
}
/*
在上述代码中，有两句话特别重要：
options.c_cc[VTIME] = 0; //设置超时 0 seconds
options.c_cc[VMIN] = 13; //定义要读取的最小字节数据
这两句话决定了对串口读取的函数 read() 的一些功能。我将着重介绍一下他们对 read() 函数的影响
对串口操作的结构体是
Struct{
       tcflag_t   c_iflag;    //输入模式标记
       tcflag_t   c_oflag;    //输出模式标记
       tcflag_t   c_cflag;    //控制模式标记
       tcflag_t   c_lflag;    //本地模式标记
       cc_t       c_line;     //线路规程
       cc_t       c_cc[NCCS]; //控制符号
} ；
其中 cc_t  c_line 只有在一些特殊的系统程序 (比如，设置通过 tty 设备来通信的网络协议 ) 中才会用
在数组 c_cc 中有两个下标 (VTIME 和 VMIN) 对应的元素不是控制符，并且只是在原始模式下有效。只有在
原始模式下，他们决定了 read() 函数在什么时候返回。在标准模式下，除非设置了 O_NONBLOCK 选项，否则
只有当遇到文件结束符或各含的字符都已经编辑完毕后才返回
控制符 VTIME 和 VMIN 之间有着复杂的关系。 VTIME 定义要求等待的零到几百号妙的是间量 ( 通常是一个
8 位的 unsigned char 变量，取值不能大于 cc_t) 。 VMIN 定义了要求等待的最小字节数 ( 不是要求读
的字节数—— read() 的第三个参数才是指定要求读的最大字节数 ) ，这个字节数可能是 0 。

如果 VTIME 取 0 ， VMIN 定义了要求等待读取的最小字节数。函数 read() 只有在读取了 VMIN 个字节
的数据或者收到一个信号的时候才返回。

如果 VMIN 取 0 ， VTIME 定义了即使没有数据可以读取， read() 函数返回前也要等待几百毫秒的时间量。
这时， read() 函数不需要像其通常情况那样要遇到一个文件结束标志才返回 0 。

如果 VTIME 和 VMIN 都不取 0 ， VTIME 定义的时当接收到底一个自己的数据后开始计算等待的时间量。
如果当调用 read 函数时可以得到数据，计时器马上开始计时。如果但调用 read 函数时还没有任何数据可读，
则等接收到底一个字节的数据后，计时器开始计时。函数 read 可能会在读取到 VMIN 个字节的数据后返回，
也可能在计时完毕后返回，这主要取决于哪个条件首先实现。不过函数至少会读取到一个字节的数据，因为
计时器是在读取到第一个数据时开始计时的。

如果 VTIME 和 VMIN 都取 0 ，即使读取不到任何数据，函数 read 也会立即返回。同时，返回值 0 表示
read 函数不需要等待文件结束标志就返回了。

这就是这两个变量对 read 函数的影响。我使用的读卡器每次传送的数据是 13 个字节，一开始，我把它们设置成
options.c_cc[VTIME] = 150
options.c_cc[VMIN] = 0;
结果，每次读取的信息只有 8 个字节，剩下的 5 个字节要等到下一次打卡时才能收到。就是由于这个原因造成的。
根据上面规则的第一条，我把 VTIME 取 0 ， VMIN=13 ，也就是正好等于一次需要接收的字节数。这样就实现了
一次读取 13 个字节值。同时，得出这样的结论，如果读卡器送出的数据为 n 个字节，那么就把 VMIN=n ，这样
一次读取的信息正好为读卡器送出的信息，并且读取的时候不需要进行循环读取。
*/

// 测试方法
JNIEXPORT jstring JNICALL
Java_com_dazhi_serialport_JniSerialPort_sayHello(JNIEnv *env, jclass thiz) {
    return (*env)->NewStringUTF(env, "Hello World!");
}