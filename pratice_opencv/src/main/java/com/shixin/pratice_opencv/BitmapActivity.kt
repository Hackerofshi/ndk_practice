package com.shixin.pratice_opencv

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity

class BitmapActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_bitmap)
        val image1: ImageView = findViewById(R.id.img1)
        val image2: ImageView = findViewById(R.id.img2)
        val options: BitmapFactory.Options = BitmapFactory.Options()
        // 版本 复用 Bitmap 内存复用，不会反复的创建和释放
        //options.inMutable = true
        // 有啥区别？  ARGB_8888  32 位最好
        //options.inPreferredConfig = Bitmap.Config.RGB_565
        //options.inPreferredConfig = Bitmap.Config.ARGB_8888
        // RGB_565 747  ARGB_8888 1494  1 倍
        // ARGB_8888 -> RGB 565 RGB 5位R 6位G 5位B   16位
        //val src: Bitmap = BitmapFactory.decodeResource(resources, R.drawable.icon_test_qr1,options)
        val src: Bitmap = BitmapFactory.decodeResource(resources, R.drawable.icon_test_document)
        image1.setImageBitmap(src)
        Log.i("TAG", "onCreate: ${src.width}")
        Log.i("TAG", "onCreate: ${src.height}")
        // Log.e("TAG", src.byteCount / 1024.toString() + "")
        // BitmapUtils.gary(src)


        findViewById<Button>(R.id.test).setOnClickListener {
            val thread = Thread(object : Runnable {
                override fun run() {
                    val res = NdkBitmapUtils.findContours(src)

                    Log.i("TAG", "onCreate: ${res.width}")
                    Log.i("TAG", "onCreate: ${res.height}")

                    runOnUiThread {
                        image2.setImageBitmap(res)
                    }
                }
            })
            thread.start()
        }

        /*image2.setImageBitmap(res)*/
    }
}