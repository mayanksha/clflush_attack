#include "../lib/util.h"
#include <math.h>
#include <linux/limits.h>
#include <signal.h>

#undef NUM_CHARS
#define NUM_CHARS 74

#define NUM_ACCESSES 10000
#define FILENAME_MAX_LEN 51
#define MAX_FILE_SIZE 100000

/* 26 + 1 because the last character, we're assuming that it's our de-limiter for file and file's data */
#define FILE_NAME_CHARS 26

volatile sig_atomic_t stop = 0;
static void hup_handler (int signum) {
    stop = 1;
}

static inline void do_something(int loop_param) {
    for(int i=0;i<loop_param;i++) {
        volatile int j = i*2;
        j++;
    }
    return;
}

static inline int compare(const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

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
    int THRESHOLD;
	char filename[FILENAME_MAX_LEN];                      // buffer to receive filename
	char *filepath, *filedata;
	char received_filename[FILENAME_MAX_LEN + 10] = "received_";
	unsigned int content_length = 0, filename_len = 0;
	clock_t t_recv;
    int fd;
	double recv_time, recv_rate;
    map_handle_t *handle;                   // declaring a handle for file mapping

    memset (filename, '\0', FILENAME_MAX_LEN);
	// Establish your cache covert channel
    void *map = map_file("../share_mem.txt", &handle);
    if (map == NULL){
        perror("File not found");
        return -1;
    }

    signal(SIGHUP, hup_handler);
    THRESHOLD = get_clflush_threshold ((void *)map);

    int break_now = 0;
    char special_chars[8] = {'/', ' ', '.', '_', '\n', '#', '$', '-'};
    /* void *slash_addr = (void *) (map + ('/' - '\n')*4096);
     * void *space_addr = (void *) (map + (' ' - '\n')*4096);
     * void *dot_addr = (void *) (map + ('.' - '\n')*4096);
     * void *underscore_addr = (void *) (map + ('_' - '\n')*4096); */

    while (1) {
        int atime;

        if (filename_len >= FILENAME_MAX_LEN || break_now)
            break;

        for (int k = ((int)('a' - '\n')); k < (int)('z' - '\n'); k++) {
            void *addr_to_check = (map + k*4096);
            atime = measure_one_block_access_time ((void *) addr_to_check);
            if (atime < THRESHOLD) {
                filename[filename_len++] = '\n' + k;
                printf ("%c\n", filename[filename_len-1]);
            }
        }

        // We omit the checking for newline character in case of filename
        for (size_t i = 0; i < sizeof (special_chars)/ sizeof (char) - 1; i++) {
            atime = measure_one_block_access_time (map + (special_chars[i] - '\n') * 4096);
            if (atime < THRESHOLD) {
                if (special_chars[i] == '/') {
                    break_now = 1;
                    break;
                } else {
                    filename[filename_len++] = '.';
                }
            }
        }

        for (int k = ((int)('a' - '\n')); k < (int)('z' - '\n'); k++) {
            void *addr_to_check = (map + k*4096);
            clflush (addr_to_check);
        }

        for (size_t i = 0; i < sizeof (special_chars)/ sizeof (char) - 1; i++) {
            clflush (map + (special_chars[i] - '\n') * 4096);
        }

        do_something (350);
    }

	strcat(received_filename, filename);
    printf ("[Receiver] received_filename: %s\n", received_filename);

    if ((filepath = malloc (sizeof(char) * PATH_MAX + 1)) == NULL) {
        perror ("Error while malloc: ");
        if (handle != NULL)
            free (handle);
        close (fd);
        return -1;
    }

    filepath = strncpy (filepath, "./", 2);
    strncat(filepath, received_filename, PATH_MAX);
    if ((fd = open (filepath, O_RDWR | O_CREAT, 0755)) < 0) {
        perror ("Error opening file: ");
        if (handle != NULL)
            free (handle);
        return -1;
    }

    if ((filedata = malloc (sizeof(char) * MAX_FILE_SIZE + 1)) == NULL) {
        perror ("Error while malloc: ");
        if (handle != NULL)
            free (handle);
        close (fd);
        return -1;
    }

	t_recv = clock();
    while (!stop) {
        int atime;
        if (content_length >= MAX_FILE_SIZE) {
            printf ("Max content length exceeded.\n");
            break;
        }

        for (int i = ('a' - '\n'); i < ('z' - '\n'); i++) {
            void *addr_to_check = (map + i*4096);
            atime = measure_one_block_access_time ((void *) addr_to_check);
            if (atime < THRESHOLD) {
                filedata[content_length++] = '\n' + i;
            }
        }

        for (size_t i = 0; i < sizeof (special_chars)/ sizeof (char) - 1; i++) {
            atime = measure_one_block_access_time (map + (special_chars[i] - '\n') * 4096);
            if (atime < THRESHOLD) {
                if (!(content_length == 0 && special_chars[i] == '/')) {
                    filedata[content_length++] = special_chars[i];
                }
            }
        }

        for (int i = ('a' - '\n'); i < ('z' - '\n'); i++) {
            void *addr_to_check = (map + i*4096);
            clflush(addr_to_check);
        }

        for (size_t i = 0; i < sizeof (special_chars) / sizeof (char) - 1; i++) {
            clflush (map + (special_chars[i] - '\n') * 4096);
        }

        do_something (350);
    }

    if (write (fd, filedata, content_length) < 0) {
        perror ("Error while writing to file!");
        if (handle != NULL)
            free (handle);
        close (fd);
        return -1;
    }

    /* TODO:
     * Receive file contents from the sender process over covert channel
     * Wrtie them into the file opened earlier
     * store the length of the file content in content_length variable
     */
	t_recv = clock() - t_recv;
	recv_time = ((double) t_recv) / CLOCKS_PER_SEC;
	recv_rate = (double) (content_length * 8) / recv_time;

	printf("[Receiver] File (%s) received : %u bytes\n", received_filename, content_length);
	printf("[Receiver] Time taken to receive file : %lf second\n", recv_time);
	printf("[Receiver] Data receiving rate : %lu bps\n", (unsigned long) recv_rate);

    /* close (fd);
     * free (filedata);
     * free (handle); */
	return 0;
}
