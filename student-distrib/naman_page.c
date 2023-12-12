#include "paging.h"

//make static page_dir_entries and table_entry_t



/* void page_init
 * Inputs: None
 * Return Value: None
 * Function: Initializes struct fields and creates two PDEs that point to phys memory for video and kernel.
 *           Also sets unused PDEs and PTEs to not present.*/
void page_init() //maps kernel from virtual (4-8 MB) to physical also and does same for video memory
{
    int i;
    /*Setting up 4mB kernel memory mapping*/
    pages_directory[1].p4m.present = 1 ; //if used then present
    pages_directory[1].p4m.read_or_write = 1 ; //always R/W enabled
    pages_directory[1].p4m.user_or_supervisor = 0 ; //always in supervisor mode for all pages
    pages_directory[1].p4m.page_write_through = 0 ; //0 for all pages
    pages_directory[1].p4m.page_cache_disabled = 0 ; 
    pages_directory[1].p4m.accessed = 0 ; 
    pages_directory[1].p4m.dirty = 0 ; 
    pages_directory[1].p4m.page_size = 1 ; //indicates 4mB page
    pages_directory[1].p4m.global_bit = 1 ; 
    pages_directory[1].p4m.avail = 0 ;
    pages_directory[1].p4m.page_attribute_table = 0 ;
    pages_directory[1].p4m.reserved = 0 ; //hardcode reserved bits
    pages_directory[1].p4m.base_address = 1 ; //1*4mB = 4mB

    pages_directory[0].p4k.present = 1 ;
    pages_directory[0].p4k.read_or_write = 1 ;
    pages_directory[0].p4k.user_or_supervisor = 0 ; //supervisor regardless 
    pages_directory[0].p4k.page_write_through = 0 ;
    pages_directory[0].p4k.page_cache_disabled = 1 ; //video memory page instead of kernel
    pages_directory[0].p4k.accessed = 0 ; //not used
    pages_directory[0].p4k.set_zero = 0 ; //not used
    pages_directory[0].p4k.page_size = 0 ; //indicates 4kB page
    pages_directory[0].p4k.global_bit = 0 ; //nonkernel page
    pages_directory[0].p4k.avail = 0 ;
    pages_directory[0].p4k.base_address = (uint32_t) page_4k_tables >> 12 ; //access upper 20 bits of 32-bit address


    pages_directory[vid_index].p4k.present = 1 ;
    pages_directory[vid_index].p4k.read_or_write = 1 ;
    pages_directory[vid_index].p4k.user_or_supervisor = 1 ; //user 
    pages_directory[vid_index].p4k.page_write_through = 0 ;
    pages_directory[vid_index].p4k.page_cache_disabled = 1 ; //video memory page instead of kernel
    pages_directory[vid_index].p4k.accessed = 0 ; //not used
    pages_directory[vid_index].p4k.set_zero = 0 ; //not used
    pages_directory[vid_index].p4k.page_size = 0 ; //indicates 4kB page
    pages_directory[vid_index].p4k.global_bit = 0 ; //nonkernel page
    pages_directory[vid_index].p4k.avail = 0 ;
    pages_directory[vid_index].p4k.base_address = ((uint32_t) vid_map_table) >> 12 ; //access upper 20 bits of 32-bit address

    int index = VIDEO_LOC / four_kB ; //calculating

    vid_map_table[0].present = 1 ; //set up PTE to present
    vid_map_table[0].read_or_write = 1 ; 
    vid_map_table[0].user_or_supervisor = 1 ; //supervisor mode regardless
    vid_map_table[0].page_write_through = 0 ; 
    vid_map_table[0].page_cache_disabled = 1 ;
    vid_map_table[0].accessed = 0 ; 
    vid_map_table[0].dirty = 0 ; //set to 0 for all pages
    vid_map_table[0].page_attribute_table = 0 ; 
    vid_map_table[0].avail = 0 ; 
    vid_map_table[0].base_address = index ;


    page_4k_tables[index].present = 1 ; //set up PTE to present
    page_4k_tables[index].read_or_write = 1 ; 
    page_4k_tables[index].user_or_supervisor = 0 ; //supervisor mode regardless
    page_4k_tables[index].page_write_through = 0 ; 
    page_4k_tables[index].page_cache_disabled = 1 ;
    page_4k_tables[index].accessed = 0 ; 
    page_4k_tables[index].dirty = 0 ; //set to 0 for all pages
    page_4k_tables[index].page_attribute_table = 0 ; 
    page_4k_tables[index].avail = 0x0 ; 
    page_4k_tables[index].base_address = index ; // 0xB8000/4096 (since 4Kb pages)


    vid_map_table[1].present = 1;
    vid_map_table[1].read_or_write = 1; 
    vid_map_table[1].user_or_supervisor = 1;
    vid_map_table[1].page_write_through = 0; 
    vid_map_table[1].page_cache_disabled = 1;
    vid_map_table[1].accessed = 0; 
    vid_map_table[1].dirty = 0;
    vid_map_table[1].page_attribute_table = 0; 
    vid_map_table[1].avail = 0x0; 
    vid_map_table[1].base_address = TERMINAL1_ADDR / FOUR_KB;

    page_4k_tables[index+1].present = 1 ; //set up PTE to present
    page_4k_tables[index+1].read_or_write = 1 ; 
    page_4k_tables[index+1].user_or_supervisor = 0 ; //supervisor mode regardless
    page_4k_tables[index+1].page_write_through = 0 ; 
    page_4k_tables[index+1].page_cache_disabled = 1 ;
    page_4k_tables[index+1].accessed = 0 ; 
    page_4k_tables[index+1].dirty = 0 ; //set to 0 for all pages
    page_4k_tables[index+1].page_attribute_table = 0 ; 
    page_4k_tables[index+1].avail = 0x0 ; 
    page_4k_tables[index+1].base_address = TERMINAL1_ADDR / FOUR_KB; // 0xB8000/4096 (since 4Kb pages)


    vid_map_table[2].present = 1;
    vid_map_table[2].read_or_write = 1; 
    vid_map_table[2].user_or_supervisor = 1;
    vid_map_table[2].page_write_through = 0; 
    vid_map_table[2].page_cache_disabled = 1;
    vid_map_table[2].accessed = 0; 
    vid_map_table[2].dirty = 0;
    vid_map_table[2].page_attribute_table = 0; 
    vid_map_table[2].avail = 0x0; 
    vid_map_table[2].base_address = TERMINAL2_ADDR / FOUR_KB;

    page_4k_tables[index+2].present = 1 ; //set up PTE to present
    page_4k_tables[index+2].read_or_write = 1 ; 
    page_4k_tables[index+2].user_or_supervisor = 0 ; //supervisor mode regardless
    page_4k_tables[index+2].page_write_through = 0 ; 
    page_4k_tables[index+2].page_cache_disabled = 1 ;
    page_4k_tables[index+2].accessed = 0 ; 
    page_4k_tables[index+2].dirty = 0 ; //set to 0 for all pages
    page_4k_tables[index+2].page_attribute_table = 0 ; 
    page_4k_tables[index+2].avail = 0x0 ; 
    page_4k_tables[index+2].base_address = TERMINAL2_ADDR / FOUR_KB; // 0xB8000/4096 (since 4Kb pages)


    vid_map_table[3].present = 1;
    vid_map_table[3].read_or_write = 1; 
    vid_map_table[3].user_or_supervisor = 1;
    vid_map_table[3].page_write_through = 0; 
    vid_map_table[3].page_cache_disabled = 1;
    vid_map_table[3].accessed = 0; 
    vid_map_table[3].dirty = 0;
    vid_map_table[3].page_attribute_table = 0; 
    vid_map_table[3].avail = 0x0; 
    vid_map_table[3].base_address = TERMINAL3_ADDR / FOUR_KB;

    page_4k_tables[index+3].present = 1 ; //set up PTE to present
    page_4k_tables[index+3].read_or_write = 1 ; 
    page_4k_tables[index+3].user_or_supervisor = 0 ; //supervisor mode regardless
    page_4k_tables[index+3].page_write_through = 0 ; 
    page_4k_tables[index+3].page_cache_disabled = 1 ;
    page_4k_tables[index+3].accessed = 0 ; 
    page_4k_tables[index+3].dirty = 0 ; //set to 0 for all pages
    page_4k_tables[index+3].page_attribute_table = 0 ; 
    page_4k_tables[index+3].avail = 0x0 ; 
    page_4k_tables[index+3].base_address = TERMINAL3_ADDR / FOUR_KB; // 0xB8000/4096 (since 4Kb pages)




    //set rest entries to not present
    for (i = 2 ; i < four_kB/4 ; i++)
    {
        if (i != vid_index)
        {
            pages_directory[i].p4m.present = 0 ;
            pages_directory[i].p4m.read_or_write = 1 ; 
        }
    }

    for (i = 2 ; i < four_kB/4 ; i++)
    {
        if (i != vid_index)
        {
            pages_directory[i].p4k.present = 0 ;
            pages_directory[i].p4k.read_or_write = 1 ;
        }
    }
    
    for (i = 0 ; i < four_kB/4 ; i++)
    {
        if (i != index && i != index + 1 && i != index + 2 && i != index + 3)
        {
            page_4k_tables[i].present = 0 ;
        }
    }

    for (i = 0 ; i < four_kB/4 ; i++)
    {
        if (i > 3)
        {
            vid_map_table[i].present = 0 ;
        }
    }

    /*Load and Enable Paging Directory and Paging*/
    loadPageDirectory((unsigned int*)pages_directory) ; 
    enablePaging() ;

}
/* void remap_mem
 * Inputs: new term_id, old term_id
 * Return Value: None
 * Function: Move the old terminal to the video memory and vice versa 
 * */
