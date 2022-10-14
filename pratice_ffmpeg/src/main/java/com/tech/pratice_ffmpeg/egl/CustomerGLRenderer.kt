package com.tech.pratice_ffmpeg.egl

import android.opengl.EGLExt.EGL_RECORDABLE_ANDROID
import android.opengl.GLES20
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.View
import com.tech.pratice_ffmpeg.drawer.IDrawer
import java.lang.ref.WeakReference

class CustomerGLRenderer : SurfaceHolder.Callback {

    private val mThread: RenderThread = RenderThread()
    private var mSurfaceView: WeakReference<SurfaceView>? = null
    private var mSurface: Surface? = null

    private val mDrawers = mutableListOf<IDrawer>()

    init {
        mThread.start()
    }

    fun setSurface(surfaceView: SurfaceView) {
        mSurfaceView = WeakReference(surfaceView)
        surfaceView.holder.addCallback(this)
        surfaceView.addOnAttachStateChangeListener(object : View.OnAttachStateChangeListener {
            override fun onViewAttachedToWindow(v: View?) {
                stop()
            }

            override fun onViewDetachedFromWindow(v: View?) {
            }
        })
    }

    fun setSurface(surface: Surface, width: Int, height: Int) {
        mSurface = surface
        mThread.onSurfaceCreate()
        mThread.onSurfaceChange(width, height)
    }

    fun setRenderMode(mode: RenderMode) {
        mThread.setRenderMode(mode)
    }

    fun notifySwap(timeUs: Long) {
        mThread.notifySwap(timeUs)
    }

    fun addDrawer(drawer: IDrawer) {
        mDrawers.add(drawer)
    }

    private fun stop() {
        mThread.onSurfaceStop()
        mSurface = null
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        mSurface = holder.surface
        mThread.onSurfaceCreate()
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        mThread.onSurfaceChange(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        mThread.onSurfaceDestroy()
    }

    inner class RenderThread : Thread() {
        //渲染状态
        private var mSate = RenderState.NO_SURFACE

        private var mEGLSurfaceHolder: EGLSurfaceHolder? = null

        //是否绑定了eglsurface
        private var mHaveBindEGLContext = false

        //是否已经新建过egl上下文，用于判断是否需要生产新的纹理id
        private var mNeverCreateEglContext = true

        private var mWidth = 0
        private var mHeight = 0

        private var mWaitLock = Object()

        private var mCurTimestamp: Long = 0L
        private var mLastTimestamp = 0L

        private var mRenderMode = RenderMode.RENDER_WHEN_DIRTY

        private fun holdOn() {
            synchronized(mWaitLock) {
                mWaitLock.wait()
            }
        }

        private fun notifyGo() {
            synchronized(mWaitLock) {
                mWaitLock.notify()
            }
        }


        fun setRenderMode(mode: RenderMode) {
            mRenderMode = mode
        }

        fun onSurfaceCreate() {
            mSate = RenderState.FRESH_SURFACE
            notifyGo()
        }

        fun onSurfaceChange(widht: Int, height: Int) {
            mWidth = widht
            mHeight = height
            mSate = RenderState.SURFACE_CHANGE
            notifyGo()
        }

        fun onSurfaceDestroy() {
            mSate = RenderState.SURFACE_DESTROY
            notifyGo()
        }

        fun onSurfaceStop() {
            mSate = RenderState.STOP
            notifyGo()
        }

        fun notifySwap(timeus: Long) {
            synchronized(mCurTimestamp) {
                mCurTimestamp = timeus
            }
            notifyGo()
        }

        override fun run() {
            initEGL()
            while (true) {
                when (mSate) {
                    RenderState.FRESH_SURFACE -> {
                        createEGLSurfaceFirst()
                        holdOn()
                    }
                    RenderState.SURFACE_CHANGE -> {
                        createEGLSurfaceFirst()
                        GLES20.glViewport(0, 0, mWidth, mHeight)
                        configWordSize()
                        mSate = RenderState.RENDERING
                    }
                    RenderState.RENDERING -> {
                        render()
                        if (mRenderMode == RenderMode.RENDER_WHEN_DIRTY) {
                            holdOn()
                        }
                    }
                    RenderState.SURFACE_DESTROY -> {
                        destroyEGLSurface()
                        mSate = RenderState.NO_SURFACE
                    }
                    RenderState.STOP -> {
                        releaseEGL()
                        return
                    }
                    else -> {
                        holdOn()
                    }
                }

                if (mRenderMode == RenderMode.RENDER_CONTINUOUSLY) {
                    sleep(16)
                }
            }
        }

        private fun initEGL() {
            mEGLSurfaceHolder = EGLSurfaceHolder()
            mEGLSurfaceHolder?.init(null, EGL_RECORDABLE_ANDROID)
        }

        private fun createEGLSurfaceFirst() {
            if (!mHaveBindEGLContext) {
                mHaveBindEGLContext = true
                createEGLSurface()
                if (mHaveBindEGLContext) {
                    mNeverCreateEglContext = false
                    GLES20.glClearColor(0f, 0f, 0f, 0f)
                    //开始混合，季半透明
                    GLES20.glEnable(GLES20.GL_BLEND)
                    GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA)
                    generateTextureID()
                }
            }
        }

        private fun generateTextureID() {
            val textureIds = OpenGLTools.createTextureIds(mDrawers.size)
            for ((idx, drawer) in mDrawers.withIndex()) {
                drawer.setTextureID(textureIds[idx])
            }
        }

        private fun createEGLSurface() {
            mEGLSurfaceHolder?.createEGLSurface(mSurface)
            mEGLSurfaceHolder?.makeCurrent()
        }

        private fun configWordSize() {
            mDrawers.forEach {
                it.setWorldSize(mWidth, mHeight)
            }
        }

        private fun render() {
            val render = if (mRenderMode == RenderMode.RENDER_CONTINUOUSLY) {
                true
            } else {
                synchronized(mCurTimestamp) {
                    if (mCurTimestamp > mLastTimestamp) {
                        mLastTimestamp = mCurTimestamp
                        true
                    } else {
                        false
                    }
                }
            }
            if (render) {
                GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT or GLES20.GL_DEPTH_BUFFER_BIT)
                mDrawers.forEach { it.draw() }
                mEGLSurfaceHolder?.setTimestamp(mCurTimestamp)
                mEGLSurfaceHolder?.swapBuffers()
            }
        }

        private fun destroyEGLSurface() {
            mEGLSurfaceHolder?.destroyEGLSurface()
            mHaveBindEGLContext = false
        }

        private fun releaseEGL() {
            mEGLSurfaceHolder?.release()
        }

    }

    enum class RenderState {
        NO_SURFACE, //没有有效的surface
        FRESH_SURFACE, //持有一个未初始化的新的surface
        SURFACE_CHANGE,//surface尺寸变化
        RENDERING,//初始化完毕，可以开始渲染
        SURFACE_DESTROY,//surface销毁
        STOP//停止绘制
    }

    enum class RenderMode {
        RENDER_CONTINUOUSLY,
        RENDER_WHEN_DIRTY
    }
}