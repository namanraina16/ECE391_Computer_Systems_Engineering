#ifndef _RTC_H
#define _RTC_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "syscall.h"

/* Interrupt vector for keyboard and rtc */
#define RTC_IV          0x08
#define RTC_PORT        0x70
#define DEFAULT_FPS     1024

/* init functions */ 
void init_rtc();

/* rtc handler and its linkage function */
int rtc_handler();
extern int linkage_rtc_handler();

/* driver functions */
int rtc_open(const uint8_t* filename);
int rtc_close(int32_t fd);
int rtc_read( int32_t fd, void* buf, int32_t nbytes, char* file_name, uint32_t offset,uint32_t inode_index);
int rtc_write(int32_t fd, const void* buf, int32_t nbytes);

#endif /* _RTC_H */
