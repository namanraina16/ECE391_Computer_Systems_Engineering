#ifndef PIT_H
#define PIT_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "syscall.h"
#include "terminal.h"


/* Interrupt vector for keyboard and rtc */
#define PIT_IV          0x00
#define PIT_PORT        0x40
#define PIT_CMD         0x43
#define DEFAULT_HZ      100


void init_pit();
int pit_handler(); 
void scheduler();
extern int linkage_pit_handler();

#endif /* PIT_H */
