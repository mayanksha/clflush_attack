// #include <time.h>
#include "../lib/util.h"

#define BYTES_SENT 50
#define NUM_CHARS 60

static inline void do_something(int loop_param) {
    for(int i=0;i<loop_param;i++) {
        volatile int j = i*2;
        j++;
    }
    return;
}

int main(int argc, char **argv) {

    char msg[BYTES_SENT];
    unsigned int msg_len = 0;
    clock_t t_send;
    double trans_time;
    double trans_rate;

    map_handle_t *handle;     // declaring a handle for file mapping
    char *map;
    map = (char *) map_file("../share_mem.txt", &handle);
    if(map == NULL){
        perror ("Error while trying to use the sharedfile ../share_mem.txt\n");
        return -1;
    }

    printf("Enter message to be send: \n");
    memset (msg, '\0', BYTES_SENT);
    fgets (msg, BYTES_SENT, stdin);  
    /* for (int i = 0; i < BYTES_SENT; i++) {
     *     msg[i] = 'A' + (i % NUM_CHARS);
     * } */
    msg[BYTES_SENT - 1] = '\0';

    t_send = clock();

    for(uint32_t i=0; i < strlen(msg) - 1; i++) {
        volatile char x = *(map + (msg[i] - 'A')*4096);
        do_something (10000);
        x += 2;
        msg_len++;
    }

    t_send = clock() - t_send;
    trans_time = ((double) t_send) / CLOCKS_PER_SEC;
    trans_rate = (double) (msg_len * 8) / trans_time;

    printf("[Sender] Total data Send : %u bytes\n", msg_len);
    printf("[Sender] Time to send data : %lf second\n", trans_time);
    printf("[Sender] Data transmission rate : %lu bps\n", (unsigned long) trans_rate);

    return 0;
}
