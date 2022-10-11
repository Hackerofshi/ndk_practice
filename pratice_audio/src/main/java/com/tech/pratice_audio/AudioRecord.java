package com.tech.pratice_audio;

public class AudioRecord {
    static {
        System.loadLibrary("native-lib");
    }

    public native static void startRecord(String pcmPath);

}
