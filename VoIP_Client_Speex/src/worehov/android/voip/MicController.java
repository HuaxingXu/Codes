package worehov.android.voip;
 
public class MicController { 
	/**
	 * ��������
	 * @param data PCM��Ƶ����Դ
	 * @param volumn ԭ�����İٷֱ�
	 */
	static void setVolumn(short[] data, float volumn) {
		for (int i = 0; i < data.length; i ++) {
			data[i]*=volumn;
		}
	}
}
