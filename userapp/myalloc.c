/**
 * vim: filetype=c:fenc=utf-8:ts=4:et:sw=4:sts=4
 */
//#include <sys/types.h>
//#include <string.h>
//#include <stdint.h>
//#include <unistd.h>
//#include <syscall.h>
//#include <stdio.h>
//
//struct chunk {
//    char signature[4];  /* "OSEX" */
//    struct chunk *next; /* ptr. to next chunk */
//    int state;          /* 0 - free, 1 - used */
//#define FREE   0
//#define USED   1
//
//    int size;           /* size of this chunk */
//};
//
//static struct chunk *chunk_head;
//
//void *g_heap;
//void *tlsf_create_with_pool(uint8_t *heap_base, size_t heap_size)
//{
//    chunk_head = (struct chunk *)heap_base;
//    strncpy(chunk_head->signature, "OSEX", 4);
//    chunk_head->next = NULL;
//    chunk_head->state = FREE;
//    chunk_head->size  = heap_size;
//
//    return NULL;
//}
//
//void *malloc(size_t size)
//{
//    return NULL;
//}
//
//void free(void *ptr)
//{
//}
//
//void *calloc(size_t num, size_t size)
//{
//    return NULL;
//}
//
//void *realloc(void *oldptr, size_t size)
//{
//    return NULL;
//}
#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <syscall.h>
#include <stdio.h>

/* 定义内存管理相关宏 */
#define MEM_ALIGNMENT       8
#define MEM_SIGNATURE      "OSEX"
#define MEM_SIGNATURE_SIZE 4
#define LOCK()             sem_wait(MEM_SEM_ID)
#define UNLOCK()           sem_signal(MEM_SEM_ID)

void* g_heap;
/* 内存块控制结构 */
struct chunk {
    char signature[MEM_SIGNATURE_SIZE];  /* 内存签名用于验证 */
    struct chunk* next;      /* 下一个内存块指针 */
    int state;               /* 块状态标记 */
    size_t size;             /* 当前块数据区大小 */
};

static struct chunk* chunk_head;  /* 内存块链表头 */
int MEM_SEM_ID;                   /* 内存操作信号量 */

/* 辅助函数声明 */
static void  init_chunk(struct chunk* chunk, size_t size, int state);
static int   is_valid_chunk(struct chunk* chunk);
static void  split_chunk(struct chunk* chunk, size_t size);
static void  merge_adjacent(struct chunk* chunk);
static struct chunk* find_prev_chunk(struct chunk* target);

/**
 * 内存池初始化函数
 * @param heap_base  堆内存起始地址
 * @param heap_size  堆内存总大小
 */
void* tlsf_create_with_pool(uint8_t* heap_base, size_t heap_size)
{
    chunk_head = (struct chunk*)heap_base;
    init_chunk(chunk_head, heap_size, 0);

    MEM_SEM_ID = sem_create(1);  // 初始化互斥信号量
    return NULL;
}

/**
 * 内存分配核心函数
 * @param size  请求分配的内存大小
 * @return 分配的内存指针，失败返回NULL
 */
void* malloc(size_t size)
{
    if (size == 0) return NULL;

    /* 内存对齐处理 */
    size = (size + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT - 1);

    LOCK();
    struct chunk* current = chunk_head;
    void* alloc_ptr = NULL;

    while (current) {
        if (current->state == 0 && current->size >= size) {
            /* 执行块分割 */
            split_chunk(current, size);
            current->state = 1;
            alloc_ptr = (void*)((uint8_t*)current + sizeof(struct chunk));
            break;
        }
        current = current->next;
    }

    UNLOCK();
    return alloc_ptr;
}

/**
 * 内存释放函数
 * @param ptr 要释放的内存指针
 */
void free(void* ptr)
{
    if (!ptr) return;

    struct chunk* chunk = (struct chunk*)((uint8_t*)ptr - sizeof(struct chunk));
    if (!is_valid_chunk(chunk)) return;

    LOCK();
    chunk->state = 0;

    /* 合并相邻空闲块 */
    merge_adjacent(chunk);
    UNLOCK();
}

/**
 * 清空内存分配函数
 * @param num  元素数量
 * @param size 单个元素大小
 */
void* calloc(size_t num, size_t size)
{
    size_t total = num * size;
    void* ptr = malloc(total);
    return ptr ? memset(ptr, 0, total) : NULL;
}

/**
 * 内存重分配函数
 * @param oldptr 原内存指针
 * @param size   新请求大小
 */
