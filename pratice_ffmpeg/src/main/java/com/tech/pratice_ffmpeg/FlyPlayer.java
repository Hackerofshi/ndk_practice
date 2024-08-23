package com.tech.pratice_ffmpeg;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.File;

public class FlyPlayer extends SurfaceView implements SurfaceHolder.Callback, Runnable {

    public FlyPlayer(Context context, AttributeSet attrs) {
        super(context, attrs);
        //设置callback
        getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        // 开启线程
        new Thread(this).start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void run() {
        File file = new File(getContext().getExternalCacheDir(), "test.yuv");
        String absolutePath = file.getAbsolutePath();
        Player.INSTANCE.testAVIOAndUDP(absolutePath, getHolder().getSurface());
    }


}