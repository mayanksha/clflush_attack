#include "../lib/util.h"
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <signal.h>

#define SIZE 10000
#define NUM_ACCESSES 10000
#define BYTES_SENT 5000
#define NUM_CHARS 60

volatile sig_atomic_t stop = 0;
static void int_handler (int signum) {
    stop = 1;
}

static inline void do_something(int loop_param) {
    volatile long long k = 0;
    for(volatile int i=0; i<loop_param ;i++) {
        volatile int j = i*2;
        j++;
        k += j;
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
    unsigned int msg_len = 0;
    clock_t t_start = 0, t_end;
    double recv_time, recv_rate;
    char msg[BYTES_SENT];
    void *map;
    int loop_param = 0;
    map_handle_t *handle;     // declaring a handle for file mapping

    signal(SIGINT, int_handler);
    if (argc == 2)
        loop_param = atoi(argv[1]);
    else if (argc > 2) {
        printf ("1 optional argument specifying the iterations in do_something() is needed. Exiting!\n");
        return -1;
    }

    printf ("loop param %d\n", loop_param);
    memset (msg, '\0', BYTES_SENT);

    map = map_file("../share_mem.txt", &handle);
    if(map==NULL){
        perror("File not found");
        return -1;
    }

    THRESHOLD = get_clflush_threshold ((void *)map);
    printf ("[Receiver] Threshold = %d\n", THRESHOLD);
    printf ("[Receiver] To stop the receiver gracefully, please use Ctrl + C or signal with SIGINT\n");
    printf ("[Receiver] Looping now...in hope for some data\n\n");

    while (!stop) {
        int atime;

        for (int i = 0; i < NUM_CHARS; i++) {
            void *addr_to_check = (map + i*4096);
            clflush(addr_to_check);
        }
        do_something (loop_param);
        for (int i = 0; i < NUM_CHARS; i++) {
            void *addr_to_check = (map + i*4096);
            atime = measure_one_block_access_time ((void *) addr_to_check);
            if (atime < THRESHOLD) {
                if (!t_start)
                    t_start = clock();
                msg[msg_len++] = 'A' + i;
                t_end = clock();
                /* printf("%c\n", msg[i]); */
            }
        }

    }

    printf ("[Receiver] Data Received:\n");
    for (int i = 0; i < BYTES_SENT; i++) {
        if (msg[i] == '\0')
            break;
        printf("%c", msg[i]);
    }
    printf("\n");
    msg_len = strlen(msg);
    recv_time = ((double) (t_end - t_start)) / CLOCKS_PER_SEC;
    recv_rate = (t_start) ? (double) (msg_len * 8) / recv_time: 0;

    /* printf("%d\n", msg_len); */
    printf("[Receiver] Total data received : %u bytes\n", msg_len);
    printf("[Receiver] Time taken to receive data : %lf second\n", recv_time);
    printf("[Receiver] Data receiving rate : %lu bps\n", (unsigned long) recv_rate);
    printf("[Receiver] Accuracy = %f\n", (msg_len / (1.0 * BYTES_SENT)) * 100); 

    return 0;
}
