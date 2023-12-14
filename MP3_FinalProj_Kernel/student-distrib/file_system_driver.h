#ifndef _FSD_H
#define _FSD_H

#include "types.h"

#define FILENAME_LEN    32
struct file_inode_t {
            int32_t length    ;
            int32_t data_block_num[1023] ; 
}file_inode_t;

struct file_dentry_t {
            int8_t  filename[FILENAME_LEN]    ;
            int32_t filetype ;
            int32_t inode_num  ;
            int8_t  reserved[24] ;

}file_dentry_t;

struct file_boot_block_t {
            int32_t dir_count ;
            int32_t inode_count ;
            int32_t data_count ;
            int8_t  reserved[52] ;
            struct  file_dentry_t direntries[63] ;            
}file_boot_block_t;


typedef struct file_ops
{
    int (*open) (const uint8_t* filename);
    int (*write) (int32_t fd, const void* buf, int32_t nbytes);
    int (*read) (int32_t fd, void* buf, int32_t nbytes, char* file_name, uint32_t offset,uint32_t inode_index); //need an offset for sysread
    int (*close) (int32_t fd);
}file_ops_t;


typedef struct file_descriptor_t
{
    struct file_ops* file_table_pointer ;   
    char* name_of_file ; 
    uint32_t inode_index  ; 
    uint32_t file_position ; 
    int32_t internal_ftype;
    uint32_t flags ;         
}file_descriptor_t;

struct file_datablock_t{
            char data[4096] ;           
}file_datablock_t;

struct file_inode_t* start_inode ;


extern void init_filesystem(uint8_t* file_start,uint8_t* file_end) ; 
extern int32_t read_dentry_by_name (const uint8_t* fname, struct file_dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, struct file_dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

extern int32_t file_read(int32_t fd, void* buf, int32_t nbytes, char* file_name, uint32_t offset,uint32_t inode_index) ;
extern int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) ;
extern int32_t file_open(const uint8_t* filename) ;
extern int32_t file_close(int32_t fd) ;

extern int32_t dir_read(int32_t fd, void* buf, int32_t nbytes , char* file_name, uint32_t offset,uint32_t inode_index) ;
extern int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes) ;
extern int32_t dir_open(const uint8_t* filename) ;
extern int32_t dir_close(int32_t fd) ;

#endif
