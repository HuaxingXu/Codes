#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "UdpChat.h"

int main(int argc, char **argv) {
    printf("begin!!\n");
    
    UdpChat uc = UdpChat();
    uc.beginRecvVoiceAsyn();

    char cmd[10];
    while (scanf("%s", cmd) != EOF) {
        //录音
        if (strncmp(cmd, "1", 1) == 0) {
            uc.stopRecordVoice();
            uc.beginRecordVoiceAysn();
        }
        //停止录音
        else if (strncmp(cmd, "2", 1) == 0) {
            uc.stopRecordVoice();
        }
        //退出
        else {
            uc.stopRecordVoice();
            break;
        }
    }
    
    return 0;
}
