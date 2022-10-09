package com.shixin.pratice_opencv

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.view.View
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStream

class MainActivity : AppCompatActivity() {


    private var mFaceImage: ImageView? = null
    private var mFaceImage1: ImageView? = null
    private var mFaceBitmap: Bitmap? = null
    private var mFaceDetection: FaceDetection? = null
    private var mCascadeFile: File? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        mFaceImage = findViewById(R.id.face_image)
        mFaceImage1 = findViewById(R.id.face_image1)
        mFaceBitmap = BitmapFactory.decodeResource(resources, R.drawable.face)
        mFaceImage?.setImageBitmap(mFaceBitmap)

        copyCascadeFile()

        mFaceDetection = FaceDetection()
        mFaceDetection?.loadCascade(mCascadeFile?.absolutePath)
    }

    private fun copyCascadeFile() {
        try {
            // load cascade file from application resources
            val stream: InputStream = resources.openRawResource(R.raw.lbpcascade_frontalface)
            val cascadeDir = getDir("cascade", Context.MODE_PRIVATE)
            mCascadeFile = File(cascadeDir, "lbpcascade_frontalface.xml")
            if (mCascadeFile?.exists() == true) return
            val os = FileOutputStream(mCascadeFile)
            val buffer = ByteArray(4096)
            var bytesRead: Int
            while (stream.read(buffer).also { bytesRead = it } != -1) {
                os.write(buffer, 0, bytesRead)
            }
            stream.close()
            os.close()
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }

    fun faceDetection(view: View?) {
        // 识别人脸，保存人脸特征信息
        mFaceDetection?.faceDetectionSaveInfo(mFaceBitmap)
        mFaceImage1?.setImageBitmap(mFaceBitmap)
    }
}