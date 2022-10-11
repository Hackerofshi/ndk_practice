package com.tech.pratice_audio;

public class AudioPlayer {
    static {
        System.loadLibrary("native-lib");
    }
    public native static void nativePlayPcm(String pcmPath);
    public native static void nativeStopPcm();
}
