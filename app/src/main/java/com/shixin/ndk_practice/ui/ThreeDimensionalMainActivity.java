package com.shixin.ndk_practice.ui;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import com.shixin.ndk_practice.R;
import com.shixin.ndk_practice.opengl.threed.Image3D;
import com.shixin.ndk_practice.opengl.threed.ImageProcessGlSurfaceView;

public class ThreeDimensionalMainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ImageProcessGlSurfaceView surfaceView = new ImageProcessGlSurfaceView(this,new Image3D(this));
        setContentView(surfaceView);
    }

}