//
//#include <inttypes.h>
//#include <stddef.h>
//#include <math.h>
//#include <stdio.h>
//#include <sys/mman.h>
//#include <syscall.h>
//#include <netinet/in.h>
//#include <stdlib.h>
//#include "graphics.h"
//#include <time.h>
//#include <unistd.h>
//
//extern void* tlsf_create_with_pool(void* mem, size_t bytes);
//extern void* g_heap;
//
//#define N 1000 /* 列表长度 */
//#define STACK_SIZE 65536 /* 增大栈大小 */
//#define NUM_THREADS 4
//
//int lists[NUM_THREADS][N];
//int g_draw_counter = 0;
//int draw_interval = 5;
//
///* 新增结构体和全局变量 */
//typedef struct {
//    int thread_id;
//    int column_x;
//    int column_width;
//    int screen_h;
//    int max_value;
//} ThreadArgs;
//
//volatile int g_threads_completed = 0;
//#define DRAW_INTERVAL 100
//
///* 函数声明 */
//void sort_thread(void* arg);
//void draw_array(ThreadArgs* args, int* arr);
//void bubble_sort(int arr[], int n, ThreadArgs* args);
//void insertion_sort(int arr[], int n, ThreadArgs* args);
//void selection_sort(int arr[], int n, ThreadArgs* args);
//void quick_sort(int arr[], int low, int high, ThreadArgs* args);
//int all_threads_done(void);
//int get_color(int thread_id, int is_background);
//void my_sleep(int milliseconds);
//
//void __main() {
//    size_t heap_size = 32 * 1024 * 1024;
//    void* heap_base = mmap(NULL, heap_size, PROT_READ | PROT_WRITE,
//        MAP_PRIVATE | MAP_ANON, -1, 0);
//    g_heap = tlsf_create_with_pool(heap_base, heap_size);
//}
//
///* 获取颜色函数 */
//int get_color(int thread_id, int is_background) {
//    if (is_background) {
//        switch (thread_id) {
//        case 0: return RGB(255, 200, 200);  /* 浅红色 */
//        case 1: return RGB(200, 255, 200);  /* 浅绿色 */
//        case 2: return RGB(200, 200, 255);  /* 浅蓝色 */
//        case 3: return RGB(255, 255, 200);  /* 浅黄色 */
//        default: return RGB(220, 220, 220); /* 浅灰色 */
//        }
//    }
//    else {
//        switch (thread_id) {
//        case 0: return RGB(255, 0, 0);   /* 红色 */
//        case 1: return RGB(0, 255, 0);   /* 绿色 */
//        case 2: return RGB(0, 0, 255);   /* 蓝色 */
//        case 3: return RGB(255, 255, 0); /* 黄色 */
//        default: return RGB(255, 255, 255); /* 白色 */
//        }
//    }
//}
//
//
///* 自定义睡眠函数 */
//void my_sleep(int milliseconds) {
//    struct timespec ts;
//    ts.tv_sec = milliseconds / 1000;
//    ts.tv_nsec = (milliseconds % 1000) * 1000000;
//    nanosleep(&ts, NULL);
//}
//
//
//void draw_array(ThreadArgs* args, int* arr) {
//    int y, x, i;
//    int color, bg_color;
//    int bar_width;
//
//    color = get_color(args->thread_id, 0);
//    bg_color = get_color(args->thread_id, 1);
//
//    /* 计算每个柱子的宽度 */
//    bar_width = args->column_width / N;
//    if (bar_width < 1) bar_width = 1;
//
//    /* 绘制当前数组状态 */
//    for (i = 0; i < N; i++) {
//        int bar_height = (arr[i] * args->screen_h) / args->max_value;
//        int x_start = args->column_x + (i * args->column_width) / N;
//
//        /* 用背景色清除这个柱子的区域 */
//        for (y = 0; y < args->screen_h; y++) {
//            for (x = x_start; x < x_start + bar_width && x < args->column_x + args->column_width; x++) {
//                setPixel(x, y, bg_color);
//            }
//        }
//
//        /* 绘制新的柱子 */
//        for (y = args->screen_h - bar_height; y < args->screen_h; y++) {
//            for (x = x_start; x < x_start + bar_width && x < args->column_x + args->column_width; x++) {
//                setPixel(x, y, color);
//            }
//        }
//    }
//
//    /* 每次绘制后添加短暂延迟 */
//    my_sleep(10);
//}
//
//void bubble_sort(int arr[], int n, ThreadArgs* args) {
//    int i, j, temp;
//    int swapped;
//    for (i = 0; i < n - 1; i++) {
//        swapped = 0;
//        for (j = 0; j < n - i - 1; j++) {
//            if (arr[j] > arr[j + 1]) {
//                /* 交换元素 */
//                temp = arr[j];
//                arr[j] = arr[j + 1];
//                arr[j + 1] = temp;
//                swapped = 1;
//
//                /* 有条件刷新 */
//                if (g_draw_counter++ % draw_interval == 0) {
//                    draw_array(args, arr);
//                }
//            }
//        }
//        /* 每轮结束后强制刷新 */
//        draw_array(args, arr);
//        if (!swapped) break;
//    }
//}
//
//void insertion_sort(int arr[], int n, ThreadArgs* args) {
//    int i, j, key;
//    for (i = 1; i < n; i++) {
//        key = arr[i];
//        j = i - 1;
//
//        while (j >= 0 && arr[j] > key) {
//            arr[j + 1] = arr[j];
//            j = j - 1;
//
//            /* 每移动5次刷新 */
//            if ((i + j) % 5 == 0) {
//                draw_array(args, arr);
//            }
//        }
//        arr[j + 1] = key;
//
//        /* 每个元素插入后刷新 */
//        draw_array(args, arr);
//    }
//}
//
//void selection_sort(int arr[], int n, ThreadArgs* args) {
//    int i, j, min_idx, temp;
//    for (i = 0; i < n - 1; i++) {
//        min_idx = i;
//        for (j = i + 1; j < n; j++) {
//            if (arr[j] < arr[min_idx])
//                min_idx = j;
//        }
//
//        /* 交换元素 */
//        temp = arr[min_idx];
//        arr[min_idx] = arr[i];
//        arr[i] = temp;
//
//        /* 每次选择后刷新 */
//        if (i % 10 == 0) {
//            draw_array(args, arr);
//        }
//    }
//    /* 最终刷新 */
//    draw_array(args, arr);
//}
//
//void quick_sort(int arr[], int low, int high, ThreadArgs* args) {
//    int j, temp, pi;
//    if (low < high) {
//        int pivot = arr[high];
//        int i = (low - 1);
//
//        for (j = low; j <= high - 1; j++) {
//            if (arr[j] < pivot) {
//                i++;
//                /* 交换元素 */
//                temp = arr[i];
//                arr[i] = arr[j];
//                arr[j] = temp;
//
//                /* 每5次交换刷新 */
//                if ((j - low) % 5 == 0) {
//                    draw_array(args, arr);
//                }
//            }
//        }
//        temp = arr[i + 1];
//        arr[i + 1] = arr[high];
//        arr[high] = temp;
//        pi = i + 1;
//
//        /* 分区后刷新 */
//        draw_array(args, arr);
//
//        quick_sort(arr, low, pi - 1, args);
//        quick_sort(arr, pi + 1, high, args);
//    }
//}
//
//void sort_thread(void* arg) {
//    ThreadArgs* args = (ThreadArgs*)arg;
//    int* arr = lists[args->thread_id];
//
//    /* 初始绘制 */
//    draw_array(args, arr);
//
//    switch (args->thread_id) {
//    case 0:
//        printf("Thread %d: Bubble Sort\n", args->thread_id);
//        bubble_sort(arr, N, args);
//        break;
//    case 1:
//        printf("Thread %d: Insertion Sort\n", args->thread_id);
//        insertion_sort(arr, N, args);
//        break;
//    case 2:
//        printf("Thread %d: Selection Sort\n", args->thread_id);
//        selection_sort(arr, N, args);
//        break;
//    case 3:
//        printf("Thread %d: Quick Sort\n", args->thread_id);
//        quick_sort(arr, 0, N - 1, args);
//        break;
//    }
//
//    /* 最终绘制 */
//    draw_array(args, arr);
//
//    /* 增加线程完成计数 */
//    __sync_fetch_and_add(&g_threads_completed, 1);
//
//    /* 线程结束后进入无限循环，防止返回到无效内存 */
//    while (1) {
//        task_yield();
//        my_sleep(100);
//    }
//}
//
//int all_threads_done() {
//    return g_threads_completed == NUM_THREADS;
//}
//
//void main(void* pv) {
//    int i, j;
//    int screen_w, screen_h, column_width;
//    ThreadArgs args[NUM_THREADS];
//    void* stacks[NUM_THREADS];
//
//    /* 初始化图形模式 */
//    init_graphic(0x143);
//
//    /* 获取屏幕参数 */
//    screen_w = g_graphic_dev.XResolution;
//    screen_h = g_graphic_dev.YResolution;
//    column_width = screen_w / NUM_THREADS;
//
//    /* 生成随机数据 */
//    srand(time(NULL));
//    for (i = 0; i < NUM_THREADS; i++) {
//        for (j = 0; j < N; j++) {
//            lists[i][j] = rand() % 10000;
//        }
//    }
//
//    /* 创建线程 */
//    for (i = 0; i < NUM_THREADS; i++) {
//        /* 设置线程参数 */
//        args[i].thread_id = i;
//        args[i].column_x = i * column_width;
//        args[i].column_width = column_width;
//        args[i].screen_h = screen_h;
//        args[i].max_value = 10000;
//
//        /* 分配栈空间 */
//        stacks[i] = malloc(STACK_SIZE);
//        if (!stacks[i]) {
//            printf("Failed to allocate stack for thread %d\n", i);
//            continue;
//        }
//
//        /* 计算栈顶指针（栈向下增长）*/
//        void* tos = (char*)stacks[i] + STACK_SIZE;
//
//        /* 创建线程 */
//        if (task_create(tos, sort_thread, &args[i]) < 0) {
//            printf("Failed to create thread %d\n", i);
//            free(stacks[i]);
//        }
//
//        /* 每创建一个线程后短暂延迟 */
//        my_sleep(100);
//    }
//
//    /* 主循环 */
//    while (!all_threads_done()) {
//        task_yield(); /* 主动让出CPU */
//        my_sleep(50); /* 添加短暂延迟 */
//    }
//
//    /* 结束前等待一段时间 */
//    my_sleep(2000);
//
//    /* 清理资源 */
//    for (i = 0; i < NUM_THREADS; i++) {
//        free(stacks[i]);
//    }
//
//    exit_graphic();
//    task_exit(0);
//}

