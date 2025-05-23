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
#include <time.h>


extern void *tlsf_create_with_pool(void* mem, size_t bytes);
extern void *g_heap;
int tid1,tid2;
typedef struct {
    double *arr;       
    int n;          
    int x1, x2;     
    int y1, y2;
    COLORREF color;
    double width;
    int height; 
} VisArr;

//start
void initVisArr(VisArr *visArr, int* _arr, int _n, int _x1, int _y1, int _x2, int _y2, COLORREF _color);
void displayVisArr(VisArr *visArr);
void hidePosi();
void showPosi();
void swap(VisArr *visArr, int i, int j);
int isSmaller(VisArr *visArr, int i, int j);
int getArrVal(VisArr *visArr, int i);


void sleepMs(int ms);
void area(int x1, int y1, int x2, int y2, COLORREF cr);
int taskFunc(void* func, unsigned int stack_size); // 假设task_create已经声明在其他地方
void CreateShuffleArray(int *array, int n);
void tsk_1();
void tsk_2();


//--------------------------------------------------------------------------------------------------
void initVisArr(VisArr *visArr, int* _arr, int _n, int _x1, int _y1, int _x2, int _y2, COLORREF _color){
    visArr->width = (double)(_x2 - _x1) / _n;
    visArr->height = _y2 - _y1; 
    visArr->arr = (double *)malloc(_n * sizeof(double));
    int i;
    int max=0;
    for(i=0;i<_n;i++){
        max=_arr[i]>max? _arr[i] : max;
    }
    for(i = 0; i < _n; ++i){
        visArr->arr[i] =(double) _arr[i]/max;
    }
    visArr->n = _n;
    visArr->x1 = _x1;
    visArr->x2 = _x2;
    visArr->y1 = _y1;
    visArr->y2 = _y2;
    visArr->color = _color;
    area(_x1,_y1,_x2,_y2,RGB(255,255,255));
    for(i=0;i<_n;i++){
        sleepMs(10);
        showPosi(visArr,i);
    }
}

void greenPosi(VisArr *visArr,int i){
    int left=visArr->width*i+visArr->x1;
    int right=visArr->width*(i+1)+visArr->x1;
    int height=visArr->height*visArr->arr[i];
    area(left,visArr->y2-height,right-2,visArr->y2,RGB(0,255,0));
}
void redPosi(VisArr *visArr,int i){
    int left=visArr->width*i+visArr->x1;
    int right=visArr->width*(i+1)+visArr->x1;
    int height=visArr->height*visArr->arr[i];
    area(left,visArr->y2-height,right-2,visArr->y2,RGB(255,0,0));
}

void hidePosi(VisArr *visArr,int i){
    int left=visArr->width*i+visArr->x1;
    int right=visArr->width*(i+1)+visArr->x1;
    area(left,visArr->y1,right-2,visArr->y2,RGB(255,255,255));
}

void showPosi(VisArr *visArr,int i){
    int left=visArr->width*i+visArr->x1;
    int right=visArr->width*(i+1)+visArr->x1;
    double rank=visArr->arr[i];
    int height=visArr->height*rank;
    int r=getRValue(visArr->color)*rank+(1-rank)*(255-getRValue(visArr->color));
    int g=getGValue(visArr->color)*rank+(1-rank)*(255-getGValue(visArr->color));
    int b=getBValue(visArr->color)*rank+(1-rank)*(255-getBValue(visArr->color));
    area(left,visArr->y2-height,right-2,visArr->y2,RGB(r,g,b));
}

void swap(VisArr *visArr,int i,int j){
    hidePosi(visArr,i);
    hidePosi(visArr,j);
    double tmp=visArr->arr[i];
    visArr->arr[i]=visArr->arr[j];
    visArr->arr[j]=tmp;
    showPosi(visArr,i);
    redPosi(visArr,j);
    sleepMs(2);
    showPosi(visArr,j);
}

int isSmaller(VisArr *visArr,int i,int j){
    greenPosi(visArr,i);
    sleepMs(1);
    showPosi(visArr,i);
    if(visArr->arr[i]<visArr->arr[j]){
        return 1;
    }else{
        return 0;
    }
    //
}
//------------------------------------------------------------------------------------


struct timespec timeG={0,0};
int n=100;
int * arr;

