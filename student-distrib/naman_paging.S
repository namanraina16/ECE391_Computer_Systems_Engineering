#define ASM     1

.text
.globl loadPageDirectory
.globl enablePaging
.globl dump_contents
.globl flush_tlb

//0xB8000

//set for %CR4 bit 4 (5th bit) to 1 -->inside enable function
loadPageDirectory:
    push %ebp
    mov %esp, %ebp
    mov 8(%esp), %eax
    mov %eax, %cr3
    leave
    ret

enablePaging:
    push %ebp
    mov %esp, %ebp
    mov %cr4, %eax
    or $0x10, %eax
    mov %eax, %cr4
    xor %eax,%eax
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    leave
    ret

dump_contents:
    push %ebp
    mov %esp, %ebp
    mov %cr2, %eax
    leave
    ret


flush_tlb:
    mov %cr3,%eax
    mov %eax,%cr3
    ret







