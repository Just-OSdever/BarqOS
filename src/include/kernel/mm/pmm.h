#ifndef  PMM_H
#define PMM_H

// includes
#include   <stdint.h>   // which contains the types
#include   <stdbool.h>  // Boalen values
#include   <limine.h>   // Limine header
#include   <hal.h>      // which contains printing and cls functions
#include   <strings.h>  // which contains memcpy

// block structure
typedef struct {
    uint64_t start_addr;
    uint64_t end_addr;
} Black_Blocks;


// Forward declarations
void pmm_init();                                //  the intializing function
uint64_t to_virt(uint64_t phys_addr);
uint64_t to_phys(uint64_t virt_addr);
void *pmm_alloc(size_t size);
void get_new_array_and_add(uint64_t start , uint64_t end);
Black_Blocks *find_collision(uint64_t start , uint64_t end);

// define the variables
extern Black_Blocks *Blocks;

// externs
extern uint64_t hhdm_offset;
extern volatile struct limine_hhdm_request hddm_request;
extern volatile struct limine_memmap_request memmap_request;

extern struct limine_hhdm_response *hhdm;       // Forward declaration
extern struct limine_memmap_response *memmap;   // Forward declaration

#endif