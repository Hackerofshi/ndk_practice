package com.shixin.ndk_practice.record;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.opengl.GLSurfaceView;
import android.view.Surface;

import com.shixin.ndk_practice.opengl.EglHelper;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.opengles.GL10;

public abstract class BaseVideoRecorder {
    public final WeakReference<BaseVideoRecorder> mVideoRecorderWr = new WeakReference<BaseVideoRecorder>(this);

    //硬编码MediaCodec的surface
    private Surface surface;

    //相机共享EGL 上下文
    private final EGLContext eglContext;
    private final Context context;


    private GLSurfaceView.Renderer renderer;

    private MediaMuxer mMediaMuxer;
    private MediaCodec videoCodec;
    private VideoRenderThread mRenderThread;
    private VideoEncoderThread mVideoThread;


    public void setRenderer(GLSurfaceView.Renderer renderer) {
        this.renderer = renderer;
        mRenderThread = new VideoRenderThread(mVideoRecorderWr);
    }

    protected BaseVideoRecorder(Context context, EGLContext eglContext) {
        this.eglContext = eglContext;
        this.context = context;
    }

    /**
     * 初始化参数
     *
     * @param audioPath   背景音乐的地址
     * @param outPath     输出文件的路径
     * @param videoWidth  录制的宽度
     * @param videoHeight 录制的高度
     */
    public void initVideo(String audioPath, String outPath, int videoWidth, int videoHeight) {
        try {
            mRenderThread.setSize(videoWidth, videoHeight);
            mMediaMuxer = new MediaMuxer(outPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
            initVideoCodec(videoWidth, videoHeight);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void startRecord() {
        mRenderThread.start();
        mVideoThread.start();
    }

    public void stopRecord() {
        mRenderThread.requestExit();
        mVideoThread.requestExit();
    }


    private void initVideoCodec(int width, int height) throws IOException {
        MediaFormat videoFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, width, height);
        //设置颜色格式
        videoFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        videoFormat.setInteger(MediaFormat.KEY_BIT_RATE, width * height * 4);

        //设置帧率
        videoFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 24);
        //设置I帧的间隔时间
        videoFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1);
        //创建编码器
        videoCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC);
        videoCodec.configure(videoFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);

        surface = videoCodec.createInputSurface();
        // 开启一个编码采集 InputSurface 上的数据，合成视频
        mVideoThread = new VideoEncoderThread(mVideoRecorderWr);
    }


    private RecordListener mRecordListener;

    public void setOnRecordListener(RecordListener recordListener) {
        this.mRecordListener = recordListener;
    }

    public interface RecordListener {
        void onTime(long times);
    }

    //视频编码线程
    private static final class VideoEncoderThread extends Thread {
        private final WeakReference<BaseVideoRecorder> mVideoRecoderWr;
        private final MediaMuxer mMediaMuxer;
        private volatile boolean mShouldExit = false;
        private final MediaCodec mVideoCodec;
        private final MediaCodec.BufferInfo mBufferInfo;
        private int mVideoTrackIndex = -1;
        private long mVideoPts = 0;

        public VideoEncoderThread(WeakReference<BaseVideoRecorder> videoRecorderWr) {
            this.mVideoRecoderWr = videoRecorderWr;
            mVideoCodec = videoRecorderWr.get().videoCodec;
            mMediaMuxer = videoRecorderWr.get().mMediaMuxer;
            mBufferInfo = new MediaCodec.BufferInfo();
        }

        @Override
        public void run() {
            try {
                //启动mediaCodec 编码器
                mVideoCodec.start();
                while (true) {
                    if (mShouldExit) {
                        return;
                    }

                    BaseVideoRecorder videoRecorder = mVideoRecoderWr.get();
                    if (videoRecorder == null) {
                        return;
                    }
                    //从surface  上获取数据，编码成H264,通过MediaMuxer合成MP4
                    int outputBufferIndex = mVideoCodec.dequeueOutputBuffer(mBufferInfo, 0);
                    if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                        mVideoTrackIndex = mMediaMuxer.addTrack(mVideoCodec.getOutputFormat());
                        mMediaMuxer.start();
                    } else {
                        while (outputBufferIndex >= 0) {
                            //获取数据
                            ByteBuffer outputBuffer = mVideoCodec.getOutputBuffer(outputBufferIndex);
                            outputBuffer.position(mBufferInfo.offset);
                            outputBuffer.limit(mBufferInfo.offset + mBufferInfo.size);

                            //修改pts
                            if (mVideoPts == 0) {
                                mVideoPts = mBufferInfo.presentationTimeUs;
                            }
                            mBufferInfo.presentationTimeUs -= mVideoPts;


                            //写入数据
                            mMediaMuxer.writeSampleData(mVideoTrackIndex, outputBuffer, mBufferInfo);

                            // 回调当前录制的时间
                            if (videoRecorder.mRecordListener != null) {
                                videoRecorder.mRecordListener.onTime(mBufferInfo.presentationTimeUs / 1000);
                            }

                            mVideoCodec.releaseOutputBuffer(outputBufferIndex, false);

                            outputBufferIndex = mVideoCodec.dequeueOutputBuffer(mBufferInfo, 0);
                        }
                    }

                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                onDestroy();
            }

        }

        private void onDestroy() {
            try {
                mVideoCodec.stop();
                mVideoCodec.release();
                mMediaMuxer.stop();
                mMediaMuxer.release();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }


        private void requestExit() {
            mShouldExit = true;
        }
    }


    //视频渲染线程
    private static final class VideoRenderThread extends Thread {
        private final WeakReference<BaseVideoRecorder> mVideoRecorderWr;
        private volatile boolean mShouldExit = false;
        private final EglHelper mEglHelper;
        private boolean mHashCreateContext = false;
        private boolean mHashSurfaceCreated = false;
        private boolean mHashSurfaceChanged = false;
        private int mWidth;
        private int mHeight;


        private VideoRenderThread(WeakReference<BaseVideoRecorder> videoRecoderWr) {
            this.mVideoRecorderWr = videoRecoderWr;
            mEglHelper = new EglHelper();
        }

        @Override
        public void run() {
            try {
                while (true) {
                    if (mShouldExit) {
                        return;
                    }
                    BaseVideoRecorder baseVideoRecoder = mVideoRecorderWr.get();
                    if (baseVideoRecoder == null) {
                        return;
                    }
                    //创建EGL上下文
                    if (!mHashCreateContext) {
                        mEglHelper.initCreateEgl(baseVideoRecoder.surface, baseVideoRecoder.eglContext);
                        mHashCreateContext = true;
                    }
                    //回调Render
                    GL10 gl = (GL10) mEglHelper.getEglContext().getGL();
                    if (!mHashSurfaceCreated) {
                        baseVideoRecoder.renderer.onSurfaceCreated(gl, mEglHelper.getEGLConfig());
                        mHashSurfaceCreated = true;
                    }
                    if (!mHashSurfaceChanged) {
                        baseVideoRecoder.renderer.onSurfaceChanged(gl, mWidth, mHeight);
                        mHashSurfaceChanged = true;
                    }

                    baseVideoRecoder.renderer.onDrawFrame(gl);

                    mEglHelper.swapBuffers();
                    //60fps
                    Thread.sleep(16);
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                onDestroy();
            }
        }

        private void onDestroy() {
            mEglHelper.destroy();
        }

        private void requestExit() {
            mShouldExit = true;
        }

        public void setSize(int width, int height) {
            this.mWidth = width;
            this.mHeight = height;
        }
    }

}
