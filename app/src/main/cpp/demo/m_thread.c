//
// Created by shixin on 2022/10/14.
//
/*
  生产者-消费者 基本模型
  一个生产者一个消费者一个buffer
  需要:
  1. 一个buffer
  2. buffer的锁
  3. 通知机制,producer写好之后通知consumer
  4. flag 生产者和消费者之间的协议
  5. 一个生产者线程
  6. 一个消费者线程
 */

#include <pthread.h>
#include <stdio.h>
#include <jni.h>

//buffer数据类型 可扩展
typedef struct {
    int num;
} BUFFER;

#define MAX_NUM 1000

//buffer
BUFFER buf;
//lock
pthread_mutex_t pc_mutex;
//通知
pthread_cond_t pc_condp, pc_condc;

/*
  消费者
 */

void *consumer(void *null) {
    int i;
    for (i = 1; i < MAX_NUM; ++i) {
        pthread_mutex_lock(&pc_mutex);

        //等待条件变量
        while (buf.num == 0) {
            pthread_cond_wait(&pc_condc, &pc_mutex);
        }
        //条件达到
        printf("consumer consumes %d \n", buf.num);
        buf.num = 0;
        //通知consumer
        pthread_cond_signal(&pc_condp);
        pthread_mutex_unlock(&pc_mutex);
    }

    pthread_exit(NULL);
}

/*
  生产者
 */

void *producer(void *null) {
    int i;
    for (i = 1; i < MAX_NUM; ++i) {
        pthread_mutex_lock(&pc_mutex);

        //等待条件变量
        while (buf.num != 0) {
            pthread_cond_wait(&pc_condp, &pc_mutex);
        }
        //生产
        buf.num = i;
        printf("producer produces %d \n", buf.num);
        //通知
        pthread_cond_signal(&pc_condc);
        pthread_mutex_unlock(&pc_mutex);
    }
    pthread_exit(NULL);

}


JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_util_NDKUtils_testThread(JNIEnv *env, jobject thiz) {
    pthread_t thread[2];
    pthread_attr_t attr;

    buf.num = 0;

    //锁和条件变量
    pthread_mutex_init(&pc_mutex, NULL);
    pthread_cond_init(&pc_condp, NULL);
    pthread_cond_init(&pc_condc, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //producer
    pthread_create(&thread[0], &attr, producer, NULL);
    //consumer
    pthread_create(&thread[1], &attr, consumer, NULL);

    //连接线程
    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);

    //清理资源
    pthread_mutex_destroy(&pc_mutex);
    pthread_cond_destroy(&pc_condc);
    pthread_cond_destroy(&pc_condp);
    pthread_attr_destroy(&attr);

    pthread_exit(NULL);
}