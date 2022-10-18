package com.shixin.ndk_practice.ui

import android.Manifest
import android.content.Intent
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.shixin.ndk_practice.databinding.ActivityMainBinding
import com.shixin.ndk_practice.practicec.Parcel
import com.shixin.ndk_practice.practicec.Test1
import com.shixin.ndk_practice.practicec.TestArray
import com.shixin.ndk_practice.practicec.TestPoniter
import permissions.dispatcher.NeedsPermission
import permissions.dispatcher.RuntimePermissions


@RuntimePermissions
class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        initLocationWithPermissionCheck()

        initevent()

       // val a = 5 / 0
    }

    private fun initevent() {
        //测试jni方法
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

        binding.btn2.setOnClickListener {

            //共享内存的练习
            val parcel = Parcel()
            parcel.writeInt(10)
            parcel.writeInt(12)

            parcel.setDataPosition(0)

            val readInt = parcel.readInt()

            Log.i("TAG", "parcel:${readInt} ")

            val testArray = TestArray()
            val arr = arrayOf(1, 2)

            TestArray.sort(arr.toIntArray())
            //  TestArray.localRef()
            //  TestArray.globalRef("str")
            //  TestArray.getGlobalRef()

            //初始化缓存
            //  TestArray.initStaticCache()

            //  TestArray.staticLocalCache("leo")

            // TestArray.exeception()


            TestPoniter.test1()
            TestPoniter.test2()

        }

        binding.btn3.setOnClickListener {
            startActivity(Intent(this, SurfaceViewActivity::class.java))
        }


        binding.btn4.setOnClickListener {
            startActivity(Intent(this, BitmapActivity::class.java))
        }


        binding.btn5.setOnClickListener {
            startActivity(Intent(this, NativeFileActivity::class.java))
        }
    }

    @NeedsPermission(
        Manifest.permission.ACCESS_COARSE_LOCATION,
        Manifest.permission.ACCESS_FINE_LOCATION,
        Manifest.permission.CAMERA,
        Manifest.permission.RECORD_AUDIO,
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.BLUETOOTH,
    )
    fun initLocation() {
    }
}