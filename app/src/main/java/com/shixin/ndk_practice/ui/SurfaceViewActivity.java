package com.shixin.ndk_practice.ui;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import com.shixin.ndk_practice.R;
import com.shixin.ndk_practice.opengl.MySurfaceView;

import java.util.List;

public class SurfaceViewActivity extends AppCompatActivity {

    private SensorManager mSensorManager;
    private Sensor mRotation;
    private MySurfaceView mGLView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_surface_view);

        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        List<Sensor> sensors = mSensorManager.getSensorList(Sensor.TYPE_ALL);
        //todo 判断是否存在rotation vector sensor
        mRotation = mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);


        mGLView = (MySurfaceView) findViewById(R.id.surface);
        mGLView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

    }

    @Override
    protected void onResume() {
        super.onResume();
        mSensorManager.registerListener(mGLView, mRotation, SensorManager.SENSOR_DELAY_GAME);
        mGLView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mSensorManager.unregisterListener(mGLView);
        mGLView.onPause();
    }
}