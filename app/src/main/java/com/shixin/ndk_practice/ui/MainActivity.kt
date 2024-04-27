package com.shixin.ndk_practice.ui

import android.Manifest
import android.content.Intent
import android.media.audiofx.BassBoost
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.provider.Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION
import android.util.Log
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import com.blankj.utilcode.util.LogUtils
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
            //val  i = 5/0
            startActivity(Intent(this, BitmapActivity::class.java))
        }


        binding.btn5.setOnClickListener {
            startActivity(Intent(this, NativeFileActivity::class.java))
        }

        binding.btn6.setOnClickListener {
            startActivity(Intent(this, AudioRecoderActivity::class.java))
        }
        binding.btn7.setOnClickListener {
            startActivity(Intent(this, VideoRecordActivity::class.java))
        }
        binding.btn8.setOnClickListener {
            startActivity(Intent(this, ThreeDimensionalMainActivity::class.java))
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

    private fun checkStorageManagerPermission() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.R ||
            Environment.isExternalStorageManager()
        ) {
            Toast.makeText(this, "已获得访问所有文件权限", Toast.LENGTH_SHORT).show()
        } else {
            val builder = AlertDialog.Builder(this)
                .setMessage("本程序需要您同意允许访问所有文件权限")
                .setPositiveButton("确定") { _, _ ->
                    val intent = Intent(ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION)
                    startActivity(intent)
                }
            builder.show()
        }
    }
}