/*
 * vim: filetype=c:fenc=utf-8:ts=4:et:sw=4:sts=4
 */
#include <inttypes.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <sys/mman.h>
#include <syscall.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "graphics.h"

extern void* tlsf_create_with_pool(void* mem, size_t bytes);
extern void* g_heap;

/**
 * GCC insists on __main
 *    http://gcc.gnu.org/onlinedocs/gccint/Collect2.html
 */
void __main()
{
    size_t heap_size = 32 * 1024 * 1024;
    void* heap_base = mmap(NULL, heap_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    g_heap = tlsf_create_with_pool(heap_base, heap_size);
}

/*  InfocEditBegin  */

/*  InfocE02 Begin  */

#include <string.h>
#define NUMS_LEN 300
#define NUMS_MAX 1000
#define METHOD_TOTAL 4
#define REFRESH_RATE 1000
#define MS 1000000
#define REFRESH_MS 1000 / REFRESH_RATE

void sleep_ms(int ms) {
    struct timespec req = {
        .tv_sec = ms / 1000,
        .tv_nsec = (ms % 1000) * MS
    };
    nanosleep(&req, &req);
}

void draw_E02_graph(int index, int* dirty_indices, int dirty_count, int* nums, int color_1[3], int color_2[3]) {
    int XRes = (int)g_graphic_dev.XResolution;
    int YRes = (int)g_graphic_dev.YResolution;
    int xWidth = XRes / METHOD_TOTAL;
    int yWidth = YRes / NUMS_LEN;
    int i, j, d;
    for (d = 0; d < dirty_count; d++) {
        i = dirty_indices[d];
        int color[3];
        for (j = 0; j < 3; j++) {
            color[j] = (color_1[j] * nums[i] + color_2[j] * (NUMS_MAX - nums[i])) / NUMS_MAX;
        }
        int xStart = xWidth * index;
        int xEnd = xWidth * index + xWidth * nums[i] / NUMS_MAX;
        int xOver = xWidth * (index + 1);
        int yStart = i * yWidth;
        int yEnd = (i + 1) * yWidth;
        for (j = yStart; j < yEnd - 1; j++) {
            line(xStart, j, xEnd - 1, j, RGB(color[0], color[1], color[2]));
            line(xEnd, j, xOver - 1, j, RGB(0, 0, 0));
        }
    }
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void quicksort(int* arr, int left, int right, int color_1[3], int color_2[3]) {
    if (left >= right) return;

    int pivot = arr[right];
    int i, j;
    i = left - 1;

    for (j = left; j < right; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
            int dirty[2] = { i, j };
            draw_E02_graph(0, dirty, 2, arr, color_1, color_2);
            sleep_ms(REFRESH_MS);
        }
    }

    swap(&arr[i + 1], &arr[right]);
    int dirty[2] = { i + 1, right };
    draw_E02_graph(0, dirty, 2, arr, color_1, color_2);
    sleep_ms(REFRESH_MS);

    quicksort(arr, left, i, color_1, color_2);
    quicksort(arr, i + 2, right, color_1, color_2);
}

void tsk_a(void* pv) {
    int* nums = (int*)pv;
    int color_1[3] = { 0xEE, 0x9C, 0xA7 };
    int color_2[3] = { 0xFF, 0xDD, 0xE1 };
    int* dirty;
    int i;
    dirty = (int*)malloc(NUMS_LEN * sizeof(int));
    for (i = 0; i < NUMS_LEN; i++) {
        dirty[i] = i;
    }
    draw_E02_graph(0, dirty, NUMS_LEN, nums, color_1, color_2);
    quicksort(nums, 0, NUMS_LEN - 1, color_1, color_2);
    draw_E02_graph(0, dirty, NUMS_LEN, nums, (int[]) { 0, 255, 0 }, (int[]) { 0, 100, 0 });
    free(dirty);
    task_exit(0);
}

void merge(int* arr, int l, int m, int r, int color_1[3], int color_2[3]) {
    int n1 = m - l + 1;
    int n2 = r - m;
    int* L = malloc(n1 * sizeof(int));
    int* R = malloc(n2 * sizeof(int));
    int i, j;
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
    i = 0, j = 0;
    int k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i++];
        }
        else {
            arr[k] = R[j++];
        }
        int dirty[1] = { k };
        draw_E02_graph(1, dirty, 1, arr, color_1, color_2);
        sleep_ms(REFRESH_MS);
        k++;
    }
    while (i < n1) {
        arr[k++] = L[i++];
        int dirty[1] = { k - 1 };
        draw_E02_graph(1, dirty, 1, arr, color_1, color_2);
        sleep_ms(REFRESH_MS);
    }
    while (j < n2) {
        arr[k++] = R[j++];
        int dirty[1] = { k - 1 };
        draw_E02_graph(1, dirty, 1, arr, color_1, color_2);
        sleep_ms(REFRESH_MS);
    }
    free(L);
    free(R);
}

