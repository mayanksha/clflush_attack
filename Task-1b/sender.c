// #include <time.h>
#include "../lib/util.h"
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>

#define FILENAME_MAX_LEN 51

static inline void do_something(int loop_param) {
    for(int i=0;i<loop_param;i++) {
        volatile int j = i*2;
        j++;
    }
    return;
}

int main(int argc, char **argv) {

	// TODO: The below should be of size 51 char array
	char *filepath = "../sample.txt";
	char *filename, *filedata, *map;
	unsigned int file_size = 0;
	clock_t t_send;
	double trans_time, trans_rate;
	int fd, bytes_read;
    struct stat st_file;
    map_handle_t *handle = NULL;     // declaring a handle for file mapping

	// TODO: Establish your cache covert channel

    /* TODO: Decide you wanna take an arg from stdin or cmdline */
	/* fgets (filename, 50, stdin); */

    if ((fd = open (filepath, O_RDONLY)) < 0) {
        perror ("Error opening file: ");
        if (handle != NULL)
            free (handle);
        return -1;
    }

    if (fstat (fd, &st_file) < 0) {
        perror ("Error getting file metadata (using fstat): ");
        if (handle != NULL)
            free (handle);
        close (fd);
        return -1;
    }

    filename = basename (filepath);
	printf("File to be sent: %s\n", filename);

    /* Allocate the memory for file contents */
    file_size = st_file.st_size;
	;
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

    map = map_file("../share_mem.txt", &handle);
    if(map == NULL){
        free (filedata);
        close (fd);
        return -1;
    }

    /* Now, transmit file contents to the Receiver over the cache covert channel */

    /* Assumption: The filename must have characters which are in smallcase */
    for (unsigned int i = 0; i < strlen (filename); i++) {
        volatile char x = *(map + (filename[i] - '\n')*4096);
        do_something (10000);
        x += 2;
    }

    /* We send the '/' character 10 times, so as to make the receiver aware
     * that filename reading has to finished now and that subsequent data
     * will be file content. Moreover, we map the '/' character to the last. */
    for (unsigned int i = 0; i < 30; i++) {
        volatile char x = *(map + ('/' - '\n')*4096);
        do_something (10000);
        x += 2;
    }

    do_something (100000);
	t_send = clock();
    printf ("[Sender] Bytes read = %d\n", bytes_read);
    for(int i = 0; i < bytes_read; i++) {
        if ((filedata[i] >= 'a' && filedata[i] <= 'z') ||
            (filedata[i] >= 'A' && filedata[i] <= 'Z') ||
             filedata[i] == '\n' ||
             filedata[i] == ' ' ||
             filedata[i] == '.'
           ) {
            volatile char x;
            if ((filedata[i] >= 'A' && filedata[i] <= 'Z')) {
                x = *(map + ((filedata[i] - 'A' + 'a') - '\n')*4096);
            } else
                x = *(map + (filedata[i] - '\n')*4096);
            do_something (10000);
            x += 2;
        }
    }

	t_send = clock() - t_send;
	trans_time = ((double) t_send) / CLOCKS_PER_SEC;
	trans_rate = (double) (file_size * 8) / trans_time;

	printf("[Sender] File (%s) data Send : %u bytes\n", filename, file_size);
	printf("[Sender] Time to send file data : %lf second\n", trans_time);
	printf("[Sender] Data transmission rate : %lu bps\n", (unsigned long) trans_rate);

    free (filedata);
    free (handle);
    close (fd);
	return 0;
}
