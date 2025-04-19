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

/* 颜色定义 */
#define LIGHT_PURPLE RGB(230, 204, 255)  // 淡紫色
#define LIGHT_RED    RGB(255, 153, 153)  // 淡红色
#define WHITE        RGB(255, 255, 255)
#define DEEP_BLUE    RGB(0, 0, 128)      // 深蓝色

extern void* tlsf_create_with_pool(void* mem, size_t bytes);
extern void* g_heap;
int producerThreadId, consumerThreadId;

/* 可视化数组结构体 */
typedef struct {
    double* normalizedValues;  // 归一化后的数值数组
    int size;                  // 数组大小
    int startX, startY;        // 起始坐标
    int endX, endY;            // 结束坐标
    COLORREF baseColor;        // 基础颜色
    double barHeight;          // 柱状图高度
    int barWidth;              // 柱状图宽度
} VisualArray;

/* 函数声明 */
void InitializeVisualArray(VisualArray* array, int* sourceData, int dataSize,
    int startX, int startY, int endX, int endY, COLORREF color);
void DisplayVisualArray(VisualArray* array);
void HighlightPosition(VisualArray* array, int index, COLORREF color);
void HidePosition(VisualArray* array, int index);
void SwapElements(VisualArray* array, int firstIdx, int secondIdx);
int CompareElements(VisualArray* array, int firstIdx, int secondIdx);

void SleepMilliseconds(int ms);
void DrawRectangleArea(int x1, int y1, int x2, int y2, COLORREF color);
int CreateTask(void* taskFunc, unsigned int stackSize);
void GenerateRandomArray(int* array, int size);
void ProducerTask();
void ConsumerTask();
void DrawProgressBar(int progress, int total);

/* 全局变量 */
struct timespec timerSpec = { 0, 0 };
const int ARRAY_SIZE = 80;
int* dataArray;

/* 内存初始化 */
void __main()
{
    size_t heapSize = 32 * 1024 * 1024;
    void* heapBase = mmap(NULL, heapSize, PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANON, -1, 0);
    g_heap = tlsf_create_with_pool(heapBase, heapSize);
}

/* 可视化数组初始化 */
void InitializeVisualArray(VisualArray* array, int* sourceData, int dataSize,
    int startX, int startY, int endX, int endY, COLORREF color)
{
    int i;
    int maxValue = 0;
    array->barHeight = (double)(endY - startY) / dataSize;
    array->barWidth = endX - startX;
    array->normalizedValues = (double*)malloc(dataSize * sizeof(double));

    for (i = 0; i < dataSize; i++)
        maxValue = sourceData[i] > maxValue ? sourceData[i] : maxValue;

    for (i = 0; i < dataSize; i++)
        array->normalizedValues[i] = (double)sourceData[i] / maxValue;

    array->size = dataSize;
    array->startX = startX;
    array->startY = startY;
    array->endX = endX;
    array->endY = endY;
    array->baseColor = color;

    DrawRectangleArea(startX, startY, endX, endY, WHITE);
    for (i = 0; i < dataSize; i++) {
        SleepMilliseconds(10);
        DisplayVisualArray(array);
    }
}

/* 图形绘制相关函数 */
void HighlightPosition(VisualArray* array, int index, COLORREF color) {
    int top = array->barHeight * index + array->startY;
    int bottom = array->barHeight * (index + 1) + array->startY;
    int width = array->barWidth * array->normalizedValues[index];
    DrawRectangleArea(array->startX, top, array->startX + width, bottom - 2, color);
}

void HidePosition(VisualArray* array, int index) {
    int top = array->barHeight * index + array->startY;
    int bottom = array->barHeight * (index + 1) + array->startY;
    DrawRectangleArea(array->startX, top, array->endX, bottom - 2, WHITE);
}

void DisplayVisualArray(VisualArray* array) {
    int i;
    for (i = 0; i < array->size; i++) {
        int top = array->barHeight * i + array->startY;
        int bottom = array->barHeight * (i + 1) + array->startY;
        double ratio = array->normalizedValues[i];
        int width = array->barWidth * ratio;

        int r = getRValue(array->baseColor) * ratio + (1 - ratio) * (255 - getRValue(array->baseColor));
        int g = getGValue(array->baseColor) * ratio + (1 - ratio) * (255 - getGValue(array->baseColor));
        int b = getBValue(array->baseColor) * ratio + (1 - ratio) * (255 - getBValue(array->baseColor));

        DrawRectangleArea(array->startX, top, array->startX + width, bottom - 2, RGB(r, g, b));
    }
}

