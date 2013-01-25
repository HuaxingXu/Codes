#define ALSA_PCM_NEW_HW_PARAMS_API
#include "Player.h"
#include "Recorder.h"

#define DEBUG
#undef DEBUG /*如不需要调试信息，则反注释*/

#define ALSA_OPT
//#undefine ALSA_OPT

static unsigned int rate=16000;
static unsigned int channels =1; /*改为单声道*/
static unsigned int buffer_time = 500000; /*每次采集500 ms语音*/
static unsigned int period_time = 100000; /*每次read的语音长度是100 ms*/

static snd_pcm_uframes_t buffer_size;
static snd_pcm_uframes_t period_size;

Player::Player() {
//    printf("Player::Player()\n");
    int rc=0;
    unsigned int val=0;
    int dir=0;
    /* Open PCM device for playback. */
    rc = snd_pcm_open(&(this->m_pHandle), "default",
                SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        printf("unable to open pcm device: %s\n",
                snd_strerror(rc));
        exit(1);
    }

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&(this->m_pParams));

    /* Fill it in with default values. */
    rc = snd_pcm_hw_params_any(this->m_pHandle, this->m_pParams);
    if (rc < 0) {
        printf("Can not configure this PCM device: %s\n",
                snd_strerror(rc));
        exit(1);
    }
    /* Set the desired hardware parameters. */

    /* Interleaved mode */
    rc = snd_pcm_hw_params_set_access(this->m_pHandle, this->m_pParams,
                    SND_PCM_ACCESS_RW_INTERLEAVED);
    if (rc < 0) {
        printf("Failed to set PCM device to interleaved: %s\n",
                snd_strerror(rc));
        exit(1);
    }
    /* Signed 16-bit little-endian format */
    rc = snd_pcm_hw_params_set_format(this->m_pHandle, this->m_pParams,
                    SND_PCM_FORMAT_S16_LE);
    if (rc < 0) {
        printf("Failed to set PCM device to 16-bit signed PCM: %s\n",
                snd_strerror(rc));
        exit(1);
    }
    /* Two channels (mono) */
    rc = snd_pcm_hw_params_set_channels(this->m_pHandle, this->m_pParams,channels);
    if (rc < 0) {
        printf("Failed to set PCM device to mono: %s\n",
                snd_strerror(rc));
        exit(1);
    }
    /* 44100 bits/second sampling rate (CD quality) */
    val = rate;
    rc = snd_pcm_hw_params_set_rate_near(this->m_pHandle, this->m_pParams,
                    &val, 0);
    if (rc < 0) {
        printf("Failed to set PCM device to sample rate =%d: %s\n",
                rate,snd_strerror(rc));
        exit(1);
    }
	if(val != rate){
		printf("Rate doesn't match (request %iHz, get %iHz\n",rate,val);
		exit(1);
	}


	rc = snd_pcm_hw_params_set_buffer_time_near(this->m_pHandle, this->m_pParams, &buffer_time, &dir);
    if (rc < 0) {
        fprintf(stderr, "Failed to set PCM device to buffer time=%d: %s\n",
                    val,snd_strerror(rc));
        exit(1);
    }

	rc = snd_pcm_hw_params_set_period_time_near(this->m_pHandle, this->m_pParams, &period_time, &dir);
	if (rc < 0) {
        fprintf(stderr, "Failed to set PCM device to period time=%d: %s\n",
                    val,snd_strerror(rc));
        exit(1);
    }
	
	//chunk size
	rc = snd_pcm_hw_params_get_period_size(this->m_pParams, &period_size, &dir);
	if(rc < 0){
		fprintf(stderr, "Failed to get period size for capture: %s\n", snd_strerror(rc));
		exit(1);
	}
	this->m_frames = period_size;

	rc = snd_pcm_hw_params_get_buffer_size(this->m_pParams, &buffer_size);
	if(rc < 0){
		fprintf(stderr, "Failed to get buffer size for capture: %s\n", snd_strerror(rc));
		exit(1);
	}

#ifdef DEBUG
	printf("Recorder alsa driver hw params setting\n");
	printf("period size =%d frames\n", (int)period_size);
	printf("buffer size =%d frames\n", (int)buffer_size);
	snd_pcm_hw_params_get_period_time(this->m_pParams,&val, &dir);
	printf("period time =%d us\n",val);
	snd_pcm_hw_params_get_buffer_time(this->m_pParams,&val, &dir);
	printf("buffer time =%d us\n", val);
	snd_pcm_hw_params_get_periods(this->m_pParams, &val, &dir);
	printf("period per buffer =%d frames\n", val);
#endif	

   /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(this->m_pHandle, this->m_pParams);
    if (rc < 0) {
        printf("unable to set hw parameters: %s\n",
                    snd_strerror(rc));
        exit(1);
    }
    