void remap_mem(int new_id, int old_id) {
    int addr;
    int new_addr;
    addr = VIDEO_LOC + (old_id + 1) * FOUR_KB;
    new_addr = VIDEO_LOC + (new_id + 1) * FOUR_KB;
    
    //SAVE THE CURRENT TERMINAL TO ITS PROPER THING!!!!
    // addr /= FOUR_KB;
    // new_addr /= FOUR_KB;
    memcpy( (char *) addr, (char *) VIDEO_LOC, FOUR_KB);

    //copies the vidmap0 mapping to the terminal one
    memcpy((char*) VIDEO_LOC, (char *) new_addr, FOUR_KB);
}

void update_mapping(int new_id) {
    int index = VIDEO_LOC / four_kB ; //calculating
    page_4k_tables[index].base_address = index + new_id; // 0xB8000/4096 (since 4Kb pages)
}

/* void switch_terminal
 * Inputs: new term_idooo
 * Return Value: None
 * Function: Move everything about the old terminal to the new terminal
 * */
void switch_terminal(int new_term_ID){
    cli();
    if (new_term_ID == curr_term_ID){
        sti();
        return;
    }

    // if (schedule_term_ID != curr_term_ID) update_mapping(0); 
    //updates cursor position and updates where in the video memory to write to
    uint16_t old_pos = get_cursor_position();
    terminals[curr_term_ID].cursor = old_pos;
    update_cursor(terminals[new_term_ID].cursor % NUM_COLS, terminals[new_term_ID].cursor / NUM_COLS);
    write_switch();

    
    //switch the terminal accordingly
    remap_mem(new_term_ID, curr_term_ID);
    
    memcpy(terminals[curr_term_ID].sbuf, keyb_buff, BUFFER_SIZE);
    memset(keyb_buff, 0, BUFFER_SIZE);
    kbb_ind = 0;
    memcpy(keyb_buff, terminals[new_term_ID].sbuf, BUFFER_SIZE);

    term_t curr_term = terminals[new_term_ID];
    pcb_t* curr_pcb = get_pcb(curr_term.processes[curr_term.proc_cnt-1]);

    uint32_t dummy = (TWO + curr_pcb->Pid);
    pages_directory[first_10bitsof_128Mb].p4m.base_address = dummy;
    loadPageDirectory((unsigned int*)pages_directory); //flushes TLB

    // if (schedule_term_ID != curr_term_ID) update_mapping(schedule_term_ID+1);
    // tss.ss0 = KERNEL_DS; 
    // tss.esp0 = EIGHT_MB - (EIGHT_KB* terminals[new_term_ID].processes[terminals[new_term_ID].proc_cnt-1]) -4;

    curr_term_ID = new_term_ID;
    sti();
    //update_cursor(old_pos / NUM_COLS, old_pos % NUM_COLS);
}

