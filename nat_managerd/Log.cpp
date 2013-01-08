#include "Log.h"

Log::Log()
{
    //ctor
}

//记录日志到日志队列
void Log::record(string message)
{
    time_t tt = time(NULL);
    struct tm *t = localtime(&tt);

    char file_name[20];
    snprintf(file_name, 20, "log/%4d-%02d-%02d.log", t->tm_year+1900, t->tm_mon+1,
             t->tm_mday);

    string format = "[%4d年%02d月%02d日 %02d:%02d:%02d] " + message + "\n";
    char tmp[MAX_MESSAGE_SIZE];
    snprintf(tmp, MAX_MESSAGE_SIZE, format.c_str(), t->tm_year+1900, t->tm_mon+1,
             t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

    map<string,string> file_message;
    file_message[file_name] = tmp;

    this->message_queues.push(file_message);
}

//将日志写入日志文件
bool Log::save()
{
    //检查日志目录是否存在，不在则创建
    if (access("log", 0))
    {
        if (mkdir("log", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
        {
            return false;
        }
    }

    map<string,string> file_message;

    while(!this->message_queues.empty())
    {
        file_message = this->message_queues.front();
        map<string,string>::iterator it = file_message.begin();
        FILE* fp = fopen(it->first.c_str(), "a");

        if (!fp)
        {
            return false;
        }

        char *message = (char*)it->second.c_str();
        fwrite(message, strlen(message), 1, fp);
        fclose(fp);

        this->message_queues.pop();
    }

    return true;
}

bool Log::hasMessage()
{
    return (!this->message_queues.empty());
}
