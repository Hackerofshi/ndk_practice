package com.tech.pratice_ffmpeg

import android.Manifest
import android.content.Intent
import android.os.Bundle
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import permissions.dispatcher.NeedsPermission
import permissions.dispatcher.RuntimePermissions

@RuntimePermissions
class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        needStorageWithPermissionCheck()

        findViewById<TextView>(R.id.tv).setOnClickListener {
            startActivity(Intent(this, PlayerActivity::class.java))
        }

        findViewById<TextView>(R.id.btn).setOnClickListener {
            startActivity(Intent(this, EglSurfaceVuewActivity::class.java))
        }

        findViewById<TextView>(R.id.btn1).setOnClickListener {
            startActivity(Intent(this, FFmpegGLPlayerActivity::class.java))
        }
    }

    @NeedsPermission(
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.READ_EXTERNAL_STORAGE,
        Manifest.permission.RECORD_AUDIO,
        Manifest.permission.MODIFY_AUDIO_SETTINGS,
        Manifest.permission.CAMERA,
    )
    fun needStorage() {
    }
}