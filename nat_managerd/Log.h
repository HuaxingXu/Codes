#ifndef LOG_H
#define LOG_H
#include <string>
#include <queue>
#include <map>
#include<time.h>
#include<unistd.h>
#include <memory.h>
#include<stdio.h>
#include<sys/stat.h>

#define MAX_MESSAGE_SIZE 512

using namespace std;

class Log
{
    public:
        Log();
        void record(string message);
        bool save();
        bool hasMessage();
    protected:
    private:
        //队列结构对:日志文件<-->日志消息
        queue< map<string,string> > message_queues;
};

#endif // LOG_H