void* realloc(void* oldptr, size_t size)
{
    if (!oldptr) return malloc(size);
    if (size == 0) {
        free(oldptr);
        return NULL;
    }

    struct chunk* chunk = (struct chunk*)((uint8_t*)oldptr - sizeof(struct chunk));
    if (!is_valid_chunk(chunk)) return NULL;

    /* 新大小对齐处理 */
    size = (size + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT - 1);

    LOCK();
    void* newptr = NULL;

    /* 尝试原地扩展 */
    if (chunk->size >= size) {
        split_chunk(chunk, size);
        newptr = oldptr;
    }
    /* 尝试合并后扩展 */
    else if (chunk->next && !chunk->next->state &&
        (chunk->size + sizeof(struct chunk) + chunk->next->size) >= size) {
        chunk->size += sizeof(struct chunk) + chunk->next->size;
        chunk->next = chunk->next->next;
        split_chunk(chunk, size);
        newptr = oldptr;
    }
    /* 需要重新分配 */
    else {
        newptr = malloc(size);
        if (newptr) {
            memcpy(newptr, oldptr, chunk->size < size ? chunk->size : size);
            free(oldptr);
        }
    }

    UNLOCK();
    return newptr;
}

/*************** 辅助函数实现 ***************/

/* 初始化内存块结构 */
static void init_chunk(struct chunk* chunk, size_t size, int state)
{
    memcpy(chunk->signature, MEM_SIGNATURE, MEM_SIGNATURE_SIZE);
    chunk->size = size;
    chunk->state = state;
    chunk->next = NULL;
}

/* 验证内存块有效性 */
static int is_valid_chunk(struct chunk* chunk)
{
    return memcmp(chunk->signature, MEM_SIGNATURE, MEM_SIGNATURE_SIZE) == 0;
}

/* 内存块分割操作 */
static void split_chunk(struct chunk* chunk, size_t size)
{
    if (chunk->size - size <= sizeof(struct chunk)) return;

    struct chunk* new_chunk = (struct chunk*)((uint8_t*)chunk + sizeof(struct chunk) + size);
    init_chunk(new_chunk, chunk->size - size - sizeof(struct chunk), 0);
    new_chunk->next = chunk->next;
    chunk->next = new_chunk;
    chunk->size = size;
}

/* 相邻块合并操作 */
static void merge_adjacent(struct chunk* chunk)
{
    /* 向前合并 */
    struct chunk* prev = find_prev_chunk(chunk);
    if (prev && prev->state == 0) {
        prev->size += sizeof(struct chunk) + chunk->size;
        prev->next = chunk->next;
        chunk = prev;
    }

    /* 向后合并 */
    if (chunk->next && chunk->next->state == 0) {
        chunk->size += sizeof(struct chunk) + chunk->next->size;
        chunk->next = chunk->next->next;
    }
}

/* 查找前驱块 */
static struct chunk* find_prev_chunk(struct chunk* target)
{
    struct chunk* current = chunk_head, * prev = NULL;
    while (current && current != target) {
        prev = current;
        current = current->next;
    }
    return prev;
}




/*************D O  N O T  T O U C H  A N Y T H I N G  B E L O W*************/
static void tsk_malloc(void *pv)
{
  int i, c = (int)pv;
  char **a = malloc(c*sizeof(char *));
  for(i = 0; i < c; i++) {
	  a[i]=malloc(i+1);
	  a[i][i]=17;
  }
  for(i = 0; i < c; i++) {
	  free(a[i]);
  }
  free(a);

  task_exit(0);
}

