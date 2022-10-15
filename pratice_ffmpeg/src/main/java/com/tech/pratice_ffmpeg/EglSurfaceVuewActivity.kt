package com.tech.pratice_ffmpeg

import android.os.Bundle
import android.os.Environment
import android.os.Handler
import android.view.Surface
import android.view.SurfaceView
import androidx.appcompat.app.AppCompatActivity
import com.tech.pratice_ffmpeg.drawer.VideoDrawer
import com.tech.pratice_ffmpeg.egl.CustomerGLRenderer
import com.tech.pratice_ffmpeg.media.BaseDecoder
import com.tech.pratice_ffmpeg.media.DefDecoderStateListener
import com.tech.pratice_ffmpeg.media.Frame
import com.tech.pratice_ffmpeg.media.decoder.VideoDecoder
import java.util.concurrent.Executors

class EglSurfaceVuewActivity : AppCompatActivity() {
    private val path = Environment.getExternalStorageDirectory().absolutePath + "/test1.mp4"
    private val path2 = Environment.getExternalStorageDirectory().absolutePath + "/mvtest.mp4"

    private val threadPool = Executors.newFixedThreadPool(10)

    private var mRenderer = CustomerGLRenderer()

    private lateinit var sfv: SurfaceView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_egl_surface_vuew)

        sfv = findViewById<SurfaceView>(R.id.sfv)
        initFirstVideo()
        setRenderSurface()
    }

    private fun initFirstVideo() {
        val drawer = VideoDrawer()
        drawer.setVideoSize(1920, 1080)
        drawer.getSurfaceTexture {
            initPlayer(path, Surface(it), true)
        }
        mRenderer.addDrawer(drawer)
    }

    private fun initSecondVideo() {
        val drawer = VideoDrawer()
        drawer.setAlpha(0.5f)
        drawer.setVideoSize(1920, 1080)
        drawer.getSurfaceTexture {
            initPlayer(path2, Surface(it), false)
        }
        mRenderer.addDrawer(drawer)

        Handler().postDelayed({
            drawer.scale(0.5f, 0.5f)
        }, 1000)
    }

    private fun initPlayer(path: String, sf: Surface, withSound: Boolean) {
        val videoDecoder = VideoDecoder(path, null, sf)
        threadPool.execute(videoDecoder)
        videoDecoder.goOn()

        videoDecoder.setStateListener(object : DefDecoderStateListener {
            override fun decodeOneFrame(decodeJob: BaseDecoder?, frame: Frame) {
                mRenderer.notifySwap(frame.bufferInfo.presentationTimeUs)
            }
        })

//        if (withSound) {
//            val audioDecoder = AudioDecoder(path)
//            threadPool.execute(audioDecoder)
//            audioDecoder.goOn()
//        }
    }

    private fun setRenderSurface() {
        mRenderer.setSurface(sfv)
    }

}