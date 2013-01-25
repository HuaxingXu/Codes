package worehov.android.voip;

import android.media.AudioFormat;


public class SpeechDetector {
	static final int DBFrameCount = 3; //使用最近3帧作为音量参考帧

	double [] dbs = new double[DBFrameCount];
	int dbsindex = 0;
	double silentDb = 0; 
	int chanelType;
	
	public SpeechDetector(int type)
	{
		chanelType = type;
	}
	
	public void reset()
	{
		dbsindex = 0;
		silentDb = 0;
		dbs = new double[DBFrameCount];
		
	}
	
	public static double toDb(byte[] data, int type)
	{
		long tv = 0;
		double v = 0;
		switch(type){
		case AudioFormat.CHANNEL_CONFIGURATION_MONO:
			for (int i = 0; i < data.length; i+=2) {
				// 这里没有做运算的优化，为了更加清晰的展示代码
				tv = (data[i+1]<<8)|data[i];		
				v += tv*tv;
			}
			break;
		case AudioFormat.CHANNEL_CONFIGURATION_STEREO:
			for(int i= 0; i < data.length; i+=4){
				tv = (data[i+3]<<24)|(data[i+2]<<16)|(data[i+1]<<8)|data[i];		
				v += tv*tv;
			}		
			break;
		}
		
		//计算分贝值
		double val =  10*Math.log10(v/(data.length/2));
		if(val<0)
			return 0;
		return val;
	}
	
	public boolean isTalking(byte[] data)
	{
		dbs[dbsindex++%DBFrameCount]=toDb(data,chanelType);
		
		double totaldb = 0;
		for(double db : dbs)
			totaldb+=db;  
		if(dbsindex<=3*DBFrameCount-1)
		{
			silentDb = totaldb; 
			return false;
		}
		if(totaldb <= silentDb*1.1)
		{  
			silentDb = (totaldb+silentDb)/2;
			return false;
		}
		return true;
	}
}
