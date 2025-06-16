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
    // 淡蓝色 (RGB: 173,216,230)
    initializeVisualizationArray(&visualArray, numberArray, arraySize,
        0, 0, 750, 300, RGB(173, 216, 230));
    bubbleSortVisualization(&visualArray);
    while (1) {}
}

void runSortingTask2(void) {
    VisualizationArray visualArray;
    // 淡紫色 (RGB: 224,176,255)
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

    //int controlTaskId = createTask(handleUserInput, 1024 * 1024);
    exit(0);
}

