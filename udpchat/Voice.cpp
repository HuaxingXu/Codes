#include <cstring>
#include <cstdio>
#include "Voice.h"


Voice::Voice() {
    printf("Voice()\n");
    int quality = 8;
    speex_bits_init(&this->m_ebits);
    this->m_pEncState = speex_encoder_init(&speex_nb_mode);
    speex_encoder_ctl(this->m_pEncState, SPEEX_SET_QUALITY, &quality);
    speex_encoder_ctl(this->m_pEncState, SPEEX_GET_FRAME_SIZE, &this->m_encFrameSize);
    int enh = 1;
    speex_bits_init(&this->m_dbits);
    this->m_pDecState = speex_decoder_init(&speex_nb_mode);
    speex_decoder_ctl(this->m_pDecState, SPEEX_GET_FRAME_SIZE, &this->m_decFrameSize);
    speex_decoder_ctl(this->m_pDecState, SPEEX_SET_ENH, &enh);
}

Voice::~Voice() {
    printf("~Voice()\n");
    speex_bits_destroy(&this->m_ebits);
    speex_encoder_destroy(this->m_pEncState);
    speex_bits_destroy(&this->m_dbits);
    speex_decoder_destroy(this->m_pDecState);
}
/*
 * 压缩编码
 * short lin[] 语音数据
 * int size 语音数据长度
 * char encoded[] 编码后保存数据的数组
 * int max_buffer_size 保存编码数据数组的最大长度
 */
int Voice::encode(short in[], int size, 
        char encoded[], int max_buffer_size) {
    short buffer[this->m_encFrameSize];
    char output_buffer[MAX_SHORT_SIZE];
    int nsamples = (size - 1) / this->m_encFrameSize + 1;
    int tot_bytes = 0;
    for (int i = 0; i < nsamples; ++ i) {
        speex_bits_reset(&this->m_ebits);
        memcpy(buffer, in + i * this->m_encFrameSize, 
                    this->m_encFrameSize * sizeof(short));

        speex_encode_int(this->m_pEncState, buffer, &this->m_ebits);
        int nbBytes = speex_bits_write(&this->m_ebits, output_buffer + 4,
                                1024 - tot_bytes);
        memcpy(output_buffer, &nbBytes, 4);

        int len = 
                max_buffer_size >= tot_bytes + nbBytes + 4 ? 
                    nbBytes + 4 : max_buffer_size - tot_bytes;

        memcpy(encoded + tot_bytes, output_buffer, len * sizeof(char));
        
        tot_bytes += nbBytes + 4;
    }
    return tot_bytes;
}
/*
 * 解码
 * char encoded[] 编码后的语音数据
 * int size 编码后的语音数据的长度
 * short output[] 解码后的语音数据
 * int max_buffer_size 保存解码后的数据的数组的最大长度
 */
int Voice::decode(char encoded[], int size, 
        short output[], int max_buffer_size) {
    char* buffer = encoded;
    short output_buffer[MAX_SHORT_SIZE];
    int encoded_length = size;
    int decoded_length = 0;
    int i;

    for (i = 0; decoded_length < encoded_length; ++ i) {
        speex_bits_reset(&this->m_dbits);
        int nbBytes = *(int*)(buffer + decoded_length);
        speex_bits_read_from(&this->m_dbits, (char *)buffer + decoded_length + 4, 
                nbBytes);
        speex_decode_int(this->m_pDecState, &this->m_dbits, output_buffer);
        
        decoded_length += nbBytes + 4;
        int len = (max_buffer_size >= this->m_decFrameSize * (i + 1)) ? 
                        this->m_decFrameSize : max_buffer_size - this->m_decFrameSize * i;
        memcpy(output + this->m_decFrameSize * i, output_buffer, len * sizeof(short));
    }
    return this->m_decFrameSize * i;
}
