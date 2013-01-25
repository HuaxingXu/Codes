package worehov.android.voip;
 
public class MicController { 
	/**
	 * 设置音量
	 * @param data PCM音频数据源
	 * @param volumn 原音量的百分比
	 */
	static void setVolumn(short[] data, float volumn) {
		for (int i = 0; i < data.length; i ++) {
			data[i]*=volumn;
		}
	}
}
