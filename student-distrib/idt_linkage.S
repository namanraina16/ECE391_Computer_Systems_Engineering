
#define ASM 1

/* macro to do linkage */
#define INTR_LINK(name, func)    \
    .globl name                 ;\
    name:                       ;\
        pushal                  ;\
        pushfl                  ;\
        call func               ;\
        popfl                   ;\
        popal                   ;\
        iret

/* linking exceptions */
INTR_LINK(linkage_exp_DE, exp_DE)      
INTR_LINK(linkage_exp_DB, exp_DB)  
INTR_LINK(linkage_exp_NMI, exp_NMI) 
INTR_LINK(linkage_exp_BP, exp_BP)  
INTR_LINK(linkage_exp_OF, exp_OF)  
INTR_LINK(linkage_exp_BR, exp_BR)  
INTR_LINK(linkage_exp_UD, exp_UD)  
INTR_LINK(linkage_exp_NM, exp_NM)  
INTR_LINK(linkage_exp_DF, exp_DF)  
INTR_LINK(linkage_exp_R9, exp_R9)  
INTR_LINK(linkage_exp_TS, exp_TS)  
INTR_LINK(linkage_exp_NP, exp_NP)  
INTR_LINK(linkage_exp_SS, exp_SS)  
INTR_LINK(linkage_exp_GP, exp_GP)  
INTR_LINK(linkage_exp_PF, exp_PF)  
INTR_LINK(linkage_exp_R15, exp_R15) 
INTR_LINK(linkage_exp_MF, exp_MF)  
INTR_LINK(linkage_exp_AC, exp_AC)  
INTR_LINK(linkage_exp_MC, exp_MC)  
INTR_LINK(linkage_exp_XF, exp_XF)  

/* linking keyboard and rtc */
INTR_LINK(linkage_keyboard_handler, keyboard_handler)
INTR_LINK(linkage_rtc_handler, rtc_handler)
INTR_LINK(linkage_pit_handler, pit_handler)
