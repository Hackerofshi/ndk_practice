package com.tech.pratice_ffmpeg

import android.view.Surface

object Player {
    external fun ffmpegInfo(): String

    external fun createPlayer(path: String, surface: Surface): Int

    external fun play(player: Int)

    external fun pause(player: Int)

    external fun stop(player: Int)

    external fun testAVIOContext(path: String)
    external fun startTCPClient(ip: String?, port: Int)
    external fun startUDPClient()
    external fun testAVIOAndUDP(fileName: String, surface: Surface)
    external fun testThread()


    init {
        System.loadLibrary("native-lib")
    }


}