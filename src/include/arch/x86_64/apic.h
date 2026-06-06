#ifndef APIC_H
#define APIC_H

#include <stdint.h>
#include "irq.h"
#include "pic.h"
#include "msr.h"
#include "port.h"
#include "screen.h"


extern uint32_t detect_apic();
extern uint64_t cpu_get_apic_base();
void enable_apic() ;
void APIC_Intialize();

#endif