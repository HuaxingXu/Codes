#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "UdpChat.h"

int str_to_int(const char *str) {
    int num = 0;
    int len = strlen(str);
    for (int i = 0; i < len; ++ i) {
        num *= 10;
        num += (str[i] - '0');
    }
    return num;
}

int main(int argc, char **argv) {
    
    if (argc != 3 && argc != 4) {
        printf("运行方式： ./udpchat target_ip target_port or "
                "./udpchat target_ip target_port local_port\n");
        exit(-1);
    }
    char *target_ip = argv[1];
    int target_port = str_to_int(argv[2]);
    int local_port = UDP_PORT;
    if (argc == 4) {
        local_port = str_to_int(argv[3]);
    }
    printf("server begin!!\n");
    
    UdpChat uc = UdpChat(target_ip, target_port, local_port);
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
        else if (strncmp(cmd, "3", 1) == 0) {
            char new_ip[50];
            int new_port;
            scanf("%s%d", new_ip, &new_port);
            uc.resetTarget(new_ip, new_port);
        }
        //退出
        else {
            break;
        }
    }
    
    return 0;
}