void merge_sort(int* arr, int l, int r, int color_1[3], int color_2[3]) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort(arr, l, m, color_1, color_2);
        merge_sort(arr, m + 1, r, color_1, color_2);
        merge(arr, l, m, r, color_1, color_2);
    }
}

void tsk_b(void* pv) {
    int* nums = (int*)pv;
    int color_1[3] = { 0xFF, 0xE2, 0x59 };
    int color_2[3] = { 0xFF, 0xA7, 0x51 };
    int* dirty;
    int i;
    dirty = (int*)malloc(NUMS_LEN * sizeof(int));
    for (i = 0; i < NUMS_LEN; i++) {
        dirty[i] = i;
    }
    draw_E02_graph(1, dirty, NUMS_LEN, nums, color_1, color_2);
    merge_sort(nums, 0, NUMS_LEN - 1, color_1, color_2);
    draw_E02_graph(1, dirty, NUMS_LEN, nums, (int[]) { 0, 255, 0 }, (int[]) { 0, 100, 0 });
    free(dirty);
    task_exit(0);
}

void shell_sort(int* arr, int n, int color_1[3], int color_2[3]) {
    int gap = 1;
    while (gap < n / 3) {
        gap = gap * 3 + 1;
    }
    while (gap > 0) {
        int i;
        for (i = gap; i < n; i++) {
            int temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
                int dirty[1] = { j };
                draw_E02_graph(2, dirty, 1, arr, color_1, color_2);
                sleep_ms(REFRESH_MS);
            }
            arr[j] = temp;
            int dirty[1] = { j };
            draw_E02_graph(2, dirty, 1, arr, color_1, color_2);
            sleep_ms(REFRESH_MS);
        }
        gap = (gap - 1) / 3;
    }
}

