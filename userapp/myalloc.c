/**
 * vim: filetype=c:fenc=utf-8:ts=4:et:sw=4:sts=4
 */
#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <syscall.h>
#include <stdio.h>

int MEM_SEM_ID;//定义内存信号量id，只要一个就行了

struct chunk {
    char signature[4];  /* "OSEX" */
    struct chunk *next; /* ptr. to next chunk */
    int state;          /* 0 - free, 1 - used */
#define FREE   0
#define USED   1

    int size;           /* size of this chunk */
};

static struct chunk *chunk_head;

void *g_heap;
void *tlsf_create_with_pool(uint8_t *heap_base, size_t heap_size)
{
    chunk_head = (struct chunk *)heap_base;
    strncpy(chunk_head->signature, "OSEX", 4);
    chunk_head->next = NULL;
    chunk_head->state = FREE;
    chunk_head->size  = heap_size;
    
    MEM_SEM_ID=sem_create(1);//初始化内存信号量
    return NULL;
}


// 合并空闲块（辅助函数）
static void merge_free_chunks(struct chunk *prev, struct chunk *current) {
    while (current != NULL && current->state == FREE) {
        prev->size += sizeof(struct chunk) + current->size;
        prev->next = current->next;
        current = current->next;
    }
}

void *malloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }
    sem_wait(MEM_SEM_ID);//获取信号量防止进程冲突（两个进程同时malloc你不就炸了吗）
    // 对齐到8字节边界 #可有可无或者随便改其他数
    size = (size + 7) & ~7;
    struct chunk *current = chunk_head;
    struct chunk *prev = NULL;

    while (current != NULL) {
        if (current->state == FREE && current->size >= size) {
            // 检查是否有足够的空间分割
            if (current->size > size + sizeof(struct chunk)) {
                // 分割块
                struct chunk *new_chunk = (struct chunk *)((uint8_t *)current + sizeof(struct chunk) + size);
                strncpy(new_chunk->signature, "OSEX", 4);
                new_chunk->next = current->next;
                new_chunk->state = FREE;
                new_chunk->size = current->size - size - sizeof(struct chunk);

                current->next = new_chunk;
                current->size = size;
            }

            current->state = USED;
            sem_signal(MEM_SEM_ID);//归还信号量，其他进程可以malloc了
            return (void *)((uint8_t *)current + sizeof(struct chunk));
        }
        prev = current;
        current = current->next;
    }
    sem_signal(MEM_SEM_ID);//归还信号量，其他进程可以malloc了
    return NULL; // 没有足够的空间
}

void free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }
    
    struct chunk *achunk = (struct chunk *)((uint8_t *)ptr - sizeof(struct chunk));

    // 验证有效性
    if (strncmp(achunk->signature, "OSEX", 4) != 0) {
        return; // 无效指针
    }
    sem_wait(MEM_SEM_ID);//获取信号量防止进程冲突
    achunk->state = FREE;

    // 合并相邻的空闲块
    struct chunk *current = chunk_head;
    struct chunk *prev = NULL;

    while (current != NULL) {
        if (current->state == FREE && prev != NULL && prev->state == FREE) {
            merge_free_chunks(prev, current);
            current = prev; // 重新检查合并后的块
        }
        prev = current;
        current = current->next;
    }
    sem_signal(MEM_SEM_ID);//归还信号量
}

void *calloc(size_t num, size_t size)
{
    size_t total_size = num * size;
    void *ptr = malloc(total_size);
    if (ptr != NULL) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

void *realloc(void *oldptr, size_t size)
{
    if (oldptr == NULL) {
        return malloc(size);
    }

    if (size == 0) {
        free(oldptr);
        return NULL;
    }

    // 验证有效性
    struct chunk *achunk = (struct chunk *)((uint8_t *)oldptr - sizeof(struct chunk));
    if (strncmp(achunk->signature, "OSEX", 4) != 0) {
        return NULL; // 无效指针
    }
    sem_wait(MEM_SEM_ID);//获取信号量防止进程冲突
    // 对齐到8字节边界
    size = (size + 7) & ~7;

    // 检查是否可以原地扩展
    if (achunk->size >= size) {
        // 如果新大小小于当前大小，可能需要分割
        if (achunk->size > size + sizeof(struct chunk)) {
            struct chunk *new_chunk = (struct chunk *)((uint8_t *)achunk + sizeof(struct chunk) + size);
            strncpy(new_chunk->signature, "OSEX", 4);
            new_chunk->next = achunk->next;
            new_chunk->state = FREE;
            new_chunk->size = achunk->size - size - sizeof(struct chunk);

            achunk->next = new_chunk;
            achunk->size = size;
        }
        sem_signal(MEM_SEM_ID);//归还信号量
        return oldptr;
    }

    // 检查是否可以合并后面的空闲块来满足需求
    if (achunk->next != NULL && achunk->next->state == FREE && 
        (achunk->size + sizeof(struct chunk) + achunk->next->size) >= size) {
        // 合并后面的空闲块
        achunk->size += sizeof(struct chunk) + achunk->next->size;
        achunk->next = achunk->next->next;
        
        // 合并后仍然太大，可以分割
        if (achunk->size > size + sizeof(struct chunk)) {
            struct chunk *new_chunk = (struct chunk *)((uint8_t *)achunk + sizeof(struct chunk) + size);
            strncpy(new_chunk->signature, "OSEX", 4);
            new_chunk->next = achunk->next;
            new_chunk->state = FREE;
            new_chunk->size = achunk->size - size - sizeof(struct chunk);

            achunk->next = new_chunk;
            achunk->size = size;
        }
        sem_signal(MEM_SEM_ID);//归还信号量
        return oldptr;
    }

    // 无法原地扩展，需要分配新内存并复制数据
    void *newptr = malloc(size);
    if (newptr != NULL) {
        size_t copy_size = (achunk->size < size) ? achunk->size : size;
        memcpy(newptr, oldptr, copy_size);
        free(oldptr);
    }
    sem_signal(MEM_SEM_ID);//归还信号量
    return newptr;
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
