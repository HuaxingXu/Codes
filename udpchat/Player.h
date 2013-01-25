/* 
 * File:   Player.h
 * Author: yzf
 *
 * Created on January 8, 2013, 9:51 AM
 */

#ifndef PLAYER_H
#define	PLAYER_H

#include <alsa/asoundlib.h>

class Player {
public:
    Player();
    ~Player();
    void play(char *data, size_t count);//播放
private:
    snd_pcm_t *m_pHandle;
    snd_pcm_hw_params_t *m_pParams;
    snd_pcm_uframes_t m_frames;
};

#endif	/* PLAYER_H */

