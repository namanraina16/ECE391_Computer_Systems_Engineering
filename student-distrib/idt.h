#ifndef _IDT_H
#define _IDT_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "rtc.h"
#include "syscall.h"
#include "pit.h"


#define DIV_BY_ZERO 0x00
#define PIC_OFFSET 0x20

// Initializes the IDT and sets all the entries of the IDT -- exceptions, interrupts, and eventually system calls
void init_idt();

// exception handlers and its linkage handlers
int exp_DE();  extern int linkage_exp_DE();    
int exp_DB();  extern int linkage_exp_DB();
int exp_NMI(); extern int linkage_exp_NMI();
int exp_BP();  extern int linkage_exp_BP();
int exp_OF();  extern int linkage_exp_OF();
int exp_BR();  extern int linkage_exp_BR();
int exp_UD();  extern int linkage_exp_UD();
int exp_NM();  extern int linkage_exp_NM();
int exp_DF();  extern int linkage_exp_DF();
int exp_R9();  extern int linkage_exp_R9();
int exp_TS();  extern int linkage_exp_TS();
int exp_NP();  extern int linkage_exp_NP();
int exp_SS();  extern int linkage_exp_SS();
int exp_GP();  extern int linkage_exp_GP();
int exp_PF();  extern int linkage_exp_PF();
int exp_R15(); extern int linkage_exp_R15();
int exp_MF();  extern int linkage_exp_MF();
int exp_AC();  extern int linkage_exp_AC();
int exp_MC();  extern int linkage_exp_MC();
int exp_XF();  extern int linkage_exp_XF();

#endif /* _IDT_H */
