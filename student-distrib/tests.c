#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "file_system_driver.h"
#include "idt.h"
#include "syscall.h"

#define PASS 1
#define FAIL 0

#define VMEM_LOWER       	0xB8000
#define VMEM_UPPER      	0xC0000
#define KERNEL_LOWER       	0x400000
#define KERRNEL_UPPER       0x800000

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

/* ############################################################ */
/*                                                              */
/* 					CHECKPOINT NUMBER ONE	 					*/
/*                                                              */
/* ############################################################ */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}


/* div_by_zero
 * 
 * Check if div by 0 raise exception
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
int div_by_zero(){
	TEST_HEADER;
	int a = 0;
	int b;
	b = 1/a;
	return FAIL;
}

/* check null 
 * 
 * Check if can dereference the address
 * 	if the address is expected to have values in it, pass PASS in expected
 * Inputs: int expected
 * Outputs: if it is expected to have input, return expected
 * Side Effects: if the address is expected to have values in it, pass PASS in expected
 */
int check_null(char * addr, int expected){
	TEST_HEADER;
	char a;
	a = *addr; // gives exception
	printf("%d\n",a) ;
	return expected;
}

/* ############################################################ */
/*                                                              */
/* 					CHECKPOINT NUMBER TWO	 					*/
/*                                                              */
/* ############################################################ */

/* test_rtc
 * 
 * Check if rtc working properly
 * Inputs: int expected, the starting freq
 * Outputs: if it is expected to have input, return expected
 * Side Effects: print 1 on screen at different frequency
 */
// int test_rtc(unsigned start){
// 	TEST_HEADER;
// 	unsigned freq;
// 	int fd;

// 	for (freq = start; freq <= 1024; freq *= 2) {
// 		fd = rtc_write(freq);
// 		if (fd == -1) return FAIL;

// 		unsigned i;
// 		for (i = 0; i <= freq; i++)
// 			fd = rtc_read("",-1,0,-1);
// 		clear();
// 	}
// 	TEST_HEADER;
// 	return PASS;
// }

/* test_list_files
 * 
 * Check if can list all files
 * Inputs: none
 * Outputs: pass if pass, fail if fail
 * Side Effects: list files to terminal
 */
int test_list_files(){
	TEST_HEADER;
	struct file_dentry_t dummy ;
	struct file_inode_t inode_dummy;
    int idx,i;
    for (idx = 0; idx < 61; ++idx) {
		char name[33] = {};
        read_dentry_by_index(idx,&dummy);
		if (dummy.filename[0] == 0) continue;
        for (i = 0; i < 32; ++i) {
			if (dummy.filename[i] == '\0') name[i] = ' ';
			name[i] = dummy.filename[i];
        }
		inode_dummy = start_inode[dummy.inode_num];
		printf("Type: %d Size %d file_name: %s\n", dummy.filetype, inode_dummy.length, name);
    }
	return PASS;
}

/* test_list_files
 * 
 * Check if can read a dir
 * Inputs: num_file, number of dir wanna read
 * Outputs: pass if pass, fail if fail
 * Side Effects: list files to terminal
 */
// int test_dir_read(int num_file){
// 	TEST_HEADER;
// 	int idx;
//     for (idx = 0; idx < num_file; ++idx) {
// 		char buf[32] = {};
// 		if (dir_read("",0, buf, 33,idx) == -1)
// 			break;
// 		printf("%s\n", buf);
// 	}
// 	return PASS;
// }

/* test_list_files
 * 
 * Check if can read a dir
 * Inputs: num_file, number of dir wanna read
 * Outputs: pass if pass, fail if fail""
 * Side Effects: list files to terminal
 */
// int test_file_read(char * name){
// 	TEST_HEADER;

// 	struct file_dentry_t dummy ;
// 	struct file_inode_t inode_dummy;
// 	if (read_dentry_by_name((const uint8_t *)name,&dummy) == -1) 
// 		return FAIL;
// 	inode_dummy = start_inode[dummy.inode_num] ;
// 	int size = start_inode[dummy.inode_num].length;

// 	static uint8_t buf[40456];
// 	// memset(&buf, 0, size);
// 	int i ;
// 	// file_open(name) ;
// 	file_read(name,0, (uint8_t*)(buf),4,24);
	
// 	// printf("%s", buf);
// 	for (i = 0 ; i < size ; i++)
// 	{	
// 		if (*(buf+i) != '\0')
// 		{
// 				putc(*(buf+i)) ;
// 		}
// 	}
// 	return PASS;
// }

/* test_filesystem
 * 
 * Not used
 * Inputs: num_file, number of dir wanna read
 * Outputs: pass if pass, fail if fail
 * Side Effects: list files to terminal
 */
