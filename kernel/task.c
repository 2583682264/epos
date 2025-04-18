/**
 * vim: filetype=c:fenc=utf-8:ts=4:et:sw=4:sts=4
 *
 * Copyright (C) 2008, 2013 Hong MingJian<hongmingjian@gmail.com>
 * All rights reserved.
 *
 * This file is part of the EPOS.
 *
 * Redistribution and use in source and binary forms are freely
 * permitted provided that the above copyright notice and this
 * paragraph and the following disclaimer are duplicated in all
 * such forms.
 *
 * This software is provided "AS IS" and without any express or
 * implied warranties, including, without limitation, the implied
 * warranties of merchantability and fitness for a particular
 * purpose.
 *
 */
#include <stddef.h>
#include <string.h>
#include "kernel.h"

#define FIXEDPT_SHIFT 16  // 固定的小数位数，16 位小数
#define FIXEDPT_FROM_INT(x) ((x) << FIXEDPT_SHIFT)  // 将整数转换为定点数
//第三次实验添加项

int g_resched;
struct tcb *g_task_head;
struct tcb *g_task_running;
struct tcb *task0;
struct tcb *g_task_own_fpu;

/**
 * CPU调度器函数，这里只实现了轮转调度算法
 *
 * 注意：该函数的执行不能被中断
 */
void schedule() {

    // 确保 max 和 min 宏定义存在
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

    struct tcb* task_running = g_task_running;  // 当前运行的任务
    struct tcb* task_cur = g_task_head;         // 任务链表头

    // 遍历所有线程，计算其优先级
    while (task_cur != NULL) {
        // 使用定点数进行运算，计算优先级
        fixedpt estcpu_div = fixedpt_div(task_cur->estcpu, fixedpt_fromint(4));  // 计算 estcpu / 4
        fixedpt priority = fixedpt_sub(FIXEDPT_FROM_INT(PRI_USER_MAX), fixedpt_toint(estcpu_div)); // PRI_USER_MAX - estcpu / 4
        priority = fixedpt_sub(priority, fixedpt_fromint(task_cur->nice * 2)); // - nice * 2

        // 限制优先级在 PRI_USER_MIN 和 PRI_USER_MAX 之间
        task_cur->priority = max(fixedpt_toint(priority), PRI_USER_MIN);
        task_cur->priority = min(task_cur->priority, PRI_USER_MAX);

        task_cur = task_cur->next;  // 继续遍历下一个线程
    }

    task_cur = g_task_head;  // 重新从头开始遍历

    // 找出优先级最高的就绪线程
    while (task_cur != NULL) {
        // 如果是就绪状态的线程并且不是task0
        if (task_cur->tid != 0 && task_cur->state == TASK_STATE_READY) {
            // 如果当前线程优先级更高，更新 task_running
            if (task_running->tid == 0 || task_cur->priority > task_running->priority) {
                task_running = task_cur;
            }
        }
        task_cur = task_cur->next;  // 继续遍历下一个线程
    }

    // 如果任务运行的线程还是当前线程，且当前线程已经就绪，则不需要调度
    if (task_running == g_task_running) {
        if (task_running->state == TASK_STATE_READY) {
            return;  // 当前任务已经准备好，不需要进行切换
        }
        task_running = task0;  // 如果没有可用的任务，运行 task0
    }

    g_resched = 0;  // 清除调度标志
    switch_to(task_running);  // 切换到选中的线程

}


/**
 * 把当前线程切换为等待状态，等待在*head队列中
 *
 * 注意：该函数的执行不能被中断
 */
void sleep_on(struct wait_queue **head)
{
    struct wait_queue wait;

    wait.tsk = g_task_running;
    wait.next = *head;
    *head = &wait;

    g_task_running->state = TASK_STATE_WAITING;
    schedule();

    if(*head == &wait)
        *head = wait.next;
    else {
        struct wait_queue *p, *q;
        p = *head;
        do {
            q = p;
            p = p->next;
            if(p == &wait) {
                q->next = p->next;
                break;
            }
        } while(p != NULL);
    }
}

/**
 * 唤醒n个等待在*head队列中的线程。
 * 如果n<0，唤醒队列中的所有线程
 *
 * 注意：该函数的执行不能被中断
 */
void wake_up(struct wait_queue **head, int n)
{
    struct wait_queue *p;

    for(p = *head; (p!=NULL) && n; p = p->next, n--)
        p->tsk->state = TASK_STATE_READY;
}

static
void add_task(struct tcb *tsk)
{
    if(g_task_head == NULL)
        g_task_head = tsk;
    else {
        struct tcb *p, *q;
        p = g_task_head;
        do {
            q = p;
            p = p->next;
        } while(p != NULL);
        q->next = tsk;
    }
}

