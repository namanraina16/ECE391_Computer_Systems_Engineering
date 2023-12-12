#include "types.h"
#include "syscall.h"
#include "terminal.h"

#define VIDEO_LOC       0xB8000
#define four_kB         4096
#define ONETWENTYEIGHTMB    0x8000000
#define VIDSIZE         1024
#define vid_index   32+1 // 128/4 + 1 = 32+1 which maps to 34th dir entry and is at 132Mb

#define second_128mB            64      //(128MB <<1) >> 22 = (0x8000000 << 1) >> 22 = 64

#define first_10bitsof_128Mb   32
#define NUM_COLS    80

#define TERMINAL1_ADDR  (VIDEO_LOC + four_kB)
#define TERMINAL2_ADDR  (VIDEO_LOC + 2*(four_kB))
#define TERMINAL3_ADDR  (VIDEO_LOC + 3*(four_kB))


struct __attribute__ ((packed)) page_directory_entry_4kB_t {
            uint32_t present       : 1;
            uint32_t read_or_write : 1 ;
            uint32_t user_or_supervisor : 1 ;
            uint32_t page_write_through  : 1;
            uint32_t page_cache_disabled  : 1;
            uint32_t accessed :1 ;
            uint32_t set_zero : 1 ;
            uint32_t page_size : 1  ;
            uint32_t global_bit  : 1;
            uint32_t avail    : 3   ;
            uint32_t base_address : 20 ;
}page_directory_entry_4kB_t ;


extern void loadPageDirectory(unsigned int*);
extern void enablePaging();
extern void page_init();
extern void setup_PID(pcb_t* current_pcb);
void switch_terminal(int term_ID);
extern uint32_t dump_contents();
extern void flush_tlb();

// extern void vid_init();

struct __attribute__ ((packed)) page_table_entry_t { //set PAT to 0
            uint32_t present       : 1;
            uint32_t read_or_write : 1 ;
            uint32_t user_or_supervisor : 1 ;
            uint32_t page_write_through  : 1;
            uint32_t page_cache_disabled  : 1;
            uint32_t accessed :1 ;
            uint32_t dirty : 1  ;
            uint32_t page_attribute_table : 1  ;
            uint32_t global_bit  : 1;
            uint32_t avail    : 3   ;
            uint32_t base_address : 20 ;
}page_table_entry_t;

struct  __attribute__ ((packed)) page_directory_entry_4mB_t {
            uint32_t present       : 1;
            uint32_t read_or_write : 1 ;
            uint32_t user_or_supervisor : 1 ;
            uint32_t page_write_through  : 1;
            uint32_t page_cache_disabled  : 1;
            uint32_t accessed :1 ;
            uint32_t dirty :1 ;
            uint32_t page_size : 1  ;
            uint32_t global_bit  : 1;
            uint32_t avail    : 3   ;
            uint32_t page_attribute_table : 1  ;
            uint32_t reserved : 9 ; 
            uint32_t base_address : 10 ;
} page_directory_entry_4mB_t;


typedef union page_dirs { //1024 size //holds only one member at a given time
    struct page_directory_entry_4kB_t p4k ; //
    struct page_directory_entry_4mB_t p4m ;
}page_dir_entries ;

void remap_mem(int new_id, int old_id);
void update_mapping(int new_id);

page_dir_entries pages_directory[1024] __attribute__((aligned(4096))); //each entry points to a page table either a table or 4mB //first entry is video and second is kernel //rest are nto presente
struct page_table_entry_t page_4k_tables[1024]  __attribute__((aligned(4096))) ;
struct page_table_entry_t vid_map_table[1024]  __attribute__((aligned(4096))) ;