int test_filesystem()//char* file_NAAM
{
	// struct file_dentry_t dummy ;
	// // uint32_t idx = 10;
	// int idx,i;
	// for (idx = 0; idx < 19; ++idx) {
	// 	read_dentry_by_index(idx,&dummy) ;
	// 	for (i = 0; i < 32; ++i) {
	// 		if (dummy.filename[i] == 0) break;
	// 		putc(dummy.filename[i]);
	// 	}
	// 	printf("\n");
	// }
	static uint8_t temp[5277] ; //MUST BE static
	int i ;
	struct file_dentry_t dummy ;
	struct file_inode_t inode_dummy;
	read_dentry_by_index(11,&dummy) ;
	inode_dummy = start_inode[dummy.inode_num] ;
	read_data(dummy.inode_num,0,(uint8_t*)temp,5277);
	
	for (i = 0 ; i < 5277 ; i++)
	{	
		if (temp[i] != '\0')
		{
				putc(temp[i]) ;
		}
	}
	return 0 ; 
	// char* file_name_example ;
	// file_name_example = "frame1.txt" ;
    // return file_open(file_name_example); 
	// file_NAAM = "frame30.txt";
	// printf("---------------------\n") ;
	// if (read_dentry_by_name((const uint8_t*)file_NAAM,&dummy) == 1)
	// {	
	// 	puts(dummy.filename) ;
	// 	return PASS ;
	// }
	// else if (read_dentry_by_name((const uint8_t*)file_NAAM,&dummy) == -1)
	// {
	// 	return FAIL ;
	// }
	// return -1 ;
	// uint8_t temp_buff ;
	// return read_data(9,150,&temp_buff,1200);
}

/* test_filesystem
 * 
 * Not used
 * Inputs: num_file, number of dir wanna read
 * Outputs: pass if pass, fail if fail
 * Side Effects: list files to terminal
 */
// int test_keyboard(){
// 	int i,j;
// 	char * test = "test: hey! nice to meetHEYDON'TCUTMEOFF";
//     i = terminal_read(0, test, 39);
//     j = terminal_write(1, (const char *)test, 23);
// 	return i - j;
// 	//then let them play around with the keyboard if they want to.
// }



/* ############################################################ */
/*                                                              */
/* 					CHECKPOINT NUMBER THREE	 					*/
/*                                                              */
/* ############################################################ */

/* test_filesystem
 * 
 * Not used
 * Inputs: num_file, number of dir wanna read
 * Outputs: pass if pass, fail if fail
 * Side Effects: list files to terminal
 */
int test_syscall_handler()
{
	return FAIL ;
	// ece391_halt(1);
}

int test_sysfileops(char* fname , void* buf_read , void* buf_write , int32_t bytes )
{
	// int fopen = sysopen("frame1.txt") ;
	// static uint8_t buf[40456];
	// sysread(fopen,buf_read,bytes) ;
	// syswrite(fopen,buf_write,bytes) ;
	// sysclose(fopen) ;
	return FAIL ; 
}





/* ############################################################ */
/*                                                              */
/* 					CHECKPOINT NUMBER FOUR	 					*/
/*                                                              */
/* ############################################################ */





/* ############################################################ */
/*                                                              */
/* 					CHECKPOINT NUMBER FIVE	 					*/
/*                                                              */
/* ############################################################ */

/* Test suite entry point */
void launch_tests(){

	/* ########## TEST CHECKPOINT 1 ##########*/

	/* TEST IDT */
	// TEST_OUTPUT("idt_test", idt_test());

	/* TEST EXCEPTIONS (Eli approved) */
	// TEST_OUTPUT("div_by_zero", div_by_zero());
	// TEST_OUTPUT("check_null", check_null(0, FAIL)); // pagefault

	/* TEST PAGING */
	// TEST_OUTPUT("check_lower_vmem", check_null(VMEM_LOWER, PASS));
	// TEST_OUTPUT("check_upper_vmem", check_null(VMEM_UPPER, FAIL)); // pagefault
	//TEST_OUTPUT("check_lower_kernel", check_null(0x414000, PASS));
	//TEST_OUTPUT("check_upper_kernel", check_null(KERRNEL_UPPER, FAIL)); // pagefault

	/* ########## TEST CHECKPOINT 2 ##########*/
	
	/* TEST FILESYSTEM */
	//TEST_OUTPUT("Reading shell", test_file_read("shell"));
	// TEST_OUTPUT("Reading fish", test_file_read("fish"));
	// TEST_OUTPUT("Read long file", test_file_read("verylargetextwithverylongname.txt"));
	// TEST_OUTPUT("Read invalid file", test_file_read("yourmom.txt")); // should fail

	// test_dir_read(20); // change number of dirs 

	// /* TEST KEYBOARD */
	// test_keyboard();

	// /* TEST RTC DRIVER */
	// TEST_OUTPUT("test_rtc at freq 2", test_rtc(2)); // expected pass
	// TEST_OUTPUT("test_rtc at freq 3", test_rtc(3)); // expected fail

	/* ########## TEST CHECKPOINT 2 ##########*/
	

	/* ########## TEST CHECKPOINT 3 ##########*/
	// test_syscall_handler();
	// static uint8_t buf_r[40456];
	// static uint8_t buf_w[175];
	// test_sysfileops("frame1.txt", buf_r , buf_w , 174) ;
	/* ########## TEST CHECKPOINT 3 ##########*/


}
