/* 
 * File:   UdpChat.h
 * Author: yzf
 *
 * Created on January 7, 2013, 10:35 AM
 */

#ifndef UDPCHAT_H
#define	UDPCHAT_H

#include <pthread.h>

#include "UdpTransfer.h"
#include "Voice.h"
#include "Player.h"

#define UDP_PORT 30000//本地默认使用的端口

class UdpChat {
public:
    UdpChat(const char *target_ip, int target_port, int local_port = UDP_PORT);
    ~UdpChat();
    void resetTarget(const char *target_ip, int target_port);//重置对方地址
    //接收和播放部分
    bool beginRecvVoiceAsyn();//后台接收语音流
    void stopRecvVoice();//停止接收语音流
    //录音和发送部分
    bool beginRecordVoiceAysn();//后台录音
    void stopRecordVoice();//停止后台录音
private:
    //接收和播放部分
    static void *_runRecving(void *obj);//接收语音
    void addToPlayBuffer(char *data, int len);//把语音数据添加到播放缓冲区
    void addToLocalBuffer();//把语音数据添加到本地缓冲区
    //录音和发送部分
    static void *_runRecording(void *obj);//录音
    void sendVoice(short data[], int count);//发送语音流
private:
    pthread_t m_recvID;
    pthread_t m_recordID;
    Voice m_voice;
    UdpTransfer m_udpTransfer;
    Player m_player;
};


#endif	/* UDPCHAT_H */

