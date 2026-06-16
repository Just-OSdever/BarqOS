#include <ml_pmm.h>

// Lists
page_node_t *block_pool_4kb   =  NULL;  
page_node_t *block_pool_16kb  =  NULL;  
page_node_t *block_pool_24kb  =  NULL;  
page_node_t *block_pool_32kb  =  NULL;  
page_node_t *block_pool_64kb  =  NULL;  
page_node_t *block_pool_256kb =  NULL;  
page_node_t *block_pool_512kb =  NULL;  
page_node_t *block_pool_1mb   =  NULL;  
// bigger than that will work with buddy allocator
// we will use slap allocator if the size is smaller than
// 1 megabyte but not in the ML sizes

// sizes:
uint64_t PAGE_SIZE = 4096;

static inline page_node_t *to_virt(uintptr_t phys_addr) {
  return (page_node_t *)(phys_addr + hhdm_offset);
}

void ml_init() {
  // here we will
  if (memmap_request.response == NULL || hhdm_request.response == NULL) {
    hal_print("memmap or hhdm response is NULL, Our mem allocator failed",
              0xff0000, 1);
    while (1)
      ;
  }
  hhdm_offset = hhdm_request.response->offset;
  struct limine_memmap_response *memmap = memmap_request.response;
  // lets make a loop in the memory
  for (uint64_t i = 0; i < memmap->entry_count; i++) {
    struct limine_memmap_entry *entry = memmap->entries[i];

    if (entry->type == LIMINE_MEMMAP_USABLE) {
      uint64_t current_addr  = entry->base   ;
      uint64_t remaining_len = entry->length ;
      uint64_t total_pages = entry->length / 4096  ;
      // we will make the protected ammounts
      uint64_t count_4kb = ((total_pages * 30) / 100) * PAGE_SIZE;
      // for safty only
      if (count_4kb == 0 && remaining_len >= PAGE_SIZE) {
        count_4kb = PAGE_SIZE;
      }
    }
  }
}