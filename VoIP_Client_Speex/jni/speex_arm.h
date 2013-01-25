#ifndef SPEEX_ARM_H
#define SPEEX_ARM_H


/*
 * 初始化Speex需要的变量。
 * 包括设置压缩质量、设置预处理效果等
 */
int speex_open(int compression);

/*
 * 压缩编码
 * lin 是PCM数据
 * size 是PCM数据长度
 * encoded是编码后保存的数组
 * max_buffer_size 是保存编码数据的数组的最大长度
 */
int speex_encode(short lin[], int offset, char encoded[], int size,
		int max_buffer_size);


/*
 * 解码
 * encoded是编码后的数组
 * size 是encoded数据的长度, 请不要超过2048
 * lin 是解码得到的PCM数据
 * max_buffer_size 是保存PCM数据的数组的最大长度
 */
int speex_decode(char encoded[], short lin[], int size, int max_buffer_size);

/*
 * 获取每次编码/解码的帧大小
 */
int speex_getFrameSize();

/*
 * 释放Speex资源
 */
void speex_close() ;

/*
 * 音质预处理
 * lin PCM数据，预处理后的数据也会保存回lin里面
 * size PCM数据的大小
 */
int speex_preprocess(short lin[], int size);

/*
 * 回音消除 扬声器部分
 * 回音消除需要获得扬声器的PCM数据作为参考量。
 * lin 扬声器播出的PCM数据
 * size PCM数据的长度
 */
int speex_echoPlayback(short lin[], int size) ;

/*
 * 回音消除
 * lin 麦克风PCM数据，同时回音消除后的数据也会保存到lin里面
 * size PCM数据的长度
 */
int speex_echoCapture(short lin[], int size);

#endif //define SPEEX_ARM_H
