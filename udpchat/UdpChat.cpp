#include <cstdio>
#include <cstring>

#include "UdpChat.h"
#include "Recorder.h"
/*
 * const char *target_ip 对方ip
 * int target_port 对方使用的端口
 */
UdpChat::UdpChat(const char *target_ip, int target_port, int local_port) 
            : m_udpTransfer(target_ip, target_port, local_port) {
    printf("local using port %d\n", local_port);
}

UdpChat::~UdpChat() {
    
}
/*
 * 重置对方地址
 * const char *target_ip 对方ip
 * int target_port 对方使用的端口
 */
void UdpChat::resetTarget(const char* target_ip, int target_port) {
    this->m_udpTransfer.resetTarget(target_ip, target_port);
}

/*
 * 把语音流加入到声卡缓冲区
 * char *data 语音流数据
 * int len 数据长度
 */
void UdpChat::addToPlayBuffer(char *data, int len) {
    printf("UdpChat::addToPlayBuffer()\n");
    this->m_player.play(data, len);
}
/*
 * 把语音流加入到本地缓冲区
 */
void UdpChat::addToLocalBuffer() {
    printf("UdpChat::addToLocalBuffer()\n");
}
/*
 * 创建线程后台进行录音
 */
bool UdpChat::beginRecvVoiceAsyn() {
    printf("UdpChat::beginRecvVoiceAsyn()\n");
    if (pthread_create(&this->m_recvID, NULL, UdpChat::_runRecving, this)) {
        return false;
    }
    return true;
}
/*
 * 接收语音流并加入到缓冲区
 * void *obj   UdpChat的指针
 */
void *UdpChat::_runRecving(void *obj) {
    printf("UdpChat::_runRecving()\n");
    UdpChat *uc = (UdpChat *)obj;
//    FILE *fp = fopen("recv.raw", "w");
    char recv_buffer[UDP_MAX_SIZE];
    short decoded[MAX_SHORT_SIZE];
    int recv_len;
    int decoded_count;
//    int write_count;
    int i = 1;
    //等待接收
    do {
        recv_len = uc->m_udpTransfer.recv(recv_buffer, UDP_MAX_SIZE);
        //将收到的数据解压
        decoded_count = uc->m_voice.decode(recv_buffer, recv_len, decoded, MAX_SHORT_SIZE);
        //将数据拷贝到本地的一个缓冲区
        uc->addToPlayBuffer((char *)decoded, decoded_count * 2);
//        write_count = fwrite(decoded, sizeof(short), decoded_count, fp);
//        if (write_count != decoded_count * 2) {
//            printf("short write\n");
//        }
        printf("%d ", i ++);
        printf("recv %d -> %d bytes\n", recv_len, decoded_count * 2);
        
    } while (true);
//    fclose(fp);

    pthread_exit(NULL);
}
/*
 * 停止接收操作
 */
void UdpChat::stopRecvVoice() {
    printf("UdpChat::stopRecvVoice %lu\n", this->m_recvID);
    pthread_cancel(this->m_recvID);
}
/*
 * 发送语音流
 * short data[] 语音流数据
 * int count 数据长度
 */
void UdpChat::sendVoice(short data[], int count) {
    printf("UdpChat::sendVoice()\n");
    if (count <= 0) {
        return ;
    }
    char encoded[MAX_CHAR_SIZE];
    int byte_count;
    //压缩语音数据
    byte_count = this->m_voice.encode(data, count, encoded, MAX_CHAR_SIZE);
    //发送语音数据
    this->m_udpTransfer.send(encoded, byte_count);
}
/*
 * 创建线程后台进行录音操作
 */
bool UdpChat::beginRecordVoiceAysn() {
    printf("UdpChat::beginRecordVoiceAysn()\n");
    if (pthread_create(&this->m_recordID, NULL, UdpChat::_runRecording, this)) {
        return false;
    }
    return true;
}
/*
 * 录音并发送
 * void *obj 为 UdpChat *类型
 */
void *UdpChat::_runRecording(void *obj) {
    printf("UdpChat::_runRecording()\n");
    UdpChat *uc = (UdpChat *)obj;
    Recorder rc = Recorder();
    char record_data[MAX_CHAR_SIZE];
    while (true) {
        size_t read_count = rc.record(record_data, MAX_CHAR_SIZE);
        printf("record %d\n", read_count);
        uc->sendVoice((short *)record_data, read_count / 2);
    }
    pthread_exit(NULL);
}
/*
 * 停止录音操作
 */
void UdpChat::stopRecordVoice() {
    printf("UdpChat::stopRecordVoice %lu\n", this->m_recordID);
    pthread_cancel(this->m_recordID);
}
