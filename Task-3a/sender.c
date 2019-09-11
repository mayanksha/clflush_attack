// #include <time.h>
#include "../lib/util.h"


int main(int argc, char **argv) {

	char filename[51];
	unsigned int file_size = 0;
	clock_t t_send;
	double trans_time;
	double trans_rate;

	// TODO: Establish your cache covert channel
	map_handle_t *handle;     // declaring a handle for file mapping
    char *map;

    map = (char *) map_file("/tmp/test.txt", &handle);
    if(map == NULL){
        return -1;
    }

	printf("Image File to be send: ");
	//fgets (filename, 50, stdin);

	/* TODO:
	 * Send filename to the receiver
	 * Open image file (binary file) in read mode.
	 * Store the size of file in file_size variable.
	*/

	
	FILE *file = fopen("red_heart.jpg", "rb");
     if(file==NULL){
     	perror("File error");
     	return -1;
     }
     fseek(file, 0L, SEEK_END);
	 long int sz = ftell(file);
	 unsigned char contents[sz+1];
     fread(contents,sz*sizeof(unsigned char),sz,file);
     fclose(file);


	for(int i=22;i < sz;i++){
        volatile char x = *(map+4096*contents[i]);
        x++;

        for(int i=0;i<50000;i++){
            //char j = *(map);
            int j = 2*i;
            j++;
        }
    }
    printf("%ld\n",sz );

	t_send = clock();

	// TODO: Transmit file contents to the Receiver over the cache covert channel




	t_send = clock() - t_send;
	trans_time = ((double) t_send) / CLOCKS_PER_SEC;
	trans_rate = (double) (file_size * 8) / trans_time;

	printf("[Sender] File (%s) data Send : %u bytes\n", filename, file_size);
	printf("[Sender] Time to send file data : %lf second\n", trans_time);
	printf("[Sender] Data transmission rate : %lu bps\n", (unsigned long) trans_rate);

	return 0;
}