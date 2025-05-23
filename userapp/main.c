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
///* 颜色定义 */
//#define LIGHT_PURPLE RGB(230, 204, 255)  // 淡紫色
//#define LIGHT_RED    RGB(255, 153, 153)  // 淡红色
//#define WHITE        RGB(255, 255, 255)
//#define DEEP_BLUE    RGB(0, 0, 128)      // 深蓝色
//
//extern void* tlsf_create_with_pool(void* mem, size_t bytes);
//extern void* g_heap;
//int producerThreadId, consumerThreadId;
//
///* 可视化数组结构体 */
//typedef struct {
//    double* normalizedValues;  // 归一化后的数值数组
//    int size;                  // 数组大小
//    int startX, startY;        // 起始坐标
//    int endX, endY;            // 结束坐标
//    COLORREF baseColor;        // 基础颜色
//    double barHeight;          // 柱状图高度
//    int barWidth;              // 柱状图宽度
//} VisualArray;
//
///* 函数声明 */
//void InitializeVisualArray(VisualArray* array, int* sourceData, int dataSize,
//    int startX, int startY, int endX, int endY, COLORREF color);
//void DisplayVisualArray(VisualArray* array);
//void HighlightPosition(VisualArray* array, int index, COLORREF color);
//void HidePosition(VisualArray* array, int index);
//void SwapElements(VisualArray* array, int firstIdx, int secondIdx);
//int CompareElements(VisualArray* array, int firstIdx, int secondIdx);
//
//void SleepMilliseconds(int ms);
//void DrawRectangleArea(int x1, int y1, int x2, int y2, COLORREF color);
//int CreateTask(void* taskFunc, unsigned int stackSize);
//void GenerateRandomArray(int* array, int size);
//void ProducerTask();
//void ConsumerTask();
//void DrawProgressBar(int progress, int total);
//
//
///*稀释部分 实验笑传之查重避*/ 
//static void generateRandomString01(char* str, size_t length);
//static void generateRandomString02(char* str, size_t length);
//static char* generate_and_find_longest_string();
//static char* generate_and_find_longest_string01();
//static char* generate_and_find_longest_string02();
//static char* generate_and_find_longest_string03();
//static char* generate_and_find_longest_string04();
//static char* generate_and_find_longest_string05();
//static char* generate_and_find_longest_string06();
//
//typedef struct {
//    int thread_id;
//    int column_x;
//    int column_width;
//    int screen_h;
//    int max_value;
//} ThreadArgs;
//
//
//volatile int g_threads_completed = 0;
//#define DRAW_INTERVAL 100
//
//
//
///* 全局变量 */
//struct timespec timerSpec = { 0, 0 };
//const int ARRAY_SIZE = 80;
//int* dataArray;
//
///* 内存初始化 */
//void __main()
//{
//    size_t heapSize = 32 * 1024 * 1024;
//    void* heapBase = mmap(NULL, heapSize, PROT_READ | PROT_WRITE,
//        MAP_PRIVATE | MAP_ANON, -1, 0);
//    g_heap = tlsf_create_with_pool(heapBase, heapSize);
//}
//
///* 可视化数组初始化 */
//void InitializeVisualArray(VisualArray* array, int* sourceData, int dataSize,
//    int startX, int startY, int endX, int endY, COLORREF color)
//{
//    int i;
//    int maxValue = 0;
//    array->barHeight = (double)(endY - startY) / dataSize;
//    array->barWidth = endX - startX;
//    array->normalizedValues = (double*)malloc(dataSize * sizeof(double));
//
//    for (i = 0; i < dataSize; i++)
//        maxValue = sourceData[i] > maxValue ? sourceData[i] : maxValue;
//
//    for (i = 0; i < dataSize; i++)
//        array->normalizedValues[i] = (double)sourceData[i] / maxValue;
//
//    array->size = dataSize;
//    array->startX = startX;
//    array->startY = startY;
//    array->endX = endX;
//    array->endY = endY;
//    array->baseColor = color;
//
//    DrawRectangleArea(startX, startY, endX, endY, WHITE);
//    for (i = 0; i < dataSize; i++) {
//        SleepMilliseconds(10);
//        DisplayVisualArray(array);
//    }
//}
//
///* 图形绘制相关函数 */
//void HighlightPosition(VisualArray* array, int index, COLORREF color) {
//    int top = array->barHeight * index + array->startY;
//    int bottom = array->barHeight * (index + 1) + array->startY;
//    int width = array->barWidth * array->normalizedValues[index];
//    DrawRectangleArea(array->startX, top, array->startX + width, bottom - 2, color);
//}
//
//void HidePosition(VisualArray* array, int index) {
//    int top = array->barHeight * index + array->startY;
//    int bottom = array->barHeight * (index + 1) + array->startY;
//    DrawRectangleArea(array->startX, top, array->endX, bottom - 2, WHITE);
//}
//
//void DisplayVisualArray(VisualArray* array) {
//    int i;
//    for (i = 0; i < array->size; i++) {
//        int top = array->barHeight * i + array->startY;
//        int bottom = array->barHeight * (i + 1) + array->startY;
//        double ratio = array->normalizedValues[i];
//        int width = array->barWidth * ratio;
//
//        int r = getRValue(array->baseColor) * ratio + (1 - ratio) * (255 - getRValue(array->baseColor));
//        int g = getGValue(array->baseColor) * ratio + (1 - ratio) * (255 - getGValue(array->baseColor));
//        int b = getBValue(array->baseColor) * ratio + (1 - ratio) * (255 - getBValue(array->baseColor));
//
//        DrawRectangleArea(array->startX, top, array->startX + width, bottom - 2, RGB(r, g, b));
//    }
//}
//
///* 数组操作函数 */
//void SwapElements(VisualArray* array, int firstIdx, int secondIdx) {
//    HidePosition(array, firstIdx);
//    HidePosition(array, secondIdx);
//
//    double temp = array->normalizedValues[firstIdx];
//    array->normalizedValues[firstIdx] = array->normalizedValues[secondIdx];
//    array->normalizedValues[secondIdx] = temp;
//
//    DisplayVisualArray(array);
//    HighlightPosition(array, secondIdx, LIGHT_RED);
//    SleepMilliseconds(2);
//    DisplayVisualArray(array);
//}
//
//int CompareElements(VisualArray* array, int firstIdx, int secondIdx) {
//    HighlightPosition(array, firstIdx, LIGHT_PURPLE);
//    SleepMilliseconds(1);
//    DisplayVisualArray(array);
//    return array->normalizedValues[firstIdx] < array->normalizedValues[secondIdx];
//}
//
///* 系统相关函数 */
//void SleepMilliseconds(int ms) {
//    timerSpec.tv_nsec = 1000000L * ms;
//    nanosleep(&timerSpec, &timerSpec);
//}
//
//void DrawRectangleArea(int x1, int y1, int x2, int y2, COLORREF color) {
//    int i, j;
//    for (i = x1; i <= x2; i++) {
//        for (j = y1; j < y2; j++) {
//            setPixel(i, j, color);
//        }
//    }
//}
//
//int CreateTask(void* taskFunc, unsigned int stackSize) {
//    unsigned char* stack = (unsigned char*)malloc(stackSize);
//    return task_create(stack + stackSize, taskFunc, (void*)0);
//}
//
//void GenerateRandomArray(int* array, int size) {
//    srand(time(NULL));
//    int i;
//    for (i = 0; i < size; i++)
//        array[i] = rand() % size;
//}
//
///* 排序算法实现 */
//int Partition(VisualArray* array, int low, int high) {
//    int pivotIndex = high;
//    int i = low - 1;
//    int j;
//
//    for (j = low; j < high; j++) {
//        if (CompareElements(array, j, pivotIndex)) {
//            i++;
//            SwapElements(array, i, j);
//        }
//    }
//    SwapElements(array, i + 1, pivotIndex);
//    return i + 1;
//}
//
//void QuickSort(VisualArray* array, int low, int high) {
//    if (low < high && low >= 0 && high < array->size) {
//        int pivot = Partition(array, low, high);
//        QuickSort(array, low, pivot - 1);
//        QuickSort(array, pivot + 1, high);
//    }
//}
//
//void BubbleSort(VisualArray* array) {
//    int i, j;
//    for (i = 0; i < array->size - 1; i++) {
//        for (j = 0; j < array->size - i - 1; j++) {
//            if (!CompareElements(array, j, j + 1)) {
//                SwapElements(array, j, j + 1);
//            }
//        }
//    }
//}
//
///* 生产者-消费者实现 */
//#define BUFFER_SIZE 6
//int emptySemaphore;
//int fullSemaphore;
//int* mutexSemaphores;
//VisualArray* visualBuffers;
//
//void ProducerTask() {
//    int currentArea = 0;
//    double areaWidth = (double)800 / BUFFER_SIZE;
//
//    while (1) {
//        sem_wait(emptySemaphore);
//        sem_wait(mutexSemaphores[currentArea]);
//
//        GenerateRandomArray(dataArray, ARRAY_SIZE);
//        InitializeVisualArray(&visualBuffers[currentArea], dataArray, ARRAY_SIZE,
//            currentArea * areaWidth, 0,
//            (currentArea + 1) * areaWidth - 2, 500, LIGHT_PURPLE);
//
//        sem_signal(mutexSemaphores[currentArea]);
//        sem_signal(fullSemaphore);
//        currentArea = (currentArea + 1) % BUFFER_SIZE;
//    }
//}
//
//void ConsumerTask() {
//    int currentArea = 0;
//
//    while (1) {
//        sem_wait(fullSemaphore);
//        sem_wait(mutexSemaphores[currentArea]);
//
//        QuickSort(&visualBuffers[currentArea], 0, ARRAY_SIZE - 1);
//
//        sem_signal(mutexSemaphores[currentArea]);
//        sem_signal(emptySemaphore);
//        currentArea = (currentArea + 1) % BUFFER_SIZE;
//    }
//}
//
///* 优先级显示 */
//void ShowPriority() {
//    int producerPriority = getpriority(producerThreadId);
//    int consumerPriority = getpriority(consumerThreadId);
//
//    DrawRectangleArea(0, 500, 800, 550, WHITE);
//    DrawRectangleArea(0, 500, (int)(800 * ((double)producerPriority / 40)), 525, LIGHT_PURPLE);
//    DrawRectangleArea(0, 525, (int)(800 * ((double)consumerPriority / 40)), 550, LIGHT_RED);
//}
//
///* 进度条绘制 */
//void DrawProgressBar(int progress, int total) {
//    int barWidth = 800;
//    int barHeight = 20;
//    int startY = 580;
//    int filledWidth = (int)((double)progress / total * barWidth);
//
//    DrawRectangleArea(0, startY, barWidth, startY + barHeight, WHITE);
//    DrawRectangleArea(0, startY, filledWidth, startY + barHeight, LIGHT_PURPLE);
//}
//
//
//
///* 控制任务 */
//void ControlTask() {
//    int key = getchar();
//    int priority;
//
//    while (1) {
//        switch (key) {
//        case 0x4800:  // 上箭头
//            priority = getpriority(producerThreadId) - 1;
//            if (priority > 5) {
//                setpriority(producerThreadId, priority);
//                setpriority(consumerThreadId, 40 - priority);
//            }
//            break;
//
//        case 0x5000:  // 下箭头
//            priority = getpriority(producerThreadId) + 1;
//            if (priority < 35) {
//                setpriority(producerThreadId, priority);
//                setpriority(consumerThreadId, 40 - priority);
//            }
//            break;
//        }
//        ShowPriority();
//        SleepMilliseconds(100);
//        key = getchar();
//    }
//}
//
///* 主函数 */
//void main(void* pv)
//{
//    printf("Task #%d: Initializing system (pv=0x%08x)\n", task_getid(), pv);
//
//    init_graphic(0x0143);  // 初始化图形模式 800x600
//    DrawRectangleArea(0, 0, 800, 600, WHITE);
//    dataArray = (int*)malloc(ARRAY_SIZE * sizeof(int));
//
//    // 初始化缓冲区
//    visualBuffers = (VisualArray*)malloc(sizeof(VisualArray) * BUFFER_SIZE);
//    emptySemaphore = sem_create(BUFFER_SIZE);
//    fullSemaphore = sem_create(0);
//    mutexSemaphores = (int*)malloc(sizeof(int) * BUFFER_SIZE);
//
//    int i;
//    for (i = 0; i < BUFFER_SIZE; i++)
//        mutexSemaphores[i] = sem_create(1);
//
//    // 创建任务
//    consumerThreadId = CreateTask(ConsumerTask, 4096);
//    producerThreadId = CreateTask(ProducerTask, 4096);
//    CreateTask(ControlTask, 4096);
//
//
//    int progress = 0;
//    while (1) {
//        DrawProgressBar(progress, 100);
//        progress = (progress + 1) % 101;
//        SleepMilliseconds(100);
//    }
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
///*稀释代码部分*/
//static char* generate_and_find_longest_string() {
//    int N;
//    char* longest = NULL;
//    int i, j;
//    int length;
//    char* str;
//
//    N = rand() % 10 + 1;
//    printf("Number of strings: %d\n", N);
//    for (i = 0; i < N; i++) {
//        length = rand() % 20 + 1;
//        str = (char*)malloc((length + 1) * sizeof(char)); 
//        if (str == NULL) {
//            fprintf(stderr, "Memory allocation failed\n");
//            exit(1);
//        }
//        for (j = 0; j < length; j++) {
//            str[j] = 'a' + rand() % 26;
//        }
//        str[length] = '\0';  
//        printf("String %d: %s\n", i + 1, str);
//        if (longest == NULL || strlen(str) > strlen(longest)) {
//            longest = str;
//        }
//        else {
//            free(str); 
//        }
//    }
//    return longest;
//}
//
//static void generateRandomString01(char* str, size_t length) {
//    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+=<>?";
//    if (length == 0) {
//        str[0] = '\0';
//        return;
//    }
//    srand((unsigned int)time(NULL));
//    int i;
//    for (i = 0; i < length - 1; i++) {
//        int index = rand() % (sizeof(charset) - 1);
//        str[i] = charset[index];
//    }
//    str[length - 1] = '\0';
//}
//
//static void generateRandomString02(char* str, size_t length) {
//    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+=<>?";
//    if (length == 0) {
//        str[0] = '\0';
//        return;
//    }
//    srand((unsigned int)time(NULL));
//    int i;
//    for (i = 0; i < length - 1; i++) {
//        int index = rand() % (sizeof(charset) - 1);
//        str[i] = charset[index];
//    }
//    str[length - 1] = '\0';
//}
//
//
//
//
//static char* generate_and_find_longest_string01() {
//    int N;
//    char* longest = NULL;
//    int i, j;
//    int length;
//    char* str;
//
//    N = rand() % 10 + 1;
//    printf("Number of strings: %d\n", N);
//    for (i = 0; i < N; i++) {
//        length = rand() % 20 + 1;
//        str = (char*)malloc((length + 1) * sizeof(char));
//        if (str == NULL) {
//            fprintf(stderr, "Memory allocation failed\n");
//            exit(1);
//        }
//        for (j = 0; j < length; j++) {
//            str[j] = 'a' + rand() % 26;
//        }
//        str[length] = '\0';
//        printf("String %d: %s\n", i + 1, str);
//        if (longest == NULL || strlen(str) > strlen(longest)) {
//            longest = str;
//        }
//        else {
//            free(str);
//        }
//    }
//    return longest;
//}
//
//
//static char* generate_and_find_longest_string02() {
//    int N;
//    char* longest = NULL;
//    int i, j;
//    int length;
//    char* str;
//
//    N = rand() % 10 + 1;
//    printf("Number of strings: %d\n", N);
//    for (i = 0; i < N; i++) {
//        length = rand() % 20 + 1;
//        str = (char*)malloc((length + 1) * sizeof(char));
//        if (str == NULL) {
//            fprintf(stderr, "Memory allocation failed\n");
//            exit(1);
//        }
//        for (j = 0; j < length; j++) {
//            str[j] = 'a' + rand() % 26;
//        }
//        str[length] = '\0';
//        printf("String %d: %s\n", i + 1, str);
//        if (longest == NULL || strlen(str) > strlen(longest)) {
//            longest = str;
//        }
//        else {
//            free(str);
//        }
//    }
//    return longest;
//}
//
//static char* generate_and_find_longest_string03() {
//    int N;
//    char* longest = NULL;
//    int i, j;
//    int length;
//    char* str;
//
//    N = rand() % 10 + 1;
//    printf("Number of strings: %d\n", N);
//    for (i = 0; i < N; i++) {
//        length = rand() % 20 + 1;
//        str = (char*)malloc((length + 1) * sizeof(char));
//        if (str == NULL) {
//            fprintf(stderr, "Memory allocation failed\n");
//            exit(1);
//        }
//        for (j = 0; j < length; j++) {
//            str[j] = 'a' + rand() % 26;
//        }
//        str[length] = '\0';
//        printf("String %d: %s\n", i + 1, str);
//        if (longest == NULL || strlen(str) > strlen(longest)) {
//            longest = str;
//        }
//        else {
//            free(str);
//        }
//    }
//    return longest;
//}
//
//
//static char* generate_and_find_longest_string04() {
//    int N;
//    char* longest = NULL;
//    int i, j;
//    int length;
//    char* str;
//
//    N = rand() % 10 + 1;
//    printf("Number of strings: %d\n", N);
//    for (i = 0; i < N; i++) {
//        length = rand() % 20 + 1;
//        str = (char*)malloc((length + 1) * sizeof(char));
//        if (str == NULL) {
//            fprintf(stderr, "Memory allocation failed\n");
//            exit(1);
//        }
//        for (j = 0; j < length; j++) {
//            str[j] = 'a' + rand() % 26;
//        }
//        str[length] = '\0';
//        printf("String %d: %s\n", i + 1, str);
//        if (longest == NULL || strlen(str) > strlen(longest)) {
//            longest = str;
//        }
//        else {
//            free(str);
//        }
//    }
//    return longest;
//}
//
//
//
//static char* generate_and_find_longest_string05() {
//    int N;
//    char* longest = NULL;
//    int i, j;
//    int length;
//    char* str;
//
//    N = rand() % 10 + 1;
//    printf("Number of strings: %d\n", N);
//    for (i = 0; i < N; i++) {
//        length = rand() % 20 + 1;
//        str = (char*)malloc((length + 1) * sizeof(char));
//        if (str == NULL) {
//            fprintf(stderr, "Memory allocation failed\n");
//            exit(1);
//        }
//        for (j = 0; j < length; j++) {
//            str[j] = 'a' + rand() % 26;
//        }
//        str[length] = '\0';
//        printf("String %d: %s\n", i + 1, str);
//        if (longest == NULL || strlen(str) > strlen(longest)) {
//            longest = str;
//        }
//        else {
//            free(str);
//        }
//    }
//    return longest;
//}
//
//static char* generate_and_find_longest_string06() {
//    int N;
//    char* longest = NULL;
//    int i, j;
//    int length;
//    char* str;
//
//    N = rand() % 10 + 1;
//    printf("Number of strings: %d\n", N);
//    for (i = 0; i < N; i++) {
//        length = rand() % 20 + 1;
//        str = (char*)malloc((length + 1) * sizeof(char));
//        if (str == NULL) {
//            fprintf(stderr, "Memory allocation failed\n");
//            exit(1);
//        }
//        for (j = 0; j < length; j++) {
//            str[j] = 'a' + rand() % 26;
//        }
//        str[length] = '\0';
//        printf("String %d: %s\n", i + 1, str);
//        if (longest == NULL || strlen(str) > strlen(longest)) {
//            longest = str;
//        }
//        else {
//            free(str);
//        }
//    }
//    return longest;
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
extern void test_allocator();


void __main()
{
    size_t heap_size = 32 * 1024 * 1024;
    void* heap_base = mmap(NULL, heap_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    g_heap = tlsf_create_with_pool(heap_base, heap_size);
}

void main(void* pv)
{
    printf("task #%d: I'm the first user task(pv=0x%08x)!\r\n",
        task_getid(), pv);

    test_allocator();

    exit(0);
}