#ifdef ALSA_OPT
	snd_pcm_sw_params_t *swparams;
	snd_pcm_sw_params_alloca(&swparams);

    /* get the current swparams */
    rc = snd_pcm_sw_params_current(this->m_pHandle, swparams);
    if (rc < 0) {
            printf("Unable to determine current swparams for playback: %s\n", snd_strerror(rc));
            exit(1);
    }
    /* start the transfer when the buffer is almost full: */
    /* (buffer_size / avail_min) * avail_min */
    rc = snd_pcm_sw_params_set_start_threshold(this->m_pHandle, swparams, (buffer_size / period_size) * period_size);
    if (rc < 0) {
            printf("Unable to set start threshold mode for playback: %s\n", snd_strerror(rc));
            exit(1);
    }

    rc = snd_pcm_sw_params_set_avail_min(this->m_pHandle, swparams, period_size);
    if (rc < 0) {
            printf("Unable to set avail min for playback: %s\n", snd_strerror(rc));
            exit(1);
    }
    /* write the parameters to the playback device */
    rc = snd_pcm_sw_params(this->m_pHandle, swparams);
    if (rc < 0) {
            printf("Unable to set sw params for playback: %s\n", snd_strerror(rc));
            exit(1);
    }

#endif
}

Player::~Player() {
//    printf("Player::~Player()\n");
    snd_pcm_drain(this->m_pHandle);
    snd_pcm_close(this->m_pHandle);
}
/*
 * 把数据添加到播放缓冲区
 * char *data 音频数据流
 * int len 数据长度
 */

//经过优化的播放函数
void Player::play(char* data, size_t count_bytes) {
    size_t chunk_bytes;
    int rc;
	size_t count_frames, chunk_size;
	size_t bits_per_sample, bits_per_frame, bytes_per_frame;

	bits_per_sample = snd_pcm_format_physical_width(SND_PCM_FORMAT_S16_LE);
	bits_per_frame = bits_per_sample * channels;
	bytes_per_frame = bits_per_frame >>3;
	chunk_size = this->m_frames;
	count_frames = count_bytes / bytes_per_frame; 

	//修改了size的赋值语句
    chunk_bytes = chunk_size * bytes_per_frame; /* 2 bytes/sample,  channels */
#ifdef DEBUG
	printf("bits per sample = %d\n", bits_per_sample);
	printf("bits per frame = %d\n", bits_per_frame);
	printf("chunk size = %d\n", chunk_size);
	printf("chunk bytes = %d\n", chunk_bytes);
	printf("sizeof data = %d\n", strlen(data));
	printf("count = %d\n", count_frames);
#endif

	if(count_frames < chunk_size){
		snd_pcm_format_set_silence(SND_PCM_FORMAT_S16_LE, data + count_frames * bytes_per_frame, (chunk_size - count_frames) * channels);
		count_frames = chunk_size;
	}

	while(count_frames > 0){

#ifdef ALSA_OPT
		usleep(5000);
#endif

        rc = snd_pcm_writei(this->m_pHandle, data, count_frames);
		if(rc == -EAGAIN ||(rc >0 && (size_t)rc < count_frames)){
			rc = snd_pcm_wait(this->m_pHandle, 1000);
		}
		else if (rc == -EPIPE) {
            /* EPIPE means underrun */
            fprintf(stderr, "xrun occurred: %s\n", strerror(rc));
            rc = snd_pcm_prepare(this->m_pHandle);
			if(rc < 0){
				fprintf(stderr, "prepare failed: %s\n", strerror(rc));
				return;
			}
        } 
        else if (rc == -ESTRPIPE) {
			fprintf(stderr, "suspended. Trying resume\n");
			while((rc = snd_pcm_resume(this->m_pHandle)) == -EAGAIN)
			  sleep(1);
			if(rc < 0){
				fprintf(stderr, "resume failed. Restart stream\n");
				if((rc = snd_pcm_prepare(this->m_pHandle)) < 0){
					fprintf(stderr, "suspend: prepare error: %s", strerror(rc));
					return;
				}
			}
		}
		else if (rc < 0){
			fprintf(stderr, "write error: %s\n", strerror(rc));
			return;
		}

		if(rc > 0){
#ifdef DEBUG
			printf("write %d frames\n", rc);
#endif
			count_frames -= rc;
			data += rc * bytes_per_frame;
		}
    }
}

//旧版本的播放函数
#if 0
void Player::play(char* data, int len) {
    int loops;
    char *buffer;
    int size;
    int rc;

	//修改了size的赋值语句
    size = this->m_frames * 2 * channels; /* 2 bytes/sample,  channels */
    buffer = (char *) malloc(size);

    loops = len / size;

    for (int i = 0; i < loops; ++ i) {
        if (i * size > len - 1) {
            printf("end of add\n");
            break;
        }
        memcpy(buffer, data + i * size, size);

        rc = snd_pcm_writei(this->m_pHandle, buffer, this->m_frames);
		if(rc == -EAGAIN ||(rc >0 && (size_t)rc < this->m_frames)){
			rc = snd_pcm_wait(this->m_pHandle, 10000);
			i--;
			continue;
		}
		else if (rc == -EPIPE) {
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
#endif

