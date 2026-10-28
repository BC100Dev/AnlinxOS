#include <stddef.h>
#include <stdlib.h>

#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20
#define MAP_FAILED ((void *)-1)

#define HDR_MAGIC 0xA110CA7E

static void* sys_mmap(void* addr, size_t len, int prot, int flags, int fd, long off) {
    return (void*)SYSCALL6(__NR_mmap,
                           (long)addr, (long)len, (long)prot,
                           (long)flags, (long)fd, off);
}

static int sys_munmap(void* addr, size_t len) {
    return (int)SYSCALL2(__NR_munmap, (long)addr, (long)len);
}

typedef struct block {
    size_t size;
    unsigned int magic;
    int used;
    struct block* next;
} block_t;

#define ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define PTR_ALIGN (sizeof(void *))
#define HDR_SIZE ALIGN_UP(sizeof(block_t), PTR_ALIGN)

#define PAGE_SIZE 4096UL
#define MIN_CHUNK PAGE_SIZE

static block_t* free_list = NULL;

static block_t* payload_to_block(void* ptr) {
    return (block_t*)((char*)ptr - HDR_SIZE);
}

static void* block_to_payload(block_t* b) {
    return (char*)b + HDR_SIZE;
}

static void coalesce(void) {
    block_t* b = free_list;
    while (b && b->next) {
        char* end_of_b = (char*)b + HDR_SIZE + b->size;
        if (!b->used && !b->next->used && end_of_b == (char*)b->next) {
            b->size += HDR_SIZE + b->next->size;
            b->next = b->next->next;
        } else
            b = b->next;
    }
}

static block_t* alloc_chunk(size_t need) {
    size_t total = HDR_SIZE + need;
    if (total < MIN_CHUNK)
        total = MIN_CHUNK;
    else
        total = ALIGN_UP(total, PAGE_SIZE);

    void* mem = sys_mmap(NULL, total, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED)
        return NULL;

    block_t* b = (block_t*)mem;
    b->size = total - HDR_SIZE;
    b->magic = HDR_MAGIC;
    b->used = 0;
    b->next = NULL;
    return b;
}

static block_t* find_free(size_t size) {
    block_t* b = free_list;
    while (b) {
        if (!b->used && b->size >= size)
            return b;

        b = b->next;
    }

    return NULL;
}

static void split_block(block_t* b, size_t size) {
    size_t min_remainder = HDR_SIZE + PTR_ALIGN;
    if (b->size < size + min_remainder)
        return;

    block_t* tail = (block_t*)((char*)b + HDR_SIZE + size);
    tail->size = b->size - size - HDR_SIZE;
    tail->magic = HDR_MAGIC;
    tail->used = 0;
    tail->next = b->next;

    b->size = size;
    b->next = tail;
}

void* malloc(size_t size) {
    if (size == 0)
        return NULL;

    size = ALIGN_UP(size, PTR_ALIGN);
    block_t* b = find_free(size);
    if (!b) {
        b = alloc_chunk(size);
        if (!b)
            return NULL;

        b->next = free_list;
        free_list = b;
    }

    split_block(b, size);
    b->used = 1;
    return block_to_payload(b);
}

void free(void* ptr) {
    if (!ptr)
        return;

    block_t* b = payload_to_block(ptr);
    if (b->magic != HDR_MAGIC || !b->used)
        return;

    b->used = 0;
    coalesce();

    size_t total = HDR_SIZE + b->size;
    if (((unsigned long)b % PAGE_SIZE == 0) && (total % PAGE_SIZE == 0)) {
        if (free_list == b)
            free_list = b->next;
        else {
            block_t* prev = free_list;
            while (prev && prev->next != b)
                prev = prev->next;

            if (prev)
                prev->next = b->next;
        }
        sys_munmap(b, total);
    }
}

void* calloc(size_t nmemb, size_t size) {
    if (nmemb && size > (size_t)-1 / nmemb)
        return NULL;

    size_t total = nmemb * size;
    void* ptr = malloc(total);
    if (!ptr)
        return NULL;

    block_t* b = payload_to_block(ptr);
    if (b->used) {
        unsigned char* p = (unsigned char*)ptr;
        size_t n = total;
        while (n--)
            *p++ = 0;
    }

    return ptr;
}

void* realloc(void* ptr, size_t size) {
    if (!ptr)
        return malloc(size);

    if (size == 0) {
        free(ptr);
        return NULL;
    }

    block_t* b = payload_to_block(ptr);
    if (b->magic != HDR_MAGIC)
        return NULL;

    size = ALIGN_UP(size, PTR_ALIGN);

    if (b->size >= size)
        return ptr;

    void* newptr = malloc(size);
    if (!newptr)
        return NULL;

    const unsigned char* src = (const unsigned char*)ptr;
    unsigned char* dst = (unsigned char*)newptr;
    size_t copy = b->size < size ? b->size : size;
    while (copy--)
        *dst++ = *src++;

    free(ptr);
    return newptr;
}

void* reallocarray(void* ptr, size_t nmemb, size_t size) {
    if (nmemb && size > (size_t)-1 / nmemb)
        return NULL;

    return realloc(ptr, nmemb * size);
}

void* aligned_alloc(size_t alignment, size_t size) {
    if (!alignment || (alignment & (alignment - 1)))
        return NULL;
    if (size % alignment)
        return NULL;

    if (alignment <= PTR_ALIGN)
        return malloc(size);

    size_t rounded = ALIGN_UP(size, PTR_ALIGN);
    size_t raw_size = ALIGN_UP(HDR_SIZE + alignment - 1 + rounded, PAGE_SIZE);

    void* raw = sys_mmap(NULL, raw_size,
                         PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS,
                         -1, 0);
    if (raw == MAP_FAILED)
        return NULL;

    unsigned long base = (unsigned long)raw + HDR_SIZE;
    unsigned long aligned_payload =
        (base + alignment - 1) & ~(alignment - 1);

    block_t* b = (block_t*)(aligned_payload - HDR_SIZE);
    b->size = rounded;
    b->magic = HDR_MAGIC;
    b->used = 1;
    b->next = NULL;

    /*
     * We do NOT insert this into the free list, while the raw mmap base
     * and the block header don't coincide, so the normal munmap path
     * in free() won't work. For now these blocks leak on free().
     *
     * TODO: store the raw base + raw_size somewhere inside the
     * over-allocation region so free() can recover and munmap it.
     */
    return (void*)aligned_payload;
}

int posix_memalign(void** memptr, size_t alignment, size_t size) {
    if (!alignment || (alignment & (alignment - 1)))
        return 22;
    if (alignment < sizeof(void*))
        return 22;

    void* ptr = aligned_alloc(alignment, ALIGN_UP(size, alignment));
    if (!ptr)
        return 12;

    *memptr = ptr;
    return 0;
}
