
# Sets the minimum version of CMake required to build the native library.
# 指定cmake最低编译版本
cmake_minimum_required(VERSION 3.10.2)

# 支持gnu++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# 需要引入我们头文件,以这个配置的目录为基准，可以是多个
include_directories(${PROJECT_SOURCE_DIR}/include)
# include_directories(src/main/jniLibs/other)

#添加共享库搜索路径
link_directories(./libs/${ANDROID_ABI})

# 日志输出在.cxx/cmake/debug//armeabi-v7a/build_output.txt
message("PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
message("ANDROID_ABI=${ANDROID_ABI}")
message("CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message("CMAKE_ANDROID_ARCH_ABI=${CMAKE_ANDROID_ARCH_ABI}")
message("CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")


# 指定源文件目录
AUX_SOURCE_DIRECTORY(${CMAKE_SOURCE_DIR}/source SRC_LIST)


# 1. 定义so库和头文件所在目录，方面后面使用
set(ffmpeg_lib_dir ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI})
set(ffmpeg_head_dir ${CMAKE_SOURCE_DIR}/ffmpeg)

# 2. 添加头文件目录
include_directories(${ffmpeg_head_dir}/include)

# 3. 添加ffmpeg相关的so库
add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavutil.so)

add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswresample.so)

add_library(avcodec
        SHARED
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavcodec.so)

add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavfilter.so)

add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswscale.so)

add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavformat.so)

add_library(avdevice
        SHARED
        IMPORTED)


set_target_properties(avdevice
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavdevice.so)


add_library(h264
        SHARED
        IMPORTED)


set_target_properties(h264
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libh264.so)


add_library(postproc
        SHARED
        IMPORTED)


set_target_properties(postproc
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libpostproc.so)


#生成可执行文件
# ADD_EXECUTABLE(hello hello.cpp)

include_directories(
        glm
)

# set 用于指定变量 set(key value) ，这里表示 CMAKE_CXX_STANDARD 变量为 17
# set(CMAKE_CXX_STANDARD 17)

# 指定生成动态库
add_library( # Sets the name of the library.
        native-lib             # Sets the library as a shared library.

        SHARED             # Provides a relative path to your source file(s).

        # Provides a relative path to your source file(s).
        # src/main/cpp/native-lib.cpp
        #-------------------------------------------------------------------
        #  ${SRC_LIST}  这种方式可以导入所有源文件，但是不能有 .h 文件在里面  |
        #-------------------------------------------------------------------
        ff-native-lib.cpp
        source/base_decoder.cpp
        source/v_decoder.cpp
        utils/timer.c
        source/audio_decoder.cpp
        source/test/base_decoder1.cpp
        source/test/i_decode_state_cb.cpp
        render/video/native_render.cpp
        media/player.cpp
        render/audio/opensl_render.cpp
        opengl/egl/egl_core.cpp
        opengl/egl/egl_surface.cpp
        opengl/drawer/drawer.cpp
        opengl/drawer/video_drawer.cpp
        opengl/drawer/proxy/def_drawer_proxy_impl.cpp
        render/video/opengl_render.cpp
        player/gl_player.cpp
        source/test/avio_test.c
        source/test/pthread_test.c
        source/test/pthread_test1.c
        )

find_library( # Sets the name of the path variable.
        log-lib              # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

target_link_libraries( # Specifies the target library.
        native-lib

        # 4. 连接 FFmpeg 相关的库
        avutil
        swresample
        avcodec
        avfilter
        swscale
        avformat
        avdevice
        h264
        postproc

        #音频播放
        #OpenSLES
        OpenSLES
        -lEGL
        -lGLESv2

        android
        #加入该依赖库
        #jnigraphics
        # included in the NDK.
        ${log-lib})
