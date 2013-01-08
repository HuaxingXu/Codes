#include<sys/stat.h>
#include<sys/wait.h>
#include <signal.h>
#include <iostream>
#include "Manager.h"

using namespace std;

volatile sig_atomic_t _running = 1;//进程运行状态

//守护进程退出处理函数
void sigterm_handler(int arg)
{
    Manager::log.record("服务器进程退出");

    if (Manager::log.hasMessage())
    {
        Manager::log.save();
    }

    Manager::end();

    _running = 0;
}

int main()
{
    pid_t pc;

    pc = fork(); //创建子进程

    if (pc < 0)
    {
        printf("error fork\n");
        exit(EXIT_FAILURE);
    }

    else if (pc > 0)
    {
        exit(EXIT_SUCCESS);
    }

    setsid(); //创建新会话

    signal(SIGTERM , sigterm_handler);//注册信号处理函数

    umask(0); //修改文件权限掩码

    Manager::start();

    Manager::log.record("服务器进程启动");

    while (_running)
    {
        if (Manager::log.hasMessage())
        {
            if (!Manager::log.save())
            {
                Manager::log.record("写日志异常");
            }
        }

        sleep(5);
    }

    return EXIT_SUCCESS;
}