/* 数组操作函数 */
void SwapElements(VisualArray* array, int firstIdx, int secondIdx) {
    HidePosition(array, firstIdx);
    HidePosition(array, secondIdx);

    double temp = array->normalizedValues[firstIdx];
    array->normalizedValues[firstIdx] = array->normalizedValues[secondIdx];
    array->normalizedValues[secondIdx] = temp;

    DisplayVisualArray(array);
    HighlightPosition(array, secondIdx, LIGHT_RED);
    SleepMilliseconds(2);
    DisplayVisualArray(array);
}

int CompareElements(VisualArray* array, int firstIdx, int secondIdx) {
    HighlightPosition(array, firstIdx, LIGHT_PURPLE);
    SleepMilliseconds(1);
    DisplayVisualArray(array);
    return array->normalizedValues[firstIdx] < array->normalizedValues[secondIdx];
}

/* 系统相关函数 */
void SleepMilliseconds(int ms) {
    timerSpec.tv_nsec = 1000000L * ms;
    nanosleep(&timerSpec, &timerSpec);
}

void DrawRectangleArea(int x1, int y1, int x2, int y2, COLORREF color) {
    int i, j;
    for (i = x1; i <= x2; i++) {
        for (j = y1; j < y2; j++) {
            setPixel(i, j, color);
        }
    }
}

int CreateTask(void* taskFunc, unsigned int stackSize) {
    unsigned char* stack = (unsigned char*)malloc(stackSize);
    return task_create(stack + stackSize, taskFunc, (void*)0);
}

void GenerateRandomArray(int* array, int size) {
    srand(time(NULL));
    int i;
    for (i = 0; i < size; i++)
        array[i] = rand() % size;
}

/* 排序算法实现 */
int Partition(VisualArray* array, int low, int high) {
    int pivotIndex = high;
    int i = low - 1;
    int j;

    for (j = low; j < high; j++) {
        if (CompareElements(array, j, pivotIndex)) {
            i++;
            SwapElements(array, i, j);
        }
    }
    SwapElements(array, i + 1, pivotIndex);
    return i + 1;
}

void QuickSort(VisualArray* array, int low, int high) {
    if (low < high && low >= 0 && high < array->size) {
        int pivot = Partition(array, low, high);
        QuickSort(array, low, pivot - 1);
        QuickSort(array, pivot + 1, high);
    }
}

void BubbleSort(VisualArray* array) {
    int i, j;
    for (i = 0; i < array->size - 1; i++) {
        for (j = 0; j < array->size - i - 1; j++) {
            if (!CompareElements(array, j, j + 1)) {
                SwapElements(array, j, j + 1);
            }
        }
    }
}

/* 生产者-消费者实现 */
#define BUFFER_SIZE 6
int emptySemaphore;
int fullSemaphore;
int* mutexSemaphores;
VisualArray* visualBuffers;

void ProducerTask() {
    int currentArea = 0;
    double areaWidth = (double)800 / BUFFER_SIZE;

    while (1) {
        sem_wait(emptySemaphore);
        sem_wait(mutexSemaphores[currentArea]);

        GenerateRandomArray(dataArray, ARRAY_SIZE);
        InitializeVisualArray(&visualBuffers[currentArea], dataArray, ARRAY_SIZE,
            currentArea * areaWidth, 0,
            (currentArea + 1) * areaWidth - 2, 500, LIGHT_PURPLE);

        sem_signal(mutexSemaphores[currentArea]);
        sem_signal(fullSemaphore);
        currentArea = (currentArea + 1) % BUFFER_SIZE;
    }
}

void ConsumerTask() {
    int currentArea = 0;

    while (1) {
        sem_wait(fullSemaphore);
        sem_wait(mutexSemaphores[currentArea]);

        QuickSort(&visualBuffers[currentArea], 0, ARRAY_SIZE - 1);

        sem_signal(mutexSemaphores[currentArea]);
        sem_signal(emptySemaphore);
        currentArea = (currentArea + 1) % BUFFER_SIZE;
    }
}

