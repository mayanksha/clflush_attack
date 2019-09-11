#include "../lib/util.h"
#include <unistd.h>
#include <time.h>
void do_something(int loop_param) {
    for(int i=0;i<loop_param;i++) {
        int j = i*2;
        j++;
    }
    return;
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
	unsigned char contents[32300];
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
	contents[j++] = 95;
	contents[j++] = 0;
	contents[j++] = 0;
	contents[j++] = 1;
	contents[j++] = 0;
	contents[j++] = 1;
	contents[j++] = 0;
	contents[j++] = 0;
	// TODO: Create a binary file with the filename stored in received_file buffer in write mode.

	t_recv = clock();
	map_handle_t *handle;     // declaring a handle for file mapping
    char *map;

    map = (char *) map_file("/tmp/test.txt", &handle);
    if(map==NULL){
        perror("File not found");
        return -1;
    }

    struct timespec tstart = {0,0};
    struct timespec tend = {0,0};

    //int loop_param = 500;
    int THRESHOLD = 100;
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    //CYCLES a,b;
    while(1) {

    	for (int i = 0; i < 256; ++i)
    	{
    		clflush((map+i*4096));
    	}
    	do_something(500);

    	for (int i = 0; i < 256; ++i)
    	{
	        volatile long int time = measure_one_block_access_time(map+i*4096);
	        if(time < THRESHOLD) {
	        	printf("%d ",i );
	            contents[j++] = i;
	        }	
    	}

    	clock_gettime(CLOCK_MONOTONIC, &tend);
        if (tend.tv_sec - tstart.tv_sec > 5)
            break;
        

     }

     contents[j++] = 255;
     contents[j++] = 217;
     printf("\n%ld",j);
     FILE *file = fopen("red_heart_rec", "wb");
     if(file==NULL){
     	perror("File error");
     }
     fwrite(contents,j*sizeof(unsigned char),j,file);
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