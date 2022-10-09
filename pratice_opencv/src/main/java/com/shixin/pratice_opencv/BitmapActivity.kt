package com.shixin.pratice_opencv

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity

class BitmapActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_bitmap)

        val options: BitmapFactory.Options = BitmapFactory.Options()
        // 版本 复用 Bitmap 内存复用，不会反复的创建和释放
        //options.inMutable = true
        // 有啥区别？  ARGB_8888  32 位最好
        //options.inPreferredConfig = Bitmap.Config.RGB_565

        // RGB_565 747  ARGB_8888 1494  1 倍
        // ARGB_8888 -> RGB 565 RGB 5位R 6位G 5位B   16位
        val src: Bitmap = BitmapFactory.decodeResource(resources, R.mipmap.mx)

        // Log.e("TAG", src.byteCount / 1024.toString() + "")
        // BitmapUtils.gary(src)
        val res = NdkBitmapUtils.hsv(src)

        val image2: ImageView = findViewById(R.id.img2)
        image2.setImageBitmap(res)
    }
}