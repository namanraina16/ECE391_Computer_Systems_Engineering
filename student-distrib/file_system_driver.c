#include "file_system_driver.h"
#include "lib.h"
#define NUMS_DIR_ENTRIES    63  
#define NUMS_DATA_BLOCKS    1023


int total_dirs ; 
static struct file_datablock_t* start_datablock ;
static struct file_boot_block_t* start_boot_block ;
// static struct file_descriptor_t file_descriptor[8] ;
int inode_nums ;
int in_file_offset = 0;
int dir_read_offset = 0 ; 


/* int32_t init_filesystem
 * Inputs: file_start -- start of bootblock adddress
           file_end -- end of filesys address (NOT USED)
 * Return Value: None
 * Function: Initializes filesystem and sets all structs to initial vals */
void init_filesystem(uint8_t* file_start,uint8_t* file_end) 
{
    total_dirs = *file_start ; 
    start_boot_block = (struct file_boot_block_t*) file_start ; //typecast
    start_inode = (struct file_inode_t*) (file_start + 4096) ; 
    inode_nums = *(file_start + 4) + 1 ;
    start_datablock = (struct file_datablock_t*) (file_start + (inode_nums*4096)) ; 
}



/* int32_t read_dentry_by_name
 * Inputs: fname -- char filename
           dentry -- struct to copy to
 * Return Value: 1 -- Success
                 -1 -- Invalid File
 * Function: Searches dir by name and if valid then copies to dentry*/
int32_t read_dentry_by_name (const uint8_t* fname, struct file_dentry_t* dentry)
{
    if (strlen((const int8_t*)fname) > 32) {
        // printf("INVALID FILE_NAME\n");
        return -1 ;
    }
    int i ;
    i = 0 ;
    uint32_t* current_dir = (uint32_t*) start_boot_block ;
    current_dir+=16 ;
    while (i < total_dirs) //63
    {
        if (strncmp((const int8_t*)current_dir,(const int8_t*)fname,32) == 0) // TODO: use strncmp(_, _, 32) //COMPARE THE ENTIRE THING
        {
            *dentry = *(struct file_dentry_t*)current_dir;
            break ;
        }
        current_dir += 16 ;
        i++ ;
    }
    if (i==total_dirs)
    {
        //printf("INVALID FILE_NAME\n");
        return -1 ;
    }
    //CALL READ_DATA HERE
    return 1 ;
}



/* int32_t read_dentry_by_index
 * Inputs: index -- node number index
           dentry -- struct to copy to
 * Return Value: 1 -- Success
                 -1 -- Invalid Index
 * Function: Searches dir by index and if valid then copies to dentry*/
int32_t read_dentry_by_index (uint32_t index, struct file_dentry_t* dentry)
{
    if (index < 0 || index > 62)
    { return -1 ;}
    uint32_t* current_dir  ;
    current_dir = (uint32_t*) start_boot_block ;
    current_dir+=16*(index+1) ;
    *dentry = *(struct file_dentry_t*)current_dir;
    //read_data
    return 1;
}


/* int32_t read_data
 * Inputs: inode -- zero-indexed index needed to access file info
           offset -- start reading file from here
           buf  -- store content that is read here
           length -- read from [offset,offset+length)
 * Return Value: 1 -- Success
                -1 -- invalid file
 * Function: Reads file contents and stores it into the buffer only if valid and then returns 1*/
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
    struct file_inode_t* temp_inode ;
    int idx ; 
    int returnval = length;
    idx = 0 ;
    int block_start_byte,block_end_byte ;
    uint32_t bytes_to_read = length ; 
    
    block_start_byte = offset/4096 + offset%4096 ;
    block_end_byte = (length+offset) % 4096 + (length+offset)/4096;
    if (inode >= inode_nums - 1){return -1;}
    if(length+offset > (start_inode+inode)->length)
    {
        length = (start_inode+inode)->length - offset ; //truncate length
        returnval = length ;
    }
    uint8_t* current_byte ;
    current_byte = (uint8_t*)(start_datablock +  (start_inode + inode)->data_block_num[offset/4096])->data ;
    current_byte+=(offset % 4096) ; 
    temp_inode = start_inode ; 
    if (length <= 4096 - (offset % 4096))
    {
        memcpy(buf,current_byte,length) ;
        bytes_to_read -= length ;
        // putc(*buf) ;
        // printf("\n") ;
        //buf+=length ; 
    }
    else
    {
        memcpy(buf,current_byte,4096 - (offset % 4096)) ;
        bytes_to_read -= 4096 - (offset % 4096) ;
        current_byte = (uint8_t*)(start_datablock +  (temp_inode + inode)->data_block_num[offset/4096 + 1])->data ; 
        while (bytes_to_read > 4096)
        {
            memcpy(buf+4096 - (offset % 4096) + 4096*idx,current_byte,4096) ;
            idx++ ;
            current_byte = (uint8_t*)(start_datablock +  (temp_inode + inode)->data_block_num[offset/4096 + idx + 1])->data ;
            bytes_to_read -= 4096 ;
        }
        memcpy(buf+4096 - (offset % 4096) + 4096*idx,current_byte,bytes_to_read);
        returnval = bytes_to_read;
    }
    
    return returnval;
}




