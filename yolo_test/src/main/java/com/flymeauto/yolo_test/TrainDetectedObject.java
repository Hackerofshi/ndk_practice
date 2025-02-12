package com.flymeauto.yolo_test;

import android.graphics.Bitmap;

import java.io.Serializable;

public class TrainDetectedObject  implements Serializable {
    public String label;
    public String labelName;
    public float prob;
    public float x;
    public float y;
    public float w;
    public float h;


}