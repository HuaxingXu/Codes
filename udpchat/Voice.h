/* 
 * File:   Voice.h
 * Author: yzf
 * 语音压缩解压类
 * Created on January 7, 2013, 9:19 AM
 */

#ifndef VOICE_H
#define	VOICE_H

#include <speex/speex.h>

#define FRAME_SIZE 160
#define MAX_SHORT_SIZE 1920
#define MAX_CHAR_SIZE 3840

class Voice {
public:
    Voice();
    ~Voice();
    int encode(short in[], int size,
                char encoded[], int max_buffer_size);//压缩语音流
    int decode(char encoded[], int size,
                short output[], int max_buffer_size);//解压语音流
private:
    int m_encFrameSize;//压缩时的帧大小
    int m_decFrameSize;//解压时的帧大小
    void *m_pEncState;
    SpeexBits m_ebits;
    void *m_pDecState;
    SpeexBits m_dbits;
};



#endif	/* VOICE_H */

