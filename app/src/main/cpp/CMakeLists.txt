# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("ndk_practice")

##官方标准配置
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")


##ANDROID_PLATFORM_LEVEL=18
add_definitions("-DDYNAMIC_ES3")
set(OPENGL_LIB GLESv3)


include_directories(
        glm
)

add_library( # Sets the name of the library.
        ndk_practice

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp
        util/GLUtils.cpp
        render/MyGLRenderContext.cpp
        sample/TriangleSample.cpp
        sample/Square.cpp
        sample/TextureMapSample.cpp
        sample/Cube.cpp
        sample/Cone.cpp
        demo/native-lib1.c
        demo/native-parcel.cpp
        demo/native-arr-err.c
        demo/native-str.c
        demo/natvie-pointer.c
        demo/native-memory.c
        demo/native-struct.c
        demo/testcpp/TestClass.cpp
        demo/testcpp/TestClass1.cpp
        demo/testcpp/TestClass2.cpp
        demo/testcpp/TestClass3.cpp
        demo/testcpp/TestClass_static_friend.cpp
        demo/testcpp/TestClassOperator_override.cpp
        demo/testcpp/Test_str.cpp
        demo/testcpp/TestClass_algorithm.cpp
        demo/testc/JNIEnv_test.c
        demo/native-arr-copy.cpp
        demo/m_thread.c
        libjpg/jpeg-compress.cpp
        )


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        ndk_practice

        jnigraphics #JNI层,添加bitmap支持

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${OPENGL_LIB}
        android
        EGL
        GLESv3
        log
        m
        )