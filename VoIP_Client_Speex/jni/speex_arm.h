#ifndef SPEEX_ARM_H
#define SPEEX_ARM_H


/*
 * ��ʼ��Speex��Ҫ�ı�����
 * ��������ѹ������������Ԥ����Ч����
 */
int speex_open(int compression);

/*
 * ѹ������
 * lin ��PCM����
 * size ��PCM���ݳ���
 * encoded�Ǳ���󱣴������
 * max_buffer_size �Ǳ���������ݵ��������󳤶�
 */
int speex_encode(short lin[], int offset, char encoded[], int size,
		int max_buffer_size);


/*
 * ����
 * encoded�Ǳ���������
 * size ��encoded���ݵĳ���, �벻Ҫ����2048
 * lin �ǽ���õ���PCM����
 * max_buffer_size �Ǳ���PCM���ݵ��������󳤶�
 */
int speex_decode(char encoded[], short lin[], int size, int max_buffer_size);

/*
 * ��ȡÿ�α���/�����֡��С
 */
int speex_getFrameSize();

/*
 * �ͷ�Speex��Դ
 */
void speex_close() ;

/*
 * ����Ԥ����
 * lin PCM���ݣ�Ԥ����������Ҳ�ᱣ���lin����
 * size PCM���ݵĴ�С
 */
int speex_preprocess(short lin[], int size);

/*
 * �������� ����������
 * ����������Ҫ�����������PCM������Ϊ�ο�����
 * lin ������������PCM����
 * size PCM���ݵĳ���
 */
int speex_echoPlayback(short lin[], int size) ;

/*
 * ��������
 * lin ��˷�PCM���ݣ�ͬʱ���������������Ҳ�ᱣ�浽lin����
 * size PCM���ݵĳ���
 */
int speex_echoCapture(short lin[], int size);

#endif //define SPEEX_ARM_H
