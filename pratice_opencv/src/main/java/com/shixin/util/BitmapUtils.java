package com.shixin.util;

import android.graphics.Bitmap;

/**
 * Created by hcDarren on 2018/12/9.
 */

public class BitmapUtils {
    public static native int gary(Bitmap src);


    //只能针对单个像素进行处理，无法批量的对像素进行处理，
    // 如高斯模糊，高斯模糊需要对像素点周围进行处理。
    /*public static Bitmap gary(Bitmap src) {
        // 怎么变成灰度的？ 矩阵去操作
        Bitmap dst = Bitmap.createBitmap(src.getWidth(), src.getHeight(), src.getConfig());
        Canvas canvas = new Canvas(dst);

        Paint paint = new Paint();
        paint.setDither(true);
        paint.setAntiAlias(true);

        // gary  f = 0.11f*R + 0.59f*B + 0.30f*G
        *//*ColorMatrix colorMatrix = new ColorMatrix(new float[]{
                0.213f,0.715f,0.072f,0,0,
                0.213f,0.715f,0.072f,0,0,
                0.213f,0.715f,0.072f,0,0,
                0,0,0,1,0,
        });*//*
        // 原图？
        *//*ColorMatrix colorMatrix = new ColorMatrix(new float[]{
                1,0,0,0,0,
                0,1,0,0,0,
                0,0,1,0,0,
                0,0,0,1,0,
        });*//*
        // 底片效果
        *//*ColorMatrix colorMatrix = new ColorMatrix(new float[]{
                -1,0,0,0,255,
                0,-1,0,0,255,
                0,0,-1,0,255,
                0,0,0,1,0,
        });*//*
     *//*ColorMatrix colorMatrix = new ColorMatrix(new float[]{
                1.2f,0,0,0,0,
                0,1.2f,0,0,0,
                0,0,1.2f,0,0,
                0,0,0,1,0,
        });*//*

        // 没办法做到高斯模式 ，
        ColorMatrix colorMatrix = new ColorMatrix(new float[]{
                1.2f, 0, 0, 0, 0,
                0, 1.2f, 0, 0, 0,
                0, 0, 1.2f, 0, 0,
                0, 0, 0, 1, 0,
        });

        // colorMatrix.setSaturation(0);
        paint.setColorFilter(new ColorMatrixColorFilter(colorMatrix));

        canvas.drawBitmap(src, 0, 0, paint);
        return dst;
    }*/


    //色彩：针对每个（单个）处理
    //特效滤镜：拼图
    //艺术滤镜：针对多个像素做运算  都可以做到
   /* public static Bitmap gary2(Bitmap src) {
        // 怎么变成灰度的？ 矩阵去操作
         Bitmap dst = Bitmap.createBitmap(src.getWidth(), src.getHeight(), src.getConfig());
        // java 层像素来操作
        int[] pixels = new int[src.getWidth() * src.getHeight()];

        //放到像素集合里面
        src.getPixels(pixels, 0, src.getWidth(), 0, 0, src.getWidth(), src.getHeight());

        // 只处理上半部分 pixels.length/2
        for (int i = 0; i < pixels.length; i++) {
            int pixel = pixels[i];

            int a = (pixel >> 24) & 0xff;    取出alpha
            int r = (pixel >> 16) & 0xff;    取出red
            int g = (pixel >> 8) & 0xff;     取出green
            int b = pixel & 0xff;            取出blue
            // f = 0.213f * r + 0.715f * g + 0.072f * b
            int gery = (int) (0.213f * r + 0.715f * g + 0.072f * b);
            pixels[i] = (a << 24) | (gery << 16) | (gery << 8) | gery;


            // 黑白
            // int black_white = (a + r + b) / 3 > 125 ? 255 : 0;
            // pixels[i] = (a << 24) | (black_white << 16) | (black_white << 8) | black_white;

        }

        return src;
    }*/


    // 需要设置options
    //         val options: BitmapFactory.Options = BitmapFactory.Options()
    //        // 版本 复用 Bitmap 内存复用
    //        options.inMutable = true


    /* public static Bitmap gary2(Bitmap src) {
        // 怎么变成灰度的？ 矩阵去操作
        // Bitmap dst = Bitmap.createBitmap(src.getWidth(), src.getHeight(), src.getConfig());
        // java 层像素来操作
        int[] pixels = new int[src.getWidth() * src.getHeight()];

        //放到像素集合里面
        src.getPixels(pixels, 0, src.getWidth(), 0, 0, src.getWidth(), src.getHeight());

        // 只处理上半部分 pixels.length/2
        for (int i = 0; i < pixels.length; i++) {
            int pixel = pixels[i];

            int a = (pixel >> 24) & 0xff;    取出alpha
            int r = (pixel >> 16) & 0xff;    取出red
            int g = (pixel >> 8) & 0xff;     取出green
            int b = pixel & 0xff;            取出blue
            // f = 0.213f * r + 0.715f * g + 0.072f * b
            int gery = (int) (0.213f * r + 0.715f * g + 0.072f * b);
            pixels[i] = (a << 24) | (gery << 16) | (gery << 8) | gery;


            // 黑白
            // int black_white = (a + r + b) / 3 > 125 ? 255 : 0;
            // pixels[i] = (a << 24) | (black_white << 16) | (black_white << 8) | black_white;

        }

         src.setPixels(pixels, 0, src.getWidth(), 0, 0, src.getWidth(), src.getHeight());
        return src;
    }*/
}
