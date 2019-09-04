#include "../lib/util.h"
#include <unistd.h>
#include <time.h>
#define SIZE 10000
void do_something(int loop_param) {
    for(int i=0;i<loop_param;i++) {
        int j = i*2;
        j++;
    }
    return;
}

int main(int argc, char **argv) {

    char msg[BYTES_SENT]; //buffer to receive data
    unsigned int msg_len = 0;
    clock_t t_recv;
    double recv_time;
    double recv_rate;

    memset (msg, '\0', BYTES_SENT);

    // TODO: Establish your cache covert channel

    t_recv = clock();

    // TODO: synchronize with sender and receive data in msg buffer.

    map_handle_t *handle;     // declaring a handle for file mapping
    char *map;

    map = (char *) map_file("/tmp/test.txt", &handle);
    if(map==NULL){
        perror("File not found");
        return -1;
    }

    int loop_param = 500;
    loop_param = atoi(argv[1]);
    char arr[SIZE];
    CYCLES a,b;

    struct timespec tstart = {0,0};
    struct timespec tend = {0,0};

    clock_gettime(CLOCK_MONOTONIC, &tstart);
    while(1) {
        /* printf("Foo\n"); */
        clflush((map));
        do_something(loop_param);
        a = rdtscp();
        /* usleep (2); */
        volatile char y = *(map);
        b = rdtscp();
        /* printf("%d\n",b-a); */
        if((b-a) < 60) {
            //break;
            msg[msg_len++] = 'A';
            /* printf ("%c", msg[msg_len-1]); */
        }


        clflush(map+64);
        do_something(loop_param);
        a = rdtscp();
        volatile char x = *(map+64);
        b = rdtscp();
        //printf("%d \n",b-a);
        if((b-a) < 60 ){
            msg[msg_len++] = 'B';
            /* printf ("%c", msg[msg_len-1]); */
        }

        clflush(map+128);
        do_something(loop_param);
        a = rdtscp();
        volatile char z = *(map+128);
        b = rdtscp();
        //printf("%d \n",b-a);
        if((b-a) < 60 ){
            msg[msg_len++] = 'C';
            /* printf ("%c", msg[msg_len-1]); */
        }

        clock_gettime(CLOCK_MONOTONIC, &tend);
        if (tend.tv_sec - tstart.tv_sec > 5)
            break;
    }

    /* while(1){
     *     clflush(map);
     *     do_something (loop_param);
     *     volatile char x = *(map);
     *     a = rdtscp();
     *     x = *(map);
     *     b = rdtscp();
     *     printf("time is %ld\n",b-a);
     * } */


    //printf("2nd %x\n",measure_one_block_access_time(map));

    //printf ("%s",msg);
    t_recv = clock() - t_recv;
    msg_len = strlen(msg);
    recv_time = ((double) t_recv) / CLOCKS_PER_SEC;
    recv_rate = (double) (msg_len * 8) / recv_time;

    /* printf("[Receiver] Received data : %s\n", msg); */
    printf("[Receiver] Total data received : %u bytes\n", msg_len);
    printf("[Receiver] Time taken to receive data : %lf second\n", recv_time);
    printf("[Receiver] Data receiving rate : %lu bps\n", (unsigned long) recv_rate);
    printf("[Receiver] Accuracy = %f%\n", (msg_len / (1.0 * BYTES_SENT)) * 100);

    return 0;

}

//taskset 0x4 PID
//taskset -cp PID
