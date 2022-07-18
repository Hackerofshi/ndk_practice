//
// Created by shixin on 2022/7/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jni.h>

#ifdef ANDROID


#include <android/log.h>

#define LOG_TAG    "TEST"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif

//动态分配内存
//编程时，如果您预先知道数组的大小，那么定义数组时就比较容易。例如，一个存储人名的数组，它最多容纳 100 个字符，所以您可以定义数组，如下所示：

void testMalloc() {

    char name[100];
    char *description;

    strcpy(name, "zara Ali");
    //动态内存分配
    description = (char *) malloc(200 * sizeof(char));
    if (description == NULL) {
        fprintf(stderr, "Error - unable to allocate required memory\n");
    } else {
        strcpy(description, "Zara ali a DPS student in class 10th");
    }
    LOGI("Name = %s\n", name);
    LOGI("Description: %s\n", description);
    //上面的程序也可以使用 calloc() 来编写，只需要把 malloc 替换为 calloc 即可，如下所示：
    //
    //calloc(200, sizeof(char));
    //当动态分配内存时，您有完全控制权，可以传递任何大小的值。而那些预先定义了大小的数组，一旦定义则无法改变大小。
}

//重新调整内存的大小和释放内存
//当程序退出时，操作系统会自动释放所有分配给程序的内存，但是，建议您在不需要内存时，都应该调用函数 free() 来释放内存。
//
//或者，您可以通过调用函数 realloc() 来增加或减少已分配的内存块的大小。让我们使用 realloc() 和 free() 函数，再次查看上面的实例：


void testRealloc() {

    char name[100];
    char *description;

    strcpy(name, "zara Ali");

    //动态内存分配
    description = (char *) malloc(30 * sizeof(char));

    if (description == NULL) {
        fprintf(stderr, "Error - unable to allocate required memory\n");
    } else {
        strcpy(description, "Zara ali a DPS student.");
    }
    /* 假设您想要存储更大的描述信息 */
    description = (char *) realloc(description, 100 * sizeof(char));

    if (description == NULL) {
        fprintf(stderr, "Error - unable to allocate required memory\n");
    } else {
        strcat(description, "She is in class 10th");
    }

    LOGI("Name = %s\n", name);
    LOGI("Description: %s\n", description);


    /* 使用 free() 函数释放内存 */
    free(description);
}


JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_TestMemory_test1(JNIEnv *env, jclass clazz) {

}