#define MESSAGE(foo) printf("%s, line %d: %s", __FILE__, __LINE__, foo)
void test_allocator()
{
  char *p, *q, *t;

  MESSAGE("[1] Test malloc/free for unusual situations\r\n");

  MESSAGE("  [1.1]  Allocate small block ... ");
  p = malloc(17);
  if (p == NULL) {
    printf("FAILED\r\n");
	return;
  }
  p[0] = p[16] = 17;
  printf("PASSED\r\n");

  MESSAGE("  [1.2]  Allocate big block ... ");
  q = malloc(4711);
  if (q == NULL) {
    printf("FAILED\r\n");
	return;
  }
  q[4710] = 47;
  printf("PASSED\r\n");

  MESSAGE("  [1.3]  Free big block ... ");
  free(q);
  printf("PASSED\r\n");

  MESSAGE("  [1.4]  Free small block ... ");
  free(p);
  printf("PASSED\r\n");

  MESSAGE("  [1.5]  Allocate huge block ... ");
  q = malloc(32*1024*1024-sizeof(struct chunk));
  if (q == NULL) {
    printf("FAILED\r\n");
	return;
  }
  q[32*1024*1024-sizeof(struct chunk)-1]=17;
  free(q);
  printf("PASSED\r\n");

  MESSAGE("  [1.6]  Allocate zero bytes ... ");
  if ((p = malloc(0)) != NULL) {
    printf("FAILED\r\n");
	return;
  }
  printf("PASSED\r\n");

  MESSAGE("  [1.7]  Free NULL ... ");
  free(p);
  printf("PASSED\r\n");

  MESSAGE("  [1.8]  Free non-allocated-via-malloc block ... ");
  int arr[5] = {0x55aa4711,0x5a5a1147,0xa5a51471,0xaa551741,0x5aa54171};
  free(&arr[4]);
  if(arr[0] == 0x55aa4711 &&
     arr[1] == 0x5a5a1147 &&
	 arr[2] == 0xa5a51471 &&
	 arr[3] == 0xaa551741 &&
	 arr[4] == 0x5aa54171) {
	  printf("PASSED\r\n");
  } else {
	  printf("FAILED\r\n");
	  return;
  }

  MESSAGE("  [1.9]  Various allocation pattern ... ");
  int i;
  size_t pagesize = sysconf(_SC_PAGESIZE);
  for(i = 0; i < 7411; i++){
    p = malloc(pagesize);
	p[pagesize-1]=17;
    q = malloc(pagesize * 2 + 1);
	q[pagesize*2]=17;
    t = malloc(1);
	t[0]=17;
    free(p);
    free(q);
    free(t);
  }

  char **a = malloc(2741*sizeof(char *));
  for(i = 0; i < 2741; i++) {
	  a[i]=malloc(i+1);
	  a[i][i]=17;
  }
  for(i = 0; i < 2741; i++) {
	  free(a[i]);
  }
  free(a);

  if(chunk_head->next != NULL || chunk_head->size != 32*1024*1024) {
	printf("FAILED\r\n");
	return;
  }
  printf("PASSED\r\n");

  MESSAGE("  [1.10] Allocate using calloc ... ");
  int *x = calloc(17, 4);
  for(i = 0; i < 17; i++)
	  if(x[i] != 0) {
		  printf("FAILED\r\n");
		  return;
	  } else
	      x[i] = i;
  free(x);
  printf("PASSED\r\n");

  MESSAGE("[2] Test realloc() for unusual situations\r\n");

  MESSAGE("  [2.1]  Allocate 17 bytes by realloc(NULL, 17) ... ");
  p = realloc(NULL, 17);
  if (p == NULL) {
    printf("FAILED\r\n");
	return;
  }
  p[0] = p[16] = 17;
  printf("PASSED\r\n");
  MESSAGE("  [2.2]  Increase size by realloc(., 4711) ... ");
  p = realloc(p, 4711);
  if (p == NULL) {
    printf("FAILED\r\n");
	return;
  }
  if ( p[0] != 17 || p[16] != 17 ) {
    printf("FAILED\r\n");
	return;
  }
  p[4710] = 47;
  printf("PASSED\r\n");

  MESSAGE("  [2.3]  Decrease size by realloc(., 17) ... ");
  p = realloc(p, 17);
  if (p == NULL) {
    printf("FAILED\r\n");
	return;
  }
  if ( p[0] != 17 || p[16] != 17 ) {
    printf("FAILED\r\n");
	return;
  }
  printf("PASSED\r\n");

  MESSAGE("  [2.4]  Free block by realloc(., 0) ... ");
  p = realloc(p, 0);
  if (p != NULL) {
	printf("FAILED\r\n");
    return;
  } else
	printf("PASSED\r\n");

  MESSAGE("  [2.5]  Free block by realloc(NULL, 0) ... ");
  p = realloc(realloc(NULL, 0), 0);
  if (p != NULL) {
    printf("FAILED\r\n");
    return;
  } else
	printf("PASSED\r\n");

  MESSAGE("[3] Test malloc/free for thread-safe ... ");

  int t1, t2;
  char *s1 = malloc(1024*1024),
       *s2 = malloc(1024*1024);
  t1=task_create(s1+1024*1024, tsk_malloc, (void *)5000);
  t2=task_create(s2+1024*1024, tsk_malloc, (void *)5000);
  task_wait(t1, NULL);
  task_wait(t2, NULL);
  free(s1);
  free(s2);

  if(chunk_head->next != NULL || chunk_head->size != 32*1024*1024) {
	printf("FAILED\r\n");
	return;
  }
  printf("PASSED\r\n");
}
/*************D O  N O T  T O U C H  A N Y T H I N G  A B O V E*************/