void tsk_c(void* pv) {
    int* nums = (int*)pv;
    int color_1[3] = { 0xAC, 0xB6, 0xE5 };
    int color_2[3] = { 0x86, 0xFD, 0xE8 };
    int* dirty;
    int i;
    dirty = (int*)malloc(NUMS_LEN * sizeof(int));
    for (i = 0; i < NUMS_LEN; i++) {
        dirty[i] = i;
    }
    draw_E02_graph(2, dirty, NUMS_LEN, nums, color_1, color_2);
    shell_sort(nums, NUMS_LEN, color_1, color_2);
    draw_E02_graph(2, dirty, NUMS_LEN, nums, (int[]) { 0, 255, 0 }, (int[]) { 0, 100, 0 });
    free(dirty);
    task_exit(0);
}

void insertion_sort(int* arr, int n, int color_1[3], int color_2[3]) {
    int i;
    for (i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            int dirty[1] = { j + 1 };
            draw_E02_graph(3, dirty, 1, arr, color_1, color_2);
            sleep_ms(REFRESH_MS);
            j--;
        }
        arr[j + 1] = key;
        int dirty[1] = { j + 1 };
        draw_E02_graph(3, dirty, 1, arr, color_1, color_2);
        sleep_ms(REFRESH_MS);
    }
}

