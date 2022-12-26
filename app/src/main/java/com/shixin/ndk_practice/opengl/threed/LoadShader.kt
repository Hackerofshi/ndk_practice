package com.shixin.ndk_practice.opengl.threed

import android.content.Context
import android.content.res.Resources
import android.opengl.GLES20
import android.util.Log
import androidx.annotation.RawRes
import java.io.BufferedReader
import java.io.IOException
import java.io.InputStreamReader

const val TAG_LOAD_SHADER = "LOAD SHADER"

/**
 * 直接加载 OpenGL 脚本
 *
 * @param type        使用 [GLES20.GL_FRAGMENT_SHADER] 或者 [GLES20.GL_VERTEX_SHADER].
 * @param shaderCode  编译加载的gl脚本
 */
fun loadShader(type: Int, shaderCode: String): Int {
    return GLES20.glCreateShader(type).also { shader ->
        GLES20.glShaderSource(shader, shaderCode)
        GLES20.glCompileShader(shader)
    }
}

/**
 * 加载资源文件中的 OpenGL 脚本.
 *
 * @param context 上下文对象
 * @param vertexResId 顶点Shader资源ID
 * @param fragmentResId 片元Shader资源ID
 */
fun loadShaderWithResource(
    context: Context,
    @RawRes vertexResId: Int,
    @RawRes fragmentResId: Int
) :Int{
    val vertexProgram = readTextFileFromResource(context, vertexResId)
    val fragmentProgram = readTextFileFromResource(context, fragmentResId)
    return buildProgram(vertexProgram, fragmentProgram)

}


/**
 * 加载文件内容
 *
 * @param context 上下文对象
 * @param resourceId 资源id
 */
fun readTextFileFromResource(context: Context, resourceId: Int): String {
    val body = StringBuilder()

    try {
        val inputStr = context.resources.openRawResource(resourceId)
        val inputStreamReader = InputStreamReader(inputStr)
        val bufferedReader = BufferedReader(inputStreamReader)
        var nextLine: String?
        while (bufferedReader.readLine().also { nextLine = it } != null) {
            body.append(nextLine)
            body.append('\n')
        }
    } catch (e: IOException) {
        throw RuntimeException(
            "Could not open resource: $resourceId", e
        )
    } catch (nfe: Resources.NotFoundException) {
        throw RuntimeException(
            "Resource not found: "
                    + resourceId, nfe
        )
    }
    return body.toString()

}

/**
 * 构建 OpenGL 程序.
 *
 * @param vertexShaderSource    顶点 Shader 程序
 * @param fragmentShaderSource  片元 Shader 程序
 */
fun buildProgram(vertexShaderSource: String, fragmentShaderSource: String): Int {
    val program: Int
    //编译Shader 程序
    val vertexShader: Int = compileShader(GLES20.GL_VERTEX_SHADER, vertexShaderSource)
    val fragmentShader: Int = compileShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderSource)

    // 将其链接为一个程序
    program = linkProgram(vertexShader, fragmentShader)
    return program
}


/**
 * 加载 shader 程序，并将对应的句柄进行返回.
 *
 * @param type       使用 [GLES20.GL_FRAGMENT_SHADER] 或者 [GLES20.GL_VERTEX_SHADER].
 * @param shaderCode 编译加载的gl脚本，通常从文件中读取，参考 [readTextFileFromResource]
 */
fun compileShader(type: Int, shaderCode: String): Int {

    //创建新的Shader obj
    val shaderObjectId = GLES20.glCreateShader(type)
    if (shaderObjectId == 0) {
        Log.e(TAG_LOAD_SHADER, "Could not create new shader.")
        return 0
    }

    //pass in the Shader source
    GLES20.glShaderSource(shaderObjectId, shaderCode)

    //编译shader
    GLES20.glCompileShader(shaderObjectId)

    // Get the compilation status.
    val compileStatus = IntArray(1)
    GLES20.glGetShaderiv(
        shaderObjectId, GLES20.GL_COMPILE_STATUS,
        compileStatus, 0
    )

    // Print the shader info log to the Android log output.
    Log.v(
        TAG_LOAD_SHADER, """
            Results of compiling source:
            $shaderCode
            :${GLES20.glGetShaderInfoLog(shaderObjectId)}
            """.trimIndent()
    )

    //vertify the compile status
    if(compileStatus[0] == 0){
        // If it failed, delete the shader object.
        GLES20.glDeleteShader(shaderObjectId)
        Log.w(TAG_LOAD_SHADER, "Compilation of shader failed.")
        return 0
    }

    // Return the shader object ID.
    return shaderObjectId
}
/**
 * 链接两个Shader程序
 */
fun linkProgram(vertexShaderId: Int, fragmentShaderId: Int): Int {
    val programObjectId = GLES20.glCreateProgram()
    if(programObjectId == 0){
        Log.e(TAG_LOAD_SHADER,"Could not create new program")
        return 0;
    }

    // Attach the vertex shader to the program.
    GLES20.glAttachShader(programObjectId, vertexShaderId)

    // Attach the fragment shader to the program.
    GLES20.glAttachShader(programObjectId, fragmentShaderId)

    //link tht two shader together into a program
    GLES20.glLinkProgram(programObjectId)

    // Get the link status.
    val linkStatus = IntArray(1)
    GLES20.glGetProgramiv(programObjectId,GLES20.GL_LINK_STATUS,linkStatus,0)

    // Print the program info log to the Android log output.
    Log.v(
        TAG_LOAD_SHADER, """
     Results of linking program:
     ${GLES20.glGetProgramInfoLog(programObjectId)}
     """.trimIndent()
    )

    // Verify the link status.
    if (linkStatus[0] == 0) {
        // If it failed, delete the program object.
        GLES20.glDeleteProgram(programObjectId)
        Log.e(TAG_LOAD_SHADER, "Linking of program failed.")
        return 0
    }

    // Return the program object ID.
    return programObjectId

}