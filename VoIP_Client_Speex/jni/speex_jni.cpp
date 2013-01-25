#include <jni.h>

#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <speex/speex.h>
#include <speex/speex_preprocess.h>
#include <speex/speex_echo.h>
#include "speex_jni.h"

static int codec_open = 0;

static int dec_frame_size;
static int enc_frame_size;

static SpeexBits ebits, dbits;
void *enc_state;
void *dec_state;
SpeexPreprocessState *preprocess_state;
SpeexEchoState *echo_state;

static JavaVM *gJavaVM;

/*
 * 初始化Speex需要的变量。
 * 包括设置压缩质量、设置预处理效果等
 */
JNIEXPORT jint JNICALL Java_worehov_android_voip_Speex_open(JNIEnv *env,
		jobject obj, jint compression) {

	if (codec_open++ != 0)
		return (jint) 0;

	speex_bits_init(&ebits);
	speex_bits_init(&dbits);

	enc_state = speex_encoder_init(&speex_nb_mode);
	dec_state = speex_decoder_init(&speex_nb_mode);

	int tmp = compression;
	int enh = 1;
	speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY, &tmp);
	speex_encoder_ctl(enc_state, SPEEX_GET_FRAME_SIZE, &enc_frame_size);
	speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &dec_frame_size);
	speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);

	preprocess_state = speex_preprocess_state_init(enc_frame_size, 8000);
	echo_state = speex_echo_state_init(enc_frame_size, 800); // the samples in 100ms

	int denoise = 1;
	int noiseSuppress = -25;
	speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_DENOISE,
			&denoise); //降噪
	speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS,
			&noiseSuppress); //设置噪声的dB

	int agc = 1;
	int q = 20000;
	//actually default is 8000(0,32768),here make it louder for voice is not loudy enough by default. 8000
	speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_AGC, &agc); //增益
	speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_AGC_LEVEL, &q);

	speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_ECHO_STATE,
			echo_state);

	/*int vad = 1;
	 int vadProbStart = 80;
	 int vadProbContinue = 65;
	 speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_VAD, &vad);//静音检测
	 speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_PROB_START , &vadProbStart);//Set probability required for the VAD to go from silence to voice
	 speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_PROB_CONTINUE, &vadProbContinue);//Set probability required for the VAD to stay in the voice state (integer percent)
	 */
	return (jint) 0;
}


/*
 * 压缩编码
 * lin 是PCM数据
 * size 是PCM数据长度
 * encoded是编码后保存的数组
 * max_buffer_size 是保存编码数据的数组的最大长度
 */
JNIEXPORT jint JNICALL Java_worehov_android_voip_Speex_encode(JNIEnv *env,
		jobject obj, jshortArray lin, jint offset, jbyteArray encoded,
		jint size, jint max_buffer_size) {

	jshort buffer[enc_frame_size];
	jbyte output_buffer[1024 + 4];
	int nsamples = (size - 1) / enc_frame_size + 1;
	int i, tot_bytes = 0;

	if (!codec_open)
		return 0;

	for (i = 0; i < nsamples; i++) {
		speex_bits_reset(&ebits);
		env->GetShortArrayRegion(lin, offset + i * enc_frame_size,
				enc_frame_size, buffer);
		speex_encode_int(enc_state, buffer, &ebits);

		int nbBytes = speex_bits_write(&ebits, ((char *) output_buffer) + 4,
				1024 - tot_bytes);
		memcpy(output_buffer, &nbBytes, 4);
		int len =
				max_buffer_size >=  tot_bytes + nbBytes + 4?
						nbBytes + 4 : max_buffer_size - tot_bytes;
		env->SetByteArrayRegion(encoded, tot_bytes, len, output_buffer);
		tot_bytes += nbBytes + 4;
	}

	return (jint) tot_bytes;
}

/*
 * 解码
 * encoded是编码后的数组
 * size 是encoded数据的长度
 * lin 是解码得到的PCM数据
 * max_buffer_size 是保存PCM数据的数组的最大长度
 */
