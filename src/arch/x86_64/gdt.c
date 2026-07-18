#include <gdt.h>

typedef struct {
    uint16_t Limitlow;          // limit (bits 00 - 15)
    uint16_t BaseLow;           // base  (bits 00 - 15)
    uint8_t BaseMiddle;         // base  (bits 16 - 23)
    uint8_t Access;             // access (bits 00 - 07)
    uint8_t FlagsLimitHigh;     // limit (bits 16 - 19) | flags (bit 0 - 3) 
    uint8_t BaseHigh;           // base (bits 24 - 31)
    // TO KNOW MORE SEE : https://wiki.osdev.org/Global_Descriptor_Table#Long_Mode_System_Segment_Descriptor

}__attribute__((packed)) GDT_ENTRY;

//TSS ENTRY
typedef struct {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
} __attribute__((packed)) TSS_ENTRY;

//TSS descriptor
typedef struct {
    uint16_t Limitlow;        
    uint16_t BaseLow;         
    uint8_t BaseMiddle;       
    uint8_t Access;           
    uint8_t FlagsLimitHigh;   
    uint8_t BaseHigh;         
    uint32_t BaseUpper32;
    uint32_t Reserved;        
} __attribute__((packed)) GDT_TSS_ENTRY;


TSS_ENTRY my_tss;

typedef struct {                 // that's what we'll load
    uint16_t Limit;             // sizeof(gdt) - 1
    uint64_t Base;             // base addres of GDT
}__attribute__((packed)) GDTR;


// Helping macros

#define GET_LIMITE_LOW(limit)                               (limit & 0xffff)        // get the first 16-bits from limit
#define GET_BASE_LOW(base)                                  (base  & 0xffff)        // get the first 16-bits from base
#define GET_BASE_MIDDLE(base)                               ((base >> 16) & 0xff)   // get the mid 8-bits from base
#define GET_FlagsLimitHigh(flags , limit)                   ((limit >> 16) & 0xf) | ((flags << 4)& 0xf0)
#define GET_BaseHigh(base)                                  ((base >> 24) & 0xff)   // get the last 8-bits from base


#define SET_GDT_ENTRY(base , limit , access , flags) { \
    GET_LIMITE_LOW(limit),                              \
    GET_BASE_LOW(base),                                  \
    GET_BASE_MIDDLE(base),                                \
    access,                                                \
    GET_FlagsLimitHigh(flags , limit),                      \
    GET_BaseHigh(base)                                       \
}


GDT_ENTRY GDT[] = {
    SET_GDT_ENTRY(0 , 0 , 0, 0),             // Null (0x00) -> bytes 0-7
    SET_GDT_ENTRY(0 , 0xFFFFF , 0x9A , 0xA), // K Code (0x08) -> bytes 8-15
    SET_GDT_ENTRY(0 , 0xFFFFF , 0x92 , 0xC), // K Data (0x10) -> bytes 16-23
    SET_GDT_ENTRY(0 , 0xFFFFF , 0xF2 , 0xC), // U Data (0x18) -> bytes 24-31
    SET_GDT_ENTRY(0 , 0xFFFFF , 0xFA , 0xA), // U Code (0x20) -> bytes 32-39
    
    SET_GDT_ENTRY(0 , 0 , 0, 0),             // TSS Low (0x28) -> bytes 40-47
    SET_GDT_ENTRY(0 , 0 , 0, 0)              // TSS High (0x30) -> bytes 48-55
};

GDTR gdt_descriptor = { sizeof(GDT) - 1, (uint64_t)GDT};

void gdt_set_tss(uint64_t tss_address) {
    uint32_t limit = sizeof(TSS_ENTRY) - 1;

    GDT_TSS_ENTRY* tss_desc = (GDT_TSS_ENTRY*)&GDT[5];

    tss_desc->Limitlow = GET_LIMITE_LOW(limit);
    tss_desc->BaseLow = GET_BASE_LOW(tss_address);
    tss_desc->BaseMiddle = GET_BASE_MIDDLE(tss_address);
    tss_desc->Access = 0x89;
    tss_desc->FlagsLimitHigh = GET_FlagsLimitHigh(0x0, limit);
    tss_desc->BaseHigh = GET_BaseHigh(tss_address);

    tss_desc->BaseUpper32 = (uint32_t)(tss_address >> 32);
    tss_desc->Reserved = 0;
}

void gdt_load(GDTR* descriptor , uint16_t code_segment , uint16_t data_segement);

void gdt_init() {
    gdt_set_tss((uint64_t)&my_tss);
    gdt_load(&gdt_descriptor , 0x08 , 0x10);
}