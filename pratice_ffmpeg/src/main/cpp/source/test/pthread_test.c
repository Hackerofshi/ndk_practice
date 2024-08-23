//
// Created by 78303 on 2024/8/23.
//

#include "pthread_test.h"


#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int count = 0;
int in = 0;
int out = 0;

// 互斥锁和条件变量
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;


// 生产者线程函数
void *producer(void *arg) {
    int item;
    while (1) {
        item = rand() % 100;  // 生成一个随机数作为数据项

        pthread_mutex_lock(&mutex);

        // 如果缓冲区满了，则等待
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&not_full, &mutex);
        }

        // 将数据放入缓冲区
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;

        LOGI("Produced: %d\n", item);

        // 通知消费者缓冲区非空
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);

        sleep(1);  // 模拟生产时间
    }
}

// 消费者线程函数
void *consumer(void *arg) {
    int item;
    while (1) {
        pthread_mutex_lock(&mutex);

        // 如果缓冲区为空，则等待
        while (count == 0) {
            pthread_cond_wait(&not_empty, &mutex);
        }

        // 从缓冲区取出数据
        item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;

        LOGI("Consumed: %d\n", item);

        // 通知生产者缓冲区非满
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);

        sleep(2);  // 模拟消费时间
    }
}



int test_thread() {
    pthread_t producer_thread, consumer_thread;

    // 创建生产者和消费者线程
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // 等待线程结束
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    return 0;
}