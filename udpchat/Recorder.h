/* 
 * File:   Recorder.h
 * Author: yzf
 *
 * Created on January 7, 2013, 4:57 PM
 */

#ifndef RECORDER_H
#define	RECORDER_H

#include <alsa/asoundlib.h>

#define CHANNELS 2  //双声道
#define TIME 60000 //每次发送的录音长度

class Recorder {
public:
    Recorder();
    ~Recorder();
    int record(char *output, int max_size);//录音
private:
    snd_pcm_t *m_pHandle;
    snd_pcm_hw_params_t *m_pParams;
    snd_pcm_uframes_t m_frames;
};

#endif	/* RECORDER_H */