/* 优先级显示 */
void ShowPriority() {
    int producerPriority = getpriority(producerThreadId);
    int consumerPriority = getpriority(consumerThreadId);

    DrawRectangleArea(0, 500, 800, 550, WHITE);
    DrawRectangleArea(0, 500, (int)(800 * ((double)producerPriority / 40)), 525, LIGHT_PURPLE);
    DrawRectangleArea(0, 525, (int)(800 * ((double)consumerPriority / 40)), 550, LIGHT_RED);
}

/* 进度条绘制 */
void DrawProgressBar(int progress, int total) {
    int barWidth = 800;
    int barHeight = 20;
    int startY = 580;
    int filledWidth = (int)((double)progress / total * barWidth);

    DrawRectangleArea(0, startY, barWidth, startY + barHeight, WHITE);
    DrawRectangleArea(0, startY, filledWidth, startY + barHeight, LIGHT_PURPLE);
}



/* 控制任务 */
void ControlTask() {
    int key = getchar();
    int priority;

    while (1) {
        switch (key) {
        case 0x4800:  // 上箭头
            priority = getpriority(producerThreadId) - 1;
            if (priority > 5) {
                setpriority(producerThreadId, priority);
                setpriority(consumerThreadId, 40 - priority);
            }
            break;

        case 0x5000:  // 下箭头
            priority = getpriority(producerThreadId) + 1;
            if (priority < 35) {
                setpriority(producerThreadId, priority);
                setpriority(consumerThreadId, 40 - priority);
            }
            break;
        }
        ShowPriority();
        SleepMilliseconds(100);
        key = getchar();
    }
}

/* 主函数 */
void main(void* pv)
{
    printf("Task #%d: Initializing system (pv=0x%08x)\n", task_getid(), pv);

    init_graphic(0x0143);  // 初始化图形模式 800x600
    DrawRectangleArea(0, 0, 800, 600, WHITE);
    dataArray = (int*)malloc(ARRAY_SIZE * sizeof(int));

    // 初始化缓冲区
    visualBuffers = (VisualArray*)malloc(sizeof(VisualArray) * BUFFER_SIZE);
    emptySemaphore = sem_create(BUFFER_SIZE);
    fullSemaphore = sem_create(0);
    mutexSemaphores = (int*)malloc(sizeof(int) * BUFFER_SIZE);

    int i;
    for (i = 0; i < BUFFER_SIZE; i++)
        mutexSemaphores[i] = sem_create(1);

    // 创建任务
    consumerThreadId = CreateTask(ConsumerTask, 4096);
    producerThreadId = CreateTask(ProducerTask, 4096);
    CreateTask(ControlTask, 4096);


    int progress = 0;
    while (1) {
        DrawProgressBar(progress, 100);
        progress = (progress + 1) % 101;
        SleepMilliseconds(100);
    }
}







/*稀释代码部分*/
static char* generate_and_find_longest_string() {
    int N;
    char* longest = NULL;
    int i, j;
    int length;
    char* str;

    N = rand() % 10 + 1;
    printf("Number of strings: %d\n", N);
    for (i = 0; i < N; i++) {
        length = rand() % 20 + 1;
        str = (char*)malloc((length + 1) * sizeof(char)); 
        if (str == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        for (j = 0; j < length; j++) {
            str[j] = 'a' + rand() % 26;
        }
        str[length] = '\0';  
        printf("String %d: %s\n", i + 1, str);
        if (longest == NULL || strlen(str) > strlen(longest)) {
            longest = str;
        }
        else {
            free(str); 
        }
    }
    return longest;
}

static void generateRandomString01(char* str, size_t length) {
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+=<>?";
    if (length == 0) {
        str[0] = '\0';
        return;
    }
    srand((unsigned int)time(NULL));
    int i;
    for (i = 0; i < length - 1; i++) {
        int index = rand() % (sizeof(charset) - 1);
        str[i] = charset[index];
    }
    str[length - 1] = '\0';
}

static void generateRandomString02(char* str, size_t length) {
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+=<>?";
    if (length == 0) {
        str[0] = '\0';
        return;
    }
    srand((unsigned int)time(NULL));
    int i;
    for (i = 0; i < length - 1; i++) {
        int index = rand() % (sizeof(charset) - 1);
        str[i] = charset[index];
    }
    str[length - 1] = '\0';
}

















