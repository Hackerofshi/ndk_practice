package com.shixin.pratice_opencv

import android.content.pm.ActivityInfo
import android.os.Bundle
import android.util.Log
import org.opencv.android.*
import org.opencv.android.CameraBridgeViewBase.CAMERA_ID_BACK
import org.opencv.android.CameraBridgeViewBase.CAMERA_ID_FRONT
import org.opencv.core.Core
import org.opencv.core.Mat


class CameraPreviewActivity : CameraActivity(), CameraBridgeViewBase.CvCameraViewListener2 {
    init {
        System.loadLibrary("native-lib")
        System.loadLibrary("opencv_java4")
    }

    private val mLoaderCallback: BaseLoaderCallback = object : BaseLoaderCallback(this) {
        override fun onManagerConnected(status: Int) {
            when (status) {
                SUCCESS -> {
                    Log.i("tag", "------初始化")
                    cameraView?.enableView()
                }
                else -> {
                    super.onManagerConnected(status)
                }
            }
        }
    }

    var cameraView: JavaCameraView? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_camera_preview)
        // Example of a call to a native method
        cameraView = findViewById<JavaCameraView>(R.id.sample_text)
        cameraView?.setVisibility(CameraBridgeViewBase.VISIBLE);
        cameraView?.setCvCameraViewListener(this)
        cameraView?.cameraIndex = CAMERA_ID_BACK

    }

    override fun onResume() {
        super.onResume()
        cameraView?.enableView()
        if (!OpenCVLoader.initDebug()) {
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION, this, mLoaderCallback);
        } else {
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
    }

    override fun onPause() {
        super.onPause()
        cameraView?.disableView()
    }


    override fun getCameraViewList(): List<CameraBridgeViewBase?> {
        return listOf<CameraBridgeViewBase>(cameraView!!)
    }


    override fun onCameraViewStarted(width: Int, height: Int) {
        mGrayMat = Mat()
        mRgbaMat = Mat()
    }

    override fun onCameraViewStopped() {
        mGrayMat?.release();
        mRgbaMat?.release();
    }

    private var mRgbaMat: Mat? = null
    private var mGrayMat: Mat? = null
    override fun onCameraFrame(inputFrame: CameraBridgeViewBase.CvCameraViewFrame?): Mat {
        mRgbaMat = inputFrame?.rgba()
        mGrayMat = inputFrame?.gray()
        val cameraIndex = cameraView?.getCameraIndex()
        Log.i("tag", "------cameraIndex=$cameraIndex")

        // 在这里写业务逻辑
        if (this.getResources().getConfiguration().orientation
            == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
            && cameraView?.getCameraIndex() == CAMERA_ID_FRONT
        ) {
            Core.rotate(mRgbaMat, mRgbaMat, Core.ROTATE_90_COUNTERCLOCKWISE);
        } else if (this.getResources().getConfiguration().orientation
            == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
            && cameraView?.getCameraIndex() == CAMERA_ID_BACK
        ) {
            Core.rotate(mRgbaMat, mRgbaMat, Core.ROTATE_90_CLOCKWISE);
        }
        return mRgbaMat!!
    }
}