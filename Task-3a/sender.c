// #include <time.h>
#include "../lib/util.h"

void do_something(int param){
	for(int i=0;i<param;i++){
            //char j = *(map);
            int j = 2*i;
            j++;
        }
    return;
}


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
    char* filedata;

	printf("Image File to be send: ");
	//fgets (filename, 50, stdin);

	/* TODO:
	 * Send filename to the receiver
	 * Open image file (binary file) in read mode.
	 * Store the size of file in file_size variable.
	*/
	int fd, bytes_read;

	if ((fd = open ("red_heart.jpg", O_RDONLY)) < 0) {
        perror ("Error opening file: ");
        if (handle != NULL)
            free (handle);
        return -1;
    }
    struct stat st_file;
    if (fstat (fd, &st_file) < 0) {
        perror ("Error getting file metadata (using fstat): ");
        if (handle != NULL)
            free (handle);
        close (fd);
        return -1;
    }


    /* Allocate the memory for file contents */
    file_size = st_file.st_size;
	//unsigned int file_size = 0;
    if ((filedata = malloc (sizeof(char) * file_size + 1)) == NULL) {
        perror ("Error while malloc: ");
        if (handle != NULL)
            free (handle);
        close (fd);
        return -1;
    }

    if ((bytes_read = read (fd, filedata, file_size)) < 0) {
        perror ("Error while reading: ");
        free (filedata);
        if (handle != NULL)
            free (handle);
        close (fd);
        return -1;
    }
	

	for(int i=258;i < file_size;i++){
		unsigned char y = filedata[i];
		//printf("%d ",y);
        volatile char x = *(map+4096*(y%16));
        x++;
        do_something(30000);
        volatile char z = *(map+4096*(y/16));
        z++;
        do_something(30000);
    }
    //printf("%ld\n",sz );

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