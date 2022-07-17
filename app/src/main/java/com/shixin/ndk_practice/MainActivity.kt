package com.shixin.ndk_practice

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.shixin.ndk_practice.databinding.ActivityMainBinding
import com.shixin.ndk_practice.practicec.Test1

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.btn1.setOnClickListener {
            val test = Test1()
            Log.i("TAG", "onCreate:${test.name} ")
            test.test1()
            Log.i("TAG", "onCreate:${test.name} ")

            Log.i("TAG", "onCreate:${Test1.age} ")
            Test1.test2()
            Log.i("TAG", "onCreate:${Test1.age} ")

            Test1.callUUIDMethod()

            val p = Test1.createPoint()

            Log.i("TAG", "createPoint:${p.x} ")

        }
    }

}