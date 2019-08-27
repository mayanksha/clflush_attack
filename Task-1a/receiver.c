#include "../lib/util.h"
#include <unistd.h>
#include <time.h>
#define SIZE 10000
void do_something() {
    for(int i=0;i<1000;i++) {
        int j = i*2;
        j++;
    }
    return;
}

int main(int argc, char **argv) {

    char msg[51]; //buffer to receive data
    unsigned int msg_len = 0;
    clock_t t_recv;
    double recv_time;
    double recv_rate;
    msg[0] = '\0';

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

    char arr[SIZE];
    CYCLES a,b;
    int characters = 0;
    struct timespec tstart = {0,0};
    struct timespec tend = {0,0};

    clock_gettime(CLOCK_MONOTONIC, &tstart);
    while(1) {
        /* printf("Foo\n"); */
        clflush((map));
        do_something();
        a = rdtscp();
        /* usleep (2); */
        volatile char y = *(map);
        b = rdtscp();
        /* printf("%d\n",b-a); */
        if((b-a) < 50) {
            //arr[characters] = 'A';
            //break;
            printf("A");
            characters++;
        }


        clflush(map+512);
        do_something();
        a = rdtscp();
        volatile char x = *(map+512);
        b = rdtscp();
        //printf("%d \n",b-a);
        if((b-a) < 50 ){
            //arr[characters] = 'B';
            printf("B");
            characters++;
        }

        clock_gettime(CLOCK_MONOTONIC, &tend);
        if (tend.tv_sec - tstart.tv_sec > 2)
            break;
    }
    //printf ("%s\n",arr);
    printf("\n");
    // while(1){
    //   char x = *(map);
    // clflush(map);
    // a = rdtscp();
    // x = *(map);
    // b = rdtscp();
    // printf("time is %d\n",b-a);  
    // }


    //printf("2nd %x\n",measure_one_block_access_time(map));

    t_recv = clock() - t_recv;
    msg_len = strlen(msg);
    recv_time = ((double) t_recv) / CLOCKS_PER_SEC;
    recv_rate = (double) (msg_len * 8) / recv_time;

    dprintf(STDERR_FILENO,"[Receiver] Received data : %s\n", msg);
    dprintf(STDERR_FILENO,"[Receiver] Total data received : %u bytes\n", msg_len);
    dprintf(STDERR_FILENO,"[Receiver] Time taken to receive data : %lf second\n", recv_time);
    dprintf(STDERR_FILENO,"[Receiver] Data receiving rate : %lu bps\n", (unsigned long) recv_rate);

    return 0;

}

//taskset 0x4 PID
//taskset -cp PID
