
#include <inttypes.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <sys/mman.h>
#include <syscall.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "graphics.h"
#include <time.h>
#include <unistd.h>

extern void* tlsf_create_with_pool(void* mem, size_t bytes);
extern void* g_heap;

#define N 1000 /* 列表长度 */
#define STACK_SIZE 65536 /* 增大栈大小 */
#define NUM_THREADS 4

int lists[NUM_THREADS][N];
int g_draw_counter = 0;
int draw_interval = 5;

/* 新增结构体和全局变量 */
typedef struct {
    int thread_id;
    int column_x;
    int column_width;
    int screen_h;
    int max_value;
} ThreadArgs;

volatile int g_threads_completed = 0;
#define DRAW_INTERVAL 100

/* 函数声明 */
void sort_thread(void* arg);
void draw_array(ThreadArgs* args, int* arr);
void bubble_sort(int arr[], int n, ThreadArgs* args);
void insertion_sort(int arr[], int n, ThreadArgs* args);
void selection_sort(int arr[], int n, ThreadArgs* args);
void quick_sort(int arr[], int low, int high, ThreadArgs* args);
int all_threads_done(void);
int get_color(int thread_id, int is_background);
void my_sleep(int milliseconds);

void __main() {
    size_t heap_size = 32 * 1024 * 1024;
    void* heap_base = mmap(NULL, heap_size, PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANON, -1, 0);
    g_heap = tlsf_create_with_pool(heap_base, heap_size);
}

/* 获取颜色函数 */
int get_color(int thread_id, int is_background) {
    if (is_background) {
        switch (thread_id) {
        case 0: return RGB(255, 200, 200);  /* 浅红色 */
        case 1: return RGB(200, 255, 200);  /* 浅绿色 */
        case 2: return RGB(200, 200, 255);  /* 浅蓝色 */
        case 3: return RGB(255, 255, 200);  /* 浅黄色 */
        default: return RGB(220, 220, 220); /* 浅灰色 */
        }
    }
    else {
        switch (thread_id) {
        case 0: return RGB(255, 0, 0);   /* 红色 */
        case 1: return RGB(0, 255, 0);   /* 绿色 */
        case 2: return RGB(0, 0, 255);   /* 蓝色 */
        case 3: return RGB(255, 255, 0); /* 黄色 */
        default: return RGB(255, 255, 255); /* 白色 */
        }
    }
}


/* 自定义睡眠函数 */
void my_sleep(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}


void draw_array(ThreadArgs* args, int* arr) {
    int y, x, i;
    int color, bg_color;
    int bar_width;

    color = get_color(args->thread_id, 0);
    bg_color = get_color(args->thread_id, 1);

    /* 计算每个柱子的宽度 */
    bar_width = args->column_width / N;
    if (bar_width < 1) bar_width = 1;

    /* 绘制当前数组状态 */
    for (i = 0; i < N; i++) {
        int bar_height = (arr[i] * args->screen_h) / args->max_value;
        int x_start = args->column_x + (i * args->column_width) / N;

        /* 用背景色清除这个柱子的区域 */
        for (y = 0; y < args->screen_h; y++) {
            for (x = x_start; x < x_start + bar_width && x < args->column_x + args->column_width; x++) {
                setPixel(x, y, bg_color);
            }
        }

        /* 绘制新的柱子 */
        for (y = args->screen_h - bar_height; y < args->screen_h; y++) {
            for (x = x_start; x < x_start + bar_width && x < args->column_x + args->column_width; x++) {
                setPixel(x, y, color);
            }
        }
    }

    /* 每次绘制后添加短暂延迟 */
    my_sleep(10);
}

void bubble_sort(int arr[], int n, ThreadArgs* args) {
    int i, j, temp;
    int swapped;
    for (i = 0; i < n - 1; i++) {
        swapped = 0;
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                /* 交换元素 */
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;

                /* 有条件刷新 */
                if (g_draw_counter++ % draw_interval == 0) {
                    draw_array(args, arr);
                }
            }
        }
        /* 每轮结束后强制刷新 */
        draw_array(args, arr);
        if (!swapped) break;
    }
}

void insertion_sort(int arr[], int n, ThreadArgs* args) {
    int i, j, key;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;

            /* 每移动5次刷新 */
            if ((i + j) % 5 == 0) {
                draw_array(args, arr);
            }
        }
        arr[j + 1] = key;

        /* 每个元素插入后刷新 */
        draw_array(args, arr);
    }
}

void selection_sort(int arr[], int n, ThreadArgs* args) {
    int i, j, min_idx, temp;
    for (i = 0; i < n - 1; i++) {
        min_idx = i;
        for (j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx])
                min_idx = j;
        }

        /* 交换元素 */
        temp = arr[min_idx];
        arr[min_idx] = arr[i];
        arr[i] = temp;

        /* 每次选择后刷新 */
        if (i % 10 == 0) {
            draw_array(args, arr);
        }
    }
    /* 最终刷新 */
    draw_array(args, arr);
}

void quick_sort(int arr[], int low, int high, ThreadArgs* args) {
    int j, temp, pi;
    if (low < high) {
        int pivot = arr[high];
        int i = (low - 1);

        for (j = low; j <= high - 1; j++) {
            if (arr[j] < pivot) {
                i++;
                /* 交换元素 */
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;

                /* 每5次交换刷新 */
                if ((j - low) % 5 == 0) {
                    draw_array(args, arr);
                }
            }
        }
        temp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = temp;
        pi = i + 1;

        /* 分区后刷新 */
        draw_array(args, arr);

        quick_sort(arr, low, pi - 1, args);
        quick_sort(arr, pi + 1, high, args);
    }
}

void sort_thread(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int* arr = lists[args->thread_id];

    /* 初始绘制 */
    draw_array(args, arr);

    switch (args->thread_id) {
    case 0:
        printf("Thread %d: Bubble Sort\n", args->thread_id);
        bubble_sort(arr, N, args);
        break;
    case 1:
        printf("Thread %d: Insertion Sort\n", args->thread_id);
        insertion_sort(arr, N, args);
        break;
    case 2:
        printf("Thread %d: Selection Sort\n", args->thread_id);
        selection_sort(arr, N, args);
        break;
    case 3:
        printf("Thread %d: Quick Sort\n", args->thread_id);
        quick_sort(arr, 0, N - 1, args);
        break;
    }

    /* 最终绘制 */
    draw_array(args, arr);

    /* 增加线程完成计数 */
    __sync_fetch_and_add(&g_threads_completed, 1);

    /* 线程结束后进入无限循环，防止返回到无效内存 */
    while (1) {
        task_yield();
        my_sleep(100);
    }
}

int all_threads_done() {
    return g_threads_completed == NUM_THREADS;
}

void main(void* pv) {
    printf("task #%d: I'm the first user task(pv = 0x%08x)!\r\n",
        task_getid(), pv);
    time_t t1, t2;

    //第一次调用time
    t1 = time(&t2);
    printf("%ld\n%ld\n%s\n", t1, t2, t1 == t2 ? "Same" : "Diff");
    sleep(5);
    //第二次调用time
    t1 = time(&t2);
    printf("%ld\n%ld\n%s\n", t1, t2, t1 == t2 ? "Same" : "Diff");
    return 0;

    /* 退出程序 */
    task_exit(0);
}
