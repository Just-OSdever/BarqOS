/*  Note:
    Memory range: [start_addr, end_addr)
*/



#include   <pmm.h>

// define our externs
struct limine_memmap_response *mmap_resp = NULL;
struct limine_hhdm_response *hhdm_resp = NULL;

// variables define
uint64_t block_size = sizeof(Black_Blocks); //Bytes
uint64_t nonusable_blocks_num = 0;
uintptr_t array_addr = 0;
bool found_addr = false;
bool found = false;
uintptr_t cursor;
Black_Blocks *my_array = NULL;
static Black_Blocks *nothing;
extern ticks;

// Virsual and Physical battle
uint64_t to_virt(uint64_t phys_addr) {
    return phys_addr + hhdm_offset;
}

uint64_t to_phys(uint64_t virt_addr) {
    return virt_addr - hhdm_offset;
}

// Intialize
void pmm_init() {
    // Get responces
    mmap_resp = memmap_request.response;
    hhdm_resp = hhdm_request.response;
    hhdm_offset = hhdm_resp->offset;
    // check them
    if (mmap_resp == NULL || hhdm_resp == NULL) {
        // panic
        cls(0xff0000);
        hal_print("An error acured while getting the Memory Map or the HigherHalf Direct Map from your device!" , 0xffffff , 2);
        // just hang
        while(1) {
            __asm__ volatile ("hlt");
        }
    }
    
    // Now let's get the black-Blocks number
    uint64_t entry_count = mmap_resp->entry_count;
    for(uint64_t i = 0 ; i < entry_count ; i++){
        struct limine_memmap_entry *entry = mmap_resp->entries[i];
        if (entry->type != LIMINE_MEMMAP_USABLE) {
            nonusable_blocks_num++ ;
        }
    }
    nonusable_blocks_num++;
    hal_print("\n" , 0xffffff , 1);
    hal_print_dec((nonusable_blocks_num) , 0xffff00 , 1);
    hal_print("\n" , 0xffffff , 1);
    uint64_t required_bytes = (nonusable_blocks_num) * block_size;

    // Now let's get place for the array
    uint64_t array_index = 0;
    for(uint64_t i = 0 ; i < entry_count ; i++){
        struct limine_memmap_entry *entry = mmap_resp->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            if (entry->length >= required_bytes) {
                // Get the addr
                array_addr= entry->base;
                hal_print("\nstart addr is:" , 0xffffff , 1);
                hal_print_hex(array_addr , 0xffffff , 1);
                hal_print("\n" , 0xffffff , 1);
                array_index = i;
                found_addr = true;
                break;
            }
        }
    }

    // Check the addr
    if (found_addr == false) {
        hal_cls(0xff0000);
        hal_print("\nNo free RAM for putting memory allocator info\n" , 0xffffff , 1);
        hal_print("System had to halt, PLZ buy more RAM." , 0xffaf00 , 1);
        while(1) {
            __asm__ ("hlt");
        }
    }

    // If ok, put the array in
    my_array = (Black_Blocks *)(to_virt( (uintptr_t)array_addr));

    // Let's fill the array
    uint64_t n = 0;
    for(uint64_t i = 0 ; i < entry_count ; i++){
        struct limine_memmap_entry *entry = mmap_resp->entries[i];
        if (array_index == i) {
            if(n < (nonusable_blocks_num)) {
                // Fill info
                my_array[n].start_addr = array_addr;
                my_array[n].end_addr = (array_addr) + (required_bytes);
                hal_print("Black list [" , 0xffffff , 1);
                hal_print_dec(n , 0xffffff , 1);
                hal_print("] is our Array start addr is: " , 0xffffff , 1);
                hal_print_hex(array_addr , 0xffffff , 1);
                hal_print("\n" , 0xffffff , 1);
                n++;
            }
        }
        if (entry->type != LIMINE_MEMMAP_USABLE) {
            if(n < (nonusable_blocks_num)) {
                // Fill info
                my_array[n].start_addr = entry->base;
                my_array[n].end_addr = (entry->base) + (entry->length);
                hal_print("Black list [" , 0xffffff , 1);
                hal_print_dec(n , 0xffffff , 1);
                hal_print("] start addr is: " , 0xffffff , 1);
                hal_print_hex(entry->base , 0xffffff , 1);
                hal_print("\n" , 0xffffff , 1);
                n++;
            }
        }
    }

    // Let's put our cursor after the first block!
    cursor = 0;
    hal_print_hex(cursor , 0xffffff , 1);
}