void tsk_d(void* pv) {
    int* nums = (int*)pv;
    int color_1[3] = { 0xF0, 0xC2, 0x7B };
    int color_2[3] = { 0x4B, 0x12, 0x48 };
    int* dirty;
    int i;
    dirty = (int*)malloc(NUMS_LEN * sizeof(int));
    for (i = 0; i < NUMS_LEN; i++) {
        dirty[i] = i;
    }
    draw_E02_graph(3, dirty, NUMS_LEN, nums, color_1, color_2);
    insertion_sort(nums, NUMS_LEN, color_1, color_2);
    draw_E02_graph(3, dirty, NUMS_LEN, nums, (int[]) { 0, 255, 0 }, (int[]) { 0, 100, 0 });
    free(dirty);
    task_exit(0);
}

/*   InfocE02 End   */

/*  InfocE03 Begin  */

#define BS_NUMS_LEN 100
#define BS_NUMS_MAX 1000
#define BS_REFRESH_MS 5

void tsk_O(void* pv) {
    while (1) {
        printf("O");
        sleep_ms(50);
    }
}

void tsk_S(void* pv) {
    while (1) {
        printf("S");
        sleep_ms(50);
    }
}

void draw_E03_maingraph(int index, int* dirty_indices, int dirty_count, int* nums, int color_1[3], int color_2[3]) {
    int XRes = (int)g_graphic_dev.XResolution;
    int YRes = (int)g_graphic_dev.YResolution;
    int xWidth = XRes / 2 - 5;
    int yWidth = (YRes - 40) / BS_NUMS_LEN;
    int i, j, d;
    for (d = 0; d < dirty_count; d++) {
        i = dirty_indices[d];
        int color[3];
        for (j = 0; j < 3; j++) {
            color[j] = (color_1[j] * nums[i] + color_2[j] * (BS_NUMS_MAX - nums[i])) / BS_NUMS_MAX;
        }
        int xStart = (index == 0 ? xWidth - xWidth * nums[i] / BS_NUMS_MAX : XRes - xWidth);
        int xEnd = (index == 0 ? xWidth : XRes - xWidth + xWidth * nums[i] / BS_NUMS_MAX);
        int yStart = i * yWidth;
        int yEnd = (i + 1) * yWidth;
        for (j = yStart; j < yEnd - 1; j++) {
            line(xStart, j, xEnd - 1, j, RGB(color[0], color[1], color[2]));
            if (index == 0) {
                line(0, j, xStart - 1, j, RGB(0, 0, 0));
            }
            else {
                line(xEnd, j, XRes - 1, j, RGB(0, 0, 0));
            }
        }
    }
}