void __main()
{
    size_t heap_size = 32*1024*1024;
    void  *heap_base = mmap(NULL, heap_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
	g_heap = tlsf_create_with_pool(heap_base, heap_size);
}

void sleepMs(int ms){
    timeG.tv_nsec=1000000L*ms;
    nanosleep(&timeG,&timeG);
}

void area(int x1, int y1, int x2, int y2, COLORREF cr){
    int i;
    int j;
    for(i=x1;i<=x2;i++){
        for(j=y1;j<y2;j++){
            setPixel(i,j,cr);
        }
    }
}


int taskFunc(void* func,unsigned int stack_size){
    unsigned char *stack_q=(unsigned char*)malloc(stack_size);
    return task_create(stack_q+stack_size,func,(void*)0);
}

void CreateShuffleArray(int *array, int n) {
    srand(time(NULL));
    int i;
    for(i=0;i<n;i++){
        array[i]=rand() %n;
    }
}


// 快速排序的分区函数
int partition(VisArr *visArr, int low, int high) {
    int pivotIndex = high;
    int i = low - 1;
    int j=low;
    for (j = low; j < high; j++) {
        if (isSmaller(visArr, j, pivotIndex)) {
            i++;
            swap(visArr, i, j);
        }
    }
    swap(visArr, i + 1, pivotIndex);
    return i + 1;
}

// 快速排序函数
void quickSort(VisArr *visArr, int low, int high) {
    if (low < high && low >= 0 && high < visArr->n) {
        int pivotIndex = partition(visArr, low, high);

        quickSort(visArr, low, pivotIndex - 1);
        quickSort(visArr, pivotIndex + 1, high);
    }
}

// 冒泡排序函数
void bubbleSort(VisArr *visArr) {
    int n = visArr->n;
    int i,j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (!isSmaller(visArr, j, j + 1)) {
                swap(visArr, j, j + 1);
            }
        }
    }
}

#define numBuf 10
int emptyId;
int fullId;
int* mutexIds;
VisArr*visArrs;



void producer(){
    int produceArea=0;
    double height=(double)600/numBuf;
    while(1){
        sem_wait(emptyId);
        sem_wait(mutexIds[produceArea]);
        CreateShuffleArray(arr,n);
        initVisArr(&visArrs[produceArea],arr,n,0,produceArea*height,760,(produceArea+1)*height-2,RGB(0,255,255));
        sem_signal(mutexIds[produceArea]);
        sem_signal(fullId);
        produceArea=(produceArea+1)%numBuf;  
    }
}

void consumer(){
    int consumeArea=0;
    
    while(1){
        sem_wait(fullId);
        sem_wait(mutexIds[consumeArea]);

        quickSort(&visArrs[consumeArea],0,n-1);
        
        sem_signal(mutexIds[consumeArea]);
        sem_signal(emptyId);
        consumeArea=(consumeArea+1)%numBuf;      
    }
}


void showPower(){
    int pr1=getpriority(tid1);
    int pr2=getpriority(tid2);
    area(760,0,799,600,RGB(255,255,255));
    area(760,600-(int)(600*((double)pr1/40)),779,600,RGB(0,255,255));
    area(780,600-(int)(600*((double)pr2/40)),799,600,RGB(255,0,0));
}

void controlTask(){
    int key = getchar();
    int pr;
    while(1){
        switch (key)
        {
        case 0x4800://right
            pr=getpriority(tid1)-1;
            if(pr>5){
                setpriority(tid1,pr);
                setpriority(tid2,40-pr);
            }

            break;
        case 0x5000://left
            pr=getpriority(tid1)+1;
            if(pr<35){
                setpriority(tid1,pr);
                setpriority(tid2,40-pr);
            }
            break;
        
        default:
            break;
        }
        showPower();
        sleepMs(100);
        key=getchar();
    }
}

/**
 * 第一个运行在用户模式的线程所执行的函数
 */
void main(void *pv)
{
    /**/
    printf("task #%d: I'm the first user task(pv=0x%08x)!\r\n",
            task_getid(), pv);
    /**/

    // init_graphic(0x0143);//初始化屏幕  -> x=800  V y=600 
    // area(0, 0, 800, 600, RGB(255,255,255));
    // arr=(int *)malloc(n * sizeof(int));
    
    
    // visArrs=( VisArr*)malloc( sizeof(VisArr)*numBuf );    
    // emptyId=sem_create(numBuf);
    // fullId=sem_create(0);
    // mutexIds=(int*)malloc(sizeof(int)*numBuf);
    // int i=0;
    // for(i=0;i<numBuf;i++){
    //     mutexIds[i]=sem_create(1);
    //     //printf("mutex[%d]=%d",i,mutexIds[i]);
    // }
    
    // tid2=taskFunc(consumer,1024*1024);
    // tid1=taskFunc(producer,1024*1024);
    // showPower();
    // taskFunc(controlTask,1024*1024);
    test_allocator();
    
    exit(0); // 或 return 0;
}

