package worehov.android.voip;

import java.io.BufferedInputStream;
import java.io.DataOutputStream;
import java.net.Socket;
import java.net.SocketException;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.util.Log;

/**
 * 音频传输模块
 * 
 * @author liling
 * @version 1.0.0
 */
public class AudioRecorder extends BaseAudioRecorder {
	private boolean isRecording = false;// 是否录放的标记
	private static final int frequency = 8000; // 不同的手机硬件配置不一样，所以需要调下参数
	private static final int audioEncoding = AudioFormat.ENCODING_PCM_16BIT;
	private static final int channelConfiguration = AudioFormat.CHANNEL_CONFIGURATION_MONO;
	private int recBufSize;
	private int playBufSize;
	private AudioRecord audioRecord;
	private AudioTrack audioTrack;
	private DataOutputStream out;
	private BufferedInputStream bufferIns;
	private Socket client;

	private Speex speex; 
	public AudioRecorder() {
	}

	/**
	 * 开始录音
	 */
	public void startRecord(Socket s) {
		try {
			speex = Speex.getInstance();
			
			client = s;
			recBufSize = AudioRecord.getMinBufferSize(frequency,
					channelConfiguration, audioEncoding);
			
			
			audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
					frequency, channelConfiguration, audioEncoding, recBufSize);

			playBufSize = AudioTrack.getMinBufferSize(frequency,
					channelConfiguration, audioEncoding);
			
			recBufSize = speex.fixToSpeexSize(recBufSize, speex.getFrameSize());
			playBufSize = speex.fixToSpeexSize(playBufSize, speex.getEncodedFrameSize());
			
			
			audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, frequency,
					channelConfiguration, audioEncoding, playBufSize,
					AudioTrack.MODE_STREAM);
			audioTrack.setStereoVolume(AudioTrack.getMaxVolume(),
					AudioTrack.getMaxVolume());// 设置当前音量大小为最大值
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}
		isRecording = true;
		new RecordPlayThread().start();
		new TrackPlayThread().start();
	}

	/**
	 * 停止录音
	 */
	public void stopRecord() {
		if (!isRecording)
			return;
		isRecording = false;
		try {
			out.flush();
			out.close();
			bufferIns.close();
			client.close();
			Speex.release();
			release();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	protected void release() {
		audioRecord.release(); // 释放资源，可多次进行音频传输
		audioTrack.release();
	}

	class RecordPlayThread extends Thread {
		public void run() {
			try { 
				byte[] buffer = new byte[recBufSize]; //其实不用那么大，Speex压缩效率很理想
				short[] bufferS = new short[recBufSize];
				
				audioRecord.startRecording(); // 开始录制
				out = new DataOutputStream(client.getOutputStream());
				while (isRecording) {
					// 从MIC保存数据到缓冲区
					int bufferReadResult = audioRecord.read(bufferS, 0,
							recBufSize);
					Log.d("tag1", "大小:" + bufferReadResult);

					if (enableMic) {
						MicController.setVolumn(bufferS, volumn);
					}

					if (!enableSp)
						out.write(toByte(bufferS), 0, bufferReadResult*2);
					
					else {
						int encodeSize = bufferReadResult;
						int decodeSize = bufferReadResult;

						if (speexEcho) {
							speex.echoCapture(bufferS, bufferReadResult);
						}
						

						if (speexPreprocess)
							speex.preprocess(bufferS, bufferReadResult);
						
						if (speexCompress) {
							encodeSize = speex.encode(bufferS, 0, buffer,
									bufferReadResult, buffer.length);
							out.write(buffer, 0, encodeSize); 
						} 
						else
							out.write(toByte(bufferS), 0, bufferReadResult*2); 
					}
					
				}
				audioRecord.stop();
			} catch (SocketException e) {
				e.printStackTrace(); 
			} catch (Exception e) {
				e.printStackTrace();
				try {
					audioRecord.stop();
				} catch (Exception e1) {
				}
			}
		}
	};

	class TrackPlayThread extends Thread {
		public void run() {
			try {
				bufferIns = new BufferedInputStream(client.getInputStream());
				byte[] buffer = new byte[playBufSize];
				short[] bufferS;
				audioTrack.play();// 开始播放
				int bufferReadLen = 0;
				while ((bufferReadLen = bufferIns.read(buffer)) >= 0) {  
					int decodeSize = bufferReadLen/2;
					bufferS = toShort(buffer);
					
					if(speexCompress)
					{
						bufferS = new short[bufferReadLen*160/(24)];
						decodeSize = speex.decode(buffer, bufferS,
								bufferReadLen, bufferS.length); //压缩长度等于socket接收到的长度
					}
					
					if (speexEcho)
					{
						speex.echoPlayback(bufferS, decodeSize); 
					}
					
					audioTrack.write(toByte(bufferS), 0, decodeSize*2); 
				}
				audioTrack.stop();
			} 
			catch (SocketException e) {
				e.printStackTrace();
				activityHandler.sendEmptyMessage(2);
				try {
					audioTrack.stop();
				} catch (Exception e1) {
				}
			} 
			catch (Exception e) {
				e.printStackTrace();
				try {
					audioTrack.stop();
				} catch (Exception e1) {
				}
			}
		}
	}

	protected byte[] toByte(short[] inputData) {
		int len = inputData.length * 2;
		byte[] ret = new byte[len];

		for (int i = 0; i < len; i += 2) {
			ret[i] = (byte) (inputData[i / 2] & 0xff);
			ret[i + 1] = (byte) ((inputData[i / 2] >> 8) & 0xff);
		}
		return ret;
	}
	
	protected short[] toShort(byte[] inputData) {
		int len = inputData.length / 2;
		short[] ret = new short[len];

		for (int i = 0; i < len; i++) {
			ret[i] = (short) ((inputData[i * 2 + 1] << 8) & 0xffff | (inputData[i * 2] & 0x00ff));
		}
		return ret;
	}
}
