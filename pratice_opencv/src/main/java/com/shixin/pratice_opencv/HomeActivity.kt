package com.shixin.pratice_opencv

import android.Manifest
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.database.ContentObserver
import android.location.LocationManager
import android.os.Bundle
import android.provider.Settings
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import com.blankj.utilcode.util.LogUtils
import permissions.dispatcher.NeedsPermission
import permissions.dispatcher.RuntimePermissions

@RuntimePermissions
class HomeActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_home)

        findViewById<Button>(R.id.btn1).setOnClickListener {
            startActivity(Intent(this, BitmapActivity::class.java))
        }
        findViewById<Button>(R.id.btn2).setOnClickListener {
            startActivity(Intent(this, CameraPreviewActivity::class.java))
        }
        findViewById<Button>(R.id.btn3).setOnClickListener {
            startActivity(Intent(this, LocalOcrActivity::class.java))
        }
        initCameraWithPermissionCheck()
        initGpsMonitor()

        observeWifiSwitch()
    }


    @NeedsPermission(
        Manifest.permission.CAMERA,
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.READ_EXTERNAL_STORAGE,
    )
    fun initCamera() {
    }

    //-----------------------------------------------监听gps----------------------------------------
    private val mGpsMonitor: ContentObserver = object : ContentObserver(null) {
        override fun onChange(selfChange: Boolean) {
            super.onChange(selfChange)
            val enabled = mLocationManager?.isProviderEnabled(LocationManager.GPS_PROVIDER)
            LogUtils.i("gps enabled? $enabled")
            if (enabled == true) {

            }
        }
    }

    private var mLocationManager: LocationManager? = null
    private fun initGpsMonitor() {
        mGpsMonitor
        getContentResolver().registerContentObserver(
            Settings.Secure
                .getUriFor(Settings.System.LOCATION_PROVIDERS_ALLOWED), false, mGpsMonitor
        );
    }

    private fun destroyGpsMonitor() {
        contentResolver.unregisterContentObserver(mGpsMonitor)
    }


    private fun observeWifiSwitch() {

        val filter = IntentFilter()
        filter.addAction("android.location.PROVIDERS_CHANGED")
        var receiver = Receiver()
        registerReceiver(receiver, filter)
    }

}

class Receiver : BroadcastReceiver() {
    private val GPS_ACTION = "android.location.PROVIDERS_CHANGED"

    override fun onReceive(context: Context?, intent: Intent) {
        LogUtils.i("gps enabled?")
        LogUtils.i("gps enabled?${intent.action}")
        val mLocationManager =
            context?.getSystemService(AppCompatActivity.LOCATION_SERVICE) as LocationManager
        val enabled = mLocationManager?.isProviderEnabled(LocationManager.GPS_PROVIDER)
        LogUtils.i("gps enabled? $enabled")
    }
}