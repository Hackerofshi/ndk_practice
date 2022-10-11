package com.tech.pratice_audio

import android.Manifest
import android.os.Bundle
import android.os.Environment
import android.view.View
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import permissions.dispatcher.NeedsPermission
import permissions.dispatcher.RuntimePermissions
import java.io.File

@RuntimePermissions
class MainActivity : AppCompatActivity() {
    private val PATH = Environment.getExternalStorageDirectory().toString() + "/_test_record.pcm"
    private val PATH_RECORD = Environment.getExternalStorageDirectory().toString() + "/_test_record.pcm"


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        needStorageWithPermissionCheck()

        findViewById<Button>(R.id.btn).setOnClickListener {
            OpenSL_Play_PCM()
        }
        findViewById<Button>(R.id.btn1).setOnClickListener {
            startRecord()
        }
    }

    fun OpenSL_Play_PCM() {
        AudioPlayer.nativePlayPcm(PATH)
    }

    fun OpenSL_Stop_PCM(view: View?) {
        AudioPlayer.nativeStopPcm()
    }

    fun startRecord() {
        val RECORD_PATH = externalCacheDir?.absolutePath + "/_test_record.pcm"

        val fileRecord = File(PATH_RECORD)
        if (!fileRecord.exists()) {
            fileRecord.createNewFile()
        }
        println(fileRecord.absolutePath.toString())
        AudioRecord.startRecord(fileRecord.absolutePath)
    }


    @NeedsPermission(
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.READ_EXTERNAL_STORAGE,
        Manifest.permission.RECORD_AUDIO,
        Manifest.permission.MODIFY_AUDIO_SETTINGS,
    )
    fun needStorage() {
    }
}