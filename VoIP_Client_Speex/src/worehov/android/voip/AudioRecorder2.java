package worehov.android.voip;

import java.net.Socket;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.util.Log;

public class AudioRecorder2 extends BaseAudioRecorder {
	boolean isRecording = false;// 是否录放的标记
	static final int frequency = 8000; // 不同的手机硬件配置不一样，所以需要调下参数
	static final int channelConfiguration = AudioFormat.CHANNEL_CONFIGURATION_MONO;
	static final int audioEncoding = AudioFormat.ENCODING_PCM_16BIT;
	int recBufSize, playBufSize;
	AudioRecord audioRecord;
	AudioTrack audioTrack;

	public AudioRecorder2() { 
	}

	public void startRecord(Socket socket) {
		if (!isRecording) {
			isRecording = true;
			new RecordPlayThread().start();
		}
	}

	public void stopRecord() {
		isRecording = false;
	}

	class RecordPlayThread extends Thread {
		public void run() {
			Speex speex = Speex.getInstance();
			
			recBufSize = AudioRecord.getMinBufferSize(frequency,
					channelConfiguration, audioEncoding);
			recBufSize = speex.fixToSpeexSize(recBufSize, speex.getFrameSize());

			playBufSize = AudioTrack.getMinBufferSize(frequency,
					channelConfiguration, audioEncoding);
			playBufSize = speex.fixToSpeexSize(playBufSize, speex.getEncodedFrameSize());
			// -----------------------------------------

			audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, frequency,
					channelConfiguration, audioEncoding, recBufSize);

			audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, frequency,
					channelConfiguration, audioEncoding, playBufSize,
					AudioTrack.MODE_STREAM); 
			try {
				byte[] buffer = new byte[recBufSize]; //其实不用那么大，Speex压缩效率很理想
				short[] bufferS = new short[recBufSize];
				audioRecord.startRecording();// 开始录制
				audioTrack.play();// 开始播放
				while (isRecording) {
					// 从MIC保存数据到缓冲区
					int bufferReadResult = audioRecord.read(bufferS, 0,
							recBufSize);

					if (enableMic) {
						MicController.setVolumn(bufferS, volumn);
					}
					if (!enableSp)
						audioTrack.write(bufferS, 0, bufferReadResult);
					else {
						int encodeSize = bufferReadResult;
						int decodeSize = bufferReadResult;


						if (speexEcho)
							speex.echoCapture(bufferS, bufferReadResult);
						
						if (speexCompress) {
							encodeSize = speex.encode(bufferS, 0, buffer,
									bufferReadResult, buffer.length);
							

							decodeSize = speex.decode(buffer, bufferS,
									encodeSize, bufferS.length);
						}

						if (speexPreprocess)
							speex.preprocess(bufferS, bufferReadResult);

						audioTrack.write(bufferS, 0, bufferReadResult);

						if (speexEcho)
							speex.echoPlayback(bufferS, bufferReadResult);
					}
				}
				audioTrack.stop();
				audioRecord.stop();
				audioTrack.release();
				audioRecord.release();
			} catch (Throwable t) {
				Log.e("spl", t.getMessage());
				audioTrack.release();
				audioRecord.release();
			}
			Speex.release();
		}
	}
}