
#include "Player.h"
#include "Recorder.h"

Player::Player() {
    printf("Player::Player()\n");
    int rc;
    unsigned int val;
    int dir;
    /* Open PCM device for playback. */
    rc = snd_pcm_open(&this->m_pHandle, "default",
                SND_PCM_STREAM_PLAYBACK, 0);
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
    snd_pcm_hw_params_set_channels(this->m_pHandle, this->m_pParams, CHANNELS);

    /* 44100 bits/second sampling rate (CD quality) */
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

Player::~Player() {
    printf("Player::~Player()\n");
    snd_pcm_drain(this->m_pHandle);
    snd_pcm_close(this->m_pHandle);
}
/*
 * 把数据添加到播放缓冲区
 * char *data 音频数据流
 * int len 数据长度
 */
void Player::play(char* data, int len) {
    int loops;
    char *buffer;
    int dir;
    int size;
    int rc;
    
    /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(this->m_pParams, &this->m_frames,
                &dir);
    size = this->m_frames * 4; /* 2 bytes/sample, 2 channels */
    buffer = (char *) malloc(size);

    loops = len / size;
//    printf("val=%d loops=%d size=%d len=%d\n", val, loops, size, len);
    for (int i = 0; i < loops; ++ i) {
        if (i * size > len) {
            printf("end of add\n");
            break;
        }
        memcpy(buffer, data + i * size, size);
        
        rc = snd_pcm_writei(this->m_pHandle, buffer, this->m_frames);
        if (rc == -EPIPE) {
            /* EPIPE means underrun */
            printf("underrun occurred\n");
            snd_pcm_prepare(this->m_pHandle);
        } 
        else if (rc < 0) {
            printf("error from writei: %s\n",
                snd_strerror(rc));
        } 
        else if (rc != (int)this->m_frames) {
            printf("short write, write %d frames\n", rc);
        }
    }
    delete buffer;
}