void draw_E03_priority(int p1, int p2) {
    int XRes = (int)g_graphic_dev.XResolution;
    int YRes = (int)g_graphic_dev.YResolution;
    int yStart = YRes - 35, yEnd = YRes - 5;
    int xMid = XRes / 2, xPadding = 5, xWidth = 3, xGap = 1;
    int i, j;
    int maxPriority = 40;
    int color[3];
    int color1_1[3] = { 0xF0, 0xC2, 0x7B };
    int color1_2[3] = { 0x4B, 0x12, 0x48 };
    for (i = 0; i < maxPriority; i++) {
        for (j = 0; j < 3; j++) {
            color[j] = (color1_1[j] * i + color1_2[j] * (maxPriority - i)) / maxPriority;
        }
        for (j = xMid - xPadding - (xWidth + xGap) * i; j > xMid - xPadding - (xWidth + xGap) * (i + 1) + xGap; j--) {
            line(j, yStart, j, yEnd, i < maxPriority - p1 ? RGB(color[0], color[1], color[2]) : RGB(0, 0, 0));
        }
    }
    int color2_1[3] = { 0xAC, 0xB6, 0xE5 };
    int color2_2[3] = { 0x86, 0xFD, 0xE8 };
    for (i = 0; i < maxPriority; i++) {
        for (j = 0; j < 3; j++) {
            color[j] = (color2_1[j] * i + color2_2[j] * (maxPriority - i)) / maxPriority;
        }
        for (j = xMid + xPadding + (xWidth + xGap) * i; j < xMid + xPadding + (xWidth + xGap) * (i + 1) - xGap; j++) {
            line(j, yStart, j, yEnd, i < maxPriority - p2 ? RGB(color[0], color[1], color[2]) : RGB(0, 0, 0));
        }
    }
}

void draw_E03_info() {
    int XRes = (int)g_graphic_dev.XResolution;
    int YRes = (int)g_graphic_dev.YResolution;
    int info_left = 20, info_up = YRes - 80;
    char ans[] = "..... .   . .....  ...       \n  .   ..  . .     .   .      \n  .   . . . ..... .   .  ....\n  .   .  .. .     .   . .    \n..... .   . .      ...   ....";
    int i, line_info = 0, num_info = 0, size_pix = 5;
    for (i = 0; i < strlen(ans); i++) {
        num_info++;
        if (ans[i] == '\n') {
            line_info++;
            num_info = 0;
        }
        else if (ans[i] == '.') {
            int left = info_left + size_pix * num_info, right = left + size_pix;
            int up = info_up + size_pix * line_info, down = up + size_pix;
            for (; up < down; up++) {
                line(left, up, right, up, RGB(86, 149, 173));
            }
        }
    }
}

void bubble_sort(int index, int* arr, int n, int color_1[3], int color_2[3]) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
                int dirty[2] = { j, j + 1 };
                draw_E03_maingraph(index, dirty, 2, arr, color_1, color_2);
            }
            sleep_ms(BS_REFRESH_MS);
        }
    }
}

void tsk_bs1(void* pv) {
    int* nums = (int*)pv;
    int color_1[3] = { 0xF0, 0xC2, 0x7B };
    int color_2[3] = { 0x4B, 0x12, 0x48 };
    int* dirty;
    int i;
    dirty = (int*)malloc(BS_NUMS_LEN * sizeof(int));
    for (i = 0; i < BS_NUMS_LEN; i++) {
        dirty[i] = i;
    }
    draw_E03_maingraph(0, dirty, BS_NUMS_LEN, nums, color_1, color_2);
    bubble_sort(0, nums, BS_NUMS_LEN, color_1, color_2);
    draw_E03_maingraph(0, dirty, BS_NUMS_LEN, nums, (int[]) { 0, 255, 0 }, (int[]) { 0, 100, 0 });
    free(dirty);
    task_exit(0);
}

