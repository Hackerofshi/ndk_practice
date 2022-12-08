package com.shixin.ndk_practice.record;

import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.media.MediaRecorder;
import android.opengl.GLSurfaceView;
import android.view.Surface;

import com.blankj.utilcode.util.LogUtils;
import com.shixin.ndk_practice.opengl.EglHelper;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.concurrent.CyclicBarrier;

import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.opengles.GL10;

public abstract class BaseVideoRecorder {
    public final WeakReference<BaseVideoRecorder> mVideoRecorderWr = new WeakReference<>(this);

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


    private static final int AUDIO_SAMPLE_RATE = 44100;
    private static final int AUDIO_CHANNELS = 2;

    private MediaCodec mAudioCodec;
    private AudioEncoderThread mAudioEncoderThread;
    private AudioRecorderThread mAudioRecorderThread;


    private final CyclicBarrier mStartCb = new CyclicBarrier(2);
    private final CyclicBarrier mDestroyCb = new CyclicBarrier(2);
    private final CyclicBarrier audioCb = new CyclicBarrier(2);


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
     * @param outPath     输出文件的路径
     * @param videoWidth  录制的宽度
     * @param videoHeight 录制的高度
     */
    public void initVideo( String outPath, int videoWidth, int videoHeight) {
        try {
            mRenderThread.setSize(videoWidth, videoHeight);
            mMediaMuxer = new MediaMuxer(outPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
            initVideoCodec(videoWidth, videoHeight);
            initAudioCodec(AUDIO_SAMPLE_RATE, AUDIO_CHANNELS);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void startRecord() {
        mRenderThread.start();
        mVideoThread.start();


        mAudioEncoderThread.start();
        //  mAudioRecorderThread.start();

    }

    public void stopRecord() {
        mRenderThread.requestExit();
        mVideoThread.requestExit();

        mAudioEncoderThread.requestExit();
        // mAudioRecorderThread.requestExit();
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

    private void initAudioCodec(int sampleRate, int channels) throws IOException {
        MediaFormat audioFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, channels);
        audioFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);
        audioFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
        audioFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, sampleRate * channels * 2);
        // 创建音频编码器
        mAudioCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC);
        mAudioCodec.configure(audioFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        //mAudioCodec.start();

        // 开启一个编码采集 音乐播放器回调的 PCM 数据，合成视频
        mAudioEncoderThread = new AudioEncoderThread(mVideoRecorderWr);

        mAudioRecorderThread = new AudioRecorderThread(mVideoRecorderWr);

    }


    public static final class AudioRecorderThread extends Thread {
        private final int mMinBufferSize;
        private volatile boolean mShouldExit = false;
        private final MediaCodec mAudioCodec;
        private long mAudioPts = 0;
        private final AudioRecord mAudioRecord;
        // 这里是 pcm 数据
        private final byte[] mAudioData;
        private final CyclicBarrier audioCb;

        public AudioRecorderThread(WeakReference<BaseVideoRecorder> videoRecorderWr) {
            mAudioCodec = videoRecorderWr.get().mAudioCodec;
            mMinBufferSize = AudioRecord.getMinBufferSize(
                    BaseVideoRecorder.AUDIO_SAMPLE_RATE,
                    AudioFormat.CHANNEL_IN_STEREO,
                    AudioFormat.ENCODING_PCM_16BIT);

            mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                    BaseVideoRecorder.AUDIO_SAMPLE_RATE,
                    AudioFormat.CHANNEL_IN_STEREO,
                    AudioFormat.ENCODING_PCM_16BIT,
                    mMinBufferSize);

            mAudioData = new byte[mMinBufferSize];
            audioCb = videoRecorderWr.get().audioCb;
        }

        @Override
        public void run() {
            try {
                mAudioRecord.startRecording();
                while (true) {
                    if (mShouldExit) {
                        return;
                    }
                    //不断读取pcm数据
                    mAudioRecord.read(mAudioData, 0, mMinBufferSize);
                    // 把数据写入到 mAudioCodec 的 InputBuffer
                    int inputBufferIndex = mAudioCodec.dequeueInputBuffer(0);
                    if (inputBufferIndex >= 0) {
                        ByteBuffer byteBuffer = mAudioCodec.getInputBuffers()[inputBufferIndex];
                        byteBuffer.clear();
                        byteBuffer.put(mAudioData);

                        // pts  44100 * 2 *2
                        mAudioPts += (mMinBufferSize * 1000000) / BaseVideoRecorder.AUDIO_SAMPLE_RATE * BaseVideoRecorder.AUDIO_CHANNELS * 2;
                        // size 22050*2*2
                        mAudioCodec.queueInputBuffer(inputBufferIndex, 0, mMinBufferSize, mAudioPts, 0);
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
                mAudioCodec.stop();
                mAudioCodec.release();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        private void requestExit() {
            mShouldExit = true;
        }
    }


    public static final class AudioEncoderThread extends Thread {
        private final WeakReference<BaseVideoRecorder> mVideoRecorderWr;
        private final MediaMuxer mMediaMuxer;
        private volatile boolean mShouldExit = false;
        private final MediaCodec mAudioCodec;
        private final MediaCodec.BufferInfo mBufferInfo;
        private long mAudioPts = 0;
        private long mAudioPtsWrite = 0;
        private final CyclicBarrier mStartCb, mDestroyCb;
        private int mAudioTrackIndex = -1;

        private final AudioRecord mAudioRecord;
        // 这里是 pcm 数据
        private final byte[] mAudioData;
        private final int mMinBufferSize;

        public AudioEncoderThread(WeakReference<BaseVideoRecorder> videoRecorderWr) {
            this.mVideoRecorderWr = videoRecorderWr;
            mAudioCodec = videoRecorderWr.get().mAudioCodec;
            mMediaMuxer = videoRecorderWr.get().mMediaMuxer;
            mBufferInfo = new MediaCodec.BufferInfo();


            //各厂商实现存在差异
            mMinBufferSize = AudioRecord.getMinBufferSize(
                    BaseVideoRecorder.AUDIO_SAMPLE_RATE,
                    AudioFormat.CHANNEL_IN_STEREO,
                    AudioFormat.ENCODING_PCM_16BIT);

            mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                    BaseVideoRecorder.AUDIO_SAMPLE_RATE,
                    AudioFormat.CHANNEL_IN_STEREO,
                    AudioFormat.ENCODING_PCM_16BIT,
                    mMinBufferSize);

            mAudioData = new byte[mMinBufferSize];

            mStartCb = videoRecorderWr.get().mStartCb;
            mDestroyCb = videoRecorderWr.get().mDestroyCb;
        }

        @Override
        public void run() {
            try {
                mAudioCodec.start();
                mAudioRecord.startRecording();

                while (true) {
                    if (mShouldExit) {
                        return;
                    }

                    // 开启 start AudioCodec
                    // mAudioCodec.start();
                    BaseVideoRecorder baseVideoRecorder = mVideoRecorderWr.get();
                    if (baseVideoRecorder == null) {
                        return;
                    }

                    //不断读取pcm数据
                    mAudioRecord.read(mAudioData, 0, mMinBufferSize);
                    // 把数据写入到 mAudioCodec 的 InputBuffer
                    int inputBufferIndex = mAudioCodec.dequeueInputBuffer(0);
                    if (inputBufferIndex >= 0) {
                        ByteBuffer byteBuffer = mAudioCodec.getInputBuffers()[inputBufferIndex];
                        byteBuffer.clear();
                        byteBuffer.put(mAudioData);

                        // pts  44100 * 2 *2
                        mAudioPts += (long) (1.0 * mMinBufferSize / (AUDIO_SAMPLE_RATE * AUDIO_CHANNELS * (2)) * 1000000.0);
                        // size 22050*2*2
                        mAudioCodec.queueInputBuffer(inputBufferIndex, 0, mMinBufferSize, mAudioPts, 0);
                    }


                    int outputBufferIndex = mAudioCodec.dequeueOutputBuffer(mBufferInfo, 0);
                    LogUtils.i("==========>音频写入" + outputBufferIndex);

                    if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                        //在两个不同的Thread中分别执行音视频的录制+编码，
                        // 所以需要等待两个线程都执行完addTrack之后，再执行start。
                        mAudioTrackIndex = mMediaMuxer.addTrack(mAudioCodec.getOutputFormat());
                        //两个线程都执行到await再放开
                        mStartCb.await();
                        //mMediaMuxer.start();
                    } else {
                        while (outputBufferIndex >= 0) {
                            LogUtils.i("==========>outputBufferIndex=" + outputBufferIndex);
                            // 获取数据
                            ByteBuffer outBuffer = mAudioCodec.getOutputBuffers()[outputBufferIndex];
                            outBuffer.position(mBufferInfo.offset);
                            outBuffer.limit(mBufferInfo.offset + mBufferInfo.size);

                            // 修改 pts
                            if (mAudioPtsWrite == 0) {
                                mAudioPtsWrite = mBufferInfo.presentationTimeUs;
                            }
                            mBufferInfo.presentationTimeUs = mBufferInfo.presentationTimeUs - mAudioPtsWrite;

                            // 打印一下音频的 AAC 数据
                            byte[] data = new byte[outBuffer.remaining()];
                            outBuffer.get(data, 0, data.length);

                            LogUtils.i("==========>音频写入" + data.length);
                            LogUtils.i("==========> presentationTimeUs" + mBufferInfo.presentationTimeUs);
                            // 写入数据
                            mMediaMuxer.writeSampleData(mAudioTrackIndex, outBuffer, mBufferInfo);

                            mAudioCodec.releaseOutputBuffer(outputBufferIndex, false);
                            outputBufferIndex = mAudioCodec.dequeueOutputBuffer(mBufferInfo, 0);
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
                mAudioCodec.stop();
                mAudioCodec.release();
                mDestroyCb.await();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        private void requestExit() {
            mShouldExit = true;
        }
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
        private final CyclicBarrier mStartCb, mDestroyCb;

        public VideoEncoderThread(WeakReference<BaseVideoRecorder> videoRecorderWr) {
            this.mVideoRecoderWr = videoRecorderWr;
            mVideoCodec = videoRecorderWr.get().videoCodec;
            mMediaMuxer = videoRecorderWr.get().mMediaMuxer;
            mBufferInfo = new MediaCodec.BufferInfo();

            mStartCb = videoRecorderWr.get().mStartCb;
            mDestroyCb = videoRecorderWr.get().mDestroyCb;
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
                        mStartCb.await();
                        mMediaMuxer.start();
                    } else {
                        while (outputBufferIndex >= 0) {
                            //获取数据
                            ByteBuffer outputBuffer = mVideoCodec.getOutputBuffers()[outputBufferIndex];
                            outputBuffer.position(mBufferInfo.offset);
                            outputBuffer.limit(mBufferInfo.offset + mBufferInfo.size);

                            //修改pts
                            if (mVideoPts == 0) {
                                mVideoPts = mBufferInfo.presentationTimeUs;
                            }
                            mBufferInfo.presentationTimeUs -= mVideoPts;

                            LogUtils.i("==========>视频写入");
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
                mDestroyCb.await();
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


    private RecordListener mRecordListener;

    public void setOnRecordListener(RecordListener recordListener) {
        this.mRecordListener = recordListener;
    }

    public interface RecordListener {
        void onTime(long times);
    }


}
