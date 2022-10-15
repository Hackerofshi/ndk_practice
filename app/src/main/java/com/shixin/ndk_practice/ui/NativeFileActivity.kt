package com.shixin.ndk_practice.ui

import android.os.Bundle
import android.os.Environment
import androidx.appcompat.app.AppCompatActivity
import com.shixin.ndk_practice.databinding.ActivityNativeFileBinding
import com.shixin.ndk_practice.util.NDKUtils
import java.io.File

class NativeFileActivity : AppCompatActivity() {
    private lateinit var binding: ActivityNativeFileBinding
    private val path = Environment.getExternalStorageDirectory().absolutePath + "/test.mp4"
    private val path1 = Environment.getExternalStorageDirectory().absolutePath + "/text.txt"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityNativeFileBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.btn1.setOnClickListener {
            NDKUtils.read(path)
        }
        binding.btn2.setOnClickListener {
            val file = File(path1)
            if(!file.exists()){
               file.createNewFile()
            }
            NDKUtils.write(file.absolutePath)
        }
    }
}