/* void setup_PID
 * Inputs: current_pcb
 * Return Value: None
 * Function: Setup the PID and the paging for multiple terminals, vmem, etc.
 * */
void setup_PID(pcb_t* current_pcb)
{
    //Our virtual address is 128 mB whose first 10 bits are 32
    /*Setting up a 4mB kernel memory mapping*/
    pages_directory[first_10bitsof_128Mb].p4m.present = 1 ; //if used then present
    pages_directory[first_10bitsof_128Mb].p4m.read_or_write = 1 ; //always R/W enabled
    pages_directory[first_10bitsof_128Mb].p4m.user_or_supervisor = 1 ; //always in supervisor mode for all pages
    pages_directory[first_10bitsof_128Mb].p4m.page_write_through = 0 ; //0 for all pages
    pages_directory[first_10bitsof_128Mb].p4m.page_cache_disabled = 0 ; 
    pages_directory[first_10bitsof_128Mb].p4m.accessed = 0 ; 
    pages_directory[first_10bitsof_128Mb].p4m.dirty = 0 ; 
    pages_directory[first_10bitsof_128Mb].p4m.page_size = 1 ; //indicates 4mB page
    pages_directory[first_10bitsof_128Mb].p4m.global_bit = 0; 
    pages_directory[first_10bitsof_128Mb].p4m.avail = 0 ;
    pages_directory[first_10bitsof_128Mb].p4m.page_attribute_table = 0 ;
    pages_directory[first_10bitsof_128Mb].p4m.reserved = 0 ; //hardcode reserved bits
    pages_directory[first_10bitsof_128Mb].p4m.base_address = 2 + current_pcb->Pid ; //2*4mB = 8mB (or 8mB rightshifted 22 times)     
    loadPageDirectory((unsigned int*)pages_directory); //flushes TLB
}

// void vid_init()
// {
//     // 4kb 
//     // B800 >> 12
//     vid_map_directory[second_128mB].read_or_write = 1 ; //always R/W enabled
//     vid_map_directory[second_128mB].user_or_supervisor = 1 ; //always in supervisor mode for all pages
//     vid_map_directory[second_128mB].present = 1 ; //if used then present
//     vid_map_directory[second_128mB].page_write_through = 0 ;
//     vid_map_directory[second_128mB].page_cache_disabled = 1 ; 
//     vid_map_directory[second_128mB].accessed = 0 ; 
//     vid_map_directory[second_128mB].dirty = 0 ; 
//     vid_map_directory[second_128mB].global_bit = 0; 
//     vid_map_directory[second_128mB].avail = 0x0 ;
//     vid_map_directory[second_128mB].page_attribute_table = 0 ;
//     vid_map_directory[second_128mB].base_address = VIDEO_LOC/four_kB;     // base address 
//     loadPageDirectory((unsigned int*)vid_map_directory); //flushes TLB
// }
