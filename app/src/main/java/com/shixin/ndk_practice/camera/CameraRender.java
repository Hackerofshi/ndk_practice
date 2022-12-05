package com.shixin.ndk_practice.camera;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.Matrix;

import com.shixin.ndk_practice.R;
import com.shixin.ndk_practice.opengl.BaseRender;
import com.shixin.ndk_practice.opengl.FboRender;
import com.shixin.ndk_practice.util.Utils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class CameraRender extends BaseRender {

    /**
     * 顶点坐标
     */
    private final float[] mVertexCoordinate = new float[]{
            -1f, -1f,
            1f, -1f,
            -1f, 1f,
            1f, 1f
    };
    /**
     * 纹理坐标
     */
    private final float[] mFragmentCoordinate = new float[]{
            0f, 1f,
            1f, 1f,
            0f, 0f,
            1f, 0f
    };


    private final Context mContext;
    private final FboRender mFboRender;

    private final FloatBuffer mVertexBuffer;
    private final FloatBuffer mFragmentBuffer;
    private int mProgram;
    private int vPosition;
    private int fPosition;
    private int mVboId;
    private SurfaceTexture mCameraSt;
    private int uMatrix;
    private final float[] matrix = new float[16];

    public CameraRender(Context context) {
        this.mContext = context;

        mVertexBuffer = ByteBuffer.allocateDirect(mVertexCoordinate.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(mVertexCoordinate);
        mVertexBuffer.position(0);

        mFragmentBuffer = ByteBuffer.allocateDirect(mFragmentCoordinate.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(mFragmentCoordinate);
        mFragmentBuffer.position(0);

        mFboRender = new FboRender(mContext);

        // fbo 的坐标是标准坐标
        Matrix.orthoM(matrix, 0, -1, 1, -1f, 1f, -1f, 1f);
    }


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        mFboRender.onSurfaceCreated(mViewWidth, mViewHeight);
        String vertexSource = Utils.getGLResource(mContext, R.raw.vertex_shader_matrix);
        String fragmentSource = Utils.getGLResource(mContext, R.raw.fragment_shader_camera);
        mProgram = Utils.createProgram(vertexSource, fragmentSource);
        // 获取坐标
        vPosition = GLES20.glGetAttribLocation(mProgram, "v_Position");
        fPosition = GLES20.glGetAttribLocation(mProgram, "f_Position");
        uMatrix = GLES20.glGetUniformLocation(mProgram, "u_Matrix");

        // 创建 vbos
        int[] vBos = new int[1];
        GLES20.glGenBuffers(1, vBos, 0);
        // 绑定 vbos
        mVboId = vBos[0];
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVboId);
        // 开辟 vbos
        GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, (mVertexCoordinate.length + mFragmentCoordinate.length) * 4,
                null, GLES20.GL_STATIC_DRAW);
        // 赋值 vbos
        GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, 0, mVertexCoordinate.length * 4, mVertexBuffer);
        GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, mVertexCoordinate.length * 4,
                mFragmentCoordinate.length * 4, mFragmentBuffer);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);

        // 激活 program
        GLES20.glUseProgram(mProgram);


        // 生成绑定纹理
        int[] textures = new int[1];
        GLES20.glGenTextures(1, textures, 0);
        int cameraTextureId = textures[0];
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_BINDING_EXTERNAL_OES, cameraTextureId);
        // 设置纹理环绕方式
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_BINDING_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_BINDING_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
        // 设置纹理过滤方式
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_BINDING_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_BINDING_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

        //讲纹理id交给摄像头
        mCameraSt = new SurfaceTexture(cameraTextureId);

        if (mRenderListener != null) {
            mRenderListener.onSurfaceCreated(mCameraSt);
        }
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_BINDING_EXTERNAL_OES, 0);

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mFboRender.onSurfaceChanged(width, height);
        GLES20.glViewport(0, 0, width, height);
        // fbo 的坐标是标准坐标
        Matrix.rotateM(matrix, 0, 180, 1, 0, 0);

    }

    @Override
    public void onDrawFrame(GL10 gl) {
        // 绑定 fbo
        mFboRender.onBindFbo();
        GLES20.glUseProgram(mProgram);
        mCameraSt.updateTexImage();

        // 设置正交投影参数
        GLES20.glUniformMatrix4fv(uMatrix, 1, false, matrix, 0);

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVboId);
        /*
         * 设置坐标
         * 2：2个为一个点
         * GLES20.GL_FLOAT：float 类型
         * false：不做归一化
         * 8：步长是 8
         */
        GLES20.glEnableVertexAttribArray(vPosition);
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8,
                0);
        GLES20.glEnableVertexAttribArray(fPosition);
        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8,
                mVertexCoordinate.length * 4);


        // 绘制到 fbo
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
        // 解绑
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
        mFboRender.onUnbindFbo();
        // 再把 fbo 绘制到屏幕
        mFboRender.onDrawFrame();
    }

    /**
     * 重置矩阵参数
     */
    public void resetMatrix() {
        Matrix.setIdentityM(matrix, 0);
    }

    /**
     * 旋转矩阵
     *
     * @param offset 偏移
     * @param a      角度
     * @param x      x轴
     * @param y      y轴
     * @param z      z轴
     */
    public void rotateMatrix(int offset, float a, float x, float y, float z) {
        Matrix.rotateM(matrix, offset, a, x, y, z);
    }

    private RenderListener mRenderListener;

    public void setOnRenderListener(RenderListener surfaceRenderListener) {
        this.mRenderListener = surfaceRenderListener;
    }

    public interface RenderListener {
        void onSurfaceCreated(SurfaceTexture surfaceTexture);
    }

    public int getTextureId() {
        return mFboRender.getTextureId();
    }
}
