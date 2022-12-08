package com.shixin.ndk_practice.ui;

import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.shixin.ndk_practice.R;
import com.shixin.ndk_practice.camera.widget.CameraFocusView;
import com.shixin.ndk_practice.camera.widget.CameraView;
import com.shixin.ndk_practice.record.BaseVideoRecorder;
import com.shixin.ndk_practice.record.DefaultVideoRecorder;
import com.shixin.ndk_practice.record.widget.RecordProgressButton;

/**
 * Created by hcDarren on 2019/7/13.
 */
public class VideoRecordActivity extends AppCompatActivity implements BaseVideoRecorder.RecordListener {
    private CameraView mCameraView;
    private CameraFocusView mFocusView;
    private DefaultVideoRecorder mVideoRecorder;
    private RecordProgressButton mRecordButton;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_recoder);
        mCameraView = findViewById(R.id.camera_view);
        mFocusView = findViewById(R.id.camera_focus_view);
        mRecordButton = findViewById(R.id.record_button);
        mRecordButton.setMaxProgress(30000);
        String path = getExternalCacheDir().getAbsolutePath();

        mCameraView.setOnFocusListener(new CameraView.FocusListener() {
            @Override
            public void beginFocus(int x, int y) {
                mFocusView.beginFocus(x, y);
            }

            @Override
            public void endFocus(boolean success) {
                mFocusView.endFocus(success);
            }
        });

        mRecordButton.setOnRecordListener(new RecordProgressButton.RecordListener() {
            @Override
            public void onStart() {
                mVideoRecorder = new DefaultVideoRecorder(VideoRecordActivity.this,
                        mCameraView.getEglContext(), mCameraView.getTextureId());
                mVideoRecorder.initVideo(
                        path + "/live_push.mp4",
                        720, 1280);
                mVideoRecorder.setOnRecordListener(VideoRecordActivity.this);
                mVideoRecorder.startRecord();
            }

            @Override
            public void onEnd() {
                mVideoRecorder.stopRecord();
            }
        });
    }

    @Override
    public void onTime(long times) {
        mRecordButton.setCurrentProgress(times);
    }
}