void OOM() {
    hal_cls(0xff0000);
    hal_print("OOM" , 0xffffff , 2);
    hal_print(" - Out Of Memory\n" , 0xffffff , 1);
    hal_print("System tried a lot times So:\nSystem had to halt" , 0xffffff , 1);
    while(1) {
        __asm__ volatile ("hlt");
    }
}

void *pmm_alloc(size_t size) {
    uintptr_t start = cursor;
    uintptr_t end = start + (uintptr_t)size;
    for(uint64_t tries = 0; tries < (nonusable_blocks_num * 2) + 1 ; tries++) {
        Black_Blocks *hit = find_collision((uint64_t)start , (uint64_t)end);
        if (hit != NULL) {
            start = hit->end_addr;
            end = start + (uintptr_t)size;   // just refrsh
            continue;
        }
        else {
            // Put it in our-array
            get_new_array_and_add(start , end);
            // alloc now! 
            hal_print("\n Allocated in: 0x" , 0x00f0f0 , 1);
            hal_print_hex( start , 0xffffff , 1);
            cursor = end;
            return (void *)to_virt(start);
        }
    }
    OOM();
}

Black_Blocks *find_collision(uint64_t start , uint64_t end) {
    for (uint64_t i = 0; i < nonusable_blocks_num ; i++) {
        if (start < my_array[i].end_addr && end > my_array[i].start_addr) {
            // This makes big noise
            // hal_print("\n Collision \n" , 0xff0000 , 1);
            return &my_array[i];
            break;
        }
    }
    return nothing;
}

void get_new_array_and_add(uint64_t start , uint64_t end) {
    // Check a place that takes all the array
    
    // Now let's add the black-Blocks number
    found = false;
    uint64_t old_size = (nonusable_blocks_num) * block_size;
    nonusable_blocks_num++;
    uint64_t required_bytes = (nonusable_blocks_num) * block_size;

    // Now let's get place for the array
    uintptr_t old_array_addr = 0;
    uint64_t block_start = 0;
    uint64_t end_of_block = block_start + required_bytes;

    for(uint64_t tries = 0; tries < (nonusable_blocks_num * 2) + 1 ; tries++) {
        Black_Blocks *hit = find_collision(block_start , end_of_block);
        if (hit != nothing) {
            block_start = hit->end_addr;
            end_of_block = block_start + required_bytes;    // refresh
            continue;
        }
        else {
            // Got a place!
            found = true;
            old_array_addr = array_addr;
            array_addr = block_start;
            // STOP
            break;
        }
    }
    
    // Check the addr
    if (found == false) {
        hal_cls(0xff0000);
        hal_print("\nNo free RAM for putting memory allocator info\n" , 0xffffff , 1);
        hal_print("System had to halt, PLZ buy more RAM." , 0xffaf00 , 1);
        while(1) {
            __asm__ ("hlt");
        }
    }
    
    // If ok, put the array in
    my_array = (Black_Blocks *)(to_virt(array_addr));

    // Copy info
    memcpy((void *)to_virt(array_addr) , (void *)to_virt(old_array_addr) , (size_t)old_size);

    // Add the start and end
    my_array[nonusable_blocks_num - 1].start_addr = start;
    my_array[nonusable_blocks_num - 1].end_addr = end;
}