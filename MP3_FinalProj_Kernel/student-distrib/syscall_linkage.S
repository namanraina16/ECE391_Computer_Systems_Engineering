
#define ASM 1

.text
.globl sys_call_handler

/* macro to do linkage */
sys_call_handler:
    pushfl
    pushl %edi
    pushl %esi
    pushl %ebp
    pushl %esp
    pushl %edx
    pushl %ecx
    pushl %ebx
                      ;\
    cmpl $1, %eax
    jl sys_call_failed
    cmpl $11, %eax
    jge sys_call_failed
    addl $-1, %eax
    call *jumptable(,%eax,4) 
    jmp syscall_pops
sys_call_failed:
    movl $-1, %eax
syscall_pops:    
                       ;\
    popl %ebx
    popl %ecx
    popl %edx
    addl $4, %esp
    popl %ebp
    popl %esi
    popl %edi
    popfl
    iret

jumptable:
    .long sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
.globl haltVoid
haltVoid:
    pushl %ebp
    movl %esp, %ebp
    xorl %ebx, %ebx         # clear
    movl 8(%ebp), %esp      # esp
    movl 16(%ebp), %eax     # status
    movl 12(%ebp), %ebp     # ebp
    # movl %ebx, %esp
    leave
    ret


