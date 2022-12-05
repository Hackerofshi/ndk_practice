package com.shixin.ndk_practice.camera.widget;

import android.content.Context;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.AttributeSet;
import android.view.MotionEvent;

import com.shixin.ndk_practice.camera.CameraHelper;
import com.shixin.ndk_practice.camera.CameraRender;
import com.shixin.ndk_practice.opengl.BaseGLSurfaceView;

public class CameraView extends BaseGLSurfaceView implements CameraRender.RenderListener {
    private final CameraHelper mCameraHelper;
    private final CameraRender mCameraRender;
    private final int mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
    private boolean mIsFocusing;

    public CameraView(Context context) {
        this(context, null);
    }

    public CameraView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(2);
        mCameraHelper = new CameraHelper(context);
        mCameraRender = new CameraRender(context);
        setRenderer(mCameraRender);
        mCameraRender.setOnRenderListener(this);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_UP) {
            // 聚焦
            focus((int) event.getX(), (int) event.getY());
        }
        return true;
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        mCameraRender.setViewHeight(getMeasuredHeight());
        mCameraRender.setViewWidth(getMeasuredWidth());
        mCameraHelper.setViewHeight(getMeasuredHeight());
        mCameraHelper.setViewWidth(getMeasuredWidth());
    }

    /**
     * 聚焦
     *
     * @param x
     * @param y
     */
    private void focus(int x, int y) {
        if (mIsFocusing) {
            return;
        }

        mIsFocusing = true;
        Point focusPoint = new Point(x, y);
        if (mFocusListener != null) {
            mFocusListener.beginFocus(x, y);
        }

        mCameraHelper.newCameraFocus(focusPoint, (success, camera) -> {
            mIsFocusing = false;
            if (mFocusListener != null) {
                mFocusListener.endFocus(success);
            }
        });
    }

    @Override
    public void onSurfaceCreated(SurfaceTexture surfaceTexture) {
        mCameraHelper.init(surfaceTexture);
        openId(mCameraId);
    }

    private void openId(int cameraId) {
        rotateCameraAngle();
        mCameraHelper.open(cameraId);
    }

    private void rotateCameraAngle() {
        mCameraRender.resetMatrix();
        //前置摄像头
        if (mCameraId == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            mCameraRender.rotateMatrix(0, 90, 0, 0, 1);
            mCameraRender.rotateMatrix(0, 180, 1, 0, 0);
        }
        // 后置摄像头
        else if (mCameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
            mCameraRender.rotateMatrix(0, 90, 0, 0, 1);
        }
    }

    public void onDestroy() {
        mCameraHelper.close();
    }


    private FocusListener mFocusListener;

    public void setOnFocusListener(FocusListener focusListener) {
        this.mFocusListener = focusListener;
    }

    public int getTextureId() {
        return mCameraRender.getTextureId();
    }

    public interface FocusListener {
        void beginFocus(int x, int y);

        void endFocus(boolean success);
    }
}
