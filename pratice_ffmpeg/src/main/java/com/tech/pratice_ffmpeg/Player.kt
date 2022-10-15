package com.tech.pratice_ffmpeg

import android.view.Surface

object Player {
    public external fun ffmpegInfo(): String

    public external fun createPlayer(path: String, surface: Surface): Int

    public external fun play(player: Int)

    public external fun pause(player: Int)

    public external fun stop(player: Int)


    init {
        System.loadLibrary("native-lib")
    }


}