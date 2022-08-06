package com.shixin.ndk_practice

import android.app.Application
import com.tencent.bugly.crashreport.CrashReport

class App:Application (){
    override fun onCreate() {
        super.onCreate()
        CrashReport.initCrashReport(getApplicationContext(), "b5500b224d", true)
    }
}