#include <cstdio>
#include <cstdlib>

#include "Recorder.h"

Recorder::Recorder() {
    printf("Recorder::Recorder()\n");
    int rc;
    unsigned int val;
    int dir;
    /* Open PCM device for recording (capture). */
    rc = snd_pcm_open(&this->m_pHandle, "default",
                        SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        printf("unable to open pcm device: %s\n",
                snd_strerror(rc));
        exit(1);
    }
    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&this->m_pParams);
    /* Fill it in with default values. */
    snd_pcm_hw_params_any(this->m_pHandle, this->m_pParams);

    /* Set the desired hardware parameters. */

    /* Interleaved mode */
    snd_pcm_hw_params_set_access(this->m_pHandle, this->m_pParams,
                          SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(this->m_pHandle, this->m_pParams,
                                  SND_PCM_FORMAT_S16_LE);

    /* Two channels (stereo) */
    snd_pcm_hw_params_set_channels(this->m_pHandle, this->m_pParams, 
                                    CHANNELS);

    /* 16000 bits/second sampling rate  */
    val = 16000;
    snd_pcm_hw_params_set_rate_near(this->m_pHandle, this->m_pParams,
                                      &val, &dir);
    /* Set period size to 32 frames. */
    this->m_frames = 32;
    snd_pcm_hw_params_set_period_size_near(this->m_pHandle,
                                  this->m_pParams, &this->m_frames, &dir);
    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(this->m_pHandle, this->m_pParams);
    if (rc < 0) {
        printf("unable to set hw parameters: %s\n",
                snd_strerror(rc));
        exit(1);
    }
}

Recorder::~Recorder() {
    printf("Recorder::~Recorder()\n");
    snd_pcm_drain(this->m_pHandle);
    snd_pcm_close(this->m_pHandle);
}
/*
 * 录音操作，每次调用只录取 TIME 时间的数据
 * 并保存到 output
 * char *output 用户存放录音数据
 * int max_size output能存放的最多数据数
 * return int output实际存放的数据数
 */
int Recorder::record(char *output, int max_size) {
    printf("Recorder::record()\n");
    int loops;
    int rc;
    int size;
    unsigned int val = 2000;
    int dir;
    char *buffer;
    int offset;
//    FILE *fp;
    /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(this->m_pParams,
                                          &this->m_frames, &dir);
    size = this->m_frames * 4; /* 2 bytes/sample, 2 channels */
    buffer = (char *) malloc(size);
  
    snd_pcm_hw_params_get_period_time(this->m_pParams,
                                             &val, &dir);
    loops = TIME / val;
    offset = 0;
//    fp = fopen("data2.raw", "a");
    while (loops > 0) {
        loops--;
        rc = snd_pcm_readi(this->m_pHandle, buffer, this->m_frames);
        if (rc == -EPIPE) {
            /* EPIPE means overrun */
            printf("overrun occurred\n");
            snd_pcm_prepare(this->m_pHandle);
        } 
        else if (rc < 0) {
            printf("error from read: %s\n",
                  snd_strerror(rc));
        } 
        else if (rc != (int)(this->m_frames)) {
            printf("short read, read %d frames\n", rc);
        }
        if (offset + size > max_size) {
            break;
        }
//        rc = fwrite(buffer, sizeof(char), size, fp);
        memcpy(output + offset, buffer, size);
        offset += size;
    }
    free(buffer);
//    fclose(fp);
    return offset;
}