package com.shixin.jpeg_pratice

import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    var imageView: ImageView? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        imageView = findViewById<ImageView>(R.id.img)

        onJniBitmap()
    }

    fun onJniBitmap() {
        val bitmap = BitmapFactory.decodeResource(resources, R.drawable.normal)
        val color = bitmap.getPixel(0, 0)
        Log.e(
            "main",
            "java getPixel[0][0] " + color + "=" + Integer.toHexString(color)
        )
        val jniBitmap = JNIBitmap()
        val start = System.currentTimeMillis()
        if (jniBitmap.negative(bitmap) == 1) {
            Log.e(
                "main",
                "negative cost:" + (System.currentTimeMillis() - start)
            )
            imageView?.setImageBitmap(bitmap)
            val color2 = bitmap.getPixel(0, 0)
            Log.e(
                "main",
                "java getPixel[0][0] " + color2 + "=" + Integer.toHexString(color2)
            )
        }
        val start2 = System.currentTimeMillis()
        val newBitmap = jniBitmap.leftRight(bitmap)
        if (newBitmap != null) {
            Log.e(
                "main",
                "leftRight cost:" + (System.currentTimeMillis() - start2)
            )
            imageView?.setImageBitmap(newBitmap)
        }
    }
}