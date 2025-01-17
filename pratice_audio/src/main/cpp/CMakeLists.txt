
# Sets the minimum version of CMake required to build the native library.
# 指定cmake最低编译版本
cmake_minimum_required(VERSION 3.10.2)

# 需要引入我们头文件,以这个配置的目录为基准，可以是多个
include_directories(${PROJECT_SOURCE_DIR}/include)
# include_directories(src/main/jniLibs/other)

#添加共享库搜索路径
link_directories(./libs/${ANDROID_ABI})

# 日志输出在.cxx/cmake/debug//armeabi-v7a/build_output.txt
#message("PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
#message("ANDROID_ABI=${ANDROID_ABI}")
#message("CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
#message("CMAKE_ANDROID_ARCH_ABI=${CMAKE_ANDROID_ARCH_ABI}")
#message("CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")


# 指定源文件目录
AUX_SOURCE_DIRECTORY(${CMAKE_SOURCE_DIR}/source SRC_LIST)


#生成可执行文件
# ADD_EXECUTABLE(hello hello.cpp)


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
        source/native-lib.cpp
        source/OpenSLAudioPlay.cpp
        source/AudioPlayer.cpp
        source/OpenSLRecord.cpp
        )

find_library( # Sets the name of the path variable.
        log-lib              # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

target_link_libraries( # Specifies the target library.
        native-lib
        #音频播放
        OpenSLES
        android
        #加入该依赖库
        jnigraphics
        # included in the NDK.
        ${log-lib})
