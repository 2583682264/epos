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
//
//extern void* tlsf_create_with_pool(void* mem, size_t bytes);
//extern void* g_heap;
//int task1Id, task2Id;
//
//typedef struct {
//    float* elements;
//    int elementCount;
//    int startX;
//    int startY;
//    int endX;
//    int endY;
//    COLORREF barColor;
//    double barWidth;
//    int totalHeight;
//} VisualizationArray;
//
///* 函数声明 */
//void initializeVisualizationArray(VisualizationArray* array, int* data, int count,
//    int startX, int startY, int endX, int endY, COLORREF color);
//void displayElement(VisualizationArray* array, int index);
//void hideElement(VisualizationArray* array, int index);
//void swapElements(VisualizationArray* array, int indexA, int indexB);
//int compareElements(VisualizationArray* array, int indexA, int indexB);
//int getElementValue(VisualizationArray* array, int index);
//
//void sleepMilliseconds(int ms);
//void drawArea(int x1, int y1, int x2, int y2, COLORREF color);
//int createTask(void* taskFunc, unsigned int stackSize);
//void createShuffledArray(int* array, int elementCount);
//void bubbleSortVisualization(VisualizationArray* array);
//void runSortingTask1(void);
//void runSortingTask2(void);
//void updatePriorityDisplay(void);
//void handleUserInput(void);
//
///* 全局变量 */
//static struct timespec sleepTimeSpec = { 0, 0 };
//static const int arraySize = 200;
//int* numberArray;
//
///* 初始化可视化数组结构 */
//void initializeVisualizationArray(VisualizationArray* array, int* data, int count,
//    int startX, int startY, int endX, int endY, COLORREF color) {
//    array->barWidth = (double)(endX - startX) / count;
//    array->totalHeight = endY - startY;
//    array->elements = (float*)malloc(count * sizeof(float));
//
//    int i;
//    for (i = 0; i < count; ++i) {
//        array->elements[i] = (float)data[i] / count;
//    }
//
//    array->elementCount = count;
//    array->startX = startX;
//    array->startY = startY;
//    array->endX = endX;
//    array->endY = endY;
//    array->barColor = color;
//
//    for (i = 0; i < count; i++) {
//        displayElement(array, i);
//    }
//}
//
///* 可视化相关辅助函数 */
//void highlightElementGreen(VisualizationArray* array, int index) {
//    int left = array->barWidth * index + array->startX;
//    int right = left + array->barWidth;
//    int height = array->totalHeight * array->elements[index];
//    drawArea(left, array->endY - height, right, array->endY, RGB(0, 255, 0));
//}
//
//void highlightElementRed(VisualizationArray* array, int index) {
//    int left = array->barWidth * index + array->startX;
//    int right = left + array->barWidth;
//    int height = array->totalHeight * array->elements[index];
//    drawArea(left, array->endY - height, right, array->endY, RGB(255, 0, 0));
//}
//
//void hideElement(VisualizationArray* array, int index) {
//    int left = array->barWidth * index + array->startX;
//    int right = left + array->barWidth;
//    drawArea(left, array->startY, right, array->endY, RGB(255, 255, 255));
//}
//
//void displayElement(VisualizationArray* array, int index) {
//    int left = array->barWidth * index + array->startX;
//    int right = left + array->barWidth;
//    float normalizedValue = array->elements[index];
//    int barHeight = array->totalHeight * normalizedValue;
//    int red = getRValue(array->barColor) * normalizedValue;
//    int green = getGValue(array->barColor) * normalizedValue;
//    int blue = getBValue(array->barColor) * normalizedValue;
//    drawArea(left, array->endY - barHeight, right, array->endY, RGB(red, green, blue));
//}
//
///* 排序操作函数 */
//void swapElements(VisualizationArray* array, int indexA, int indexB) {
//    hideElement(array, indexA);
//    hideElement(array, indexB);
//    float temp = array->elements[indexA];
//    array->elements[indexA] = array->elements[indexB];
//    array->elements[indexB] = temp;
//    displayElement(array, indexA);
//    highlightElementRed(array, indexB);
//    sleepMilliseconds(5);
//    displayElement(array, indexB);
//}
//
//int compareElements(VisualizationArray* array, int indexA, int indexB) {
//    highlightElementGreen(array, indexA);
//    sleepMilliseconds(2);
//    displayElement(array, indexA);
//    return array->elements[indexA] < array->elements[indexB];
//}
//
///* 系统相关函数 */
//void __main() {
//    size_t heapSize = 32 * 1024 * 1024;
//    void* heapBase = mmap(NULL, heapSize, PROT_READ | PROT_WRITE,
//        MAP_PRIVATE | MAP_ANON, -1, 0);
//    g_heap = tlsf_create_with_pool(heapBase, heapSize);
//}
//
//void sleepMilliseconds(int ms) {
//    sleepTimeSpec.tv_nsec = 1000000L * ms;
//    nanosleep(&sleepTimeSpec, &sleepTimeSpec);
//}
//
//void drawArea(int x1, int y1, int x2, int y2, COLORREF color) {
//    int i, j;
//    for (i = x1; i <= x2; i++) {
//        for (j = y1; j < y2; j++) {
//            setPixel(i, j, color);
//        }
//    }
//}
//
//int createTask(void* taskFunc, unsigned int stackSize) {
//    unsigned char* stack = (unsigned char*)malloc(stackSize);
//    return task_create(stack + stackSize, taskFunc, NULL);
//}
//
///* 数组处理函数 */
//void createShuffledArray(int* array, int elementCount) {
//    srand(time(NULL));
//    int i;
//    for (i = 0; i < elementCount; i++) {
//        array[i] = i;
//    }
//    for (i = elementCount - 1; i > 0; i--) {
//        int j = rand() % (i + 1);
//        int temp = array[i];
//        array[i] = array[j];
//        array[j] = temp;
//    }
//}
//
///* 排序算法实现 */
//void bubbleSortVisualization(VisualizationArray* array) {
//    int count = array->elementCount;
//    int i, j;
//    for (i = 0; i < count - 1; i++) {
//        for (j = 0; j < count - i - 1; j++) {
//            if (compareElements(array, j + 1, j)) {
//                swapElements(array, j, j + 1);
//            }
//        }
//    }
//}
//
///* 任务函数 */
//void runSortingTask1(void) {
//    VisualizationArray visualArray;
//    initializeVisualizationArray(&visualArray, numberArray, arraySize,
//        0, 0, 750, 300, RGB(0, 0, 255));
//    bubbleSortVisualization(&visualArray);
//    while (1) {}
//}
//
//void runSortingTask2(void) {
//    VisualizationArray visualArray;
//    initializeVisualizationArray(&visualArray, numberArray, arraySize,
//        0, 300, 750, 600, RGB(255, 255, 0));
//    bubbleSortVisualization(&visualArray);
//    while (1) {}
//}
//
///* 界面更新函数 */
//void updatePriorityDisplay(void) {
//    int priority1 = getpriority(task1Id);
//    int priority2 = getpriority(task2Id);
//    drawArea(760, 0, 790, 600, RGB(255, 255, 255));
//    drawArea(760, (int)(300 * ((double)priority1 / 40)), 790, 300, RGB(0, 0, 255));
//    drawArea(760, (int)(300 + 300 * ((double)priority2 / 40)), 790, 600, RGB(255, 255, 0));
//}
//
///* 用户输入处理 */
//void handleUserInput(void) {
//    int keyCode = getchar();
//    int newPriority;
//    while (1) {
//        switch (keyCode) {
//        case 0x4800: /* 上箭头 */
//            newPriority = getpriority(task1Id) - 1;
//            if (newPriority > 5) {
//                setpriority(task1Id, newPriority);
//                setpriority(task2Id, 40 - newPriority);
//            }
//            break;
//        case 0x5000: /* 下箭头 */
//            newPriority = getpriority(task1Id) + 1;
//            if (newPriority < 35) {
//                setpriority(task1Id, newPriority);
//                setpriority(task2Id, 40 - newPriority);
//            }
//            break;
//        default:
//            break;
//        }
//        updatePriorityDisplay();
//        sleepMilliseconds(100);
//        keyCode = getchar();
//    }
//}
//
///* 主入口函数 */
//void main(void* param) {
//    printf("Task #%d: Initializing user task (param=0x%08x)!\r\n",
//        task_getid(), param);
//    init_graphic(0x0143); /* 初始化显示: 800x600 */
//    drawArea(0, 0, 800, 600, RGB(255, 255, 255));
//
//    numberArray = (int*)malloc(arraySize * sizeof(int));
//    createShuffledArray(numberArray, arraySize);
//
//    task1Id = createTask(runSortingTask1, 1024 * 1024);
//    task2Id = createTask(runSortingTask2, 1024 * 1024);
//    updatePriorityDisplay();
//
//    int controlTaskId = createTask(handleUserInput, 1024 * 1024);
//    exit(0);
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