JNIEXPORT jint JNICALL Java_worehov_android_voip_Speex_decode(JNIEnv *env,
		jobject obj, jbyteArray encoded, jshortArray lin, jint size,
		jint max_buffer_size) {

	jbyte buffer[2048];
	jshort output_buffer[2048];
	jsize encoded_length = size;
	memset(output_buffer, 0, sizeof(output_buffer));

	if (!codec_open)
		return 0;

	env->GetByteArrayRegion(encoded, 0, encoded_length, buffer);

	int i, decoded_length = 0;
	for (i = 0; decoded_length < encoded_length; i++) {
		speex_bits_reset(&dbits);

		int nbBytes = *(int*) (buffer + decoded_length);

		speex_bits_read_from(&dbits, (char *) buffer + decoded_length + 4,
				nbBytes);

		speex_decode_int(dec_state, &dbits, output_buffer);

		decoded_length += nbBytes + 4;
		speex_decode_int(dec_state, &dbits, output_buffer);

		int len =
				(max_buffer_size >= -dec_frame_size * (i + 1)) ?
						dec_frame_size : max_buffer_size - dec_frame_size * i;
		env->SetShortArrayRegion(lin, dec_frame_size * i, len,
				output_buffer);
	}

	return (jint) dec_frame_size * i;
}

JNIEXPORT jint JNICALL Java_worehov_android_voip_Speex_getFrameSize(
		JNIEnv *env, jobject obj) {

	if (!codec_open)
		return 0;
	return (jint) enc_frame_size;

}

/*
 * 释放Speex资源
 */
JNIEXPORT void JNICALL Java_worehov_android_voip_Speex_close
(JNIEnv *env, jobject obj) {

	if (--codec_open != 0)
	return;

	speex_bits_destroy(&ebits);
	speex_bits_destroy(&dbits);
	speex_decoder_destroy(dec_state);
	speex_encoder_destroy(enc_state);
	speex_preprocess_state_destroy(preprocess_state);
	speex_echo_state_destroy(echo_state);
}

/*
 * 音质预处理
 * lin PCM数据，预处理后的数据也会保存回lin里面
 * size PCM数据的大小
 */
JNIEXPORT jboolean JNICALL Java_worehov_android_voip_Speex_preprocess(
		JNIEnv * env, jobject obj, jshortArray lin, jint size) {
	jshort audio_frame[512];
	bool ret;
	int i, nsamples = (size - 1) / enc_frame_size;
	for (i = 0; i < nsamples; i++) {
		env->GetShortArrayRegion(lin, i * enc_frame_size, enc_frame_size,
				audio_frame);

		ret = speex_preprocess_run(preprocess_state, audio_frame);

		env->SetShortArrayRegion(lin, enc_frame_size * i, enc_frame_size,
				audio_frame);
	}
	return ret;
}

/*
 * 回音消除
 * lin 麦克风PCM数据，同时回音消除后的数据也会保存到lin里面
 * size PCM数据的长度
 */
JNIEXPORT jint JNICALL Java_worehov_android_voip_Speex_echoPlayback(
		JNIEnv * env, jobject obj, jshortArray lin, jint size) {
	jshort echo_frame[512];
	int i, nsamples = (size - 1) / enc_frame_size;
	for (i = 0; i < nsamples; i++) {
		env->GetShortArrayRegion(lin, i * enc_frame_size, enc_frame_size,
				echo_frame);
		speex_echo_playback(echo_state, echo_frame);
	}

	return nsamples * enc_frame_size;
}

/*
 * 回音消除 扬声器部分
 * 回音消除需要获得扬声器的PCM数据作为参考量。
 * lin 扬声器播出的PCM数据
 * size PCM数据的长度
 */
JNIEXPORT jint JNICALL Java_worehov_android_voip_Speex_echoCapture(
		JNIEnv * env, jobject, jshortArray lin, jint size) {
	jshort input_frame[512];
	jshort output_frame[512];
	int i, nsamples = (size - 1) / enc_frame_size;
	for (i = 0; i < nsamples; i++) {
		env->GetShortArrayRegion(lin, i * enc_frame_size, enc_frame_size,
				input_frame);
		speex_echo_capture(echo_state, input_frame, output_frame);
		env->SetShortArrayRegion(lin, dec_frame_size * i, dec_frame_size,
				output_frame);
	}

	return nsamples * enc_frame_size;
}
