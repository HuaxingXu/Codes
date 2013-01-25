/* 
 * File:   UdpChat.h
 * Author: yzf
 *
 * Created on January 7, 2013, 10:35 AM
 */

#ifndef UDPCHAT_H
#define	UDPCHAT_H

#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Voice.h"
#include "Player.h"

#define UDP_PORT 30000//本地默认使用的端口
#define Local_BUFFER_SIZE 3200 //32000 1s数据
#define PACKAGE_SIZE 420

class UdpChat {
public:
    UdpChat();
    ~UdpChat();
    //接收和播放部分
    bool beginRecvVoiceAsyn();//后台接收语音流
    void stopRecvVoice();//停止接收语音流
    //录音和发送部分
    bool beginRecordVoiceAysn();//后台录音
    void stopRecordVoice();//停止后台录音
private:
    //接收和播放部分
    static void *_runRecving(void *obj);//接收语音
    void addToPlayBuffer(char *data, unsigned int len);//把语音数据添加到播放缓冲区
    void addToLocalBuffer(char *data, unsigned int len);//把语音流加入到本地缓冲区
    //录音和发送部分
    static void *_runRecording(void *obj);//录音
    void sendVoice(short data[], int count);//发送语音流
private:
    pthread_t m_recvID;
    pthread_t m_recordID;
    Voice m_voice;
    Player m_player;
    
    int m_offset;//当前缓冲数据的偏移量
    char m_localBuffer[Local_BUFFER_SIZE];//本地缓冲区
    
    int m_sendFd;//管道文件
    int m_recvFd;//管道文件
};


#endif	/* UDPCHAT_H */

