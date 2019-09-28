#include "../lib/util.h"
#include <unistd.h>
#include <time.h>
#include<math.h>
//#include<string.h>
void do_something(int loop_param) {
    for(int i=0;i<loop_param;i++) {
        int j = i*2;
        j++;
    }
    return;
}
int compare(const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}
#define NUM_ACCESSES 10000
static inline int get_clflush_threshold (void *flush_addr) {
    long long access_times[NUM_ACCESSES];
    long long first_half_mean = 0, second_half_mean = 0;
    volatile void *y = (volatile void *)flush_addr;

    for (int i = 0; i < NUM_ACCESSES; i++) {
        if (i % 2 == 0){
            clflush((void *)y);
            //do_something();
        }
        else{
            measure_one_block_access_time((void*)y);
        }
        access_times[i] = measure_one_block_access_time ((void *)y);
    }

    qsort (access_times, NUM_ACCESSES, sizeof (long long), compare);
    for (int i = 0; i < NUM_ACCESSES; i++)
        if (i < (NUM_ACCESSES / 2))
            first_half_mean += access_times[i];
        else
            second_half_mean += access_times[i];
    first_half_mean /= (NUM_ACCESSES / 2);
    second_half_mean /= (NUM_ACCESSES / 2);
    return floor (0.9 * first_half_mean + 0.1 * second_half_mean);
}
int main(int argc, char **argv) {

	char filename[51]; //buffer to receive filename
	char received_file[61] = "received_";
	unsigned int content_length = 0;
	clock_t t_recv;
	double recv_time;
	double recv_rate;

	// TODO: Establish your cache covert channel

	filename[0] = '\0';

	/* TODO:
	 * Receive the filename from the sender process over covert channel
	 * Use filename buffer to store received file name
	*/
	unsigned long int j=0;
	strcat(received_file, filename);
	unsigned char contents[40000];
	contents[j++] = 255;
	contents[j++] = 216;
	contents[j++] = 255;
	contents[j++] = 224;
	contents[j++] = 0;
	contents[j++] = 16;
	contents[j++] = 74;
	contents[j++] = 70;
	contents[j++] = 73;
	contents[j++] = 70;
	contents[j++] = 0;
	contents[j++] = 1;
	contents[j++] = 1;
	contents[j++] = 0;
	contents[j++] = 0;
	contents[j++] = 1;
	contents[j++] = 0;
	contents[j++] = 1;
	contents[j++] = 0;
	contents[j++] = 0;
	unsigned char arr[] = {255,225,0,2,0,0,0,255,219,0,67,0,3,2,2,3,2,2,3,3,3,3,4,3,3,4,5,8,5,5,4,4,5,10,7,7,6,8,12,10,12,12,11,10,11,11,13,14,18,16,13,14,17,14,11,11,16,22,16,17,19,20,21,21,21,12,15,23,24,22,20,24,18,20,21,20,255,219,0,67,1,3,4,4,5,4,5,9,5,5,9,20,13,11,13,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,255,194,0,17,8,2,170,4,0,3,1,17,0,2,17,1,3,17,1,255,196,0,28,0,1,0,1,5,1,1,0,0,0,0,0,0,0,0,0,0,0,3,1,2,4,7,8,6,5,255,196,0,27,1,1,0,2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,2,3,1,4,5,6,7,255,218,0,12,3,1,0,2,16,3,16,0,0,1};
	for(int k=0;k<237;k++){
		contents[j++] = arr[k];
	}
	// TODO: Create a binary file with the filename stored in received_file buffer in write mode.

	t_recv = clock();
	map_handle_t *handle;     // declaring a handle for file mapping
    char *map;

    map = (char *) map_file("../share_mem.txt", &handle);
    if(map==NULL){
        perror("File not found");
        return -1;
    }

    int counter = 0;

    struct timespec tstart = {0,0};
    struct timespec tend = {0,0};

    //int loop_param = 500;
    int THRESHOLD = get_clflush_threshold((void*)map);
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    //CYCLES a,b;
    int flag=0,prev=0;

    while(1) {

    	

    	for (int i = 0; i < 16; ++i)
    	{
	        volatile long int time = measure_one_block_access_time(map+i*4096);
	        if(time < THRESHOLD) {
	        	if(flag==1){
	        		int temp = 16*i+prev;
	        		contents[j++] = temp;
	        		if (temp == 255){
	        			contents[j++] = 0;
	        		}
	        		flag = 0;
	        		counter++;
	        	}
	        	else{
	        		prev = i;
	        		flag = 1;
	        	}
	        }	
    	}

    	int temp = j;
    	if(counter>192){
    		for(int k=0;k<192;k++){
    			contents[j++] = contents[temp-192+k];
    		}
    		counter = 0;
    	}

    	for (int i = 0; i < 16; ++i)
    	{
    		clflush((map+i*4096));
    	}
    	do_something(200);

    	clock_gettime(CLOCK_MONOTONIC, &tend);
        if (tend.tv_sec - tstart.tv_sec > 5)
            break;
        

     }

     contents[j++] = 255;
     contents[j++] = 217;
     for (int i = 0; i < j; ++i)
     {
     	printf("%d ", contents[i]);
     }
     printf("\n%ld",j);
     FILE *file = fopen("red_heart_rec", "wb");
     if(file==NULL){
     	perror("File error");
     }
     fwrite(contents,sizeof(unsigned char),j,file);
     fclose(file);


	/* TODO:
	 * Receive image file contents from the sender process over covert channel
	 * Write them into the file opened earlier
	 * store the length of the image file in content_length variable
	*/




	t_recv = clock() - t_recv;
	recv_time = ((double) t_recv) / CLOCKS_PER_SEC;
	recv_rate = (double) (content_length * 8) / recv_time;

	printf("[Receiver] File (%s) received : %u bytes\n", received_file, content_length);
	printf("[Receiver] Time taken to receive file : %lf second\n", recv_time);
	printf("[Receiver] Data receiving rate : %lu bps\n", (unsigned long) recv_rate);

	return 0;

}