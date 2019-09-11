#include "../lib/util.h"
#include <unistd.h>
#include <time.h>
#include <math.h>

#define SIZE 10000
#define NUM_ACCESSES 10000

static inline void do_something(int loop_param) {
    for(int i=0;i<loop_param;i++) {
        volatile int j = i*2;
        j++;
    }
    return;
}

int compare(const void * a, const void * b) {
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
    unsigned int msg_len = 0;
    clock_t t_recv;
    double recv_time, recv_rate;
    char *map, msg[BYTES_SENT];
    int loop_param = 500;
    char arr[SIZE];
    struct timespec tstart = {0,0}, tend = {0,0};
    map_handle_t *handle;     // declaring a handle for file mapping

    /* loop_param = atoi(argv[1]); */

    memset (msg, '\0', BYTES_SENT);
    t_recv = clock();

    map = (char *) map_file("/tmp/test.txt", &handle);
    if(map==NULL){
        perror("File not found");
        return -1;
    }

    THRESHOLD = get_clflush_threshold ((void *)map);
    printf ("THRESHOLD = %d\n", THRESHOLD);

    clock_gettime(CLOCK_MONOTONIC, &tstart);
    while (1) {
        int atime;

        for (int i = 0; i < NUM_CHARS; i++) {
            void *addr_to_check = (map + i*4096);
            atime = measure_one_block_access_time ((void *) addr_to_check);
            if (atime < THRESHOLD) {
                msg[msg_len++] = 'A' + i;
                /* printf("%c\n", msg[i]); */
               /* printf("%c, %d\n", 'A' + i, atime); */
            }
        }

        for (int i = 0; i < NUM_CHARS; i++) {
            void *addr_to_check = (map + i*4096);
            clflush(addr_to_check);
        }
        do_something (400);

        clock_gettime(CLOCK_MONOTONIC, &tend);
        if (tend.tv_sec - tstart.tv_sec > 1.5)
            break;
    }

    for (int i = 0; i < BYTES_SENT; i++) {
        if (msg[i] == '\0')
            break;
        printf("%c", msg[i]);
    }
    printf("\n");
    t_recv = clock() - t_recv;
    msg_len = strlen(msg);
    recv_time = ((double) t_recv) / CLOCKS_PER_SEC;
    recv_rate = (double) (msg_len * 8) / recv_time;

    /* printf("%d\n", msg_len); */
    printf("[Receiver] Total data received : %u bytes\n", msg_len);
    printf("[Receiver] Time taken to receive data : %lf second\n", recv_time);
    printf("[Receiver] Data receiving rate : %lu bps\n", (unsigned long) recv_rate);
    printf("[Receiver] Accuracy = %f\n", (msg_len / (1.0 * BYTES_SENT)) * 100); 

    return 0;
}
