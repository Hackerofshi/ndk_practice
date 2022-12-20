package com.tech.pratice_ffmpeg

import android.os.Bundle
import android.os.Environment
import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.appcompat.app.AppCompatActivity
import com.tech.pratice_ffmpeg.Player.play

class NativePlayerActivity : AppCompatActivity() {
    val path = Environment.getExternalStorageDirectory().absolutePath + "/test1.mp4"


    private var player: Int? = null
    private var sfv: SurfaceView? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player)
        sfv = findViewById(R.id.sfv)
        initSfv()
    }

    private fun initSfv() {
        sfv?.holder?.addCallback(object : SurfaceHolder.Callback {
            override fun surfaceChanged(
                holder: SurfaceHolder,
                format: Int,
                width: Int,
                height: Int
            ) {

            }

            override fun surfaceDestroyed(holder: SurfaceHolder) {
            }

            override fun surfaceCreated(holder: SurfaceHolder) {
                if (player == null) {
                    player = Player.createPlayer(path, holder.surface)
                    play(player!!)
                }
            }
        })
    }

    override fun onDestroy() {
        super.onDestroy()
        if (player != null) {
            Player.stop(player!!)
        }
    }

}