void tsk_bs2(void* pv) {
    int* nums = (int*)pv;
    int color_1[3] = { 0xAC, 0xB6, 0xE5 };
    int color_2[3] = { 0x86, 0xFD, 0xE8 };
    int* dirty;
    int i;
    dirty = (int*)malloc(BS_NUMS_LEN * sizeof(int));
    for (i = 0; i < BS_NUMS_LEN; i++) {
        dirty[i] = i;
    }
    draw_E03_maingraph(1, dirty, BS_NUMS_LEN, nums, color_1, color_2);
    bubble_sort(1, nums, BS_NUMS_LEN, color_1, color_2);
    draw_E03_maingraph(1, dirty, BS_NUMS_LEN, nums, (int[]) { 0, 255, 0 }, (int[]) { 0, 100, 0 });
    free(dirty);
    task_exit(0);
}

struct ctrl {
    int tid_A;
    int tid_B;
    int Mod;
};

void tsk_control(void* pv) {
#define DEBUG 0
#define TEST  1

    struct ctrl* c = (struct ctrl*)pv;
    if (c->Mod == TEST) {
        draw_E03_priority(getpriority(c->tid_A), getpriority(c->tid_B));
    }
    while (1) {
        int k = getchar();
        switch (k) {
        case 0x4800: // up
        {
            setpriority(c->tid_A, getpriority(c->tid_A) - 1);
            if (c->Mod == DEBUG) {
                printf("\ntask_A priority up\nA:%d B:%d\n", getpriority(c->tid_A), getpriority(c->tid_B));
            }
            else if (c->Mod == TEST) {
                draw_E03_priority(getpriority(c->tid_A), getpriority(c->tid_B));
            }
        }
        break;
        case 0x5000: // down
        {
            setpriority(c->tid_A, getpriority(c->tid_A) + 1);
            if (c->Mod == DEBUG) {
                printf("\ntask_A priority down\nA:%d B:%d\n", getpriority(c->tid_A), getpriority(c->tid_B));
            }
            else if (c->Mod == TEST) {
                draw_E03_priority(getpriority(c->tid_A), getpriority(c->tid_B));
            }
        }
        break;
        case 0x4d00: // right
        {
            setpriority(c->tid_B, getpriority(c->tid_B) - 1);
            if (c->Mod == DEBUG) {
                printf("\ntask_B priority up\nA:%d B:%d\n", getpriority(c->tid_A), getpriority(c->tid_B));
            }
            else if (c->Mod == TEST) {
                draw_E03_priority(getpriority(c->tid_A), getpriority(c->tid_B));
            }
        }
        break;
        case 0x4b00: // left
        {
            setpriority(c->tid_B, getpriority(c->tid_B) + 1);
            if (c->Mod == DEBUG) {
                printf("\ntask_B priority down\nA:%d B:%d\n", getpriority(c->tid_A), getpriority(c->tid_B));
            }
            else if (c->Mod == TEST) {
                draw_E03_priority(getpriority(c->tid_A), getpriority(c->tid_B));
            }
        }
        break;
        }
    }
}

/*   InfocE03 End   */

/*   InfocEditEnd   */

/**
 * 第一个运行在用户模式的线程所执行的函数
 */
