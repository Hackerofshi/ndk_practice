package com.shixin.ndk_practice.ui

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.os.Environment
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.shixin.ndk_practice.R
import com.shixin.ndk_practice.databinding.ActivityBitmapBinding
import com.shixin.ndk_practice.practicec.Compressor
import com.shixin.ndk_practice.util.NDKUtils

class BitmapActivity : AppCompatActivity() {
    private lateinit var binding: ActivityBitmapBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityBitmapBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.btn1.setOnClickListener {
            Log.i("TAG", "onCreate: " + Environment.getExternalStorageDirectory())
            val bitmap = BitmapFactory.decodeResource(resources, R.drawable.dzzz)
            val Compressor = Compressor()
            val bitmapWip = Bitmap.createBitmap(
                bitmap.getWidth(),
                bitmap.getHeight(), Bitmap.Config.ARGB_8888
            )

            Compressor.convertBmp(
                bitmap,
                bitmapWip,
            )

           // NDKUtils.testCmd()
        }

    }
}