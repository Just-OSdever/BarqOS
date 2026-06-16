#ifndef ML_PMM_H
#define ML_PMM_H

// Includes
#include <limine.h>

// Lists
extern uint64_t *block_pool_4kb  ;
extern uint64_t *block_pool_16kb ;
extern uint64_t *block_pool_24kb ;
extern uint64_t *block_pool_32kb ;
extern uint64_t *block_pool_64kb ;
extern uint64_t *block_pool_256kb;
extern uint64_t *block_pool_512kb;
extern uint64_t *block_pool_1mb  ;

// Types
typedef struct {
  struct page_node *next;
} page_node_t;

// Requests
extern uintptr_t hhdm_offset;
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;

// Forward declarations
void ml_init() ;

#endif