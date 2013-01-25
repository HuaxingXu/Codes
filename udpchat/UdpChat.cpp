#include <cstdio>
#include <cstring>

#include "UdpChat.h"
#include "Recorder.h"
#include "Player.h"
#define DEBUG
#undef DEBUG

//调试函数运行时间
#define DGTIME
#undef DGTIME

#ifdef DGTIME
#include <time.h>
#endif
/*
 * const char *target_ip 对方ip
 * int target_port 对方使用的端口
 */
UdpChat::UdpChat() : m_offset(0) {
    printf("UdpChat::UdpChat()\n");
    //打开管道文件
    this->m_sendFd = open("send.fifo", O_RDWR, 0775);
    if (this->m_sendFd == -1) {
        printf("send.fifo open fail\n");
        exit(1);
    }
    this->m_recvFd = open("recv.fifo", O_RDWR, 0775);
    if (this->m_recvFd == -1) {
        printf("recv.fifo open fail\n");
        exit(1);
    }
}

UdpChat::~UdpChat() {
    printf("UdpChat::~UdpChat()\n");
    //关闭管道文件
    close(this->m_sendFd);
    close(this->m_recvFd);
}

/*
 * 把语音流加入到声卡缓冲区
 * char *data 语音流数据
 * int len 数据长度
 */
void UdpChat::addToPlayBuffer(char *data, unsigned int len) {
    printf("UdpChat::addToPlayBuffer()\n");
    this->m_player.play(data, len);
}
/*
 * 把语音流添加到本地缓冲区，如果缓冲区满了，则添加到声卡缓冲区播放
 * char *data 语音流数据
 * int len 数据长度
 */
void UdpChat::addToLocalBuffer(char *data, unsigned int len) {
    if (this->m_offset + len >= Local_BUFFER_SIZE) {
//        addToPlayBuffer(this->m_localBuffer, this->m_offset);
//这里没必要再封装一层函数了
		this->m_player.play(this->m_localBuffer , this->m_offset);
        this->m_offset = 0;
    }
    memcpy(this->m_localBuffer + this->m_offset,
                data, len);
    this->m_offset += len;
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
//    printf("UdpChat::_runRecving()\n");
    UdpChat *uc = (UdpChat *)obj;
    char recv_buffer[PACKAGE_SIZE];
    short decoded[MAX_SHORT_SIZE];
    int recv_len;
    int decoded_count;

//    FILE *fp = fopen("data.raw", "a");
//    int write_count;
//    等待接收
    do {

#ifdef DGTIME
		clock_t start, end;
		start = clock();
#endif
        recv_len = read(uc->m_recvFd, recv_buffer, PACKAGE_SIZE);

#ifdef DGTIME
		end = clock();
		printf("[_runRecving]recv time:%lf us\n",(double)((double)end - (double)start)/CLOCKS_PER_SEC*1000000);
#endif


#ifdef DEBUG
		printf("[UdpChat _runRecving]recv_len= %d\n",recv_len);
#endif

#ifdef DGTIME
		start = clock();
#endif
        //将收到的数据解压
        decoded_count = uc->m_voice.decode(recv_buffer, recv_len, decoded, MAX_SHORT_SIZE);
        //将数据拷贝到本地的一个缓冲区
        uc->addToLocalBuffer((char *)decoded, decoded_count * 2);

#ifdef DGTIME
		end = clock();
		printf("[_runRecving]decode + addToLocalBuffer time:%lf us\n",(double)((double)end - (double)start)/CLOCKS_PER_SEC*1000000);
#endif
       
//        write_count = fwrite(decoded, sizeof(short), decoded_count, fp);
//        if (write_count != decoded_count) {
//            printf("short write: %d <--- %d\n", write_count, decoded_count);
//        }
    } while (true);
//    fclose(fp);

    pthread_exit(NULL);
}
/*
 * 停止接收操作
 */
void UdpChat::stopRecvVoice() {
    if (this->m_recvID != 0) {
        printf("UdpChat::stopRecvVoice %lu\n", this->m_recvID);
        pthread_cancel(this->m_recvID);
    }
}
/*
 * 发送语音流
 * short data[] 语音流数据
 * int count 数据长度
 */
void UdpChat::sendVoice(short data[], int count) {
//    printf("UdpChat::sendVoice()\n");
    if (count <= 0) {
        return ;
    }
    char encoded[MAX_CHAR_SIZE];
    int byte_count;
    //压缩语音数据

    byte_count = this->m_voice.encode(data, count, encoded, MAX_CHAR_SIZE);

    //发送语音数据
    int write_len = write(this->m_sendFd, encoded, byte_count);
#ifdef DEBUG
    printf("[UdpChat sendVoice]write_len=%d\n", write_len);
#endif
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
//    printf("UdpChat::_runRecording()\n");
    UdpChat *uc = (UdpChat *)obj;
    Recorder rc = Recorder();
    char record_data[MAX_CHAR_SIZE];
    while (true) {

#ifdef DGTIME
		clock_t start,end;
		start = clock();
#endif
        size_t read_count = rc.record(record_data, MAX_CHAR_SIZE);

#ifdef DGTIME
		end = clock();
		printf("[_runRecording]record time:%lf us\n",(double)((double)end - (double)start)/CLOCKS_PER_SEC*1000000);
#endif

#ifdef DGTIME
		start = clock();
#endif

        uc->sendVoice((short *)record_data, read_count / 2);

#ifdef DGTIME
		end = clock();
		printf("[_runRecording]send time:%lf us\n",(double)((double)end - (double)start)/CLOCKS_PER_SEC*1000000);
#endif

    }
    pthread_exit(NULL);
}
/*
 * 停止录音操作
 */
void UdpChat::stopRecordVoice() {
    if (this->m_recordID != 0) {
        printf("UdpChat::stopRecordVoice %lu\n", this->m_recordID);
        pthread_cancel(this->m_recordID);
    }
}
