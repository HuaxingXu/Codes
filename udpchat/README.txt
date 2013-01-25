基于UDP数据传输的语音聊天程序（linux）
使用说明：
1. 配置环境（里面使用到speex语音压缩开源项目）
    a) 下载 speex，网址： http://www.speex.org/downloads/
    b) 安装 speex
        tar zxvf speex-1.2rc1.tar.gz
        cd speex-1.2rc1
        ./configure --prefix=/home/yzf/lib/speex   (路径改成自己喜欢的)
        make && make install

        编译安装后，把/home/yzf/lib/speex/include 下的文件拷贝到 /usr/include下
        把/home/yzf/lib/speex/lib/libspeex.so.1.5.0 拷贝到 /usr/lib下
        并建立该文件的软链接 
            libspeex.so  :  ln -s libspeex.so.1.5.0 libspeex.so
            libspeex.so.1  :  ln -s libspeex.so.1.5.0 libspeex.so.1
        因为有些系统-lspeex使用的是 libspeex.so，有些则是 libspeex.so.1
    *c) 下载 alsa，网址：ftp://ftp.alsa-project.org/pub/lib/
    *d) 安装 alsa 
        tar jxvf alsa-lib-1.0.24.1.tar.bz2
        cd alsa-lib-1.0.24.1
        ./configure --prefix=/home/yzf/lib/alsa
        make && make install
        编译安装后，把/home/yzf/lib/alsa/include 下的文件拷贝到 /usr/include下
        把/home/yzf/lib/alsa/lib/libasound.so.2.0.0 拷贝到 /usr/lib下
        并建立该文件的软链接
        ln -s libasound.so.2.0.0 libasound.so
    *为可选项目，因为有些系统已经自带了这些库

2. 编译程序
    cd udpchat
    make
    编译成功后，目录下会生成udpchat，如果编译不成功，可能是speex没配置好，或者缺少其他库，
    里面用到的库有 -pthread -lspeex -lm -lasound
    因为除了speex外，linux应该自带了这些库，所以他们的配置就不说了;

3. 运行
    ./udpchat 对方ip 对方端口 （默认本地使用端口30000）
    或
    ./udpchat 对方ip 对方端口 本地端口
    运行后，根据不同命令进行不同的操作
    1   开始语音聊天
    2   停止语音聊天
    3 ip port    重置对方的地址
    其他  退出程序

PS：
相关文件说明：
Player.h Player.cpp 播放器类，封装了alsa播放的接口
Recorder.h Recorder.cpp 录音类，封装了alsa录音的接口
以上可参考 alsa 官网的文档 
Voice.h Voice.cpp 封装了speex的接口
有关speex接口的说明，可参考speex的官方文档
UdpTransfer.h UdpTransfer.cpp 封装的udp发送接收的接口
UdpChat.h UdpChat.cpp 整合了以上说到的所有功能
udpchat.cpp 主程序