extern void* tlsf_create_with_pool(void* mem, size_t bytes);
extern void* g_heap;
int task1Id, task2Id;

typedef struct {
    float* elements;
    int elementCount;
    int startX;
    int startY;
    int endX;
    int endY;
    COLORREF barColor;
    double barWidth;
    int totalHeight;
} VisualizationArray;

/* 函数声明 */
void initializeVisualizationArray(VisualizationArray* array, int* data, int count,
    int startX, int startY, int endX, int endY, COLORREF color);
void displayElement(VisualizationArray* array, int index);
void hideElement(VisualizationArray* array, int index);
void swapElements(VisualizationArray* array, int indexA, int indexB);
int compareElements(VisualizationArray* array, int indexA, int indexB);
int getElementValue(VisualizationArray* array, int index);

void sleepMilliseconds(int ms);
void drawArea(int x1, int y1, int x2, int y2, COLORREF color);
int createTask(void* taskFunc, unsigned int stackSize);
void createShuffledArray(int* array, int elementCount);
void bubbleSortVisualization(VisualizationArray* array);
void runSortingTask1(void);
void runSortingTask2(void);
void updatePriorityDisplay(void);
void handleUserInput(void);

/* 全局变量 */
static struct timespec sleepTimeSpec = { 0, 0 };
static const int arraySize = 250;
int* numberArray;

