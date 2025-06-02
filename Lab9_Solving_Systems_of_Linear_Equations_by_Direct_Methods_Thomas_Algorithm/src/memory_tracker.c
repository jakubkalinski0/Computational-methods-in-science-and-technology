// Plik: src/memory_tracker.c

#include <stdio.h>  // Dla fprintf
#include <string.h> // Dla memset
#include <stdlib.h> // Dołącz stdlib.h NAJPIERW, aby uzyskać oryginalne funkcje

// Zdefiniuj aliasy ZANIM dołączysz memory_tracker.h, który nadpisze malloc itp.
static void* (*system_malloc)(size_t) = malloc;
static void  (*system_free)(void*)     = free;
// static void* (*system_calloc)(size_t, size_t) = calloc; // Nie jest używany bezpośrednio, my_calloc_proxy używa my_malloc_proxy


// Teraz dołączamy nasz nagłówek, który zdefiniuje makra, jeśli MEMORY_TRACKING_ENABLED
#include "memory_tracker.h"

// Definicje globalnych liczników
size_t g_current_memory_usage = 0;
size_t g_peak_memory_usage = 0;
size_t g_total_allocations = 0;
size_t g_total_deallocations = 0;

#define HEADER_SIZE sizeof(size_t)

void mem_tracker_init(void) {
    g_current_memory_usage = 0;
    g_peak_memory_usage = 0;
    g_total_allocations = 0;
    g_total_deallocations = 0;
}

void mem_tracker_reset_peak(void) {
    // Peak jest resetowany do obecnego użycia, bo nie może być mniejszy
    g_peak_memory_usage = g_current_memory_usage;
}

void mem_tracker_reset_all(void) {
    mem_tracker_init(); // To samo co init
}

void mem_tracker_print_stats(const char* context) {
    fprintf(stdout, "MEM_STATS (%s): Current Usage: %zu B, Peak Usage: %zu B, Total Allocs: %zu, Total Frees: %zu\n",
            context, g_current_memory_usage, g_peak_memory_usage, g_total_allocations, g_total_deallocations);
    if (g_total_allocations != g_total_deallocations && strcmp(context,"PROGRAM_END")==0) {
        fprintf(stderr, "MEM_WARN (%s): Mismatch in allocations (%zu) and deallocations (%zu)! Memory leak likely.\n",
                context, g_total_allocations, g_total_deallocations);
    }
     if (g_current_memory_usage > 0 && strcmp(context,"PROGRAM_END")==0 ) {
         fprintf(stderr, "MEM_WARN (%s): Current usage > 0 (%zu B) at program end.\n", context, g_current_memory_usage);
    }
}

void* my_malloc_proxy(size_t size, const char* file, int line) {
    if (size == 0) {
        return NULL;
    }
    // Używamy naszego aliasu do oryginalnego malloc
    void* ptr_with_header = system_malloc(HEADER_SIZE + size);

    if (!ptr_with_header) {
        fprintf(stderr, "MY_MALLOC_PROXY_ERROR: System allocation (system_malloc) failed at %s:%d for size %zu\n", file, line, size);
        return NULL;
    }

    *(size_t*)ptr_with_header = size;
    g_current_memory_usage += size;
    if (g_current_memory_usage > g_peak_memory_usage) {
        g_peak_memory_usage = g_current_memory_usage;
    }
    g_total_allocations++;
    return (char*)ptr_with_header + HEADER_SIZE;
}

void my_free_proxy(void* ptr, const char* file, int line) {
    if (!ptr) {
        return;
    }
    void* ptr_with_header = (char*)ptr - HEADER_SIZE;
    size_t size = *(size_t*)ptr_with_header;

    if (g_current_memory_usage < size) {
        fprintf(stderr, "MY_FREE_PROXY_ERROR: Current usage (%zu) < freed size (%zu) at %s:%d. Potential double free or corruption.\n",
                g_current_memory_usage, size, file, line);
    } else {
        g_current_memory_usage -= size;
    }
    g_total_deallocations++;
    // Używamy naszego aliasu do oryginalnego free
    system_free(ptr_with_header);
}

void* my_calloc_proxy(size_t num, size_t size_element, const char* file, int line) {
    if (num == 0 || size_element == 0) {
        return NULL;
    }
    size_t total_size = num * size_element;
    if (size_element != 0 && total_size / size_element != num) {
        fprintf(stderr, "MY_CALLOC_PROXY_ERROR: Integer overflow calculating total size (%zu * %zu) at %s:%d\n",
                num, size_element, file, line);
        return NULL;
    }

    void* ptr = my_malloc_proxy(total_size, file, line);
    if (ptr) {
        memset(ptr, 0, total_size); // Oryginalny memset, ptr jest już wskaźnikiem na obszar użytkownika
    }
    return ptr;
}