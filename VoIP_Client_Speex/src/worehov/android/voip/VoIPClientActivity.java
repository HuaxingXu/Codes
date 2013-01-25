package worehov.android.voip;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

import android.app.Activity;
import android.content.SharedPreferences;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.WindowManager;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.ToggleButton;

public class VoIPClientActivity extends Activity {
	Socket speakSocket = null;
	ToggleButton speakBt;
	BaseAudioRecorder m_Recorder;
	EditText m_TargetAddr;

	SharedPreferences sp;

	WelThread m_WelThread = new WelThread();

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.getWindow().setFlags(
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setContentView(R.layout.main);

		sp = this.getPreferences(MODE_PRIVATE);

		String text = sp.getString("addr", "");

		speakBt = (ToggleButton) findViewById(R.id.toggleButton1);
		m_TargetAddr = (EditText) findViewById(R.id.editText1);
		m_TargetAddr.setText(text);
		m_Recorder = new AudioRecorder();
		initEvent(); 
	}

	int config = 0;
	void initEvent() {
		
		final RadioGroup rg = (RadioGroup) findViewById(R.id.radioGroup1); 
		
		final SeekBar seekbar = (SeekBar) findViewById(R.id.seekBar1);
		seekbar.setProgress(100);
		seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
			
			public void onStopTrackingTouch(SeekBar arg0) {
				// TODO Auto-generated method stub
				
			}
			
			public void onStartTrackingTouch(SeekBar arg0) {
				// TODO Auto-generated method stub
				
			}
			
			public void onProgressChanged(SeekBar sb, int position, boolean arg2) {
				// TODO Auto-generated method stub
				float volumn = position*1.0f/sb.getMax();
				m_Recorder.setMicVolumn(volumn);
			}
		});

		
		final CheckBox cbEnableMic = (CheckBox) findViewById(R.id.checkBox1);
		cbEnableMic.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener(){

			public void onCheckedChanged(CompoundButton arg0, boolean arg1) {
				// TODO Auto-generated method stub
				if(m_Recorder!=null)
					m_Recorder.enableMicVolumn(arg1);
			}});

		final CheckBox cbEnableSpeex = (CheckBox) findViewById(R.id.checkBox2);
		cbEnableSpeex.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener(){

			public void onCheckedChanged(CompoundButton arg0, boolean arg1) {
				// TODO Auto-generated method stub
				if(m_Recorder!=null)
					m_Recorder.enableSpeex(arg1);
			}
			
		});
	
		final CheckBox cbSpeexCompress = (CheckBox) findViewById(R.id.checkBox3);
		final CheckBox cbSpeexPreprocess = (CheckBox) findViewById(R.id.checkBox4);
		final CheckBox cbSpeexEcho = (CheckBox) findViewById(R.id.checkBox5);
		
		CheckBox.OnCheckedChangeListener configListener = new CheckBox.OnCheckedChangeListener(){

			public void onCheckedChanged(CompoundButton buttonView,
					boolean isChecked) {
				config = 0;
				if(cbSpeexCompress.isChecked()) config|=BaseAudioRecorder.SPEEX_COMPRESS;
				if(cbSpeexPreprocess.isChecked()) config|=BaseAudioRecorder.SPEEX_PREPROCESS;
				if(cbSpeexEcho.isChecked()) config|=BaseAudioRecorder.SPEEX_ECHO; 
				
				if(m_Recorder!=null)
					m_Recorder.speexConfig(config);
			}
		};
		
		cbSpeexCompress.setOnCheckedChangeListener(configListener);
		cbSpeexPreprocess.setOnCheckedChangeListener(configListener);
		cbSpeexEcho.setOnCheckedChangeListener(configListener);

		speakBt.setOnCheckedChangeListener(new ToggleButton.OnCheckedChangeListener() {

			public void onCheckedChanged(CompoundButton bt, boolean isChecked) {
				// TODO Auto-generated method stub
				if (isChecked) {
					try {
						
						if (speakSocket == null) {
							switch (rg.getCheckedRadioButtonId())
							{
							case R.id.radio0:
								speakSocket = new Socket(m_TargetAddr.getText()
								.toString(), 60000); 
								m_Recorder = new AudioRecorder();
								break;
							case R.id.radio1:
								m_Recorder = new AudioRecorder2();
								break;
							}
							SharedPreferences.Editor ed = sp.edit();
							ed.putString("addr", m_TargetAddr.getText()
									.toString());
							ed.commit();
						} 
						float volumn = seekbar.getProgress()*1.0f/seekbar.getMax();
						m_Recorder.setHandler(m_Handler);
						m_Recorder.setMicVolumn(volumn);
						m_Recorder.enableSpeex(cbEnableSpeex.isChecked());
						m_Recorder.speexConfig(config);
						m_Recorder.enableMicVolumn(cbEnableMic.isChecked());
						m_Recorder.startRecord(speakSocket);
					} catch (Exception e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				} else {
					m_Recorder.stopRecord();
					try {
						if (speakSocket != null && !speakSocket.isClosed())
							speakSocket.close();
						speakSocket = null;
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}

		});
		
	
	}

	@Override
	protected void onResume() { 
		AudioManager audioManager = (AudioManager) this.getSystemService(AUDIO_SERVICE);
		audioManager.setMode(AudioManager.MODE_IN_CALL); //设定为通话中
		
		m_WelThread.start();
		super.onResume();
	}

	@Override
	protected void onPause() {
		AudioManager audioManager = (AudioManager) this.getSystemService(AUDIO_SERVICE);
		audioManager.setMode(AudioManager.MODE_NORMAL); //设定为扬声器
		
		if (speakBt.isChecked())
			speakBt.setChecked(false);

		m_WelThread.safeStop();
		super.onPause();
	}

	protected Handler m_Handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case 1:
				if (!speakBt.isChecked())
					speakBt.setChecked(false);
				speakSocket = (Socket) msg.obj;
				speakBt.setChecked(true);
				break;
			case 2:
				speakBt.setChecked(false);
			}
		}
	};

	class WelThread extends Thread {
		ServerSocket ss;
		boolean isLooped = true;

		@Override
		public void run() {
			try {
				ss = new ServerSocket(60000);
				isLooped = true;

				while (isLooped) {
					Socket temp_speakSocket = ss.accept();
					Message msg = new Message();
					msg.what = 1;
					msg.obj = temp_speakSocket;
					m_Handler.sendMessage(msg);
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

		public void safeStop() {
			if (ss != null && !ss.isClosed())
				try {
					ss.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			isLooped = false;
		}
		
	}
	
}