/* 初始化可视化数组结构 */
void initializeVisualizationArray(VisualizationArray* array, int* data, int count,
    int startX, int startY, int endX, int endY, COLORREF color) {
    array->barWidth = (double)(endX - startX) / count;
    array->totalHeight = endY - startY;
    array->elements = (float*)malloc(count * sizeof(float));

    int i;
    for (i = 0; i < count; ++i) {
        array->elements[i] = (float)data[i] / count;
    }

    array->elementCount = count;
    array->startX = startX;
    array->startY = startY;
    array->endX = endX;
    array->endY = endY;
    array->barColor = color;

    for (i = 0; i < count; i++) {
        displayElement(array, i);
    }
}

/* 可视化相关辅助函数 */
void highlightElementGreen(VisualizationArray* array, int index) {
    int left = array->barWidth * index + array->startX;
    int right = left + array->barWidth;
    int height = array->totalHeight * array->elements[index];
    drawArea(left, array->endY - height, right, array->endY, RGB(0, 255, 0));
}

void highlightElementRed(VisualizationArray* array, int index) {
    int left = array->barWidth * index + array->startX;
    int right = left + array->barWidth;
    int height = array->totalHeight * array->elements[index];
    drawArea(left, array->endY - height, right, array->endY, RGB(255, 0, 0));
}

void hideElement(VisualizationArray* array, int index) {
    int left = array->barWidth * index + array->startX;
    int right = left + array->barWidth;
    drawArea(left, array->startY, right, array->endY, RGB(255, 255, 255));
}

void displayElement(VisualizationArray* array, int index) {
    int left = array->barWidth * index + array->startX;
    int right = left + array->barWidth;
    float normalizedValue = array->elements[index];
    int barHeight = array->totalHeight * normalizedValue;
    int red = getRValue(array->barColor) * normalizedValue;
    int green = getGValue(array->barColor) * normalizedValue;
    int blue = getBValue(array->barColor) * normalizedValue;
    drawArea(left, array->endY - barHeight, right, array->endY, RGB(red, green, blue));
}

