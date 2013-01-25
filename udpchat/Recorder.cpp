#define ALSA_PCM_NEW_HW_PARAMS_API
#include <cstdio>
#include <cstdlib>
#include "Recorder.h"
#define DEBUG
#undef DEBUG /*如不需调试信息，则反注释*/


static unsigned int rate=16000;
static unsigned int channels =1; /*设置为单声道*/
static unsigned int buffer_time = 500000;
static unsigned int period_time = 100000; /*每次readi到的时间是100ms*/

static snd_pcm_uframes_t buffer_size;
static snd_pcm_uframes_t period_size;

Recorder::Recorder() {
    printf("Recorder::Recorder()\n");
    int rc=0;
    unsigned int val=0;
    int dir=0;
    /* Open PCM device for recording (capture). */
    rc = snd_pcm_open(&(this->m_pHandle), "default",
                        SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "unable to open pcm device: %s\n",
                snd_strerror(rc));
        exit(1);
    }
    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&(this->m_pParams));
    /* Fill it in with default values. */
    rc = snd_pcm_hw_params_any(this->m_pHandle, this->m_pParams);
    if (rc < 0) {
        fprintf(stderr, "Can not configure this PCM device: %s\n",
                snd_strerror(rc));
        exit(1);
    }
    /* Set the desired hardware parameters. */

    /* Interleaved mode */
    rc = snd_pcm_hw_params_set_access(this->m_pHandle, this->m_pParams,
                          SND_PCM_ACCESS_RW_INTERLEAVED);
    if (rc < 0) {
        fprintf(stderr, "Failed to set PCM device to interleaved: %s\n",
                snd_strerror(rc));
        exit(1);
    }
    /* Signed 16-bit little-endian format */
    rc = snd_pcm_hw_params_set_format(this->m_pHandle, this->m_pParams,
                                  SND_PCM_FORMAT_S16_LE);
    if (rc < 0) {
        fprintf(stderr, "Failed to set PCM device to 16-bit signed PCM: %s\n",
                snd_strerror(rc));
        exit(1);
    }
    /* One channels (mono) */
    rc = snd_pcm_hw_params_set_channels(this->m_pHandle, this->m_pParams, channels);
    if (rc < 0) {
        fprintf(stderr, "Failed to set PCM device to mono: %s\n",
                snd_strerror(rc));
        exit(1);
    }
    /* 16000 bits/second sampling rate  */
    val = rate;
    rc = snd_pcm_hw_params_set_rate_near(this->m_pHandle, this->m_pParams,&val, &dir);
    if (rc < 0) {
        fprintf(stderr, "Failed to set PCM device to sample rate =%d: %s\n",
                val,snd_strerror(rc));
        exit(1);
    }
	if(val !=rate){
		fprintf(stderr, "Rate doesn't match (request %iHz, get %iHz)\n",rate, val);
		exit(1);
	}

	rc = snd_pcm_hw_params_set_buffer_time_near(this->m_pHandle, this->m_pParams, &buffer_time, &dir);
	if(rc < 0){
		fprintf(stderr, "Failed to set PCM device to buffer time=%d: %s\n", val, snd_strerror(rc));
		exit(1);
	}
	
	rc = snd_pcm_hw_params_set_period_time_near(this->m_pHandle, this->m_pParams, &period_time, &dir);
	if(rc < 0){
		fprintf(stderr, "Failed to set PCM device to period time=%d: %s\n", val, snd_strerror(rc));
		exit(1);
	}

	rc = snd_pcm_hw_params_get_period_size(this->m_pParams,&period_size, &dir);
	if(rc < 0){
		fprintf(stderr, "Failed to get period size for capture: %s\n", snd_strerror(rc));
		exit(1);
	}
	this->m_frames = period_size;

	rc = snd_pcm_hw_params_get_buffer_size(this->m_pParams,&buffer_size);
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
	

    //调节录音音量
    snd_mixer_t *mixer;
    snd_mixer_elem_t *pcm_element;
    snd_mixer_open(&mixer, 0);
    snd_mixer_attach(mixer, "default");
    snd_mixer_selem_register(mixer, NULL, NULL);
    snd_mixer_load(mixer);
    for (pcm_element = snd_mixer_first_elem(mixer); 
            pcm_element; 
            pcm_element = snd_mixer_elem_next(pcm_element)) 
    {
        if (snd_mixer_elem_get_type(pcm_element) == SND_MIXER_ELEM_SIMPLE &&
             snd_mixer_selem_is_active(pcm_element)) 
        {
			if (strncmp(snd_mixer_selem_get_name(pcm_element), "Mic", 3) == 0)
			{
				snd_mixer_selem_set_capture_volume_range(pcm_element, 0, 100);
                snd_mixer_selem_set_capture_volume_all(pcm_element, (long)100);
			}

            if (strncmp(snd_mixer_selem_get_name(pcm_element), "Capture", 7) == 0) 
            {
                snd_mixer_selem_set_capture_volume_range(pcm_element, 0, 100);
                snd_mixer_selem_set_capture_volume_all(pcm_element, (long)100);
            }
        }
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
//    printf("Recorder::record()\n");
    unsigned int loops;
    int rc;
    int size;
    char *buffer;
    int offset;

	/*修改了size的赋值语句*/
    /* Use a buffer large enough to hold one period */
    size = this->m_frames * 2 * channels; /* 2 bytes/sample, 2 channels */
    buffer = (char *) malloc(size);
    
	//这一句有问题，获取到的val 值为0
    //snd_pcm_hw_params_get_period_time(this->m_pParams,&val, &dir);
	//loops = (unsigned int)(2000 / val);
    offset = 0;
	loops = 1;
#ifdef DEBUG
    printf("loops=%d\n",  loops);
#endif
    while (loops > 0) {
        loops--;
        rc = snd_pcm_readi(this->m_pHandle, buffer, this->m_frames);

		if(rc == -EAGAIN || (rc > 0 && (size_t)rc < this->m_frames) ){
			snd_pcm_wait(this->m_pHandle, 1000);
		}
		else if (rc == -EPIPE) {
            /* EPIPE means overrun */
            fprintf(stderr, "overrun occurred\n");
            rc = snd_pcm_prepare(this->m_pHandle);
			if(rc < 0){
				fprintf(stderr, "Can't recover from overrun, prepare failed: %s\n", snd_strerror(rc));
				return 0;
			}

        } 
		else if(rc == -ESTRPIPE){
			while((rc = snd_pcm_resume(this->m_pHandle) == -EAGAIN)){
				sleep(1);
			}
			if(rc < 0){
				rc = snd_pcm_prepare(this->m_pHandle);
				if(rc < 0){
					fprintf(stderr, "Can't recover from suspend, prepare failed: %s\n", snd_strerror(rc));
					return 0;
				}
			}
		}

        if (offset + size > max_size) {
            break;
        }
        memcpy(output + offset, buffer, size);
        offset += size;
    }
    free(buffer);

    return offset;
}