void main(int* pv)
{
    printf("task #%d: I'm the first user task(pv=0x%08x)!\r\n",
        task_getid(), pv);

    /*  InfocEditBegin  */

    int Experiment = 3;

    /*  InfocE01 Begin  */

    if (Experiment == 1)
    {
        time_t t1, t2;
        t1 = time(&t2);
        printf("%d\n%d\n%s\n", t1, t2, t1 == t2 ? "Same" : "Diff");
        sleep(5);
        t1 = time(&t2);
        printf("%d\n%d\n%s\n", t1, t2, t1 == t2 ? "Same" : "Diff");
    }

    /*   InfocE01 End   */

    /*  InfocE02 Begin  */

    if (Experiment == 2)
    {
        init_graphic(0x143); // 800x600 px 32 byte
        int i;
        srand(time(NULL));
        int nums[NUMS_LEN];
        for (i = 0; i < NUMS_LEN; i++) {
            nums[i] = rand() % NUMS_MAX;
        }
        unsigned char* stack_a, * stack_b, * stack_c, * stack_d;
        unsigned int stack_size = 1024 * 1024;
        stack_a = (unsigned char*)malloc(stack_size);
        stack_b = (unsigned char*)malloc(stack_size);
        stack_c = (unsigned char*)malloc(stack_size);
        stack_d = (unsigned char*)malloc(stack_size);
        int* nums_a = malloc(NUMS_LEN * sizeof(int));
        int* nums_b = malloc(NUMS_LEN * sizeof(int));
        int* nums_c = malloc(NUMS_LEN * sizeof(int));
        int* nums_d = malloc(NUMS_LEN * sizeof(int));
        memcpy(nums_a, nums, NUMS_LEN * sizeof(int));
        memcpy(nums_b, nums, NUMS_LEN * sizeof(int));
        memcpy(nums_c, nums, NUMS_LEN * sizeof(int));
        memcpy(nums_d, nums, NUMS_LEN * sizeof(int));
        task_create(stack_a + stack_size, &tsk_a, nums_a);
        task_create(stack_b + stack_size, &tsk_b, nums_b);
        task_create(stack_c + stack_size, &tsk_c, nums_c);
        task_create(stack_d + stack_size, &tsk_d, nums_d);
    }

    /*   InfocE02 End   */

    /*  InfocE03 Begin  */

    if (Experiment == 3)
    {
#define DEBUG 0
#define TEST  1
        int Mod = TEST;

        if (Mod == DEBUG) {
            unsigned char* stack_O, * stack_S, * stack_control;
            unsigned int stack_size = 1024 * 1024;
            stack_O = (unsigned char*)malloc(stack_size);
            stack_S = (unsigned char*)malloc(stack_size);
            stack_control = (unsigned char*)malloc(stack_size);
            int tid_O = task_create(stack_O + stack_size, &tsk_O, NULL);
            int tid_S = task_create(stack_S + stack_size, &tsk_S, NULL);
            struct ctrl* c = (struct ctrl*)malloc(sizeof(struct ctrl));
            c->tid_A = tid_O;
            c->tid_B = tid_S;
            c->Mod = Mod;
            task_create(stack_control + stack_size, &tsk_control, c);
        }
        else if (Mod == TEST) {
            init_graphic(0x143); // 800x600 px 32 byte
            draw_E03_info();
            int i;
            srand(time(NULL));
            int nums[BS_NUMS_LEN];
            for (i = 0; i < BS_NUMS_LEN; i++) {
                nums[i] = rand() % BS_NUMS_MAX;
            }
            unsigned char* stack_bs1, * stack_bs2, * stack_control;
            unsigned int stack_size = 1024 * 1024;
            stack_bs1 = (unsigned char*)malloc(stack_size);
            stack_bs2 = (unsigned char*)malloc(stack_size);
            stack_control = (unsigned char*)malloc(stack_size);
            int* nums_bs1 = malloc(BS_NUMS_LEN * sizeof(int));
            int* nums_bs2 = malloc(BS_NUMS_LEN * sizeof(int));
            memcpy(nums_bs1, nums, BS_NUMS_LEN * sizeof(int));
            memcpy(nums_bs2, nums, BS_NUMS_LEN * sizeof(int));
            int tid_bs1 = task_create(stack_bs1 + stack_size, &tsk_bs1, nums_bs1);
            int tid_bs2 = task_create(stack_bs2 + stack_size, &tsk_bs2, nums_bs2);
            struct ctrl* c = (struct ctrl*)malloc(sizeof(struct ctrl));
            c->tid_A = tid_bs1;
            c->tid_B = tid_bs2;
            c->Mod = Mod;
            task_create(stack_control + stack_size, &tsk_control, c);
        }
    }

    /*   InfocE03 End   */

    /*   InfocEditEnd   */

    while (1)
        ;
    task_exit(0);
}