/* 排序操作函数 */
void swapElements(VisualizationArray* array, int indexA, int indexB) {
    hideElement(array, indexA);
    hideElement(array, indexB);
    float temp = array->elements[indexA];
    array->elements[indexA] = array->elements[indexB];
    array->elements[indexB] = temp;
    displayElement(array, indexA);
    highlightElementRed(array, indexB);
    sleepMilliseconds(5);
    displayElement(array, indexB);
}

int compareElements(VisualizationArray* array, int indexA, int indexB) {
    highlightElementGreen(array, indexA);
    sleepMilliseconds(2);
    displayElement(array, indexA);
    return array->elements[indexA] < array->elements[indexB];
}

/* 系统相关函数 */
void __main() {
    size_t heapSize = 32 * 1024 * 1024;
    void* heapBase = mmap(NULL, heapSize, PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANON, -1, 0);
    g_heap = tlsf_create_with_pool(heapBase, heapSize);
}

void sleepMilliseconds(int ms) {
    sleepTimeSpec.tv_nsec = 1000000L * ms;
    nanosleep(&sleepTimeSpec, &sleepTimeSpec);
}

void drawArea(int x1, int y1, int x2, int y2, COLORREF color) {
    int i, j;
    for (i = x1; i <= x2; i++) {
        for (j = y1; j < y2; j++) {
            setPixel(i, j, color);
        }
    }
}

int createTask(void* taskFunc, unsigned int stackSize) {
    unsigned char* stack = (unsigned char*)malloc(stackSize);
    return task_create(stack + stackSize, taskFunc, NULL);
}

/* 数组处理函数 */
void createShuffledArray(int* array, int elementCount) {
    srand(time(NULL));
    int i;
    for (i = 0; i < elementCount; i++) {
        array[i] = i;
    }
    for (i = elementCount - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

/* 排序算法实现 */
void bubbleSortVisualization(VisualizationArray* array) {
    int count = array->elementCount;
    int i, j;
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (compareElements(array, j + 1, j)) {
                swapElements(array, j, j + 1);
            }
        }
    }
}

/* 任务函数 */
void runSortingTask1(void) {
    VisualizationArray visualArray;
    // 修改为淡蓝色 (RGB: 173,216,230)
    initializeVisualizationArray(&visualArray, numberArray, arraySize,
        0, 0, 750, 300, RGB(173, 216, 230));
    bubbleSortVisualization(&visualArray);
    while (1) {}
}

void runSortingTask2(void) {
    VisualizationArray visualArray;
    // 修改为淡紫色 (RGB: 224,176,255)
    initializeVisualizationArray(&visualArray, numberArray, arraySize,
        0, 300, 750, 600, RGB(224, 176, 255));
    bubbleSortVisualization(&visualArray);
    while (1) {}
}

/* 界面更新函数 */
void updatePriorityDisplay(void) {
    int priority1 = getpriority(task1Id);
    int priority2 = getpriority(task2Id);
    drawArea(760, 0, 790, 600, RGB(255, 255, 255));
    // 修改进度条颜色为对应的淡蓝色和淡紫色
    drawArea(760, (int)(300 * ((double)priority1 / 40)), 790, 300, RGB(173, 216, 230));
    drawArea(760, (int)(300 + 300 * ((double)priority2 / 40)), 790, 600, RGB(224, 176, 255));
}

/* 用户输入处理 */
void handleUserInput(void) {
    int keyCode = getchar();
    int newPriority;
    while (1) {
        switch (keyCode) {
        case 0x4800: /* 上箭头 */
            newPriority = getpriority(task1Id) - 1;
            if (newPriority > 5) {
                setpriority(task1Id, newPriority);
                setpriority(task2Id, 40 - newPriority);
            }
            break;
        case 0x5000: /* 下箭头 */
            newPriority = getpriority(task1Id) + 1;
            if (newPriority < 35) {
                setpriority(task1Id, newPriority);
                setpriority(task2Id, 40 - newPriority);
            }
            break;
        default:
            break;
        }
        updatePriorityDisplay();
        sleepMilliseconds(100);
        keyCode = getchar();
    }
}




