package com.shixin.ndk_practice.ui;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.shixin.ndk_practice.R;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class AudioRecoderActivity extends AppCompatActivity {

    private AudioRecord mRecorder;
    private final static int SAMPLE_RATE = 44100;
    private int recordBufSize;
    private volatile boolean isRecording = false;
    //    private static final String FILE_NAME = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MUSIC)
//            + File.separator + "test.pcm";
    private Thread thread;
    private String FILE_NAME;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_recoder);
        createRecoder();
        findViewById(R.id.btn_start).setOnClickListener(v -> {
            startRecord();
        });
        findViewById(R.id.btn_stop).setOnClickListener(v -> {
            stopRecord();
        });
    }

    private void startRecord() {

        if (isRecording) {
            return;
        }
        isRecording = true;
        mRecorder.startRecording();
        thread = new Thread(() -> {
            byte[] data = new byte[recordBufSize];
            File file = new File(FILE_NAME);
            FileOutputStream os = null;
            int read;
            try {
                if (!file.exists()) {
                    file.createNewFile();
                    Log.i("audioRecordTest", "创建录音文件->" + FILE_NAME);
                }
                os = new FileOutputStream(file);
                while (isRecording) {
                    if (!isRecording) {
                        return;
                    }
                    read = mRecorder.read(data, 0, recordBufSize);
                    if (AudioRecord.ERROR_INVALID_OPERATION != read) {
                        os.write(data);
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                try {
                    if (os != null)
                        os.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

        });
        thread.start();
    }

    private void stopRecord() {
        isRecording = false;
        if (mRecorder != null) {
            mRecorder.stop();
            Log.i("audioRecordTest", "停止录音");
            mRecorder.release();
            mRecorder = null;
            thread = null;
        }

        File file = new File(FILE_NAME);
        Log.i("========>", file.length() + "," + file.getAbsolutePath());
    }

    private void createRecoder() {
        //FILE_NAME = this.getFilesDir() + File.separator + "test.pcm";
        FILE_NAME = this.getCacheDir() + File.separator + "test.pcm";
        recordBufSize = AudioRecord.getMinBufferSize(SAMPLE_RATE, AudioFormat.CHANNEL_IN_STEREO,
                AudioFormat.ENCODING_PCM_16BIT
        );
        mRecorder = new AudioRecord(MediaRecorder.AudioSource.MIC,
                SAMPLE_RATE,
                AudioFormat.CHANNEL_IN_STEREO,
                AudioFormat.ENCODING_PCM_16BIT, recordBufSize
        );
    }
}
