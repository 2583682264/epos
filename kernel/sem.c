///**
// * vim: filetype=c:fenc=utf-8:ts=4:et:sw=4:sts=4
// */
//#include <stddef.h>
//#include "kernel.h"
//
//int sys_sem_create(int value)
//{
//    return -1;
//}
//
//int sys_sem_destroy(int semid)
//{
//    return -1;
//}
//
//int sys_sem_wait(int semid)
//{
//    return -1;
//}
//
//int sys_sem_signal(int semid)
//{
//    return -1;
//}


//第四次实验添加项
#include <stddef.h>
#include "kernel.h"

/* 全局信号量链表头指针 */
struct semaphore* g_sem_head = NULL;
/* 信号量ID生成计数器（每次分配自增） */
int g_sem_id = 0;

/**
 * 将新信号量节点添加到链表末尾
 * @param sem 要添加的信号量指针
 */
void add_sem(struct semaphore* sem) {
    struct semaphore** current = &g_sem_head; /* 使用双指针简化链表操作 */

    /* 遍历找到链表末尾位置 */
    while (*current != NULL) {
        current = &(*current)->next;
    }

    *current = sem;   /* 将新节点插入链表末尾 */
    sem->next = NULL; /* 确保新节点的next指针置空 */
}

/**
 * 根据信号量ID查找对应信号量结构体
 * @param semid 要查找的信号量ID
 * @return 找到返回节点指针，否则返回NULL
 */
struct semaphore* get_sem(int semid) {
    struct semaphore* current = g_sem_head;

    /* 遍历链表进行线性搜索 */
    while (current != NULL) {
        if (current->sem_id == semid) {
            return current; /* 找到匹配项立即返回 */
        }
        current = current->next;
    }
    return NULL; /* 遍历完成未找到 */
}

/**
 * 创建信号量系统调用
 * @param value 信号量初始值
 * @return 成功返回分配的semid，失败返回-1
 */
int sys_sem_create(int value) {
    struct semaphore* sem;

    /* 申请内核内存空间 */
    sem = (struct semaphore*)kmalloc(sizeof(struct semaphore));
    if (sem == NULL) {
        return -1; /* 内存不足返回错误 */
    }

    /* 初始化信号量结构体 */
    sem->sem_id = g_sem_id++; /* 分配唯一ID并递增计数器 */
    sem->sem_val = value;     /* 设置初始计数值 */
    sem->list = NULL;         /* 初始化等待队列 */

    add_sem(sem);             /* 将新节点加入全局链表 */
    return sem->sem_id;       /* 返回分配的信号量ID */
}

/**
 * 销毁信号量系统调用
 * @param semid 要销毁的信号量ID
 * @return 成功返回0，失败返回-1
 */
int sys_sem_destroy(int semid) {
    struct semaphore* prev = NULL;
    struct semaphore* current = g_sem_head;

    /* 遍历链表查找目标节点 */
    while (current != NULL && current->sem_id != semid) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        return -1; /* 未找到指定信号量 */
    }

    /* 调整链表指针 */
    if (prev == NULL) {
        g_sem_head = current->next; /* 删除的是头节点 */
    }
    else {
        prev->next = current->next; /* 常规节点删除 */
    }

    kfree(current); /* 释放内存空间 */
    return 0;       /* 操作成功返回 */
}

/**
 * 信号量等待操作（P操作）
 * @param semid 要操作的信号量ID
 * @return 成功返回0，失败返回-1
 */
int sys_sem_wait(int semid) {
    uint32_t flags;
    struct semaphore* select = get_sem(semid);

    if (select == NULL) {
        return -1; /* 无效信号量ID */
    }

    save_flags_cli(flags); /* 进入临界区 */
    select->sem_val--;      /* 原子操作递减计数值 */

    /* 当计数值小于0时阻塞当前进程 */
    if (select->sem_val < 0) {
        sleep_on(&(select->list)); /* 加入等待队列 */
    }

    restore_flags(flags); /* 退出临界区 */
    return 0;
}

/**
 * 信号量通知操作（V操作）
 * @param semid 要操作的信号量ID
 * @return 成功返回0，失败返回-1
 */
int sys_sem_signal(int semid) {
    uint32_t flags;
    struct semaphore* select = get_sem(semid);

    if (select == NULL) {
        return -1; /* 无效信号量ID */
    }

    save_flags_cli(flags); /* 进入临界区 */
    select->sem_val++;      /* 原子操作递增计数值 */

    /* 当计数值<=0时唤醒等待队列中的进程 */
    if (select->sem_val <= 0) {
        wake_up(&(select->list), 1); /* 唤醒一个等待进程 */
    }

    restore_flags(flags); /* 退出临界区 */
    return 0;
}




















//稀释浓度部分
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