static
void remove_task(struct tcb *tsk)
{
    if(g_task_head != NULL) {
        if(tsk == g_task_head) {
            g_task_head = g_task_head->next;
        } else {
            struct tcb *p, *q;
            p = g_task_head;
            do {
                q = p;
                p = p->next;
                if(p == tsk)
                    break;
            } while(p != NULL);

            if(p == tsk)
                q->next = p->next;
        }
    }
}

static
struct tcb* get_task(int tid)
{
    struct tcb *tsk;

    tsk = g_task_head;
    while(tsk != NULL) {
        if(tsk->tid == tid)
            break;
        tsk = tsk->next;
    }

    return tsk;
}



/**
 * 系统调用task_create的执行函数
 *
 * 创建一个新的线程，该线程执行func函数，并向新线程传递参数pv
 */
struct tcb *sys_task_create(void *tos,
                            void (*func)(void *pv), void *pv)
{
    static int tid = 0;
    struct tcb *new;
    char *p;
    uint32_t flags;
    uint32_t ustack=(uint32_t)tos;

    if(ustack & 3)
        return NULL;

    p = (char *)kmemalign(PAGE_SIZE, PAGE_SIZE);
    if(p == NULL)
        return NULL;

    new = (struct tcb *)p;

    memset(new, 0, sizeof(struct tcb));

    new->kstack = (uint32_t)(p+PAGE_SIZE);

    new->nice = 0;  // 新增初始化（直接跟在kstack之后）
    new->priority = 0;
    new->estcpu = 0;
    //第三次实验添加项

    new->tid = tid++;
    new->state = TASK_STATE_READY;
    new->timeslice = TASK_TIMESLICE_DEFAULT;
    new->wq_exit = NULL;
    new->next = NULL;
    new->signature = TASK_SIGNATURE;

    /*XXX - should be elsewhere*/
    new->fpu.cwd = 0x37f;
    new->fpu.twd = 0xffff;

    INIT_TASK_CONTEXT(ustack, new->kstack, func, pv);

    save_flags_cli(flags);
    add_task(new);
    restore_flags(flags);

    return new;
}

/**
 * 系统调用task_exit的执行函数
 *
 * 结束当前线程，code_exit是它的退出代码
 */
void sys_task_exit(int code_exit)
{
    uint32_t flags;

    save_flags_cli(flags);

    wake_up(&g_task_running->wq_exit, -1);

    g_task_running->code_exit = code_exit;
    g_task_running->state = TASK_STATE_ZOMBIE;

    if(g_task_own_fpu == g_task_running)
        g_task_own_fpu = NULL;

    schedule();
}

/**
 * 系统调用task_wait的执行函数
 *
 * 当前线程等待线程tid结束执行。
 * 如果pcode_exit不是NULL，用于保存线程tid的退出代码
 */
int sys_task_wait(int tid, int *pcode_exit)
{
    uint32_t flags;
    struct tcb *tsk;

    if(g_task_running == NULL)
        return -1;

    if(tid == 0)
        return -1;

    save_flags_cli(flags);

    if((tsk = get_task(tid)) == NULL) {
        restore_flags(flags);
        return -1;
    }

    if(tsk->state != TASK_STATE_ZOMBIE)
        sleep_on(&tsk->wq_exit);

    if(pcode_exit != NULL)
        *pcode_exit= tsk->code_exit;

    if(tsk->wq_exit == NULL) {
        remove_task(tsk);
        //printk("%d: Task %d reaped\r\n", sys_task_getid(), tsk->tid);
        restore_flags(flags);

        kfree(tsk);
        return 0;
    }

    restore_flags(flags);
    return 0;
}

/**
 * 系统调用task_getid的执行函数
 *
 * 获取当前线程的tid
 */
int sys_task_getid()
{
    return (g_task_running==NULL)?-1:g_task_running->tid;
}

/**
 * 系统调用task_yield的执行函数
 *
 * 当前线程主动放弃CPU，让调度器调度其他线程运行
 */
void sys_task_yield()
{
    uint32_t flags;
    save_flags_cli(flags);
    schedule();
    restore_flags(flags);
}

/**
 * 初始化多线程子系统
 */
void init_task()
{
    g_resched = 0;
    g_task_running = NULL;
    g_task_head = NULL;
    g_task_own_fpu = NULL;

    /*
     * 创建线程task0，即系统空闲线程
     */
    task0 = sys_task_create(NULL, NULL/*task0执行的函数将由run_as_task0填充*/, NULL);
}
