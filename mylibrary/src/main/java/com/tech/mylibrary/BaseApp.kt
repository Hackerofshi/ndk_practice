package com.tech.mylibrary

import android.app.Application
import com.tencent.bugly.crashreport.CrashReport

open class BaseApp : Application() {
    override fun onCreate() {
        super.onCreate()
        CrashReport.initCrashReport(getApplicationContext(), "b5500b224d", true)
    }
}