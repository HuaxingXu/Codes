package worehov.android.voip;

public class Speex {

	/*
	 * quality 1 : 4kbps (very noticeable artifacts, usually intelligible) 
	 * 2 : 6kbps (very noticeable artifacts, good intelligibility) 
	 * 4 : 8kbps (noticeable artifacts sometimes) 
	 * 6 : 11kpbs (artifacts usually only noticeable with headphones) 
	 * 8 : 15kbps (artifacts not usually noticeable)
	 */
	private static final int DEFAULT_COMPRESSION = 4;

	private Speex() {
	}
	
	private static Speex single = null;
	public static Speex getInstance()
	{
		if(single==null)
		{
			single = new Speex();
			single.init();
		}
		return single;
	}

	public static void release()
	{
		single.close();
		single = null;
	}
	
	private void init() {
		try {
			load();
			open(DEFAULT_COMPRESSION);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private void load() {
		try {
			System.loadLibrary("speex");
		} catch (Throwable e) {
			e.printStackTrace();
		}

	}
  
	public int fixToSpeexSize(int size, int frameSize){
		return (size-1)/frameSize * frameSize + frameSize;
	}
	
	public native int open(int compression);

	public native int getFrameSize();
	
	public int getEncodedFrameSize(){return 24;}
	
	public native int decode(byte encoded[], short lin[], int size, int maxBufferSize);

	public native int encode(short lin[], int offset, byte encoded[], int size, int maxBufferSize);
	
	public native boolean preprocess(short lin[], int size);
	
	public native int echoPlayback(short speakerin[], int size);
	
	public native int echoCapture(short lin[], int size);

	private native void close();

}
