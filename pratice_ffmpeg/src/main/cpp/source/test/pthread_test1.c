//
// Created by 78303 on 2024/8/23.
//
#include "pthread_test.h"

//缓冲区大小
#define BUFFER_SIZE 10

//生产者和消费者数量
#define PRODUCER_COUNT 2
#define CONSUMER_COUNT 2


//缓冲区结构
typedef struct {
    int buffer[BUFFER_SIZE];// 数据存储
    int in;// 写入位置索引
    int out;// 读取位置索引
    int count; // 当前缓冲区中的数据数量
    pthread_mutex_t mutex; // 互斥锁
    pthread_cond_t not_full;// 条件变量：缓冲区未满
    pthread_cond_t not_empty;// 条件变量：缓冲区非空
} Buffer;

//初始化缓冲区
void buffer_init(Buffer *buf) {
    buf->in = 0;
    buf->out = 0;
    buf->count = 0;
    pthread_mutex_init(&(buf->mutex), NULL);
    pthread_cond_init(&(buf->not_full), NULL);
    pthread_cond_init(&(buf->not_empty), NULL);
}

// 销毁缓冲区
void buffer_destroy(Buffer *buf) {
    pthread_mutex_destroy(&(buf->mutex));
    pthread_cond_destroy(&(buf->not_empty));
    pthread_cond_destroy(&(buf->not_full));
}

// 向缓冲区添加数据
void buffer_put(Buffer *buf, int item) {
    pthread_mutex_lock(&(buf->mutex));
    //缓冲区满，等待
    while (buf->count == BUFFER_SIZE) {
        LOGI("Buffer is full. Producer is waiting.\n")
        pthread_cond_wait(&(buf->not_full), &(buf->mutex));
    }

    buf->buffer[buf->in] = item;
    buf->in = (buf->in + 1) % BUFFER_SIZE;
    buf->count++;
    LOGI("Produced item: %d (Buffer count: %d)\n", item, buf->count)

    //通知消费者缓冲区非空
    pthread_cond_signal(&(buf->not_empty));
    pthread_mutex_unlock(&(buf->mutex));

}


// 从缓冲区获取数据
int buffer_get(Buffer* buf) {
    pthread_mutex_lock(&(buf->mutex));

    // 缓冲区空，等待
    while (buf->count == 0) {
        printf("Buffer is empty. Consumer is waiting.\n");
        pthread_cond_wait(&(buf->not_empty), &(buf->mutex));
    }

    // 取出数据
    int item = buf->buffer[buf->out];
    buf->out = (buf->out + 1) % BUFFER_SIZE;
    buf->count--;

    printf("Consumed item: %d (Buffer count: %d)\n", item, buf->count);

    // 通知生产者缓冲区未满
    pthread_cond_signal(&(buf->not_full));
    pthread_mutex_unlock(&(buf->mutex));

    return item;
}


// 生产者线程函数
void* producer(void* arg) {
    Buffer* buf = (Buffer*)arg;
    int item;
    while (1) {
        item = rand() % 100; // 生产随机数据
        buffer_put(buf, item);
        sleep(rand() % 3); // 模拟生产耗时
    }
    return NULL;
}

// 消费者线程函数
void* consumer(void* arg) {
    Buffer* buf = (Buffer*)arg;
    int item;
    while (1) {
        item = buffer_get(buf);
        // 模拟数据处理
        printf("Processing item: %d\n", item);
        sleep(rand() % 3); // 模拟消费耗时
    }
    return NULL;
}


int main() {
    srand(time(NULL)); // 初始化随机数种子

    Buffer buf;
    buffer_init(&buf);

    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumers[CONSUMER_COUNT];

    // 创建生产者线程
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        if (pthread_create(&producers[i], NULL, producer, &buf) != 0) {
            fprintf(stderr, "Error creating producer thread\n");
            exit(EXIT_FAILURE);
        }
    }

    // 创建消费者线程
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        if (pthread_create(&consumers[i], NULL, consumer, &buf) != 0) {
            fprintf(stderr, "Error creating consumer thread\n");
            exit(EXIT_FAILURE);
        }
    }

    // 主线程等待（可根据需要调整）
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_join(consumers[i], NULL);
    }

    buffer_destroy(&buf);

    return 0;
}