/* int32_t file_open
 * Inputs: filename -- char filename
 * Return Value: 0 -- implement in CP3
 * Function: Implement in CP3*/
int32_t file_open(const uint8_t* filename) 
{
    // struct file_dentry_t temp_dentry ; 
    // int i ; 
    // i = 2;
    // while (i < 8)
    // {
    //     if(file_descriptor[i].flags != 1)
    //     {
    //         file_descriptor[i].flags = 1 ; 
    //         i = 7 ;
    //     }
    //     i++ ;
    // }
    // if (read_dentry_by_name(filename,&temp_dentry) == -1) 
    // {
    //     return -1 ; 
    // }
    // file_descriptor[i].inode_index = temp_dentry.inode_num ;
    // file_descriptor[i].file_name = (char*) filename ; 
    // file_descriptor[i].file_position = 0 ;
    // return i ; 
    return 0 ;
}

/* int32_t dir_open
 * Inputs: filename -- char filename
 * Return Value: 0 -- implement in CP3
 * Function: implement in CP3*/
int32_t dir_open(const uint8_t* filename) 
{
    // struct file_dentry_t temp_dentry ; 
    // int i ; 
    // i = 2;
    // while (i < 8)
    // {
    //     if(file_descriptor[i].flags != 1)
    //     {
    //         file_descriptor[i].flags = 1 ; 
    //         i = 7 ;
    //     }
    //     i++ ;
    // }
    // if (read_dentry_by_name(filename,&temp_dentry) == -1) 
    // {
    //     return -1 ; 
    // }
    // file_descriptor[i].inode_index = 0 ;
    // file_descriptor[i].file_name = "." ; 
    // file_descriptor[i].file_position = 0 ;
    return 0 ; 
}
/*Open Functions End*/



/* int32_t file_read
 * Inputs: file_name --char filename
           fd -- index in fd array
           buf  -- store content that is read here
           nbytes -- bytes to read
 * Return Value: 1 -- Success
                 -1 -- Invalid file
 * Function: Reads file content and prints to console */
int32_t file_read( int32_t fd, void* buf, int32_t nbytes, char* file_name, uint32_t offset,uint32_t inode_index) //offset arg
{
    //read_data
    // offset = offset ;
    struct file_dentry_t dummy;
    if (file_name == '\0') return -1;
    //memset((char *)buf, 0, nbytes);
    // int i;
    // for (i = 0; i < nbytes; i++) *((char *)buf+i) = '\0';
    read_dentry_by_index(inode_index,&dummy);
    //int32_t curr_length = (start_inode+dummy.inode_num)->length;
    int bytes_ret = read_data(inode_index, offset, (uint8_t*)buf, nbytes) ; 

    return bytes_ret;
}

/* int32_t dir_read
 * Inputs: fd -- index in fd array
           buf  -- store content that is read here
           nbytes -- bytes to read
 * Return Value: 1 -- Success
                 -1 -- Invalid directory
 * Function: Reads dir content and prints one-by-one to console*/
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes, char* file_name, uint32_t offset,uint32_t inode_index) //increment file position
{
    /*Reads directory entries from the bootblock one at a time
    The file descriptor will have a file position entry that you use to determine which directory entry you’re reading
    Grab the dentry
    Increment the file position by 1 each time you read a directory
    Copy the file name from the dentry to the buffer you passed in
    Return the bytes read
    */

    struct file_dentry_t dummy;
    inode_index = offset ;
    //int position = file_descriptor[fd].file_position;
    // offset = offset ;
    read_dentry_by_index(offset,&dummy);
    if (dummy.filename[0] == '\0') return -1;

    char string[33] = {};
	// memset(string, '\0', 33);

    int i;
    for (i = 0; i < 32; ++i) {
        *(string+i) = dummy.filename[i];
        if (i >= 32 || dummy.filename[i] == '\0') break;
    }
    memcpy(buf, string, i);
    // puts(buf);
    // puts("\n");
    //file_descriptor[fd].file_position++;

    return i ;
}
/*Read Functions End*/
/*Read Functions End*/



/* int32_t file_close
 * Inputs: fd -- fd index in array
           
 * Return Value: 0 -- CP3
 * Function: CP3 */
int32_t file_close(int32_t fd) 
{
    // if (fd == 0 || fd == 1 || fd > 7)
    // {
    //     return -1 ;
    // }
    // file_descriptor[fd].flags = 0 ; //not busy
    return 0 ;
}

/* int32_t dir_close
 * Inputs: fd -- fd index in array
           
 * Return Value: 0 -- CP3
 * Function: CP3 */
int32_t dir_close(int32_t fd) 
{
    // if (fd == 0 || fd == 1 || fd > 7)
    // {
    //     return -1 ;
    // }
    // file_descriptor[fd].flags = 0 ; //not busy
    return 0 ;
}
/*Close Functions End*/



/* int32_t file_write
 * Inputs: fd -- fd index in array
            buf -- to write to
            nbytes - bytes to write
 * Return Value: 0 -- CP3
 * Function: CP3 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes)
{
    return -1 ; 
}

/* int32_t dir_write
 * Inputs: fd -- fd index in array
            buf -- to write to
            nbytes - bytes to write
 * Return Value: 0 -- CP3
 * Function: CP3 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes) 
{
    return -1 ; 
}
