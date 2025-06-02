#ifndef MEMORY_TRACKER_H
#define MEMORY_TRACKER_H

#include <stddef.h> // Dla size_t

// Deklaracje globalnych liczników pamięci
extern size_t g_current_memory_usage;
extern size_t g_peak_memory_usage;
extern size_t g_total_allocations;
extern size_t g_total_deallocations;

// Funkcje do zarządzania trackerem
void mem_tracker_init(void);
void mem_tracker_reset_peak(void);
void mem_tracker_reset_all(void);
void mem_tracker_print_stats(const char* context);

// Deklaracje funkcji opakowujących (te będą implementowane w memory_tracker.c)
// Te funkcje *nie* powinny być bezpośrednio wołane przez użytkownika,
// tylko przez poniższe makra.
void* my_malloc_proxy(size_t size, const char* file, int line);
void  my_free_proxy(void* ptr, const char* file, int line);
void* my_calloc_proxy(size_t num, size_t size_element, const char* file, int line);

// Makra do zastępowania standardowych wywołań
// Tylko jeśli MEMORY_TRACKING_ENABLED jest zdefiniowane
// To pozwala łatwo włączać/wyłączać śledzenie
#ifdef MEMORY_TRACKING_ENABLED
#define malloc(size) my_malloc_proxy(size, __FILE__, __LINE__)
#define free(ptr)    my_free_proxy(ptr, __FILE__, __LINE__)
#define calloc(num, size_element) my_calloc_proxy(num, size_element, __FILE__, __LINE__)
#else
// Jeśli śledzenie jest wyłączone, upewnij się, że stdlib.h jest dołączony
// w plikach, które używają malloc/free/calloc.
// Common.h dołącza stdlib.h, więc to powinno być ok.
// Alternatywnie, można jawnie dołączyć stdlib.h tutaj, jeśli MEMORY_TRACKING_ENABLED nie jest zdefiniowane.
// #include <stdlib.h> // Ale to może prowadzić do wielokrotnego dołączania, jeśli nie ma include guards w stdlib.h
#endif


#endif // MEMORY_TRACKER_H