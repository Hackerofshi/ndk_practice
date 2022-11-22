package com.shixin.pratice_opencv

import android.Manifest
import android.content.Intent
import android.os.Bundle
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import permissions.dispatcher.NeedsPermission
import permissions.dispatcher.RuntimePermissions

@RuntimePermissions
class HomeActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_home)

        findViewById<Button>(R.id.btn1).setOnClickListener {
            startActivity(Intent(this, BitmapActivity::class.java))
        }
        findViewById<Button>(R.id.btn2).setOnClickListener {
            startActivity(Intent(this, CameraPreviewActivity::class.java))
        }
        findViewById<Button>(R.id.btn3).setOnClickListener {
            startActivity(Intent(this, LocalOcrActivity::class.java))
        }
        initCameraWithPermissionCheck()
    }


    @NeedsPermission(
        Manifest.permission.CAMERA,
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.READ_EXTERNAL_STORAGE,
    )
    fun initCamera() {
    }
}