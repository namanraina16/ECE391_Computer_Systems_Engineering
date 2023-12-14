#define ASM 1

.globl linkage_sched
linkage_sched:
    movl 4(%esp), %ebp
    leave
    ret