/* 主入口函数 */
void main(void* param) {
    printf("Task #%d: Initializing user task (param=0x%08x)!\r\n",
        task_getid(), param);
    init_graphic(0x0143); /* 初始化显示: 800x600 */
    drawArea(0, 0, 800, 600, RGB(255, 255, 255));

    numberArray = (int*)malloc(arraySize * sizeof(int));
    createShuffledArray(numberArray, arraySize);

    task1Id = createTask(runSortingTask1, 1024 * 1024);
    task2Id = createTask(runSortingTask2, 1024 * 1024);
    updatePriorityDisplay();

    int controlTaskId = createTask(handleUserInput, 1024 * 1024);
    exit(0);
}





















//下面是稀释浓度部分的代码

static void generateRandomString01(char* str, size_t length) {
    // 定义字符集：包括小写字母、大写字母、数字和特殊符号
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+=<>?";

    // 确保str足够长，可以存储字符和结尾符'\0'
    if (length == 0) {
        str[0] = '\0';  // 如果长度为0，返回一个空字符串
        return;
    }

    // 使用当前时间作为种子初始化随机数生成器
    srand((unsigned int)time(NULL));

    int i; // 将变量声明移到循环外，符合C90标准
    for (i = 0; i < length - 1; i++) { // 留出最后一个位置存放'\0'
        int index = rand() % (sizeof(charset) - 1);  // 获取字符集中的随机索引
        str[i] = charset[index];
    }

    // 字符串结尾加上'\0'
    str[length - 1] = '\0';
}

static void generateRandomString02(char* str, size_t length) {
    // 定义字符集：包括小写字母、大写字母、数字和特殊符号
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+=<>?";

    // 确保str足够长，可以存储字符和结尾符'\0'
    if (length == 0) {
        str[0] = '\0';  // 如果长度为0，返回一个空字符串
        return;
    }

    // 使用当前时间作为种子初始化随机数生成器
    srand((unsigned int)time(NULL));

    int i; // 将变量声明移到循环外，符合C90标准
    for (i = 0; i < length - 1; i++) { // 留出最后一个位置存放'\0'
        int index = rand() % (sizeof(charset) - 1);  // 获取字符集中的随机索引
        str[i] = charset[index];
    }

    // 字符串结尾加上'\0'
    str[length - 1] = '\0';
}


static void generateRandomString03(char* str, size_t length) {
    // 定义字符集：包括小写字母、大写字母、数字和特殊符号
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+=<>?";

    // 确保str足够长，可以存储字符和结尾符'\0'
    if (length == 0) {
        str[0] = '\0';  // 如果长度为0，返回一个空字符串
        return;
    }

    // 使用当前时间作为种子初始化随机数生成器
    srand((unsigned int)time(NULL));

    int i; // 将变量声明移到循环外，符合C90标准
    for (i = 0; i < length - 1; i++) { // 留出最后一个位置存放'\0'
        int index = rand() % (sizeof(charset) - 1);  // 获取字符集中的随机索引
        str[i] = charset[index];
    }

    // 字符串结尾加上'\0'
    str[length - 1] = '\0';
}


static void generateRandomString04(char* str, size_t length) {
    // 定义字符集：包括小写字母、大写字母、数字和特殊符号
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+=<>?";

    // 确保str足够长，可以存储字符和结尾符'\0'
    if (length == 0) {
        str[0] = '\0';  // 如果长度为0，返回一个空字符串
        return;
    }

    // 使用当前时间作为种子初始化随机数生成器
    srand((unsigned int)time(NULL));

    int i; // 将变量声明移到循环外，符合C90标准
    for (i = 0; i < length - 1; i++) { // 留出最后一个位置存放'\0'
        int index = rand() % (sizeof(charset) - 1);  // 获取字符集中的随机索引
        str[i] = charset[index];
    }

    // 字符串结尾加上'\0'
    str[length - 1] = '\0';
}

static void generateRandomString05(char* str, size_t length) {
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

static void generateRandomString06(char* str, size_t length) {
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

static void generateRandomString07(char* str, size_t length) {
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

static void generateRandomString08(char* str, size_t length) {
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